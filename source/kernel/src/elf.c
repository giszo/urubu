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

// =====================================================================================================================
ptr_t elf_load(struct process* p, uint8_t* module)
{
    elf_header* ehdr = (elf_header*)module;
    elf_section_header* shdr = (elf_section_header*)(module + ehdr->shoff);

    for (uint16_t i = 0; i < ehdr->shnum; ++i, ++shdr)
    {
	switch (shdr->type)
	{
	    case ELF_SHT_PROGBITS :
	    {
		// skip sections that does not take any memory
		if ((shdr->flags & ELF_SHF_ALLOC) == 0)
		    break;

		ptr_t addr = shdr->address;
		size_t size = shdr->size;

		uint8_t* data = module + shdr->offset;

		while (size > 0)
		{
		    size_t offset = addr & ~PAGE_MASK;
		    // calculate the number of bytes we can copy to the current page
		    size_t psize = min(size, PAGE_SIZE - offset);

		    ptr_t page = vmm_arch_proc_alloc(p, addr - offset, VMM_WRITE | VMM_USER);

		    if (!page)
			panic("elf: unable to allocate page for PROGBITS section\n");

		    memcpy((uint8_t*)page + offset, data, psize);

		    addr += psize;
		    data += psize;
		    size -= psize;
		}

		break;
	    }

	    case ELF_SHT_NOBITS :
	    {
		ptr_t addr = shdr->address;
		size_t size = shdr->size;

		while (size > 0)
		{
		    size_t offset = addr & ~PAGE_MASK;
		    size_t psize = min(size, PAGE_SIZE - offset);

		    ptr_t page = vmm_arch_proc_alloc(p, addr - offset, VMM_WRITE | VMM_USER);

		    if (!page)
			panic("elf: unable to allocate page for NOBITS section\n");

		    memset((uint8_t*)page + offset, 0, psize);

		    addr += psize;
		    size -= psize;
		}

		break;
	    }
	}
    }

    return ehdr->entry;
}
