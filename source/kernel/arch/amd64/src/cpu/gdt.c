/* amd64 GDT implementation.
 *
 * Copyright (c) 2012 Zoltan Kovacs
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

// null descriptor, cs kernel, ds kernel, cs user, ds user, tss (required 2 entries)
static uint8_t s_gdt[7 * GDT_DESC_SIZE] _aligned(16);

// =====================================================================================================================
static inline void gdt_init_cs(unsigned index, unsigned dpl)
{
    struct code_segment* cs = (struct code_segment*)&s_gdt[index * GDT_DESC_SIZE];

    cs->always1 = 0x3;
    cs->dpl = dpl;
    cs->present = 1;
    cs->longmode = 1;
    cs->d = 0;
}

// =====================================================================================================================
static inline void gdt_init_ds(unsigned index, unsigned dpl)
{
    struct data_segment* ds = (struct data_segment*)&s_gdt[index * GDT_DESC_SIZE];

    ds->writable = 1;
    ds->always1 = 1;
    ds->dpl = dpl;
    ds->present = 1;
}

// =====================================================================================================================
void gdt_arch_set_tss(struct amd64_tss* tss)
{
    // use the last segment for TSS
    struct tss_segment* ts = (struct tss_segment*)&s_gdt[5 * GDT_DESC_SIZE];

    uint64_t base = (uint64_t)tss;
    uint16_t limit = sizeof(struct amd64_tss);

    ts->limit0 = limit & 0xffff;
    ts->base0 = base & 0xffff;
    ts->base1 = (base >> 16) & 0xff;
    ts->type = 0x9;
    ts->unused0 = 0;
    ts->dpl = 0;
    ts->present = 1;
    ts->limit1 = (limit >> 16) & 0xf;
    ts->available = 0;
    ts->unused1 = 0;
    ts->granularity = 0;
    ts->base2 = (base >> 24) & 0xff;
    ts->base3 = (base >> 32) & 0xffffffff;
    ts->unused2 = 0;
}

// =====================================================================================================================
void gdt_init()
{
    struct gdt_info info;

    memset(s_gdt, 0, sizeof(s_gdt));

    gdt_init_cs(1, 0);
    gdt_init_ds(2, 0);
    gdt_init_cs(3, 3);
    gdt_init_ds(4, 3);

    info.size = sizeof(s_gdt) - 1;
    info.address = (uint64_t)s_gdt;

    __asm__ __volatile__(
	 "lgdt %0\n"
	:
	: "m" (info)
    );

    gdt_arch_reload_segments();
}
