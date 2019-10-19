#ifndef _IDT_H
#define _IDT_H

#include "lib.h"
#include "types.h"
#include "x86_desc.h"
#include "idthandler.h"

#define KEYBOARD_IDT 0x21
#define RTC_IDT 0x28
#define SYSCALL_IDT 0x80

typedef void (*idt_handler_t)();

void init_idt();

#endif
