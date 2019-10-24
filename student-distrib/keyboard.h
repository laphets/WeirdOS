#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "keyboard.h"
#include "lib.h"
#include "types.h"
#include "i8259.h"

#define KEYBOARD_IRQ 0x1
#define KEYBOARD_PORT 0x60
#define KEYBOARD_CAPS_LOCK_PRESS 0x3A
#define KEYBOARD_LEFT_SHIFT_PRESS 0x2A
#define KEYBOARD_RIGHT_SHIFT_PRESS 0x36
#define KEYBOARD_LEFT_SHIFT_RELEASE 0xAA
#define KEYBOARD_RIGHT_SHIFT_RELEASE 0xB6
#define KEYBOARD_LEFT_CTRL_PRESS 0x1D
#define KEYBOARD_RIGHT_CTRL_PRESS 0xE0 | 0x1D
#define KEYBOARD_LEFT_CTRL_RELEASE 0x9D
#define KEYBOARD_RIGHT_CTRL_RELEASE 0xE0 | 0x9D
#define CAPS_LOCK_BIT_MASK 0x1
#define LEFT_SHIFT_BIT_MASK 0x2
#define RIGHT_SHIFT_BIT_MASK 0x4
#define LEFT_CTRL_MASK 0x8
#define RIGHT_CTRL_MASK 0x10
#define KEYBOARD_BUF_MAX_POS 127

uint8_t keyboard_flag;

unsigned char keyboard_buf[129];
uint8_t keyboard_buf_pos;

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
