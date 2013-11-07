/* Thread handling.
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

#ifndef _KERNEL_PROC_THREAD_H_
#define _KERNEL_PROC_THREAD_H_

#include <kernel/mm/slab.h>
#include <kernel/lib/hashtable.h>

#include <arch/mm/config.h>

#define THREAD_NAME_SIZE 32

struct process;

enum thread_state
{
    CREATED,
    SLEEPING,
    READY,
    RUNNING,
    DEAD
};

struct thread
{
    struct hashitem* _item;

    int id;
    char name[THREAD_NAME_SIZE];

    // the owner process of this thread
    struct process* proc;
    enum thread_state state;

    void* kernel_stack;
    size_t kernel_stack_size;
    void* user_stack;
    size_t user_stack_size;

    struct thread* sched_next;

    void* arch_data;
};

struct thread_statistics
{
    // number of threads in the system
    uint64_t number;
};

void thread_arch_init();
int thread_arch_create(struct thread* t);
void thread_arch_destroy(struct thread* t);
int thread_arch_create_kernel(struct thread* t, void* entry, void* arg);
int thread_arch_create_user(struct thread* t, void* entry, void* arg);

// Terminates the execution of the current thread
void thread_exit();

struct thread* thread_create_kernel(const char* name, void* entry, void* arg);
struct thread* thread_create_user(struct process* p, const char* name, void* entry, void* arg, void* stack, size_t stack_size);

struct thread* thread_current();

int thread_sleep();
int thread_wake_up(struct thread* t);

long sys_thread_exit(int code);
long sys_thread_get_statistics(struct thread_statistics* stat);

void thread_init();

#endif
