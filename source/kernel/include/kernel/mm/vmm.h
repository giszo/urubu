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

#ifndef _KERNEL_MM_VMM_H_
#define _KERNEL_MM_VMM_H_

#include <kernel/types.h>
#include <kernel/mm/map.h>
#include <kernel/mm/slab.h>

struct process;

enum vmm_flags
{
    VMM_READ = (1 << 0),
    VMM_WRITE = (1 << 1),
    VMM_USER = (1 << 2),
    // an address is passed to vmm_alloc() to allocate the new region at that place
    VMM_FIXED = (1 << 3),
    // the region is allocated for stack
    VMM_STACK = (1 << 4)
};

struct vmm_context
{
    // map used for tracking free regions in the address space
    struct memory_map free_map;
};

int vmm_arch_map(ptr_t virt, ptr_t phys, unsigned flags);
int vmm_arch_unmap(ptr_t virt);

int vmm_arch_proc_alloc(struct process* proc, ptr_t virt, unsigned flags);
int vmm_arch_proc_translate(struct process* proc, ptr_t virt, ptr_t* phys);

/**
 * Converts the physical address of a memory page to a virtual that can be accessed in the address space of the kernel.
 */
ptr_t vmm_provide_phys(ptr_t phys);
/**
 * Converts the virtual address of a physical memory page back into its physical equivalent.
 */
ptr_t vmm_revert_phys(ptr_t virt);

int vmm_alloc(struct process* proc, ptr_t* base, size_t size, unsigned flags);

int vmm_copy_to(struct process* proc, ptr_t virt, void* p, size_t size);
int vmm_clear(struct process* proc, ptr_t virt, size_t size);

int vmm_context_init(struct vmm_context* ctx);

int vmm_init();

#endif
