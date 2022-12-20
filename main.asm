	TITLE	main.c
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
PUBLIC	_szTitle
_DATA	SEGMENT
_szModule$S644 DB 'main.c', 00H
	ORG $+1
_szTitle DB	'386 Monitor v0.1 by Jeff Parsons, June 5, 1992', 0aH, 0aH
	DB	00H
_DATA	ENDS
PUBLIC	_Main
EXTRN	_printf:NEAR
EXTRN	_cbBSSData:DWORD
EXTRN	_InitMemMgr:NEAR
EXTRN	_x86Debug:NEAR
EXTRN	_szAssert:BYTE
EXTRN	_InitVideoMgr:NEAR
_TEXT	SEGMENT
; File main.c
_esf$ = 8
_Main	PROC NEAR
; Line 19
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi
; Line 22
	call	_InitMemMgr
; Line 26
	call	_InitVideoMgr
; Line 31
	cmp	DWORD PTR _cbBSSData, 0
	je	$L649
	push	31					; 0000001fH
	push	OFFSET FLAT:_szModule$S644
	push	OFFSET FLAT:_szAssert
	call	_printf
	add	esp, 12					; 0000000cH
$L649:
; Line 33
	push	OFFSET FLAT:_szTitle
	call	_printf
	add	esp, 4
; Line 35
	push	1
	lea	eax, DWORD PTR _esf$[ebp]
	push	eax
	call	_x86Debug
	add	esp, 8
; Line 36
$L648:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_Main	ENDP
_TEXT	ENDS
END
