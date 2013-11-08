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
int ipc_port_receive(int port, struct ipc_message* msg)
{
    int r = syscall2(SYS_ipc_port_receive, port, (unsigned long)msg);
    memory_barrier();
    return r;
}

// =====================================================================================================================
int ipc_port_send_broadcast(unsigned broadcast, struct ipc_message* msg)
{
    return syscall2(SYS_ipc_port_send_broadcast, broadcast, (unsigned long)msg);
}

// =====================================================================================================================
int ipc_port_set_broadcast_mask(int port, unsigned mask)
{
    return syscall2(SYS_ipc_port_set_broadcast_mask, port, mask);
}
