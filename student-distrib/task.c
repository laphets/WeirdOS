#include "task.h"

/**
 * This fuction is going to initialize all the PCBs into kernel stack
 * The first task is starts at [8MB-8KB, 8MB]
 * The general task is at [8MB-(pid+1)*8KB, 8MB-(pid)*8KB]
 */
void init_tasks() {
    current_task_num = 0;
    uint32_t pid = 0;
    for(pid = 0; pid < MAX_TASK_NUM; pid++) {
        task_t task;
        task.present = 0;   /* Set to unpresent */
        task.parent = -1;
        task.fd_size = 0;
        task.argument_num = 0;
        int32_t fd = 0;
        for(fd = 0; fd < MAX_FD_NUM; fd++) {
            task.fd_table[fd].present = 0;
        }
        memcpy((void*)(KERNEL_BOTTOM-(pid+1)*PCB_BLOCK_SIZE), (const void*)&task, sizeof(task));
    }
}
/**
 * Set task for some pid
 * @param task task to set
 */
void set_task(task_t* task) {
    memcpy((void *)(KERNEL_BOTTOM - (task->pid + 1) * PCB_BLOCK_SIZE), (const void *)&task, sizeof(task));
}

/**
 * Get current task pcb by the esp position
 * @return pointer tp current pcb
 */
task_t* get_current_task() {
    uint32_t esp = 0;
    asm volatile("                  \n\
            movl %%esp, %0           \n\
            "
    : "=r"(esp)
    :
    : "esp");
    esp &= 0xffffe000; /* 8KB Mask */
    return (task_t*)esp;
}

/* Copy data from kernel to user space */
void* copy_to_user(int8_t* dest, const int8_t* src, uint32_t n) {
    return memcpy(dest, src, n);
}

/* Copy data from user space to kernel */
void* copy_from_user(int8_t* dest, const int8_t* src, uint32_t n) {
    return memcpy(dest, src, n);
}
