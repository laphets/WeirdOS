vm:
	qemu-system-i386 -hda ./student-distrib/mp3.img -m 256 -gdb tcp:127.0.0.1:1234 -name mp3

vmdebug:
	qemu-system-i386 -hda ./student-distrib/mp3.img -m 256 -gdb tcp:127.0.0.1:1234 -S -name mp3
