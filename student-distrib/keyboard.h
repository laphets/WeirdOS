#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "keyboard.h"
#include "lib.h"
#include "types.h"
#include "i8259.h"

#define KEYBOARD_IRQ 0x1
#define KEYBOARD_PORT 0x60



void keyboard_handler();
void init_keyboard();

#endif //MP3_GROUP_42_KEYBOARD_H
