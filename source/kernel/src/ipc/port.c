/* Ports for inter process communication.
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

// TODO: revise locking ...

#include <kernel/console.h>
#include <kernel/proc/thread.h>
#include <kernel/ipc/port.h>
#include <kernel/mm/slab.h>
#include <kernel/sync/spinlock.h>
#include <kernel/lib/hashtable.h>

static int s_next_port_id = 0;
static struct slab_cache s_port_cache;
static struct hashtable s_port_table;
static struct spinlock s_port_lock = SPINLOCK_INIT("ipc");

// =====================================================================================================================
static struct ipc_port* ipc_port_create()
{
    struct ipc_port* p = (struct ipc_port*)slab_cache_alloc(&s_port_cache);

    if (!p)
	return NULL;

    p->id = -1;
    p->broadcast_mask = 0;
    p->msg_first = NULL;
    p->msg_last = NULL;
    threadqueue_init(&p->waiters);

    return p;
}

// =====================================================================================================================
static int ipc_port_insert(struct ipc_port* p)
{
    spinlock_disable(&s_port_lock);

    p->id = s_next_port_id++;
    hashtable_add(&s_port_table, (struct hashitem*)p);

    spinunlock_enable(&s_port_lock);

    return 0;
}

// =====================================================================================================================
static void do_send(struct ipc_port* p, struct ipc_message* msg)
{
    struct thread* t;

    // link the new message into the list of the port
    if (p->msg_last)
	p->msg_last->next = msg;
    p->msg_last = msg;
    if (!p->msg_first)
	p->msg_first = msg;

    // get the first thread off the waiters
    t = threadqueue_pop(&p->waiters);

    // wake up the thread
    if (t)
	thread_wake_up(t);
}

// =====================================================================================================================
int ipc_port_send(int port, void* data)
{
    struct ipc_port* p;
    struct ipc_message* msg;

    spinlock_disable(&s_port_lock);

    // lookup the target port
    p = (struct ipc_port*)hashtable_get(&s_port_table, &port);

    if (!p)
	goto err1;

    // allocate a new message for storing the data
    msg = ipc_message_create();

    if (!msg)
	goto err1;

    // save the contents of the message
    memcpy(&msg->data, data, sizeof(struct ipc_user_msg));

    do_send(p, msg);

    spinunlock_enable(&s_port_lock);

    return 0;

err1:
    spinunlock_enable(&s_port_lock);
    return -1;
}

// =====================================================================================================================
long sys_ipc_port_create()
{
    struct ipc_port* p = ipc_port_create();

    if (!p)
	return -1;

    ipc_port_insert(p);

    return p->id;
}

// =====================================================================================================================
long sys_ipc_port_receive(int port, void* data)
{
    struct ipc_port* p;
    struct ipc_message* msg;

    spinlock_disable(&s_port_lock);

    // lookup the target port
    p = (struct ipc_port*)hashtable_get(&s_port_table, &port);

    if (!p)
    {
	spinunlock_enable(&s_port_lock);
	return -1;
    }

    // wait until a message is available on the port
    while (!p->msg_first)
    {
	// add the current thread to the waiters of the port
	threadqueue_add(&p->waiters, thread_current());
	spinunlock(&s_port_lock);
	// it's time to wait for a message ...
	thread_sleep();
	spinlock_disable(&s_port_lock);
    }

    // pop the first message from the port
    msg = p->msg_first;
    p->msg_first = msg->next;
    if (!p->msg_first)
	p->msg_last = NULL;

    spinunlock_enable(&s_port_lock);

    // copy the contents of the message to the buffer
    memcpy(data, &msg->data, sizeof(struct ipc_user_msg));

    // destroy the message
    ipc_message_destroy(msg);

    return 0;
}

// =====================================================================================================================
struct broadcast_msg
{
    unsigned mask;
    void* data;
};

// =====================================================================================================================
static void broadcast_sender(struct hashitem* item, void* data)
{
    struct ipc_port* p = (struct ipc_port*)item;
    struct broadcast_msg* d = (struct broadcast_msg*)data;

    // check whether this port is interested in the broadcast message
    if ((p->broadcast_mask & d->mask) == 0)
	return;

    // create a new message for this port
    struct ipc_message* msg = ipc_message_create();

    if (!msg)
	return;

    memcpy(&msg->data, d->data, sizeof(struct ipc_user_msg));

    // send the message ...
    do_send(p, msg);
}

// =====================================================================================================================
long sys_ipc_port_send_broadcast(unsigned broadcast, void* data)
{
    struct broadcast_msg d;
    d.mask = broadcast;
    d.data = data;

    spinlock_disable(&s_port_lock);
    hashtable_iterate(&s_port_table, broadcast_sender, &d);
    spinunlock_enable(&s_port_lock);

    return 0;
}

// =====================================================================================================================
long sys_ipc_port_set_broadcast_mask(int port, unsigned mask)
{
    struct ipc_port* p;

    spinlock_disable(&s_port_lock);

    // lookup the target port
    p = (struct ipc_port*)hashtable_get(&s_port_table, &port);

    if (!p)
    {
	spinunlock_enable(&s_port_lock);
	return -1;
    }

    // update the mask
    p->broadcast_mask = mask;

    spinunlock_enable(&s_port_lock);

    return 0;
}

// =====================================================================================================================
static void* ipc_port_item_key(struct hashitem* item)
{
    struct ipc_port* p = (struct ipc_port*)item;
    return &p->id;
}

// =====================================================================================================================
static int ipc_port_item_compare(const void* k1, const void* k2)
{
    return *(int*)k1 == *(int*)k2;
}

// =====================================================================================================================
void ipc_port_init()
{
    slab_cache_init(&s_port_cache, sizeof(struct ipc_port));
    hashtable_init(&s_port_table, ipc_port_item_key, hashtable_hash_unsigned, ipc_port_item_compare);
}
