/* Virtual memory manager.
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

#include <kernel/kernel.h>
#include <kernel/mm/vmm.h>
#include <kernel/mm/pmm.h>
#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>

#include <arch/mm/config.h>

static struct slab_cache s_entry_cache;

// =====================================================================================================================
static struct memory_map_entry* entry_alloc()
{
    return (struct memory_map_entry*)slab_cache_alloc(&s_entry_cache);
}

// =====================================================================================================================
static void entry_free(struct memory_map_entry* entry)
{
    slab_cache_free(&s_entry_cache, (void*)entry);
}

// =====================================================================================================================
static struct memory_map_allocator s_map_allocator =
{
    .alloc = entry_alloc,
    .free = entry_free
};

// =====================================================================================================================
ptr_t vmm_provide_phys(ptr_t phys)
{
    if (phys >= PHYS_MAP_SIZE)
	panic("vmm: unable to provide physical memory at %p\n", phys);

    if (phys == 0)
	panic("vmm: tried to provide NULL address\n");

    return PHYS_MAP_BASE + phys;
}

// =====================================================================================================================
ptr_t vmm_revert_phys(ptr_t virt)
{
    if (virt < PHYS_MAP_BASE || virt > (PHYS_MAP_BASE + PHYS_MAP_SIZE - 1))
	panic("vmm: unable to revert physical memory at %p\n", virt);

    return virt - PHYS_MAP_BASE;
}

// =====================================================================================================================
int vmm_map(struct process* proc, ptr_t* base, ptr_t phys, size_t size, unsigned flags)
{
    if ((phys & ~PAGE_MASK) ||
	(size & ~PAGE_MASK))
	return -1;

    struct vmm_context* ctx = &proc->vmm_ctx;

    mutex_lock(&ctx->lock);

    // allocate a region in the virtual address space
    *base = memory_map_alloc(&ctx->free_map, size);

    if (*base == 0)
	goto err1;

    // map the required pages
    for (size_t i = 0; i < size; i += PAGE_SIZE)
    {
	if (vmm_arch_proc_map(proc, *base + i, phys + i, flags) != 0)
	    goto err2;
    }

    mutex_unlock(&ctx->lock);

    return 0;

err2:
    // TODO: unmap already mapped pages ...
err1:
    mutex_unlock(&ctx->lock);
    return -1;
}

// =====================================================================================================================
int vmm_alloc(struct process* proc, ptr_t* base, size_t size, unsigned flags)
{
    // validate input
    if (size & ~PAGE_MASK)
	return -1;

    struct vmm_context* ctx = &proc->vmm_ctx;

    mutex_lock(&ctx->lock);

    int r;

    if (flags & VMM_FIXED)
    {
	if (*base & ~PAGE_MASK)
	    panic("vmm: invalid base for fixed alloc\n");

	r = memory_map_alloc_at(&proc->vmm_ctx.free_map, *base, size);
    }
    else
    {
	*base = memory_map_alloc(&proc->vmm_ctx.free_map, size);

	if (*base == 0)
	    r = -1;
	else
	    r = 0;
    }

    if (r != 0)
	goto err1;

    for (size_t i = 0; i < size; i += PAGE_SIZE)
    {
	ptr_t p = pmm_alloc();

	if (!p)
	    goto err2;

	if (vmm_arch_proc_map(proc, *base + i, p, flags) != 0)
	    goto err2;
    }

    mutex_unlock(&ctx->lock);

    return 0;

err2:
    // TODO: cleanup already allocated pages, etc ...
err1:
    mutex_unlock(&ctx->lock);
    return -1;
}

// =====================================================================================================================
int vmm_copy_to(struct process* proc, ptr_t virt, void* p, size_t size)
{
    uint8_t* data = (uint8_t*)p;
    struct vmm_context* ctx = &proc->vmm_ctx;

    mutex_lock(&ctx->lock);

    while (size > 0)
    {
	size_t off = virt & ~PAGE_MASK;
	size_t s = min(size, PAGE_SIZE - off);

	ptr_t phys;

	// translate the virtual address to a physical one that we can access
	if (vmm_arch_proc_translate(proc, virt - off, &phys) != 0)
	    goto err1;

	phys = vmm_provide_phys(phys);

	memcpy((uint8_t*)phys + off, data, s);

	virt += s;
	size -= s;
	data += s;
    }

    mutex_unlock(&ctx->lock);

    return 0;

err1:
    mutex_unlock(&ctx->lock);
    return -1;
}

// =====================================================================================================================
int vmm_clear(struct process* proc, ptr_t virt, size_t size)
{
    struct vmm_context* ctx = &proc->vmm_ctx;

    mutex_lock(&ctx->lock);

    while (size > 0)
    {
	size_t off = virt & ~PAGE_MASK;
	size_t s = min(size, PAGE_SIZE - off);

	ptr_t phys;

	// translate the virtual address to a physical one that we can access
	if (vmm_arch_proc_translate(proc, virt - off, &phys) != 0)
	    goto err1;

	phys = vmm_provide_phys(phys);

	memset((uint8_t*)phys + off, 0, s);

	virt += s;
	size -= s;
    }

    mutex_unlock(&ctx->lock);

    return 0;

err1:
    mutex_unlock(&ctx->lock);
    return -1;
}

// =====================================================================================================================
long sys_vmm_map(ptr_t phys, size_t size, ptr_t* base)
{
    // TODO: VMM_WRITE should be optional here
    return vmm_map(thread_current()->proc, base, phys, size, VMM_READ | VMM_WRITE | VMM_USER);
}

// =====================================================================================================================
long sys_vmm_alloc(size_t size, ptr_t* base)
{
    // TODO: VMM_WRITE should be optional here
    return vmm_alloc(thread_current()->proc, base, size, VMM_READ | VMM_WRITE | VMM_USER);
}

// =====================================================================================================================
int vmm_context_init(struct vmm_context* ctx)
{
    memory_map_init(&ctx->free_map, &s_map_allocator);

    // initialize the free map with the full userspace region and exclude the first page containing the NULL address
    memory_map_add(&ctx->free_map, USER_REGION_BASE, USER_REGION_SIZE);
    memory_map_exclude(&ctx->free_map, 0, PAGE_SIZE);

    mutex_init(&ctx->lock);

    return 0;
}

// =====================================================================================================================
int vmm_init()
{
    slab_cache_init(&s_entry_cache, sizeof(struct memory_map_entry));
    return 0;
}
