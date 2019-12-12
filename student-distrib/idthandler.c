#include "idt.h"

void show_msg() {
    printf_error("If this is the first time you've seen this Stop error screen, \nrestart your computer. If this screen appears again, follow \nthese steps:\n\n");
    printf_error("Check to make sure any new hardware or software is properly installed.\nIf this is a new installation, ask your hardware or software manufacturer\nfor any mp3 updates you might need.\n\n");
    printf_error("If problems continue, disable or remove any newly installed hardware\nor software. Disable BIOS memory options such as caching or shadowing.\nIf you need to use safe mode to remove or disable components, restart\nyour computer, press F8 to select Advanced Startup Options, and then\nselect Safe Mode.\n\n");
}

void TRAP_0(uint32_t code, uint32_t eip) {
    blue_screen();
    printf_error("0x00 DIVISION_BY_ZERO\n\n");
    show_msg();
    printf_error("Technical Information:\n\n***  CODE: 0x%x  STOP: 0x%x ***\n", code, eip);
    while(1) {};
}
void TRAP_1() {
    printf("0x01: Single-step interrupt (see trap flag)\n");
}
void TRAP_2() {
    printf("0x02: NMI\n");
}
void TRAP_3() {
    printf("0x03: Breakpoint (callable by the special 1-byte instruction 0xCC, used by debuggers)\n");
}
void TRAP_4() {
    printf("0x04: Overflow\n");
}
void TRAP_5() {
    printf("0x05: Bounds\n");
}
void TRAP_6() {
    printf("0x06: Invalid Opcode\n");
}
void TRAP_7() {
    printf("0x07: Coprocessor not available\n");
}
void TRAP_8() {
    printf("0x08: Double fault\n");
}
void TRAP_9() {
    printf("0x09: Coprocessor Segment Overrun (386 or earlier only)\n");
}
void TRAP_A() {
    printf("0x0A: Invalid Task State Segment\n");
}
void TRAP_B() {
    printf("0x0B: Segment not present\n");
}
void TRAP_C() {
    printf("0x0C: Stack Fault\n");
}
void TRAP_D() {
    uint32_t* esp;
    asm volatile("                  \n\
            movl %%esp, %0           \n\
            "
    :  "=r"(esp)
    :
    : "esp");
    if(gui_enabled) {
        render_string(20, 20, "0x0E PAGE_FAULT | UNIT_TESTING_FAIL\n", 0);
    } else {
        printf("0x0D: General protection fault\n");
        printf("esp: 0x%x, [esp]: 0x%x, [esp+4]: 0x%x, [esp+8]: 0x%x, [esp-4]: 0x%x\n", esp, *esp, *(esp+1), *(esp+2), *(esp-1));
    }
}
void TRAP_E(registers_trap_error_t registers) {
    kprintf("TRAPE: code: 0x%x eip: 0x%x, esp: 0x%x\n", registers.code, registers.eip, registers.esp);
    if(registers.eip >= USER_VM_START) {
        kprintf("user space error: 0x%x\n", registers.code);
        /* We should do halt here then */
        halt(1);
        return;
    }

    if(gui_enabled) {
        char message[200];
        static char header[] = "If this is the first time you've seen this Stop error screen, restart your computer. \nIf this screen appears again, follow these steps:"
                                  "Check to make sure any new hardware or software is properly installed. If this is a new installation, ask your hardware or software manufacturer for any ECE391 OS updates you might need."
                                  "If problems continue, disable or remove any newly installed hardware or software. Disable BIOS memory options such as caching or shadowing. If you need to use safe mode to remove or disable components, restart your computer, press F8 to select Advanced Startup Options, and then select Safe Mode.\n\n\n";
        sprintf((uint8_t*)message, "%s0x0E PAGE_FAULT\nTechnical Information:\n\n*** CODE: 0x%x  LOCATION: 0x%x  MEMORY ACCESS LOCATION: 0x%x HEAP END: 0x%x ***\n", header, registers.code, registers.eip, registers.eax, heap.end_addr);

        gui_error(message);
    } else {
        kprintf("0x0E PAGE_FAULT | UNIT_TESTING_FAIL\n");
        kprintf("Technical Information:\n\n*** CODE: 0x%x  LOCATION: 0x%x  MEMORY ACCESS LOCATION: 0x%x ***\n", registers.code, registers.eip, registers.eax);
    }


    cli();
    while(1) {};
}
void TRAP_F() {
    printf("0x0F: reserved\n");
}
void TRAP_10() {
    printf("0x10: Math Fault\n");
}
void TRAP_11() {
    printf("0x11: Alignment Check\n");
}
void TRAP_12() {
    printf("0x12: Machine Check\n");
}
void TRAP_13() {
    printf("0x13: SIMD Floating-Point Exception\n");
}
void TRAP_14() {
    printf("0x14: Virtualization Exception\n");
}
void TRAP_15() {
    printf("0x15: Control Protection Exception\n");
}
