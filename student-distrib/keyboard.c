#include "keyboard.h"

/*
 * mapping from scancode to ascii. gotten from:
 * 
 * http://www.osdever.net/bkerndev/Docs/keyboard.htm
 */
static unsigned char kbdus[TOTAL_NUM_SCANCODES] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b', 0, 'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, /* Rest are 0 */
};

/*
 * mapping from non shifted ascii to shifted ascii.
 */
static unsigned char shift_table[TOTAL_NUM_ASCII_CODES] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, ' ', '!', '"', '#', '$',
    '%', '&', '"', '(', ')', '*', '+', '<', '_', '>', '?', ')', '!',
    '@', '#', '$', '%', '^', '&', '*', '(', ':', ':', '<', '+', '>',
    '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', '{', '|', '}', '^', '_', '~', 'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
    'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '~', 127};

/* char handle_keybaord_flags(uint8_t scancode);
 * Input: uint8_t scancode - keyboard scancode pressed
 * Return: 1 if special key was pressed, 0 otherwise
 * Function: Looks at the scancode to determine if a special
 *          key was pressed that should be kept track of.
 */
char handle_keybaord_flags(uint8_t scancode) {
    /* Set flags to deal with caps, shift, and ctrl presses */
    if (scancode == KEYBOARD_CAPS_LOCK_PRESS) {
        keyboard_flag ^= CAPS_LOCK_BIT_MASK;
        return 1;
    } else if ((scancode == KEYBOARD_LEFT_SHIFT_PRESS) ||
                (scancode == KEYBOARD_RIGHT_SHIFT_PRESS)) {
        keyboard_flag |= SHIFT_BIT_MASK;
        return 1;
    } else if ((scancode == KEYBOARD_LEFT_SHIFT_RELEASE) ||
                (scancode == KEYBOARD_RIGHT_SHIFT_RELEASE)) {
        keyboard_flag &= ~SHIFT_BIT_MASK;
        return 1;
    }  else if (scancode == KEYBOARD_CTRL_PRESS) {
        keyboard_flag |= CTRL_BIT_MASK;
        return 1;
    } else if (scancode == KEYBOARD_CTRL_RELEASE) {
        keyboard_flag &= ~CTRL_BIT_MASK;
        return 1;
    } else if (scancode == KEYBOARD_ALT_PRESS) {
        keyboard_flag |= ALT_BIT_MASK;
        return 1;
    } else if (scancode == KEYBOARD_ALT_RELEASE) {
        keyboard_flag &= ~ALT_BIT_MASK;
    }
    return 0;
}

/* char handle_keybaord_flags(uint8_t scancode);
 * Input: uint8_t scancode - keyboard scancode pressed
 * Return: 0 if special key was pressed, 1 otherwise
 * Function: Looks at the scancode to determine if a special
 *          key was pressed that should be kept track of.
 */
char handle_special_keys(uint8_t ascii) {
    /* Checks if CTRL-L is pressed to clear screen and reset cursor*/
    if ((keyboard_flag & CTRL_BIT_MASK) && (ascii == 'l')) {
        clear();
        return 1;
    }

    /* If the key is combo of CTRL+number, we just switch to that terminal */
    if((keyboard_flag & CTRL_BIT_MASK) && (ascii >= (uint8_t)'1' && ascii <= (uint8_t)'5')) {
        send_eoi(KEYBOARD_IRQ);
        switch_terminal((uint32_t)(ascii - (uint8_t)'1'), TYPE_SWITCH_ACTIVE_TERMINAL /* 0 for active terminal switch */);
        return 1;
    }

    return 0;
}

char handle_terminal_switch(uint8_t scancode) {
    if(keyboard_flag & ALT_BIT_MASK){
        switch (scancode) {
            case KEYBOARD_KEY_F1: {
                send_eoi(KEYBOARD_IRQ);
                switch_terminal(0, TYPE_SWITCH_ACTIVE_TERMINAL /* 0 for active terminal switch */);
                return 1;
            }
            case KEYBOARD_KEY_F2: {
                send_eoi(KEYBOARD_IRQ);
                switch_terminal(1, TYPE_SWITCH_ACTIVE_TERMINAL /* 0 for active terminal switch */);
                return 1;
            }
            case KEYBOARD_KEY_F3: {
                send_eoi(KEYBOARD_IRQ);
                switch_terminal(2, TYPE_SWITCH_ACTIVE_TERMINAL /* 0 for active terminal switch */);
                return 1;
            }
            case KEYBOARD_KEY_F4: {
                send_eoi(KEYBOARD_IRQ);
                switch_terminal(3, TYPE_SWITCH_ACTIVE_TERMINAL /* 0 for active terminal switch */);
                return 1;
            }
            case KEYBOARD_KEY_F5: {
                send_eoi(KEYBOARD_IRQ);
                switch_terminal(4, TYPE_SWITCH_ACTIVE_TERMINAL /* 0 for active terminal switch */);
                return 1;
            }
            default: {
                return 0;
            }
        }
    }

    return 0;
}

/**
 * @brief Translate scancode into the appropriate char
 *        accounting for CAPS and shift. Also keeps
 *        track of the CAPS and SHIFT state.
 * 
 * @param scancode 
 * @return unsigned char 
 */
unsigned char
scancode2char(uint8_t scancode)
{
    unsigned char ascii;
    
    /* Update the keyboard flags if necesary */
    if(handle_keybaord_flags(scancode)) {
        return 0;
    }

    /* We pre check for the terminal switch hotkey */
    if(handle_terminal_switch(scancode)) {
        return 0;
    }


    /* Convert the scancode to the ascii equivalent */
    ascii = kbdus[scancode];

    /* Checks if a special combination has been pressed */
    if (handle_special_keys(ascii)) {
        return 0;
    }

    /* Check is a letter is pressed while both CAPS and SHIFT are enabled */
    if ((keyboard_flag & CAPS_LOCK_BIT_MASK) &&
        (keyboard_flag & SHIFT_BIT_MASK) &&
        (ascii >= 'a') && (ascii <= 'z')) {
        return ascii;
    } 
    /* Check if the input needs to be shifted */
    else if (keyboard_flag & SHIFT_BIT_MASK) {
        return shift_table[ascii];
    }
    else if ((keyboard_flag & CAPS_LOCK_BIT_MASK) &&
            (ascii >= 'a') && (ascii <= 'z')) {
        return shift_table[ascii];
    }
    return ascii;
}

/**
 * Interrupt handler for keyboard, will be called in idtwrapper.S
 */
void keyboard_handler(registers_t registers) {
    cli();

    /* printf("keyboard interrupt comes...\n"); */
    uint8_t scancode;
    unsigned char ascii;
    scancode = inb(KEYBOARD_PORT);
    /* Check to not print when scancode is non pritable */
    if (0 != (ascii = scancode2char(scancode))) {
        if ((ascii == '\b') && (keyboard_buf_size > 0)) {
            /**
             * Check for Backspace
             */
            keyboard_buf_size--;
            putc2buffer((char*)VIDEO_MEMORY_START_ADDRESS, ascii);
        } else if (ascii == '\n' || ascii == '\r') {
            keyboard_buf[keyboard_buf_size++] = ascii;
            strncpy(terminal_buf, keyboard_buf, keyboard_buf_size);
            terminal_buf_size = keyboard_buf_size;
            terminal_t* active_terminal = &terminal_list[current_active_terminal];
            /* kprintf("enter received: %d\n", active_terminal); */
            active_terminal->enter_pressed_flag = 1;
            keyboard_buf_size = 0;
            putc2buffer((char*)VIDEO_MEMORY_START_ADDRESS, ascii);
        } else if ((ascii != '\b') && (keyboard_buf_size < KEYBOARD_BUF_MAX_SIZE-1)) {
            keyboard_buf[keyboard_buf_size++] = ascii;
            putc2buffer((char*)VIDEO_MEMORY_START_ADDRESS, ascii);
        }
    }
    sti();
    send_eoi(KEYBOARD_IRQ);
    /* printf("Keyboard Interrupt: register: 0x%x\n", registers.eip); */
}

/**
 * Routine to init keyboard
 */
void init_keyboard() {
    keyboard_buf_size = 0;
    keyboard_flag = 0;
    enable_irq(KEYBOARD_IRQ);
}
