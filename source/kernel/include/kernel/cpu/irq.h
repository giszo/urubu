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

#ifndef _KERNEL_CPU_IRQ_H_
#define _KERNEL_CPU_IRQ_H_

#include <kernel/types.h>

typedef int irq_handler(int irq, void* data, struct irq_context* ctx);

enum irq_target
{
    NONE,
    KERNEL,
    SERVER
};

struct irq_slot
{
    enum irq_target target;

    union
    {
	struct
	{
	    irq_handler* handler;
	    void* data;
	} kernel;

	struct
	{
	    int port;
	} server;
    };
};

void irq_arch_enable(int irq);
void irq_arch_disable(int irq);
void irq_arch_finished(int irq);

int irq_handle(int irq, struct irq_context* ctx);

int irq_register(int irq, irq_handler* handler, void* data);

int sys_irq_register(int irq, int port);
int sys_irq_finished(int irq);

void irq_init();

#endif
