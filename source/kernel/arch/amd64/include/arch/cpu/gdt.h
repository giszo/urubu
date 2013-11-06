/* amd64 GDT definitions.
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

#ifndef _AMD64_CPU_GDT_H_
#define _AMD64_CPU_GDT_H_

#define CS_KERNEL 0x08
#define DS_KERNEL 0x10
#define CS_USER   0x18
#define DS_USER   0x20

#define GDT_DESC_SIZE 8

#ifndef __ASSEMBLER__

#include <kernel/types.h>
#include <kernel/macros.h>

#include <arch/cpu/tss.h>

struct code_segment
{
    unsigned limit0 : 16;
    unsigned base0 : 16;
    unsigned base1 : 8;
    unsigned accessed : 1;
    unsigned readable : 1;
    unsigned c : 1;
    unsigned always1 : 2;
    unsigned dpl : 2;
    unsigned present : 1;
    unsigned limit1 : 4;
    unsigned available : 1;
    unsigned longmode : 1;
    unsigned d : 1;
    unsigned granularity : 1;
    unsigned base2 : 8;
} _packed;

struct data_segment
{
    unsigned limit0 : 16;
    unsigned base0 : 16;
    unsigned base1 : 8;
    unsigned accessed : 1;
    unsigned writable : 1;
    unsigned expand_down : 1;
    unsigned always0 : 1;
    unsigned always1 : 1;
    unsigned dpl : 2;
    unsigned present : 1;
    unsigned limit1 : 4;
    unsigned available : 1;
    unsigned unused1 : 1;
    unsigned d : 1;
    unsigned granularity : 1;
    unsigned base2 : 8;
} _packed;

struct tss_segment
{
    unsigned limit0 : 16;
    unsigned base0 : 16;
    unsigned base1 : 8;
    unsigned type : 4;
    unsigned unused0 : 1;
    unsigned dpl : 2;
    unsigned present : 1;
    unsigned limit1 : 4;
    unsigned available : 1;
    unsigned unused1 : 2;
    unsigned granularity : 1;
    unsigned base2 : 8;
    unsigned base3 : 32;
    unsigned reserved1 : 8;
    unsigned unused2 : 5;
    unsigned reserved2 : 19;
} _packed;

struct gdt_info
{
    uint16_t size;
    uint64_t address;
} _packed;

void gdt_arch_set_tss(struct amd64_tss* tss);

void gdt_arch_reload_segments();

void gdt_init();

#endif /* __ASSEMBLER__ */

#endif
