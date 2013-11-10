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

#ifndef _KERNEL_IPC_SERVER_H_
#define _KERNEL_IPC_SERVER_H_

#include <kernel/lib/hashtable.h>

#define SERVER_NAME_SIZE 32

struct ipc_server
{
    struct hashitem _item;

    char name[SERVER_NAME_SIZE];
    int port;
};

struct ipc_server_waiter
{
    char name[SERVER_NAME_SIZE];
    struct thread* waiter;
    struct ipc_server_waiter* next;
};

long sys_ipc_server_register(const char* name, int port);
long sys_ipc_server_lookup(const char* name, int wait);

void ipc_server_init();

#endif
