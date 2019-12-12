#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "keyboard.h"
#include "lib.h"
#include "types.h"
#include "i8259.h"
#include "terminal.h"
#include "scheduler.h"
#include "gui.h"


/* Macros for mapping keyboard scan codes */
#define KEYBOARD_IRQ 0x1
#define KEYBOARD_PORT 0x60
#define KEYBOARD_CAPS_LOCK_PRESS 0x3A
#define KEYBOARD_LEFT_SHIFT_PRESS 0x2A
#define KEYBOARD_RIGHT_SHIFT_PRESS 0x36
#define KEYBOARD_LEFT_SHIFT_RELEASE 0xAA
#define KEYBOARD_RIGHT_SHIFT_RELEASE 0xB6
#define KEYBOARD_CTRL_PRESS 0x1D
#define KEYBOARD_CTRL_RELEASE 0x9D
#define KEYBOARD_ALT_PRESS 0x38
#define KEYBOARD_ALT_RELEASE 0xB8

#define KEYBOARD_KEY_F1 0x3B
#define KEYBOARD_KEY_F2 0x3C
#define KEYBOARD_KEY_F3 0x3D
#define KEYBOARD_KEY_F4 0x3E
#define KEYBOARD_KEY_F5 0x3F

#define KEYBOARD_UP_ARROW 0x48
#define KEYBOARD_DOWN_ARROW 0x50


/* For MacOS keyboard mapping */
#define KEYBOARD_OPTION_PRESS 56
#define KEYBOARD_OPTION_RELEASE 184

/* Macros for mapping bit masks to keep track of special keys */
#define CAPS_LOCK_BIT_MASK 0x1
#define SHIFT_BIT_MASK 0x2
#define CTRL_BIT_MASK 0x4
#define ALT_BIT_MASK 0x8

/* Macros for the keyboard buffer */
#define KEYBOARD_BUF_MAX_SIZE 128
#define MAX_LETTERS_IN_ROW 80
#define MAX_NUM_ROWS 25
#define NUM_KEYBOARD_BUFS 5

/* Macros for scancode and ascii table size info */
#define TOTAL_NUM_SCANCODES 256
#define TOTAL_NUM_ASCII_CODES 128

uint8_t keyboard_flag;

char keyboard_buf[KEYBOARD_BUF_MAX_SIZE];
uint32_t keyboard_buf_size;


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
