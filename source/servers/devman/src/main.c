/* Device manager server.
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

#include <urubu/ipc.h>
#include <urubu/debug.h>

#include <libslab/cache.h>

#include <libdevman/device.h>

// =====================================================================================================================
struct waiter
{
    // the reply port
    int port;
    struct waiter* next;
};

// =====================================================================================================================
struct device
{
    // the main port of the device
    int port;
    struct device* next;
};

// =====================================================================================================================
struct list
{
    // list of devices
    struct device* next;
    // list of applications waiting for this type of device
    struct waiter* waiters;
};

static struct list s_devices[DEV_TYPE_COUNT];

static struct slab_cache s_device_cache;
static struct slab_cache s_waiter_cache;

// =====================================================================================================================
static void announce(struct ipc_message* m)
{
    dbprintf("devman: new device announced!\n");

    struct device* dev = (struct device*)slab_cache_alloc(&s_device_cache);

    if (!dev)
	return;

    dev->port = m->data[2];

    enum device_type type = m->data[1];

    // add the new device to the global list
    dev->next = s_devices[type].next;
    s_devices[type].next = dev;

    // notify waiters
    struct ipc_message msg;
    msg.data[0] = 0;
    msg.data[1] = dev->port;

    struct waiter* w = s_devices[type].waiters;

    while (w)
    {
	ipc_port_send(w->port, &msg);

	struct waiter* tmp = w;
	w = w->next;

	slab_cache_free(&s_waiter_cache, (void*)tmp);
    }

    s_devices[type].waiters = NULL;
}

// =====================================================================================================================
static void lookup(struct ipc_message* m)
{
    struct ipc_message reply;
    enum device_type type = m->data[1];

    // lookup the device
    struct device* dev = s_devices[type].next;

    if (!dev)
    {
	if (m->data[2])
	{
	    // wait for a device
	    struct waiter* w = (struct waiter*)slab_cache_alloc(&s_waiter_cache);

	    if (!w)
		goto send_error;

	    w->port = m->data[3];

	    w->next = s_devices[type].waiters;
	    s_devices[type].waiters = w;

	    return;
	}
	else
	    goto send_error;
    }

    reply.data[0] = 0;
    reply.data[1] = dev->port;

send_reply:
    ipc_port_send(m->data[3], &reply);

send_error:
    reply.data[0] = -1;
    goto send_reply;
}

// =====================================================================================================================
int main(int argc, char** argv)
{
    // initialize memory allocators
    slab_cache_init(&s_device_cache, sizeof(struct device));
    slab_cache_init(&s_waiter_cache, sizeof(struct waiter));

    // make each device list empty
    for (unsigned i = 0; i < DEV_TYPE_COUNT; ++i)
    {
	s_devices[i].next = NULL;
	s_devices[i].waiters = NULL;
    }

    int p = ipc_port_create();

    if (p < 0)
    {
	dbprintf("devman: unable to create IPC port!\n");
	return -1;
    }

    // we are interested in device broadcast messages
    ipc_port_set_broadcast_mask(p, IPC_BROADCAST_DEVICE);

    while (1)
    {
	struct ipc_message msg;

	if (ipc_port_receive(p, &msg) != 0)
	{
	    dbprintf("devman: unable to receive from port\n");
	    break;
	}

	switch (msg.data[0])
	{
	    case MSG_ANNOUNCE_DEVICE :
		announce(&msg);
		break;

	    case MSG_LOOKUP_DEVICE :
		lookup(&msg);
		break;
	}
    }

    return 0;
}
