/* Shared memory for inter process communication.
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

#include <kernel/ipc/shmem.h>
#include <kernel/mm/slab.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/sync/mutex.h>

static int s_next_shmem_id = 0;
static struct hashtable s_shmem_table;
static struct slab_cache s_shmem_cache;
static struct slab_cache s_shmem_block_cache;
static struct mutex s_shmem_lock;

// =====================================================================================================================
long sys_ipc_shmem_create(size_t size, void** base)
{
    struct shmem* s;

    if (size & ~PAGE_SIZE)
	return -1;

    mutex_lock(&s_shmem_lock);

    s = (struct shmem*)slab_cache_alloc(&s_shmem_cache);

    if (!s)
	goto err1;

    // initialize the shared memory
    s->owner = thread_current()->proc;
    s->user = NULL;
    s->size = size;

    struct shmem_block* prev = NULL;
    struct shmem_block* block = NULL;

    // allocate physical pages for the shared region
    for (size_t i = 0; i < size; i += PAGE_SIZE)
    {
	// allocate a new shmem block if needed
	if (!block)
	{
	    block = (struct shmem_block*)slab_cache_alloc(&s_shmem_block_cache);

	    if (!block)
		goto err2;

	    block->used = 0;
	    block->next = NULL;

	    if (!prev)
		s->blocks = block;
	    else
		prev->next = block;
	}

	// allocate a physical page
	ptr_t p = pmm_alloc();

	if (!p)
	    goto err2;

	block->pages[block->used] = p;

	if (++block->used == SHMEM_PAGES_PER_BLOCK)
	    block = NULL;
    }

    // map the memory to the address space of the owner
    ptr_t p = 0;

    if (vmm_map_shmem(s->owner, &p, s->blocks, s->size, VMM_READ | VMM_WRITE | VMM_USER) != 0)
	goto err2;

    *base = (void*)p;

    // add to the global table
    s->id = s_next_shmem_id++;
    hashtable_add(&s_shmem_table, (struct hashitem*)s);

    mutex_unlock(&s_shmem_lock);

    return s->id;

err2:
    // TODO: cleanup
err1:
    mutex_unlock(&s_shmem_lock);
    return -1;
}

// =====================================================================================================================
long sys_ipc_shmem_accept(int id, void** base, size_t* size)
{
    struct shmem* s;

    mutex_lock(&s_shmem_lock);

    // lookup the port
    s = (struct shmem*)hashtable_get(&s_shmem_table, &id);

    if (!s)
	goto err;

    // check whether someone already accepted this piece of memory
    if (s->user != NULL)
	goto err;

    // update the user
    s->user = thread_current()->proc;

    ptr_t p = 0;

    // map the shared memory pages to this process
    if (vmm_map_shmem(s->user, &p, s->blocks, s->size, VMM_READ | VMM_WRITE | VMM_USER) != 0)
	goto err;

    *base = (void*)p;
    *size = s->size;

    mutex_unlock(&s_shmem_lock);

    return 0;

err:
    mutex_unlock(&s_shmem_lock);
    return -1;
}

// =====================================================================================================================
long sys_ipc_shmem_close(int id)
{
    // TODO
    return -1;
}

// =====================================================================================================================
static void* ipc_shmem_item_key(struct hashitem* item)
{
    struct shmem* s = (struct shmem*)item;
    return &s->id;
}

// =====================================================================================================================
static int ipc_shmem_item_compare(const void* k1, const void* k2)
{
    return *(int*)k1 == *(int*)k2;
}

// =====================================================================================================================
void ipc_shmem_init()
{
    hashtable_init(&s_shmem_table, ipc_shmem_item_key, hashtable_hash_unsigned, ipc_shmem_item_compare);
    slab_cache_init(&s_shmem_cache, sizeof(struct shmem));
    slab_cache_init(&s_shmem_block_cache, sizeof(struct shmem_block));
    mutex_init(&s_shmem_lock);
}
