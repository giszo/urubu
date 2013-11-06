/* Process handling.
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

#ifndef _KERNEL_PROC_PROCESS_H_
#define _KERNEL_PROC_PROCESS_H_

#include <kernel/mm/vmm.h>
#include <kernel/mm/slab.h>
#include <kernel/lib/hashtable.h>

#define PROC_NAME_SIZE 32

struct process
{
    struct hashitem* _item;

    int id;
    char name[PROC_NAME_SIZE];

    void* arch_data;
};

void process_arch_init();
int process_arch_create(struct process* p);

struct process* process_create(const char* name);

void process_init();

#endif
