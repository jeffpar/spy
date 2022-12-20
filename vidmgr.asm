	TITLE	vidmgr.c
	.386P
include listing.inc
if @Version gt 510
.model FLAT
else
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT DWORD USE32 PUBLIC 'BSS'
_BSS	ENDS
_TLS	SEGMENT DWORD USE32 PUBLIC 'TLS'
_TLS	ENDS
FLAT	GROUP _DATA, CONST, _BSS
	ASSUME	CS: FLAT, DS: FLAT, SS: FLAT
endif
PUBLIC	_pVRAM
PUBLIC	_vsMonitor
PUBLIC	_vsVM
PUBLIC	_rowCursor
PUBLIC	_colCursor
PUBLIC	_rowCursorMax
PUBLIC	_colCursorMax
PUBLIC	_rowScreenVis
PUBLIC	_rowScreenMax
PUBLIC	_bDefColor
_DATA	SEGMENT
_szModule$S622 DB 'vidmgr.c', 00H
	ORG $+3
_pVRAM	DD	0a0000H
_vsVM	DD	0ffffffffH
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	ORG $+916
	ORG $+4
_vsMonitor DD	0ffffffffH
	DD	01H
	DD	08000H
	DD	00H
	DD	02000H
	DD	00H
	ORG $+916
_rowCursor DD	00H
_colCursor DD	00H
_rowCursorMax DD 00H
_colCursorMax DD 00H
_rowScreenVis DD 00H
_rowScreenMax DD 00H
_bDefColor DB	01fH
_DATA	ENDS
PUBLIC	_InitVS
PUBLIC	_InitVideoMgr
EXTRN	_SaveVS:NEAR
EXTRN	__scroll:NEAR
EXTRN	__setvistop:NEAR
EXTRN	_pmbZero:DWORD
_TEXT	SEGMENT
; File vidmgr.c
_InitVideoMgr PROC NEAR
; Line 43
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi
; Line 44
	mov	eax, DWORD PTR _pmbZero
	add	eax, 655360				; 000a0000H
	mov	DWORD PTR _pVRAM, eax
; Line 46
	push	OFFSET FLAT:_vsVM
	call	_InitVS
	add	esp, 4
; Line 47
	push	OFFSET FLAT:_vsMonitor
	call	_InitVS
	add	esp, 4
; Line 49
	push	1
	push	OFFSET FLAT:_vsVM
	call	_SaveVS
	add	esp, 8
; Line 58
	mov	eax, DWORD PTR _pmbZero
	movzx	eax, WORD PTR [eax+1098]
	mov	DWORD PTR _colCursorMax, eax
; Line 64
	mov	ecx, DWORD PTR _colCursorMax
	add	ecx, ecx
	mov	eax, 32768				; 00008000H
	cdq
	idiv	ecx
	mov	DWORD PTR _rowCursorMax, eax
; Line 65
	mov	eax, DWORD PTR _rowCursorMax
	mov	ecx, DWORD PTR _pmbZero
	movzx	ecx, BYTE PTR [ecx+1156]
	inc	ecx
	sub	eax, ecx
	mov	DWORD PTR _rowScreenMax, eax
; Line 66
	push	DWORD PTR _rowScreenMax
	call	__setvistop
	add	esp, 4
; Line 67
	mov	DWORD PTR _colCursor, 0
; Line 68
	mov	eax, DWORD PTR _rowScreenMax
	mov	DWORD PTR _rowCursor, eax
; Line 72
	push	0
	push	0
	push	0
	push	0
	push	0
	call	__scroll
	add	esp, 20					; 00000014H
; Line 74
$L623:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_InitVideoMgr ENDP
_TEXT	ENDS
EXTRN	_MemAlloc:NEAR
_TEXT	SEGMENT
_pvs$ = 8
_i$ = -4
_InitVS	PROC NEAR
; Line 78
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	push	ebx
	push	esi
	push	edi
; Line 81
	mov	DWORD PTR _i$[ebp], 0
	jmp	$L627
$L628:
	inc	DWORD PTR _i$[ebp]
$L627:
	cmp	DWORD PTR _i$[ebp], 4
	jge	$L629
; Line 82
	mov	eax, DWORD PTR _i$[ebp]
	mov	ecx, DWORD PTR _pvs$[ebp]
	mov	DWORD PTR [ecx+eax*4+8], 65536		; 00010000H
; Line 83
	mov	eax, DWORD PTR _i$[ebp]
	mov	ecx, DWORD PTR _pvs$[ebp]
	push	DWORD PTR [ecx+eax*4+8]
	call	_MemAlloc
	add	esp, 4
	mov	ecx, DWORD PTR _i$[ebp]
	mov	edx, DWORD PTR _pvs$[ebp]
	mov	DWORD PTR [edx+ecx*4+24], eax
; Line 84
	jmp	$L628
$L629:
; Line 89
	mov	eax, DWORD PTR _pvs$[ebp]
	mov	WORD PTR [eax+40], 948			; 000003b4H
; Line 90
	mov	eax, DWORD PTR _pvs$[ebp]
	mov	WORD PTR [eax+42], 949			; 000003b5H
; Line 91
	mov	eax, DWORD PTR _pvs$[ebp]
	mov	WORD PTR [eax+44], 954			; 000003baH
; Line 92
	mov	eax, DWORD PTR _pvs$[ebp]
	mov	WORD PTR [eax+46], 954			; 000003baH
; Line 93
	mov	eax, DWORD PTR _pvs$[ebp]
	mov	BYTE PTR [eax+145], 8
; Line 95
	mov	eax, DWORD PTR _pmbZero
	movzx	eax, BYTE PTR [eax+1159]
	test	al, 2
	jne	$L630
; Line 96
	mov	eax, DWORD PTR _pvs$[ebp]
	movzx	eax, BYTE PTR [eax+107]
	or	al, 1
	mov	ecx, DWORD PTR _pvs$[ebp]
	mov	BYTE PTR [ecx+107], al
; Line 97
	mov	eax, DWORD PTR _pvs$[ebp]
	mov	WORD PTR [eax+40], 980			; 000003d4H
; Line 98
	mov	eax, DWORD PTR _pvs$[ebp]
	mov	WORD PTR [eax+42], 981			; 000003d5H
; Line 99
	mov	eax, DWORD PTR _pvs$[ebp]
	mov	WORD PTR [eax+44], 986			; 000003daH
; Line 100
	mov	eax, DWORD PTR _pvs$[ebp]
	mov	WORD PTR [eax+46], 986			; 000003daH
; Line 101
	mov	eax, DWORD PTR _pvs$[ebp]
	mov	BYTE PTR [eax+145], 12			; 0000000cH
; Line 103
$L630:
	mov	eax, DWORD PTR _pvs$[ebp]
	mov	BYTE PTR [eax+131], 3
; Line 105
	mov	eax, DWORD PTR _pvs$[ebp]
	mov	DWORD PTR [eax+155], -1
; Line 106
$L625:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_InitVS	ENDP
_TEXT	ENDS
END
