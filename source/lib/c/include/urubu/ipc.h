/* Inter process communication.
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

#ifndef _URUBU_IPC_H_
#define _URUBU_IPC_H_

#include <stdint.h>
#include <stddef.h>

// TODO: remove this when PAGE_SIZE is removed from mm.h
#include <urubu/mm.h>

#ifdef __cplusplus
extern "C"
{
#endif

// possible IPC message identifiers
enum
{
    // device operations
    MSG_DEVICE_ANNOUNCE = 1000,
    MSG_DEVICE_LOOKUP,
    MSG_DEVICE_OPEN,
    MSG_DEVICE_CLOSE,
    MSG_DEVICE_READ,
    MSG_DEVICE_WRITE,

    // terminal server messages
    MSG_TERMINAL_CREATE
};

struct ipc_message
{
    uint64_t data[6];
} __attribute__((packed));

int ipc_port_create();
void ipc_port_delete(int port);

int ipc_port_send(int port, struct ipc_message* msg);
int ipc_port_receive(int port, struct ipc_message* msg);

int ipc_shmem_create(size_t size, void** base);
int ipc_shmem_accept(int id, void** base, size_t* size);
void ipc_shmem_close(int id);

int ipc_server_register(const char* name, int port);
int ipc_server_lookup(const char* name, int wait);

#ifdef __cplusplus
}
#endif

#endif
