/* Device manager server library.
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

#include "device.h"

#include <libsupport/slab.h>

#include <urubu/ipc.h>

static int s_local_port = -1;
static int s_devman_port = -1;

static int s_next_id = 0;
static struct hashtable s_dev_table;

static struct slab_cache s_dev_cache;
static struct slab_cache s_dev_conn_cache;

// =====================================================================================================================
int device_announce(enum device_type type, struct device_ops* ops, void* data, int* id)
{
    struct device* dev = (struct device*)slab_cache_alloc(&s_dev_cache);

    if (!dev)
	return -1;

    dev->id = s_next_id++;
    dev->ops = ops;
    dev->data = data;

    hashtable_add(&s_dev_table, (struct hashitem*)dev);

    struct ipc_message msg;
    msg.data[0] = MSG_DEVICE_ANNOUNCE;
    msg.data[1] = type;
    msg.data[2] = s_local_port;
    msg.data[3] = dev->id;

    if (ipc_port_send(s_devman_port, &msg) != 0)
	return -1;

    if (id)
	*id = dev->id;

    return 0;
}

// =====================================================================================================================
static void do_open(struct ipc_message* m)
{
    struct device* dev;
    struct ipc_message rep;

    int id = m->data[1];
    dev = (struct device*)hashtable_get(&s_dev_table, &id);

    if (!dev)
	goto err1;

    // allocate a new connection object
    struct device_conn* c = (struct device_conn*)slab_cache_alloc(&s_dev_conn_cache);

    if (!c)
	goto err1;

    c->dev = dev;
    c->shmem = m->data[2];

    // accept the shared memory
    if (ipc_shmem_accept(c->shmem, &c->data, &c->size) != 0)
	goto err2;

    rep.data[0] = 0;
    rep.data[1] = (unsigned long)c; // TODO: a proper ID should be used instead of the pointer ...
    ipc_port_send(m->data[3], &rep);

    return;

err2:
    slab_cache_free(&s_dev_conn_cache, (void*)c);
err1:
    rep.data[0] = -1;
    ipc_port_send(m->data[2], &rep);
}

// =====================================================================================================================
static void do_write(struct ipc_message* m)
{
    struct ipc_message rep;

    // TODO: do a proper lookup with an ID ...
    struct device_conn* c = (struct device_conn*)m->data[1];
    struct device* dev = c->dev;

    if (dev->ops->write == NULL || c->dev->ops->write(dev->data, (const void*)c->data, m->data[2]) != 0)
	rep.data[0] = -1;
    else
	rep.data[0] = 0;

    ipc_port_send(m->data[3], &rep);
}

// =====================================================================================================================
int libdevman_server_run(msg_handler* handler)
{
    while (1)
    {
	struct ipc_message msg;

	if (ipc_port_receive(s_local_port, &msg) != 0)
	    return -1;

	switch (msg.data[0])
	{
	    case MSG_DEVICE_OPEN :
		do_open(&msg);
		break;

	    case MSG_DEVICE_CLOSE :
		break;

	    case MSG_DEVICE_READ :
		break;

	    case MSG_DEVICE_WRITE:
		do_write(&msg);
		break;

	    default :
		if (handler)
		    handler(&msg);
		break;
	}
    }

    return 0;
}

// =====================================================================================================================
static const void* device_key(struct hashitem* item)
{
    struct device* dev = (struct device*)item;
    return &dev->id;
}

// =====================================================================================================================
int libdevman_server_init()
{
    s_local_port = ipc_port_create();

    if (s_local_port < 0)
	return -1;

    s_devman_port = ipc_server_lookup("devman", 1);

    hashtable_init(&s_dev_table, device_key, hashtable_hash_int, hashtable_compare_int);

    slab_cache_init(&s_dev_cache, sizeof(struct device));
    slab_cache_init(&s_dev_conn_cache, sizeof(struct device_conn));

    return 0;
}

// =====================================================================================================================
int libdevman_server_get_port()
{
    return s_local_port;
}
