/* amd64 IDT handling.
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

#include <kernel/lib/string.h>

#include <arch/cpu/gdt.h>
#include <arch/cpu/idt.h>

#include <pc/pic.h>

static struct idt_descriptor s_idt[IDT_ENTRIES] _aligned(16);

void isr0();
void isr3();
void isr6();
void isr7();
void isr13();
void isr14();
void isr16();
void isr19();
void isr32();
void isr33();
void isr34();
void isr35();
void isr36();
void isr37();
void isr38();
void isr39();
void isr40();
void isr41();
void isr42();
void isr43();
void isr44();
void isr45();
void isr46();
void isr47();
void isr128();

// =====================================================================================================================
static inline void set_trap_gate(unsigned num, void* handler, unsigned dpl)
{
    uint64_t addr;
    struct idt_descriptor* desc;

    desc = &s_idt[num];
    addr = (uint64_t)handler;

    desc->base_low = addr & 0xffff;
    desc->selector = CS_KERNEL;
    desc->type = TRAP_GATE;
    desc->dpl = dpl;
    desc->present = 1;
    desc->base_mid = (addr >> 16) & 0xffff;
    desc->base_high = addr >> 32;
}

// =====================================================================================================================
static inline void set_intr_gate(unsigned num, void* handler, unsigned dpl)
{
    uint64_t addr;
    struct idt_descriptor* desc;

    desc = &s_idt[num];
    addr = (uint64_t)handler;

    desc->base_low = addr & 0xffff;
    desc->selector = CS_KERNEL;
    desc->type = INTR_GATE;
    desc->dpl = dpl;
    desc->present = 1;
    desc->base_mid = (addr >> 16) & 0xffff;
    desc->base_high = addr >> 32;
}

// =====================================================================================================================
void idt_init()
{
    struct idt_info info;

    memset(s_idt, 0, sizeof(s_idt));

    set_trap_gate(0, isr0, 0);
    set_trap_gate(3, isr3, 0);
    set_trap_gate(6, isr6, 0);
    set_trap_gate(7, isr7, 0);
    set_trap_gate(13, isr13, 0);
    set_trap_gate(14, isr14, 0);
    set_trap_gate(16, isr16, 0);
    set_trap_gate(19, isr19, 0);

    set_intr_gate(32, isr32, 0);
    set_intr_gate(33, isr33, 0);
    set_intr_gate(34, isr34, 0);
    set_intr_gate(35, isr35, 0);
    set_intr_gate(36, isr36, 0);
    set_intr_gate(37, isr37, 0);
    set_intr_gate(38, isr38, 0);
    set_intr_gate(39, isr39, 0);
    set_intr_gate(40, isr40, 0);
    set_intr_gate(41, isr41, 0);
    set_intr_gate(42, isr42, 0);
    set_intr_gate(43, isr43, 0);
    set_intr_gate(44, isr44, 0);
    set_intr_gate(45, isr45, 0);
    set_intr_gate(46, isr46, 0);
    set_intr_gate(47, isr47, 0);

    // system call handler
    set_trap_gate(128, isr128, 3);

    info.size = sizeof(s_idt) - 1;
    info.address = (uint64_t)s_idt;

    __asm__ __volatile__(
	"lidt %0\n"
	:
	: "m" (info)
    );

    pic_remap();
}
