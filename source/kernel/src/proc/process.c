/* Process handling.
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

#include <kernel/proc/process.h>
#include <kernel/mm/slab.h>
#include <kernel/lib/hashtable.h>

static int s_next_process_id = 0;
static struct hashtable s_process_table;
static struct slab_cache s_process_cache;
static struct spinlock s_process_lock = SPINLOCK_INIT("process");

// =====================================================================================================================
struct process* process_create(const char* name)
{
    struct process* p = (struct process*)slab_cache_alloc(&s_process_cache);

    if (!p)
	goto err1;

    if (process_arch_create(p) != 0)
	goto err2;

    if (vmm_context_init(&p->vmm_ctx) != 0)
	goto err3;

    strncpy(p->name, name, PROC_NAME_SIZE);
    p->name[PROC_NAME_SIZE - 1] = 0;

    // insert the process to the global table
    spinlock_disable(&s_process_lock);
    p->id = s_next_process_id++;
    hashtable_add(&s_process_table, (struct hashitem*)p);
    spinunlock_enable(&s_process_lock);

    return p;

err3:
    process_arch_destroy(p);
err2:
    slab_cache_free(&s_process_cache, (void*)p);
err1:
    return NULL;
}

// =====================================================================================================================
static void* process_item_key(struct hashitem* item)
{
    struct process* p = (struct process*)item;
    return &p->id;
}

// =====================================================================================================================
static int process_item_compare(const void* k1, const void* k2)
{
    return *(int*)k1 == *(int*)k2;
}

// =====================================================================================================================
void process_init()
{
    process_arch_init();
    hashtable_init(&s_process_table, process_item_key, hashtable_hash_unsigned, process_item_compare);
    slab_cache_init(&s_process_cache, sizeof(struct process));

    // reserve the first process for the kernel
    process_create("kernel");
}
