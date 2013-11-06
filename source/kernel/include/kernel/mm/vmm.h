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

struct process;

enum vmm_flags
{
    VMM_WRITE = (1 << 0),
    VMM_USER = (1 << 1)
};

int vmm_arch_map(ptr_t virt, ptr_t phys, unsigned flags);
int vmm_arch_unmap(ptr_t virt);

ptr_t vmm_arch_proc_alloc(struct process* proc, ptr_t virt, unsigned flags);

/**
 * Converts the physical address of a memory page to a virtual that can be accessed in the address space of the kernel.
 */
ptr_t vmm_provide_phys(ptr_t phys);
/**
 * Converts the virtual address of a physical memory page back into its physical equivalent.
 */
ptr_t vmm_revert_phys(ptr_t virt);

#endif
