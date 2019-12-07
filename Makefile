vm:
	qemu-system-i386 -hda ./student-distrib/mp3.img -m 256 -netdev user,id=ece391_net -device rtl8139,netdev=ece391_net -gdb tcp:127.0.0.1:1234 -name mp3

vmdebug:
	qemu-system-i386 -hda ./student-distrib/mp3.img -m 256 -netdev user,id=ece391_net,hostfwd=tcp::10086-:22 -device rtl8139,netdev=ece391_net -object filter-dump,id=f1,netdev=ece391_net,file=dump.dat -gdb tcp:127.0.0.1:1234 -name mp3

vmtap:
	sudo qemu-system-i386 -hda ./student-distrib/mp3.img -m 256 -netdev tap,id=ece391_net_tap,ifname=tap0,script=no,downscript=no -device rtl8139,netdev=ece391_net_tap -gdb tcp:127.0.0.1:1234 -name mp3
