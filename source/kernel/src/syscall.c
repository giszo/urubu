/* System call handler.
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
#include <kernel/cpu/irq.h>
#include <kernel/ipc/port.h>
#include <kernel/mm/vmm.h>
#include <kernel/mm/pmm.h>

struct syscall_entry
{
    const char* name;
    void* p;
};

typedef long syscall_func(long p1, long p2, long p3, long p4, long p5);

// =====================================================================================================================
static long sys_dprintf(const char* msg)
{
    kprintf("%s", msg);
    return 0;
}

// =====================================================================================================================
static struct syscall_entry s_syscall_table[] =
{
    // IPC
    {"ipc_port_create", sys_ipc_port_create},
    {"ipc_port_receive", sys_ipc_port_receive},

    // IRQ
    {"irq_register", sys_irq_register},
    {"irq_finished", sys_irq_finished},

    // memory management
    {"pmm_get_statistics", sys_pmm_get_statistics},
    {"vmm_map", sys_vmm_map},

    {"dprintf", sys_dprintf}
};

// =====================================================================================================================
long do_syscall(long p1, long p2, long p3, long p4, long p5, long p6)
{
    // make sure the system call number is valid
    if ((unsigned long)p1 >= (sizeof(s_syscall_table) / sizeof(s_syscall_table[0])))
	return -1;

    struct syscall_entry* entry = &s_syscall_table[p1];
    syscall_func* func = (syscall_func*)entry->p;

    return func(p2, p3, p4, p5, p6);
}
