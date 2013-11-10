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

#include <urubu/ipc.h>

#include <syscall.h>

// =====================================================================================================================
int ipc_port_create()
{
    return syscall0(SYS_ipc_port_create);
}

// =====================================================================================================================
void ipc_port_delete(int port)
{
    syscall1(SYS_ipc_port_delete, port);
}

// =====================================================================================================================
int ipc_port_send(int port, struct ipc_message* msg)
{
    return syscall2(SYS_ipc_port_send, port, (unsigned long)msg);
}

// =====================================================================================================================
int ipc_port_receive(int port, struct ipc_message* msg)
{
    int r = syscall2(SYS_ipc_port_receive, port, (unsigned long)msg);
    memory_barrier();
    return r;
}

// =====================================================================================================================
int ipc_shmem_create(size_t size, void** base)
{
    int result = syscall2(SYS_ipc_shmem_create, size, (unsigned long)base);
    memory_barrier();
    return result;
}

// =====================================================================================================================
int ipc_shmem_accept(int id, void** base, size_t* size)
{
    int result = syscall3(SYS_ipc_shmem_accept, id, (unsigned long)base, (unsigned long)size);
    memory_barrier();
    return result;
}

// =====================================================================================================================
void ipc_shmem_close(int id)
{
    syscall1(SYS_ipc_shmem_close, id);
}

// =====================================================================================================================
int ipc_server_register(const char* name, int port)
{
    return syscall2(SYS_ipc_server_register, (unsigned long)name, port);
}

// =====================================================================================================================
int ipc_server_lookup(const char* name, int wait)
{
    return syscall2(SYS_ipc_server_lookup, (unsigned long)name, wait);
}
