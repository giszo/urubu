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

#include <libsupport/hashtable.h>

#include <urubu/mm.h>

// =====================================================================================================================
unsigned hashtable_hash_int(const void* k)
{
    unsigned hash = 0;
    const char* n = (const char*)k;

    for (size_t i = 0; i < sizeof(int); ++i, ++n)
    {
	hash += *n;
	hash += (hash << 10);
	hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;

}

// =====================================================================================================================
int hashtable_compare_int(const void* k1, const void* k2)
{
    return *(int*)k1 == *(int*)k2;
}

// =====================================================================================================================
void hashtable_add(struct hashtable* table, struct hashitem* item)
{
    const void* key = table->key(item);
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
struct hashitem* hashtable_remove(struct hashtable* t, const void* key)
{
    uint32_t idx = t->hash(key) % t->size;

    struct hashitem* prev = NULL;
    struct hashitem* item = t->table[idx];

    while (item)
    {
	const void* k = t->key(item);

	if (t->compare(key, k))
	{
	    if (prev)
		prev->next = item->next;
	    else
		t->table[idx] = item->next;

	    return item;
	}

	prev = item;
	item = item->next;
    }

    return NULL;
}

// =====================================================================================================================
int hashtable_init(struct hashtable* t, key_func_t* key, hash_func_t* hash, compare_func_t* compare)
{
    t->table = (struct hashitem**)mm_alloc(PAGE_SIZE);

    if (!t->table)
	return -1;

    t->size = PAGE_SIZE / sizeof(struct hashitem*);
    t->items = 0;
    t->key = key;
    t->hash = hash;
    t->compare = compare;

    return 0;
}
