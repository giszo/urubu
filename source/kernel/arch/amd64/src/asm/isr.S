/* amd64 interrupt service routines.
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

#define ISR(num, handler) \
    .type isr##num, @function; \
    .global isr##num; \
    isr##num:; \
    pushq $0; \
    pushq $##num; \
    pushq %rax; \
    pushq %rbx; \
    pushq %rcx; \
    pushq %rdx; \
    pushq %rsi; \
    pushq %rdi; \
    pushq %rbp; \
    pushq %r8; \
    pushq %r9; \
    pushq %r10; \
    pushq %r11; \
    pushq %r12; \
    pushq %r13; \
    pushq %r14; \
    pushq %r15; \
    movw $DS_KERNEL, %ax; \
    movw %ax, %ds; \
    movw %ax, %es; \
    movw %ax, %fs; \
    movq %rsp, %rdi; \
    call handler; \
    popq %r15; \
    popq %r14; \
    popq %r13; \
    popq %r12; \
    popq %r11; \
    popq %r10; \
    popq %r9; \
    popq %r8; \
    popq %rbp; \
    popq %rdi; \
    popq %rsi; \
    popq %rdx; \
    popq %rcx; \
    popq %rbx; \
    popq %rax; \
    addq $16, %rsp; \
    iretq

#define ISR_ERRCODE(num, handler) \
    .type isr##num, @function; \
    .global isr##num; \
    isr##num:; \
    pushq $##num; \
    pushq %rax; \
    pushq %rbx; \
    pushq %rcx; \
    pushq %rdx; \
    pushq %rsi; \
    pushq %rdi; \
    pushq %rbp; \
    pushq %r8; \
    pushq %r9; \
    pushq %r10; \
    pushq %r11; \
    pushq %r12; \
    pushq %r13; \
    pushq %r14; \
    pushq %r15; \
    movw $DS_KERNEL, %ax; \
    movw %ax, %ds; \
    movw %ax, %es; \
    movw %ax, %fs; \
    movq %rsp, %rdi; \
    call handler; \
    popq %r15; \
    popq %r14; \
    popq %r13; \
    popq %r12; \
    popq %r11; \
    popq %r10; \
    popq %r9; \
    popq %r8; \
    popq %rbp; \
    popq %rdi; \
    popq %rsi; \
    popq %rdx; \
    popq %rcx; \
    popq %rbx; \
    popq %rax; \
    addq $16, %rsp; \
    iretq

// Exception handlers
ISR(0, isr_division_by_zero)
ISR(3, isr_debug_exception)
ISR(6, isr_invalid_opcode)
ISR(7, isr_device_not_available)
ISR_ERRCODE(13, isr_general_protection)
ISR_ERRCODE(14, isr_page_fault)
ISR(16, isr_fpu_exception)
ISR(19, isr_sse_exception)

// HW interrupt handlers
ISR(32, arch_irq_handler)
ISR(33, arch_irq_handler)
ISR(34, arch_irq_handler)
ISR(35, arch_irq_handler)
ISR(36, arch_irq_handler)
ISR(37, arch_irq_handler)
ISR(38, arch_irq_handler)
ISR(39, arch_irq_handler)
ISR(40, arch_irq_handler)
ISR(41, arch_irq_handler)
ISR(42, arch_irq_handler)
ISR(43, arch_irq_handler)
ISR(44, arch_irq_handler)
ISR(45, arch_irq_handler)
ISR(46, arch_irq_handler)
ISR(47, arch_irq_handler)

// system call handler
ISR(128, arch_syscall_handler)
