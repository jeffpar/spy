./spytrap.obj ./spytrap.lst: ./spytrap.asm ../../../dev/ddk/inc/debugsys.inc \
	../../../dev/ddk/inc/vmm.INC ../../../dev/ddk/inc/vxdldr.INC \
	../../../dos/dos386/include/loader.INC \
	../../../dos/dos386/include/vmmsys.INC ./inc/386.INC ./inc/387.INC \
	./inc/all.inc ./inc/bios.INC ./inc/cmos.INC ./inc/debug.INC \
	./inc/disk.INC ./inc/dos.INC ./inc/ems.INC ./inc/globals.INC \
	./inc/kbd.INC ./inc/lib.INC ./inc/pic.INC ./inc/timer.INC \
	./inc/types.INC ./inc/video.INC ./inc/x86.INC

./386trap.obj ./386trap.lst: ./386trap.asm ../../../dev/ddk/inc/vmm.INC \
	../../../dev/ddk/inc/vxdldr.INC ../../../dos/dos386/include/loader.INC \
	../../../dos/dos386/include/vmmsys.INC ./inc/386.INC ./inc/387.INC \
	./inc/all.inc ./inc/bios.INC ./inc/cmos.INC ./inc/debug.INC \
	./inc/disk.INC ./inc/dos.INC ./inc/ems.INC ./inc/globals.INC \
	./inc/kbd.INC ./inc/lib.INC ./inc/pic.INC ./inc/timer.INC \
	./inc/types.INC ./inc/video.INC ./inc/x86.INC

./dosserv.obj ./dosserv.lst: ./dosserv.asm ../../../dev/ddk/inc/vmm.INC \
	../../../dev/ddk/inc/vxdldr.INC ../../../dos/dos386/include/loader.INC \
	../../../dos/dos386/include/vmmsys.INC ./inc/386.INC ./inc/387.INC \
	./inc/all.inc ./inc/bios.INC ./inc/cmos.INC ./inc/debug.INC \
	./inc/disk.INC ./inc/dos.INC ./inc/ems.INC ./inc/globals.INC \
	./inc/kbd.INC ./inc/lib.INC ./inc/pic.INC ./inc/timer.INC \
	./inc/types.INC ./inc/video.INC ./inc/x86.INC

./spykbd.obj ./spykbd.lst: ./spykbd.asm ../../../dev/ddk/inc/vkd.inc \
	../../../dev/ddk/inc/VKDpaste.INC ../../../dev/ddk/inc/vmm.INC \
	../../../dev/ddk/inc/vxdldr.INC ../../../dos/dos386/include/loader.INC \
	../../../dos/dos386/include/vmmsys.INC ./inc/386.INC ./inc/387.INC \
	./inc/all.inc ./inc/bios.INC ./inc/cmos.INC ./inc/debug.INC \
	./inc/disk.INC ./inc/dos.INC ./inc/ems.INC ./inc/globals.INC \
	./inc/kbd.INC ./inc/lib.INC ./inc/pic.INC ./inc/timer.INC \
	./inc/types.INC ./inc/video.INC ./inc/x86.INC

./386init.obj ./386init.lst: ./386init.asm ../../../dev/ddk/inc/vmm.INC \
	../../../dev/ddk/inc/vxdldr.INC ../../../dos/dos386/include/loader.INC \
	../../../dos/dos386/include/vmmsys.INC ./inc/386.INC ./inc/387.INC \
	./inc/all.inc ./inc/bios.INC ./inc/cmos.INC ./inc/debug.INC \
	./inc/disk.INC ./inc/dos.INC ./inc/ems.INC ./inc/globals.INC \
	./inc/kbd.INC ./inc/lib.INC ./inc/pic.INC ./inc/timer.INC \
	./inc/types.INC ./inc/video.INC ./inc/x86.INC

./spyinit.obj ./spyinit.lst: ./spyinit.asm ../../../dev/ddk/inc/vmm.INC \
	../../../dev/ddk/inc/vxdldr.inc ../../../dos/dos386/include/loader.INC \
	../../../dos/dos386/include/vmmsys.INC ./inc/386.INC ./inc/387.INC \
	./inc/all.inc ./inc/bios.INC ./inc/cmos.INC ./inc/debug.INC \
	./inc/disk.INC ./inc/dos.INC ./inc/ems.INC ./inc/globals.INC \
	./inc/kbd.INC ./inc/lib.INC ./inc/pic.INC ./inc/timer.INC \
	./inc/types.INC ./inc/video.INC ./inc/x86.INC

./reboot.obj ./reboot.lst: ./reboot.asm ../../../dev/ddk/inc/vmm.INC \
	../../../dev/ddk/inc/vxdldr.INC ../../../dos/dos386/include/loader.INC \
	../../../dos/dos386/include/vmmsys.INC ./inc/386.INC ./inc/387.INC \
	./inc/all.inc ./inc/bios.INC ./inc/cmos.INC ./inc/debug.INC \
	./inc/disk.INC ./inc/dos.INC ./inc/ems.INC ./inc/globals.INC \
	./inc/kbd.INC ./inc/lib.INC ./inc/pic.INC ./inc/timer.INC \
	./inc/types.INC ./inc/video.INC ./inc/x86.INC

./comio.obj ./comio.lst: ./comio.asm ../../../dev/ddk/inc/vmm.INC \
	../../../dev/ddk/inc/vxdldr.INC ../../../dos/dos386/include/loader.INC \
	../../../dos/dos386/include/vmmsys.INC ./inc/386.INC ./inc/387.INC \
	./inc/all.inc ./inc/bios.INC ./inc/cmos.INC ./inc/com.inc \
	./inc/debug.INC ./inc/disk.INC ./inc/dos.INC ./inc/ems.INC \
	./inc/globals.INC ./inc/kbd.INC ./inc/lib.INC ./inc/pic.INC \
	./inc/timer.INC ./inc/types.INC ./inc/video.INC ./inc/x86.INC

./v86io.obj ./v86io.lst: ./v86io.asm ../../../dev/ddk/inc/vmm.INC \
	../../../dev/ddk/inc/vxdldr.INC ../../../dos/dos386/include/loader.INC \
	../../../dos/dos386/include/vmmsys.INC ./inc/386.INC ./inc/387.INC \
	./inc/all.inc ./inc/bios.INC ./inc/cmos.INC ./inc/debug.INC \
	./inc/disk.INC ./inc/dos.INC ./inc/ems.INC ./inc/globals.INC \
	./inc/kbd.INC ./inc/lib.INC ./inc/pic.INC ./inc/timer.INC \
	./inc/types.INC ./inc/video.INC ./inc/x86.INC

./v86int.obj ./v86int.lst: ./v86int.asm ../../../dev/ddk/inc/vmm.INC \
	../../../dev/ddk/inc/vxdldr.INC ../../../dos/dos386/include/loader.INC \
	../../../dos/dos386/include/vmmsys.INC ./inc/386.INC ./inc/387.INC \
	./inc/all.inc ./inc/bios.INC ./inc/cmos.INC ./inc/debug.INC \
	./inc/disk.INC ./inc/dos.INC ./inc/ems.INC ./inc/globals.INC \
	./inc/kbd.INC ./inc/lib.INC ./inc/pic.INC ./inc/timer.INC \
	./inc/types.INC ./inc/video.INC ./inc/x86.INC

./vidsave.obj ./vidsave.lst: ./vidsave.asm ../../../dev/ddk/inc/vmm.INC \
	../../../dev/ddk/inc/vxdldr.INC ../../../dos/dos386/include/loader.INC \
	../../../dos/dos386/include/vmmsys.INC ./inc/386.INC ./inc/387.INC \
	./inc/all.inc ./inc/bios.INC ./inc/cmos.INC ./inc/debug.INC \
	./inc/disk.INC ./inc/dos.INC ./inc/ems.INC ./inc/globals.INC \
	./inc/kbd.INC ./inc/lib.INC ./inc/pic.INC ./inc/timer.INC \
	./inc/types.INC ./inc/video.INC ./inc/x86.INC

./symload.obj ./symload.lst: ./symload.c ../../../dev/ddk/inc/vmm.h \
	../../../dev/ddk/inc/vxdldr.h ../../../dos/dos386/include/loader.h \
	../../../dos/dos386/include/vmmsys.h ./inc/386.h ./inc/387.h \
	./inc/all.h ./inc/basedef.h ./inc/bios.h ./inc/cmos.h ./inc/debug.h \
	./inc/disk.h ./inc/dos.h ./inc/ems.h ./inc/globals.h ./inc/kbd.h \
	./inc/lib.h ./inc/pic.h ./inc/sym.h ./inc/timer.h ./inc/types.h \
	./inc/video.h ./inc/x86.h

./intparse.obj ./intparse.lst: ./intparse.c ../../../dev/ddk/inc/vmm.h \
	../../../dev/ddk/inc/vxdldr.h ../../../dos/dos386/include/loader.h \
	../../../dos/dos386/include/vmmsys.h ../../vxd/mrci32/mrci.h \
	./inc/386.h ./inc/387.h ./inc/all.h ./inc/basedef.h ./inc/bios.h \
	./inc/cmos.h ./inc/debug.h ./inc/disk.h ./inc/dos.h ./inc/ems.h \
	./inc/globals.h ./inc/kbd.h ./inc/lib.h ./inc/mult.h ./inc/parse.h \
	./inc/pic.h ./inc/sym.h ./inc/timer.h ./inc/types.h ./inc/video.h \
	./inc/x86.h

./memmgr.obj ./memmgr.lst: ./memmgr.c ../../../dev/ddk/inc/vmm.h \
	../../../dev/ddk/inc/vxdldr.h ../../../dos/dos386/include/loader.h \
	../../../dos/dos386/include/vmmsys.h ./inc/386.h ./inc/387.h \
	./inc/all.h ./inc/basedef.h ./inc/bios.h ./inc/cmos.h ./inc/debug.h \
	./inc/disk.h ./inc/dos.h ./inc/ems.h ./inc/globals.h ./inc/kbd.h \
	./inc/lib.h ./inc/pic.h ./inc/timer.h ./inc/types.h ./inc/video.h \
	./inc/x86.h

./main.obj ./main.lst: ./main.c ../../../dev/ddk/inc/vmm.h \
	../../../dev/ddk/inc/vxdldr.h ../../../dos/dos386/include/loader.h \
	../../../dos/dos386/include/vmmsys.h ./inc/386.h ./inc/387.h \
	./inc/all.h ./inc/basedef.h ./inc/bios.h ./inc/cmos.h ./inc/debug.h \
	./inc/disk.h ./inc/dos.h ./inc/ems.h ./inc/globals.h ./inc/kbd.h \
	./inc/lib.h ./inc/pic.h ./inc/sym.h ./inc/timer.h ./inc/types.h \
	./inc/video.h ./inc/x86.h

./dosdump.obj ./dosdump.lst: ./dosdump.c ../../../dev/ddk/inc/vmm.h \
	../../../dev/ddk/inc/vxdldr.h ../../../dos/dos386/include/loader.h \
	../../../dos/dos386/include/vmmsys.h ./inc/386.h ./inc/387.h \
	./inc/all.h ./inc/basedef.h ./inc/bios.h ./inc/cmos.h ./inc/debug.h \
	./inc/disk.h ./inc/dos.h ./inc/ems.h ./inc/globals.h ./inc/kbd.h \
	./inc/lib.h ./inc/pic.h ./inc/timer.h ./inc/types.h ./inc/video.h \
	./inc/x86.h

./x86debug.obj ./x86debug.lst: ./x86debug.c ../../../dev/ddk/inc/vmm.h \
	../../../dev/ddk/inc/vxdldr.h ../../../dos/dos386/include/loader.h \
	../../../dos/dos386/include/vmmsys.h ./inc/386.h ./inc/387.h \
	./inc/all.h ./inc/basedef.h ./inc/bios.h ./inc/cmos.h ./inc/com.h \
	./inc/debug.h ./inc/disk.h ./inc/dos.h ./inc/ems.h ./inc/globals.h \
	./inc/kbd.h ./inc/lib.h ./inc/parse.h ./inc/pic.h ./inc/sym.h \
	./inc/timer.h ./inc/types.h ./inc/video.h ./inc/win386.h ./inc/x86.h

./vxddump.obj ./vxddump.lst: ./vxddump.c ../../../dev/ddk/inc/vmm.h \
	../../../dev/ddk/inc/vxdldr.h ../../../dos/dos386/include/loader.h \
	../../../dos/dos386/include/vmmsys.h ./inc/386.h ./inc/387.h \
	./inc/all.h ./inc/basedef.h ./inc/bios.h ./inc/cmos.h ./inc/debug.h \
	./inc/disk.h ./inc/dos.h ./inc/ems.h ./inc/globals.h ./inc/kbd.h \
	./inc/lib.h ./inc/parse.h ./inc/pic.h ./inc/sym.h ./inc/timer.h \
	./inc/types.h ./inc/video.h ./inc/win386.h ./inc/x86.h

./vmmdump.obj ./vmmdump.lst: ./vmmdump.c ../../../dev/ddk/inc/vmm.h \
	../../../dev/ddk/inc/vxdldr.h ../../../dos/dos386/include/loader.h \
	../../../dos/dos386/include/vmmsys.h ./inc/386.h ./inc/387.h \
	./inc/all.h ./inc/basedef.h ./inc/bios.h ./inc/cmos.h ./inc/debug.h \
	./inc/disk.h ./inc/dos.h ./inc/ems.h ./inc/globals.h ./inc/kbd.h \
	./inc/lib.h ./inc/parse.h ./inc/pic.h ./inc/timer.h ./inc/types.h \
	./inc/video.h ./inc/win386.h ./inc/x86.h

./vidmgr.obj ./vidmgr.lst: ./vidmgr.c ../../../dev/ddk/inc/vmm.h \
	../../../dev/ddk/inc/vxdldr.h ../../../dos/dos386/include/loader.h \
	../../../dos/dos386/include/vmmsys.h ./inc/386.h ./inc/387.h \
	./inc/all.h ./inc/basedef.h ./inc/bios.h ./inc/cmos.h ./inc/debug.h \
	./inc/disk.h ./inc/dos.h ./inc/ems.h ./inc/globals.h ./inc/kbd.h \
	./inc/lib.h ./inc/pic.h ./inc/timer.h ./inc/types.h ./inc/video.h \
	./inc/x86.h

