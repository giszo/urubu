/* amd64 memory manager configuration.
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

#ifndef _AMD64_MM_CONFIG_H_
#define _AMD64_MM_CONFIG_H_

#define KERNEL_PHYS_BASE 0x0000000000200000
#define KERNEL_VIRT_BASE 0xffffffff80000000

// Boot modules will be mapped here temporarly during booting
#define MODULE_MAP_BASE  0xffffffffc0000000

// The physical page mapping area (size is 128Tb - 2Gb)
#define PHYS_MAP_BASE 0xffff800000000000
#define PHYS_MAP_SIZE 0x00007fff80000000

// Userspace memory region base and size
#define USER_REGION_BASE 0x0000000000000000
#define USER_REGION_SIZE 0x0000800000000000

#define PAGE_SIZE 0x1000
#define PAGE_MASK (~(PAGE_SIZE - 1))

#define PAGE_ALIGN(a) (((a) + PAGE_SIZE - 1) & PAGE_MASK)

// Converts a physical address to its virtual equivalent.
#define MM_P2V(a) ((a) + KERNEL_VIRT_BASE)

// Converts a virtual address to its physical equivalent.
#define MM_V2P(a) ((a) - KERNEL_VIRT_BASE)

#endif
