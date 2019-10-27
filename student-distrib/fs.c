#include "fs.h"

void* FS_START_ADDR = 0;
void* FS_END_ADDR = 0;

#define FS_INODE_ENTRY_SIZE 4
#define FS_BLOCK_SIZE 4096
#define FS_ENTRY_SIZE 64
#define FS_DENTRY_NUM_MAX 63

boot_block_t boot_block;



/**
 * Init read-only file system
 * @param fs_start_addr start addr in memory
 * @param fs_end_addr end addr in memory
 */
void init_fs(uint32_t fs_start_addr, uint32_t fs_end_addr) {
    printf("fs_start_addr: %x; fs_end_addr: %x\n", fs_start_addr, fs_end_addr);
    FS_START_ADDR = (void*)fs_start_addr;
    FS_END_ADDR = (void*)fs_end_addr;
    memcpy(&boot_block, FS_START_ADDR, FS_ENTRY_SIZE);
    printf("dir_entry_num: %d; inode_num: %d; data_block_num: %d;\n", boot_block.dir_entry_num, boot_block.inode_num, boot_block.data_block_num);
}


/* Basic file system interfaces */

/**
 * Read dentry by file name
 * @param fname file_name
 * @param dentry the detry we want ro fill
 * @return -1 on failure(non-existent file or invalid index)
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
    uint16_t i;
    char current_name[33];
    for(i = 0; i < boot_block.dir_entry_num; i++) {
        void* target = FS_START_ADDR + FS_ENTRY_SIZE + i * FS_ENTRY_SIZE;
        /**
         * Then extract current name and add a 0 padding to the end
         */
        memcpy(current_name, target, 32);
        current_name[32] = 0;
        if(strncmp((const char*)fname, (const char*)current_name, strlen((const char*)current_name)) == 0) {
            /* We find the dentry */
            memcpy(dentry, target, FS_ENTRY_SIZE);
            memcpy((void*)dentry+64, dentry, 32);
            dentry->file_name[32] = 0;
            return 0;
        }
    }
    return -1;
}

/**
 * Read dentry by index [0-62], 0 for directory
 * @param index index
 * @param dentry the detry we want ro fill
 * @return -1 on failure(non-existent file or invalid index)
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
    if(index >= boot_block.dir_entry_num)
        return -1;
    void* target = FS_START_ADDR + FS_ENTRY_SIZE + index * FS_ENTRY_SIZE;
    memcpy(dentry, target, FS_ENTRY_SIZE);
    memcpy((void*)dentry+64, dentry, 32);
    dentry->file_name[32] = 0;
    return 0;
}

static int32_t get_min(uint32_t a, uint32_t b) {
    return a < b ? a : b;
}

/**
 * Read data from certain inode
 * @param inode idx for inode
 * @param offset offset for the data
 * @param buf store location
 * @param length length we want to read
 * @return only check for inode in range
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    if(inode >= boot_block.inode_num) {
        return -1;
    }
    uint32_t read_num = 0;
    uint32_t* inode_target = (uint32_t*)(FS_START_ADDR + FS_BLOCK_SIZE /* boot block */ + FS_BLOCK_SIZE * inode);
    uint32_t data_length = inode_target[0];

    /**
     * Then we check whether it has reached end of the file
     */
    if((offset + length) >= data_length) {
        return 0;
    }

    void* data_block_start_addr = FS_START_ADDR + (boot_block.inode_num+1) * FS_BLOCK_SIZE;

    uint32_t inode_block_idx = offset / FS_BLOCK_SIZE;
    uint32_t inode_block_offset = offset % FS_BLOCK_SIZE;
    uint32_t orig_inode_block_offset = inode_block_offset;

    while(read_num < length) {
        if(inode_block_idx*FS_BLOCK_SIZE >= data_length) {
            break;
        }
        uint32_t data_block_idx = inode_target[1 + inode_block_idx];
        void* data_block_addr = data_block_start_addr + data_block_idx * FS_BLOCK_SIZE + inode_block_offset;
        uint32_t desired_data_before_last_block = (1 + inode_block_idx) * FS_BLOCK_SIZE - orig_inode_block_offset - read_num;
        uint32_t block_desired_data_size = get_min(desired_data_before_last_block, length - read_num);
        inode_block_offset = 0;

        if ((length - read_num) <= block_desired_data_size)
        {
            /**
             * All the data is in this block
             * We're done
             */
            memcpy((void *)((uint32_t)buf + read_num), data_block_addr, block_desired_data_size);
            read_num += block_desired_data_size;
            break;
        } else {
            /**
             * We still need some new block
             */
            memcpy((void *)((uint32_t)buf + read_num), data_block_addr, block_desired_data_size);
            read_num += block_desired_data_size;
            inode_block_idx++;
        }
    }

    return read_num;
}


/** Wrapper file system interface */
int32_t dir_open(const uint8_t* filename) {
    dentry_t file_info;
    read_dentry_by_name(filename, file_info);
    return 0;
}

int32_t dir_read(int32_t fd, void* buf, int32_t nbytes) {
    dentry_t file_info;
    read_dentry_by_name(filename, file_info);
    return 0;
}

int32_t dir_write(int32_t fd, const void *buf, int32_t nbytes) {
    return -1;
}

int32_t dir_close(int32_t fd) {
    return 0;
}

int32_t file_open(const uint8_t *filename) {
    return 0;
}

int32_t file_read(int32_t fd, void *buf, int32_t nbytes) {
    
    return read_data(, 0, buf, nbytes);
}

int32_t file_write(int32_t fd, const void *buf, int32_t nbytes) {
    return -1;
}

int32_t file_close(int32_t fd) {
    return 0;
}
