	TITLE	memmgr.c
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
PUBLIC	_pmbZero
_DATA	SEGMENT
_szModule$S622 DB 'memmgr.c', 00H
	ORG $+3
_pmbZero DD	00H
_DATA	ENDS
PUBLIC	_InitMemMgr
PUBLIC	_SetPages
PUBLIC	_MemAlloc
EXTRN	_pLinFree:DWORD
_TEXT	SEGMENT
; File memmgr.c
_InitMemMgr PROC NEAR
; Line 18
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi
; Line 22
	push	272					; 00000110H
	push	0
	push	DWORD PTR _pLinFree
	call	_SetPages
	add	esp, 12					; 0000000cH
; Line 26
	push	1114112					; 00110000H
	call	_MemAlloc
	add	esp, 4
	mov	DWORD PTR _pmbZero, eax
; Line 27
$L623:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_InitMemMgr ENDP
_TEXT	ENDS
EXTRN	_printf:NEAR
EXTRN	_pPgTbl:DWORD
EXTRN	_szAssert:BYTE
_TEXT	SEGMENT
_pLinear$ = 8
_dwPhysical$ = 12
_nPages$ = 16
_pte$ = -4
_SetPages PROC NEAR
; Line 31
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 34
	test	DWORD PTR _dwPhysical$[ebp], 4095	; 00000fffH
	je	$L629
	push	34					; 00000022H
	push	OFFSET FLAT:_szModule$S622
	push	OFFSET FLAT:_szAssert
	call	_printf
	add	esp, 12					; 0000000cH
$L629:
; Line 36
	mov	eax, DWORD PTR _pLinear$[ebp]
	and	eax, -4096				; fffff000H
	shr	eax, 10					; 0000000aH
	add	eax, DWORD PTR _pPgTbl
	mov	DWORD PTR _pte$[ebp], eax
; Line 38
$L631:
	mov	eax, DWORD PTR _nPages$[ebp]
	mov	DWORD PTR -8+[ebp], eax
	dec	DWORD PTR _nPages$[ebp]
	cmp	DWORD PTR -8+[ebp], 0
	je	$L632
; Line 39
	mov	eax, DWORD PTR _dwPhysical$[ebp]
	or	eax, 3
	mov	ecx, DWORD PTR _pte$[ebp]
	mov	DWORD PTR [ecx], eax
	add	DWORD PTR _pte$[ebp], 4
; Line 40
	add	DWORD PTR _dwPhysical$[ebp], 4096	; 00001000H
; Line 41
	jmp	$L631
$L632:
; Line 42
$L627:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_SetPages ENDP
_TEXT	ENDS
EXTRN	_pFree:DWORD
_TEXT	SEGMENT
_cbSize$ = 8
_p$ = -4
_MemAlloc PROC NEAR
; Line 46
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	push	ebx
	push	esi
	push	edi
; Line 49
	mov	eax, DWORD PTR _pFree
	mov	DWORD PTR _p$[ebp], eax
; Line 50
	mov	eax, DWORD PTR _cbSize$[ebp]
	add	DWORD PTR _pFree, eax
; Line 51
	mov	eax, DWORD PTR _cbSize$[ebp]
	add	DWORD PTR _pLinFree, eax
; Line 53
	mov	eax, DWORD PTR _p$[ebp]
	jmp	$L634
; Line 54
$L634:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_MemAlloc ENDP
_TEXT	ENDS
PUBLIC	_MemFree
_TEXT	SEGMENT
_MemFree PROC NEAR
; Line 58
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi
; Line 59
	mov	eax, 1
	jmp	$L638
; Line 60
$L638:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_MemFree ENDP
_TEXT	ENDS
END
