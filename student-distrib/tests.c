#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here
int deref(int* a) {
    return *a;
}
int paging_test() {
    deref((int*)-100);
    deref(NULL);


    int b = 391;
    int* a = &b;

    if(b != deref(a)) {
        return FAIL;
    }
    return PASS;
}

// Tests keyboard by pressing values and see if all values work. Enter will cause exit the loop and pass
int keyboard_test() {
	int i;
	uint8_t scancode = 0;
	init_keyboard();
	printf("Echos keyboard until enter is pressed:\n");
	while (kdbus[scancode] != '\n') {
    scancode = inb(KEYBOARD_PORT);
    printf("Keyboard comes: %c\n", kbdus[scancode]);
    send_eoi(KEYBOARD_IRQ);
	}
	return PASS;
}

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
    TEST_OUTPUT("paging_test", paging_test());
	// launch your tests here
	TEST_OUTPUT("Keyboard_Test", keyboard_test());
}
