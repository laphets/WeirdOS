#include "idt.h"

void TRAP_0() {
    printf("0x00: Division by zero\n");
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
    printf("0x0D: General protection fault\n");
}
void TRAP_E() {
    printf("0x0E: Page fault\n");
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
