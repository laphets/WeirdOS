#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"

void syscall_handler(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

#endif //MP3_GROUP_42_SYSCALL_H
