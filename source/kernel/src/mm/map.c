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

// TODO: testing, a lot of!

#include <kernel/kernel.h>
#include <kernel/mm/map.h>
#include <kernel/lib/string.h>

// =====================================================================================================================
enum entry_collision_type
{
    NOTHING,
    BEGIN,
    END,
    INSIDE,
    OVERLAP
};

// =====================================================================================================================
static inline enum entry_collision_type check_entry_collision(struct memory_map_entry* e1, struct memory_map_entry* e2)
{
    ptr_t e1_begin = e1->base;
    ptr_t e1_end = e1->base + e1->size - 1;

    ptr_t e2_begin = e2->base;
    ptr_t e2_end = e2->base + e2->size - 1;

    if (e2_begin <= e1_begin && e1_end <= e2_end)
	return OVERLAP;
    if (e2_begin >= e1_begin && e2_end <= e1_end)
	return INSIDE;
    if (e2_begin >= e1_begin && e2_begin <= e1_end)
	return BEGIN;
    if (e2_end >= e1_begin && e2_end <= e1_end)
	return END;

    return NOTHING;
}

// =====================================================================================================================
int memory_map_add(struct memory_map* map, ptr_t base, size_t size)
{
    // allocate a new entry
    struct memory_map_entry* e = map->allocator->alloc();

    if (!e)
	return -1;

    // ... and initialize it
    e->base = base;
    e->size = size;

    struct memory_map_entry* prev = NULL;
    struct memory_map_entry* item = map->list;

    while (item)
    {
	// check overlapping with the current item
	if (check_entry_collision(item, e) != NOTHING)
	    return -1;

	// terminate iteration if the new entry should be inserted before the current one
	if (item->base > base)
	    break;

	prev = item;
	item = item->next;
    }

    // insert the new entry
    if (prev)
    {
	e->next = prev->next;
	prev->next = e;
    }
    else
    {
	e->next = map->list;
	map->list = e;
    }

    return 0;
}

// =====================================================================================================================
int memory_map_exclude(struct memory_map* map, ptr_t base, size_t size)
{
    // create an entry for the excluded region for collision checking
    struct memory_map_entry e;
    e.base = base;
    e.size = size;

    struct memory_map_entry* prev = NULL;
    struct memory_map_entry* item = map->list;

    while (item)
    {
	enum entry_collision_type type = check_entry_collision(item, &e);

	switch (type)
	{
	    case BEGIN :
		item->size = e.base - item->base;

		if (item->size == 0)
		{
		    panic("map: invalid size after truncating entry!\n");
		}

		break;

	    case END :
	    {
		ptr_t new_base = e.base + e.size;
		item->size -= (new_base - item->base);
		item->base = new_base;

		if (item->size == 0)
		    panic("map: invalid size after truncating entry!\n");

		break;
	    }

	    case OVERLAP :
	    {
		struct memory_map_entry* tmp = item;

		// the current entry must be removed from the list
		if (prev)
		    prev->next = item->next;
		else
		    map->list = item->next;

		item = item->next;

		// free the removed item
		map->allocator->free(tmp);

		// skip the normal list iteration ...
		continue;
	    }

	    case INSIDE :
	    {
		ptr_t e_end = e.base + e.size - 1;
		ptr_t item_end = item->base + item->size - 1;

		if (item->base == e.base)
		{
		    item->base += e.size;
		    item->size -= e.size;

		    if (item->size == 0)
			panic("map: invalid size after truncating entry!\n");
		}
		else if (item_end == e_end)
		{
		    item->size -= e.size;

		    if (item->size == 0)
			panic("map: invalid size after truncating entry!\n");
		}
		else
		{
		    // create a new item with the remaining space at the end of the current one
		    struct memory_map_entry* tmp = map->allocator->alloc();

		    if (!tmp)
			return -1;

		    tmp->base = e.base + e.size;
		    tmp->size = item->size - (tmp->base - item->base);

		    // link the new item into the map
		    tmp->next = item->next;
		    item->next = tmp;

		    // truncate the current item with the base of the excluded region
		    item->size = e.base - item->base;

		    if (item->size == 0)
			panic("map: invalid size after truncating entry!\n");

		    prev = tmp;
		    item = tmp->next;

		    // skip the normal list iteration ...
		    continue;
		}

		break;
	    }

	    case NOTHING :
		// nothing to do here, whew ...
		break;
	}

	prev = item;
	item = item->next;
    }

    return 0;
}

// =====================================================================================================================
ptr_t memory_map_alloc(struct memory_map* map, size_t size)
{
    ptr_t p = 0;
    struct memory_map_entry* prev = NULL;
    struct memory_map_entry* item = map->list;

    while (item)
    {
	if (item->size >= size)
	{
	    p = item->base;

	    if (item->size == size)
	    {
		// remove the current item
		prev->next = item->next;
		map->allocator->free(item);
	    }
	    else
	    {
		// truncate the current item
		item->base += size;
		item->size -= size;
	    }

	    break;
	}

	prev = item;
	item = item->next;
    }

    return p;
}

// =====================================================================================================================
void memory_map_iterate(struct memory_map* map, memory_map_iterator* it)
{
    struct memory_map_entry* item = map->list;

    while (item)
    {
	it(item->base, item->size);
	item = item->next;
    }
}

// =====================================================================================================================
int memory_map_init(struct memory_map* map, struct memory_map_allocator* allocator)
{
    map->list = NULL;
    map->allocator = allocator;

    return 0;
}
