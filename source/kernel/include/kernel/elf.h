/* Generic ELF binary loader.
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

#ifndef _KERNEL_ELF_H_
#define _KERNEL_ELF_H_

#include <kernel/types.h>
#include <kernel/proc/process.h>

enum
{
    ID_MAGIC0 = 0,
    ID_MAGIC1,
    ID_MAGIC2,
    ID_MAGIC3,
    ID_CLASS,
    ID_DATA,
    ID_VERSION,
    ID_SIZE = 16
};

enum elf_shdr_flag
{
    ELF_SHF_WRITE = 1,
    ELF_SHF_ALLOC = 2
};

enum elf_shdr_type
{
    ELF_SHT_PROGBITS = 1,
    ELF_SHT_NOBITS = 8
};

// include architecture specific elf definitions
#include <arch/elf.h>

ptr_t elf_load(struct process* p, uint8_t* module);

#endif
