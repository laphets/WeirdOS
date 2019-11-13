#ifndef _SYSCALL_H
#define _SYSCALL_H

/**
 * Size for syscall, will be referenced from asm linkage
 */
#define SYSCALL_SIZE 10

#ifndef ASM

#include "types.h"
#include "lib.h"
#include "fs.h"
#include "paging.h"
#include "terminal.h"
#include "task.h"
#include "rtc.h"

/**
 * Placeholder for 0 syscall
 * @return always fail for this call, let's say -1
 */
int32_t syscall0();

/**
 * Syscall for executing a command
 * For context switch, we should do the following things:
 * Set up EIP, CS, EFLAGS, ESP, SS
 * CS -> USER_CS
 * SS -> USER_DS
 * Finally, we need set TSS to new kernel-mode stack pointer(set to the new allocated kernel-stack?)
 * Our userspace pid should start at 1
 * @param command command string to execute
 * @return status, -1 for fail
 */
int32_t execute(const uint8_t* command);

/**
 * Halt for a process
 * @param status the return value to return from execute
 * @return status, -1 for fail
 */
int32_t halt(uint8_t status);

/**
 * Syscall for read from a file to some buffer
 * @param fd file descriptor
 * @param buf buffer to read to
 * @param nbytes bytes to read
 * @return status, -1 for fail
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes);

/**
 * Syscall for write to a file by some file descriptor
 * @param fd file descriptor
 * @param buf buffer write from
 * @param nbytes
 * @return
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes);

/**
 * Syscall for open a file
 * @param filename file to open
 * @return -1 if open fail
 */
int32_t open(const uint8_t* filename);

/**
 * Close the file descriptor and free that for later usage
 * @param fd file descriptor
 * @return status: -1 for fail
 */
int32_t close(int32_t fd);

/**
 * Getargs syscall to get current process arguments
 * @param buf the argument string will return to
 * @param nbytes bytes to copy
 * @return status, -1 for fail
 */
int32_t getargs(uint8_t* buf, int32_t nbytes);

/**
 * Not implemented yet
 * @param screen_start
 * @return
 */
int32_t vidmap(uint8_t** screen_start);


/**
 * Signal handling system calls. Currently return failure
 * upon call
 */
 int32_t set_handler(int32_t signum, void* handler_address);
 int32_t sigreturn(void);

/**
 * Jump table for syscalls
 */
void* syscall_list[SYSCALL_SIZE+1];

/**
 * Method to init syscall into jump table
 */
void init_syscall_list();

#endif

#endif //MP3_GROUP_42_SYSCALL_H
