#include "syscall.h"

void syscall_handler(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {

}


int32_t read(int32_t fd, void* buf, int32_t nbytes) {
    return 0;
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    return 0;
}

int32_t open(const uint8_t* filename) {
    return 0;
}

int32_t close(int32_t fd) {
    return 0;
}
