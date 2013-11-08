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

#include <urubu/mm.h>

#include <syscall.h>

// =====================================================================================================================
void* mm_map(void* p, size_t size)
{
    void* base = NULL;
    int result = syscall3(SYS_vmm_map, (unsigned long)p, size, (unsigned long)&base);
    memory_barrier();
    if (result != 0)
	return NULL;
    return base;
}

// =====================================================================================================================
void* mm_alloc(size_t size)
{
    void* base = NULL;
    int result = syscall2(SYS_vmm_alloc, size, (unsigned long)&base);
    memory_barrier();
    if (result != 0)
	return NULL;
    return base;
}

// =====================================================================================================================
void mm_get_phys_stat(struct mm_phys_stat* stat)
{
    syscall1(SYS_pmm_get_statistics, (unsigned long)stat);
    memory_barrier();
}
