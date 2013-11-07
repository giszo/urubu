/* C entry point of the kernel.
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

#include <kernel/console.h>
#include <kernel/kernel.h>
#include <kernel/elf.h>
#include <kernel/cpu/cpu.h>
#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>
#include <kernel/mm/map.h>
#include <kernel/mm/pmm.h>
#include <kernel/lib/string.h>

#include <arch/cpu/cpu.h>
#include <arch/cpu/gdt.h>
#include <arch/cpu/idt.h>
#include <arch/mm/config.h>
#include <arch/mm/vmm.h>
#include <arch/mm/pmm.h>

#include <pc/screen.h>
#include <pc/pic.h>
#include <pc/serial.h>
#include <pc/multiboot.h>

extern size_t s_memory_size;

#define MOD_NAME_SIZE 32

// multiboot module informations are saved here during booting
struct module_info
{
    ptr_t addr;
    size_t size;
    char name[MOD_NAME_SIZE];
};

static size_t s_moduleCount = 0;
static struct module_info s_modules[32];

// =====================================================================================================================
static void save_module_list(struct mb_info* info)
{
    struct mb_mod_entry* mods = (struct mb_mod_entry*)(uint64_t)info->mod_info.addr;

    // save module informations
    for (size_t i = 0; i < min(info->mod_info.length, 32); ++i)
    {
	// try to find out the name of the module
	char* name = strrchr((char*)(uint64_t)mods[i].params, '/');

	if (name == NULL)
	    continue;

	s_modules[s_moduleCount].addr = mods[i].start;
	s_modules[s_moduleCount].size = PAGE_ALIGN(mods[i].end - mods[i].start);

	strncpy(s_modules[s_moduleCount].name, name + 1, MOD_NAME_SIZE);
	s_modules[s_moduleCount].name[MOD_NAME_SIZE - 1] = 0;

	++s_moduleCount;
    }
}

// =====================================================================================================================
static void modules_start()
{
    for (size_t i = 0; i < s_moduleCount; ++i)
    {
	struct module_info* mod = &s_modules[i];

	kprintf("starting module: %s\n", mod->name);

	// create a new process for the module
	struct process* p = process_create(mod->name);

	if (!p)
	    panic("Unable to create process for module: %s\n", mod->name);

	// map the module into the kernel address space to get access to its contents
	for (size_t i = 0; i < mod->size; i += PAGE_SIZE)
	    vmm_arch_map(MODULE_MAP_BASE + i, mod->addr + i, 0);

	// load the elf module into the new address space
	ptr_t entry = elf_load(p, (uint8_t*)MODULE_MAP_BASE);

	// unmap the module from the kernel
	for (size_t i = 0; i < mod->size; i += PAGE_SIZE)
	    vmm_arch_unmap(MODULE_MAP_BASE + i);

	if (!entry)
	    panic("Unable to load ELF module: %s\n", mod->name);

	// allocate stack for the main thread
	ptr_t stack;

	if (vmm_alloc(p, &stack, PAGE_SIZE, VMM_READ | VMM_WRITE | VMM_STACK | VMM_USER) != 0)
	    panic("Unable to allocate stack for the main thread of the new process!\n");

	// create the main thread of the loaded module
	struct thread* t = thread_create_user(p, "main", (void*)entry, NULL, (void*)stack, PAGE_SIZE);

	if (!t)
	    panic("Unable to create main thread for %s module\n", mod->name);

	// ... and start it :]
	thread_wake_up(t);
    }
}

// =====================================================================================================================
void arch_start(struct mb_info* info)
{
    pc_screen_init(KERNEL_VIRT_BASE + PC_SCREEN_BASE);
    pc_serial_init();

    kprintf("Loading urubu ...\n");

    gdt_init();
    idt_init();
    cpu_arch_init();
    // Build the map of free memory regions.
    struct memory_map* free_map = pmm_arch_build_free_map(info);
    // Save module informations because we will turn identity mapping off.
    save_module_list(info);
    // Perform architecture specific VMM initialization because paging is already enabled so we will need some
    // support from there to be able to initialize architecture independent MM parts of the kernel.
    vmm_arch_pre_init();
    pmm_init(s_memory_size, free_map);
    vmm_arch_init(s_memory_size);

    kernel_start();

    modules_start();

    // enable interrupts
    cpu_arch_enable_int();
}
