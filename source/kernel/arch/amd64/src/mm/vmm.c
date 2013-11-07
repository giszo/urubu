/* amd64 virtual memory manager.
 *
 * Copyright (c) 2013 Zoltan Kovacs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <kernel/types.h>
#include <kernel/proc/process.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/lib/string.h>

#include <arch/cpu/cpu.h>
#include <arch/proc/process.h>
#include <arch/mm/vmm.h>
#include <arch/mm/paging.h>
#include <arch/mm/config.h>

#include <kernel/console.h>

extern uint64_t _vmm_kpml4[512];
extern uint64_t _vmm_kpdp[512];

extern uint64_t _vmm_phys_pdp[512];
extern uint64_t _vmm_phys_pd[512];

// =====================================================================================================================
static inline unsigned get_paging_flags(unsigned vmm_flags)
{
    unsigned ret = PAGE_PRESENT;

    if (vmm_flags & VMM_WRITE)
	ret |= PAGE_WRITE;
    if (vmm_flags & VMM_USER)
	ret |= PAGE_USER;

    return ret;
}

// =====================================================================================================================
static inline void set_pml4_entry(uint64_t* pml4, uint64_t addr, uint64_t p, unsigned flags)
{
    pml4[PML4_INDEX(addr)] = p | flags;
}

// =====================================================================================================================
static inline uint64_t* get_pml4_entry(uint64_t* pml4, uint64_t addr, int alloc, unsigned flags)
{
    unsigned idx = PML4_INDEX(addr);

    // check whether the pml4 entry is already allocated
    if (pml4[idx] != 0)
	return (uint64_t*)(pml4[idx] & PAGE_MASK);

    // return NULL if allocation is not allowed
    if (!alloc)
	return NULL;

    // allocate a new entry and set it
    ptr_t p = pmm_alloc();

    if (!p)
	return NULL;

    memsetl((void*)vmm_provide_phys(p), 0, PAGE_SIZE / 4);
    pml4[idx] = p | flags | PAGE_WRITE;

    return (uint64_t*)p;
}

// =====================================================================================================================
static inline uint64_t* get_pdp_entry(uint64_t* pdp, uint64_t addr, int alloc, unsigned flags)
{
    unsigned idx = PDP_INDEX(addr);

    // check whether the pdp entry is already allocated
    if (pdp[idx] != 0)
	return (uint64_t*)(pdp[idx] & PAGE_MASK);

    // return NULL if allocation is disabled
    if (!alloc)
	return NULL;

    // allocate a new entry and set it
    ptr_t p = pmm_alloc();

    if (!p)
	return NULL;

    memsetl((void*)vmm_provide_phys(p), 0, PAGE_SIZE / 4);
    pdp[idx] = p | flags | PAGE_WRITE;

    return (uint64_t*)p;
}

// =====================================================================================================================
static inline uint64_t* get_pd_entry(uint64_t* pd, uint64_t addr, int alloc, unsigned flags)
{
    unsigned idx = PD_INDEX(addr);

    // check whether the pdp entry is already allocated
    if (pd[idx] != 0)
	return (uint64_t*)(pd[idx] & PAGE_MASK);

    // return NULL if allocation is disabled
    if (!alloc)
	return NULL;

    // allocate a new entry and set it
    ptr_t p = pmm_alloc();

    if (!p)
	return NULL;

    memsetl((void*)vmm_provide_phys(p), 0, PAGE_SIZE / 4);
    pd[idx] = p | flags | PAGE_WRITE;

    return (uint64_t*)p;
}

// =====================================================================================================================
static inline void set_pdp_entry(uint64_t* pdp, uint64_t addr, uint64_t p, unsigned flags)
{
    pdp[PDP_INDEX(addr)] = p | flags;
}

// =====================================================================================================================
void vmm_arch_init_process(struct amd64_process* p)
{
    uint64_t* pml4 = (uint64_t*)vmm_provide_phys(p->cr3);

    // set the higher half of the process' address space to the same as in the kernel's
    for (unsigned i = 256; i < 512; ++i)
	pml4[i] = _vmm_kpml4[i];
}

// =====================================================================================================================
static uint64_t* vmm_arch_get_pt(uint64_t* pml4, ptr_t virt, unsigned alloc, unsigned flags)
{
    uint64_t* pdp;
    uint64_t* pd;

    pdp = get_pml4_entry(pml4, virt, alloc, flags);

    if (!pdp)
	return NULL;

    pdp = (uint64_t*)vmm_provide_phys((ptr_t)pdp);
    pd = get_pdp_entry(pdp, virt, alloc, flags);

    if (!pd)
	return NULL;

    pd = (uint64_t*)vmm_provide_phys((ptr_t)pd);
    return get_pd_entry(pd, virt, alloc, flags);
}

// =====================================================================================================================
int vmm_arch_map(ptr_t virt, ptr_t phys, unsigned flags)
{
    uint64_t* pt = vmm_arch_get_pt(_vmm_kpml4, virt, 1, get_paging_flags(flags));

    if (!pt)
	return -1;

    pt = (uint64_t*)vmm_provide_phys((ptr_t)pt);
    pt[PT_INDEX(virt)] = phys | get_paging_flags(flags);

    // TODO: this is far from being optimal ...
    cpu_flush_cr3();

    return 0;
}

// =====================================================================================================================
int vmm_arch_unmap(ptr_t virt)
{
    uint64_t* pt = vmm_arch_get_pt(_vmm_kpml4, virt,
	0 /* do not alloc */, 0 /* no paging flags as we do not allow allocation */);

    if (!pt)
	return -1;

    pt = (uint64_t*)vmm_provide_phys((ptr_t)pt);
    pt[PT_INDEX(virt)] = 0;

    // TODO: this is far from being optimal ...
    cpu_flush_cr3();

    return 0;
}

// =====================================================================================================================
int vmm_arch_proc_map(struct process* proc, ptr_t virt, ptr_t phys, unsigned flags)
{
    struct amd64_process* arch_proc = (struct amd64_process*)proc->arch_data;

    uint64_t* pt = vmm_arch_get_pt((uint64_t*)vmm_provide_phys(arch_proc->cr3), virt, 1, get_paging_flags(flags));

    if (!pt)
	return -1;

    pt = (uint64_t*)vmm_provide_phys((ptr_t)pt);

    unsigned idx = PT_INDEX(virt);

    // return the entry of the page table if it is not empty
    if (pt[idx] != 0)
	return -1;

    // map the allocated page
    pt[idx] = phys | get_paging_flags(flags);

    return 0;
}

// =====================================================================================================================
int vmm_arch_proc_translate(struct process* proc, ptr_t virt, ptr_t* phys)
{
    struct amd64_process* arch_proc = (struct amd64_process*)proc->arch_data;

    uint64_t* pt = vmm_arch_get_pt((uint64_t*)vmm_provide_phys(arch_proc->cr3), virt, 0 /* do not alloc */, 0);

    if (!pt)
	return -1;

    pt = (uint64_t*)vmm_provide_phys((ptr_t)pt);

    unsigned idx = PT_INDEX(virt);

    if (pt[idx] == 0)
	return -1;

    *phys = pt[idx] & PAGE_MASK;

    return 0;
}

// =====================================================================================================================
void vmm_arch_pre_init()
{
    // turn off the identity mapping we did before at start.S
    _vmm_kpml4[0] = 0;
    _vmm_kpdp[0] = 0;
    cpu_flush_cr3();

    // initialize the first 1Gb of physical page mapping that does not require any allocation
    for (unsigned i = 0; i < 512; ++i)
	_vmm_phys_pd[i] = (i * 2 * 1024 * 1024) /* 2Mb pages */ | PAGE_PRESENT | PAGE_WRITE | PAGE_2MB;
    set_pdp_entry(_vmm_phys_pdp, PHYS_MAP_BASE, MM_V2P((uint64_t)_vmm_phys_pd), PAGE_PRESENT | PAGE_WRITE);
    set_pml4_entry(_vmm_kpml4, PHYS_MAP_BASE, MM_V2P((uint64_t)_vmm_phys_pdp), PAGE_PRESENT | PAGE_WRITE);
}

// =====================================================================================================================
void vmm_arch_init(size_t mem_size)
{
    // build the full physical map region because the page allocator is now working
    for (ptr_t p = 1 * 1024 * 1024 * 1024 /* 1Gb - it was already mapped before in vmm_arch_pre_init() */;
	 p < mem_size;
	 p += 2 * 1024 * 1024 /* 2Mb pages are used for mapping */)
    {
	// TODO ...
    }
}
