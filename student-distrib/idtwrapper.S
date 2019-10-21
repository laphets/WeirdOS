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
 */
SYSCALL_80:
	pushal

	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax

	call syscall_handler

	popal
	iret
