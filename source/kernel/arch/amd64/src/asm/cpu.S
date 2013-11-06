/* amd64 specific low level CPU functions.
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

#include <arch/cpu/cpu.h>

.global cpu_set_cr3
.global cpu_flush_cr3
.global cpu_arch_idle
.global cpu_arch_enable_int
.global cpu_arch_disable_int

.section .text

.type cpu_set_cr3, @function
cpu_set_cr3:
    movq %rdi, %cr3
    ret
.size cpu_set_cr3,.-cpu_set_cr3

.type cpu_flush_cr3, @function
cpu_flush_cr3:
    movq %cr3, %rax
    movq %rax, %cr3
    ret
.size cpu_flush_cr3,.-cpu_flush_cr3

.type cpu_arch_idle, @function
cpu_arch_idle:
    hlt
    ret
.size cpu_arch_idle,.-cpu_arch_idle

.type cpu_arch_enable_int, @function
cpu_arch_enable_int:
    sti
    retq
.size cpu_arch_enable_int,.-cpu_arch_enable_int

.type cpu_arch_disable_int, @function
cpu_arch_disable_int:
    pushfq
    popq %rax
    cli
    andl $EFLAG_IF, %eax
    retq
.size cpu_arch_disable_int,.-cpu_arch_disable_int
