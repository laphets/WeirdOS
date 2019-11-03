#include "fs.h"

void* FS_START_ADDR = 0;
void* FS_END_ADDR = 0;

#define FS_INODE_ENTRY_SIZE 4
#define FS_BLOCK_SIZE 4096
#define FS_ENTRY_SIZE 64
#define FS_DENTRY_NUM_MAX 63
#define FD_TABLE_SIZE 20

boot_block_t boot_block;

/**
 * Array for file descriptor(should begin from 2)
 */
file_descriptor_t file_descriptor_table[FD_TABLE_SIZE];

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
    char current_name[FS_DENTRY_FILE_NAME_NULL_TERM_SIZE];
    for(i = 0; i < boot_block.dir_entry_num; i++) {
        void* target = FS_START_ADDR + FS_ENTRY_SIZE + i * FS_ENTRY_SIZE;
        /**
         * Then extract current name and add a 0 padding to the end
         */
        memcpy(current_name, target, FS_DENTRY_FILE_NAME_SIZE);
        current_name[FS_DENTRY_FILE_NAME_SIZE] = 0;
        if(strncmp((const char*)fname, (const char*)current_name, strlen((const char*)current_name)) == 0) {
            /* We find the dentry */
            memcpy(dentry, target, FS_ENTRY_SIZE);
            memcpy((void *)dentry + FS_ENTRY_SIZE, dentry, FS_DENTRY_FILE_NAME_SIZE);
            dentry->file_name[FS_DENTRY_FILE_NAME_SIZE] = 0;
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
    memcpy((void *)dentry + FS_ENTRY_SIZE, dentry, FS_DENTRY_FILE_NAME_SIZE);
    dentry->file_name[FS_DENTRY_FILE_NAME_SIZE] = 0;
    return 0;
}

static int32_t get_min(uint32_t a, uint32_t b) {
    return a < b ? a : b;
}

/**
 * Read file size for inode
 * @param inode inode idx
 * @return the file size
 */
uint32_t read_file_size(uint32_t inode) {
    uint32_t* inode_target = (uint32_t*)(FS_START_ADDR + FS_BLOCK_SIZE /* boot block */ + FS_BLOCK_SIZE * inode);
    uint32_t file_size = inode_target[0];
    return file_size;
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
    uint32_t file_size = inode_target[0];

    /**
     * Then we check whether it has reached end of the file
     */
    if(offset >= file_size) {
        return 0;
    }

    void* data_block_start_addr = FS_START_ADDR + (boot_block.inode_num+1) * FS_BLOCK_SIZE;

    uint32_t inode_block_idx = offset / FS_BLOCK_SIZE;
    uint32_t inode_block_offset = offset % FS_BLOCK_SIZE;

    while(read_num < length) {
        /* Exceed the file size */
        if(inode_block_idx*FS_BLOCK_SIZE >= file_size) {
            break;
        }
        uint32_t data_block_idx = inode_target[1 + inode_block_idx];
        void* data_block_addr = data_block_start_addr + data_block_idx * FS_BLOCK_SIZE + inode_block_offset;
        uint32_t remain_data_length = file_size - inode_block_idx * FS_BLOCK_SIZE;
        uint32_t current_block_size = get_min(FS_BLOCK_SIZE, remain_data_length) - inode_block_offset;
        inode_block_offset = 0;

        uint32_t reamin_bytes_to_read = length-read_num;
        if(reamin_bytes_to_read <= current_block_size) {
            /**
             * All the data is in this block
             * We're done
             */
            memcpy((void*)((uint32_t)buf+read_num), data_block_addr, reamin_bytes_to_read);
            read_num += reamin_bytes_to_read;
            break;
        } else {
            /**
             * We still need some new block
             */
            memcpy((void *)((uint32_t)buf + read_num), data_block_addr, current_block_size);
            read_num += current_block_size;
            inode_block_idx++;
        }
    }

    return read_num;
}


/** Wrapper file system interface */

/**
 * Open a directory type file
 * @param filename should be '.'
 * @return 0 for success
 */
int32_t dir_index = 0;
int32_t dir_open(const uint8_t* filename) {
    dir_index = 0;
    dentry_t file_info;
    return read_dentry_by_name(filename, &file_info);
}

/**
 * Read current dir content(file_name)
 * @param fd file descriptor
 * @param buf buffer copy to
 * @param nbytes bytes length
 * @return -1 fail, or bytes copied
 */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes) {
    dentry_t file_info;
    if(dir_index >= boot_block.dir_entry_num)
        return 0;
    if(read_dentry_by_index(dir_index, &file_info) == -1) {
        return -1;
    }
    uint32_t file_name_length = strlen(file_info.file_name);
    int32_t bytes_to_copy = nbytes < file_name_length ? nbytes : file_name_length;
    memcpy(buf, file_info.file_name, bytes_to_copy);
    dir_index++;
    return bytes_to_copy;
}

/**
 * Write current dir content(file_name)
 * @param fd file descriptor
 * @param buf buffer write from
 * @param nbytes bytes length
 * @return -1 fail
 */
int32_t dir_write(int32_t fd, const void *buf, int32_t nbytes) {
    return -1;
}

/**
 * Close a directory
 * @param fd file_descriptor
 * @return 0 on success
 */
int32_t dir_close(int32_t fd) {
    return 0;
}

/**
 * Open a file, initialize any temporary structures,
 * @param filename filename
 * @return 0 for success, -1 on fail
 */
int32_t read_offset;
dentry_t open_file_info;
int32_t file_open(const uint8_t* filename) {
    read_offset = 0;
    return read_dentry_by_name(filename, &open_file_info);
}

/**
 * Read current file content(file_name)
 * @param fd file descriptor
 * @param buf buffer copy to
 * @param nbytes bytes length
 * @return -1 fail, or bytes copied
 */
int32_t file_read(int32_t fd, void *buf, int32_t nbytes) {
    int32_t bytes_read = read_data(open_file_info.inode_idx, read_offset, buf, nbytes);
    read_offset += bytes_read;
    return bytes_read;
}

/**
 * Write current file content(file_name)
 * @param fd file descriptor
 * @param buf buffer write from
 * @param nbytes bytes length
 * @return -1 fail
 */
int32_t file_write(int32_t fd, const void *buf, int32_t nbytes) {
    return -1;
}

/**
 * Close a file
 * @param fd file_descriptor
 * @return 0 on success
 */
int32_t file_close(int32_t fd) {
    read_offset = 0;
    return 0;
}
