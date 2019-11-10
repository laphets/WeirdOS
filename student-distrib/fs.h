#ifndef _FS_H
#define _FS_H


#include "types.h"
#include "lib.h"
#include "vfs.h"
#include "task.h"

#define FS_DENTRY_FILE_NAME_OFFSET 33
#define FS_FILE_NAME_SIZE 32
#define FS_FILE_TYPE_FILE 2
#define FS_FILE_TYPE_DIR 1
#define FS_FILE_TYPE_RTC 0

#define FS_DENTRY_FILE_NAME_SIZE 32
#define FS_DENTRY_FILE_NAME_NULL_TERM_SIZE 33

typedef struct dentry {
    char file_name_bytes[FS_DENTRY_FILE_NAME_SIZE];
    uint32_t file_type;
    uint32_t inode_idx;
    uint8_t reserved[24];
    char file_name[FS_DENTRY_FILE_NAME_NULL_TERM_SIZE];
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
/**
typedef struct file_descriptor {
    void* file_operation_table_ptr;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flag;
} file_descriptor_t;
 */

/**
 * Init read-only file system
 * @param fs_start_addr start addr in memory
 * @param fs_end_addr end addr in memory
 */
void init_fs(uint32_t fs_start_addr, uint32_t fs_end_addr);

/**
 * Read dentry by file name
 * @param fname file_name
 * @param dentry the detry we want ro fill
 * @return -1 on failure(non-existent file or invalid index)
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

/**
 * Read dentry by index [0-62], 0 for directory
 * @param index index
 * @param dentry the detry we want ro fill
 * @return -1 on failure(non-existent file or invalid index)
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);

/**
 * Read data from certain inode
 * @param inode idx for inode
 * @param offset offset for the data
 * @param buf store location
 * @param length length we want to read
 * @return only check for inode in range
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/**
 * Read file size for inode
 * @param inode inode idx
 * @return the file size
 */
uint32_t read_file_size(uint32_t inode);

/**
 * Open a directory type file
 * @param filename should be '.'
 * @return 0 for success
 */
int32_t dir_open(const uint8_t *filename);

/**
 * Read current dir content(file_name)
 * @param fd file descriptor
 * @param buf buffer copy to
 * @param nbytes bytes length
 * @return -1 fail, or bytes copied
 */
int32_t dir_read(int32_t fd, void *buf, int32_t nbytes);

/**
 * Write current dir content(file_name)
 * @param fd file descriptor
 * @param buf buffer write from
 * @param nbytes bytes length
 * @return -1 fail
 */
int32_t dir_write(int32_t fd, const void *buf, int32_t nbytes);

/**
 * Close a directory
 * @param fd file_descriptor
 * @return 0 on success
 */
int32_t dir_close(int32_t fd);

/**
 * Open a file, initialize any temporary structures,
 * @param filename filename
 * @return 0 for success, -1 on fail
 */
int32_t file_open(const uint8_t *filename);

/**
 * Read current file content(file_name)
 * @param fd file descriptor
 * @param buf buffer copy to
 * @param nbytes bytes length
 * @return -1 fail, or bytes copied
 */
int32_t file_read(int32_t fd, void *buf, int32_t nbytes);

/**
 * Write current file content(file_name)
 * @param fd file descriptor
 * @param buf buffer write from
 * @param nbytes bytes length
 * @return -1 fail
 */
int32_t file_write(int32_t fd, const void *buf, int32_t nbytes);

/**
 * Close a file
 * @param fd file_descriptor
 * @return 0 on success
 */
int32_t file_close(int32_t fd);

/* Operator for file */
file_operator_t file_operator;

/* Operator for directory */
file_operator_t directory_operator;

/**
 * Init for file operator and its handler functions
 */
void init_file_operator();

/**
 * Init for directory operator and its handler functions
 */
void init_directory_operator();

#endif //MP3_GROUP_42_FS_H
