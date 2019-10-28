/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */
uint32_t flags;


/* Initialize the 8259 PIC */
void i8259_init(void) {
    /* First mask all IRQ of i8259 */
    outb(0xFF, PIC_MASTER_IMR);
    outb(0xFF, PIC_SLAVE_IMR);

    /* Init master PIC */
    outb(ICW1, PIC_MASTER_CMD); /* Send 0x11 to select init */
    outb(ICW2_MASTER, PIC_MASTER_IMR); /* IR0-7 map to 0x20-0x27 */
    outb(ICW3_MASTER, PIC_MASTER_IMR); /* Set for the cascade IR(here is 2) */
    outb(ICW4, PIC_MASTER_IMR);

    /* Init slave PIC */
    outb(ICW1, PIC_SLAVE_CMD);  /* Send 0x11 to select init */
    outb(ICW2_SLAVE, PIC_SLAVE_IMR);    /* IR0-7 map to 0x28-0x2f */
    outb(ICW3_SLAVE, PIC_SLAVE_IMR);    /* On IR2 for master */
    outb(ICW4, PIC_SLAVE_IMR);

    /* Remask */
    outb(0xFF, PIC_MASTER_IMR);
    outb(0xFF, PIC_SLAVE_IMR);

    /* Enable irq2 */
    enable_irq(2);

    printf("Inited with i8259\n");
}


/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t data;

    if (irq_num < 8) {
        port = PIC_MASTER_IMR;
    } else {
        port = PIC_SLAVE_IMR;
        irq_num -= 8;
    }
    data = inb(port) & ~(1 << irq_num);
    outb(data, port);
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t data;

    if (irq_num < 8) {
        port = PIC_MASTER_IMR;
    } else {
        port = PIC_SLAVE_IMR;
        irq_num -= 8;
    }
    data = inb(port) | (1 << irq_num);
    outb(data, port);
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if(irq_num >= 8) {
        outb(EOI | (irq_num - 8), PIC_SLAVE_CMD);
        outb(EOI | 2, PIC_MASTER_CMD);
    } else {
        outb(EOI | irq_num, PIC_MASTER_CMD);
    }
}
