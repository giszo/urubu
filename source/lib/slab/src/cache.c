/* SLAB allocator.
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

#include <libslab/cache.h>

#include <urubu/mm.h>

// =====================================================================================================================
static struct slab* slab_create(struct slab_cache* cache)
{
    uint8_t* data = (uint8_t*)mm_alloc(PAGE_SIZE);

    if (!data)
	return NULL;

    struct slab* s = (struct slab*)(data + cache->slab_size - sizeof(struct slab));

    s->next = NULL;
    s->cache = cache;
    s->base = (void*)data;
    s->free_list = NULL;
    s->free_count = cache->obj_per_slab;

    for (size_t i = 0; i < cache->obj_per_slab; ++i)
    {
	struct slab_buffer* buffer = (struct slab_buffer*)data;

	buffer->next = s->free_list;
	s->free_list = buffer;

	data += cache->obj_size;
    }

    return s;
}

// =====================================================================================================================
static void slab_free(struct slab_cache* cache, struct slab* s)
{
    uint8_t* p = (uint8_t*)s + sizeof(struct slab) - cache->slab_size;
    // TODO: free ...
}

// =====================================================================================================================
int slab_cache_init(struct slab_cache* cache, size_t obj_size)
{
    // align the object size to the native pointer size
    cache->obj_size = (obj_size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

    if (cache->obj_size < sizeof(struct slab_buffer))
	return -1;

    cache->slab_size = PAGE_SIZE; /* for now we support only PAGE_SIZEd slabs */
    cache->obj_per_slab = (cache->slab_size - sizeof(struct slab)) / cache->obj_size;

    if (cache->obj_per_slab < 1)
	return -1;

    cache->slab_full = NULL;
    cache->slab_partial = NULL;

    return 0;
}

// =====================================================================================================================
void* slab_cache_alloc(struct slab_cache* cache)
{
    struct slab* s;

    /* Create a new slab if we run out of space. */
    if (!cache->slab_partial)
    {
	s = slab_create(cache);

	if (!s)
	    return NULL;

	s->next = NULL;
	cache->slab_partial = s;
    }

    /* Get a slab to allocate from. */
    s = cache->slab_partial;

    /* Remove the first buffer from the free list, this will be the newly allocated object. */
    struct slab_buffer* buffer = s->free_list;
    s->free_list = buffer->next;
    s->free_count--;

    /* Check whether the slab run out of free objects. */
    if (!s->free_list)
    {
	/* Remove from the partial list. */
	cache->slab_partial = s->next;

	/* Add to the full list. */
	s->next = cache->slab_full;
	cache->slab_full = s;
    }

    return (void*)buffer;
}

// =====================================================================================================================
void slab_cache_free(struct slab_cache* cache, void* p)
{
    /* Get the slab for the pointer. */
    struct slab* s = (struct slab*)(((unsigned long)p & ~(cache->slab_size - 1)) + cache->slab_size - sizeof(struct slab));

    // TODO: abort() should be called here ...
    if (s->cache != cache)
	return;

    int slab_was_full = (s->free_count == 0);

    /* Add the buffer to the free list of the slab. */
    struct slab_buffer* buffer = (struct slab_buffer*)p;
    buffer->next = s->free_list;
    s->free_list = buffer;
    s->free_count++;

    int slab_is_free = (s->free_count == cache->obj_per_slab);

    /* Remove this slab from its list? */
    if (slab_was_full || slab_is_free)
    {
	struct slab* prev = NULL;
	struct slab* item = slab_was_full ? cache->slab_full : cache->slab_partial;

	while (item)
	{
	    if (item == s)
	    {
		if (prev == NULL)
		{
		    if (slab_was_full)
			cache->slab_full = item->next;
		    else
			cache->slab_partial = item->next;
		}
		else
		    prev->next = item->next;

		break;
	    }

	    prev = item;
	    item = item->next;
	}
    }

    if (slab_is_free)
	slab_free(cache, s);
    else if (slab_was_full)
    {
	s->next = cache->slab_partial;
	cache->slab_partial = s;
    }
}
