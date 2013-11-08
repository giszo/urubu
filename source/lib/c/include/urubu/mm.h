/* Urubu specific memory manager features.
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

#ifndef _URUBU_MM_H_
#define _URUBU_MM_H_

#include <stddef.h>
#include <stdint.h>

// TODO: move this to some architecture specific header
#define PAGE_SIZE 4096

#ifdef __cplusplus
extern "C"
{
#endif

struct mm_phys_stat
{
    // size of the full available system memory
    uint64_t memory_size;
    // size of the currently used physical memory
    uint64_t used_size;
};

void* mm_map(void* p, size_t size);
void* mm_alloc(size_t size);

void mm_get_phys_stat(struct mm_phys_stat* stat);

#ifdef __cplusplus
}
#endif

#endif
