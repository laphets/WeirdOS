#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "keyboard.h"
#include "lib.h"

/* Macros for the terminal buffer */
#define MAX_TERMINAL_BUF_SIZE 128
#define MAX_LETTERS_IN_ROW 80
#define MAX_NUM_ROWS 25

/* Terminal buffer data */
unsigned char terminal_buf[MAX_TERMINAL_BUF_SIZE];
uint8_t last_read_pos;
uint8_t terminal_buf_size;

/* Driver functions */
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);

#endif
