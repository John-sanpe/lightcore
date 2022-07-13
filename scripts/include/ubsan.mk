# SPDX-License-Identifier: GPL-2.0-or-later
ubsan-cflags-$(CONFIG_UBSAN_ALIGNMENT)      += -fsanitize=alignment
ubsan-cflags-$(CONFIG_UBSAN_ONLY_BOUNDS)    += -fsanitize=bounds
ubsan-cflags-$(CONFIG_UBSAN_ARRAY_BOUNDS)   += -fsanitize=array-bounds
ubsan-cflags-$(CONFIG_UBSAN_LOCAL_BOUNDS)   += -fsanitize=local-bounds
ubsan-cflags-$(CONFIG_UBSAN_SHIFT)          += -fsanitize=shift
ubsan-cflags-$(CONFIG_UBSAN_DIV_ZERO)       += -fsanitize=integer-divide-by-zero
ubsan-cflags-$(CONFIG_UBSAN_UNREACHABLE)    += -fsanitize=unreachable
ubsan-cflags-$(CONFIG_UBSAN_BOOL)           += -fsanitize=bool
ubsan-cflags-$(CONFIG_UBSAN_ENUM)           += -fsanitize=enum
ubsan-cflags-$(CONFIG_UBSAN_TRAP)           += -fsanitize-undefined-trap-on-error
export CFLAGS_UBSAN := $(ubsan-cflags-y)
