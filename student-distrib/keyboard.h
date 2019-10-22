#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "keyboard.h"
#include "lib.h"
#include "types.h"
#include "i8259.h"

#define KEYBOARD_IRQ 0x1
#define KEYBOARD_PORT 0x60
#define CAPS_LOCK 0x3A

unsigned uint8_t keyboard_flag;

/**
 * Translate scancode into char
 */
unsigned char scancode2char(uint8_t scancode);

/**
 * Interrupt handler for keyboard, will be called in idtwrapper.S
 */
void keyboard_handler();

/**
 * Routine to init keyboard
 */
void init_keyboard();

#endif //MP3_GROUP_42_KEYBOARD_H
