#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
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

/* IDT Test - Example
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
	for (i = 0; i < 10; ++i)
	{
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL))
		{
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here
int deref(int *a)
{
	return *a;
}
int paging_test()
{
	TEST_HEADER;
	int result = PASS;
	unsigned long reg;

	// Checks for video memory mapping
	if (!((&first_page_table[0xB8])->present))
	{
		// printf("%x in video memory is not mapped\n", i);
		assertion_failure();
		result = FAIL;
	}
	deref((int*) 0xB8000); // Accesses the first element in video memory. Should not blue screen
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

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests()
{
	TEST_OUTPUT("idt_test", idt_test());
	TEST_OUTPUT("paging_test", paging_test());
	// launch your tests here
}
