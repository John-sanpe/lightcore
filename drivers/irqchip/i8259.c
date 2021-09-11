/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i8259"

#include <mm.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/irqchip/i8259.h>

#include <asm/io.h>

#define PIC1_BASE   0x20
#define PIC2_BASE   0xa0
#define PIC_NR      2
#define PIC(nr)     ((nr) / 8)
#define IRQ(nr)     ((nr) % 8)

static resource_size_t pic_base[PIC_NR] = {PIC1_BASE, PIC2_BASE};

static inline uint8_t pic_inb(int pic, int reg)
{
    return inb(pic_base[pic] + reg);
}

static inline void pic_outb(int pic, int reg, uint8_t val)
{
    outb(pic_base[pic] + reg, val);
}

static state pic_irq_pass(struct irqchip_device *idev, irqnr_t vector)
{
    uint8_t val = pic_inb(PIC(vector), I8259_DATA);
    val &= ~BIT(IRQ(vector));
    pic_outb(PIC(vector), I8259_DATA, val);
    return -ENOERR;
}

static state pic_irq_mask(struct irqchip_device *idev, irqnr_t vector)
{
    uint8_t val = pic_inb(PIC(vector), I8259_DATA);
    val |= BIT(IRQ(vector));
    pic_outb(PIC(vector), I8259_DATA, val);
    return -ENOERR;
}

static struct irqchip_ops pic_ops = {
    .pass = pic_irq_pass,
    .mask = pic_irq_mask,
};

static void i8250_hw_init(void)
{
    pic_outb(0, I8259_DATA, 0xff);

    /* starts the initialization sequence */
    pic_outb(0, I8259_CMD,  ICW1_INIT | ICW1_ICW4);
    /* ICW2: PIC vector offset */
    pic_outb(0, I8259_DATA, IRQ_EXTERNAL);
    /* ICW3: tell Master PIC that there is a slave PIC at IRQ2 */
    pic_outb(0, I8259_DATA, 1 << PIC1_IRQ2);
    /* ICW4: Auto EOI */
    pic_outb(0, I8259_DATA, ICW4_8086 | ICW4_AUTO);

    pic_outb(1, I8259_CMD,  ICW1_INIT | ICW1_ICW4);
    pic_outb(1, I8259_DATA, IRQ_EXTERNAL + 8);
    pic_outb(1, I8259_DATA, 2);
    pic_outb(1, I8259_DATA, ICW4_8086 | ICW4_AUTO);

    pic_outb(0, I8259_DATA, ~PIC1_IRQ2);
    pic_outb(1, I8259_DATA, 0xff);
}

static state i8259_probe(struct platform_device *pdev)
{
    struct irqchip_device *irqchip;

    irqchip = dev_kzalloc(&pdev->device, sizeof(*irqchip), GFP_KERNEL);
    if(!irqchip)
        return -ENOMEM;

    i8250_hw_init();
    irqchip->ops = &pic_ops;
    irqchip->dt_node = pdev->dt_node;
    return irqchip_register(irqchip);
}

static struct dt_device_id i8259_id[] = {
    { .compatible = "intel,i8259" },
    { }, /* NULL */
};

static struct platform_driver i8259_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = i8259_id,
    .probe = i8259_probe,
};

static state i8259_init(void)
{
    return platform_driver_register(&i8259_driver);
}
irqchip_initcall(i8259_init);
