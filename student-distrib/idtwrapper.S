.global INTERRUPT_21
.global INTERRUPT_28
.global SYSCALL_80


INTERRUPT_21:
	pusha
	call keyboard_handler
	popa
	iret

INTERRUPT_28:
	pusha
	call rtc_handler
	popa
	iret

SYSCALL_80:
	pusha

	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax

	call syscall_handler

	popa
	iret
