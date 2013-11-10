/* Support for userspace servers.
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

#include <kernel/ipc/server.h>
#include <kernel/mm/slab.h>
#include <kernel/proc/thread.h>
#include <kernel/sync/spinlock.h>

static struct hashtable s_server_table;
static struct ipc_server_waiter* s_waiters = NULL;

static struct slab_cache s_server_cache;
static struct slab_cache s_waiter_cache;

static struct spinlock s_server_lock;

// =====================================================================================================================
long sys_ipc_server_register(const char* name, int port)
{
    struct ipc_server* s;

    spinlock_disable(&s_server_lock);

    s = (struct ipc_server*)slab_cache_alloc(&s_server_cache);

    if (!s)
	goto err;

    strncpy(s->name, name, SERVER_NAME_SIZE - 1);
    s->name[SERVER_NAME_SIZE - 1] = 0;
    s->port = port;

    hashtable_add(&s_server_table, (struct hashitem*)s);

    // notify possible waiters
    struct ipc_server_waiter* prev = NULL;
    struct ipc_server_waiter* waiter = s_waiters;

    while (waiter)
    {
	if (strcmp(waiter->name, name) == 0)
	{
	    // remove from the list
	    if (prev)
		prev->next = waiter->next;
	    else
		s_waiters = waiter->next;

	    // wake the waiter up
	    thread_wake_up(waiter->waiter);

	    struct ipc_server_waiter* tmp = waiter;
	    waiter = waiter->next;

	    // free the waiter entry
	    slab_cache_free(&s_waiter_cache, (void*)tmp);

	    continue;
	}

	prev = waiter;
	waiter = waiter->next;
    }

    spinunlock_enable(&s_server_lock);

    return 0;

err:
    spinunlock_enable(&s_server_lock);
    return -1;
}

// =====================================================================================================================
long sys_ipc_server_lookup(const char* name, int wait)
{
    long ret;
    struct ipc_server* s;

    spinlock_disable(&s_server_lock);

    s = (struct ipc_server*)hashtable_get(&s_server_table, name);

    while (!s && wait)
    {
	// wait for the server to register
	struct ipc_server_waiter* w = (struct ipc_server_waiter*)slab_cache_alloc(&s_waiter_cache);

	if (!w)
	    break;

	strncpy(w->name, name, SERVER_NAME_SIZE - 1);
	w->name[SERVER_NAME_SIZE - 1] = 0;
	w->waiter = thread_current();

	spinunlock(&s_server_lock);
	thread_sleep();
	spinlock_disable(&s_server_lock);

	s = (struct ipc_server*)hashtable_get(&s_server_table, name);
    }

    if (s)
	ret = s->port;
    else
	ret = -1;

    spinunlock_enable(&s_server_lock);

    return ret;
}

// =====================================================================================================================
static const void* ipc_server_key(struct hashitem* item)
{
    struct ipc_server* s = (struct ipc_server*)item;
    return s->name;
}

// =====================================================================================================================
void ipc_server_init()
{
    hashtable_init(&s_server_table, ipc_server_key, hashtable_hash_string, hashtable_compare_string);
    slab_cache_init(&s_server_cache, sizeof(struct ipc_server));
    slab_cache_init(&s_waiter_cache, sizeof(struct ipc_server_waiter));
    spinlock_init(&s_server_lock, "ipc server");
}
