.global INTERRUPT_21
.global INTERRUPT_28
.global SYSCALL_80


INTERRUPT_21:
	pushad
	call keyboard_handler
	popad
	iret

INTERRUPT_28:
	pushad
	call rtc_handler
	popad
	iret

SYSCALL_80:
	pushad

	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax

	call syscall_handler

	popad
	iret
