	page	,132
	title	V86IO.ASM

;
;   V86IO.ASM
;   V86-mode I/O instruction support
;
;   by Jeff Parsons 05-Jun-1992
;   with a little help from Intel's 386 Programmer's Reference Manual :)
;


	include all.inc


OPEN_CODE

	extrn	Debugger:near		; in 386trap.asm


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   EmulateInALnn (0E4h)
;
;   ENTRY
;	EBX -> megabyte alias (eg, pmbZero)
;	ESI -> current instruction
;	EBP -> ESF with valid v86* fields
;	Interrupts disabled (so don't dink around!)
;	EAX and EDX still contain the VM's settings
;
;   EXIT
;	Nothing in particular
;
;   USES
;	Anything it pleases
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	EmulateInALnn
EmulateInALnn	proc near

	test	[_flTrace],TRACE_TRAPIO
	jz	short EmIO

	push	DEBUG_VIO
	call	Debugger		; call debugger router
	test	[_flTrace],TRACE_REEXEC
	jnz	short EmExit
EmIO:
	movzx	dx,byte ptr [ebx+esi+1] ; DX == imm. portion of opcode
	in	al,dx			; do the actual IN, update AL
	mov	byte ptr [ebp].esf_EAX,al

	inc	word ptr [ebp].esf_EIP	; advance instruction pointer
EmExit:
	ret

EmulateInALnn	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   EmulateInAXnn (0E5h)
;
;   ENTRY
;	EBX -> megabyte alias (eg, pmbZero)
;	ESI -> current instruction
;	EBP -> ESF with valid v86* fields
;	Interrupts disabled (so don't dink around!)
;	EAX and EDX still contain the VM's settings
;
;   EXIT
;	Nothing in particular
;
;   USES
;	Anything it pleases
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	EmulateInAXnn
EmulateInAXnn	proc near

	test	[_flTrace],TRACE_TRAPIO
	jz	short EmIO

	push	DEBUG_VIO
	call	Debugger		; call debugger router
	test	[_flTrace],TRACE_REEXEC
	jnz	short EmExit
EmIO:
	movzx	dx,byte ptr [ebx+esi+1] ; DX == imm. portion of opcode
	in	ax,dx			; do the actual IN, update AX
	mov	word ptr [ebp].esf_EAX,ax

	inc	word ptr [ebp].esf_EIP	; advance instruction pointer
EmExit:
	ret

EmulateInAXnn	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   EmulateOutnnAL (0E6h)
;
;   ENTRY
;	EBX -> megabyte alias (eg, pmbZero)
;	ESI -> current instruction
;	EBP -> ESF with valid v86* fields
;	Interrupts disabled (so don't dink around!)
;	EAX and EDX still contain the VM's settings
;
;   EXIT
;	Nothing in particular
;
;   USES
;	Anything it pleases
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	EmulateOutnnAL
EmulateOutnnAL	proc near

	test	[_flTrace],TRACE_TRAPIO
	jz	short EmIO

	push	DEBUG_VIO
	call	Debugger		; call debugger router
	test	[_flTrace],TRACE_REEXEC
	jnz	short EmExit
EmIO:
	movzx	dx,byte ptr [ebx+esi+1] ; DX == imm. portion of opcode
	out	dx,al			; do the actual OUT

	inc	word ptr [ebp].esf_EIP	; advance instruction pointer
EmExit:
	ret

EmulateOutnnAL	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   EmulateOutnnAX (0E7h)
;
;   ENTRY
;	EBX -> megabyte alias (eg, pmbZero)
;	ESI -> current instruction
;	EBP -> ESF with valid v86* fields
;	Interrupts disabled (so don't dink around!)
;	EAX and EDX still contain the VM's settings
;
;   EXIT
;	Nothing in particular
;
;   USES
;	Anything it pleases
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	EmulateOutnnAX
EmulateOutnnAX	proc near

	test	[_flTrace],TRACE_TRAPIO
	jz	short EmIO

	push	DEBUG_VIO
	call	Debugger		; call debugger router
	test	[_flTrace],TRACE_REEXEC
	jnz	short EmExit
EmIO:
	movzx	dx,byte ptr [ebx+esi+1] ; DX == imm. portion of opcode
	out	dx,ax			; do the actual OUT

	inc	word ptr [ebp].esf_EIP	; advance instruction pointer
EmExit:
	ret

EmulateOutnnAX	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   EmulateInALDX (0ECh)
;
;   ENTRY
;	EBX -> megabyte alias (eg, pmbZero)
;	ESI -> current instruction
;	EBP -> ESF with valid v86* fields
;	Interrupts disabled (so don't dink around!)
;	EAX and EDX still contain the VM's settings
;
;   EXIT
;	Nothing in particular
;
;   USES
;	Anything it pleases
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	EmulateInALDX
EmulateInALDX	proc near

	test	[_flTrace],TRACE_TRAPIO
	jz	short EmIO

	push	DEBUG_VIO
	call	Debugger		; call debugger router
	test	[_flTrace],TRACE_REEXEC
	jnz	short EmExit
EmIO:
	in	al,dx			; do the actual IN, update AL
	mov	byte ptr [ebp].esf_EAX,al

	inc	word ptr [ebp].esf_EIP	; advance instruction pointer
EmExit:
	ret

EmulateInALDX	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   EmulateInAXDX (0EDh)
;
;   ENTRY
;	EBX -> megabyte alias (eg, pmbZero)
;	ESI -> current instruction
;	EBP -> ESF with valid v86* fields
;	Interrupts disabled (so don't dink around!)
;	EAX and EDX still contain the VM's settings
;
;   EXIT
;	Nothing in particular
;
;   USES
;	Anything it pleases
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	EmulateInAXDX
EmulateInAXDX	proc near

	test	[_flTrace],TRACE_TRAPIO
	jz	short EmIO

	push	DEBUG_VIO
	call	Debugger		; call debugger router
	test	[_flTrace],TRACE_REEXEC
	jnz	short EmExit
EmIO:
	in	ax,dx			; do the actual IN, update AX
	mov	word ptr [ebp].esf_EAX,ax

	inc	word ptr [ebp].esf_EIP	; advance instruction pointer
EmExit:
	ret

EmulateInAXDX	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   EmulateOutDXAL (0EEh)
;
;   ENTRY
;	EBX -> megabyte alias (eg, pmbZero)
;	ESI -> current instruction
;	EBP -> ESF with valid v86* fields
;	Interrupts disabled (so don't dink around!)
;	EAX and EDX still contain the VM's settings
;
;   EXIT
;	Nothing in particular
;
;   USES
;	Anything it pleases
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	EmulateOutDXAL
EmulateOutDXAL	proc near

	test	[_flTrace],TRACE_TRAPIO
	jz	short EmIO

	push	DEBUG_VIO
	call	Debugger		; call debugger router
	test	[_flTrace],TRACE_REEXEC
	jnz	short EmExit
EmIO:
	out	dx,al			; do the actual OUT

	inc	word ptr [ebp].esf_EIP	; advance instruction pointer
EmExit:
	ret

EmulateOutDXAL	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   EmulateOutDXAX (0EFh)
;
;   ENTRY
;	EBX -> megabyte alias (eg, pmbZero)
;	ESI -> current instruction
;	EBP -> ESF with valid v86* fields
;	Interrupts disabled (so don't dink around!)
;	EAX and EDX still contain the VM's settings
;
;   EXIT
;	Nothing in particular
;
;   USES
;	Anything it pleases
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	EmulateOutDXAX
EmulateOutDXAX	proc near

	test	[_flTrace],TRACE_TRAPIO
	jz	short EmIO

	push	DEBUG_VIO
	call	Debugger		; call debugger router
	test	[_flTrace],TRACE_REEXEC
	jnz	short EmExit
EmIO:
	out	dx,ax			; do the actual OUT

	inc	word ptr [ebp].esf_EIP	; advance instruction pointer
EmExit:
	ret

EmulateOutDXAX	endp


CLOSE_CODE


	end
