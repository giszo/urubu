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

#include <libdevman/server/device.h>

#include <libslab/cache.h>

#include <urubu/ipc.h>

static struct slab_cache s_dev_conn_cache;

// =====================================================================================================================
int device_announce(struct device* dev, enum device_type type, int port, struct device_ops* ops)
{
    dev->port = port;
    dev->ops = ops;

    if (slab_cache_init(&dev->conn_cache, sizeof(struct device_conn)) != 0)
	return -1;

    struct ipc_message msg;
    msg.data[0] = MSG_DEVICE_ANNOUNCE;
    msg.data[1] = type;
    msg.data[2] = port;

    return ipc_port_send_broadcast(IPC_BROADCAST_DEVICE, &msg);
}

// =====================================================================================================================
static void do_open(struct device* dev, struct ipc_message* m)
{
    struct ipc_message rep;

    // allocate a new connection object
    struct device_conn* c = (struct device_conn*)slab_cache_alloc(&s_dev_conn_cache);

    if (!c)
	goto err1;

    c->dev = dev;
    c->shmem = m->data[1];

    // accept the shared memory
    if (ipc_shmem_accept(c->shmem, &c->data, &c->size) != 0)
	goto err2;

    rep.data[0] = 0;
    rep.data[1] = (unsigned long)c; // TODO: a proper ID should be used instead of the pointer ...
    ipc_port_send(m->data[2], &rep);

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

    if (c->dev->ops->write == NULL || c->dev->ops->write((const void*)c->data, m->data[2]) != 0)
	rep.data[0] = -1;
    else
	rep.data[0] = 0;

    ipc_port_send(m->data[3], &rep);
}

// =====================================================================================================================
int device_run(struct device* dev)
{
    while (1)
    {
	struct ipc_message msg;

	if (ipc_port_receive(dev->port, &msg) != 0)
	    return -1;

	switch (msg.data[0])
	{
	    case MSG_DEVICE_OPEN :
		do_open(dev, &msg);
		break;

	    case MSG_DEVICE_CLOSE :
		break;

	    case MSG_DEVICE_READ :
		break;

	    case MSG_DEVICE_WRITE:
		do_write(&msg);
		break;
	}
    }

    return 0;
}

// =====================================================================================================================
int libdevman_init()
{
    slab_cache_init(&s_dev_conn_cache, sizeof(struct device_conn));
    return 0;
}
