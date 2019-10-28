#ifndef _FS_H
#define _FS_H


#include "types.h"
#include "lib.h"

#define FS_DENTRY_FILE_NAME_OFFSET 33
#define FS_DENTRY_FILE_NAME_SIZE 32


typedef struct dentry {
    char file_name_bytes[32];
    uint32_t file_type;
    uint32_t inode_idx;
    uint8_t reserved[24];
    char file_name[33];
} dentry_t;

typedef struct boot_block {
    uint32_t dir_entry_num;
    uint32_t inode_num;
    uint32_t data_block_num;
    uint8_t reserved[52];
} boot_block_t;

typedef struct inode {
    int32_t length;
    int32_t data_block_num[1023];
} inode;


/* typedef enum file_type{STDIN_T, STDOUT_T, DIRECTORY_T, FILE_T, RTC_T} file_type_t; */

typedef struct file_descriptor {
    void* file_operation_table_ptr;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flag;
} file_descriptor_t;

void init_fs(uint32_t fs_start_addr, uint32_t fs_end_addr);
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
uint32_t read_file_size(uint32_t inode);

int32_t dir_open(const uint8_t *filename);
int32_t dir_read(int32_t fd, void *buf, int32_t nbytes);
int32_t dir_write(int32_t fd, const void *buf, int32_t nbytes);
int32_t dir_close(int32_t fd);

int32_t file_open(const uint8_t *filename);
int32_t file_read(int32_t fd, void *buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void *buf, int32_t nbytes);
int32_t file_close(int32_t fd);

#endif //MP3_GROUP_42_FS_H
