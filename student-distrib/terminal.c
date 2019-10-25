#include "terminal.h"

/*
 * int open();
 * Inputs: none
 * Return: 0
 * Function: initializes variables for terminal
 */
int open() {
    last_read_pos = 0;
    terminal_buf_size = 0;
    return 0;
}

/*
 * int close();
 * Inputs: none
 * Return: 0
 * Function: returns 0s
 */
int close() {
    return 0;
}

/*
 * int read();
 * Inputs: none
 * Return: Number of bytes read
 * Function: Copies the keyboard buffer to the terminal buffer
 */
int read() {
    while(last_read_pos != keyboard_buf_pos) {
        terminal_buf[last_read_pos] = keyboard_buf[last_read_pos];
        last_read_pos++;
    }
    terminal_buf_size = last_read_pos;
    return last_read_pos;
}

/*
 * int write();
 * Inputs: none
 * Return: Number of bytes written, or -1 in none
 * Function: writes the terminal buffer to video
 *          memory to display it.
 */
int write() {
    int output_byte_count = 0;
    last_read_pos = 0;
    while (last_read_pos != terminal_buf_size) {
        if ((terminal_buf[last_read_pos] == '\n') ||
            (terminal_buf[last_read_pos] == '\r')) {
            shift_video_up(1);
        } else if (last_read_pos == MAX_LETTERS_IN_ROW) {
            shift_video_up(1);
            putc(terminal_buf[last_read_pos]);
        } else {
            putc(terminal_buf[last_read_pos]);
        }
        last_read_pos++;
        output_byte_count++;
    }
    last_read_pos = 0;
    return (output_byte_count > 0) ? output_byte_count : -1;

    
}
