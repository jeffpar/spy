	page	,132
	title	REBOOT.ASM

;
;   REBOOT.ASM
;   Fast reboot utility
;
;   by Jeff Parsons 17-Jun-1992
;


	__acrtused  equ 1
	public	__acrtused


	include all.inc


ORG_VALUE   equ 100h		; for .COM file


GRP16	group	_TEXT16,_DATA16

;
;   Insure that _TEXT16 is the first segment in the image file
;   (although MASM6 seems to do the right thing anyway)
;

_TEXT16 segment para public 'CODE'
_TEXT16 ends


_DATA16 segment para public 'DATA'

stack	label	word

_DATA16 ends


ROMSEG	segment at 0FFFFh

	org	0000h
Reset	label	far

ROMSEG	ends


_TEXT16 segment para public 'CODE'

	org	ORG_VALUE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Init16
;
;   ENTRY
;	None
;
;   EXIT
;	Never
;
;   USES
;	All
;
	assume	cs:GRP16,ds:GRP16,es:GRP16,ss:GRP16

	public	Init16
Init16	proc	far

	mov	ah,DOS_DISK_RESET   ; try to insure that all disk
	int	INT_DOS		    ; buffers are flushed

	mov	ax,VIDEO_RESET shl 8 or VMODE_CO80
	int	INT_VIDEO	    ; reset the video mode
;
;   Check processor type; if it's not 286 or higher, we can't
;   (and needn't) reboot via an 8042 keyboard controller command
;
	Check286
	je	InitContinue	    ; it's pre-286 (808x or 8018x)
;
;   This is the new, improved way:  reset the processor and get control
;   back from the ROM immediately using the appropriate CMOS shutdown code
;
	cli

	mov	ax,SEG_ROMDATA
	mov	ds,ax
	assume	ds:_ROMDATA

	mov	[RomData].rb_IOROMInit.w_lo,offset GRP16:InitContinue
	mov	[RomData].rb_IOROMInit.w_hi,es

	ResetCPU CMOSSHUTDOWN_FARJMP


	public	InitContinue
InitContinue label far
	assume	ds:nothing,es:nothing,ss:nothing

	cli
	cld
	mov	ax,cs
	mov	ss,ax
	mov	sp,offset GRP16:stack+512

	mov	ax,SEG_ROMDATA
	mov	ds,ax
	mov	es,ax
	assume	ds:_ROMDATA,es:_ROMDATA

	SumVecs			    ; chksum ok?
	je	init_veccopy	    ; yes
	mov	[RomData].rb_ResetFlag,RESETFLAG_WARM
	jmp	far ptr ROMSEG:Reset; no, so just do a warm boot
init_veccopy:
	CopyVecs		    ; copy vectors back into the IVT

	ResetPICs		    ; reset PICS for real-mode operation

	int	INT_REBOOT	    ; now do an INT 19h soft boot

Init16	endp


_TEXT16 ends


	end	Init16
