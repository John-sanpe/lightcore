/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_FLOPPY_H_
#define _DRIVER_FLOPPY_H_

/*
 * The 82077AA floppy disk controller has completely integrated all of the logic required for floppy disk control.
 * The 82077AA, a 24 MHz crystal, a resistor package and a device chip select implements a PC AT or PS/2
 * solution. All programmable options default to compatible values. The dual PLL data separator has better
 * performance than most board level/discrete PLL implementations. The FIFO allows better system perform-
 * ance in multi-master systems (e.g. PS/2, EISA). The 82077AA is available in three versionsÐ82077AA-5,
 * 82077AA and 82077AA-1. 82077AA-1 has all features listed in this data sheet. It supports both tape drives and
 * 4 Mb floppy drives. The 82077AA supports 4 Mb floppy drives and is capable of operation at all data rates
 * through 1 Mbps. The 82077AA-5 supports 500/300/250 Kbps data rates for high and low density floppy
 * drives.
 */

enum floppy_registers {
    FLOPPY_SRA      = 0x00, /* (RO): STATUS REGISTER A */
    FLOPPY_SRB      = 0x01, /* (RO): STATUS REGISTER B */
    FLOPPY_DOR      = 0x02, /* (RW): DIGITAL OUTPUT_REGISTER */
    FLOPPY_TDR      = 0x03, /* (RW): TAPE DRIVE REGISTER */
    FLOPPY_MSR      = 0x04, /* (RO): MAIN STATUS REGISTER */
    FLOPPY_DSR      = 0x04, /* (WO): DATARATE SELECT REGISTER */
    FLOPPY_FIFO     = 0x05, /* (RW): DATA FIFO */
    FLOPPY_DIR      = 0x07, /* (RO): DIGITAL INPUT REGISTER */
    FLOPPY_CCR      = 0x07, /* (WO): CONFIGURATION CONTROL REGISTER */
};

enum floppy_commands {
    FLOPPY_CMD_READ_TRACK           = 0x02, // generates IRQ6
    FLOPPY_CMD_SPECIFY              = 0x03, // set drive parameters
    FLOPPY_CMD_SENSE_DRIVE_STATUS   = 0x04, //
    FLOPPY_CMD_WRITE_DATA           = 0xc5, // write to the disk
    FLOPPY_CMD_READ_DATA            = 0xe6, // read from the disk
    FLOPPY_CMD_RECALIBRATE          = 0x07, // seek to cylinder 0
    FLOPPY_CMD_SENSE_INTERRUPT      = 0x08, // ack IRQ6, get status of last command
    FLOPPY_CMD_WRITE_DELETED_DATA   = 0x09, //
    FLOPPY_CMD_READ_ID              = 0x4a, // generates IRQ6
    FLOPPY_CMD_READ_DELETED_DATA    = 12,   //
    FLOPPY_CMD_FORMAT_TRACK         = 0x4d, //
    FLOPPY_CMD_DUMPREG              = 14,   //
    FLOPPY_CMD_SEEK                 = 0x0f, // seek both heads to cylinder X
    FLOPPY_CMD_VERSION              = 16,   // used during initialization, once
    FLOPPY_CMD_SCAN_EQUAL           = 17,   //
    FLOPPY_CMD_PERPENDICULAR_MODE   = 18,   // used during initialization, once, maybe
    FLOPPY_CMD_CONFIGURE            = 19,   // set controller parameters
    FLOPPY_CMD_LOCK                 = 20,   // protect controller params from a reset
    FLOPPY_CMD_VERIFY               = 22,   //
    FLOPPY_CMD_PARTID               = 24,   // part id, extended version
    FLOPPY_CMD_SCAN_LOW_OR_EQUAL    = 25,   //
    FLOPPY_CMD_SCAN_HIGH_OR_EQUAL   = 29,   //

};

enum fdc_version {
    FDC_NONE        = 0x00,
    FDC_UNKNOWN     = 0x10,
    FDC_8272A       = 0x20,     /* Intel 8272a, NEC 765 */
    FDC_765ED       = 0x30,     /* Non-Intel 1MB-compatible FDC, can't detect */
    FDC_82072       = 0x40,     /* Intel 82072; 8272a + FIFO + DUMPREGS */
    FDC_82072A      = 0x45,     /* 82072A (on Sparcs) */
    FDC_82077_ORIG  = 0x51,     /* Original version of 82077AA, sans LOCK */
    FDC_82077       = 0x52,     /* 82077AA-1 */
    FDC_82078_UNKN  = 0x5f,     /* Unknown 82078 variant */
    FDC_82078       = 0x60,     /* 44pin 82078 or 64pin 82078SL */
    FDC_82078_1     = 0x61,     /* 82078-1 (2Mbps fdc) */
    FDC_S82078B     = 0x62,     /* S82078B (first seen on Adaptec AVA-2825 VLB SCSI/EIDE/Floppy controller) */
    FDC_87306       = 0x63,     /* National Semiconductor PC 87306 */
};

/*************************************************************************************/
/*      Mnemonic                 value        meaning/usage                          */

/* SRA bitflag definitions */
#define FLOPPY_SRA_INTP         0x80
#define FLOPPY_SRA_DRV2         0x40
#define FLOPPY_SRA_STEP         0x20
#define FLOPPY_SRA_TRK0         0x10
#define FLOPPY_SRA_HDSEL        0x08
#define FLOPPY_SRA_INDX         0x04
#define FLOPPY_SRA_WP           0x02
#define FLOPPY_SRA_DIR          0x01

/* SRB bitflag definitions */
#define FLOPPY_SRB_DRISEL0      0x20
#define FLOPPY_SRB_WRTOGGLE     0x10
#define FLOPPY_SRB_RDTOGGLE     0x08
#define FLOPPY_SRB_WE           0x04
#define FLOPPY_SRB_MOT1         0x02
#define FLOPPY_SRB_MOT0         0x01

/*
 * The Digital Output Register contains the drive select
 * and motor enable bits, a reset bit and a DMA GATE bit.
 */

/* DOR bitflag definitions */
#define FLOPPY_DOR_MOTOR_D      0x80        // Set to turn drive 3's motor ON
#define FLOPPY_DOR_MOTOR_C      0x40        // Set to turn drive 2's motor ON
#define FLOPPY_DOR_MOTOR_B      0x20        // Set to turn drive 1's motor ON
#define FLOPPY_DOR_MOTOR_A      0x10        // Set to turn drive 0's motor ON
#define FLOPPY_DOR_MOTOR_MASK   0xf0
#define FLOPPY_DOR_IRQ          0x08        // Set to enable IRQs and DMA
#define FLOPPY_DOR_RESET        0x04        // Clear = enter reset mode, Set = normal operation
#define FLOPPY_DOR_DSEL_MASK    0x03        // Select drive number for next access

/*
 * This register allows the user to assign tape support
 * to a particular drive during initialization. Any future
 * references to that drive number automatically in-
 * vokes tape support. This register is cleared by Hard-
 * ware reset, Software resets have no effect.
 */

/* TDR bitflag definitions */
#define FLOPPY_TDR_SEL_D        0x03        // Drive selected: 3
#define FLOPPY_TDR_SEL_C        0x02        // Drive selected: 2
#define FLOPPY_TDR_SEL_B        0x01        // Drive selected: 1
#define FLOPPY_TDR_SEL_A        0x00        // Drive selected: 0
#define FLOPPY_TDR_SEL_MASK     0x03        // Drive selected Mask

/*
 * The Main Status Register is a read-only register and
 * is used for controlling command input and result out-
 * put for all commands.
 */

/* MSR bitflag definitions */
#define FLOPPY_MSR_RQM          0x80        // Set if it's OK (or mandatory) to exchange bytes with the FIFO IO port
#define FLOPPY_MSR_DIO          0x40        // Set if FIFO IO port expects an IN opcode
#define FLOPPY_MSR_NDMA         0x20        // Set in Execution phase of PIO mode read/write commands only.
#define FLOPPY_MSR_CB           0x10        // Command Busy: set when command byte received, cleared at end of Result phase
#define FLOPPY_MSR_ACTD         0x08        // Drive 3 is seeking
#define FLOPPY_MSR_ACTC         0x04        // Drive 2 is seeking
#define FLOPPY_MSR_ACTB         0x02        // Drive 1 is seeking
#define FLOPPY_MSR_ACTA         0x01        // Drive 0 is seeking

/*
 * This register is included for compatibility with the
 * 82072 floppy controller and is write-only. Changing
 * the data rate changes the timings of the drive con-
 * trol signals. To ensure that drive timings are not vio-
 * lated when changing data rates, choose a drive tim-
 * ing such that the fastest data rate will not violate the
 * timing.
 */

/* DSR bitflag definitions */
#define FLOPPY_DSR_SWRST        0x80        // same as DOR RESET
#define FLOPPY_DSR_PD           0x40        // POWER DOWN deactivates the internal clocks and shuts off the oscillator

#define FLOPPY_DSR_COMP_0       0x1c        // Precompensation Delay: 0 ns
#define FLOPPY_DSR_COMP_250     0x18        // Precompensation Delay: 250.00 ns
#define FLOPPY_DSR_COMP_208     0x14        // Precompensation Delay: 208.33 ns
#define FLOPPY_DSR_COMP_166     0x10        // Precompensation Delay: 166.67 ns
#define FLOPPY_DSR_COMP_125     0x0c        // Precompensation Delay: 125.00 ns
#define FLOPPY_DSR_COMP_83      0x08        // Precompensation Delay: 83.34 ns
#define FLOPPY_DSR_COMP_41      0x04        // Precompensation Delay: 41.67 ns
#define FLOPPY_DSR_COMP_DEF     0x00        // Precompensation Delay: DEFAULT

#define FLOPPY_DSR_RATE_MASK    0x03
#define FLOPPY_DSR_RATE_1M      0x03        // Data Rates: 1 Mbps
#define FLOPPY_DSR_RATE_250K    0x02        // Data Rates: 250 Kbps
#define FLOPPY_DSR_RATE_300K    0x01        // Data Rates: 300 Kbps
#define FLOPPY_DSR_RATE_500K    0x00        // Data Rates: 500 Kbps

/*
 *  FIFO: The FIFO register may not have a 16byte buffer in all modes,
 *  but this is a minor difference that does not really affect its operation.
 */

/* FIFO bitflag definitions */
#define FLOPPY_FIFO_MASK        0xff

/* DIR bitflag definitions */
#define FLOPPY_DIR_DSKCHG       0x80
#define FLOPPY_DIR_DMAGATE      0x08

/* CCR bitflag definitions */
#define FLOPPY_CCR_RATE_1M      0x03        // Data Rates: 1 Mbps
#define FLOPPY_CCR_RATE_250K    0x02        // Data Rates: 250 Kbps
#define FLOPPY_CCR_RATE_300K    0x01        // Data Rates: 300 Kbps
#define FLOPPY_CCR_RATE_500K    0x00        // Data Rates: 500 Kbps

/* ST0 bitflag definitions */
#define FLOPPY_ST0_INTR1        0x80
#define FLOPPY_ST0_INTR0        0x40
#define FLOPPY_ST0_SE           0x20
#define FLOPPY_ST0_ECE          0x10
#define FLOPPY_ST0_NR           0x08
#define FLOPPY_ST0_HA           0x04

#define FLOPPY_ST0_DS_D         0x03
#define FLOPPY_ST0_DS_C         0x02
#define FLOPPY_ST0_DS_B         0x01
#define FLOPPY_ST0_DS_A         0x00

/* ST1 bitflag definitions */
#define FLOPPY_ST1_EOC          0x80
#define FLOPPY_ST1_CRC          0x20
#define FLOPPY_ST1_OR           0x10
#define FLOPPY_ST1_ND           0x04
#define FLOPPY_ST1_WP           0x02
#define FLOPPY_ST1_MAM          0x01

/* ST2 bitflag definitions */
#define FLOPPY_ST2_CM           0x40
#define FLOPPY_ST2_CRC          0x20
#define FLOPPY_ST2_WC           0x10
#define FLOPPY_ST2_SEH          0x08
#define FLOPPY_ST2_SNS          0x04
#define FLOPPY_ST2_BC           0x02
#define FLOPPY_ST2_MAM          0x01

#define FLOPPY_SECTOR_SIZE_128  0
#define FLOPPY_SECTOR_SIZE_256  1
#define FLOPPY_SECTOR_SIZE_512  2
#define FLOPPY_SECTOR_SIZE_1K   3
#define FLOPPY_SECTOR_SIZE_2K   4
#define FLOPPY_SECTOR_SIZE_4K   5
#define FLOPPY_SECTOR_SIZE_8K   6
#define FLOPPY_SECTOR_SIZE_16K  7

#endif /* _DRIVER_FLOPPY_H_ */