/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <kernel.h>
#include <driver/mtd/spinor.h>

bool norflash_id(void)
{
    uint8_t cmd[4] = {
        [0] = NORFLASH_JEDEC_ID,
    };

    spi_sel(true);
    spi_transmit(cmd, cmd, ARRAY_SIZE(cmd));
    spi_sel(false);

    pr_boot("norflash manufacturer: %#x\n", cmd[1]);
    pr_boot("norflash memory type: %#x\n", cmd[2]);
    pr_boot("norflash capacity: %#x\n", cmd[3]);

    return (cmd[1] == 0x00 && cmd[2] == 0x00 && cmd[3] == 0x00) ||
           (cmd[1] == 0xff && cmd[2] == 0xff && cmd[3] == 0xff);
}

static bool norflash_detect(void)
{
    uint8_t cmd[6] = {
        [0] = 0x03,
    };

    spi_sel(true);
    spi_transmit(cmd, cmd, sizeof(cmd));
    spi_sel(false);

    return cmd[5] == (uint8_t)IMAGE_MAGIC;
}

void norflash_read(uint8_t *buff, size_t addr, uint32_t len)
{
    unsigned int cmdlen;
    uint8_t cmd[5];

    if (norflash_detect()) {
        cmdlen = 5;
        cmd[0] = 0x03;
        cmd[1] = addr >> 24;
        cmd[2] = addr >> 16;
        cmd[3] = addr >> 8;
        cmd[4] = addr >> 0;
    } else {
        cmdlen = 4;
        cmd[0] = 0x03;
        cmd[1] = addr >> 16;
        cmd[2] = addr >> 8;
        cmd[3] = addr >> 0;
    }

    spi_sel(true);
    spi_transmit(cmd, NULL, cmdlen);
    spi_transmit(NULL, buff, len);
    spi_sel(false);
}
