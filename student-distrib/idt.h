#ifndef _IDT_H
#define _IDT_H

#include "lib.h"
#include "types.h"
#include "x86_desc.h"
#include "idthandler.h"

#define PIT_IDT 0x20
#define KEYBOARD_IDT 0x21
#define RTC_IDT 0x28
#define MOUSE_IDT 0x2C

#define SYSCALL_IDT 0x80

typedef void (*idt_handler_t)();

void init_idt();

typedef struct registers {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers_t;

#endif
