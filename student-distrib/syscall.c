#include "syscall.h"

/**
 * Init syscall list
 */
void init_syscall_list() {
    syscall_list[0] = NULL;
    syscall_list[1] = halt;
    syscall_list[2] = execute;
    syscall_list[3] = read;
    syscall_list[4] = write;
    syscall_list[5] = open;
    syscall_list[6] = close;
    syscall_list[7] = getargs;
    syscall_list[8] = vidmap;
}

/**
 * For context switch, we should do the following things:
 * Set up EIP, CS, EFLAGS, ESP, SS
 * CS -> USER_CS
 * SS -> USER_DS?
 * Finally, we need set TSS to new kernel-mode stack pointer(set to the new allocated kernel-stack?)
 * Our userspace pid should start at 1
 */

int32_t execute(const uint8_t* command) {
    /**
     * First we should parse the command
     * Then check for the file
     * Then set up paging for our new program
     * Then we should read and load the file into that memory
     * Then we should create PCB into kernel stack, and set up 0,1 fd
     * Then we should prepare for context switch and push the target iret arguments
     * Finally, we use iret to jump to target program
     * As for the userspace stack, we just set to the button of the 4MB memory
     * ....
     * After executing halt, we will come here with return, then we return the value
     */

    cli();
    /* Init for task struct */
    task_t task;
    task.present = 1;

    if(current_task_num == 0) {
        /**
         * First userspace program
         */
        task.parent = -1;
        task.parent_addr = NULL;
        task.pid = 0;
    } else {
        /* Get current task info */
        task_t* current_task = get_current_task();
        task.parent = current_task->pid;
        task.parent_addr = current_task;
        task.pid = current_task->pid+1;
    }


    /* Parse command */
    /**
     * TODO: We need some length checking here
     */

    uint32_t it = 0;
    uint32_t cmd_length = strlen((const int8_t*)command);
    task.full_argument[0] = '\0';
    for(it = 0; it < cmd_length; it++) {
        if(command[it] == ' ') {
            strcpy((int8_t*)task.full_argument, (const int8_t*)(command + it + 1));
            break;
        }
    }

    task.argument_num = 0;
    int8_t* token = strtok((int8_t*)command, (const int8_t*)" ");
    if (token != NULL) {
        memcpy((void*)task.name, (const void*)token, strlen((const int8_t*)token));
        token = strtok(NULL, " ");
    }
    while(token != NULL) {
        memcpy((void*)task.argument_list[task.argument_num++], (const void*)token, strlen((const int8_t*)token));
        token = strtok(NULL, (const int8_t*)" ");
    }

    /* printf("task_name: %s %d\n", task.name, strlen(task.name)); */

    /* Then we check for the file */
    dentry_t dentry;
    if (read_dentry_by_name((const uint8_t*)task.name, &dentry) == -1) {
        printf("read_dentry_by_name fail\n");
        return -1;
    }
    /* Check for the file type */
    if(dentry.file_type != FS_FILE_TYPE_FILE) {
        return -1;
    }
    /* Check for first 4 bytes for magic number */
    /* 0x7f 0x45 0x4c 0x46 */
    uint8_t magic_buf[4];
    if (read_data(dentry.inode_idx, 0, magic_buf, 4) != 4) {
        printf("read_data fail\n");
        return -1;
    }

    /* printf("magic_buf: 0x%x 0x%x 0x%x 0x%x\n", magic_buf[0], magic_buf[1], magic_buf[2], magic_buf[3]); */

    if (magic_buf[0] != 0x7f || magic_buf[1] != 0x45 || magic_buf[2] != 0x4c || magic_buf[3] != 0x46) {
        printf("magic_buf checking fail\n");
        return -1;
    }

    /* Then read for the entry point */
    void* entry_point = NULL;
    if (read_data(dentry.inode_idx, 24, (uint8_t*)&entry_point, 4) != 4) {
        printf("read entry_point fail\n");
        return -1;
    }
    /* printf("entry_point: 0x%x\n", entry_point); */

    /* Copy for the name */
    strcpy(task.name, dentry.file_name);

    /* Then set up paging for our new program */
    /* It should be start at va:128MB */
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
    user_page_directory->address = (uint32_t)((uint32_t)(2+task.pid) << 10); /* TODO: Set to the 8 + pid * (4) MB */

    /* printf("refreshing paging...: 0x%x\n", *user_page_directory); */

    /* TODO: Refresh the paging */
    /* IDK */
    flush_paging();


    /* Then we should read and load the image into that location */
    /* 0x08048000 */
    uint32_t read_offset = 0;
    uint32_t tmp_buf_size = 1000;   /* bug: if we set to 3000, it will crash */
    uint8_t tmp_buf[tmp_buf_size];
    int32_t bytes_read;
    while (1) {
        bytes_read = read_data(dentry.inode_idx, read_offset, tmp_buf, tmp_buf_size);
        if(bytes_read == -1) {
            return -1;
        }
        if(bytes_read == 0) {
            break;
        }
        /* printf("bytes_read: %d\n", bytes_read); */
        /* Then we copy those data */
        memcpy((void*)(IMAGE_START_ADDR + read_offset), (const void*)tmp_buf, (uint32_t)bytes_read);
        read_offset += bytes_read;
    }

    /* read_data(dentry.inode_idx, 0, (uint8_t*)IMAGE_START_ADDR, 300000); */

    /* printf("copy image data done\n"); */

    /* Then we initialize the file descriptor table */
    task.fd_size = 2;
    /* For stdin */
    task.fd_table[0].operator = &terminal_operator;
    task.fd_table[0].file_position = 0;
    task.fd_table[0].inode = -1;
    task.fd_table[0].flag = 0;
    task.fd_table[0].present = 1;
    /* For stdout */
    task.fd_table[1].operator = &terminal_operator;
    task.fd_table[1].file_position = 0;
    task.fd_table[1].inode = -1;
    task.fd_table[1].flag = 0;
    task.fd_table[1].present = 1;

    /* printf("init default fd_table done\n"); */

    /* We should modify the TSS */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = KERNEL_BOTTOM - task.pid * PCB_BLOCK_SIZE ;

    /* printf("changing TSS done\n"); */

    /* Update the current task num */
    current_task_num++;

    /* Record currrent esp and ebp state */
    asm volatile("                  \n\
            movl %%ebp, %0        \n\
            movl %%esp, %1           \n\
            "
    : "=r"(task.prev_ebp), "=r"(task.prev_esp)
    :
    : "ebp", "esp");

    /* printf("store task stack done\n"); */

    /* Then we should setup PCB */
    memcpy((void*)(KERNEL_BOTTOM-(task.pid+1)*PCB_BLOCK_SIZE), (const void*)&task, sizeof(task));

    /* printf("setup pcb done\n"); */

    /* prepare for context switch and push the target iret arguments */
    /* The order we push should be: SS, ESP, EFLAGS, CS, EIP */
    /* The ESP addr should be the bottom of program image chunk, let's say 132MB = 0x8400000 */
    asm volatile ("                  \n\
            cli           \n\
            movl %0, %%eax           \n\
            movw %%ax, %%ds           \n\
            movw %%ax, %%es           \n\
            movw %%ax, %%fs           \n\
            movw %%ax, %%gs           \n\
            pushl %0           \n\
            pushl %1        \n\
            pushfl        \n\
            popl %%eax        \n\
            orl $0x200, %%eax        \n\
            pushl %%eax        \n\
            pushl %2   \n\
            pushl %3        \n\
            iret        \n\
            EXECUTE_RET: \n\
            leave \n\
            ret \n\
            "
            :
            : "r"(USER_DS), "r"(TASK_STACK_SATRT_ADDR), "r"(USER_CS), "r"(entry_point)
            : "memory", "cc", "eax"
    );

    /* We should never go here for return */
    return -1;
}

int32_t halt(uint8_t status) {
    /**
     * We should first figure out the current executing process
     * Restore parent data, restore parent paging
     * Close filedescriptors
     * Then we jump to return location of system_execute
     */
    /* printf("sys_halt...\n"); */
    task_t* current_task = get_current_task();
    /**
     * If it's the first task, we can't do halt
     */
    if(current_task->parent == -1) {
        return -1;
    }

    task_t* parent_task = (task_t*)current_task->parent_addr;

    /* Then we just setback the tss */
    tss.esp0 = KERNEL_BOTTOM - parent_task->pid * PCB_BLOCK_SIZE ;

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
    user_page_directory->address = ((2+parent_task->pid) << 10);

    flush_paging();

    /* Clean up data for current process */

    /* Close all the open files */
    int32_t fd = 0;
    for(fd = 0; fd < MAX_FD_NUM; fd++) {
        if(current_task->fd_table[fd].present == 1) {
            current_task->fd_table[fd].operator->close(fd); /* TODO: DO we need some error handling here */
            current_task->fd_table[fd].present = 0;
        }
    }
    current_task->fd_size = 0;
    current_task->present = 0;
    current_task_num--;

    /* Then jump to execute return */
    asm volatile ("                  \n\
            movl %0, %%eax            \n\
            movl %1, %%ebp        \n\
            movl %2, %%esp \n\
            jmp EXECUTE_RET \n\
            "
    :
    : "r"((uint32_t)status), "r"(current_task->prev_ebp), "r"(current_task->prev_esp)
    : "memory", "cc", "eax"
    );

    /* We should be successfully jmp, otherwise, return -1 */
    return -1;
}


int32_t read(int32_t fd, void* buf, int32_t nbytes) {
    /**
     * Resolve the file handler according to fd
     * After that, we call the read fuction to buf
     * (copy_to_user needed?)
     */
    /* printf("sys_read...\n"); */


    /* Check for the fd range, we can't read stdout */
    if(fd < 0 || fd >= MAX_FD_NUM || fd == 1) {
        return -1;
    }
    task_t* current_task = get_current_task();
    file_desc_t* fd_entry = &current_task->fd_table[fd];

    if(fd_entry->present != 1) {
        return -1;
    }

    int32_t ret = fd_entry->operator->read(fd, buf, nbytes);
    /* printf("sys_read: code: %d\n", ret); */

    return ret;
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    /**
     * First resolve the file handler according to fd
     * After that, we can call write function from the pointer of buf
     * (Seems like we may consider copy_from_user?)
     */
     /* printf("sys_write...\n"); */

    /* Check for the fd range, we can't write stdin */
    if(fd < 0 || fd >= MAX_FD_NUM || fd == 0) {
        return -1;
    }

    task_t* current_task = get_current_task();
    file_desc_t* fd_entry = &current_task->fd_table[fd];

    if(fd_entry->present != 1) {
        return -1;
    }

    int32_t ret = fd_entry->operator->write(fd, buf, nbytes);
    /* printf("sys_write: code: %d\n", ret); */
    return ret;
}

int32_t open(const uint8_t* filename) {
    /**
     * We first try to resolve the filename(by first listing the directory, then find the file)
     * then we allocate an unuseful file descriptor
     * then we set up data necessary to handler that given type of file
     * If descriptor is full or the file doesn't exist, we return -1
     */

    /* printf("sys_open...\n"); */

    task_t* current_task = get_current_task();

    dentry_t dentry;
    if (read_dentry_by_name(filename, &dentry) == -1) {
        return -1;
    }

    /* Then we allocate the file descriptor */
    if(current_task->fd_size >= MAX_FD_NUM) {
        return -1;
    }

    int32_t fd = 0;
    for(fd = 0; fd < MAX_FD_NUM; fd++) {
        if(current_task->fd_table[fd].present == 0) {
            break;
        }
    }

    file_desc_t* fd_entry = &current_task->fd_table[fd];
    fd_entry->present = 1;
    fd_entry->file_position = 0;
    fd_entry->inode = dentry.inode_idx;
    fd_entry->flag = 0;


    switch (dentry.file_type) {
        case FS_FILE_TYPE_RTC: {
            fd_entry->operator = &rtc_operator;
            /* printf("Open RTC...\n"); */
            break;
        }
        case FS_FILE_TYPE_DIR: {
            fd_entry->operator = &directory_operator;
            /* printf("Open DIR...\n"); */
            break;
        }
        case FS_FILE_TYPE_FILE: {
            fd_entry->operator = &file_operator;
            /* printf("Open FILE...\n"); */
            break;
        }
        default:
            /* Not support file */
            return -1;
    }

    /* Then we call the open function if necessary */
    fd_entry->operator->open(filename);

    return fd;
}

int32_t close(int32_t fd) {
    /**
     * Close the file descriptor and free that for later usage
     * if the fd is invalid(0,1,...), we should return -1
     */

    /* printf("sys_close...\n"); */

    /* User shouldn't close stdin and stdout */
    if(fd <= 1 || fd >= MAX_FD_NUM) {
        return -1;
    }

    task_t* current_task = get_current_task();
    file_desc_t* fd_entry = &current_task->fd_table[fd];

    /* Check for the fd availablity */
    if(fd_entry->present != 1) {
        return -1;
    }

    /* Call the clean up handler */
    if (fd_entry->operator->close(fd) == -1) {
        return -1;
    }

    /* Then we do some munually clean up */
    fd_entry->operator = NULL;
    fd_entry->flag = 0;
    fd_entry->inode = -1;
    fd_entry->file_position = 0;
    fd_entry->present = 0;
    current_task->fd_size--;

    return 0;
}

int32_t getargs(uint8_t* buf, int32_t nbytes) {
    task_t* current_task = get_current_task();
    if (current_task->argument_num == 0 || strlen(current_task->full_argument)+1 > nbytes) {
        return -1;
    }
    strcpy((int8_t*)buf, (const int8_t*)current_task->full_argument);
    return 0;
}

/**
 * Not implemented yet
 * @param screen_start
 * @return
 */
int32_t vidmap(uint8_t** screen_start) {
    return -1;
}
