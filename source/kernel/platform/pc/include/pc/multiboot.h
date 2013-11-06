/* Multiboot definitions.
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

#ifndef _PC_MULTIBOOT_H_
#define _PC_MULTIBOOT_H_

#define MB_HEADER_MAGIC 0x1badb002
#define MB_LOADER_MAGIC 0x2badb002

#define MB_FLAG_ALIGN_MODULES (1 << 0)
#define MB_FLAG_MEMORY_INFO   (1 << 1)
#define MB_FLAG_AOUT_KLUDGE   (1 << 16)

#ifndef __ASSEMBLER__

#include <kernel/types.h>
#include <kernel/macros.h>

struct mb_mmap_info
{
    uint32_t length;
    uint32_t addr;
} _packed;

struct mb_mod_info
{
    uint32_t length;
    uint32_t addr;
} _packed;

struct mb_info
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_dev;
    uint32_t kernel_params;
    struct mb_mod_info mod_info;
    uint32_t sym_info[4];
    struct mb_mmap_info mmap_info;
} _packed;

struct mb_mmap_entry
{
    uint32_t size;
    uint64_t base;
    uint64_t length;
    uint32_t type;
} _packed;

struct mb_mod_entry
{
    uint32_t start;
    uint32_t end;
    uint32_t params;
    uint32_t reserved;
} _packed;

enum mb_mmap_type
{
    MEM_AVAILABLE = 1,
    MEM_RESERVED = 2
};

#endif /* __ASSEMBLER__ */

#endif
