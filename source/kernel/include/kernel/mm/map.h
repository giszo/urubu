/* Generic memory map handler.
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

#ifndef _KERNEL_MM_MAP_H_
#define _KERNEL_MM_MAP_H_

#include <kernel/types.h>

struct memory_map_entry
{
    // base address of the memory region
    ptr_t base;
    // size of the memory region
    size_t size;

    // link to the next entry in the memory map
    struct memory_map_entry* next;
};

struct memory_map_allocator
{
    struct memory_map_entry* (*alloc)();
    void (*free)(struct memory_map_entry*);
};

struct memory_map
{
    // the list of memory map entries
    struct memory_map_entry* list;

    // allocator for memory map entries
    struct memory_map_allocator* allocator;
};

typedef void memory_map_iterator(ptr_t, size_t);

int memory_map_add(struct memory_map* map, ptr_t base, size_t size);
int memory_map_exclude(struct memory_map* map, ptr_t base, size_t size);

ptr_t memory_map_alloc(struct memory_map* map, size_t size);
void memory_map_iterate(struct memory_map* map, memory_map_iterator* it);

int memory_map_init(struct memory_map* map, struct memory_map_allocator* allocator);

#endif
