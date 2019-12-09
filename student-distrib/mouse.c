/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/19
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 * Ref: https://wiki.osdev.org/PS/2_Mouse
 * Ref: https://forum.osdev.org/viewtopic.php?t=10247
 */
#include "mouse.h"

uint8_t mouse_cycle = 0;
int8_t mouse_data[3];

/**
 * Handler for mouse interrupt
 */
void mouse_handler() {
//    cli();
    if(gui_enabled) {
        uint8_t buf[20];
        itoa((uint32_t)mouse_cycle, (int8_t*)buf, 10);
        draw_rect(0, 0, 50, 10, 0xFFFFFF);
        render_string(0, 0, (char*)buf, 0);
        render_string(200, 300, "mouse comes outer... ", 0);
    }

    switch (mouse_cycle) {
        case 0: {
//            render_string(100, 200, "mouse comes innear0...", 0);

            mouse_data[0] = mouse_read();

            if((mouse_data[0] & 0x8) != 0x8) {
                /* kprintf("bad mouse0!!!\n"); */
                break;
            }

            if((mouse_data[0] & 0x1) == 0x1) {
                /* kprintf("left click\n"); */
                draw_rect(mouse_x, mouse_y, 4,4,0xFF0000);
            }
            if((mouse_data[0] & 0x2) == 0x2) {
                /* kprintf("right click\n"); */
            }
            mouse_cycle++;
            break;
        }
        case 1: {
//            render_string(100, 300, "mouse comes innear1...", 0);

            mouse_data[1] = mouse_read();
            mouse_cycle++;
            break;
        }
        case 2: {
            mouse_data[2] = mouse_read();
            mouse_x += mouse_data[1];
            mouse_y -= mouse_data[2];

            if(mouse_x < 0)
                mouse_x = 0;
            if(mouse_y < 0)
                mouse_y = 0;
            if(mouse_x > SVGA_X_DIM-1)
                mouse_x = SVGA_X_DIM-1;
            if(mouse_y > SVGA_Y_DIM-1)
                mouse_y = SVGA_Y_DIM-1;

//            kprintf("mouse_x: %d, mouse_y: %d\n", mouse_x, mouse_y);

            if(gui_enabled) {
                render_cursor(mouse_x, mouse_y);
            }
//            kprintf("mouse_x: %d, mouse_y: %d\n", mouse_data[1], mouse_data[2]);
//            kprintf("mouse_x_real: %d, mouse_y_real: %d\n", real_x, real_y);
            mouse_cycle = 0;

            break;
        }
        default: {
            mouse_cycle = 0;
            break;
        }

    }

//    sti();
    send_eoi(MOUSE_IRQ);
}

/**
 * Init mouse
 */
void init_mouse() {
    mouse_x = 50;
    mouse_y = 50;
    uint8_t status;

    mouse_wait(MOUSE_WAIT_TYPE_SIGNAL);
    outb(0xA8, MOUSE_CTRL_PORT);

    mouse_wait(MOUSE_WAIT_TYPE_SIGNAL);
    outb(0x20, MOUSE_CTRL_PORT);
    mouse_wait(MOUSE_WAIT_TYPE_DATA);
    status = (inb(MOUSE_DATA_PORT) | 2);
    mouse_wait(MOUSE_WAIT_TYPE_SIGNAL);
    outb(0x60, MOUSE_CTRL_PORT);
    mouse_wait(MOUSE_WAIT_TYPE_SIGNAL);
    outb(status, MOUSE_DATA_PORT);

    /* Use the default settings */
    mouse_write(0xF6);
    mouse_read();  /* Wait for ack */

    /* Enable the mouse */
    mouse_write(0xF4);
    mouse_read();  /* Wait for ack */

    enable_irq(MOUSE_IRQ);
    kprintf("success init mouse\n");
}

/**
 * Write some data to mouse
 * @param data
 */
void mouse_write(uint8_t data) {
    mouse_wait(MOUSE_WAIT_TYPE_SIGNAL);
    outb(0xD4, MOUSE_CTRL_PORT); /* We are sending a command */
    mouse_wait(MOUSE_WAIT_TYPE_SIGNAL); /* Wait for ack */
    outb(data, MOUSE_DATA_PORT);
}

/**
 * Read data from mouse
 * @return
 */
uint8_t mouse_read() {
    mouse_wait(MOUSE_WAIT_TYPE_DATA);
    return inb(MOUSE_DATA_PORT);
}

/**
 * Wait for some circle
 * @param type
 */
void mouse_wait(uint8_t type) {
    uint32_t timeout = MOUSE_TIMEOUT;
    switch (type) {
        case MOUSE_WAIT_TYPE_DATA: {
            /* Wait until we can read */
            while(timeout--) {
                if((inb(MOUSE_CTRL_PORT) & 1) == 1)
                    return;
            }
            break;
        }
        case MOUSE_WAIT_TYPE_SIGNAL: {
            while(timeout--) {
                if((inb(MOUSE_CTRL_PORT) & 2) == 0)
                    return;
            }
            break;
        }
        default:
            return;
    }
}
