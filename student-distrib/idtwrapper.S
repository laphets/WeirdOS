.global INTERRUPT_21
.global INTERRUPT_28
.global SYSCALL_80


INTERRUPT_21:
	pushal
	call keyboard_handler
	popal
	iret

INTERRUPT_28:
	pushal
	call rtc_handler
	popal
	iret

SYSCALL_80:
	pushal

	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax

	call syscall_handler

	popal
	iret
