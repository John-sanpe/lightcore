/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <initcall.h>
#include <kmalloc.h>
#include <printk.h>
#include <export.h>

LIST_HEAD(kshell_list);
SPIN_LOCK(kshell_lock);

#define KSHELL_BOOTARGS CONFIG_KSHELL_BOOTARGS
static char bootargs[32][KSHELL_BOOTARGS];
static char *bootarg_ptr[KSHELL_BOOTARGS + 1];
static unsigned int bootarg_num;

static unsigned int kshell_console_read(char *str, unsigned int len, void *data)
{
    return console_read(str, len);
}

static void kshell_console_write(const char *str, unsigned int len, void *data)
{
    console_write(str, len);
}

static long kshell_sort(struct list_head *a, struct list_head *b, void *pdata)
{
    struct kshell_command *ca = list_to_kshell(a);
    struct kshell_command *cb = list_to_kshell(b);
    return strcmp(ca->name, cb->name);
}

struct kshell_command *kshell_find(const char *name)
{
    struct kshell_command *cmd, *find = NULL;

    spin_lock(&kshell_lock);

    list_for_each_entry(cmd, &kshell_list, list) {
        if (!strcmp(name, cmd->name)) {
            find = cmd;
            break;
        }
    }

    spin_unlock(&kshell_lock);
    return find;
}
EXPORT_SYMBOL(kshell_find);

state kshell_register(struct kshell_command *cmd)
{
    if (!cmd->name || !cmd->desc)
        return -EINVAL;

    if (kshell_find(cmd->name)) {
        pr_err("cmd %s already registered\n", cmd->name);
        return -EINVAL;
    }

    spin_lock(&kshell_lock);
    list_add(&kshell_list, &cmd->list);
    spin_unlock(&kshell_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(kshell_register);

void kshell_unregister(struct kshell_command *cmd)
{
    spin_lock(&kshell_lock);
    list_del(&cmd->list);
    spin_unlock(&kshell_lock);
}
EXPORT_SYMBOL(kshell_unregister);

state kshell_exec(struct kshell_context *ctx, const struct kshell_command *cmd, int argc, char *argv[])
{
    unsigned int count;
    char **vbuff;
    state retval;

    if (!cmd->exec)
        return -ENXIO;

    vbuff = kzalloc(sizeof(char *) * (argc + 1), GFP_KERNEL);
    if (!vbuff)
        return -ENOMEM;

    for (count = 0; count < argc; ++count) {
        unsigned int len;
        char *string;

        len = strlen(argv[count]);
        string = kmalloc(len + 1, GFP_KERNEL);
        if (!string) {
            retval = -ENOMEM;
            goto finish;
        }

        strcpy(string, argv[count]);
        vbuff[count] = string;
    }

    vbuff[argc] = NULL;
    retval = cmd->exec(ctx, argc, vbuff);

finish:
    for (count = 0; vbuff[count]; ++count)
        kfree(vbuff[count]);

    kfree(vbuff);
    return retval;
}
EXPORT_SYMBOL(kshell_exec);

state kshell_execv(struct kshell_context *ctx, const char *name, int argc, char *argv[])
{
    struct kshell_command *cmd;

    cmd = kshell_find(name);
    if (!cmd)
        return -EBADF;

    return kshell_exec(ctx, cmd, argc, argv);
}
EXPORT_SYMBOL(kshell_execv);

static void __init ksh_initcall(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, kshell_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed\n", fn->name);
    }
}

static void bootarg_recode(void)
{
    unsigned int count;
    for (count = 0; count < bootarg_num; ++count)
        bootarg_ptr[count + 1] = bootargs[count];
}

state kshell_bootarg_add(const char *args)
{
    if (bootarg_num >= KSHELL_BOOTARGS)
        return -ENOMEM;
    strcpy(bootargs[bootarg_num++], args);
    return -ENOERR;
}

static state kshell_bootarg(char *args)
{
    if (*args)
        kshell_bootarg_add(args);
    return -ENOERR;
}
bootarg_initcall("kshell", kshell_bootarg);

void ksh_init(void)
{
    unsigned int maxdepth = CONFIG_KSHELL_DEPTH;
    struct readline_state rstate;
    struct kshell_context ctx;

    ctx.read = kshell_console_read;
    ctx.write = kshell_console_write;
    ctx.env = RB_INIT;
    ctx.depth = &maxdepth;
    ctx.readline = &rstate;
    rstate.read = kshell_console_read;
    rstate.write = kshell_console_write;

    printk("##########################\n");
    printk("Welcome to lightcore shell\n");

    ksh_initcall();
    list_qsort(&kshell_list, kshell_sort, NULL);
    bootarg_recode();

    printk("Have a lot of fun..\n");
    kshell_main(&ctx, bootarg_num + 1, bootarg_ptr);
}
