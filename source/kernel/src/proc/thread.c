/* Thread handling.
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

#include <kernel/console.h>
#include <kernel/cpu/cpu.h>
#include <kernel/proc/thread.h>
#include <kernel/proc/sched.h>
#include <kernel/mm/slab.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/lib/hashtable.h>

static int s_next_thread_id = 0;
static struct hashtable s_thread_table;
static struct slab_cache s_thread_cache;

// =====================================================================================================================
static struct thread* thread_create(const char* name)
{
    ptr_t p;
    struct thread* t = (struct thread*)slab_cache_alloc(&s_thread_cache);

    if (!t)
	goto err1;

    p = pmm_alloc();

    if (!p)
	goto err2;

    t->kernel_stack = (void*)vmm_provide_phys(p);

    if (thread_arch_create(t) != 0)
	goto err3;

    t->id = -1;
    t->state = CREATED;
    t->proc = NULL;
    t->kernel_stack_size = PAGE_SIZE;
    t->user_stack = NULL;
    t->user_stack_size = 0;

    strncpy(t->name, name, THREAD_NAME_SIZE);
    t->name[THREAD_NAME_SIZE - 1] = 0;

    return t;

err3:
    pmm_free(p);
err2:
    slab_cache_free(&s_thread_cache, (void*)t);
err1:
    return NULL;
}

// =====================================================================================================================
static void thread_destroy(struct thread* t)
{
    pmm_free(vmm_revert_phys((ptr_t)t->kernel_stack));
    thread_arch_destroy(t);
    slab_cache_free(&s_thread_cache, (void*)t);
}

// =====================================================================================================================
static void thread_insert(struct thread* t)
{
    t->id = s_next_thread_id++;
    hashtable_add(&s_thread_table, (struct hashitem*)t);
}

// =====================================================================================================================
static void* thread_item_key(struct hashitem* item)
{
    struct thread* t = (struct thread*)item;
    return &t->id;
}

// =====================================================================================================================
static int thread_item_compare(const void* k1, const void* k2)
{
    return *(int*)k1 == *(int*)k2;
}

// =====================================================================================================================
void thread_kernel_exit()
{
    kprintf("thread: exiting ...\n");
    while (1) ;
}

// =====================================================================================================================
struct thread* thread_create_kernel(const char* name, void* entry, void* arg)
{
    struct thread* t = thread_create(name);

    if (!t)
	goto err1;

    if (thread_arch_create_kernel(t, entry, arg) != 0)
	goto err2;

    thread_insert(t);

    return t;

err2:
    thread_destroy(t);
err1:
    return NULL;
}

// =====================================================================================================================
struct thread* thread_create_user(struct process* p, const char* name, void* entry, void* arg, void* stack, size_t stack_size)
{
    struct thread* t = thread_create(name);

    if (!t)
	goto err1;

    t->proc = p;

    // set the userspace stack of the process
    t->user_stack = stack;
    t->user_stack_size = stack_size;

    if (thread_arch_create_user(t, entry, arg) != 0)
	goto err2;

    thread_insert(t);

    return t;

 err2:
    thread_destroy(t);
 err1:
    return NULL;
}

// =====================================================================================================================
struct thread* thread_current()
{
    return cpu_get_current()->thread_current;
}

// =====================================================================================================================
int thread_sleep()
{
    struct thread* t;

    // disable interrupts to avoid getting preempted before we get into the scheduler
    cpu_arch_disable_int();

    t = thread_current();
    t->state = SLEEPING;

    // give up ...
    sched_yield();

    return 0;
}

// =====================================================================================================================
int thread_wake_up(struct thread* t)
{
    sched_add_ready(t);
    return 0;
}

// =====================================================================================================================
void thread_init()
{
    thread_arch_init();
    hashtable_init(&s_thread_table, thread_item_key, hashtable_hash_unsigned, thread_item_compare);
    slab_cache_init(&s_thread_cache, sizeof(struct thread));
}
