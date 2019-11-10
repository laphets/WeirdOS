#ifndef _VFS_H
#define _VFS_H

#include "types.h"

/* typedef for vfs operator interfaces */
typedef int32_t (*open_t)(const uint8_t* filename);
typedef int32_t (*read_t)(int32_t fd, void *buf, int32_t nbytes);
typedef int32_t (*write_t)(int32_t fd, const void *buf, int32_t nbytes);
typedef int32_t (*close_t)(int32_t fd);

/**
 * File operator interface (oop like)
 */
typedef struct file_operator_s {
    open_t open;
    read_t read;
    write_t write;
    close_t close;
} file_operator_t;

/**
 * File descriptor element
 */
typedef struct file_desc {
    uint8_t present;
    file_operator_t* operator;
    int32_t inode;
    uint32_t file_position;     /* Read position */
    uint32_t flag;
} file_desc_t;

#endif //_VFS_H
