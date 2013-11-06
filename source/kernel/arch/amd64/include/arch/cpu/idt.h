/* amd64 IDT definitions.
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

#ifndef _AMD64_CPU_IDT_H_
#define _AMD64_CPU_IDT_H_

#include <kernel/types.h>
#include <kernel/macros.h>

#define IDT_ENTRIES 256

#define TRAP_GATE 0xf
#define INTR_GATE 0xe

struct idt_descriptor
{
    unsigned base_low : 16;
    unsigned selector : 16;
    unsigned ist : 3;
    unsigned always0 : 5;
    unsigned type : 4;
    unsigned always0_2 : 1;
    unsigned dpl : 2;
    unsigned present : 1;
    unsigned base_mid : 16;
    unsigned base_high : 32;
    unsigned reserved : 32;
} _packed;

struct idt_info
{
    uint16_t size;
    uint64_t address;
} _packed;

void idt_init();

#endif
