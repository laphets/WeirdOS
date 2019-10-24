#include "keyboard.h"

/*
 * mapping from scancode to ascii. gotten from:
 * 
 * http://www.osdever.net/bkerndev/Docs/keyboard.htm
 */
static unsigned char kbdus[256] =
        {
                0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
                '9', '0', '-', '=', '\b',	/* Backspace */
                '\t',			/* Tab */
                'q', 'w', 'e', 'r',	/* 19 */
                't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
                0,			/* 29   - Control */
                'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
                '\'', '`',   0,		/* Left shift */
                '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
                'm', ',', '.', '/',   0,				/* Right shift */
                '*',
                0,	/* Alt */
                ' ',	/* Space bar */
                0,	/* Caps lock */
                0,	/* 59 - F1 key ... > */
                0,   0,   0,   0,   0,   0,   0,   0,
                0,	/* < ... F10 */
                0,	/* 69 - Num lock*/
                0,	/* Scroll Lock */
                0,	/* Home key */
                0,	/* Up Arrow */
                0,	/* Page Up */
                '-',
                0,	/* Left Arrow */
                0,
                0,	/* Right Arrow */
                '+',
                0,	/* 79 - End key*/
                0,	/* Down Arrow */
                0,	/* Page Down */
                0,	/* Insert Key */
                0,	/* Delete Key */
                0,   0,   0,
                0,	/* F11 Key */
                0,	/* F12 Key */
                0,	/* All other keys are undefined */
        };

/*
 * mapping from non shifted ascii to shifted ascii.
 */
static unsigned char shift_table[200] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
    21,22,23,24,25,26,27,28,29,30,31,' ','!','"','#','$',
    '%','&','"','(',')','*','+','<','_','>','?',')','!',
    '@','#','$','%','^','&','*','(',':',':','<','+','>',
    '?','@','A','B','C','D','E','F','G','H','I','J','K',
    'L','M','N','O','P','Q','R','S','T','U','V','W','X',
    'Y','Z','{','|','}','^','_','~','A','B','C','D','E',
    'F','G','H','I','J','K','L','M','N','O','P','Q','R',
    'S','T','U','V','W','X','Y','Z','{','|','}','~',127
};

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

    /* Set flags to deal with caps and shift presses */
    if (scancode == KEYBOARD_CAPS_LOCK_PRESS) {
        keyboard_flag ^= CAPS_LOCK_BIT_MASK;
        return 0;
    } else if (scancode == KEYBOARD_LEFT_SHIFT_PRESS) {
        keyboard_flag |= LEFT_SHIFT_BIT_MASK;
        return 0;
    } else if (scancode == KEYBOARD_RIGHT_SHIFT_PRESS) {
        keyboard_flag |= RIGHT_SHIFT_BIT_MASK;
        return 0;
    } else if (scancode == KEYBOARD_LEFT_CTRL_PRESS) {
        keyboard_flag |= LEFT_CTRL_MASK;
        return 0;
    } else if (scancode == KEYBOARD_RIGHT_CTRL_PRESS) {
        keyboard_flag |= RIGHT_CTRL_MASK;
        return 0;
    } else if (scancode == KEYBOARD_LEFT_SHIFT_RELEASE) {
        keyboard_flag &= ~LEFT_SHIFT_BIT_MASK;
        return 0;
    } else if (scancode == KEYBOARD_RIGHT_SHIFT_RELEASE) {
        keyboard_flag &= ~RIGHT_SHIFT_BIT_MASK;
        return 0;
    } else if (scancode == KEYBOARD_LEFT_CTRL_RELEASE) {
        keyboard_flag &= ~LEFT_CTRL_MASK;
        return 0;
    } else if (scancode == KEYBOARD_RIGHT_CTRL_RELEASE) {
        keyboard_flag &= ~RIGHT_CTRL_MASK;
        return 0;
    }

    /* Convert the scancode to the ascii equivalent */
    ascii = kbdus[scancode];

    /* Checks if CTRL-L is pressed to clear screen */
    if (keyboard_flag & (RIGHT_CTRL_MASK | LEFT_CTRL_MASK) {
        if (ascii == 'l') {
            clear();
            return 0;
        }
    }

    /* Check is a letter is pressed while both CAPS and SHIFT are enabled */
    if ((keyboard_flag & 0x1) && (keyboard_flag & 0x6) && (ascii >= 'a') && (ascii <= 'z')) {
        return ascii;
    } else if (keyboard_flag & 0x7) { /* Check if the input needs to be shifted */
        return shift_table[ascii];
    }
    return ascii;
}

/**
 * Interrupt handler for keyboard, will be called in idtwrapper.S
 */
void keyboard_handler() {
    uint8_t scancode;
    unsigned char ascii;
    scancode = inb(KEYBOARD_PORT);

    /* Check to not print when scancode is non pritable */
    if (0 != (ascii = scancode2char(scancode)))
    {
        printf("Keyboard comes: %c\n", ascii);
    }

    send_eoi(KEYBOARD_IRQ);
}

/**
 * Routine to init keyboard
 */
void init_keyboard() {
    enable_irq(KEYBOARD_IRQ);
}
