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

env = Environment()

# currently these are the only supported values so it is fine to keep them here
env["ARCH"] = "amd64"
env["PLATFORM"] = "pc"

# setup our own build environment
import environment
environment.setup(env)

env["CFLAGS"] = ["-O2", "-Wall", "-std=gnu99"]
env["CXXFLAGS"] = ["-O2", "-Wall", "-std=c++11", "-fno-exceptions", "-fno-rtti"]
env["CPPPATH"] = []

# scan all of the SConscript files ...
SConscript(dirs = ["source"], exports = ["env"])

# build the bootable CD image
iso = env.ISO("urubu-amd64.iso", None)
Default(iso)
