/* amd64 assembler part of the scheduler.
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

#include <arch/cpu/gdt.h>

.section .text

.global sched_arch_switch_to
.global sched_yield

.type sched_arch_switch_to, @function
sched_arch_switch_to:
    movq %rdi, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    addq $16, %rsp // skip int_num and err_code
    iretq
.size sched_arch_switch_to,.-sched_arch_switch_to

.type sched_yield, @function
sched_yield:
    // get the return address and save in rax
    movq 0(%rsp), %rax
    // remove the return address from the stack
    addq $8, %rsp
    // save rsp in rcx, we will use it later on to set the rsp value in the IRQ context
    mov %rsp, %rcx
    // start building an IRQ context on the stack
    pushq $DS_KERNEL
    pushq %rcx // saved stack pointer, see above
    pushfq
    pushq $CS_KERNEL
    pushq %rax
    pushq $0
    pushq $0
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    // set the parameters of sched_irq
    // the first two parameters (irq and data) are not used, so we don't waste our time setting them here ...
    movq %rsp, %rdx
    // enter to the scheduler
    call sched_irq
    // sched_irq should not return ...
.size sched_yield,.-sched_yield
