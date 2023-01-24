	page	,132
	title	386TRAP.ASM

;
;   386 protected-mode control program
;
;   386TRAP.ASM
;   386 protected-mode trap and interrupt handlers
;
;   by Jeff Parsons 05-Jun-1992
;   with a little help from Intel's 386 Programmer's Reference Manual :)
;


	include all.inc


	extrn	EmulateINT:near		; in v86int.asm
	extrn	EmulateINT3:near	; in v86int.asm
	extrn	SimulateINT:near	; in v86int.asm
	extrn	EmulateInALnn:near	; in v86io.asm
	extrn	EmulateInAXnn:near	; in v86io.asm
	extrn	EmulateOutnnAL:near	; in v86io.asm
	extrn	EmulateOutnnAX:near	; in v86io.asm
	extrn	EmulateInALDX:near	; in v86io.asm
	extrn	EmulateInAXDX:near	; in v86io.asm
	extrn	EmulateOutDXAL:near	; in v86io.asm
	extrn	EmulateOutDXAX:near	; in v86io.asm
	extrn	GetVRAMAddress:near	; in vidsave.asm


OPEN_DATA

	public	_flKeyEvent
_flKeyEvent dd	0

	public	_abKeyBuff
_abKeyBuff  db	KEYBUFF_MAX dup(?)	; don't insert goop between
					; abKeyBuff and pbKeyHead; the latter
					; marks the end of the former
	public	_pbKeyHead, _pbKeyTail
_pbKeyHead  dd	offset _abKeyBuff	; advanced at task-time
_pbKeyTail  dd	offset _abKeyBuff	; advanced at interrupt-time


	public	_flKeyShift
_flKeyShift dd	0			; very similar to the BIOS flags


	public	_abXlatNorm
_abXlatNorm label   byte
	db	00h, ESCAPE,'1',  '2',	'3',  '4',  '5',  '6'	; 00-07h
	db	'7',  '8',  '9',  '0',	'-',  '=',   BS,  TAB	; 08-0Fh
	db	'q',  'w',  'e',  'r',	't',  'y',  'u',  'i'	; 10-17h
	db	'o',  'p',  '[',  ']',	 CR,  00h,  'a',  's'	; 18-1Fh
	db	'd',  'f',  'g',  'h',	'j',  'k',  'l',  ';'	; 20-27h
	db	"'",  '`',  00h,  '\',	'z',  'x',  'c',  'v'	; 28-2Fh
	db	'b',  'n',  'm',  ',',	'.',  '/',  00h,  '*'	; 30-37h
	db	00h,  ' ',  00h,  00h,	CTRLA,CTRLK,00h,  00h	; 38-3Fh
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  CTRLB ; 40-47h
	db	CTRLK,CTRLU,'-',  CTRLH,00h,  CTRLL,'+',  CTRLE ; 48-4Fh
	db	CTRLJ,CTRLD,00h,  00h,	00h,  00h,  00h,  00h	; 50-57h
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 58-5Fh
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 60-67h
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 68-6Fh
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 70-77h
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 78-7Fh

	public	_abXlatCtrl
_abXlatCtrl label  byte
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 00-07h
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 08-0Fh
	db	CTRLQ,CTRLW,CTRLE,CTRLR,CTRLT,CTRLY,CTRLU,CTRLI ; 10-17h
	db	CTRLO,CTRLP,00h,  00h,	00h,  00h,  CTRLA,CTRLS ; 18-1Fh
	db	CTRLD,CTRLF,CTRLG,CTRLH,CTRLJ,CTRLK,CTRLL,00h	; 20-27h
	db	00h,  00h,  00h,  00h,	CTRLZ,CTRLX,CTRLC,CTRLV ; 28-2Fh
	db	CTRLB,CTRLN,CTRLM,00h,	00h,  00h,  00h,  00h	; 30-37h
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 38-3Fh
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 40-47h
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 48-4Fh
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 50-57h
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 58-5Fh
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 60-67h
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 68-6Fh
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 70-77h
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 78-7Fh

	public	_abXlatShift
_abXlatShift label  byte
	db	00h, ESCAPE,'!',  '@',	'#',  '$',  '%',  '^'	; 00-07h
	db	'&',  '*',  '(',  ')',	'_',  '+',   BS,  TAB	; 08-0Fh
	db	'Q',  'W',  'E',  'R',	'T',  'Y',  'U',  'I'	; 10-17h
	db	'O',  'P',  '{',  '}',	 CR,  00h,  'A',  'S'	; 18-1Fh
	db	'D',  'F',  'G',  'H',	'J',  'K',  'L',  ':'	; 20-27h
	db	'"',  '~',  00h,  '|',  'Z',  'X',  'C',  'V'   ; 28-2Fh
	db	'B',  'N',  'M',  '<',	'>',  '?',  00h,  '*'	; 30-37h
	db	00h,  ' ',  00h,  00h,	00h,  00h,  00h,  00h	; 38-3Fh
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  CTRLB ; 40-47h
	db	CTRLK,CTRLU,'-',  CTRLH,00h,  CTRLL,'+',  CTRLE ; 48-4Fh
	db	CTRLJ,CTRLD,00h,  00h,	00h,  00h,  00h,  00h	; 50-57h
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 58-5Fh
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 60-67h
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 68-6Fh
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 70-77h
	db	00h,  00h,  00h,  00h,	00h,  00h,  00h,  00h	; 78-7Fh

	public	_bNestedEntries
_bNestedEntries	db	0		; don't allow more than 1 nested entry

	ifdef	DEBUG
	public	_szAssert
_szAssert   db	"Assertion failure in module %s, line %d", LF, NULL
	endif

	public	_aintSimulate		; indexed by IDT # (less IDT_MASTER_BASE)
_aintSimulate	label byte
	db	INT_TMR
	db	INT_KBD
	db	INT_SLAVE
	db	INT_COM2
	db	INT_COM1
	db	INT_LPT2
	db	INT_DISKETTE
	db	INT_LPT1
	db	INT_RTC
	db	INT_IRQ2
	db	INT_IRQ10
	db	INT_IRQ11
	db	INT_IRQ12
	db	INT_CP
	db	INT_DISK
	db	INT_IRQ15

	align	4
	public	_afnEmulate
_afnEmulate label dword
	dd	0			; 00h	ADD
	dd	0			; 01h	ADD
	dd	0			; 02h	ADD
	dd	0			; 03h	ADD
	dd	0			; 04h	ADD
	dd	0			; 05h	ADD
	dd	0			; 06h	PUSH
	dd	0			; 07h	POP
	dd	0			; 08h	OR
	dd	0			; 09h	OR
	dd	0			; 0Ah	OR
	dd	0			; 0Bh	OR
	dd	0			; 0Ch	OR
	dd	0			; 0Dh	OR
	dd	0			; 0Eh	PUSH
	dd	0			; 0Fh
	dd	0			; 10h	ADC
	dd	0			; 11h	ADC
	dd	0			; 12h	ADC
	dd	0			; 13h	ADC
	dd	0			; 14h	ADC
	dd	0			; 15h	ADC
	dd	0			; 16h	PUSH
	dd	0			; 17h	POP
	dd	0			; 18h	SBB
	dd	0			; 19h	SBB
	dd	0			; 1Ah	SBB
	dd	0			; 1Bh	SBB
	dd	0			; 1Ch	SBB
	dd	0			; 1Dh	SBB
	dd	0			; 1Eh	PUSH
	dd	0			; 1Fh	POP
	dd	0			; 20h	AND
	dd	0			; 21h	AND
	dd	0			; 22h	AND
	dd	0			; 23h	AND
	dd	0			; 24h	AND
	dd	0			; 25h	AND
	dd	0			; 26h	ES
	dd	0			; 27h	DAA
	dd	0			; 28h	SUB
	dd	0			; 29h	SUB
	dd	0			; 2Ah	SUB
	dd	0			; 2Bh	SUB
	dd	0			; 2Ch	SUB
	dd	0			; 2Dh	SUB
	dd	0			; 2Eh	CS
	dd	0			; 2Fh	DAS
	dd	0			; 30h	XOR
	dd	0			; 31h	XOR
	dd	0			; 32h	XOR
	dd	0			; 33h	XOR
	dd	0			; 34h	XOR
	dd	0			; 35h	XOR
	dd	0			; 36h	SS
	dd	0			; 37h	AAA
	dd	0			; 38h	CMP
	dd	0			; 39h	CMP
	dd	0			; 3Ah	CMP
	dd	0			; 3Bh	CMP
	dd	0			; 3Ch	CMP
	dd	0			; 3Dh	CMP
	dd	0			; 3Eh	DS
	dd	0			; 3Fh	AAS
	dd	0			; 40h	INC
	dd	0			; 41h	INC
	dd	0			; 42h	INC
	dd	0			; 43h	INC
	dd	0			; 44h	INC
	dd	0			; 45h	INC
	dd	0			; 46h	INC
	dd	0			; 47h	INC
	dd	0			; 48h	DEC
	dd	0			; 49h	DEC
	dd	0			; 4Ah	DEC
	dd	0			; 4Bh	DEC
	dd	0			; 4Ch	DEC
	dd	0			; 4Dh	DEC
	dd	0			; 4Eh	DEC
	dd	0			; 4Fh	DEC
	dd	0			; 50h	PUSH
	dd	0			; 51h	PUSH
	dd	0			; 52h	PUSH
	dd	0			; 53h	PUSH
	dd	0			; 54h	PUSH
	dd	0			; 55h	PUSH
	dd	0			; 56h	PUSH
	dd	0			; 57h	PUSH
	dd	0			; 58h	POP
	dd	0			; 59h	POP
	dd	0			; 5Ah	POP
	dd	0			; 5Bh	POP
	dd	0			; 5Ch	POP
	dd	0			; 5Dh	POP
	dd	0			; 5Eh	POP
	dd	0			; 5Fh	POP
	dd	0			; 60h	PUSHA
	dd	0			; 61h	POPA
	dd	0			; 62h	BOUND
	dd	0			; 63h	ARPL
	dd	0			; 64h	FS
	dd	0			; 65h	GS
	dd	0			; 66h	OSIZE
	dd	0			; 67h	ASIZE
	dd	0			; 68h	PUSH
	dd	0			; 69h	IMUL
	dd	0			; 6Ah	PUSH
	dd	0			; 6Bh	IMUL
	dd	0			; 6Ch	INS
	dd	0			; 6Dh	INS
	dd	0			; 6Eh	OUTS
	dd	0			; 6Fh	OUTS
	dd	0			; 70h	JO
	dd	0			; 71h	JNO
	dd	0			; 72h	JB
	dd	0			; 73h	JNB
	dd	0			; 74h	JZ
	dd	0			; 75h	JNZ
	dd	0			; 76h	JBE
	dd	0			; 77h	JNBE
	dd	0			; 78h	JS
	dd	0			; 79h	JNS
	dd	0			; 7Ah	JP
	dd	0			; 7Bh	JNP
	dd	0			; 7Ch	JL
	dd	0			; 7Dh	JGE
	dd	0			; 7Eh	JLE
	dd	0			; 7Fh	JG
	dd	0			; 80h
	dd	0			; 81h
	dd	0			; 82h
	dd	0			; 83h
	dd	0			; 84h	TEST
	dd	0			; 85h	TEST
	dd	0			; 86h	XCHG
	dd	0			; 87h	XCHG
	dd	0			; 88h	MOV
	dd	0			; 89h	MOV
	dd	0			; 8Ah	MOV
	dd	0			; 8Bh	MOV
	dd	0			; 8Ch	MOV
	dd	0			; 8Dh	LEA
	dd	0			; 8Eh	MOV
	dd	0			; 8Fh	POP
	dd	0			; 90h	NOP
	dd	0			; 91h	XCHG
	dd	0			; 92h	XCHG
	dd	0			; 93h	XCHG
	dd	0			; 94h	XCHG
	dd	0			; 95h	XCHG
	dd	0			; 96h	XCHG
	dd	0			; 97h	XCHG
	dd	0			; 98h	CBW
	dd	0			; 99h	CWD
	dd	0			; 9Ah	CALL
	dd	0			; 9Bh	WAIT
	dd	0			; 9Ch	PUSHF
	dd	0			; 9Dh	POPF
	dd	0			; 9Eh	SAHF
	dd	0			; 9Fh	LAHF
	dd	0			; A0h	MOV
	dd	0			; A1h	MOV
	dd	0			; A2h	MOV
	dd	0			; A3h	MOV
	dd	0			; A4h	MOVSB
	dd	0			; A5h	MOVSW
	dd	0			; A6h	CMPSB
	dd	0			; A7h	CMPSW
	dd	0			; A8h	TEST
	dd	0			; A9h	TEST
	dd	0			; AAh	STOSB
	dd	0			; ABh	STOSW
	dd	0			; ACh	LODSB
	dd	0			; ADh	LODSW
	dd	0			; AEh	SCASB
	dd	0			; AFh	SCASW
	dd	0			; B0h	MOV
	dd	0			; B1h	MOV
	dd	0			; B2h	MOV
	dd	0			; B3h	MOV
	dd	0			; B4h	MOV
	dd	0			; B5h	MOV
	dd	0			; B6h	MOV
	dd	0			; B7h	MOV
	dd	0			; B8h	MOV
	dd	0			; B9h	MOV
	dd	0			; BAh	MOV
	dd	0			; BBh	MOV
	dd	0			; BCh	MOV
	dd	0			; BDh	MOV
	dd	0			; BEh	MOV
	dd	0			; BFh	MOV
	dd	0			; C0h
	dd	0			; C1h
	dd	0			; C2h	RET
	dd	0			; C3h	RET
	dd	0			; C4h	LES
	dd	0			; C5h	LDS
	dd	0			; C6h	MOV
	dd	0			; C7h	MOV
	dd	0			; C8h	ENTER
	dd	0			; C9h	LEAVE
	dd	0			; CAh	RETF
	dd	0			; CBh	RETF
	dd	EmulateINT3		; CCh	INT3
	dd	EmulateINT		; CDh	INT
	dd	0			; CEh	INTO
	dd	0			; CFh	IRET
	dd	0			; D0h
	dd	0			; D1h
	dd	0			; D2h
	dd	0			; D3h
	dd	0			; D4h	AAM
	dd	0			; D5h	AAD
	dd	0			; D6h	GBP
	dd	0			; D7h	XLAT
	dd	0			; D8h	ESC
	dd	0			; D9h	ESC
	dd	0			; DAh	ESC
	dd	0			; DBh	ESC
	dd	0			; DCh	ESC
	dd	0			; DDh	ESC
	dd	0			; DEh	ESC
	dd	0			; DFh	ESC
	dd	0			; E0h	LOOPNZ
	dd	0			; E1h	LOOPZ
	dd	0			; E2h	LOOP
	dd	0			; E3h	JCXZ
	dd	EmulateInALnn		; E4h	IN
	dd	EmulateInAXnn		; E5h	IN
	dd	EmulateOutnnAL		; E6h	OUT
	dd	EmulateOutnnAX		; E7h	OUT
	dd	0			; E8h	CALL
	dd	0			; E9h	JMP
	dd	0			; EAh	JMP
	dd	0			; EBh	JMP
	dd	EmulateInALDX		; ECh	IN AL,DX
	dd	EmulateInAXDX		; EDh	IN AX,DX
	dd	EmulateOutDXAL		; EEh	OUT DX,AL
	dd	EmulateOutDXAX		; EFh	OUT DX,AX
	dd	0			; F0h	LOCK
	dd	0			; F1h
	dd	0			; F2h	REPNZ
	dd	0			; F3h	REPZ
	dd	0			; F4h	HLT
	dd	0			; F5h	CMC
	dd	0			; F6h
	dd	0			; F7h
	dd	0			; F8h	CLC
	dd	0			; F9h	STC
	dd	0			; FAh	CLI
	dd	0			; FBh	STI
	dd	0			; FCh	CLD
	dd	0			; FDh	STD
	dd	0			; FEh
	dd	0			; FFh

	public	SavedIMRs
SavedIMRs	dd  ?

CLOSE_DATA


OPEN_CODE

	extrn	_x86Trap:near		; in x86debug.c

;
;   Entry conditions for all these handlers dictate that
;   CS == CODE_SEG and SS == DATA_SEG, because either the fault occured
;   in ring 0 code, in which case SS was already DATA_SEG, or it didn't,
;   in which case the processor automatically loaded SS from SS0 in
;   the TSS we created.
;

	assume	cs:CODE_SEG,ds:nothing,es:nothing,ss:DATA_SEG

	public	DivError
DivError	label  far
	push	0			; dummy error code
	push	IDT_DIVERROR
	jmp	short GeneralFault

	public	Debug
Debug		label	far
	push	0			; dummy error code
	push	IDT_DEBUG
	jmp	short GeneralFault

	public	NMI
NMI		label	far
	ResetNMI
	push	0			; dummy error code
	push	IDT_NMI
	jmp	short GeneralFault

	public	BreakPoint
BreakPoint	label	far
	push	0			; dummy error code
	push	IDT_BREAKPOINT
	jmp	short GeneralFault

	public	Overflow
Overflow	label	far
	push	0			; dummy error code
	push	IDT_OVERFLOW
	jmp	short GeneralFault

	public	Bounds
Bounds		label	far
	push	0			; dummy error code
	push	IDT_BOUNDS
	jmp	short GeneralFault

	public	InvOpCode
InvOpCode	label	far
	push	0			; dummy error code
	push	IDT_INVOPCODE
	jmp	short GeneralFault

	public	NoCP
NoCP		label	far
	push	0			; dummy error code
	push	IDT_NOCP
	jmp	short GeneralFault

	public	DblFault
DblFault	label	far
	push	IDT_DBLFAULT
	jmp	short GeneralFault

	public	TSSFault
TSSFault	label	far
	push	IDT_TSSFAULT
	jmp	short GeneralFault

	public	SegFault
SegFault	label	far
	push	IDT_SEGFAULT
	jmp	short GeneralFault

	public	StackFault
StackFault	label	far
	push	IDT_STACKFAULT
	jmp	short GeneralFault

	public	PageFault
PageFault	label	far
	push	IDT_PAGEFAULT
	jmp	short GeneralFault

	public	CPFault
CPFault		label	far
	push	IDT_CPFAULT
	jmp	short GeneralFault

	public	UnknownFault
UnknownFault	label	far
	push	0			; dummy error code
	push	IDT_RESERVED1F
	jmp	short GeneralFault


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   GeneralFault
;
;   ENTRY
;	None
;
;   EXIT
;	None
;
;   USES
;	None
;
	assume	cs:CODE_SEG,ds:nothing,es:nothing,ss:DATA_SEG

	public	GeneralFault
GeneralFault	proc far

	pusha
	push	ds
	push	es
	sub	esp,esf_saveES		; reserve space for the rest
	mov	ebp,esp			; EBP -> ESF

	mov	ax,SEL_DATA
	mov	ds,eax
	mov	es,eax
	assume	ds:DATA_SEG,es:DATA_SEG

	push	DEBUG_NONE
	call	Debugger		; call debugger router

	add	esp,esf_saveES		; throw away the rest
	pop	es
	pop	ds
	assume	ds:nothing,es:nothing

	popa
	add	esp,isf_EIP		; throw away iTrap and iErrCode
	iretd
GeneralFault	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   GPFault
;
;   ENTRY
;	None
;
;   EXIT
;	None
;
;   USES
;	None
;
	assume	cs:CODE_SEG,ds:nothing,es:nothing,ss:DATA_SEG

	public	GPFault
GPFault proc	far
	push	IDT_GPFAULT

	pusha
	push	ds
	push	es
	sub	esp,esf_saveES		; reserve space for the rest
	mov	ebp,esp			; EBP -> ESF

	mov	cx,SEL_DATA		; use CX to give AX a break
	mov	ds,ecx			; (seriously, AX often contains
	mov	es,ecx			; data related to the operation we
	assume	ds:DATA_SEG,es:DATA_SEG ; want to emulate, so leave it alone)

	test	[ebp].esf_Flags,FLAGS_V86
	jz	short gpfault_error	; faulted in protected-mode?!

;;;	push	DEBUG_PEEK		; take a peek
;;;	call	Debugger		; call debugger router

	mov	ebx,[_pmbZero]
	movzx	esi,word ptr [ebp].esf_CS
	shl	esi,4
	add	esi,[ebp].esf_EIP
	movzx	edi,byte ptr [ebx+esi]	; EDI == opcode byte
	mov	ecx,_afnEmulate[edi*4]
	jecxz	short gpfault_error	; no emulation function exists
	call	ecx			; call it

	public	GPFaultReturn
GPFaultReturn  label near
	add	esp,esf_saveES		; throw away the rest
	pop	es
	pop	ds
	assume	ds:nothing,es:nothing

	popa
	add	esp,isf_EIP		; throw away iTrap and iErrCode
	iretd

gpfault_error:
	push	DEBUG_NONE		; something's very wrong
	call	Debugger		;
	jmp	GPFaultReturn		; we can't do anything but return
GPFault endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   HWInterrupt
;
;   ENTRY
;	None
;
;   EXIT
;	None
;
;   USES
;	None
;
	assume	cs:CODE_SEG,ds:nothing,es:nothing,ss:DATA_SEG

	public	HWInterrupt
HWInterrupt	proc far
	test	[esp].isf_Flags,FLAGS_V86
	jnz	short hwint_v86		; interrupted v86-mode

	mov	al,PIC_EOI
	cmp	byte ptr [esp].isf_iTrap,IDT_RTC
	jb	short hwint_notslave	;
	out	PIC_SLAVE,al		; EOI the slave
hwint_notslave:
	out	PIC_MASTER,al		; EOI the master
	add	esp,isf_EIP		; throw away iTrap and iErrCode
	iretd

hwint_v86:
	pusha
	push	ds
	push	es
	sub	esp,esf_saveES		; reserve space for the rest
	mov	ebp,esp			; EBP -> ESF

	mov	ax,SEL_DATA
	mov	ds,eax
	mov	es,eax
	assume	ds:DATA_SEG,es:DATA_SEG

	mov	ebx,[_pmbZero]
	mov	edi,[ebp].esf_iTrap	; EDI == IDT #
	movzx	edi,_aintSimulate[edi-IDT_MASTER_BASE]	; convert to IVT #
	call	SimulateINT		; simulate it!

	add	esp,esf_saveES		; throw away the rest
	pop	es
	pop	ds
	assume	ds:nothing,es:nothing

	popa
	add	esp,isf_EIP		; throw away iTrap and iErrCode
	iretd
HWInterrupt	endp


	public	IRQ2Interrupt
IRQ2Interrupt	label	far
	push	0			; dummy error code
	push	IDT_SLAVE
	jmp	HWInterrupt

	public	IRQ3Interrupt
IRQ3Interrupt	label	far
	push	0			; dummy error code
	push	IDT_COM2
	jmp	HWInterrupt

	public	IRQ4Interrupt
IRQ4Interrupt	label	far
	push	0			; dummy error code
	push	IDT_COM1
	jmp	HWInterrupt

	public	IRQ5Interrupt
IRQ5Interrupt	label	far
	push	0			; dummy error code
	push	IDT_LPT2
	jmp	HWInterrupt

	public	IRQ6Interrupt
IRQ6Interrupt	label	far
	push	0			; dummy error code
	push	IDT_DISKETTE
	jmp	HWInterrupt

	public	IRQ7Interrupt
IRQ7Interrupt	label	far
	iretd				; this is the 8259's spurious int vector!
;	push	0			; dummy error code
;	push	IDT_LPT1
;	jmp	HWInterrupt


	public	IRQ8Interrupt
IRQ8Interrupt	label	far
	push	0			; dummy error code
	push	IDT_RTC
	jmp	HWInterrupt

	public	IRQ9Interrupt
IRQ9Interrupt	label	far
	push	0			; dummy error code
	push	IDT_IRQ2
	jmp	HWInterrupt

	public	IRQAInterrupt
IRQAInterrupt	label	far
	push	0			; dummy error code
	push	IDT_IRQ10
	jmp	HWInterrupt

	public	IRQBInterrupt
IRQBInterrupt	label	far
	push	0			; dummy error code
	push	IDT_IRQ11
	jmp	HWInterrupt

	public	IRQCInterrupt
IRQCInterrupt	label	far
	push	0			; dummy error code
	push	IDT_IRQ12
	jmp	HWInterrupt

	public	IRQDInterrupt
IRQDInterrupt	label	far
	push	0			; dummy error code
	push	IDT_CP
	jmp	HWInterrupt

	public	IRQEInterrupt
IRQEInterrupt	label	far
	push	0			; dummy error code
	push	IDT_DISK
	jmp	HWInterrupt

	public	IRQFInterrupt
IRQFInterrupt	label	far
	iretd				; this is the 8259's spurious int vector!
;	push	0			; dummy error code
;	push	IDT_IRQ15
;	jmp	HWInterrupt


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   TMRInterrupt
;
;   ENTRY
;	None
;
;   EXIT
;	None
;
;   USES
;	None
;
	assume	cs:CODE_SEG,ds:nothing,es:nothing,ss:DATA_SEG

	public	TMRInterrupt
TMRInterrupt	proc far
	push	0			; dummy iErrCode
	push	IDT_TMR			; iTrap

	test	[esp].isf_Flags,FLAGS_V86
	jnz	HWInterrupt		; interrupted v86-mode

	push	eax
	push	ebx
	push	edx

;;;	ifdef	PS2
;
;   On those level-triggered PS/2's, we've got to shut the damn device up!
;
	in	al,61h
	or	al,80h
	out	61h,al

;;;	endif

	mov	ebx,offset _vsMonitor
	call	GetVRAMAddress		; sets EAX (address) and EDX (size)
	xor	[eax+158].b_lo,(' ' xor '*')

	mov	al,PIC_EOI
	out	PIC_MASTER,al

	pop	edx
	pop	ebx
	pop	eax

	add	esp,isf_EIP		; throw away iTrap and iErrCode
	iretd
TMRInterrupt endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   KBDInterrupt
;
;   ENTRY
;	None
;
;   EXIT
;	None
;
;   USES
;	None
;
	assume	cs:CODE_SEG,ds:nothing,es:nothing,ss:DATA_SEG

	public	KBDInterrupt
KBDInterrupt	proc far
	push	0			; dummy iErrCode
	push	IDT_KBD			; iTrap

	pusha
	push	ds
	push	es
	sub	esp,esf_saveES		; reserve space for the rest
	mov	ebp,esp			; EBP -> ESF

	mov	ax,SEL_DATA
	mov	ds,eax
	mov	es,eax
	assume	ds:DATA_SEG,es:DATA_SEG

	in	al,KBDPORT_DATA		; read scan code

	cmp	al,KBDSCAN_F12		; F12 key?
	je	short kbd_debugger	; yes

	cmp	al,KBDSCAN_SYSREQ	; SYSREQ key?
	jne	short kbd_testv86	; no

kbd_debugger:
	push	DEBUG_NONE		; yes, break into debugger immediately
	call	Debugger		; call debugger router
	jnc	kbd_return		; terminate this interrupt if the call was accepted

kbd_testv86:
	test	[ebp].esf_Flags,FLAGS_V86
	jz	short kbd_bufferit	; did not interrupt v86-mode
	add	esp,esf_saveES		; throw away the rest
	pop	es
	pop	ds
	assume	ds:nothing,es:nothing
	popa
	jmp	HWInterrupt		; simulate interrupt into VM

kbd_bufferit:
	call	CheckShift		; is AL shift key or break code?
	jz	short kbd_eoi		; yes

	mov	edi,[_pbKeyTail]	;
	mov	esi,edi			;
	inc	edi			;
	cmp	edi,offset _pbKeyHead	;
	jb	short kbd_tailok	;
	mov	edi,offset _abKeyBuff	;
kbd_tailok:				;
	cmp	edi,[_pbKeyHead]	; will this overflow the buffer?
	je	short kbd_eoi		; yes

	mov	ebx,offset _abXlatNorm	;
	test	byte ptr [_flKeyShift],KBFLAG_SHIFTMASK
	jz	short kbd_xlatctrl	;
	mov	ebx,offset _abXlatShift ;
	jmp	short kbd_xlatnorm	;
kbd_xlatctrl:
	test	byte ptr [_flKeyShift],KBFLAG_LCTRL
	jz	short kbd_xlatnorm	;
	mov	ebx,offset _abXlatCtrl	;
kbd_xlatnorm:
	xlat				;
	cmp	al,CTRLC		; abort sequence?
	je	short kbd_abort		; yes
	cmp	al,ESCAPE		; abort sequence?
	jne	short kbd_save		; no
kbd_abort:				; yes
	or	[_flKeyEvent],KEYEVENT_ABORT
	jmp	short kbd_eoi
kbd_save:
	mov	[esi],al		; save the code received
	mov	[_pbKeyTail],edi	; and update the tail pointer

kbd_eoi:
	mov	al,PIC_EOI
	out	PIC_MASTER,al

kbd_return:
	add	esp,esf_saveES		; throw away the rest
	pop	es
	pop	ds
	assume	ds:nothing,es:nothing

	popa
	add	esp,isf_EIP		; throw away iTrap and iErrCode
	iretd
KBDInterrupt endp


	public	CheckShift
CheckShift proc near
	mov	ah,al			; save original scan code in AH
	and	al,NOT KBDSCAN_BREAKPREFIX
	cmp	al,KBDSCAN_RSHIFT	; check for shift keys
	jne	short cs_notrs		;
	mov	al,KBFLAG_RSHIFT	;
	jmp	short cs_shift		;
cs_notrs:
	cmp	al,KBDSCAN_LSHIFT	;
	jne	short cs_notls		;
	mov	al,KBFLAG_LSHIFT	;
	jmp	short cs_shift		;
cs_notls:
	cmp	al,KBDSCAN_LCTRL	;
	jne	short cs_notshift	;
	mov	al,KBFLAG_LCTRL		;
cs_shift:
	test	ah,KBDSCAN_BREAKPREFIX	; is this a shift make or break?
	jnz	short cs_shiftbreak	; break
	or	byte ptr [_flKeyShift],al
	sub	al,al			; set Z flag
	ret				; bye-bye
cs_shiftbreak:
	not	al			;
	and	byte ptr [_flKeyShift],al
	sub	al,al			; set Z flag
	ret				; bye-bye
cs_notshift:
	and	ah,KBDSCAN_BREAKPREFIX	;
	cmp	ah,KBDSCAN_BREAKPREFIX	; return Z flag set if break key
	ret
CheckShift endp


	public	_DrainKbd
_DrainKbd proc	near
	pushfd
	cli
	push	eax
	in	al,KBDPORT_STATUS
	test	al,KBDSTATUS_OUTPUT
	jz	short drain_done
drain_loop:
	in	al,KBDPORT_DATA
	call	_WaitKbdOutput
	jnz	drain_loop
drain_done:
	pop	eax
	popfd
	ret
_DrainKbd	endp

	public	_EnableKbd
_EnableKbd	proc near
	push	eax
	call	_WaitKbdInput
	mov	al,KBDCMD_ENABLE_KBD
	out	KBDPORT_CMD,al
	pop	eax
	ret
_EnableKbd	endp

	public	_DisableKbd
_DisableKbd	proc near
	push	eax
	call	_WaitKbdInput
	mov	al,KBDCMD_DISABLE_KBD
	out	KBDPORT_CMD,al
	call	_WaitKbdInput
	pop	eax
	ret
_DisableKbd	endp

	public	_DrainMse
_DrainMse proc	near
	pushfd
	cli
	push	eax
	in	al,KBDPORT_STATUS
	test	al,KBDSTATUS_OUTPUT
	jz	short drain_done
drain_loop:
	test	al,KBDSTATUS_AUXDATA
	jz	short drain_done
	in	al,KBDPORT_DATA
	call	_WaitKbdOutput
	jnz	drain_loop
drain_done:
	pop	eax
	popfd
	ret
_DrainMse	endp

	public	_EnableMse
_EnableMse	proc near
	push	eax
	call	_WaitKbdInput
	mov	al,KBDCMD_MSE
	out	KBDPORT_CMD,al
	call	_WaitKbdInput
	call	_WaitKbdOutput
	mov	al,KBDMSE_ENABLE
	out	KBDPORT_DATA,al
	pop	eax
	ret
_EnableMse	endp

	public	_DisableMse
_DisableMse	proc near
	push	eax
	call	_WaitKbdInput
	mov	al,KBDCMD_MSE
	out	KBDPORT_CMD,al
	call	_WaitKbdInput
	call	_WaitKbdOutput
	mov	al,KBDMSE_DISABLE
	out	KBDPORT_DATA,al
	pop	eax
	ret
_DisableMse	endp

	public	_WaitKbdInput
_WaitKbdInput	proc near
	push	ecx
	mov	ecx,20000h
wait_loop:
	in	al,KBDPORT_STATUS
	test	al,KBDSTATUS_INPUT
	loopnz	wait_loop
	pop	ecx
	ret
_WaitKbdInput	endp

	public	_WaitKbdOutput
_WaitKbdOutput	proc near
	push	ecx
	mov	ecx,20000h
wait_loop:
	in	al,KBDPORT_STATUS
	test	al,KBDSTATUS_OUTPUT
	loopz	wait_loop
	pop	ecx
	ret
_WaitKbdOutput	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Debugger
;
;   ENTRY
;	EBP -> ESF
;	DEBUG_* flags on stack
;	Interrupts off
;
;   EXIT
;	Carry clear if debugger entered, set if not
;
;   USES
;	None
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	Debugger
Debugger proc near
	cmp	[_bNestedEntries],2	; have we already re-entered once?
	cmc
	jb	dbg_exit		; yes

	inc	[_bNestedEntries]
	push	eax			; save scratch regs
	push	edx			;

	in	al,PIC_MASTER_IMR
	mov	dl,al			; save master interrupt mask in DL
	mov	al,not ((1 shl IRQ_TMR) or (1 shl IRQ_KBD))
	IODelay
	out	PIC_MASTER_IMR,al	; enable only timer and keyboard ints
	in	al,PIC_SLAVE_IMR
	mov	dh,al			; save slave interrupt mask in DH
	mov	al,0FFh
	IODelay
	out	PIC_SLAVE_IMR,al

	test	[_flTrace],TRACE_MASKINTS
	jz	short no_mask_ints
	mov	edx,[SavedIMRs]
no_mask_ints:
	mov	[ebp].esf_dwIMRs,edx	; EDX == saved IMRs

	ReadISR PIC_MASTER		; EAX == master ISR register

	push	eax
	test	al,1 shl IRQ_TMR	; timer interrupt in service?
	jz	short no_tmr_int	; no
	mov	al,PIC_EOI		; yes, we have to issue EOI to
	IODelay				; allow further timer interrupts
	out	PIC_MASTER,al		;
no_tmr_int:
	pop	eax

	test	al,1 shl IRQ_KBD	; previous interrupt in service?
	jz	short no_kbd_int	; no
	mov	al,PIC_EOI		; yes, we have to issue EOI to
	IODelay				; allow further keyboard interrupts
	out	PIC_MASTER,al		;
no_kbd_int:				;

	push	ecx			; save C-trashable regs
	mov	ecx,[esp+10h]		; ECX == flDebug pushed by caller

	push	edi			; push nn
	push	ecx			; push flDebug
	push	ebp			; push PESF
	call	_x86Trap		; call debugger
	add	esp,12			; clean up the stack

	pop	ecx			; restore C-trashable regs

	mov	edx,[ebp].esf_dwIMRs
	test	[_flTrace],TRACE_MASKINTS
	jz	short no_mask_ints2
	mov	[SavedIMRs],edx
	sub	edx,edx
	dec	edx			; mask everything (EDX == -1)
no_mask_ints2:

	mov	al,dh			; restore slave interrupt mask
	out	PIC_SLAVE_IMR,al	; done!
	mov	al,dl			; restore master interrupt mask
	out	PIC_MASTER_IMR,al	; done!
	clc

	dec	[_bNestedEntries]
	pop	edx			; restore scratch regs
	pop	eax			;
dbg_exit:				; throw away DEBUG_* flags
	ret	4			; because this is a PASCAL function
Debugger endp


CLOSE_CODE


	end
