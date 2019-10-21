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
    // deref((int*)-100);
    // deref(NULL);


    int b = 391;
    int* a = &b;

    if(b != deref(a)) {
        return FAIL;
    }
    return PASS;
}

/* Stores the values of the GDTR in the struct pointed to by the 
   passed pointer */
static inline void sgdt(void* ret) {
    do {                                    \
        asm volatile ("sgdt %0"           \
                : "=m" (ret)                \
				:							\
                : "memory"                  \
        );                                  \
    } while (0);
}

int gdt_test() {
	x86_desc_t response;
	sgdt((void*) &(response.size));
	if((gdt_desc.size != response.size) || 
	   (gdt_desc.addr != response.addr)) {
		return FAIL;
	}
	return PASS;
}

int keyboard_handler_tester(uint8_t scancode) {
	return kbdus[scancode];
}

int keyboard_translation_test() {
	if (0 != keyboard_handler_tester(0x0)) {
		printf("0");
		return FAIL;
	}
	if (27 != keyboard_handler_tester(0x1)) {
		printf("1");
		return FAIL;
	}
	if ('1' != keyboard_handler_tester(0x2)) {
		printf("2");
		return FAIL;
	}
	if ('2' != keyboard_handler_tester(0x3)) {
printf("3");
		return FAIL;
	}
	if ('3' != keyboard_handler_tester(0x4)) {
printf("4");
		return FAIL;
	}
	if ('4' != keyboard_handler_tester(0x5)) {
printf("5");
		return FAIL;
	}
	if ('5' != keyboard_handler_tester(0x6)) {
printf("6");
		return FAIL;
	}
	if ('6' != keyboard_handler_tester(0x7)) {
		printf("7");
		return FAIL;
	}
	if ('7' != keyboard_handler_tester(0x8)) {
printf("8");
		return FAIL;
	}
	if ('8' != keyboard_handler_tester(0x9)) {
printf("0");
		return FAIL;
	}
	if ('9' != keyboard_handler_tester(0xA)) {
printf("0");
		return FAIL;
	}
	if ('0' != keyboard_handler_tester(0xB)) {
printf("0");
		return FAIL;
	}
	if ('-' != keyboard_handler_tester(0xC)) {
printf("0");
		return FAIL;
	}
	if ('=' != keyboard_handler_tester(0xD)) {
printf("0");
		return FAIL;
	}
	if ('\b' != keyboard_handler_tester(0xE)) {
		printf("0");
		return FAIL;
	}
	if ('\t' != keyboard_handler_tester(0xF)) {
		printf("0");
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
	if ('c' != keyboard_handler_tester(0x2D)) {
		return FAIL;
	}
	if ('v' != keyboard_handler_tester(0x2E)) {
		return FAIL;
	}
	if ('b' != keyboard_handler_tester(0x2F)) {
		return FAIL;
	}
	if ('n' != keyboard_handler_tester(0x30)) {
		return FAIL;
	}
	if ('m' != keyboard_handler_tester(0x31)) {
		return FAIL;
	}
	if (',' != keyboard_handler_tester(0x32)) {
		return FAIL;
	}
	if ('.' != keyboard_handler_tester(0x33)) {
		return FAIL;
	}
	if ('/' != keyboard_handler_tester(0x34)) {
		return FAIL;
	}
	if (0 != keyboard_handler_tester(0x35)) {
		return FAIL;
	}
	if ('*' != keyboard_handler_tester(0x36)) {
		return FAIL;
	}
	if (0 != keyboard_handler_tester(0x37)) {
		return FAIL;
	}
	if (' ' != keyboard_handler_tester(0x38)) {
		return FAIL;
	}
	if ('.' != keyboard_handler_tester(0x39)) {
		return FAIL;
	}
	if ('.' != keyboard_handler_tester(0x3A)) {
		return FAIL;
	}
	if ('.' != keyboard_handler_tester(0x3B)) {
		return FAIL;
	}
	if ('.' != keyboard_handler_tester(0x3C)) {
		return FAIL;
	}
	if ('.' != keyboard_handler_tester(0x3D)) {
		return FAIL;
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
	TEST_OUTPUT("gdt_test", gdt_test());
	TEST_OUTPUT("keyboard_translation_test", keyboard_translation_test());

	// launch your tests here
}
