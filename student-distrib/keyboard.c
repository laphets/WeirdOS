#include "keyboard.h"


void keyboard_handler() {
    uint8_t scancode;
    scancode = inb(KEYBOARD_PORT);

    printf("Keyboard comes: %c\n", kbdus[scancode]);

    send_eoi(KEYBOARD_IRQ);
}

void init_keyboard() {

    enable_irq(KEYBOARD_IRQ);
}
