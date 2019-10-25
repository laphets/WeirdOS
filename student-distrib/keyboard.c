#include "keyboard.h"

/*
 * mapping from scancode to ascii. gotten from:
 * 
 * http://www.osdever.net/bkerndev/Docs/keyboard.htm
 */
static unsigned char kbdus[256] =
        {
            0,27,'1','2','3','4','5','6','7','8',
            '9','0','-','=','\b','\t','q','w','e','r',
            't','y','u','i','o','p','[',']','\n',0,
            'a','s','d','f','g','h','j','k','l',';',
            '\'','`',0,'\\','z','x','c','v','b','n',
            'm',',','.','/',0,'*',0,' ',0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,'-',0,0,0,'+',
            0,0,0,0,0,0,0,0,0,0,
            0,
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

/* char handle_keybaord_flags(uint8_t scancode);
 * Input: uint8_t scancode - keyboard scancode pressed
 * Return: 0 if special key was pressed, 1 otherwise
 * Function: Looks at the scancode to determine if a special
 *          key was pressed that should be kept track of.
 */
char handle_keybaord_flags(uint8_t scancode) {
    /* Set flags to deal with caps, shift, and ctrl presses */
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
    return 1;
}

/* char handle_keybaord_flags(uint8_t scancode);
 * Input: uint8_t scancode - keyboard scancode pressed
 * Return: 0 if special key was pressed, 1 otherwise
 * Function: Looks at the scancode to determine if a special
 *          key was pressed that should be kept track of.
 */
char handle_special_keys(uint8_t scancode) {
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
    if(!handle_keybaord_flags(scancode)) {
        return 0;
    }

    /* Convert the scancode to the ascii equivalent */
    ascii = kbdus[scancode];

    /* Checks if CTRL-L is pressed to clear screen and reset cursor*/
    if (keyboard_flag & (RIGHT_CTRL_MASK | LEFT_CTRL_MASK)) {
        if (ascii == 'l') {
            clear();
            reset_cursor_pos();
            return 0;
        }
    }

    /* Check is a letter is pressed while both CAPS and SHIFT are enabled */
    if ((keyboard_flag & CAPS_LOCK_BIT_MASK) &&
        (keyboard_flag & (LEFT_SHIFT_BIT_MASK | RIGHT_SHIFT_BIT_MASK)) &&
        (ascii >= 'a') && (ascii <= 'z')) {
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
        if((ascii == '\b') && (keyboard_buf_pos > 0)) {
            keyboard_buf[--keyboard_buf_pos] = 0;
        } else if (keyboard_buf_pos < KEYBOARD_BUF_MAX_SIZE) {
            keyboard_buf[keyboard_buf_pos++] = ascii;
            if(ascii == '\n') {
                read();
                write();
                keyboard_buf_pos = 0;
            }
        }
        //printf("Keyboard comes: %c\n", ascii);
    }

    send_eoi(KEYBOARD_IRQ);
}

/**
 * Routine to init keyboard
 */
void init_keyboard() {
    open();
    keyboard_buf_pos = 0;
    enable_irq(KEYBOARD_IRQ);
}
