#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "keyboard.h"
#include "lib.h"

/* Macros for the terminal buffer */
#define MAX_TERMINAL_BUF_SIZE 128

/* Terminal buffer data */
char terminal_buf[MAX_TERMINAL_BUF_SIZE];
uint32_t terminal_buf_size;
uint8_t enter_pressed_flag;

/* Driver functions */
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);

#endif
