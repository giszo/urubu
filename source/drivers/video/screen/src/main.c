/* Text screen driver.
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

#include <libdevman/device.h>

#include <urubu/ipc.h>
#include <urubu/debug.h>

// =====================================================================================================================
int main(int argc, char** argv)
{
    int p = ipc_port_create();

    if (p < 0)
    {
	dbprintf("screen: unable to create IPC port\n");
	return -1;
    }

    // announce the new driver
    device_announce(SCREEN, p);

    while (1)
    {
	struct ipc_message msg;

	if (ipc_port_receive(p, &msg) != 0)
	{
	    dbprintf("screen: unable to receive IPC message\n");
	    break;
	}
    }

    return 0;
}
