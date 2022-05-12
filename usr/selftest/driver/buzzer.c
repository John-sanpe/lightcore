/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <delay.h>
#include <driver/buzzer.h>

struct buzzer_test {
    const char *name;
    enum buzzer_frequency freq;
};

#define BUZZER_TEST(freq) \
    {__stringify(freq), freq}

static struct buzzer_test test_freq[] = {
    BUZZER_TEST(BUZZER_FREQ_C0), BUZZER_TEST(BUZZER_FREQ_CS0),
    BUZZER_TEST(BUZZER_FREQ_D0), BUZZER_TEST(BUZZER_FREQ_DS0),
    BUZZER_TEST(BUZZER_FREQ_E0),
    BUZZER_TEST(BUZZER_FREQ_F0), BUZZER_TEST(BUZZER_FREQ_FS0),
    BUZZER_TEST(BUZZER_FREQ_G0), BUZZER_TEST(BUZZER_FREQ_GS0),
    BUZZER_TEST(BUZZER_FREQ_A0), BUZZER_TEST(BUZZER_FREQ_AS0),
    BUZZER_TEST(BUZZER_FREQ_B0),

    BUZZER_TEST(BUZZER_FREQ_C1), BUZZER_TEST(BUZZER_FREQ_CS1),
    BUZZER_TEST(BUZZER_FREQ_D1), BUZZER_TEST(BUZZER_FREQ_DS1),
    BUZZER_TEST(BUZZER_FREQ_E1),
    BUZZER_TEST(BUZZER_FREQ_F1), BUZZER_TEST(BUZZER_FREQ_FS1),
    BUZZER_TEST(BUZZER_FREQ_G1), BUZZER_TEST(BUZZER_FREQ_GS1),
    BUZZER_TEST(BUZZER_FREQ_A1), BUZZER_TEST(BUZZER_FREQ_AS1),
    BUZZER_TEST(BUZZER_FREQ_B1),

    BUZZER_TEST(BUZZER_FREQ_C2), BUZZER_TEST(BUZZER_FREQ_CS2),
    BUZZER_TEST(BUZZER_FREQ_D2), BUZZER_TEST(BUZZER_FREQ_DS2),
    BUZZER_TEST(BUZZER_FREQ_E2),
    BUZZER_TEST(BUZZER_FREQ_F2), BUZZER_TEST(BUZZER_FREQ_FS2),
    BUZZER_TEST(BUZZER_FREQ_G2), BUZZER_TEST(BUZZER_FREQ_GS2),
    BUZZER_TEST(BUZZER_FREQ_A2), BUZZER_TEST(BUZZER_FREQ_AS2),
    BUZZER_TEST(BUZZER_FREQ_B2),

    BUZZER_TEST(BUZZER_FREQ_C3), BUZZER_TEST(BUZZER_FREQ_CS3),
    BUZZER_TEST(BUZZER_FREQ_D3), BUZZER_TEST(BUZZER_FREQ_DS3),
    BUZZER_TEST(BUZZER_FREQ_E3),
    BUZZER_TEST(BUZZER_FREQ_F3), BUZZER_TEST(BUZZER_FREQ_FS3),
    BUZZER_TEST(BUZZER_FREQ_G3), BUZZER_TEST(BUZZER_FREQ_GS3),
    BUZZER_TEST(BUZZER_FREQ_A3), BUZZER_TEST(BUZZER_FREQ_AS3),
    BUZZER_TEST(BUZZER_FREQ_B3),

    BUZZER_TEST(BUZZER_FREQ_C4), BUZZER_TEST(BUZZER_FREQ_CS4),
    BUZZER_TEST(BUZZER_FREQ_D4), BUZZER_TEST(BUZZER_FREQ_DS4),
    BUZZER_TEST(BUZZER_FREQ_E4),
    BUZZER_TEST(BUZZER_FREQ_F4), BUZZER_TEST(BUZZER_FREQ_FS4),
    BUZZER_TEST(BUZZER_FREQ_G4), BUZZER_TEST(BUZZER_FREQ_GS4),
    BUZZER_TEST(BUZZER_FREQ_A4), BUZZER_TEST(BUZZER_FREQ_AS4),
    BUZZER_TEST(BUZZER_FREQ_B4),

    BUZZER_TEST(BUZZER_FREQ_C5), BUZZER_TEST(BUZZER_FREQ_CS5),
    BUZZER_TEST(BUZZER_FREQ_D5), BUZZER_TEST(BUZZER_FREQ_DS5),
    BUZZER_TEST(BUZZER_FREQ_E5),
    BUZZER_TEST(BUZZER_FREQ_F5), BUZZER_TEST(BUZZER_FREQ_FS5),
    BUZZER_TEST(BUZZER_FREQ_G5), BUZZER_TEST(BUZZER_FREQ_GS5),
    BUZZER_TEST(BUZZER_FREQ_A5), BUZZER_TEST(BUZZER_FREQ_AS5),
    BUZZER_TEST(BUZZER_FREQ_B5),

    BUZZER_TEST(BUZZER_FREQ_C6), BUZZER_TEST(BUZZER_FREQ_CS6),
    BUZZER_TEST(BUZZER_FREQ_D6), BUZZER_TEST(BUZZER_FREQ_DS6),
    BUZZER_TEST(BUZZER_FREQ_E6),
    BUZZER_TEST(BUZZER_FREQ_F6), BUZZER_TEST(BUZZER_FREQ_FS6),
    BUZZER_TEST(BUZZER_FREQ_G6), BUZZER_TEST(BUZZER_FREQ_GS6),
    BUZZER_TEST(BUZZER_FREQ_A6), BUZZER_TEST(BUZZER_FREQ_AS6),
    BUZZER_TEST(BUZZER_FREQ_B6),

    BUZZER_TEST(BUZZER_FREQ_C7), BUZZER_TEST(BUZZER_FREQ_CS7),
    BUZZER_TEST(BUZZER_FREQ_D7), BUZZER_TEST(BUZZER_FREQ_DS7),
    BUZZER_TEST(BUZZER_FREQ_E7),
    BUZZER_TEST(BUZZER_FREQ_F7), BUZZER_TEST(BUZZER_FREQ_FS7),
    BUZZER_TEST(BUZZER_FREQ_G7), BUZZER_TEST(BUZZER_FREQ_GS7),
    BUZZER_TEST(BUZZER_FREQ_A7), BUZZER_TEST(BUZZER_FREQ_AS7),
    BUZZER_TEST(BUZZER_FREQ_B7),

    BUZZER_TEST(BUZZER_FREQ_C8), BUZZER_TEST(BUZZER_FREQ_CS8),
    BUZZER_TEST(BUZZER_FREQ_D8), BUZZER_TEST(BUZZER_FREQ_DS8),
};

static state buzzer_testing(struct kshell_context *ctx, void *pdata)
{
    struct buzzer_device *bdev;
    unsigned int index;
    state retval = -ENOERR;

    spin_lock(&buzzer_lock);
    list_for_each_entry(bdev, &buzzer_list, list) {
        buzzer_start(bdev);
        for (index = 0; index < ARRAY_SIZE(test_freq); ++index) {
            retval = buzzer_freq_set(bdev, test_freq[index].freq);
            kshell_printf(ctx, "buzzer %s test freq %s: %d\n", bdev->dev->name, test_freq[index].name, retval);
            if (retval)
                break;
            mdelay(10);
        }
        buzzer_stop(bdev);
    }
    spin_unlock(&buzzer_lock);

    return retval;
}

static struct selftest_command buzzer_command = {
    .group = "driver",
    .name = "buzzer",
    .desc = "buzzer unit test",
    .testing = buzzer_testing,
};

static state selftest_buzzer_init(void)
{
    return selftest_register(&buzzer_command);
}
kshell_initcall(selftest_buzzer_init);
