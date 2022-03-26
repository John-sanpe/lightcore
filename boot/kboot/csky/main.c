/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <kernel.h>
#include <kboot.h>

asmlinkage void main(uint32_t mask, uint32_t dtb_start, void *printf)
{
    struct boot_head *head;

    /* Init console */
    if (mask == KMAGIC) {
        pr_init(printf);
    }

    /* Init Memory */
    heap_init(heap_start, heap_size);
    kernel_map();

    /* Extract kernel */
    extract_kernel(kernel_entry, piggy_start, piggy_size);
    head = kernel_entry;
    head->dtb = dtb_start ? CONFIG_PAGE_OFFSET + dtb_start : 0;
    head->stdout = (size_t)stdout;

    /* Boot kernel */
    pr_boot("boot to kernel...\n");
    kernel_start(pa_to_va(NORMAL_OFFSET));
}

