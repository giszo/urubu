# System call table generator.
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

import re
import sys

MASK = re.compile('.*\{\"(.+)\"\,.+\}.*')

def generate_table(target, source) :
    f = open(source, "r")

    header = open(target, "w")
    header.write("/* This file is generated automatically, DO NOT modify it! */\n\n")
    header.write("enum\n{\n")

    idx = 0
    parsing = False

    for line in f.readlines() :
        if line.find("s_syscall_table[]") != -1 :
            parsing = True
        elif line.find("};") != -1 :
            parsing = False

        if parsing :
            m = MASK.match(line)
            if not m :
                continue
            header.write("    SYS_%s,\n" % m.groups()[0])

    header.write("};\n")
    header.close()

    f.close()
