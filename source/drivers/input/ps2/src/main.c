/* PS/2 keyboard & mouse driver.
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

#include <inttypes.h>

#include <urubu/ipc.h>
#include <urubu/irq.h>
#include <urubu/debug.h>

#include <arch/io.h>

#define PS2_IRQ 1

// =====================================================================================================================
int main(int argc, char** argv)
{
    // create a port to be able to receive messages on
    int port = ipc_port_create();

    if (port < 0)
    {
	dbprintf("ps2: unable to create port!\n");
	return -1;
    }

    // register the keyboard interrupt
    if (irq_register(PS2_IRQ, port) != 0)
    {
	dbprintf("ps2: unable to register interrupt handler!\n");
	return -1;
    }

    while (1)
    {
	struct ipc_message msg;

	int r = ipc_port_receive(port, &msg);

	if (r < 0)
	    break;

	// read the scancode from the keyboard controller
	unsigned c = inb(0x60);
	dbprintf("c=%u\n", c);

	// finish the handling of the interrupt
	irq_finished(msg.data[1] /* the IRQ number parameter */);
    }

    return 0;
}
