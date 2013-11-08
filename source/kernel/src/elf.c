/* ELF binary loader.
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

#include <kernel/kernel.h>
#include <kernel/elf.h>
#include <kernel/mm/vmm.h>
#include <kernel/lib/string.h>

#include <arch/mm/config.h>

#include <kernel/console.h>

// =====================================================================================================================
struct region_info
{
    int found;
    ptr_t base;
    ptr_t end;
};

// =====================================================================================================================
static inline int elf_alloc_region(struct process* p, struct region_info* r, int writable)
{
    ptr_t base = r->base & PAGE_MASK;
    size_t off = r->base & ~PAGE_MASK;
    size_t size = r->end - r->base + 1 + off;
    unsigned flags = VMM_READ | (writable ? VMM_WRITE : 0) | VMM_USER | VMM_FIXED;

    return vmm_alloc(p, &base, PAGE_ALIGN(size), flags);
}

// =====================================================================================================================
ptr_t elf_load(struct process* p, uint8_t* module)
{
    elf_header* ehdr = (elf_header*)module;
    elf_section_header* shdr = (elf_section_header*)(module + ehdr->shoff);

    struct region_info text = {0, 0, 0};
    struct region_info data = {0, 0, 0};

    // first run through the section headers to find out the regions we need to allocate in the virtual address space
    for (uint16_t i = 0; i < ehdr->shnum; ++i, ++shdr)
    {
	// skip sections that does not require allocation
	if ((shdr->flags & ELF_SHF_ALLOC) == 0)
	    continue;

	// find out which region we want to use for this section
	struct region_info* r = (shdr->flags & ELF_SHF_WRITE) ? &data : &text;

	ptr_t end = shdr->address + shdr->size - 1;

	if (r->found)
	{
	    if (shdr->address <= r->base)
		panic("elf: section base is lower compared to the already found region!\n");
	    if (end <= r->end)
		panic("elf: section end is lower compared to the already found region!\n");

	    r->end = end;
	}
	else
	{
	    r->found = 1;
	    r->base = shdr->address;
	    r->end = end;
	}
    }

    dprintf("elf: text @ %p-%p, data @ %p-%p\n", text.base, text.end, data.base, data.end);

    // allocate text region
    if (text.found && elf_alloc_region(p, &text, 0 /* not writable */) != 0)
    {
	kprintf("elf: unable to allocate text region\n");
	return -1;
    }

    // allocate data region
    if (data.found && elf_alloc_region(p, &data, 1 /* writable */) != 0)
    {
	kprintf("elf: unable to allocate data region\n");
	return -1;
    }

    // reset the section header pointer to the beginning ..
    shdr = (elf_section_header*)(module + ehdr->shoff);

    for (uint16_t i = 0; i < ehdr->shnum; ++i, ++shdr)
    {
	// skip sections that does not take any memory
	if ((shdr->flags & ELF_SHF_ALLOC) == 0)
	    continue;

	switch (shdr->type)
	{
	    case ELF_SHT_PROGBITS :
		if (vmm_copy_to(p, shdr->address, module + shdr->offset, shdr->size) != 0)
		{
		    kprintf("elf: unable to copy into PROGBITS section\n");
		    return -1;
		}

		break;

	    case ELF_SHT_NOBITS :
		if (vmm_clear(p, shdr->address, shdr->size) != 0)
		{
		    kprintf("elf: unable to clear NOBITS section\n");
		    return -1;
		}

		break;
	}
    }

    return ehdr->entry;
}
