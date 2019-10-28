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

void init_fs(uint32_t fs_start_addr, uint32_t fs_end_addr);
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif //MP3_GROUP_42_FS_H
