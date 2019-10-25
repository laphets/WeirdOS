#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "keyboard.h"
#include "lib.h"

/* Macros for the terminal buffer */
#define MAX_TERMINAL_BUF_SIZE 128
#define MAX_LETTERS_IN_ROW 80

/* Terminal buffer data */
unsigned char terminal_buf[MAX_TERMINAL_BUF_SIZE];
uint8_t last_read_pos;
uint8_t terminal_buf_size;

/* Driver functions */
int open();
int close();
int read();
int write();

#endif
