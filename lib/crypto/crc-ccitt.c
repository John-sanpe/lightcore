/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/crc.h>
#include <crypto/crc-ccitt-table.h>
#include <export.h>

uint16_t crc_ccitt(const uint8_t *src, size_t len, uint16_t crc)
{
    return crc_ccitt_inline(src, len, crc);
}
EXPORT_SYMBOL(crc_ccitt);
