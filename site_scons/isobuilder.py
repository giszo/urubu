# Urubu build system
#
# Copyright (c) 2013 Zoltan Kovacs
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import os
import tempfile
import shutil

def _write_grub_config(d, drivers, servers) :
    f = open(os.path.join(d, "menu.lst"), "w")
    f.write("title urubu\n")
    f.write("kernel /system/kernel\n")
    for server in servers :
        f.write("module /system/servers/%s\n" % server[server.rfind('/')+1:])
    for driver in drivers :
        f.write("module /system/drivers/%s\n" % driver[driver.rfind('/')+1:])
    f.close()

def _iso_builder(target, source, env) :
    # Create a temporary directory to build the image tree
    d = tempfile.mkdtemp()

    # Copy the kernel image
    system = os.path.join(d, "system")
    os.mkdir(system)
    shutil.copy(str(env["KERNEL"][0]), system)

    # Copy drivers to the image
    drivers = os.path.join(d, "system", "drivers")
    os.makedirs(drivers)
    for driver in env["DRIVERS"] :
	shutil.copy(str(driver[0]), drivers)

    # Copy servers to the image
    servers = os.path.join(d, "system", "servers")
    os.makedirs(servers)
    for server in env["SERVERS"] :
	shutil.copy(str(server[0]), servers)

    # Copy GRUB to the image and create the menu.lst file
    grub = os.path.join(d, "boot", "grub")
    os.makedirs(grub)
    _write_grub_config(
        grub,
        [str(driver[0]) for driver in env["DRIVERS"]],
        [str(server[0]) for server in env["SERVERS"]]
    )
    shutil.copy("utilities/grub/stage2_eltorito", grub)

    # Create the iso image
    os.system("mkisofs -R -l -b boot/grub/stage2_eltorito -boot-load-size 4 -boot-info-table -no-emul-boot -o %s %s > /dev/null 2>&1" % (target[0], d))

    # Clean up our mess
    shutil.rmtree(d)

    return 0

def _iso_emitter(target, source, env) :
    return target, source + [env["KERNEL"]] + env["LIBRARIES"] + env["DRIVERS"] + env["SERVERS"]
