/* Hashtable implementation.
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

#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/lib/hashtable.h>

#include <arch/mm/config.h>

// =====================================================================================================================
static inline unsigned hash_number(uint8_t* d, size_t len) {
    unsigned hash = 0;

    for (size_t i = 0; i < len; ++i, ++d)
    {
	hash += *d;
	hash += (hash << 10);
	hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

// =====================================================================================================================
unsigned hashtable_hash_unsigned(const void* k)
{
    return hash_number((uint8_t*)k, sizeof(unsigned));
}

// =====================================================================================================================
void hashtable_add(struct hashtable* table, struct hashitem* item)
{
    void* key = table->key(item);
    uint32_t idx = table->hash(key) % table->size;

    item->next = table->table[idx];
    table->table[idx] = item;

    ++table->items;
}

// =====================================================================================================================
struct hashitem* hashtable_get(struct hashtable* table, const void* key)
{
    uint32_t idx = table->hash(key) % table->size;

    struct hashitem* item = table->table[idx];

    while (item)
    {
	const void* k = table->key(item);

	if (table->compare(key, k))
	    return item;

	item = item->next;
    }

    return NULL;
}

// =====================================================================================================================
void hashtable_iterate(struct hashtable* t, hashtable_iterator* it, void* data)
{
    for (size_t i = 0; i < t->size; ++i)
    {
	struct hashitem* item = t->table[i];

	while (item)
	{
	    it(item, data);
	    item = item->next;
	}
    }
}

// =====================================================================================================================
size_t hashtable_size(struct hashtable* t)
{
    return t->items;
}

// =====================================================================================================================
void hashtable_init(struct hashtable* t, key_func_t* key, hash_func_t* hash, compare_func_t* compare)
{
    ptr_t p = pmm_alloc();

    if (!p)
	return;

    t->table = (struct hashitem**)vmm_provide_phys(p);
    t->size = PAGE_SIZE / sizeof(struct hashitem*);
    t->items = 0;
    t->key = key;
    t->hash = hash;
    t->compare = compare;
}
