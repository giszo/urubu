/* Messages for inter process communication.
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

#ifndef _KERNEL_IPC_MESSAGE_H_
#define _KERNEL_IPC_MESSAGE_H_

#include <kernel/types.h>
#include <kernel/macros.h>

// IPC message structure user by userspace applications
struct ipc_user_msg
{
    uint64_t data[6];
} _packed;

struct ipc_message
{
    // data of the message
    struct ipc_user_msg data;

    // link to the next message in the queue
    struct ipc_message* next;
};

struct ipc_message* ipc_message_create();
void ipc_message_destroy(struct ipc_message* m);

void ipc_message_init();

#endif
