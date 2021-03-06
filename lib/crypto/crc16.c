/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/crc.h>
#include <crypto/crc16-table.h>
#include <export.h>

uint16_t crc16(const uint8_t *src, size_t len, uint16_t crc)
{
    return crc16_inline(src, len, crc);
}
EXPORT_SYMBOL(crc16);
