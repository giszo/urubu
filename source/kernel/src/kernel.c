/* Entry point of the kernel.
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

#include <kernel/cpu/irq.h>
#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>
#include <kernel/proc/sched.h>
#include <kernel/ipc/port.h>
#include <kernel/ipc/message.h>

#include <kernel/console.h>
void t1() { while (1) kprintf("1"); }
void t2() { while (1) kprintf("2"); }

// =====================================================================================================================
void kernel_start()
{
    vmm_init();
    irq_init();
    process_init();
    thread_init();
    sched_init();
    ipc_port_init();
    ipc_message_init();

#if 0
    thread* t = thread_create_kernel("t1", (void*)t1, NULL);
    sched_add_ready(t);
    t = thread_create_kernel("t2", (void*)t2, NULL);
    sched_add_ready(t);
#endif
}
