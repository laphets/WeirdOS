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
    char last_char = keyboard_buf[0];
    int keyboard_read_pos;
    while (last_char != '\n') {
        keyboard_read_pos = 0;
        while(keyboard_read_pos != keyboard_buf_size) {
            last_char = keyboard_buf[keyboard_buf_size];
            if ((last_char == '\b') && (terminal_buf_size > 0)) {
                terminal_buf_size--;
            } else {
                buf[terminal_buf_size] = last_char;
                terminal_buf[terminal_buf_size] = last_char;
                terminal_buf_size++;
            }
            if (last_char == '\n' || last_char == '\r') {
                if (get_screen_y() >= MAX_NUM_ROWS - 1) {
                    shift_video_up(1);
                    reset_screen_x();
                } else {
                    putc('\n');
                }
            } else {
                if (terminal_buf_size == MAX_LETTERS_IN_ROW) {
                    if (get_screen_y() >= MAX_NUM_ROWS - 1) {
                        shift_video_up(1);
                        reset_screen_x();
                    }
                    else {
                        putc('\n');
                    }
                }
                putc(keyboard_buf[keyboard_read_pos]);
            }
            keyboard_read_pos++;
        }
        keyboard_buf_size = 0;
    }
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
        if (buf[i] == '\n' || buf[i] == '\r') {
            if (get_screen_y() >= MAX_NUM_ROWS-1) {
                shift_video_up(1);
                reset_screen_x();
                continue;
            } else {
                putc('\n');
            }
        } else if (i == MAX_LETTERS_IN_ROW) {
            if (get_screen_y() >= MAX_NUM_ROWS - 1) {
                shift_video_up(1);
                reset_screen_x();
            }
            else {
                putc('\n');
            }
        }
        putc(buf[i]);
    }
    return nbytes;
}
