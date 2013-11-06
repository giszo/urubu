/* Physical memory manager.
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
#include <kernel/console.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/sync/spinlock.h>

#include <arch/mm/config.h>

// the table storing pointers to free physical memory pages
static ptr_t* s_freeTable;
// the number of pages put into the free table during initialization
static size_t s_freePageCount = 0;
// the number of currently allocated pages
static size_t s_allocatedPageCount = 0;

static struct spinlock s_pmm_lock = SPINLOCK_INIT("pmm");

// =====================================================================================================================
ptr_t pmm_alloc()
{
    ptr_t p;

    spinlock_disable(&s_pmm_lock);

    // check whether we run out of free memory
    if (s_allocatedPageCount == s_freePageCount)
	p = 0;
    else
	p = s_freeTable[s_allocatedPageCount++];

    spinunlock_enable(&s_pmm_lock);

    return p;
}

// =====================================================================================================================
void pmm_free(ptr_t p)
{
    if (p & ~PAGE_MASK)
	panic("pmm: tried to free a non page-aligned pointer!");

    spinlock_disable(&s_pmm_lock);

    if (s_allocatedPageCount == 0)
	panic("pmm: tried to free memory while there are no allocated pages!");

    s_freeTable[--s_allocatedPageCount] = p;

    spinunlock_enable(&s_pmm_lock);
}

// =====================================================================================================================
static void free_table_filler(ptr_t base, size_t size)
{
    for (size_t s = 0; s < size; s += PAGE_SIZE)
	s_freeTable[s_freePageCount++] = base + s;
}

// =====================================================================================================================
void pmm_init(size_t mem_size, struct memory_map* free_map)
{
    // Calculate how much memory is required to the addresses of free pages.
    size_t size = mem_size / PAGE_SIZE * sizeof(ptr_t);
    kprintf("pmm: %lu bytes of memory is used for free table\n", size);

    // Look for an entry in the free map that could store the table of free pages.
    ptr_t p = memory_map_alloc(free_map, PAGE_ALIGN(size));

    if (!p)
	panic("pmm: there is no memory to store the free table!");

    s_freeTable = (ptr_t*)vmm_provide_phys(p);

    // Build the free page table according to the provided map of free regions.
    memory_map_iterate(free_map, free_table_filler);

    kprintf("pmm: %lu bytes of physical memory is free\n", s_freePageCount * PAGE_SIZE);
}
