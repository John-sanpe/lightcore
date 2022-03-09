/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <idr.h>
#include <kmalloc.h>
#include <export.h>

static struct kcache *idr_cache;
static struct kcache *idrn_cache;

static long idr_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    struct idr_node *node_a = rb_to_idr(rba);
    struct idr_node *node_b = rb_to_idr(rbb);
    return node_a->index < node_b->index ? -1 : 1;
}

static long idr_rb_find(const struct rb_node *rb, const void *key)
{
    struct idr_node *node = rb_to_idr(rb);
    if (node->index == (unsigned long)key) return 0;
    return node->index > (unsigned long)key ? -1 : 1;
}

/**
 * idr_get_free_range - get the first unused id
 * @idr: the idr handle
 * @min: minimum id to get
 * @max: maximum id to get
 * @nextp: return the next linked list node
 */
static unsigned long idr_get_free_range(struct idr *idr, unsigned long min, unsigned long max, struct list_head **nextp)
{
    unsigned long walk;
    struct idr_node *node;
    struct rb_node *rb;

    min = max(max(min, 1), idr->id_base);

    rb = rb_find(&idr->root, (void *)min, idr_rb_find);
    if (unlikely(!(node = rb_to_idr_safe(rb)))) {
        *nextp = idr->list.next;
        return min;
    }

    for (walk = min + 1; walk < max; ++walk) {
        if (list_check_end(&idr->list, &node->list))
            goto finish;

        node = list_next_entry(node, list);
        if (node->index != walk)
            goto finish;
    }

    return IDR_NONE;

finish:
    *nextp = node->list.next;
    return walk;
}

/**
 * idr_node_alloc_range - sequence add an node to the idr
 * @idr: the idr handle
 * @node: idr node to add
 * @pdata: node pdata
 * @min: minimum id
 * @max: maximum id
 */
unsigned long idr_node_alloc_range(struct idr *idr, struct idr_node *node, void *pdata, unsigned long min, unsigned long max)
{
    struct list_head *next;
    unsigned long id;

    spin_lock(&idr->lock);

    id = idr_get_free_range(idr, min, max, &next);
    if (unlikely(id == IDR_NONE))
        goto finish;

    node->index = id;
    node->pdata = pdata;
    list_add_prev(next, &node->list);
    rb_insert(&idr->root, &node->rb, idr_rb_cmp);

finish:
    spin_unlock(&idr->lock);
    return id;
}
EXPORT_SYMBOL(idr_node_alloc_range);

/**
 * idr_node_alloc_cyclic_range - cyclic add an node to the idr
 * @idr: the idr handle
 * @node: idr node to add
 * @pdata: node pdata
 * @min: minimum id
 * @max: maximum id
 */
unsigned long idr_node_alloc_cyclic_range(struct idr *idr, struct idr_node *node, void *pdata, unsigned long min, unsigned long max)
{
    unsigned long next;
    unsigned long id;

    next = clamp(idr->id_next, min, max);

    id = idr_node_alloc_range(idr, node, pdata, next, max);
    if (id == IDR_NONE)
        id = idr_node_alloc_range(idr, node, pdata, min, max);

    if (id == IDR_NONE)
        return id;

    idr->id_next = id + 1;
    return id;
}
EXPORT_SYMBOL(idr_node_alloc_cyclic_range);

/**
 * idr_alloc_range - alloc a node and sequence add the id to idr
 * @idr: the idr handle
 * @pdata: node pdata
 * @min: minimum id
 * @max: maximum id
 * @flags: gfp flags
 */
unsigned long idr_alloc_range(struct idr *idr, void *pdata, unsigned long min, unsigned long max, gfp_t flags)
{
    struct idr_node *node;
    unsigned long id;

    node = kcache_alloc(idrn_cache, flags);
    if (!node)
        return IDR_NONE;

    id = idr_node_alloc_range(idr, node, pdata, min, max);
    if (unlikely(id == IDR_NONE))
        kcache_free(idrn_cache, node);

    return id;
}
EXPORT_SYMBOL(idr_alloc_range);

/**
 * idr_alloc_cyclic_range - alloc a node and cyclic add the id to idr
 * @idr: the idr handle
 * @pdata: node pdata
 * @min: minimum id
 * @max: maximum id
 * @flags: gfp flags
 */
unsigned long idr_alloc_cyclic_range(struct idr *idr, void *pdata, unsigned long min, unsigned long max, gfp_t flags)
{
    struct idr_node *node;
    unsigned long id;

    node = kcache_alloc(idrn_cache, flags);
    if (!node)
        return IDR_NONE;

    id = idr_node_alloc_cyclic_range(idr, node, pdata, min, max);
    if (unlikely(id == IDR_NONE))
        kcache_free(idrn_cache, node);

    return id;
}
EXPORT_SYMBOL(idr_alloc_cyclic_range);

/**
 * idr_node_free - remove an id from the idr
 * @idr: the idr handle
 * @id: the id to release
 */
struct idr_node *idr_node_free(struct idr *idr, unsigned long id)
{
    struct idr_node *node;
    struct rb_node *rb;

    spin_lock(&idr->lock);

    rb = rb_find(&idr->root, (void *)id, idr_rb_find);
    if (!(node = rb_to_idr_safe(rb)))
        goto finish;

    list_del(&node->list);
    rb_delete(&idr->root, &node->rb);

finish:
    spin_unlock(&idr->lock);
    return node;
}
EXPORT_SYMBOL(idr_node_free);

/**
 * idr_free - remove id from the idr and free
 * @idr: the idr handle
 * @id: the id to release
 */
void idr_free(struct idr *idr, unsigned long id)
{
    struct idr_node *node;

    node = idr_node_free(idr, id);
    if (node)
        kcache_free(idrn_cache, node);
}
EXPORT_SYMBOL(idr_free);

/**
 * idr_find - remove id from the idr and free
 * @idr: the idr handle
 * @id: the id to find
 */
void *idr_find(struct idr *idr, unsigned long id)
{
    struct idr_node *node;
    struct rb_node *rb;

    spin_lock(&idr->lock);

    rb = rb_find(&idr->root, (void *)id, idr_rb_find);
    node = rb_to_idr_safe(rb);

    spin_unlock(&idr->lock);
    return node ? node->pdata : NULL;
}
EXPORT_SYMBOL(idr_find);

/**
 * idr_release - remove id from the idr and free
 * @idr: the idr handle
 */
void idr_release(struct idr *idr)
{
    struct idr_node *node, *next;

    list_for_each_entry_safe(node, next, &idr->list, list)
        kcache_free(idrn_cache, node);

    idr->root = RB_INIT;
    list_head_init(&idr->list);
}
EXPORT_SYMBOL(idr_release);

/**
 * idr_create - create an idr
 * @base: id number base
 */
struct idr *idr_create(unsigned long base)
{
    struct idr *idr;

    idr = kcache_alloc(idr_cache, GFP_KERNEL);
    if (!idr)
        return idr;

    idr->id_base = base;
    idr->root = RB_INIT;
    list_head_init(&idr->list);
    spin_lock_init(&idr->lock);

    return idr;
}
EXPORT_SYMBOL(idr_create);

/**
 * idr_delete - delete an idr
 * @idr: the idr handle
 */
void idr_delete(struct idr *idr)
{
    idr_release(idr);
    kcache_free(idr_cache, idr);
}
EXPORT_SYMBOL(idr_delete);

void __init idr_init(void)
{
    idr_cache = KCACHE_CREATE(struct idr, KCACHE_PANIC);
    idrn_cache = KCACHE_CREATE(struct idr_node, KCACHE_PANIC);
}
