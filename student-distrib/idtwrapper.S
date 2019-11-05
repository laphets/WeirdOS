# boot.S - start point for the kernel after GRUB gives us control
# vim:ts=4 noexpandtab

#define ASM     1

#include "syscall.h"

.global INTERRUPT_21
.global INTERRUPT_28
.global SYSCALL_80

/*
 * interface between keyboard interrupt and keyboard handler
 * Side_Effet: calls the keyboard handler
 */
INTERRUPT_21:
	pushal
	call keyboard_handler
	popal
	iret

/*
 * interface between rtc interrupt and rtc handler
 * Side_Effet: calls the rtc handler
 */
INTERRUPT_28:
	pushal
	call rtc_handler
	popal
	iret

/*
 * interface between system call and system call handler.
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
	jb CALL_HANDLER	# if syscall_size > eax, then call the handler

	# Then we handle some error
	movl $-1, %eax
	jmp RET

CALL_HANDLER:
	call *syscall_list(, %eax, 4)


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
