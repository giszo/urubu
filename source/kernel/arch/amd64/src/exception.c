/* Generic x86 exception handlers.
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

#include <kernel/kernel.h>
#include <kernel/console.h>
#include <kernel/types.h>
#include <kernel/cpu/cpu.h>

#include <arch/cpu/cpu.h>

#define UNHANDLED_EXCEPTION \
    cpu_arch_disable_int(); \
    kprintf_unlocked("CS:EIP=%x:%p\n", ctx->cs, ctx->ip); \
    panic("%s: unhandled exception!\n", __FUNCTION__)

// =====================================================================================================================
void isr_division_by_zero(struct irq_context* ctx)
{
    UNHANDLED_EXCEPTION;
}

// =====================================================================================================================
void isr_debug_exception(struct irq_context* ctx)
{
    UNHANDLED_EXCEPTION;
}

// =====================================================================================================================
void isr_invalid_opcode(struct irq_context* ctx)
{
    UNHANDLED_EXCEPTION;
}

// =====================================================================================================================
void isr_device_not_available(struct irq_context* ctx)
{
    UNHANDLED_EXCEPTION;
}

// =====================================================================================================================
void isr_general_protection(struct irq_context* ctx)
{
    UNHANDLED_EXCEPTION;
}

// =====================================================================================================================
void isr_fpu_exception(struct irq_context* ctx)
{
    UNHANDLED_EXCEPTION;
}

// =====================================================================================================================
void isr_sse_exception(struct irq_context* ctx)
{
    UNHANDLED_EXCEPTION;
}

// =====================================================================================================================
void isr_page_fault(struct irq_context* ctx)
{
    UNHANDLED_EXCEPTION;
}
