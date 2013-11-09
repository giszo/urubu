/* Device manager client library.
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

#include <libdevman/client/device.h>

#include <urubu/ipc.h>

#include <string.h>

// =====================================================================================================================
int device_lookup(enum device_type type, int wait, struct device_info* info)
{
    int p;
    struct ipc_message req;
    struct ipc_message rep;

    // create a port for getting the reply
    p = ipc_port_create();

    if (p < 0)
	return -1;

    req.data[0] = MSG_DEVICE_LOOKUP;
    req.data[1] = type;
    req.data[2] = wait;
    req.data[3] = p;

    if (ipc_port_send_broadcast(IPC_BROADCAST_DEVICE, &req) != 0)
	goto err;

    if (ipc_port_receive(p, &rep) != 0)
	goto err;

    ipc_port_delete(p);

    // check result code
    if (rep.data[0] != 0)
	return -1;

    // fill device information structure
    info->port = rep.data[1];

    return 0;

err:
    ipc_port_delete(p);
    return -1;
}

// =====================================================================================================================
int device_open(struct device* dev, struct device_info* info)
{
    dev->port = info->port;
    dev->size = PAGE_SIZE;

    // allocate a reply port
    dev->reply = ipc_port_create();

    if (dev->reply < 0)
	goto err1;

    // allocate a shared memory region for data transfer
    dev->shmem = ipc_shmem_create(dev->size, &dev->data);

    if (dev->shmem < 0)
	goto err2;

    // send the request
    struct ipc_message msg;
    msg.data[0] = MSG_DEVICE_OPEN;
    msg.data[1] = dev->shmem;
    msg.data[2] = dev->reply;

    if (ipc_port_send(info->port, &msg) != 0)
	goto err3;

    // wait for the reply
    struct ipc_message rep;

    if (ipc_port_receive(dev->reply, &rep) != 0)
	goto err3;

    if (rep.data[0] != 0)
	goto err3;

    // save the connection ID received from the device
    dev->conn_id = rep.data[1];

    return 0;

err3:
    // TODO: destroy shmem
err2:
    ipc_port_delete(dev->reply);
err1:
    return -1;
}

// =====================================================================================================================
int device_write(struct device* dev, const void* data, size_t size)
{
    uint8_t* p = (uint8_t*)data;

    while (size > 0)
    {
	// find out how much data we can transfer in a chunk
	size_t s = size < dev->size ? size : dev->size;

	// copy the data to the shared buffer
	memcpy(dev->data, p, s);
	size -= s;
	p += s;

	// send the write request
	struct ipc_message msg;
	msg.data[0] = MSG_DEVICE_WRITE;
	msg.data[1] = dev->conn_id;
	msg.data[2] = s;
	msg.data[3] = dev->reply;

	if (ipc_port_send(dev->port, &msg) != 0)
	    return -1;

	// wait for the reply
	struct ipc_message rep;

	if (ipc_port_receive(dev->reply, &rep) != 0)
	    return -1;

	if (rep.data[0] != 0)
	    return  -1;
    }

    return 0;
}
