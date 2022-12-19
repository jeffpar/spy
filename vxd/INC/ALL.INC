.xlist
ifdef   WIN386
.386p
endif

ifdef  WIN386
ifndef INC_VMMSYS
INCLUDE vmm.INC
else
INCLUDE vmmsys.INC
INCLUDE vxdldr.INC
endif
endif
INCLUDE types.INC
INCLUDE 386.INC
INCLUDE 387.INC
INCLUDE bios.INC
INCLUDE dos.INC
INCLUDE ems.INC
INCLUDE debug.INC
INCLUDE globals.INC
INCLUDE x86.INC
INCLUDE lib.INC
.list

