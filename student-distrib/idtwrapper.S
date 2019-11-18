# idtwrapper.S - asm linkage for idt
# vim:ts=4 noexpandtab

#define ASM     1

#include "syscall.h"

.global INTERRUPT_20
.global INTERRUPT_21
.global INTERRUPT_28
.global SYSCALL_80

/*
 * Interface between keyboard interrupt and keyboard handler
 * Side_Effet: calls the keyboard handler
 */
INTERRUPT_20:
	pushal
	call pit_handler
	popal
	iret

/*
 * Interface between keyboard interrupt and keyboard handler
 * Side_Effet: calls the keyboard handler
 */
INTERRUPT_21:
	pushal
	call keyboard_handler
	popal
	iret

/*
 * Interface between rtc interrupt and rtc handler
 * Side_Effet: calls the rtc handler
 */
INTERRUPT_28:
	pushal
	call rtc_handler
	popal
	iret

/*
 * Interface between system call and system call handler.
 * places the parameters in the appropriate registers for
 * the system call handler.
 * Side_Effet: calls the system call handler
 * Push EAX, ECX, EDX, EBX, original ESP, EBP, ESI, and EDI.
 */
SYSCALL_80:
	/* pushal */
	pushl %esp
	pushl %ebp
	pushl %esi
	pushl %edi
	pushfl
	pushl %edx
    pushl %ecx
    pushl %ebx

	cmpl $SYSCALL_SIZE, %eax
	jb CALL_HANDLER	# if syscall_size > eax (eax in range), then call the corresponding handler

	# Then we handle some error by setting the return value -1
ERROR_HANDLER:
	movl $-1, %eax
	jmp RET

CALL_HANDLER:
	call *syscall_list(, %eax, 4) # call hanlder by jump table, which is defined in syscall.h
RET:
	popl %ebx
	popl %ecx
	popl %edx
	popfl
	popl %edi
	popl %esi
	popl %ebp
	popl %esp

	iret
