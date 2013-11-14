/* IRQ handling.
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
#include <kernel/cpu/irq.h>
#include <kernel/cpu/cpu.h>
#include <kernel/ipc/port.h>

#include <arch/cpu/irq.h>

static struct irq_slot s_irq_table[ARCH_IRQ_COUNT];

// =====================================================================================================================
int irq_handle(int irq, struct irq_context* ctx)
{
    if ((irq < 0) || (irq >= ARCH_IRQ_COUNT))
	panic("irq: invalid IRQ fired: %d!\n", irq);

    struct irq_slot* is = &s_irq_table[irq];

    switch (is->target)
    {
	case KERNEL :
	    is->kernel.handler(irq, is->kernel.data, ctx);
	    irq_arch_finished(irq);
	    break;

	case SERVER :
	{
	    struct ipc_user_msg msg;
	    msg.data[0] = 1; // TODO: irq message ID
	    msg.data[1] = irq;

	    if (ipc_port_send(is->server.port, &msg, 0 /* kernel */) != 0)
		dprintf("irq: unable to send message to the requested port");

	    break;
	}

	case NONE :
	    panic("irq: there is no handler for %d\n", irq);
    }

    return 0;
}

// =====================================================================================================================
int irq_register(int irq, irq_handler* handler, void* data)
{
    // Make sure IRQ number is valid.
    if ((irq < 0) || (irq >= ARCH_IRQ_COUNT))
	return -1;

    // disable interrupts to avoid receiving one while working on the IRQ table ...
    int ints = cpu_arch_disable_int();

    int ret = 0;
    struct irq_slot* is = &s_irq_table[irq];

    // Check whether this IRQ slot is available.
    if (is->target == NONE)
    {
	// Register the new handler.
	is->target = KERNEL;
	is->kernel.handler = handler;
	is->kernel.data = data;

	// Enable the IRQ.
	irq_arch_enable(irq);
    }
    else
	ret = -1;

    if (ints)
	cpu_arch_enable_int();

    return ret;
}

// =====================================================================================================================
int sys_irq_register(int irq, int port)
{
    // Make sure IRQ number is valid.
    if ((irq < 0) || (irq >= ARCH_IRQ_COUNT))
	return -1;

    dprintf("irq: registering port %d for irq %d\n", port, irq);

    // disable interrupts to avoid receiving one while working on the IRQ table ...
    int ints = cpu_arch_disable_int();

    int ret = 0;
    struct irq_slot* is = &s_irq_table[irq];

    // Check whether this IRQ slot is available.
    if (is->target == NONE)
    {
	// Register the new handler.
	is->target = SERVER;
	is->server.port = port;

	// Enable the IRQ.
	irq_arch_enable(irq);
    }
    else
	ret = -1;

    if (ints)
	cpu_arch_enable_int();

    return ret;
}

// =====================================================================================================================
int sys_irq_finished(int irq)
{
    irq_arch_finished(irq);
    return 0;
}

// =====================================================================================================================
void irq_init()
{
    for (size_t i = 0; i < ARCH_IRQ_COUNT; ++i)
        s_irq_table[i].target = NONE;
}
