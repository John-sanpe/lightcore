/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KMALLOC_H_
#define _KMALLOC_H_

#include <bits.h>
#include <list.h>
#include <mm/gfp.h>

enum kcache_flags {
    KCACHE_PANIC    = BIT(0),
};

struct kcache {
    size_t objsize;
    size_t align;
    struct list_head free;
};

void __malloc *kmalloc(size_t size, gfp_t);
void __malloc *krealloc(const void* block, size_t rsize, gfp_t);
size_t ksize(const void *block);
void kfree(const void *mem);

void __malloc *kcache_alloc(struct kcache *, gfp_t flags);
void kcache_free(struct kcache *, void *block);

struct kcache __malloc *kcache_create(const char *name, size_t size, enum kcache_flags flags);
void kcache_release(struct kcache *);
void kcache_delete(struct kcache *);

/**
 * kzalloc - allocate memory and set to zero.
 * @size: alloc memory size.
 * @flags: the type of memory to allocate.
 */
static __always_inline __malloc void *
kzalloc(size_t size, gfp_t flags)
{
    return kmalloc(size, flags | GFP_ZERO);
}

/**
 * kcache_zalloc - allocate kcache and set to zero.
 * @cache: kcache object to allocate
 * @flags: the type of memory to allocate.
 */
static __always_inline void *
kcache_zalloc(struct kcache *cache, gfp_t flags)
{
    return kcache_alloc(cache, flags | GFP_ZERO);
}

/**
 * kmalloc_array - allocate memory for an array.
 * @nr: number of elements.
 * @size: single element size.
 * @flags: the type of memory to allocate.
 */
static __always_inline __malloc void *
kmalloc_array(size_t nr, size_t size, gfp_t flags)
{
    return kmalloc(size * nr, flags);
}

/**
 * krealloc_array - reallocate memory for an array.
 * @block: number of elements.
 * @nr: number of elements.
 * @size: single element size.
 * @flags: the type of memory to allocate.
 */
static __always_inline __malloc void *
krealloc_array(const void* block, size_t nr, size_t size, gfp_t flags)
{
    return krealloc(block, size * nr, flags);
}

/**
 * kmalloc_array - allocate memory for an array and set to zero.
 * @nr: number of elements.
 * @size: single element size.
 * @flags: the type of memory to allocate.
 */
static __always_inline __malloc void *
kcalloc(size_t nr, size_t size, gfp_t flags)
{
    return kmalloc_array(nr, size, flags | GFP_ZERO);
}

void __init kmem_init(void);
void __init kcache_init(void);

#endif /* _KMALLOC_H_ */