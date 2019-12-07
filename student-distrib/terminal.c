#include "terminal.h"


/**
 * Init for terminal list
 */
void init_terminal() {
    current_active_terminal = -1;   /* Set -1 for a notenable number */
    int tid = 0;
    for(tid = 0; tid < MAX_TERMINAL_NUM; tid++) {
        terminal_t* terminal = &terminal_list[tid];
        terminal->current_task_pid = 0;
        terminal->tid = tid;
        terminal->present = 0;
        terminal->screen_x = 0;
        terminal->screen_y = 0;
        terminal->inactive_video_mem = NULL;
        terminal->enter_pressed_flag = 0;
        /* We will map the unshown_vm_addr into kernel space at 0x80000 + tid * 4KB */
        terminal->unshown_vm_addr = UNSHOWN_VM_START_ADDR + tid * 0x1000 /* 4KB */ ; /* TODO: Record the layout mapping here */
        set_kernel_vm((uint8_t*)terminal->unshown_vm_addr);   /* Set up the paging mapping */
        memset(terminal->terminal_buf, 0, MAX_TERMINAL_BUF_SIZE);
    }
    flush_paging();
}

/**
 * Launch the first terminal carried with a root shell
 */
void launch_terminal() {
    /* Init for running and active terminal status */
    current_running_terminal = 0;
    current_active_terminal = 0;
    /* Init for the first terminal, let say terminal 0 */
    terminal_t* terminal = &terminal_list[0];
    terminal->present = 1;
    terminal_buf = (char*)terminal->terminal_buf;
    terminal->enter_pressed_flag = 0;
    clear_vm_buffer((char*)terminal->unshown_vm_addr);
    clear();
    /* Then execute that as the root shell */
    execute_as_root = 1;
    execute((const uint8_t*)"shell");
}

/**
 * Switch active/runing terminal
 * @param tid used for swicth the next active terminal
 * @param type 0 -> switch active terminal triggled by keyboard, 1 -> schedule switch running terminal triggled by pit
 */
void switch_terminal(uint32_t tid, uint8_t type) {
    /* Have some input checking */
    if(type > 1)
        return;

    if(type == TYPE_SWITCH_RUNNING_TERMINAL) {
        /**
         * Schedule next running terminal
         */
        if(current_running_terminal == -1 /* Not enable */ || current_active_terminal == -1 /* Not enable */)
            return;
        int32_t turns = 0;
        next_running_terminal = current_running_terminal;

        for(turns = 0; turns <= MAX_TERMINAL_NUM; turns++) {
            next_running_terminal = (next_running_terminal+1) % MAX_TERMINAL_NUM;
            if(terminal_list[next_running_terminal].present != 1)
                continue;
            if(next_running_terminal == current_running_terminal)
                continue;

            /* Then we find the next terminal to run, context switch to there */
            task_t* current_task = get_task_by_pid(terminal_list[current_running_terminal].current_task_pid);
            task_t* next_task = get_task_by_pid(terminal_list[next_running_terminal].current_task_pid);

            /* terminal_t* current_terminal = &terminal_list[current_running_terminal]; */
            /* terminal_t* next_terminal = &terminal_list[next_running_terminal]; */

            /**
             * If next_task is in inactive terminal, we should set the video paging mapping to some unshown phys address
             * If next_task is in active terminal, we just set back video paging mapping to real address
             */
            /* printf("current_running_terminal: %d, next_running_terminal: %d\n", current_running_terminal, next_running_terminal); */

            current_running_terminal = next_running_terminal;
            /* We should re-map the user paging */
            /* Reset parent's page */
            page_directory_entry_t *user_page_directory = &default_page_directory[32];
            user_page_directory->present = 1;
            user_page_directory->rw = 1;
            user_page_directory->us = 1;
            user_page_directory->pwt = 0;
            user_page_directory->pcd = 1;
            user_page_directory->accessed = 0;
            user_page_directory->dirty = 0;
            user_page_directory->ps = 1;
            user_page_directory->global = 0;
            user_page_directory->avail = 0;
            user_page_directory->address = ((2+next_task->pid) << 10);

            flush_paging();

//            /* We should switch the RTC rate */
//            rtc_write(0, &next_task->rtc_hertz, 4); /* 4 Byte for int */

            /* We should modify the TSS */
            tss.ss0 = KERNEL_DS;
            tss.esp0 = KERNEL_BOTTOM - next_task->pid * PCB_BLOCK_SIZE ;

            /* kprintf("Begin exchange stack[0/2]\n"); */

            /* Then do some context switch */
            /* Record currrent esp and ebp state */
            asm volatile("                  \n\
            movl %%ebp, %0        \n\
            movl %%esp, %1           \n\
            "
            : "=r"(current_task->schd_ebp), "=r"(current_task->schd_esp)
            :
            : "ebp", "esp");
            /* Restore previous esp and ebp state */
            asm volatile ("                  \n\
            movl %0, %%esp        \n\
            movl %1, %%ebp\n\
            "
            :
            : "r"(next_task->schd_esp), "r"(next_task->schd_ebp)
            : "memory", "cc", "ebp", "esp"
            );

            /* kprintf("sched: next: %d esp: 0x%x, ebp: 0x%x\n", next_task->pid, next_task->schd_esp, next_task->schd_ebp); */
            return;
        }
        return;
    }


    /**
     * Swicth to target active terminal
     */

    /* Have some input checking */
    if( tid < 0 || tid >= MAX_TERMINAL_NUM || current_active_terminal == tid) {
        return;
    }

    if(current_running_terminal == -1)
        current_running_terminal = tid;

    terminal_t* next_terminal = &terminal_list[tid];
    if(current_active_terminal != -1 && terminal_list[current_active_terminal].present == 1) {
        /**
         * If there is currently some terminal running
         * we should store the current video memory to the unshown location
         */
         terminal_t* current_terminal = &terminal_list[current_active_terminal];
        /**
        * If we have enable video map, we should set that t othe unshown_vm_addr
        */
        task_t* current_task = get_task_by_pid(current_terminal->current_task_pid);
        if(current_task->video_addr != NULL) {
            set_user_vm((char*)current_terminal->unshown_vm_addr, current_task->video_addr);
            flush_paging();
        }

         memcpy((void*)current_terminal->unshown_vm_addr, (const void*)VIDEO_MEMORY_START_ADDRESS, _4KB);
         current_terminal->screen_x = get_screen_x();
         current_terminal->screen_y = get_screen_y();
    }

    if(next_terminal->present == 1) {
        /**
         * If that terminal is present
         * We can just set switch to that termianl
         * We should set up cursor_pos, terminal_buf, and video memory
         * For the old terminal, we should just copy the video memory from some buffer space
         */

        terminal_buf = (char*)next_terminal->terminal_buf;
        memcpy((void*)VIDEO_MEMORY_START_ADDRESS, (const void*)next_terminal->unshown_vm_addr, _4KB);
        set_cursor_pos(next_terminal->screen_x, next_terminal->screen_y);

        task_t* next_task = get_task_by_pid(next_terminal->current_task_pid);
        if(next_task->video_addr != NULL) {
            set_user_vm((char*)VIDEO_MEMORY_START_ADDRESS, next_task->video_addr);
            flush_paging();
        }

        current_active_terminal = tid;
    } else {
        /**
         * That terminal is not present
         * Then we should create one and bind a shell
         * For the new terminal, we should init(clear) the video memory
         */
        /* terminal_t* current_terminal = &terminal_list[current_active_terminal]; */

        /* First init for terminal */
        next_terminal->present = 1;
        next_terminal->screen_x = 0;
        next_terminal->screen_y = 0;
        next_terminal->enter_pressed_flag = 0;
        memset(next_terminal->terminal_buf, 0, MAX_TERMINAL_BUF_SIZE);
        clear_vm_buffer((char*)next_terminal->unshown_vm_addr);

        memcpy((void*)VIDEO_MEMORY_START_ADDRESS, (const void*)next_terminal->unshown_vm_addr, _4KB);
        set_cursor_pos(next_terminal->screen_x, next_terminal->screen_y);
        current_active_terminal = tid;

        /* Then we should set for the global state */
        terminal_buf = (char*)next_terminal->terminal_buf;

        /* Store for last task we are swicth from */
        task_t* crt = get_task_by_pid(terminal_list[current_running_terminal].current_task_pid);
        asm volatile("                  \n\
            movl %%ebp, %0        \n\
            movl %%esp, %1           \n\
            "
        : "=r"(crt->schd_ebp), "=r"(crt->schd_esp)
        :
        : "ebp", "esp");

        execute_as_root = 1;
        execute((const uint8_t*)"shell");
    }
}

/*
 * int32_t open(const uint8_t* filename);
 * Inputs: none
 * Return: 0
 * Function: initializes variables for terminal
 */
int32_t terminal_open(const uint8_t* filename) {
    terminal_t* current_terminal = &terminal_list[current_running_terminal];
    terminal_buf_size = 0;
    current_terminal -> enter_pressed_flag = 0;
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
        terminal_t* current_terminal = &terminal_list[get_current_task()->terminal_id];
        current_terminal->enter_pressed_flag = 0;
        terminal_buf_size = 0;
        /* kprintf("tss: 0x%x begin_terminal_read: task: %d current: %d %d %d, active: %d\n",tss.esp0, get_current_task()->pid, get_current_task()->terminal_id, current_terminal->tid, current_running_terminal, current_active_terminal); */

        while (!current_terminal->enter_pressed_flag) {
            current_terminal = &terminal_list[get_current_task()->terminal_id];
        }

        current_terminal->enter_pressed_flag = 0;
        /* kprintf("tss: 0x%x end_terminal_read: task: %d current: %d %d %d, active: %d\n",tss.esp0, get_current_task()->pid, get_current_task()->terminal_id, current_terminal->tid, current_running_terminal, current_active_terminal); */
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
        cli();
        for (i = 0; i < nbytes; i++) {
            if(((char*)buf)[i] == '\b' && length_of_print > 0) {
                length_of_print--;
                putc(((char*)buf)[i]);
            } else if (((char*)buf)[i] != '\b') {
                length_of_print++;
                putc(((char*)buf)[i]);
            }
        }
        sti();
        return length_of_print;
    }
    return -1;
}

/**
 * Operator define
 */
void init_terminal_operator() {
    terminal_operator.open = terminal_open;
    terminal_operator.read = terminal_read;
    terminal_operator.write = terminal_write;
    terminal_operator.close = terminal_close;
}
