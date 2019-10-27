#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "keyboard.h"
#include "paging.h"

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

/* Keyboard Test
 *
 * Asserts scancode translation is correct
 * Inputs: None
 * Outputs: PASS/FAIL

 */
int keyboard_test()
{
    TEST_HEADER;

    int i;
    int result = PASS;

    for(i = 2; i <= 10; i++) {
        if(scancode2char(i) != ('0'+i - 1)) {
            assertion_failure();
            result = FAIL;
        }
    }

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
	if (0 != keyboard_handler_tester(0x0)) {
		return FAIL;
	}
	if (27 != keyboard_handler_tester(0x1)) {
		return FAIL;
	}
	if ('1' != keyboard_handler_tester(0x2)) {
		return FAIL;
	}
	if ('2' != keyboard_handler_tester(0x3)) {
		return FAIL;
	}
	if ('3' != keyboard_handler_tester(0x4)) {
		return FAIL;
	}
	if ('4' != keyboard_handler_tester(0x5)) {
		return FAIL;
	}
	if ('5' != keyboard_handler_tester(0x6)) {
		return FAIL;
	}
	if ('6' != keyboard_handler_tester(0x7)) {
		return FAIL;
	}
	if ('7' != keyboard_handler_tester(0x8)) {
		return FAIL;
	}
	if ('8' != keyboard_handler_tester(0x9)) {
		return FAIL;
	}
	if ('9' != keyboard_handler_tester(0xA)) {
		return FAIL;
	}
	if ('0' != keyboard_handler_tester(0xB)) {
		return FAIL;
	}
	if ('-' != keyboard_handler_tester(0xC)) {
		return FAIL;
	}
	if ('=' != keyboard_handler_tester(0xD)) {
		return FAIL;
	}
	if ('\b' != keyboard_handler_tester(0xE)) {
		return FAIL;
	}
	if ('\t' != keyboard_handler_tester(0xF)) {
		return FAIL;
	}
	if ('q' != keyboard_handler_tester(0x10)) {
		return FAIL;
	}
	if ('w' != keyboard_handler_tester(0x11)) {
		return FAIL;
	}
	if ('e' != keyboard_handler_tester(0x12)) {
		return FAIL;
	}
	if ('r' != keyboard_handler_tester(0x13)) {
		return FAIL;
	}
	if ('t' != keyboard_handler_tester(0x14)) {
		return FAIL;
	}
	if ('y' != keyboard_handler_tester(0x15)) {
		return FAIL;
	}
	if ('u' != keyboard_handler_tester(0x16)) {
		return FAIL;
	}
	if ('i' != keyboard_handler_tester(0x17)) {
		return FAIL;
	}
	if ('o' != keyboard_handler_tester(0x18)) {
		return FAIL;
	}
	if ('p' != keyboard_handler_tester(0x19)) {
		return FAIL;
	}
	if ('[' != keyboard_handler_tester(0x1A)) {
		return FAIL;
	}
	if (']' != keyboard_handler_tester(0x1B)) {
		return FAIL;
	}
	if ('\n' != keyboard_handler_tester(0x1C)) {
		return FAIL;
	}
	if (0 != keyboard_handler_tester(0x1D)) {
		return FAIL;
	}
	if ('a' != keyboard_handler_tester(0x1E)) {
		return FAIL;
	}
	if ('s' != keyboard_handler_tester(0x1F)) {
		return FAIL;
	}
	if ('d' != keyboard_handler_tester(0x20)) {
		return FAIL;
	}
	if ('f' != keyboard_handler_tester(0x21)) {
		return FAIL;
	}
	if ('g' != keyboard_handler_tester(0x22)) {
		return FAIL;
	}
	if ('h' != keyboard_handler_tester(0x23)) {
		return FAIL;
	}
	if ('j' != keyboard_handler_tester(0x24)) {
		return FAIL;
	}
	if ('k' != keyboard_handler_tester(0x25)) {
		return FAIL;
	}
	if ('l' != keyboard_handler_tester(0x26)) {
		return FAIL;
	}
	if (';' != keyboard_handler_tester(0x27)) {
		return FAIL;
	}
	if ('\'' != keyboard_handler_tester(0x28)) {
		return FAIL;
	}
	if ('`' != keyboard_handler_tester(0x29)) {
		return FAIL;
	}
	if (0 != keyboard_handler_tester(0x2A)) {
		return FAIL;
	}
	if ('\\' != keyboard_handler_tester(0x2B)) {
		return FAIL;
	}
	if ('z' != keyboard_handler_tester(0x2C)) {
		return FAIL;
	}
	if ('x' != keyboard_handler_tester(0x2D)) {
		return FAIL;
	}
	if ('c' != keyboard_handler_tester(0x2E)) {
		return FAIL;
	}
	if ('v' != keyboard_handler_tester(0x2F)) {
		return FAIL;
	}
	if ('b' != keyboard_handler_tester(0x30)) {
		return FAIL;
	}
	if ('n' != keyboard_handler_tester(0x31)) {
		return FAIL;
	}
	if ('m' != keyboard_handler_tester(0x32)) {
		return FAIL;
	}
	if (',' != keyboard_handler_tester(0x33)) {
		return FAIL;
	}
	if ('.' != keyboard_handler_tester(0x34)) {
		return FAIL;
	}
	if ('/' != keyboard_handler_tester(0x35)) {
		return FAIL;
	}
	if (0 != keyboard_handler_tester(0x36)) {
		return FAIL;
	}
	if ('*' != keyboard_handler_tester(0x37)) {
		return FAIL;
	}
	if (0 != keyboard_handler_tester(0x38)) {
		return FAIL;
	}
	if (' ' != keyboard_handler_tester(0x39)) {
		return FAIL;
	}
	return PASS;
}

/* Checkpoint 2 tests */
int terimal_read_write()
{
    TEST_HEADER;

    int i;
    int result = PASS;
	uint16_t check_result;
	uint8_t fd;

	check_result = open(fd);
    if(check_result != 0) {
            assertion_failure();
            result = FAIL;
    }
	if (terminal_buf_size != 0 && enter_pressed_flag != 0){
		assertion_failure();
        result = FAIL;
	}

	char buf[128];
	printf("Type something:\n");
	check_result = read(fd, (unsigned char *)buf, 128);
	if(check_result == 0) {
            assertion_failure();
            result = FAIL;
    }

	printf("Type something:\n");

	if ( read(fd, (unsigned char *)buf, 128) ) ){

		char buf_print = (unsigned char) strlen (buf);
		for (i = 0; i < strlen(buf_print); i++)
		 printf("%c \n", buf_print[i]);
	}

	printf("Print something with terimal write");
	check_result = write(fd, (unsigned char *)buf, check_result);
	if(check_result == 0) {
            assertion_failure();
            result = FAIL;
    }

    return result;
}



/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests()
{
	TEST_OUTPUT("idt_test", idt_test());
    TEST_OUTPUT("paging_test", paging_test());
	TEST_OUTPUT("keyboard_translation_test", keyboard_translation_test());
}
