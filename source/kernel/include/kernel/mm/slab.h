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

#ifndef _KERNEL_MM_SLAB_H_
#define _KERNEL_MM_SLAB_H_

#include <kernel/types.h>
#include <kernel/sync/spinlock.h>

struct slab;
struct slab_buffer;

struct slab_cache
{
    size_t slab_size;
    size_t obj_size;
    size_t obj_per_slab;

    struct slab* slab_full;
    struct slab* slab_partial;

    struct spinlock lock;
};

struct slab
{
    struct slab* next;
    struct slab_cache* cache;
    void* base;
    struct slab_buffer* free_list;
    size_t free_count;
};

struct slab_buffer
{
    struct slab_buffer* next;
};

void slab_cache_init(struct slab_cache* cache, size_t obj_size);

void* slab_cache_alloc(struct slab_cache* cache);
void slab_cache_free(struct slab_cache* cache, void* p);

#endif /* _KERNEL_MM_SLAB_H_ */
