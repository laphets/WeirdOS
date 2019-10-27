#include "terminal.h"

/*
 * int32_t open(const uint8_t* filename);
 * Inputs: none
 * Return: 0
 * Function: initializes variables for terminal
 */
int32_t open(const uint8_t* filename) {
    terminal_buf_size = 0;
    enter_pressed_flag = 0;
    return 0;
}

/*
 * int32_t close(int32_t fd);
 * Inputs: none
 * Return: 0
 * Function: returns 0s
 */
int32_t close(int32_t fd) {
    return 0;
}

/*
 * int32_t read(int32_t fd, void* buf, int32_t nbytes);
 * Inputs: none
 * Return: Number of bytes read
 * Function: Copies the keyboard buffer to the terminal buffer
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes) {
    enter_pressed_flag = 0;
    terminal_buf_size = 0;
    while (!enter_pressed_flag) {}
    strncpy((int8_t*)buf, terminal_buf, terminal_buf_size);
    return terminal_buf_size;
}

/*
 * int32_t write(int32_t fd, const void* buf, int32_t nbytes);
 * Inputs: none
 * Return: Number of bytes written, or -1 in none
 * Function: writes the terminal buffer to video
 *          memory to display it.
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        if (((char*)buf)[i] == '\n' || ((char*)buf)[i] == '\r') {
            if (get_screen_y() >= MAX_NUM_ROWS-1) {
                shift_video_up(1);
                continue;
            } else {
                putc('\n');
            }
        } else if (i == MAX_LETTERS_IN_ROW) {
            if (get_screen_y() >= MAX_NUM_ROWS - 1) {
                shift_video_up(1);
            }
            else {
                putc('\n');
            }
        }
        putc(((char*)buf)[i]);
    }
    return nbytes;
}
