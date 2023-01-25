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
EXTRN	_printf:NEAR
EXTRN	_pLinFree:DWORD
EXTRN	_szAssert:BYTE
_TEXT	SEGMENT
; File memmgr.c
_dwPhys$ = -4
_InitMemMgr PROC NEAR
; Line 18
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	push	ebx
	push	esi
	push	edi
; Line 21
	push	272					; 00000110H
	push	0
	push	DWORD PTR _pLinFree
	call	_SetPages
	add	esp, 12					; 0000000cH
	mov	DWORD PTR _dwPhys$[ebp], eax
; Line 27
	mov	eax, DWORD PTR _pLinFree
	cmp	DWORD PTR _dwPhys$[ebp], eax
	je	$L625
	push	27					; 0000001bH
	push	OFFSET FLAT:_szModule$S622
	push	OFFSET FLAT:_szAssert
	call	_printf
	add	esp, 12					; 0000000cH
$L625:
; Line 39
	push	272					; 00000110H
	push	DWORD PTR _dwPhys$[ebp]
	mov	eax, DWORD PTR _pLinFree
	add	eax, 1114096				; 0010fff0H
	push	eax
	call	_SetPages
	add	esp, 12					; 0000000cH
; Line 43
	push	1114112					; 00110000H
	call	_MemAlloc
	add	esp, 4
	mov	DWORD PTR _pmbZero, eax
; Line 44
$L623:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_InitMemMgr ENDP
_TEXT	ENDS
EXTRN	_pPgTbl:DWORD
_TEXT	SEGMENT
_pLinear$ = 8
_dwPhysical$ = 12
_nPages$ = 16
_iPage$ = -8
_dwPhys$ = -12
_pte$ = -4
_SetPages PROC NEAR
; Line 48
	push	ebp
	mov	ebp, esp
	sub	esp, 20					; 00000014H
	push	ebx
	push	esi
	push	edi
; Line 53
	test	DWORD PTR _dwPhysical$[ebp], 4095	; 00000fffH
	je	$L633
	push	53					; 00000035H
	push	OFFSET FLAT:_szModule$S622
	push	OFFSET FLAT:_szAssert
	call	_printf
	add	esp, 12					; 0000000cH
$L633:
; Line 55
	mov	eax, DWORD PTR _pLinear$[ebp]
	shr	eax, 12					; 0000000cH
	mov	DWORD PTR _iPage$[ebp], eax
; Line 56
	mov	eax, DWORD PTR _iPage$[ebp]
	shl	eax, 2
	add	eax, DWORD PTR _pPgTbl
	mov	DWORD PTR _pte$[ebp], eax
; Line 57
	mov	eax, DWORD PTR _pte$[ebp]
	mov	eax, DWORD PTR [eax]
	and	eax, -4096				; fffff000H
	mov	DWORD PTR _dwPhys$[ebp], eax
; Line 59
$L635:
	mov	eax, DWORD PTR _nPages$[ebp]
	mov	DWORD PTR -16+[ebp], eax
	dec	DWORD PTR _nPages$[ebp]
	cmp	DWORD PTR -16+[ebp], 0
	je	$L636
	mov	eax, DWORD PTR _iPage$[ebp]
	mov	DWORD PTR -20+[ebp], eax
	inc	DWORD PTR _iPage$[ebp]
	cmp	DWORD PTR -20+[ebp], 1024		; 00000400H
	jge	$L636
; Line 60
	mov	eax, DWORD PTR _dwPhysical$[ebp]
	or	eax, 3
	mov	ecx, DWORD PTR _pte$[ebp]
	mov	DWORD PTR [ecx], eax
	add	DWORD PTR _pte$[ebp], 4
; Line 61
	add	DWORD PTR _dwPhysical$[ebp], 4096	; 00001000H
; Line 62
	jmp	$L635
$L636:
; Line 63
	mov	eax, DWORD PTR _dwPhys$[ebp]
	jmp	$L629
; Line 64
$L629:
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
; Line 68
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	push	ebx
	push	esi
	push	edi
; Line 71
	mov	eax, DWORD PTR _pFree
	mov	DWORD PTR _p$[ebp], eax
; Line 72
	mov	eax, DWORD PTR _cbSize$[ebp]
	add	DWORD PTR _pFree, eax
; Line 73
	mov	eax, DWORD PTR _cbSize$[ebp]
	add	DWORD PTR _pLinFree, eax
; Line 75
	mov	eax, DWORD PTR _p$[ebp]
	jmp	$L638
; Line 76
$L638:
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
; Line 80
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi
; Line 81
	mov	eax, 1
	jmp	$L642
; Line 82
$L642:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_MemFree ENDP
_TEXT	ENDS
END
