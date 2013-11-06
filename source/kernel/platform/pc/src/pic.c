/* PIC handling.
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

#include <kernel/types.h>
#include <kernel/kernel.h>
#include <kernel/console.h>

#include <arch/io.h>

#include <pc/pic.h>

#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_IMR 0x21
#define PIC_SLAVE_CMD  0xa0
#define PIC_SLAVE_IMR  0xa1
#define PIC_CASCADE_IR 0x02

#define PIC_MASTER_ELCR 0x4d0
#define PIC_SLAVE_ELCR 0x4d1

#define MASTER_ICW4_DEFAULT 0x01
#define SLAVE_ICW4_DEFAULT  0x01

static uint8_t s_master_mask = 0xff;
static uint8_t s_slave_mask = 0xff;

// =====================================================================================================================
int pic_is_masked(unsigned irq)
{
    if (irq & 8)
	return s_slave_mask & (1 << (irq - 8));
    else
	return s_master_mask & (1 << irq);
}

// =====================================================================================================================
int pic_is_irq_real(unsigned irq)
{
    unsigned isr;
    unsigned irr;

    if (irq & 8)
    {
	outb(0x0b, PIC_SLAVE_CMD);
	isr = inb(PIC_SLAVE_CMD);
	outb(0x0a, PIC_SLAVE_CMD);
	irr = inb(PIC_SLAVE_CMD);
    }
    else
    {
	outb(0x0b, PIC_MASTER_CMD);
	isr = inb(PIC_MASTER_CMD);
	outb(0x0a, PIC_MASTER_CMD);
	irr = inb(PIC_MASTER_CMD);
    }

    int real = isr & (1 << (irq & 7));

    if (!real)
	kprintf("pic: IRQ %d is not real! (ISR=%x IRR=%x)\n", irq, isr, irr);

    return real;
}

// =====================================================================================================================
void pic_enable_irq(unsigned irq)
{
    if (irq & 8)
    {
	s_slave_mask &= ~(1 << (irq - 8));
	outb(s_slave_mask, PIC_SLAVE_IMR);
    }
    else
    {
	s_master_mask &= ~(1 << irq);
	outb(s_master_mask, PIC_MASTER_IMR);
    }
}

// =====================================================================================================================
void pic_disable_irq(unsigned irq)
{
    if (irq & 8)
    {
	s_slave_mask |= (1 << (irq - 8));
	outb(s_slave_mask, PIC_SLAVE_IMR);
    }
    else
    {
	s_master_mask |= (1 << irq);
	outb(s_master_mask, PIC_MASTER_IMR);
    }
}

// =====================================================================================================================
void pic_send_eoi(unsigned irq)
{
    if (irq & 8)
	outb(0x20, PIC_SLAVE_CMD);

    outb(0x20, PIC_MASTER_CMD);
}

// =====================================================================================================================
void pic_remap()
{
    // mask everything
    outb(0xff, PIC_SLAVE_IMR);
    outb(0xff, PIC_MASTER_IMR);

    // select 8259A-1 init
    outb(0x11, PIC_MASTER_CMD);
    // irq 0-7 is mapped to 0x20-0x27
    outb(0x20, PIC_MASTER_IMR);
    // set the cascade irq
    outb(1 << PIC_CASCADE_IR, PIC_MASTER_IMR);
    outb(MASTER_ICW4_DEFAULT, PIC_MASTER_IMR);

    // select 8259A-2 init
    outb(0x11, PIC_SLAVE_CMD);
    // irq 8-15 is mapped to 0x28-0x2f
    outb(0x28, PIC_SLAVE_IMR);
    // set the cascade irq
    outb(PIC_CASCADE_IR, PIC_SLAVE_IMR);
    outb(SLAVE_ICW4_DEFAULT, PIC_SLAVE_IMR);

    // enable IRQ 2
    s_master_mask &= ~(1 << 2);

    outb(s_slave_mask, PIC_SLAVE_IMR);
    outb(s_master_mask, PIC_MASTER_IMR);
}
