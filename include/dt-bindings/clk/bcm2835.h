/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DT_BINDINGS_CLK_BCM2835_H_
#define _DT_BINDINGS_CLK_BCM2835_H_

#define BCM2835_PLLA                0
#define BCM2835_PLLB                1
#define BCM2835_PLLC                2
#define BCM2835_PLLD                3
#define BCM2835_PLLH                4

#define BCM2835_PLLA_CORE           5
#define BCM2835_PLLA_PER            6
#define BCM2835_PLLB_ARM            7
#define BCM2835_PLLC_CORE0          8
#define BCM2835_PLLC_CORE1          9
#define BCM2835_PLLC_CORE2          10
#define BCM2835_PLLC_PER            11
#define BCM2835_PLLD_CORE           12
#define BCM2835_PLLD_PER            13
#define BCM2835_PLLH_RCAL           14
#define BCM2835_PLLH_AUX            15
#define BCM2835_PLLH_PIX            16

#define BCM2835_CLOCK_TIMER         17
#define BCM2835_CLOCK_OTP           18
#define BCM2835_CLOCK_UART          19
#define BCM2835_CLOCK_VPU           20
#define BCM2835_CLOCK_V3D           21
#define BCM2835_CLOCK_ISP           22
#define BCM2835_CLOCK_H264          23
#define BCM2835_CLOCK_VEC           24
#define BCM2835_CLOCK_HSM           25
#define BCM2835_CLOCK_SDRAM         26
#define BCM2835_CLOCK_TSENS         27
#define BCM2835_CLOCK_EMMC          28
#define BCM2835_CLOCK_PERI_IMAGE    29
#define BCM2835_CLOCK_PWM           30
#define BCM2835_CLOCK_PCM           31

#define BCM2835_PLLA_DSI0           32
#define BCM2835_PLLA_CCP2           33
#define BCM2835_PLLD_DSI0           34
#define BCM2835_PLLD_DSI1           35

#define BCM2835_CLOCK_AVEO          36
#define BCM2835_CLOCK_DFT           37
#define BCM2835_CLOCK_GP0           38
#define BCM2835_CLOCK_GP1           39
#define BCM2835_CLOCK_GP2           40
#define BCM2835_CLOCK_SLIM          41
#define BCM2835_CLOCK_SMI           42
#define BCM2835_CLOCK_TEC           43
#define BCM2835_CLOCK_DPI           44
#define BCM2835_CLOCK_CAM0          45
#define BCM2835_CLOCK_CAM1          46
#define BCM2835_CLOCK_DSI0E         47
#define BCM2835_CLOCK_DSI1E         48
#define BCM2835_CLOCK_DSI0P         49
#define BCM2835_CLOCK_DSI1P         50
#define BCM2835_CLOCK_EMMC2         51

#define BCM2835_AUX_CLOCK_UART      0
#define BCM2835_AUX_CLOCK_SPI1      1
#define BCM2835_AUX_CLOCK_SPI2      2
#define BCM2835_AUX_CLOCK_COUNT     3

#endif  /* _DT_BINDINGS_CLK_BCM2835_H_ */
