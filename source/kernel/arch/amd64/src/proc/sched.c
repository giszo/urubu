/* Scheduler.
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

#include <kernel/cpu/cpu.h>
#include <kernel/cpu/irq.h>
#include <kernel/proc/sched.h>

#include <arch/cpu/cpu.h>
#include <arch/proc/thread.h>
#include <arch/proc/sched.h>
#include <arch/proc/process.h>

#include <pc/pic.h>

// =====================================================================================================================
int sched_irq(int irq, void* data, struct irq_context* ctx)
{
    // We need to send EOI on the PIC here because this function will not return to the common IRQ handler path ...
    pic_send_eoi(0);

    struct cpu* curr_cpu = cpu_get_current();
    struct thread* curr = curr_cpu->thread_current;
    struct amd64_thread* t_arch;

    // Save the kernel stack pointer of the previous thread.
    if (curr)
    {
        t_arch = (struct amd64_thread*)curr->arch_data;
        t_arch->rsp = (uint64_t)ctx;
    }

    // Select the next thread to run.
    struct thread* next = sched_next(curr_cpu, curr);

    // upadte the TSS structure of the current CPU for the new thread
    struct amd64_cpu* arch_cpu = (struct amd64_cpu*)curr_cpu->arch_data;
    arch_cpu->tss.rsp0 = (ptr_t)next->kernel_stack + next->kernel_stack_size;

    // switch to the address space of the new process
    if (next->proc)
    {
	struct amd64_process* proc_arch = (struct amd64_process*)next->proc->arch_data;
	cpu_set_cr3(proc_arch->cr3);
    }

    // Switch to the next thread.
    t_arch = (struct amd64_thread*)next->arch_data;
    sched_arch_switch_to(t_arch->rsp);

    return 0;
}

// =====================================================================================================================
void sched_arch_init()
{
    irq_register(0, sched_irq, NULL);
}
