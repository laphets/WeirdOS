#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "keyboard.h"
#include "lib.h"
#include "vfs.h"
#include "types.h"
#include "task.h"
#include "syscall.h"
#include "paging.h"

/* Macros for the terminal buffer */
#define MAX_TERMINAL_BUF_SIZE 128

#define TYPE_SWITCH_ACTIVE_TERMINAL 0
#define TYPE_SWITCH_RUNNING_TERMINAL 1

/* Terminal buffer data */
/* char terminal_buf[MAX_TERMINAL_BUF_SIZE]; */
char* terminal_buf; /* This pointer will point to the char array stored in terminal_t */

volatile uint32_t terminal_buf_size;
//volatile uint8_t enter_pressed_flag;

/* Driver functions */
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

file_operator_t terminal_operator;

void init_terminal_operator();


#define MAX_TERMINAL_NUM 5
typedef struct terminal {
    uint32_t tid; /* terminal id */
    int8_t present;
    char terminal_buf[MAX_TERMINAL_BUF_SIZE]; /* For task switch */
    int screen_x, screen_y;
    uint8_t* inactive_video_mem;
    uint32_t current_task_pid;  /* current running task pid */
    uint32_t root_task_pid; /* the root shell pid */

    uint32_t unshown_vm_addr;

    volatile uint8_t enter_pressed_flag;
} terminal_t;
terminal_t terminal_list[MAX_TERMINAL_NUM];

volatile int32_t current_active_terminal;

void init_terminal();
void switch_terminal(uint32_t tid, uint8_t type);
void launch_terminal();

#endif
