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
struct device
{
    enum device_type type;
    int port;
    struct device* next;
};

static struct device* s_devices = NULL;
static struct slab_cache s_device_cache;

// =====================================================================================================================
int main(int argc, char** argv)
{
    // initialize device allocator
    slab_cache_init(&s_device_cache, sizeof(struct device));

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
	    {
		dbprintf("devman: new device announced!\n");

		struct device* dev = (struct device*)slab_cache_alloc(&s_device_cache);

		if (dev)
		{
		    dev->type = msg.data[1];
		    dev->port = msg.data[2];

		    // add the new device to the global list
		    dev->next = s_devices;
		    s_devices = dev;
		}

		break;
	    }
	}
    }

    return 0;
}
