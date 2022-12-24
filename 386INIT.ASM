	page	,132
	title	386INIT.ASM

;
;   386 protected-mode control program
;
;   386INIT.ASM
;   386 protected-mode initialization module
;
;   by Jeff Parsons 05-Jun-1992
;   with a little help from Intel's 386 Programmer's Reference Manual :)
;
;
;   When this module is assembled and linked as a .COM file, the segments
;   must be arranged by the linker as follows:
;
;	_TEXT16
;	_TEXT
;	_DATA16
;	_DATA
;
;   This unusual mingling of 16-bit and 32-bit segments occurs because
;   both code segments are of class "CODE" and both data segments are of
;   class "DATA".  This makes it easy for the _TEXT16 segment to determine
;   where the _TEXT segment begins (ie, on the next paragraph boundary
;   after the end of the _TEXT16 segment).  Likewise for the data segments.
;
;   After we switch into protected-mode and have access to the complete
;   physical address space, we build the following data structures @ 1Mb+64Kb:
;
;	Resident code		(@SEL_TEXT:?)
;	Resident data		(@SEL_DATA:?)
;	IDT			(   30h*8 ==  180h bytes)
;	TSS			(	  == 2068h bytes)
;	GDT			(     5*8 ==   28h bytes)
;	PgDir x 1		(4*1024*1 == 1000h bytes)   @page boundary
;	PgTbl x n		(4*1024*n == 1000h*n bytes) @page boundaries
;	Stack			(	  == 2000h bytes)
;	Alias to first MB
;


	__acrtused  equ 1
	public	__acrtused


	include all.inc


ORG_VALUE   equ 100h		; for .COM file

PHYS_TEXT   equ 00110000h	; physical address for resident _TEXT (1Mb+64Kb)

STACK_SIZE  equ 8192


GRP16	group	_TEXT16,_DATA16

;
;   Insure that _TEXT16 is the first segment in the image file
;   (although MASM6 seems to do the right thing anyway)
;

_TEXT16 segment para public 'CODE'
_TEXT16 ends


_DATA16 segment para public 'DATA'


SEGLIMIT_DEFAULT equ	(SEGLIMIT_GRANULAR or SEGLIMIT_BIG or 0Fh)

;
;   Temporary GDT.  This is what we're running on when we first
;   enter protected-mode.  The "?" fields have to be filled in at
;   run-time, because this .COM file can be loaded anywhere in low memory
;

	public	adesGDT		; GDT array of descriptors
adesGDT label	byte

	public	desNull		; the first descriptor should be NULL
desNull DES	<0,0,0,0,0,0>	; (for errata mostly I think)

	public	desFlat		; the second descriptor is for all of memory
desFlat DES	<0FFFFh,0,0,SEG_DATA,SEGLIMIT_DEFAULT,0>

	public	SEL_FLAT
SEL_FLAT	equ	desFlat-desNull

	public	desText		; the third descriptor is for the _TEXT seg.
desText DES	<0FFFFh,?,?,SEG_CODE,SEGLIMIT_DEFAULT,0>

	public	SEL_TEXT
SEL_TEXT	equ	desText-desNull

	public	desData		; the fourth descriptor is for the _DATA seg.
desData DES	<0FFFFh,?,?,SEG_DATA,SEGLIMIT_DEFAULT,0>

	public	SEL_DATA
SEL_DATA	equ	desData-desNull

	public	desCode		; the fifth descriptor is for scratch code seg
desCode DES	<0FFFFh,?,?,SEG_CODE,SEGLIMIT_DEFAULT,0>

	public	SEL_CODE
SEL_CODE	equ	desCode-desNull

	public	desTSS		; the fifth descriptor is for the TSS
desTSS	DES	<size TSS-1,?,?,SEG_386TSS,0,0>

	public	SEL_TSS
SEL_TSS		equ	desTSS-desNull

	public	adesGDTend	; end of this GDT
adesGDTend label byte

	public	GDT_SIZE
GDT_SIZE	equ	adesGDTend-adesGDT

	public	dtrGDTTmp	; initial setting for GDTR register
dtrGDTTmp DTR	<GDT_SIZE-1, ?>


	public	bSwitches
bSwitches	db	0
SW_REBOOT	equ	0001h	; /R
SW_50LINES	equ	0002h	; /50


szHelp	 db	"386 Monitor",CR,LF
	 db	"By Jeff Parsons 05-Jun-1992",CR,LF
	 db	LF
	 db	"Command-line options",CR,LF
	 db	"    /?    This message",CR,LF
	 db	"    /R    Reset via reboot (incompatible w/386Max)",CR,LF
	 db	"    /50   Enable 50-line debugger display",CR,LF
	 db	LF
	 db	"After loading, use '?' to display debugger commands",CR,LF
	 db	'$'

szBad386 db	"Not a 386-compatible processor",CR,LF,'$'

	align	16
	public	__DataEnd
__DataEnd	label	byte

_DATA16 ends


;
;   To transfer control from Init16 to Init32, I really want to say:
;
;	jmp	far ptr SEL_TEXT:0
;
;   but since MASM won't let me do that, I've created these "template
;   segments" (hence the prefix T) that allow me to code far jumps that
;   don't need segment fixups.
;


TEXT16	segment at 0

TFar16	label	far

TEXT16	ends


DefCode macro	none

CODE	segment at SEL_CODE

	ifb	<none>
	org	__TextStart-Init16 + ORG_VALUE

TInit32 label	far
	endif

CODE	ends

endm


DefText macro	none

TEXT	segment at SEL_TEXT

	ifb	<none>
	org	__TextStart-Init16 + Main32-Init32 + ORG_VALUE

TMain32 label	far
	endif

TEXT	ends

endm


	DefCode None
	DefText None


_TEXT16 segment para public 'CODE'

	org	ORG_VALUE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Init16
;
;   16-bit initialization entry point
;
;   We want to take over the machine.  The whole machine.  But we
;   want to do it somewhat gracefully;	we don't want to leave disk
;   caches unflushed, for example.  And some drivers make life hard for
;   us, like EMM386, which may be running the processor in protected-
;   mode already (ie, v86-mode).
;
;   INT 19h (REBOOT_INT) to the rescue!  All the drivers we're
;   worried about should (in theory) have hooked INT 19h and be able to
;   flush, unhook, or unload whatever they need to flush, unhook or
;   unload.  The only problem is that we lose control too!
;
;   But, there's a undocumented location (106h) in the DOS BIOS data
;   segment (70h) that contains the original INT 13h disk vector address,
;   which is what DOS restores the disk vector to when *it* sees an
;   INT 19h.  So we edit that field so that DOS restores INT 13h to point
;   to us, and as soon as we see an INT 13h that's about to start the
;   boot process, we take over, so you never even see any disk boot
;   activity.
;
;   A reasonably elegant, surprisingly simple, and only mildly hacky
;   solution.
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
	sub	ax,ax
	mov	si,psp_cbCommand
	lodsb
	mov	cx,ax		    ; CX == length of command tail
do_char:
	lodsb
try_char:
	cmp	al,CR		    ; end of the line?
	je	short init	    ; yes
	cmp	al,'/'		    ; switch char?
	je	short do_switch	    ; yes
	cmp	al,'-'		    ; switch char?
	jne	short next_switch   ; no
do_switch:
	lodsb			    ;
	cmp	al,'?'		    ; /? help switch?
	jne	short not_?	    ; no
	mov	dx,offset GRP16:szHelp
	mov	ah,DOS_STD_CON_STRING_OUTPUT
	int	INT_DOS
	int	INT_DOSEXIT
not_?:
	cmp	al,'5'		    ; /50 line switch?
	jne	short not_50	    ; no
	lodsb			    ;
	cmp	al,'0'		    ;
	jne	short try_char	    ;
	or	[bSwitches],SW_50LINES
	jmp	short next_switch   ;
not_50:
	and	al,not LC_BIT
	cmp	al,'R'		    ; /R reboot switch?
	jne	short next_switch   ; no
	or	[bSwitches],SW_REBOOT
next_switch:
	loop	do_char
;
;   Check processor type
;
init:
	Check286
	je	short init_86	    ; it's pre-286 (808x or 8018x)

	Check386
	jnz	short init_386	    ; it's 386 or better

init_86:
	mov	dx,offset GRP16:szBad386
	mov	ah,DOS_STD_CON_STRING_OUTPUT
	int	INT_DOS
	int	INT_DOSEXIT

init_386:
	.386c
	mov	ah,DOS_DISK_RESET   ; try to insure that all disk
	int	INT_DOS		    ; buffers are flushed

	.errnz	SEG_ROMDATA	    ;
	sub	ax,ax		    ; AX == SEG_ROMDATA
	mov	ds,ax		    ;
	mov	es,ax		    ;
	assume	ds:_ROMDATA,es:_ROMDATA

	mov	al,RomData.rb_vga.vga_CRTRows
	test	[bSwitches],SW_50LINES
;;;	jz	short init_notforced
	jz	short init_not50    ; for now, no /50 means no video mucking
	mov	al,50-1		    ; force 50-line mode
init_notforced:
	push	ax		    ; save current # of rows
	mov	ax,VIDEO_RESET shl 8 or VMODE_CO80
	int	INT_VIDEO	    ; reset the video mode
	pop	ax		    ; recover original # of rows
	cmp	al,50-1		    ; 50-line mode?
	jne	short init_not50    ; no

	mov	ax,VIDEO_EGASELECTFONT shl 8 or VFONT_8X8
	mov	bl,0		    ; load 8x8 font in block #0
	int	INT_VIDEO	    ;
	mov	cx,0 shl 8 or 7	    ; CH=0, CL=7 (scan lines)
	mov	ah,VIDEO_SETCURSORTYPE
	int	INT_VIDEO	    ; block cursor enabled

init_not50:
	test	[bSwitches],SW_REBOOT
	jz	short init_reset    ;
;
;   This is the original, hacky way I tried forcing a transition
;   back to real-mode if currently in v86-mode, but it doesn't work
;   well with software like 386MAX
;
	mov	ax,SEG_DOSDATA
	mov	ds,ax
	assume	ds:_DOSDATA

	mov	eax,[DosData].dos_OldInt13
	mov	dword ptr [Int13Vector],eax
	mov	[DosData].dos_OldInt13.w_lo,offset GRP16:Int13Hook
	mov	[DosData].dos_OldInt13.w_hi,cs
	int	INT_REBOOT	    ; start warm boot

Int13Hook:
	assume	ds:nothing,es:nothing,ss:nothing

	cmp	ah,DISKIO_RESET	    ; disk reset?
	je	short InitContinue  ; yes, take control

Int13Vector equ $+1
	jmp	far ptr TEXT16:TFar16

;
;   This is the new, improved way:  reset the processor and get control
;   back from the ROM immediately using the appropriate CMOS shutdown code
;
init_reset:
	assume	ds:_ROMDATA,es:_ROMDATA,ss:GRP16

	cli
	SumVecs			    ; vector chksum ok?
	jne	short init_pics	    ; no
	CopyVecs		    ; copy vectors back into the IVT
	jmp	short init_vecset

init_pics:
	in	al,PIC_MASTER_IMR
	mov	ds:[rb_SaveMasterIMR],al
	in	al,PIC_SLAVE_IMR
	mov	ds:[rb_SaveSlaveIMR],al

init_vecset:
	mov	al,0FFh		    ; don't take any chances, shut off
	out	PIC_SLAVE_IMR,al    ; all interrupts
	out	PIC_MASTER_IMR,al   ;

	mov	[RomData].rb_IOROMInit.w_lo,offset GRP16:InitContinue
	mov	[RomData].rb_IOROMInit.w_hi,cs

	ResetCPU CMOSSHUTDOWN_FARJMP


	public	InitContinue
InitContinue label far
	assume	ds:nothing,es:nothing,ss:nothing

	cli
	cld

	.errnz	SEG_ROMDATA	    ; AX == SEG_ROMDATA
	sub	ax,ax		    ; zero all segments in preparation
	mov	ds,ax		    ; for transition to protected-mode
	mov	es,ax		    ; (not clear this is necessary though)
	mov	fs,ax
	mov	gs,ax
	mov	ss,ax		    ; all we need this stack for is
	mov	sp,400h		    ; for the few calls below (very temporary)
	assume	ds:_ROMDATA,es:_ROMDATA,ss:_ROMDATA

	test	[bSwitches],SW_REBOOT
	jz	short init_a20
	mov	eax,dword ptr [Int13Vector]
	mov	[RomData].rb_IVT[INT_DISKIO*4],eax
;
;   Ensure A20 enabled
;
init_a20:
	call	WaitKbdInput16
	mov	al,KBDCMD_WRITE_OPORT
	out	KBDPORT_STATUS,al
	call	WaitKbdInput16
	mov	al,KBDOPORT_A20ON
	out	KBDPORT_DATA,al
	call	WaitKbdInput16
;
;   Set fast typematic rate
;
	mov	al,KEYCMD_SETTYPEMATIC
	out	KBDPORT_DATA,al	    ; tell keyboard to set new typematic rate
	call	WaitKbdOutput16	    ; wait for acknowledgement
	in	al,KBDPORT_DATA	    ; get it
	mov	al,TYPEMATIC_FAST   ; select rate (bits 0-4), delay (bits 5-6)
	out	KBDPORT_DATA,al	    ;
	call	WaitKbdOutput16	    ; wait for acknowledgement
	in	al,KBDPORT_DATA	    ; get it

	ifdef	KBDSCAN3
;
;   Enable scan code set #3
;
	mov	al,KEYCMD_SETSCANSET
	out	KBDPORT_DATA,al	    ; tell keyboard to enable new scan set
	call	WaitKbdOutput16	    ; wait for acknowledgement
	in	al,KBDPORT_DATA	    ; get it
	mov	al,3		    ; select scan set #3
	out	KBDPORT_DATA,al	    ;
	call	WaitKbdOutput16	    ; wait for acknowledgement

	endif  ;KBDSCAN3

	SetPICs <IRQ_TMR,IRQ_KBD>   ; set PICS for protected-mode operation
				    ; w/TMR and KBD interrupts
;
;   Establish initial CS:IP for v86-mode, by setting _v86Init to v86Init
;
	mov	cs:[_v86Init].w_lo,offset v86Init
	mov	cs:[_v86Init].w_hi,cs
;
;   Create temporary GDT
;
	mov	ax,cs
	movzx	eax,ax
	mov	edx,eax		    ; EDX -> CS:0
	shl	eax,4
	mov	[desText].des_wBase0_15,ax
	mov	[desCode].des_wBase0_15,ax
	shr	eax,16
	mov	[desText].des_bBase16_23,al
	mov	[desCode].des_bBase16_23,al

	mov	eax,edx
	shl	eax,4
	add	eax,offset GRP16:_v86Init
	mov	[desData].des_wBase0_15,ax
	shr	eax,16
	mov	[desData].des_bBase16_23,al

	shl	edx,4
	add	edx,offset GRP16:adesGDT
	mov	[dtrGDTTmp].dtr_dwBase,edx

	.386p			    ; we're not quite there yet, but make masm happy
	lgdt	[dtrGDTTmp]

	mov	eax,cr0		    ;
	or	eax,CR0_PE	    ; switch to protected mode
	mov	cr0,eax		    ; DONE!
				    ; now jump to flush the prefetch and
				    ; load our new, cool 32-bit code selector
	jmp	far ptr CODE:TInit32


	public	v86Init
v86Init label	near
	int	INT_REBOOT

Init16	endp


	public	WaitKbdInput16
WaitKbdInput16	proc near
	sub	cx,cx
wait_loop:
	in	al,KBDPORT_STATUS
	test	al,KBDSTATUS_INPUT
	loopnz	wait_loop
	ret
WaitKbdInput16	endp


	public	WaitKbdOutput16
WaitKbdOutput16 proc near
	sub	cx,cx
wait_loop:
	in	al,KBDPORT_STATUS
	test	al,KBDSTATUS_OUTPUT
	loopz	wait_loop
	ret
WaitKbdOutput16 endp


	align	16
	public	__TextStart
__TextStart:


_TEXT16 ends


	DefCode			    ; define TInit32 for far transfer now


	.386p

OPEN_CODE

	extrn	DivError:near
	extrn	Debug:near
	extrn	NMI:near
	extrn	BreakPoint:near
	extrn	Overflow:near
	extrn	Bounds:near
	extrn	InvOpCode:near
	extrn	NoCP:near
	extrn	DblFault:near
	extrn	TSSFault:near
	extrn	SegFault:near
	extrn	StackFault:near
	extrn	GPFault:near
	extrn	PageFault:near
	extrn	CPFault:near
	extrn	UnknownFault:near

	extrn	TMRInterrupt:near
	extrn	KBDInterrupt:near
	extrn	IRQ2Interrupt:near
	extrn	IRQ3Interrupt:near
	extrn	IRQ4Interrupt:near
	extrn	IRQ5Interrupt:near
	extrn	IRQ6Interrupt:near
	extrn	IRQ7Interrupt:near
	extrn	IRQ8Interrupt:near
	extrn	IRQ9Interrupt:near
	extrn	IRQAInterrupt:near
	extrn	IRQBInterrupt:near
	extrn	IRQCInterrupt:near
	extrn	IRQDInterrupt:near
	extrn	IRQEInterrupt:near
	extrn	IRQFInterrupt:near

CLOSE_CODE

OPEN_DATA
CLOSE_DATA

CONST	segment dword use32 public 'CONST'
CONST	ends

_STATIC segment dword use32 public 'STATIC'

	public	__StaticDataEnd
__StaticDataEnd label  byte

_STATIC ends

_BSS	segment dword use32 public 'BSS'
_BSS	ends

c_common segment dword use32 public 'BSS'
c_common ends


DATA	GROUP	_DATA, CONST, _STATIC, _BSS, c_common, _EDATA


OPEN_DATA

; memmgr.c globals

	extrn	_pmbZero:dword

	public	_v86Init
_v86Init    dd	0		    ; must be 1st dword in _DATA

	public	aoffIDTInit
aoffIDTInit dd	DivError	    ; IDT_DIVERROR    0x00
	    dd	Debug		    ; IDT_DEBUG	      0x01
	    dd	NMI		    ; IDT_NMI	      0x02
	    dd	BreakPoint	    ; IDT_BREAKPOINT  0x03
	    dd	Overflow	    ; IDT_OVERFLOW    0x04
	    dd	Bounds		    ; IDT_BOUNDS      0x05
	    dd	InvOpCode	    ; IDT_INVOPCODE   0x06
	    dd	NoCP		    ; IDT_NOCP	      0x07
	    dd	DblFault	    ; IDT_DBLFAULT    0x08
	    dd	UnknownFault	    ; IDT_RESERVED09  0x09
	    dd	TSSFault	    ; IDT_TSSFAULT    0x0A
	    dd	SegFault	    ; IDT_SEGFAULT    0x0B
	    dd	StackFault	    ; IDT_STACKFAULT  0x0C
	    dd	GPFault		    ; IDT_GPFAULT     0x0D
	    dd	PageFault	    ; IDT_PAGEFAULT   0x0E
	    dd	UnknownFault	    ; IDT_RESERVED0F  0x0F
	    dd	CPFault		    ; IDT_CPFAULT     0x10
	    dd	UnknownFault	    ; IDT_RESERVED11  0x11
	    dd	UnknownFault	    ; IDT_RESERVED12  0x12
	    dd	UnknownFault	    ; IDT_RESERVED13  0x13
	    dd	UnknownFault	    ; IDT_RESERVED14  0x14
	    dd	UnknownFault	    ; IDT_RESERVED15  0x15
	    dd	UnknownFault	    ; IDT_RESERVED16  0x16
	    dd	UnknownFault	    ; IDT_RESERVED17  0x17
	    dd	UnknownFault	    ; IDT_RESERVED18  0x18
	    dd	UnknownFault	    ; IDT_RESERVED19  0x19
	    dd	UnknownFault	    ; IDT_RESERVED1A  0x1A
	    dd	UnknownFault	    ; IDT_RESERVED1B  0x1B
	    dd	UnknownFault	    ; IDT_RESERVED1C  0x1C
	    dd	UnknownFault	    ; IDT_RESERVED1D  0x1D
	    dd	UnknownFault	    ; IDT_RESERVED1E  0x1E
	    dd	UnknownFault	    ; IDT_RESERVED1F  0x1F
	    dd	TMRInterrupt	    ; IDT_TMR	      0x20
	    dd	KBDInterrupt	    ; IDT_KBD	      0x21
	    dd	IRQ2Interrupt	    ; IDT_SLAVE	      0x22
	    dd	IRQ3Interrupt	    ; IDT_COM2	      0x23
	    dd	IRQ4Interrupt	    ; IDT_COM1	      0x24
	    dd	IRQ5Interrupt	    ; IDT_LPT2	      0x25
	    dd	IRQ6Interrupt	    ; IDT_DISKETTE    0x26
	    dd	IRQ7Interrupt	    ; IDT_LPT1	      0x27
	    dd	IRQ8Interrupt	    ; IDT_RTC	      0x28
	    dd	IRQ9Interrupt	    ; IDT_IRQ2	      0x29
	    dd	IRQAInterrupt	    ; IDT_IRQ10	      0x2A
	    dd	IRQBInterrupt	    ; IDT_IRQ11	      0x2B
	    dd	IRQCInterrupt	    ; IDT_IRQ12	      0x2C
	    dd	IRQDInterrupt	    ; IDT_CP	      0x2D
	    dd	IRQEInterrupt	    ; IDT_DISK	      0x2E
	    dd	IRQFInterrupt	    ; IDT_IRQ15	      0x2F

	public	__ResDataStart
__ResDataStart	label byte

	public	_sel_Flat,_sel_Text,_sel_Data
_sel_Flat   dw	SEL_FLAT
_sel_Text   dw	SEL_TEXT
_sel_Data   dw	SEL_DATA

	public	_pPhysData,_cbPhysData
_pPhysData  dd	?
_cbPhysData dd	?

	ifdef	DEBUG
	public	_cbBSSData	    ; the linker doesn't fixup BSS data
_cbBSSData  dd	?		    ; correctly, so we assert the size is zero
	endif

	public	_dtrIDT,_pIDT
_dtrIDT	   DTR	<IDT_SIZE-1, ?>
_pIDT	    dd	?

	public	_dtrGDT,_pGDT
_dtrGDT	   DTR	<GDT_SIZE-1, ?>
_pGDT	    dd	?

	public	_pPhysTSS,_pTSS
_pPhysTSS   dd	?
_pTSS	    dd	?

	public	_pPhysPgDir,_pPgDir
_pPhysPgDir dd	?
_pPgDir	    dd	?

	public	_pPhysPgTbl,_pPgTbl
_pPhysPgTbl dd	?
_pPgTbl	    dd	?

	public	_pPhysFree,_pLinFree,_pFree
_pPhysFree  dd	?
_pLinFree   dd	?
_pFree	    dd	?

CLOSE_DATA


_EDATA	segment dword use32 public 'ENDDATA'

	public	__ResDataEnd
__ResDataEnd label  byte

_EDATA	ends


_ETEXT	segment dword use32 public 'CODE'

	public	__ResTextEnd
__ResTextEnd label  byte

_ETEXT	ends


OPEN_CODE


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Init32
;
;   32-bit initialization entry point
;
;   ENTRY
;	EDX -> physical address of temporary GDT
;
;   EXIT
;	Never
;
;   USES
;	All
;
	assume	cs:CODE_SEG,ds:nothing,es:nothing,ss:nothing

	public	Init32
Init32	proc	far

	mov	ax,SEL_DATA
	mov	ds,ax
	assume	ds:DATA_SEG

	mov	ax,SEL_FLAT
	mov	es,ax

	mov	edi,PHYS_TEXT	    ; ES:EDI -> resident code location
	mov	esi,offset __ResTextStart
	mov	ecx,offset __ResTextEnd
	sub	ecx,esi		    ;
	rep	movs byte ptr es:[edi],byte ptr cs:[esi]

	mov	[_pPhysData],edi    ; ES:EDI -> resident data location
	mov	ebx,offset DATA:__ResDataStart
	mov	ecx,offset DATA:__ResDataEnd
	sub	ecx,ebx		    ;
	mov	[_cbPhysData],ecx   ;
	add	edi,ecx		    ;
	add	ebx,ecx		    ;

	mov	[_dtrIDT].dtr_dwBase,edi
	mov	[_pIDT],ebx	    ;
	mov	ecx,IDT_ENTRIES	    ; 20h reserved + 10h physical interrupts
	lea	ebx,[ebx+ecx*8]	    ;
	mov	esi,offset aoffIDTInit
init_idt:
	lodsd
	mov	es:[edi].gate_wBase0_15,ax
	mov	es:[edi].gate_wSel,SEL_TEXT
	mov	es:[edi].gate_bReserved,0
	mov	es:[edi].gate_bType,SEGTYPE_386INTGATE or SEGTYPE_DPL_0 or SEGTYPE_PRESENT
	shr	eax,16
	mov	es:[edi].gate_wBase16_31,ax
	add	edi,8
	loop	init_idt

	mov	[_pPhysTSS],edi	    ;
	mov	[_pTSS],ebx	    ;
	sub	eax,eax		    ;
	mov	ecx,size TSS/4	    ;
	lea	ebx,[ebx+ecx*4]	    ;
	rep	stosd		    ; create empty TSS for now

	mov	[_dtrGDT].dtr_dwBase,edi
	mov	[_pGDT],ebx	    ;
	mov	esi,edx		    ; ESI -> GDT (from above)
	mov	ecx,GDT_SIZE/4	    ;
	lea	ebx,[ebx+ecx*4]	    ;
	rep	movs dword ptr es:[edi],dword ptr es:[esi]
;
;   The GDT may have room for growth, since the page directory
;   which immediately follows must be located on a page boundary
;
	mov	edx,edi		    ;
	add	edi,PAGE_SIZE-1	    ;
	and	edi,NOT (PAGE_SIZE-1)
	mov	cr3,edi		    ; ES:EDI -> resident page directory
	mov	[_pPhysPgDir],edi   ;
	mov	eax,edi		    ;
	sub	eax,edx		    ;
	add	ebx,eax		    ;

	mov	[_pPgDir],ebx	    ;
	lea	eax,[edi+PAGE_SIZE] ;
	or	eax,PG_USER or PG_WRITE or PG_PRESENT
	mov	ecx,PAGE_ENTRIES    ;
	lea	ebx,[ebx+ecx*4]	    ;
	rep	stosd		    ; for now, limit support to 4Mb

	mov	[_pPhysPgTbl],edi   ;
	mov	[_pPgTbl],ebx	    ;
	mov	eax,PG_USER or PG_WRITE or PG_PRESENT
	mov	ecx,PAGE_ENTRIES    ; ES:EDI -> first resident page table
	lea	ebx,[ebx+ecx*4]	    ;
init_pgtbl:
	stosd			    ;
	add	eax,PG_FRAMEINC	    ;
	loop	init_pgtbl	    ;
;
;   Now that all data structures are in place, allocate stack space
;   and record the first free physical location
;
	add	edi,STACK_SIZE	    ;
	add	ebx,STACK_SIZE	    ;
	mov	esp,ebx		    ; stack will be ready later, when we
	mov	[_pPhysFree],edi    ; update SS as well
	mov	[_pLinFree],edi	    ; same as PhysFree, since linear==physical
	mov	[_pFree],ebx	    ;
;
;   Before switching to the new GDT and then the new page tables,
;   we need to edit the bases and limits of the GDT selectors again
;
;   BUGBUG: Should set accurate limits of selectors
;
	mov	edi,[_dtrGDT].dtr_dwBase

	mov	eax,PHYS_TEXT	    ;
	sub	eax,offset __ResTextStart
	mov	es:[edi+SEL_TEXT].des_wBase0_15,ax
	shr	eax,16		    ;
	mov	es:[edi+SEL_TEXT].des_bBase16_23,al
	shr	eax,8		    ;
	mov	es:[edi+SEL_TEXT].des_bBase24_31,al

	mov	eax,[_pPhysData]    ;
	sub	eax,offset DATA:__ResDataStart
	mov	es:[edi+SEL_DATA].des_wBase0_15,ax
	shr	eax,16		    ;
	mov	es:[edi+SEL_DATA].des_bBase16_23,al
	shr	eax,8		    ;
	mov	es:[edi+SEL_DATA].des_bBase24_31,al

	mov	eax,[_pPhysTSS]	    ;
	mov	es:[edi+SEL_TSS].des_wBase0_15,ax
	shr	eax,16		    ;
	mov	es:[edi+SEL_TSS].des_bBase16_23,al
	shr	eax,8		    ;
	mov	es:[edi+SEL_TSS].des_bBase24_31,al
;
;   Now that the TSS selector is initialized, init the TSS segment
;
	mov	edi,[_pPhysTSS]	    ;
	mov	eax,cr3		    ;
	mov	es:[edi].tss_CR3,eax; save current CR3 in new TSS
	mov	es:[edi].tss_offIOPM,tss_abIOPM
;
;   Now copy the resident data up (we only reserved space for it earlier)
;
	mov	edi,[_pPhysData]    ; ES:EDI -> new resident data location
	mov	esi,offset DATA:__ResDataStart
	mov	ecx,[_cbPhysData]   ;
	mov	eax,offset DATA:__ResDataEnd
	sub	eax,offset DATA:__StaticDataEnd

	ifdef	DEBUG
	mov	[_cbBSSData],eax    ; save size of BSS data (for assertion)
	endif

	sub	ecx,eax		    ;
	rep	movsb		    ; resident static portion of _DATA moved
	mov	ecx,eax		    ;
	mov	al,0		    ;
	rep	stosb		    ; resident BSS/c_common portion moved, too

	mov	edx,[_v86Init]	    ; EDX == initial v86-mode CS:IP

	lidt	[_dtrIDT]	    ; new IDT has been activated
	lgdt	[_dtrGDT]	    ; new GDT has been activated
;
;   Note that since we're running on the scratch code selector CODE_SEL,
;   it doesn't matter (yet) that we've changed SEL_TEXT, so CS is still ok.
;   Similarly, SEL_FLAT is unchanged.  We must now be careful not to
;   reference any non-resident data items however, because DS will only
;   access resident data items correctly.
;
	mov	ax,SEL_DATA	    ;
	mov	ds,ax		    ; I'm not sure if I really need to
	mov	es,ax		    ; reload all these after reloading GDTR
	mov	ss,ax		    ;
	assume	ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	mov	eax,cr0		    ;
	or	eax,CR0_PG	    ; enable paging
	mov	cr0,eax		    ; DONE!
				    ; now jump to flush the prefetch and
				    ; load our new, cool 32-bit code selector
	jmp	far ptr TEXT:TMain32
Init32	endp


	public	__ResTextStart
__ResTextStart:

	extrn	_Main:near

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Main32
;
;   32-bit resident code entry point
;
;   ENTRY
;	EDX == initial v86-mode CS:IP (from _v86Init)
;
;   EXIT
;	Never
;
;   USES
;	All
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	Main32
Main32	proc	far

	mov	ebx,[_pTSS]
	mov	word ptr [ebx].tss_SS0,ss
	mov	[ebx].tss_ESP0,esp

	mov	ax,SEL_TSS
	ltr	ax		    ; set task register

	EnableNMI		    ; insure that NMI is enabled
;
;   Create the initial v86-mode ESF frame
;
	sub	eax,eax
	push	eax		    ; v86GS
	push	eax		    ; v86FS
	push	eax		    ; v86DS
	push	eax		    ; v86ES
	push	40h		    ; v86SS
	push	eax		    ; v86ESP
	push	FLAGS_V86 or FLAGS_IOPL3 or FLAGS_IF; Flags
	movzx	ecx,dx		    ; ECX == new IP
	shr	edx,16		    ; EDX == new CS
	push	edx		    ; CS
	push	ecx		    ; EIP
	push	eax		    ; iErrCode	(dummy)
	push	IDT_DEBUG	    ; iTrap	(dummy)
	push	eax		    ; EAX
	push	eax		    ; ECX
	push	eax		    ; EDX
	push	eax		    ; EBX
	push	eax		    ; ESP
	push	eax		    ; EBP
	push	eax		    ; ESI
	push	eax		    ; EDI
	push	ds		    ; saveDS
	push	es		    ; saveES
	sub	esp,esf_saveES	    ; reserve space for the rest
;
;   We are now running at physical address PHYS_TEXT, well above the 1Mb
;   boundary, and with paging enabled, but interrupts disabled;	 it is up to
;   _Main to enable ints once all *its* data structures are initialized
;
	call	_Main		    ; dispatch to C code

	mov	ebx,[_pmbZero]	    ; restore master and slave IMRs
	mov	al,[ebx].rb_SaveMasterIMR
	out	PIC_MASTER_IMR,al
	mov	al,[ebx].rb_SaveSlaveIMR
	out	PIC_SLAVE_IMR,al

	add	esp,esf_EDI	    ; throw away DS, ES, and the rest
	popa
	add	esp,isf_EIP	    ; throw away iTrap and iErrCode
	iretd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   _Reboot
;
;   ENTRY:
;	None
;
;   EXIT:
;	None
;
;   USES:
;	All
;

	public	_Reboot
_Reboot label	near

	extrn	_DrainKbd:near

	ifdef	KBDSCAN3

	extrn	_WaitKbdInput:near, _WaitKbdOutput:near

	call	_WaitKbdInput	    ; wait for ready state
	mov	al,KEYCMD_SETSCANSET
	out	KBDPORT_DATA,al	    ; tell keyboard to enable new scan set
	call	_WaitKbdOutput	    ; wait for acknowledgement
	in	al,KBDPORT_DATA	    ; get it
	mov	al,1		    ; select scan set #1 (default set)
	out	KBDPORT_DATA,al	    ;
	call	_WaitKbdOutput	    ; wait for acknowledgement

	endif  ;KBDSCAN3

	cli
	mov	ax,SEL_FLAT
	mov	ds,ax		    ; trashes DS but we don't care at this point
	mov	es,ax		    ; trashes ES but we don't care at this point
	assume	ds:_ROMDATA,es:_ROMDATA

	SumVecs386		    ; vector chksum ok?
	jne	reboot_vecbad	    ; no
	CopyVecs386		    ; copy vectors back into the IVT

	ResetPICs		    ; reset PICS for real-mode operation
	call	_DrainKbd
;
;   Do some rudimentary reinitialization of the ROM BIOS data area
;
	mov	[RomData].rb_vid.vid_CRTMode,VMODE_CO80

	mov	[RomData].rb_IOROMInit,00700000h
	mov	edi,700h	    ; where to dump our "pre-boot" code
	mov	esi,offset cs:reboot_code
	mov	ecx,reboot_code_end - reboot_code
	rep	movs byte ptr es:[edi],byte ptr cs:[esi]

	ResetCPU CMOSSHUTDOWN_FARJMP

reboot_code label byte
	db	OP_MOVAX, 70h, 00h  ; mov ax,0070h
	mov	ss,eax		    ; mov ss,ax
	db	OP_MOVSP, 00h, 00h  ; mov sp,0000h
	int	19h		    ; int 19h
reboot_code_end label byte

reboot_vecbad:
	mov	[RomData].rb_ResetFlag,RESETFLAG_WARM
	ResetCPU		    ; vector info is hosed, just warm boot

Main32	endp


CLOSE_CODE


	DefText			    ; define TMain32 for far transfer now


	end	Init16
