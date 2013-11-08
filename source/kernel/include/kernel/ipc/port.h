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

#ifndef _KERNEL_IPC_PORT_H_
#define _KERNEL_IPC_PORT_H_

#include <kernel/proc/threadqueue.h>
#include <kernel/ipc/message.h>
#include <kernel/lib/hashtable.h>

struct ipc_port
{
    struct hashitem _item;

    // ID of the port
    int id;

    // mask broadcast informations this port is interested in
    unsigned broadcast_mask;

    // list of messages waiting on this port
    struct ipc_message* msg_first;
    struct ipc_message* msg_last;

    // threads waiting for something to receive on this port
    struct threadqueue waiters;
};

int ipc_port_send(int port, void* data);

long sys_ipc_port_create();
long sys_ipc_port_receive(int port, void* data);
long sys_ipc_port_send_broadcast(unsigned broadcast, void* data);

long sys_ipc_port_set_broadcast_mask(int port, unsigned mask);

void ipc_port_init();

#endif
