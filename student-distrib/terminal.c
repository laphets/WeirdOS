#include "terminal.h"

/*
 * int32_t open(const uint8_t* filename);
 * Inputs: none
 * Return: 0
 * Function: initializes variables for terminal
 */
int32_t terminal_open(const uint8_t* filename) {
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
int32_t terminal_close(int32_t fd) {
    return 0;
}

/*
 * int32_t read(int32_t fd, void* buf, int32_t nbytes);
 * Inputs: none
 * Return: Number of bytes read
 * Function: Copies the keyboard buffer to the terminal buffer
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    if (buf != NULL) {   
        enter_pressed_flag = 0;
        terminal_buf_size = 0;
        while (!enter_pressed_flag) {}
        uint32_t bytes_to_copy = nbytes < terminal_buf_size ? nbytes : terminal_buf_size;
        strncpy((int8_t*)buf, terminal_buf, bytes_to_copy);
        return bytes_to_copy;
    }
    return -1;
}

/*
 * int32_t write(int32_t fd, const void* buf, int32_t nbytes);
 * Inputs: none
 * Return: Number of bytes written, or -1 in none
 * Function: writes the terminal buffer to video
 *          memory to display it.
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    int i;
    int length_of_print = 0;
    if (buf != NULL) {    
        for (i = 0; i < nbytes; i++) {
            if(((char*)buf)[i] == '\b' && length_of_print > 0) {
                length_of_print--;
                putc(((char*)buf)[i]);
            } else if (((char*)buf)[i] != '\b') {
                length_of_print++;
                putc(((char*)buf)[i]);
            }
        }
        return length_of_print;
    }
    return -1;
}
