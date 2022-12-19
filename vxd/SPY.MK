#   Makefile for SPY.386
#
#   History
#   -------
#   Created: 31-Jan-1993 by Jeff Parsons
#

.SUFFIXES:
.SUFFIXES:  .c .asm .h .inc .obj .lst .sys .exe .dll .com .map .sym .def


#SUBDIR=1

!ifndef ROOT
ROOT    = ..\..\..
!endif

!ifndef SUBDIR
SRCDIR  = .
!else
SRCDIR  = ..
VER     = debug
VERSIONLIST = debug retail
!endif

MASM6   = TRUE
IS_32   = TRUE
IS_OEM  = TRUE

INC     = $(SRCDIR)\inc
IFLAGS  = -I$(INC) -I..\..\vxd\mrci32 -I$(ROOT)\dos\dos386\include
INCFLAGS= $(IFLAGS)
DEPENDNAME = $(SRCDIR)\depend.mk

HFLAGS  = -t -s $(INC)\types.h
AFLAGS  = -DWIN386 -DDEBUG $(IFLAGS)
CFLAGS  = -DWIN386 -DDEBUG $(IFLAGS)

DEVICE  = SPY

TARGETS = spy.386 spy.sym

OBJS    = spyinit.obj \
          spytrap.obj \
          spykbd.obj \
          main.obj \
          memmgr.obj \
          vidmgr.obj \
          vidsave.obj \
          comio.obj \
          x86debug.obj \
          dosdump.obj \
          dosserv.obj \
          intparse.obj \
          vmmdump.obj \
          vxddump.obj \
          symload.obj \
          lib\gets.obj \
          lib\printf.obj \
          lib\sprintf.obj \
          lib\string.obj

!include $(ROOT)\dev\master.mk
