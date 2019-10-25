#include "terminal.h"

/*
 * int32_t open(const uint8_t* filename);
 * Inputs: none
 * Return: 0
 * Function: initializes variables for terminal
 */
int32_t open(const uint8_t* filename) {
    last_read_pos = 0;
    terminal_buf_size = 0;
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
    char last_char = buf[0];
    while (last_char != '\n') {
        while(last_read_pos != keyboard_buf_pos) {
            last_char = keyboard_buf[last_read_pos];
            terminal_buf[last_read_pos] = last_char;
            last_read_pos++;
        }
    }


    
    terminal_buf_size = last_read_pos;
    return last_read_pos;
}

/*
 * int32_t write(int32_t fd, const void* buf, int32_t nbytes);
 * Inputs: none
 * Return: Number of bytes written, or -1 in none
 * Function: writes the terminal buffer to video
 *          memory to display it.
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    int output_byte_count = 0;
    last_read_pos = 0;
    while (last_read_pos != terminal_buf_size) {
        if ((terminal_buf[last_read_pos] == '\n') ||
            (terminal_buf[last_read_pos] == '\r')) {
            if (get_screen_y() >= MAX_NUM_ROWS-1) {
                shift_video_up(1);
                reset_screen_x();
            } else {
                putc(terminal_buf[last_read_pos]);
            }
        } else if (last_read_pos == MAX_LETTERS_IN_ROW) {
            if (get_screen_y() >= MAX_NUM_ROWS-1) {
                shift_video_up(1);
                reset_screen_x();
            } else {
                putc('\n');
            }
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
