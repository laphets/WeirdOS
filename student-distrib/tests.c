#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "keyboard.h"
#include "paging.h"
#include "terminal.h"
#include "types.h"
#include "fs.h"
#include "rtc.h"
#include "task.h"
#include "syscall.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER \
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result) \
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure()
{
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

/* Checkpoint 1 tests */

/* IDT Test
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test()
{
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i) {
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)) {
			assertion_failure();
			result = FAIL;
		}
	}

	if ((idt[KEYBOARD_IDT].offset_15_00 == NULL) &&			//check for keyboard call
			(idt[KEYBOARD_IDT].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}

		if ((idt[RTC_IDT].offset_15_00 == NULL) &&			//check for RTC
			(idt[RTC_IDT].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}

		if ((idt[SYSCALL_IDT].offset_15_00 == NULL) &&		//check for Sys call
			(idt[SYSCALL_IDT].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	return result;
}

/**
 * Dereference for a point
 * @param a a pointer
 * @return the value the pointer point to
 */
int deref(int *a)
{
	return *a;
}
/* Paging Test
 *
 * Asserts that video paging entry is present
 * Asserts Register values are correct(cr0, cr3, cr4)
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging Initizlize
 * Files: paging.h/S
 */
int paging_test()
{
	TEST_HEADER;
	int result = PASS;
	unsigned long reg;

	// Checks for video memory mapping
	if (!((&first_page_table[VIDEO_MEMORY_START])->present))
	{
		// printf("%x in video memory is not mapped\n", i);
		assertion_failure();
		result = FAIL;
	}
	deref((int*) VIDEO_MEMORY_START_ADDRESS); // Accesses the first element in video memory. Should not blue screen
	// deref((int*)-100); // Will cause the kernel to panic "blue screen"
	// deref(NULL); // Will cause the kernel to panic "blue screen"
	int b = 391;
	int *a = &b;

	// Checks for
	if (b != deref(a))
	{
		result = FAIL;
	}
	// Checks if cr3 is set to the page directory
	asm volatile("                  \n\
            movl %%cr3, %%eax        \n\
            movl %%eax, %0           \n\
            "
				 : "=r"(reg)
				 :
				 : "eax");
	if ((void *)reg != default_page_directory)
	{
		// printf("CR3 does not equal our page directory\n");
		assertion_failure();
		result = FAIL;
	}
	// printf("Page Directory = %x\nCR3 = %x\n", default_page_directory, reg);
	asm volatile("                  \n\
            movl %%cr4, %%eax        \n\
            movl %%eax, %0           \n\
            "
				 : "=r"(reg)
				 :
				 : "eax");
	if (!(reg & 0x00000010))
	{
		// printf("Paging not enabled in CR4\n");
		assertion_failure();
		result = FAIL;
	}
	// printf("CR4 = %x\n", reg);
	asm volatile("                  \n\
            movl %%cr0, %%eax        \n\
            movl %%eax, %0           \n\
            "
				 : "=r"(reg)
				 :
				 : "eax");
	if (!(reg & 0x80000000))
	{
		// printf("Paging not enabled in CR0\n");
		assertion_failure();
		result = FAIL;
	}
	// printf("CR0 = %x\n", reg);
	return result;
}

/*
 * Turn a scancode to a ascii character
 * Input: scancode number
 * Output: ascii number
 */
int keyboard_handler_tester(uint8_t scancode) {
	return scancode2char(scancode);
}

/*
 * Tests the mapping of scancodes to ascii
 * Ouput: PASS if none of the tests fail
 * 		  FAIL is any of the tests fail
 * Side Effects: None
 * Coverage: scancode translation
 * Files: keyboard.h/S
 */
int keyboard_translation_test() {
	TEST_HEADER;
	int result = PASS;
	if (0 != keyboard_handler_tester(0x0)) {
		printf("0x0");result = FAIL;
	}
	if (27 != keyboard_handler_tester(0x1)) {
		printf("0x1");result = FAIL;
	}
	if ('1' != keyboard_handler_tester(0x2)) {
		printf("0x2"); result = FAIL;
	}
	if ('2' != keyboard_handler_tester(0x3)) {
		printf("0x3"); result = FAIL;
	}
	if ('3' != keyboard_handler_tester(0x4)) {
		printf("0x4"); result = FAIL;
	}
	if ('4' != keyboard_handler_tester(0x5)) {
		printf("0x5"); result = FAIL;
	}
	if ('5' != keyboard_handler_tester(0x6)) {
		printf("0x6"); result = FAIL;
	}
	if ('6' != keyboard_handler_tester(0x7)) {
		printf("0x7"); result = FAIL;
	}
	if ('7' != keyboard_handler_tester(0x8)) {
		printf("0x8"); result = FAIL;
	}
	if ('8' != keyboard_handler_tester(0x9)) {
		printf("0x9"); result = FAIL;
	}
	if ('9' != keyboard_handler_tester(0xA)) {
		printf("0xA"); result = FAIL;
	}
	if ('0' != keyboard_handler_tester(0xB)) {
		printf("0xB"); result = FAIL;
	}
	if ('-' != keyboard_handler_tester(0xC)) {
		printf("0xC"); result = FAIL;
	}
	if ('=' != keyboard_handler_tester(0xD)) {
		printf("0xD"); result = FAIL;
	}
	if ('\b' != keyboard_handler_tester(0xE)) {
		printf("0xE"); result = FAIL;
	}
	if ('\t' != keyboard_handler_tester(0xF)) {
		printf("0xF"); result = FAIL;
	}
	if ('q' != keyboard_handler_tester(0x10)) {
		printf("0x10"); result = FAIL;
	}
	if ('w' != keyboard_handler_tester(0x11)) {
		printf("0x11"); result = FAIL;
	}
	if ('e' != keyboard_handler_tester(0x12)) {
		printf("0x12"); result = FAIL;
	}
	if ('r' != keyboard_handler_tester(0x13)) {
		printf("0x13"); result = FAIL;
	}
	if ('t' != keyboard_handler_tester(0x14)) {
		printf("0x14"); result = FAIL;
	}
	if ('y' != keyboard_handler_tester(0x15)) {
		printf("0x15"); result = FAIL;
	}
	if ('u' != keyboard_handler_tester(0x16)) {
		printf("0x16"); result = FAIL;
	}
	if ('i' != keyboard_handler_tester(0x17)) {
		printf("0x17"); result = FAIL;
	}
	if ('o' != keyboard_handler_tester(0x18)) {
		printf("0x18"); result = FAIL;
	}
	if ('p' != keyboard_handler_tester(0x19)) {
		printf("0x19"); result = FAIL;
	}
	if ('[' != keyboard_handler_tester(0x1A)) {
		printf("0x1A"); result = FAIL;
	}
	if (']' != keyboard_handler_tester(0x1B)) {
		printf("0x1B"); result = FAIL;
	}
	if ('\n' != keyboard_handler_tester(0x1C)) {
		printf("0x1C"); result = FAIL;
	}
	if ('a' != keyboard_handler_tester(0x1E)) {
		printf("0x1E"); result = FAIL;
	}
	if ('s' != keyboard_handler_tester(0x1F)) {
		printf("0x1F"); result = FAIL;
	}
	if ('d' != keyboard_handler_tester(0x20)) {
		printf("0x20"); result = FAIL;
	}
	if ('f' != keyboard_handler_tester(0x21)) {
		printf("0x21"); result = FAIL;
	}
	if ('g' != keyboard_handler_tester(0x22)) {
		printf("0x22"); result = FAIL;
	}
	if ('h' != keyboard_handler_tester(0x23)) {
		printf("0x23"); result = FAIL;
	}
	if ('j' != keyboard_handler_tester(0x24)) {
		printf("0x24"); result = FAIL;
	}
	if ('k' != keyboard_handler_tester(0x25)) {
		printf("0x25"); result = FAIL;
	}
	if ('l' != keyboard_handler_tester(0x26)) {
		printf("0x26"); result = FAIL;
	}
	if (';' != keyboard_handler_tester(0x27)) {
		printf("0x27"); result = FAIL;
	}
	if ('\'' != keyboard_handler_tester(0x28)) {
		printf("0x28"); result = FAIL;
	}
	if ('`' != keyboard_handler_tester(0x29)) {
		printf("0x29"); result = FAIL;
	}
	if ('\\' != keyboard_handler_tester(0x2B)) {
		printf("0x2B"); result = FAIL;
	}
	if ('z' != keyboard_handler_tester(0x2C)) {
		printf("0x2C"); result = FAIL;
	}
	if ('x' != keyboard_handler_tester(0x2D)) {
		printf("0x2D"); result = FAIL;
	}
	if ('c' != keyboard_handler_tester(0x2E)) {
		printf("0x2E"); result = FAIL;
	}
	if ('v' != keyboard_handler_tester(0x2F)) {
		printf("0x2F"); result = FAIL;
	}
	if ('b' != keyboard_handler_tester(0x30)) {
		printf("0x30"); result = FAIL;
	}
	if ('n' != keyboard_handler_tester(0x31)) {
		printf("0x31"); result = FAIL;
	}
	if ('m' != keyboard_handler_tester(0x32)) {
		printf("0x32"); result = FAIL;
	}
	if (',' != keyboard_handler_tester(0x33)) {
		printf("0x33"); result = FAIL;
	}
	if ('.' != keyboard_handler_tester(0x34)) {
		printf("0x34"); result = FAIL;
	}
	if ('/' != keyboard_handler_tester(0x35)) {
		printf("0x35"); result = FAIL;
	}
	if ('*' != keyboard_handler_tester(0x37)) {
		printf("0x37"); result = FAIL;
	}
	if (0 != keyboard_handler_tester(0x38)) {
		printf("0x38"); result = FAIL;
	}
	if (' ' != keyboard_handler_tester(0x39)) {
		printf("0x39"); result = FAIL;
	}
	return result;
}

/**
 * Do some test for the fs read
 * Will first try to get the file to read
 * Then print out the file content up to some bytes
 * Since fd is not working for this cp, we just set them to any number
 */
int fs_read_test() {
    TEST_HEADER;
    char buf[200]; /* 200 is the continer size we want to read from terminal */

    /* Read the file name from user */
    int cnt = 0;
    printf("Type the file name you want to read:\n");
    cnt = terminal_read(1, (unsigned char *)buf, 200);
    if (cnt > 0 && '\n' == buf[cnt - 1])
        cnt--;
    buf[cnt] = '\0';

    if(file_open((const uint8_t*)buf) == -1) {
        printf("File Not Exist for file_open : %s\n", buf);
        return FAIL;
    }

    dentry_t dentry;
    if(read_dentry_by_name((const uint8_t*)buf, &dentry) == -1) {
        printf("File Not Exist for read_dentry_by_name\n");
        return FAIL;
    }

    int read_length = 0;
    int total_length = 0;
    uint8_t* data[200]; /* 200 is the continer size each time we want to read something */
    while((read_length = read_data(dentry.inode_idx, total_length, (uint8_t*)data, 200)) != 0) {
        if(read_length == -1) {
            return FAIL;
        }
        total_length += read_length;
        terminal_write(1, data, read_length);
    }
    printf("\ntotal_length: %d\n", total_length);

//    uint8_t* dentry_data[6000];
//    int32_t byte_read = read_data(dentry.inode_idx, 0, (uint8_t*)dentry_data, 6000);
//    terminal_write(1, dentry_data, 6000);
//    printf("\nbyte_read: %d\n", byte_read);


    printf("file_name: %s, file_size: %d\n",dentry.file_name, read_file_size(dentry.inode_idx));

    if(file_write(0, "test", 4) != -1) {
        return FAIL;
    }

    if(file_close(0) != 0) {
        return FAIL;
    }

    /* Then test for read by index */
    dentry_t dentry_testidx;
    printf("Testing for read_dentry_by_index...\n");
    read_dentry_by_index(14, &dentry_testidx);
    if(file_open((const uint8_t*)dentry_testidx.file_name) == -1) {
        printf("File Not Exist\n");
        return FAIL;
    }
    int read_length_testidx = 0;
    uint8_t* data_testidx[300]; /* 300 is the continer size each time we want to read something */
    while((read_length_testidx = file_read(0, (uint8_t*)data_testidx, 300)) != 0) {
        if(read_length_testidx == -1) {
            return FAIL;
        }
        terminal_write(1, data_testidx, read_length_testidx);
    }
    putc('\n');
    printf("file_name: %s, file_size: %d\n",dentry_testidx.file_name, read_file_size(dentry_testidx.inode_idx));

    return PASS;
}

/**
 * Test for file system listfiles functionality
 * will try to test for dir_open, dir_close, dir_read and dir_write
 * Since fd is not working for this cp, we just set them to any number
 * @return pass or fail
 */
int fs_listfiles_test() {
    TEST_HEADER;
    clear();
    int result = PASS;

    int32_t cnt;
    uint8_t buf[33];    /*  32 is the max file_name size, we need one more for terminator */

    /* Try to open a directory */
    if (dir_open ((uint8_t*)".") == -1) {
        return FAIL;
    }

    /* Try read the directory */
    /* 32 is the max file_name size */
    while (0 != (cnt = dir_read(0, buf, 32))) {
        if (-1 == cnt) {
            return FAIL;
        }
        buf[cnt] = '\0';    /* '\0' is the string termiantor */

        dentry_t dentry;
        read_dentry_by_name((uint8_t*)buf, &dentry);
        if(strncmp((const int8_t*)buf, dentry.file_name, strlen(dentry.file_name)) != 0) {
            result = FAIL;
            assertion_failure();
        }
        printf("file_name: %s, file_type: %d, file_size: %d\n", dentry.file_name, dentry.file_type, read_file_size(dentry.inode_idx));
    }

    /* Try to write a directory */
    if(dir_write(0, (const void*)"test", 4) != -1) {
        return FAIL;
    }

    /* Try to close a directory */
    if(dir_close(0) != 0) {
        return FAIL;
    }

    return result;
}


/* Checkpoint 2 tests */
/*
 *  Test for check wheather terminal_read_write works
 * Ouput: PASS if none of the tests fail
 * 		  FAIL is any of the tests fail
 * Side Effects: None
 * Coverage: termnal functionality check
 * Files: keyboard.h/S
 */
int terminal_read_write() {
    TEST_HEADER;

    int i, buf_size = 0;
    int result = PASS;
	int32_t check_result;
	uint8_t *fd;
	int32_t fdint;

	/* Test terminal_open function */
	check_result = terminal_open(fd);
    if(check_result != 0) {
            assertion_failure();
            result = FAIL;
    }
	if (terminal_buf_size != 0 && enter_pressed_flag != 0){
		assertion_failure();
        result = FAIL;
	}

	/* Test terminal_close function */
	check_result = terminal_close(fdint);
    if(check_result != 0) {
            assertion_failure();
            result = FAIL;
    }

	/* Test terminal_close and terminal_write function */
	char buf[200];  /* 200 is an enough number for the buffer container */
	printf("Type something:\n");
	check_result = terminal_read(fdint, (unsigned char *)buf, 200);

	/* Check for return value */
	if(check_result == 0) {
            assertion_failure();
            result = FAIL;
    }

	/* Check for buf greater that 128 bytes */
	check_result = 1;
	for (i = 0; i < 128; i++) { /* 128 is the max buffer size */
		if(buf[i] == '\n' || buf[i] == '\r') {
			check_result = 0;
			buf_size = i;
			break;
		}
	}
	if (check_result) {
		assertion_failure();
        result = FAIL;
	}

	/* Check for succesfull keyboard echo and write functionality */
	buf[++buf_size] = '\0';
	printf("%s", buf);
	check_result = terminal_write(fdint, buf, buf_size);
	printf("Depending if the last three strings printed are the same type y/n:");
	terminal_read(fdint, buf, 1);
	if(buf[0] == 'n') {
		assertion_failure();
        result = FAIL;
	}

	// /* Check return value of termianl_write */
	// if (check_result != buf_size) {
	// 	assertion_failure();
    //     result = FAIL;
	// }

	/* Check result for writing more bytes than available termianl_write */
	/* 50 is the enough size for buf container */
	char buf2[50] = "Tesing for terminal_write...\n\0";
	check_result = terminal_write(1, (unsigned char *)buf2, strlen(buf2));
	if(check_result != strlen(buf2)) {
            assertion_failure();
            result = FAIL;
    }

	/* NULL buf read and write test */
	unsigned char* null_buf = NULL;
	check_result = terminal_write(1, null_buf, strlen(buf2));
	if (check_result != -1) {
		assertion_failure();
		result = FAIL;
	}
	check_result = terminal_read(1, null_buf, strlen(buf2));
	if (check_result != -1) {
		assertion_failure();
		result = FAIL;
	}

    return result;
}

/* RTC Test:
 * Opens the RTC and tests the handler for 5 seconds @ 2 Hz
 * Writes all valid Hz values to the RTC then tests them
 *  	for 1 second each (Can't distinguish between
 * 		values > 30 Hz due to screen refresh)
 * Writes all valid Hz values > 1024 Hz which are rejected
 * 		due to kernel wanting to limit interrupts per
 * 		second
 * Writes an invalid Hz value which is rejected
 * Writes a valid Hz value with an invalid nbytes value
 * 		which is rejected
 * Writes with a valid nbytes but an invalid buf pointer
 * 		which is rejected
 */
int rtc_test(){
	int i, j, result = PASS;
	uint8_t *filename = NULL;
	int32_t fd, nbytes = 4;
	int hertz_array[1];
	TEST_HEADER;
	// init_rtc();
	// printf("RTC Open Test: ");
	if(rtc_open(filename) != 0){
		result = FAIL;
	}
	for (i = 0; i < 10; i++){
		if (rtc_read(fd, hertz_array, nbytes)){
				result = FAIL;
			}
	}
	char buf3[1] = "l";
	// printf("%s\nRTC Good Hertz Test: ", (result ? "Success" : "Fail"));
	for(j = 6; j < 15; j++){
		hertz_array[0] = hertzmap[j];
		if (rtc_write(fd, (void *)hertz_array, nbytes) != 0){
			result = FAIL;
		}
		for (i = 0; i < 100; i++){
			if (rtc_read(fd, hertz_array, nbytes)){
				result = FAIL;
			}
			terminal_write(1,(void*)buf3,1);
		}
		clear();
	}

	// printf("%s\nRTC High Hertz Test: \n", (result ? "Success" : "Fail"));
	for(j = 3; j < 6; j++){
		hertz_array[0] = hertzmap[j];
		// printf("%d\t%s\n", hertz_array[0], (result ? "Success" : "Fail"));
		if (rtc_write(fd, (void *)hertz_array, nbytes) != -1){
			result = FAIL;
		}
	}

	// printf("%s\nRTC Bad Hertz Test: ", (result ? "Success" : "Fail"));
	hertz_array[0] = 300;
	if (rtc_write(fd, (void *)hertz_array, nbytes) != -1) {
		result = FAIL;
	}

	// printf("%s\nRTC Bad Nbytes Test: ", (result ? "Success" : "Fail"));
	hertz_array[0] = 256;
	if (rtc_write(fd, (void *)hertz_array, 0) != -1) {
		result = FAIL;
	}

	// printf("%s\nRTC Bad Buffer Test: ", (result ? "Success" : "Fail"));
	if (rtc_write(fd, NULL, nbytes) != -1) {
		result = FAIL;
	}
	// printf("%s\n", (result ? "Success" : "Fail"));

	return result;
}


/* Checkpoint 3 tests */

/**
 * Checks for read, write, and close to fail for invalid/inactive FDs
 * Test for read/write syscall for negative fd
 * Test for read/write syscall for big fd
 * Test for read/write syscall for unopened fd
 * Test bad input for buf
 * Test for close file descripor which is not present
 * @return pass or fail
 */
int syscall_rw_c_test()
{
    TEST_HEADER;
    int result = PASS;

		/* 200 is the enough size for buf container */
    int32_t tmp_buf_size = 200;
    uint8_t buf[tmp_buf_size];
    int32_t neg = -1;
    int32_t big = MAX_FD_NUM;
    int32_t unopened = 6; // number of unused fd.

    /*
     * Test for read/write syscall for negative fd
     */
    if (read(neg, (unsigned char *)buf, tmp_buf_size) != -1 )
        result = FAIL;
    if (write(neg, (unsigned char *)buf, tmp_buf_size) != -1 )
        result = FAIL;

    /**
     * Test for read/write syscall for big fd
     */
    if (read(big, (unsigned char *)buf, tmp_buf_size) != -1 )
        result = FAIL;
    if (write(big, (unsigned char *)buf, tmp_buf_size) != -1 )
        result = FAIL;

    /**
     * Test for read/write syscall for unopened fd
     */
    if (read(unopened, (unsigned char *)buf, tmp_buf_size) != -1 )
        result = FAIL;
    if (write(unopened, (unsigned char *)buf, tmp_buf_size) != -1 )
        result = FAIL;

    /**
     * Test bad input for buf
     */
    if (read(unopened, (unsigned char *)NULL, tmp_buf_size) != -1 )
        result = FAIL;
    if (write(unopened, (unsigned char *)NULL, tmp_buf_size) != -1 )
        result = FAIL;

    /**
     * Test for close file descripor which is not present
     */
    if (close(neg) != -1 )
        result = FAIL;
    if (close(big) != -1 )
        result = FAIL;
    if (close(unopened) != -1 )
        result = FAIL;

    return result;
}

/**
 * Checks if system_open catches invalid filenames or bad inputs
 * Test for unexist file
 * Test for bad input
 * @return pass or fail
 */
int syscall_open_test(){
	TEST_HEADER;
	int result = PASS;

	/* Test for unexist file */
    if (open((uint8_t*)"helloooo") != -1) {
        result = FAIL;
        printf("helloooo fail");
    }
    if (open((uint8_t*)"shel") != -1) {
        result = FAIL;
        printf("shel fail");
    }
    if (open((uint8_t*)"") != -1) {
        result = FAIL;
        printf("empty fail");
    }
    /* Test for bad input */
    if (open((uint8_t*)NULL) != -1) {
        result = FAIL;
        printf("bad input fail");
    }

    return result;
}

/**
 * Check for valid system call execute and halt
 * And also calling userspace syserr for test
 * @return pass or fail
 */
int syscall_execute_halt_test(){
    TEST_HEADER;

    int result = PASS;

    printf("Begin testing for syscall execute and halt...\n");

    /* We will add a fake PCB */
    task_t* fake_task = get_current_task();
    fake_task->parent = -1;
    fake_task->present = 1;
    fake_task->pid = 0;
    fake_task->fd_size = 2;
    current_task_num++;

    printf("Begin testing for shell...\n");
    if (execute((uint8_t*)"shell") != 0)
        result = FAIL;
    printf("Begin testing for testprint...\n");
    if (execute((uint8_t*)"testprint") != 0)
        result = FAIL;
    printf("Begin intergrated testing for syserr...\n");
    if (execute((uint8_t*)"syserr") != 0)
        result = FAIL;

    /* Set back the task state */
    init_tasks();

    return result;
}

/**
 * Test for fd array under read/write/open/close syscalls
 * @return pass or fail
 */
int syscall_fd_test() {
    TEST_HEADER;

    int result = PASS;

    printf("Begin testing for syscall execute and halt...\n");

    /* We will add a fake PCB */
    task_t* fake_task = get_current_task();
    fake_task->parent = -1;
    fake_task->present = 1;
    fake_task->pid = 0;
    fake_task->fd_size = 2; /* Assume initial 2 is for stdin and stdout */
    fake_task->fd_table[0].present = 1;
    fake_task->fd_table[1].present = 1;
    current_task_num++;

    /* Test for fd under open syscall */
    int fd = 0;
    fd = open((uint8_t*)".");
    if (fd == -1) {
        result = FAIL;
        printf("fd open fail");
    }

    if(fake_task->fd_size != 3 /* 2+1 (origin 2 + open 1 */) {
        result = FAIL;
        printf("fd_size fail");
    }

    /* Test for fd under read/write syscall */
    int32_t tmp_buf_size = 200; /* Just set for a buffer size */
    uint8_t buf[tmp_buf_size];
    if(read(fd, buf, tmp_buf_size) == -1){
        result = FAIL;
        printf("fd read fail");
    }

    if(write(fd, buf, tmp_buf_size) != -1) {
        result = FAIL;
        printf("fd write fail");
    }

    /* Test for fd under close syscall */
    if(close(fd) == -1 || fake_task->fd_size != 2) {
        printf("fd close fail: fd_size: %d\n", fake_task->fd_size);
        result = FAIL;
    }

    /* Reset task states */
    init_tasks();
    return result;
}


/* Checkpoint 4 tests */

/**
 * Test for getargs syscall
 * @return pass or fail
 */
int syscall_getargs_test() {
	TEST_HEADER;

	uint8_t* null_buf = NULL;
	uint8_t valid_buf[4] = {'b','u','f','f'};
	uint8_t string[4] = {' ',' ',' ','\0'};
	int32_t nbytes = 1;
	int32_t result = PASS;
	task_t *current_task = get_current_task();
	uint32_t argument_num_save = current_task->argument_num;
	char full_argument_save[strlen(current_task->full_argument)];
	strcpy(full_argument_save, current_task->full_argument);

	if(getargs(null_buf, nbytes) != -1) {
		printf("getargs NULL buf fail");
		result = FAIL;
	}

	nbytes = -1;
	if(getargs(valid_buf, nbytes) != -1) {
		printf("getargs invalid nbytes fail");
		result = FAIL;
	}

	nbytes = 4;
	current_task->argument_num = 0;
	strcpy(current_task->full_argument, (int8_t*)string);
	if(getargs(valid_buf, nbytes) != -1) {
		printf("getargs invalid argument_num fail");
		result = FAIL;
	}

	nbytes = 3;
	current_task->argument_num = 1;
	if(getargs(valid_buf, nbytes) != -1) {
		printf("getargs invalid buf size fail");
		result = FAIL;
	}

	nbytes = 4;
	current_task->argument_num = 1;
	if(getargs(valid_buf, nbytes) != 0 && strncmp((int8_t*)valid_buf, (int8_t*)string, nbytes) != 0) {
		printf("getargs valid fail");
		result = FAIL;
	}

	current_task->argument_num = argument_num_save;
	strcpy(current_task->full_argument, full_argument_save);

	return result;
} 

/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests()
{
	/* Run Tests */
    TEST_OUTPUT("syscall_rw_c_test", syscall_rw_c_test());
    TEST_OUTPUT("syscall_open_test", syscall_open_test());
    TEST_OUTPUT("syscall_fd_test", syscall_fd_test());
    TEST_OUTPUT("syscall_execute_halt_test", syscall_execute_halt_test());
	TEST_OUTPUT("syscall_getargs_test", syscall_getargs_test());

	/* Ask user for history checkpoint tests */
    char buf[1];
    printf("Do you want to test for past checkpoints, type y/n:");
    terminal_read(0, buf, 1);
    if(buf[0] != 'y') {
        return;
    }

    TEST_OUTPUT("idt_test", idt_test());
    TEST_OUTPUT("paging_test", paging_test());
    TEST_OUTPUT("fs_listfiles_test", fs_listfiles_test());
    TEST_OUTPUT("fs_read_test", fs_read_test());
    TEST_OUTPUT("keyboard_translation_test", keyboard_translation_test());
    TEST_OUTPUT("terimal_test", terminal_read_write());
    TEST_OUTPUT("rtc_test", rtc_test());
}
