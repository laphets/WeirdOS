#include "idt.h"

void SET_IDT_TRAP_GATE(uint8_t index, const char* name, idt_handler_t handler) {
    /* Set for callback function */
    SET_IDT_ENTRY(idt[index], handler);
    /* Set for segment selector */
    idt[index].seg_selector = KERNEL_CS;
    idt[index].reserved4 = 0;
    /* 80386 32-bit trap gate: 0xF */
    idt[index].reserved3 = 1;
    idt[index].reserved2 = 1;
    idt[index].reserved1 = 1;
    idt[index].size = 1;
    /* Storage Segment*/
    idt[index].reserved0 = 0;
    /* DPL */
    idt[index].dpl = 0;
    /* Present */
    idt[index].present = 1;
}

void SET_IDT_INTERRUPT_GATE(uint8_t index, const char* name, idt_handler_t handler) {
    /* Set for callback function */
    SET_IDT_ENTRY(idt[index], handler);
    /* Set for segment selector */
    idt[index].seg_selector = KERNEL_CS;
    idt[index].reserved4 = 0;
    /* 80386 32-bit trap gate: 0xF */
    idt[index].reserved3 = 0;
    idt[index].reserved2 = 1;
    idt[index].reserved1 = 1;
    idt[index].size = 1;
    /* Storage Segment*/
    idt[index].reserved0 = 0;
    /* DPL */
    idt[index].dpl = 0;
    /* Present */
    idt[index].present = 1;
}

void SET_IDT_SYSCALL_GATE(uint8_t index, const char* name, idt_handler_t handler) {
    /* Set for callback function */
    SET_IDT_ENTRY(idt[index], handler);
    /* Set for segment selector */
    idt[index].seg_selector = KERNEL_CS;
    idt[index].reserved4 = 0;
    /* 80386 32-bit trap gate: 0xF */
    idt[index].reserved3 = 0;
    idt[index].reserved2 = 0;
    idt[index].reserved1 = 0;
    idt[index].size = 0;
    /* Storage Segment*/
    idt[index].reserved0 = 1;
    /* DPL */
    idt[index].dpl = 3;
    /* Present */
    idt[index].present = 1;
}

void init_trap() {
    SET_IDT_TRAP_GATE(0x0, "", TRAP_0);
    SET_IDT_TRAP_GATE(0x1, "", TRAP_1);
    SET_IDT_TRAP_GATE(0x2, "", TRAP_2);
    SET_IDT_TRAP_GATE(0x3, "", TRAP_3);
    SET_IDT_TRAP_GATE(0x4, "", TRAP_4);
    SET_IDT_TRAP_GATE(0x5, "", TRAP_5);
    SET_IDT_TRAP_GATE(0x6, "", TRAP_6);
    SET_IDT_TRAP_GATE(0x7, "", TRAP_7);
    SET_IDT_TRAP_GATE(0x8, "", TRAP_8);
    SET_IDT_TRAP_GATE(0x9, "", TRAP_9);
    SET_IDT_TRAP_GATE(0xA, "", TRAP_A);
    SET_IDT_TRAP_GATE(0xB, "", TRAP_B);
    SET_IDT_TRAP_GATE(0xC, "", TRAP_C);
    SET_IDT_TRAP_GATE(0xD, "", TRAP_D);
    SET_IDT_TRAP_GATE(0xE, "", TRAP_E);
    SET_IDT_TRAP_GATE(0xF, "", TRAP_F);
    SET_IDT_TRAP_GATE(0x10, "", TRAP_10);
    SET_IDT_TRAP_GATE(0x11, "", TRAP_11);
    SET_IDT_TRAP_GATE(0x12, "", TRAP_12);
    SET_IDT_TRAP_GATE(0x13, "", TRAP_13);
    SET_IDT_TRAP_GATE(0x14, "", TRAP_14);
    SET_IDT_TRAP_GATE(0x15, "", TRAP_15);
}
void init_interrupt() {
    SET_IDT_INTERRUPT_GATE(KEYBOARD_IDT, "keyboard", INTERRUPT_21);
    SET_IDT_INTERRUPT_GATE(RTC_IDT, "rtc", INTERRUPT_28);
}
void init_syscall() {
    SET_IDT_SYSCALL_GATE(SYSCALL_IDT, "syscall", SYSCALL_80);
}

void init_idt() {
    init_trap();
    init_interrupt();
    init_syscall();

    lidt(idt_desc_ptr);
    printf("Initialize IDT done\n");
}

