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

from SCons.Builder import Builder
from SCons.Action import Action

import isobuilder

def _get_toolchain_path(env) :
    return "toolchain/%s" % env["ARCH"]

def setup(env) :
    env["LIBRARIES"] = []
    env["DRIVERS"] = []
    env["SERVERS"] = []
    env["SYS_APPS"] = []

    # setup compiler executables
    # TODO: the x86_64 part is still hardcoded ...
    toolchain = _get_toolchain_path(env)
    env["CC"] = "%s/bin/x86_64-pc-urubu-gcc" % toolchain
    env["CXX"] = "%s/bin/x86_64-pc-urubu-g++" % toolchain
    env["LD"] = "%s/bin/x86_64-pc-urubu-ld" % toolchain

    # customize the generated output of builders
    env["ASPPCOMSTR"]   = " [AS]  $SOURCE"
    env["CCCOMSTR"]     = " [CC]  $SOURCE"
    env["CXXCOMSTR"]    = " [C++] $SOURCE"
    env["LINKCOMSTR"]   = " [LD]  $TARGET"
    env["ARCOMSTR"]     = " [AR]  $TARGET"
    env["RANLIBCOMSTR"] = " [RL]  $TARGET"
    env["CPCOMSTR"]     = " [CP]  $SOURCE -> $TARGET"
    env["SCGENCOMSTR"]  = " [GEN] $TARGET"
    env["ISOCOMSTR"]    = " [ISO] $TARGET"

    # insall custom builders
    env["BUILDERS"]["ISO"] = Builder(
        action = Action(isobuilder._iso_builder, env["ISOCOMSTR"]),
        emitter = isobuilder._iso_emitter
    )
