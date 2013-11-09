/* Shared memory for inter process communication.
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

#ifndef _KERNEL_IPC_SHMEM_H_
#define _KERNEL_IPC_SHMEM_H_

#include <kernel/types.h>
#include <kernel/lib/hashtable.h>

#define SHMEM_PAGES_PER_BLOCK 10

struct shmem_block
{
    // the number of pages used in this block
    unsigned used;
    ptr_t pages[SHMEM_PAGES_PER_BLOCK];

    struct shmem_block* next;
};

struct shmem
{
    struct hashitem _item;

    int id;

    // the process that created the shared memory
    struct process* owner;
    // the process that accepted the shared memory
    struct process* user;

    size_t size;
    struct shmem_block* blocks;
};

long sys_ipc_shmem_create(size_t size, void** base);
long sys_ipc_shmem_accept(int id, void** base, size_t* size);
long sys_ipc_shmem_close(int id);

void ipc_shmem_init();

#endif
