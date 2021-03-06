/* lib.c - Some basic library functions (printf, strlen, etc.)
 * vim:ts=4 noexpandtab */

#include "lib.h"

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25
#define ATTRIB      0x7

static int screen_x;
static int screen_y;
static char* video_mem = (char *)VIDEO;

/* void clear(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears video memory */
void clear(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
    }
    reset_cursor_pos();
}

/**
 * Clear for a virtual memory buffer
 * @param video_mem video_memory pointer
 */
void clear_vm_buffer(char* video_mem) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
    }
}

/*
 * From: https://wiki.osdev.org/Text_Mode_Cursor
 */
void update_cursor_pos(char* video_mem_local, int x, int y) {
    if(video_mem_local != video_mem)
        return;
	uint16_t pos = y * NUM_COLS + x;
	outb(0x0F, 0x3D4);
	outb((uint8_t) (pos & 0xFF), 0x3D5);
	outb(0x0E, 0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF), 0x3D5);
}

/* void reset_cursos_pos();
 * Inputs: none
 * Return Value: none
 * Function: reset cursor position to top left */
void reset_cursor_pos() {
    screen_y = 0;
    screen_x = 0;
    update_cursor_pos(video_mem, screen_x, screen_y);
}

/**
 * Set for the current cursor position
 * @param x the x position we want to set
 * @param y the y position we want to set
 */
void set_cursor_pos(int x, int y) {
    screen_y = y;
    screen_x = x;
    update_cursor_pos(video_mem, screen_x, screen_y);
}

/**
 * Deprecated!, print a char for error message
 * @param c
 */
void putc_error(uint8_t c) {
    if(c == '\n' || c == '\r') {
        screen_y = (screen_y + 1) % NUM_ROWS;
        screen_x = 0;
    } else {
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = 0x1F;
        screen_x++;
        screen_x %= NUM_COLS;
        screen_y = (screen_y + (screen_x / NUM_COLS)) % NUM_ROWS;
    }
}
/**
 * Deprecated! print an error string for error message
 * @param s
 */
int32_t puts_error(int8_t* s) {
    register int32_t index = 0;

    while (s[index] != '\0') {
        putc_error(s[index]);
        index++;
    }
    return index;
}
/**
 * Deprecated!, print a fotmat string for error message
 * @param format
 */
int32_t printf_error(int8_t *format, ...) {

    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
            {
                int32_t alternate = 0;
                buf++;

                format_char_switch:
                /* Conversion specifiers */
                switch (*buf) {
                    /* Print a literal '%' character */
                    case '%':
                        putc('%');
                        break;

                        /* Use alternate formatting */
                    case '#':
                        alternate = 1;
                        buf++;
                        /* Yes, I know gotos are bad.  This is the
                         * most elegant and general way to do this,
                         * IMHO. */
                        goto format_char_switch;

                        /* Print a number in hexadecimal form */
                    case 'x':
                    {
                        int8_t conv_buf[64];
                        if (alternate == 0) {
                            itoa(*((uint32_t *)esp), conv_buf, 16);
                            puts_error(conv_buf);
                        } else {
                            int32_t starting_index;
                            int32_t i;
                            itoa(*((uint32_t *)esp), &conv_buf[8], 16);
                            i = starting_index = strlen(&conv_buf[8]);
                            while(i < 8) {
                                conv_buf[i] = '0';
                                i++;
                            }
                            puts_error(&conv_buf[starting_index]);
                        }
                        esp++;
                    }
                        break;

                        /* Print a number in unsigned int form */
                    case 'u':
                    {
                        int8_t conv_buf[36];
                        itoa(*((uint32_t *)esp), conv_buf, 10);
                        puts_error(conv_buf);
                        esp++;
                    }
                        break;

                        /* Print a number in signed int form */
                    case 'd':
                    {
                        int8_t conv_buf[36];
                        int32_t value = *((int32_t *)esp);
                        if(value < 0) {
                            conv_buf[0] = '-';
                            itoa(-value, &conv_buf[1], 10);
                        } else {
                            itoa(value, conv_buf, 10);
                        }
                        puts_error(conv_buf);
                        esp++;
                    }
                        break;

                        /* Print a single character */
                    case 'c':
                        putc_error((uint8_t) *((int32_t *)esp));
                        esp++;
                        break;

                        /* Print a NULL-terminated string */
                    case 's':
                        puts_error(*((int8_t **)esp));
                        esp++;
                        break;

                    default:
                        break;
                }

            }
                break;

            default:
                putc_error(*buf);
                break;
        }
        buf++;
    }
    return (buf - format);
}
/**
 * Deprecated! Show a blue screen when fault occurs
 * @param c
 */
void blue_screen() {
    cli();
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = 0x1F;
    }
    screen_x = 0;
    screen_y = 0;
    printf_error("\nA problem has been detect and our OS has been shut down to prevent damage\nto your computer.\n\n");
}

/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output. */
int32_t printf(int8_t *format, ...) {

    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
                {
                    int32_t alternate = 0;
                    buf++;

format_char_switch:
                    /* Conversion specifiers */
                    switch (*buf) {
                        /* Print a literal '%' character */
                        case '%':
                            putc('%');
                            break;

                        /* Use alternate formatting */
                        case '#':
                            alternate = 1;
                            buf++;
                            /* Yes, I know gotos are bad.  This is the
                             * most elegant and general way to do this,
                             * IMHO. */
                            goto format_char_switch;

                        /* Print a number in hexadecimal form */
                        case 'x':
                            {
                                int8_t conv_buf[64];
                                if (alternate == 0) {
                                    itoa(*((uint32_t *)esp), conv_buf, 16);
                                    puts(conv_buf);
                                } else {
                                    int32_t starting_index;
                                    int32_t i;
                                    itoa(*((uint32_t *)esp), &conv_buf[8], 16);
                                    i = starting_index = strlen(&conv_buf[8]);
                                    while(i < 8) {
                                        conv_buf[i] = '0';
                                        i++;
                                    }
                                    puts(&conv_buf[starting_index]);
                                }
                                esp++;
                            }
                            break;

                        /* Print a number in unsigned int form */
                        case 'u':
                            {
                                int8_t conv_buf[36];
                                itoa(*((uint32_t *)esp), conv_buf, 10);
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a number in signed int form */
                        case 'd':
                            {
                                int8_t conv_buf[36];
                                int32_t value = *((int32_t *)esp);
                                if(value < 0) {
                                    conv_buf[0] = '-';
                                    itoa(-value, &conv_buf[1], 10);
                                } else {
                                    itoa(value, conv_buf, 10);
                                }
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a single character */
                        case 'c':
                            putc((uint8_t) *((int32_t *)esp));
                            esp++;
                            break;

                        /* Print a NULL-terminated string */
                        case 's':
                            puts(*((int8_t **)esp));
                            esp++;
                            break;

                        default:
                            break;
                    }

                }
                break;

            default:
                putc(*buf);
                break;
        }
        buf++;
    }
    return (buf - format);
}

/**
 * Directly print a string into current video memory
 * @param s the string we want to print
 * @return number of char we ahd printed
 */
int32_t kputs(int8_t* s) {
    register int32_t index = 0;
    while (s[index] != '\0') {
        putc2buffer(video_mem, s[index]);
        index++;
    }
    return index;
}

/**
 * Directly print a format string into current video memory
 * @param format the format string container
 * @param ... other parameters
 * @return number that printed
 */
int32_t kprintf(int8_t *format, ...) {

    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
            {
                int32_t alternate = 0;
                buf++;

                format_char_switch:
                /* Conversion specifiers */
                switch (*buf) {
                    /* Print a literal '%' character */
                    case '%':
                        putc2buffer(video_mem,'%');
                        break;

                        /* Use alternate formatting */
                    case '#':
                        alternate = 1;
                        buf++;
                        /* Yes, I know gotos are bad.  This is the
                         * most elegant and general way to do this,
                         * IMHO. */
                        goto format_char_switch;

                        /* Print a number in hexadecimal form */
                    case 'x':
                    {
                        int8_t conv_buf[64];
                        if (alternate == 0) {
                            itoa(*((uint32_t *)esp), conv_buf, 16);
                            kputs(conv_buf);
                        } else {
                            int32_t starting_index;
                            int32_t i;
                            itoa(*((uint32_t *)esp), &conv_buf[8], 16);
                            i = starting_index = strlen(&conv_buf[8]);
                            while(i < 8) {
                                conv_buf[i] = '0';
                                i++;
                            }
                            kputs(&conv_buf[starting_index]);
                        }
                        esp++;
                    }
                        break;

                        /* Print a number in unsigned int form */
                    case 'u':
                    {
                        int8_t conv_buf[36];
                        itoa(*((uint32_t *)esp), conv_buf, 10);
                        kputs(conv_buf);
                        esp++;
                    }
                        break;

                        /* Print a number in signed int form */
                    case 'd':
                    {
                        int8_t conv_buf[36];
                        int32_t value = *((int32_t *)esp);
                        if(value < 0) {
                            conv_buf[0] = '-';
                            itoa(-value, &conv_buf[1], 10);
                        } else {
                            itoa(value, conv_buf, 10);
                        }
                        kputs(conv_buf);
                        esp++;
                    }
                        break;

                        /* Print a single character */
                    case 'c':
                        putc2buffer(video_mem, (uint8_t) *((int32_t *)esp));
                        esp++;
                        break;

                        /* Print a NULL-terminated string */
                    case 's':
                        kputs(*((int8_t **)esp));
                        esp++;
                        break;

                    default:
                        break;
                }

            }
                break;

            default:
                putc2buffer(video_mem, *buf);
                break;
        }
        buf++;
    }
    return (buf - format);
}

/* int32_t puts(int8_t* s);
 *   Inputs: int_8* s = pointer to a string of characters
 *   Return Value: Number of bytes written
 *    Function: Output a string to the console */
int32_t puts(int8_t* s) {
    register int32_t index = 0;
    while (s[index] != '\0') {
        putc(s[index]);
        index++;
    }
    return index;
}

/**
 * Put a char into video_mem_local buffer
 * @param video_mem_local a buffer for video memory, can be real or unshown
 * @param c the char we want to output
 */
void putc2buffer(char* video_mem_local, uint8_t c) {
    if(c == 0) {
        return;
    }
    int screen_x_local = screen_x;
    int screen_y_local = screen_y;
    uint8_t put_to_buffer = 0;

    if(init_ed == 1 && video_mem_local != video_mem && current_active_terminal != current_running_terminal) {
        /* In this case we should have own copy */
        terminal_t* terminal = &terminal_list[current_running_terminal];
        screen_x_local = terminal->screen_x;
        screen_y_local = terminal->screen_y;
        put_to_buffer = 1;
    }

    if(c == '\n' || c == '\r') {
        if (screen_y_local >= NUM_ROWS - 1) {
            shift_video_up(video_mem_local, 1);
            screen_x_local = 0;
            update_cursor_pos(video_mem_local, screen_x_local, screen_y_local);
        } else {
            screen_y_local = (screen_y_local + 1) % NUM_ROWS;
            screen_x_local = 0;
        }
        update_cursor_pos(video_mem_local, screen_x_local, screen_y_local);
    } else if (c == '\b') {
        if (screen_x_local > 0) {
            screen_x_local--;
        } else if(screen_y_local > 0) {
            screen_x_local = NUM_COLS - 1;
            screen_y_local--;
        }
        *(uint8_t *)(video_mem_local + ((NUM_COLS * screen_y_local + screen_x_local) << 1)) = ' ';
        *(uint8_t *)(video_mem_local + ((NUM_COLS * screen_y_local + screen_x_local) << 1) + 1) = ATTRIB;
    } else {
        *(uint8_t *)(video_mem_local + ((NUM_COLS * screen_y_local + screen_x_local) << 1)) = c;
        *(uint8_t *)(video_mem_local + ((NUM_COLS * screen_y_local + screen_x_local) << 1) + 1) = ATTRIB;
        screen_x_local++;
    }

    if (screen_x_local < NUM_COLS) {
        update_cursor_pos(video_mem_local, screen_x_local, screen_y_local);
    } else {
        if (screen_y_local >= NUM_ROWS - 1) {
            shift_video_up(video_mem_local, 1);
            screen_x_local = 0;
            update_cursor_pos(video_mem_local, screen_x_local, screen_y_local);
        } else {
            screen_y_local = (screen_y_local + 1) % NUM_ROWS;
            screen_x_local = 0;
        }
        update_cursor_pos(video_mem_local, screen_x_local, screen_y_local);
    }

    if(!put_to_buffer) {
        screen_x = screen_x_local;
        screen_y = screen_y_local;
        update_cursor_pos(video_mem, screen_x_local, screen_y_local);
    } else {
        terminal_t* terminal = &terminal_list[current_running_terminal];
        terminal->screen_x = screen_x_local;
        terminal->screen_y = screen_y_local;
    }
}

/* void putc(uint8_t c);
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Output a character to the console */
void putc(uint8_t c) {
    char* video_mem_local = video_mem;
    if(init_ed == 1 && current_active_terminal != -1) {
        /* Then we should put to current running's unshown buffer */
        if(/* current_running_terminal != 1 && */ terminal_list[current_running_terminal].present && current_running_terminal != current_active_terminal) {
            video_mem_local = (char*)terminal_list[current_running_terminal].unshown_vm_addr;
            /* kprintf("current_active: %d, current_running: %d\n", current_active_terminal, current_running_terminal); */
        }
    }
    putc2buffer(video_mem_local, c);
}

/* void shift_video_up(int num_row_shift);
 * Inputs: int num_row_shift = number of rows to shift up
 * Return Value: void
 * Function: Shift the screen up by num_row_shift lines */
void shift_video_up(char* video_mem, int num_row_shift) {
    int row,col;
    for (row = 0; row < (NUM_ROWS - num_row_shift); row++)
    {
        for(col = 0; col < NUM_COLS; col++) {
            *(uint8_t *)(video_mem + ((NUM_COLS * row + col) << 1)) =
                *(uint8_t *)(video_mem + ((NUM_COLS * (row + num_row_shift) + col) << 1));
            *(uint8_t *)(video_mem + ((NUM_COLS * row + col) << 1) + 1) =
                *(uint8_t *)(video_mem + ((NUM_COLS * (row + num_row_shift) + col) << 1) + 1);
        }
    }
    for (; row < NUM_ROWS; row++) {
        for(col = 0; col < NUM_COLS; col++) {
            *(uint8_t *)(video_mem + ((NUM_COLS * row + col) << 1)) = ' ';
        }
    }
}

/* int get_screen_y();
 * Inputs: none
 * Return Value: the current value of screen_y
 * Function: returns the current value of screen_y */
int get_screen_y() {
    return screen_y;
}

/* int get_screen_x();
 * Inputs: none
 * Return Value: the current value of screen_x
 * Function: returns the current value of screen_x */
int get_screen_x() {
    return screen_x;
}

/* int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
 * Inputs: uint32_t value = number to convert
 *            int8_t* buf = allocated buffer to place string in
 *          int32_t radix = base system. hex, oct, dec, etc.
 * Return Value: number of bytes written
 * Function: Convert a number to its ASCII representation, with base "radix" */
int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix) {
    static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int8_t *newbuf = buf;
    int32_t i;
    uint32_t newval = value;

    /* Special case for zero */
    if (value == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    /* Go through the number one place value at a time, and add the
     * correct digit to "newbuf".  We actually add characters to the
     * ASCII string from lowest place value to highest, which is the
     * opposite of how the number should be printed.  We'll reverse the
     * characters later. */
    while (newval > 0) {
        i = newval % radix;
        *newbuf = lookup[i];
        newbuf++;
        newval /= radix;
    }

    /* Add a terminating NULL */
    *newbuf = '\0';

    /* Reverse the string and return */
    return strrev(buf);
}

/* int8_t* strrev(int8_t* s);
 * Inputs: int8_t* s = string to reverse
 * Return Value: reversed string
 * Function: reverses a string s */
int8_t* strrev(int8_t* s) {
    register int8_t tmp;
    register int32_t beg = 0;
    register int32_t end = strlen(s) - 1;

    while (beg < end) {
        tmp = s[end];
        s[end] = s[beg];
        s[beg] = tmp;
        beg++;
        end--;
    }
    return s;
}

/* uint32_t strlen(const int8_t* s);
 * Inputs: const int8_t* s = string to take length of
 * Return Value: length of string s
 * Function: return length of string s */
uint32_t strlen(const int8_t* s) {
    register uint32_t len = 0;
    while (s[len] != '\0')
        len++;
    return len;
}

/* void* memset(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive bytes of pointer s to value c */
void* memset(void* s, int32_t c, uint32_t n) {
    c &= 0xFF;
    asm volatile ("                 \n\
            .memset_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memset_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memset_aligned \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memset_top     \n\
            .memset_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     stosl           \n\
            .memset_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memset_done    \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%edx       \n\
            jmp     .memset_bottom  \n\
            .memset_done:           \n\
            "
            :
            : "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_word(void* s, int32_t c, uint32_t n);
 * Description: Optimized memset_word
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set lower 16 bits of n consecutive memory locations of pointer s to value c */
void* memset_word(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosw           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_dword(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive memory locations of pointer s to value c */
void* memset_dword(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosl           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memcpy(void* dest, const void* src, uint32_t n);
 * Inputs:      void* dest = destination of copy
 *         const void* src = source of copy
 *              uint32_t n = number of byets to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of src to dest */
void* memcpy(void* dest, const void* src, uint32_t n) {
    asm volatile ("                 \n\
            .memcpy_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memcpy_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memcpy_aligned \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memcpy_top     \n\
            .memcpy_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     movsl           \n\
            .memcpy_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memcpy_done    \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%edx       \n\
            jmp     .memcpy_bottom  \n\
            .memcpy_done:           \n\
            "
            :
            : "S"(src), "D"(dest), "c"(n)
            : "eax", "edx", "memory", "cc"
    );
    return dest;
}

/* void* memmove(void* dest, const void* src, uint32_t n);
 * Description: Optimized memmove (used for overlapping memory areas)
 * Inputs:      void* dest = destination of move
 *         const void* src = source of move
 *              uint32_t n = number of byets to move
 * Return Value: pointer to dest
 * Function: move n bytes of src to dest */
void* memmove(void* dest, const void* src, uint32_t n) {
    asm volatile ("                             \n\
            movw    %%ds, %%dx                  \n\
            movw    %%dx, %%es                  \n\
            cld                                 \n\
            cmp     %%edi, %%esi                \n\
            jae     .memmove_go                 \n\
            leal    -1(%%esi, %%ecx), %%esi     \n\
            leal    -1(%%edi, %%ecx), %%edi     \n\
            std                                 \n\
            .memmove_go:                        \n\
            rep     movsb                       \n\
            "
            :
            : "D"(dest), "S"(src), "c"(n)
            : "edx", "memory", "cc"
    );
    return dest;
}

/* int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
 * Inputs: const int8_t* s1 = first string to compare
 *         const int8_t* s2 = second string to compare
 *               uint32_t n = number of bytes to compare
 * Return Value: A zero value indicates that the characters compared
 *               in both strings form the same string.
 *               A value greater than zero indicates that the first
 *               character that does not match has a greater value
 *               in str1 than in str2; And a value less than zero
 *               indicates the opposite.
 * Function: compares string 1 and string 2 for equality */
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n) {
    int32_t i;
    for (i = 0; i < n; i++) {
        if ((s1[i] != s2[i]) || (s1[i] == '\0') /* || s2[i] == '\0' */) {

            /* The s2[i] == '\0' is unnecessary because of the short-circuit
             * semantics of 'if' expressions in C.  If the first expression
             * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
             * s2[i], then we only need to test either s1[i] or s2[i] for
             * '\0', since we know they are equal. */
            return s1[i] - s2[i];
        }
    }
    return 0;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 * Return Value: pointer to dest
 * Function: copy the source string into the destination string */
int8_t* strcpy(int8_t* dest, const int8_t* src) {
    int32_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 *                uint32_t n = number of bytes to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of the source string into the destination string */
int8_t* strncpy(int8_t* dest, const int8_t* src, uint32_t n) {
    int32_t i = 0;
    while (src[i] != '\0' && i < n) {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

/*
 * The strchr() function locates the ﬁrst occurrence of c (converted to a char) in the string pointed to by s.
 * The terminating null character is considered to be part of the string.
 */
int8_t* strchr(const int8_t *s, int8_t c) {
    while (*s != (int8_t)c)
        if (!*s++)
            return 0;
    return (int8_t*)s;
}

/*
 * The strcspn() function computes the length of the maximum initial segment of the string pointed to by s1
 * which consists entirely of characters not from the string pointed to by s2.
 */
uint32_t strcspn(const int8_t *s1, const int8_t *s2) {
    uint32_t ret=0;
    while(*s1)
        if(strchr(s2,*s1))
            return ret;
        else
            s1++,ret++;
    return ret;
}

/*
 * The strspn() function computes the length of the maximum initial segment of the string pointed to by s1
 * which consists entirely of characters from the string pointed to by s2.
 */
uint32_t strspn(const int8_t *s1, const int8_t *s2) {
    uint32_t ret=0;
    while(*s1 && strchr(s2,*s1++))
        ret++;
    return ret;
}

/*
 * The strtok() function breaks the string s1 into tokens and null-terminates them.
 * Delimiter-Characters at the beginning and end of str are skipped. On each subsequent call delim may change.
 */
int8_t* strtok(int8_t* str, const int8_t* delim) {
    static int8_t* p=0;
    if(str)
        p=str;
    else if(!p)
        return 0;
    str=p+strspn(p,delim);
    p=str+strcspn(str,delim);
    if(p==str)
        return p=0;
    p = *p ? *p=0,p+1 : 0;
    return str;
}

/* void test_interrupts(uint32_t count)
 * Inputs: count -- # of interrupts after last change in RTC rate
 * Return Value: void
 * Function: Updates a counter in the top right corner. To be used to test rtc */
void test_interrupts(uint32_t count) {
    // int32_t i;
    // for (i = 0; i < 1 * NUM_COLS; i++) {
    //     /* Remain some space for keyboard echo */
    //     if(i % NUM_COLS <= 77)
    //         continue;
    //     video_mem[i << 1]++;
    // }
    video_mem[(TOP_RIGHT_CHAR - 1) << 1] = count % 100 / 10 + '0';
    video_mem[TOP_RIGHT_CHAR << 1] = count % 10 + '0';
}

void throw(const char *file, uint32_t line, const char *desc) {
    kprintf("ERROR: file: %s, line: %s, desc: %s\n", desc);
}

uint16_t flip_word(uint16_t data) {
    uint32_t byte0 = *((uint8_t*)(&data));
    uint32_t byte1 = *((uint8_t*)(&data) + 1);
    return ((byte0 << 8) | byte1);
}

uint32_t flip_long(uint32_t data) {
    uint32_t byte0 = *((uint8_t*)(&data));
    uint32_t byte1 = *((uint8_t*)(&data) + 1);
    uint32_t byte2 = *((uint8_t*)(&data) + 2);
    uint32_t byte3 = *((uint8_t*)(&data) + 3);
    return ((byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3);
}

uint8_t flip_byte(uint8_t byte, uint8_t num_bits) {
    uint8_t high_num_bits = byte << (8 - num_bits);
    return high_num_bits | (byte >> num_bits);
}

uint8_t htonb(uint8_t hostbyte, uint8_t num_bits) {
    return flip_byte(hostbyte, num_bits);
}

uint8_t ntohb(uint8_t netbyte, uint8_t num_bits) {
    return flip_byte(netbyte, 8 - num_bits);
}

uint16_t htons(uint16_t hostshort) {
    return flip_word(hostshort);
}

uint32_t htonl(uint32_t hostlong) {
    return flip_long(hostlong);
}

uint16_t ntohs(uint16_t netshort) {
    return flip_word(netshort);
}

uint32_t ntohl(uint32_t netlong) {
    return flip_long(netlong);
}

uint32_t random_seed=0;
/**
 * Generate some random number by init seed
 * @param seed
 * @param max
 * @return
 */
uint32_t rand(uint32_t seed,uint32_t max) {
    random_seed = random_seed+seed * 1103515245 +12345;
    return (uint32_t)(random_seed / 65536) % (max+1);
}

void putc2str(uint8_t* target, uint32_t* target_size, uint8_t c) {
    target[*target_size] = c;
    (*target_size) = (*target_size) + 1;
}

int32_t puts2str(uint8_t* target, uint32_t* target_size, int8_t* s) {
    register int32_t index = 0;
    while (s[index] != '\0') {
        putc2str(target, target_size, s[index]);
        index++;
    }
    return index;
}

int32_t sprintf(uint8_t* target, int8_t *format, ...) {

    /* Pointer to the format string */
    int8_t* buf = format;

    uint32_t target_size = 0;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
            {
                int32_t alternate = 0;
                buf++;

                format_char_switch:
                /* Conversion specifiers */
                switch (*buf) {
                    /* Print a literal '%' character */
                    case '%':
                        putc2str(target, &target_size, '%');
                        break;

                        /* Use alternate formatting */
                    case '#':
                        alternate = 1;
                        buf++;
                        /* Yes, I know gotos are bad.  This is the
                         * most elegant and general way to do this,
                         * IMHO. */
                        goto format_char_switch;

                        /* Print a number in hexadecimal form */
                    case 'x':
                    {
                        int8_t conv_buf[64];
                        if (alternate == 0) {
                            itoa(*((uint32_t *)esp), conv_buf, 16);
                            puts2str(target, &target_size, conv_buf);
                        } else {
                            int32_t starting_index;
                            int32_t i;
                            itoa(*((uint32_t *)esp), &conv_buf[8], 16);
                            i = starting_index = strlen(&conv_buf[8]);
                            while(i < 8) {
                                conv_buf[i] = '0';
                                i++;
                            }
                            puts2str(target, &target_size, &conv_buf[starting_index]);
                        }
                        esp++;
                    }
                        break;

                        /* Print a number in unsigned int form */
                    case 'u':
                    {
                        int8_t conv_buf[36];
                        itoa(*((uint32_t *)esp), conv_buf, 10);
                        puts2str(target, &target_size, conv_buf);
                        esp++;
                    }
                        break;

                        /* Print a number in signed int form */
                    case 'd':
                    {
                        int8_t conv_buf[36];
                        int32_t value = *((int32_t *)esp);
                        if(value < 0) {
                            conv_buf[0] = '-';
                            itoa(-value, &conv_buf[1], 10);
                        } else {
                            itoa(value, conv_buf, 10);
                        }
                        puts2str(target, &target_size, conv_buf);
                        esp++;
                    }
                        break;

                        /* Print a single character */
                    case 'c':
                        putc2str(target, &target_size, (uint8_t) *((int32_t *)esp));
                        esp++;
                        break;

                        /* Print a NULL-terminated string */
                    case 's':
                        puts2str(target, &target_size, *((int8_t **)esp));
                        esp++;
                        break;

                    default:
                        break;
                }

            }
                break;

            default:
                putc2str(target, &target_size, *buf);
                break;
        }
        buf++;
    }
    target[target_size] = '\0';
    return (buf - format);
}

uint8_t isalpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

uint8_t isspace(int c) {
    return c == ' ' || c == '\t'; // || whatever other char you consider space
}

uint8_t ispunct(int c) {
    static const char *punct = ".;!?...";
    return strchr(punct, c) == NULL ? 0 : 1; // you can make this shorter
}

int tolower(int c) {
    if ( !isalpha(c) ) return c;
    return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

char* strcat(char *dest, const char *src) {
    char *rdest = dest;

    while (*dest)
        dest++;
    while ((*dest++ = *src++)) {};
    return rdest;
}

int atoi(char *p) {
    int k = 0;
    while (*p) {
        k = (k << 3) + (k << 1) + (*p) - '0';
        p++;
    }
    return k;
}

char* strstr(char *str, char *substr) {
    while (*str)
    {
        char *Begin = str;
        char *pattern = substr;

        // If first character of sub string match, check for whole string
        while (*str && *pattern && *str == *pattern)
        {
            str++;
            pattern++;
        }
        // If complete sub string match, return starting address
        if (!*pattern)
            return Begin;

        str = Begin + 1;	// Increament main string
    }
    return NULL;
}
