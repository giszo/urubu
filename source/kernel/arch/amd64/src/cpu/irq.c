/* amd64 specific IRQ handling.
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

#include <pc/pic.h>

#include <kernel/console.h>
#include <arch/io.h>

// =====================================================================================================================
void arch_irq_handler(struct irq_context* ctx)
{
    unsigned irq = ctx->int_num - 0x20 /* pic is remapped to starts HW interrupts at 0x20 */;
    int spurious = pic_is_masked(irq) && !pic_is_irq_real(irq);

    if (spurious)
    {
	kprintf("irq: received spurious IRQ: %d\n", irq);
	return;
    }

    irq_handle(irq, ctx);
}

// =====================================================================================================================
void irq_arch_enable(int irq)
{
    pic_enable_irq(irq);
}

// =====================================================================================================================
void irq_arch_disable(int irq)
{
    pic_disable_irq(irq);
}

// =====================================================================================================================
void irq_arch_finished(int irq)
{
    pic_send_eoi(irq);
}
