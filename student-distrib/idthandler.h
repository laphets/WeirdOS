#ifndef _TRAP_H
#define _TRAP_H

#include "lib.h"

/**
 * The defination for trap handler
 */

/**
    0x00	Division by zero
    0x01	Single-step interrupt (see trap flag)
    0x02	NMI
    0x03	Breakpoint (callable by the special 1-byte instruction 0xCC, used by debuggers)
    0x04	Overflow
    0x05	Bounds
    0x06	Invalid Opcode
    0x07	Coprocessor not available
    0x08	Double fault
    0x09	Coprocessor Segment Overrun (386 or earlier only)
    0x0A	Invalid Task State Segment
    0x0B	Segment not present
    0x0C	Stack Fault
    0x0D	General protection fault
    0x0E	Page fault
    0x0F	reserved
    0x10	Math Fault
    0x11	Alignment Check
    0x12	Machine Check
    0x13	SIMD Floating-Point Exception
    0x14	Virtualization Exception
    0x15	Control Protection Exception
*/
void TRAP_0();
void TRAP_1();
void TRAP_2();
void TRAP_3();
void TRAP_4();
void TRAP_5();
void TRAP_6();
void TRAP_7();
void TRAP_8();
void TRAP_9();
void TRAP_A();
void TRAP_B();
void TRAP_C();
void TRAP_D();
void TRAP_E();
void TRAP_F();
void TRAP_10();
void TRAP_11();
void TRAP_12();
void TRAP_13();
void TRAP_14();
void TRAP_15();

/**
 * The defination for interrupt handler
 */
extern void INTERRUPT_20();    /* For pit */
extern void INTERRUPT_21();    /* For keyboard */
extern void INTERRUPT_28();    /* For RTC */
extern void INTERRUPT_2C();    /* For Mouse */

/**
 * The defination for syscall
 */
extern void SYSCALL_80();


#endif
