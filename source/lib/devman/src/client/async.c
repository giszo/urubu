/* Asynchronous device handling functions.
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

#include <libdevman/client/async.h>

#include <libsupport/slab.h>

#include <urubu/ipc.h>

#include <string.h>

static int s_async_port = -1;

static struct slab_cache s_conn_req_cache;

// =====================================================================================================================
static void queue_request(struct device_conn* dev, struct device_conn_request* req)
{
    req->next = NULL;

    if (dev->queue_tail)
	dev->queue_tail->next = req;

    dev->queue_tail = req;

    if (!dev->queue_head)
	dev->queue_head = req;
}

// =====================================================================================================================
static void perform_request(struct device_conn* dev, struct device_conn_request* req)
{
    // set the current request on the device
    dev->request = req;

    struct ipc_message msg;

    switch (req->op)
    {
	case READ : msg.data[0] = MSG_DEVICE_READ; break;
	case WRITE : msg.data[0] = MSG_DEVICE_WRITE; break;
    }

    msg.data[1] = dev->conn_id;
    msg.data[2] = req->size;
    msg.data[3] = (unsigned long)dev; // TODO: use a proper ID here
    msg.data[4] = s_async_port;

    ipc_port_send(dev->port, &msg);
}

// =====================================================================================================================
static void complete_request(struct device_conn* dev, struct ipc_message* msg)
{
    struct device_conn_request* req = dev->request;
    int ret = msg->data[1];

    switch (req->op)
    {
	case READ :
	{
	    // copy the data into the read buffer
	    if (ret > 0)
		memcpy(req->data, dev->data, ret);
	    break;
	}

	case WRITE :
	    break;
    }

    // notify the caller that the request has been finished
    if (req->cb)
	req->cb(ret, req->p);

    slab_cache_free(&s_conn_req_cache, (void*)req);

    if (dev->queue_head)
    {
	req = dev->queue_head;

	dev->queue_head = req->next;

	if (!dev->queue_head)
	    dev->queue_tail = NULL;

	// perform the next request from the queue of the device
	perform_request(dev, req);
    }
    else
	dev->request = NULL;
}

// =====================================================================================================================
int device_read_async(struct device_conn* dev, void* data, size_t size, async_callback* cb, void* p)
{
    struct device_conn_request* req = (struct device_conn_request*)slab_cache_alloc(&s_conn_req_cache);

    if (!req)
	return -1;

    req->op = READ;
    req->data = data;
    req->size = size;
    req->cb = cb;
    req->p = p;

    if (dev->request)
	queue_request(dev, req);
    else
	perform_request(dev, req);

    return 0;
}

// =====================================================================================================================
int libdevman_client_async_process(struct ipc_message* msg)
{
    switch (msg->data[0])
    {
	case MSG_DEVICE_READ_REPLY :
	case MSG_DEVICE_WRITE_REPLY :
	    complete_request((struct device_conn*)msg->data[2], msg);
	    break;

	default :
	    // no, we are not interested in this message ...
	    return 0;
    }

    // okay, we handled the message :]
    return 1;
}

// =====================================================================================================================
int libdevman_client_async_init(int port)
{
    s_async_port = port;
    slab_cache_init(&s_conn_req_cache, sizeof(struct device_conn_request));
    return 0;
}
