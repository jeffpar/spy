	TITLE	dosdump.c
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
PUBLIC	_DOSDumpArena
PUBLIC	_apszDOSDump
PUBLIC	_apfnDOSDump
_DATA	SEGMENT
_szModule$S622 DB 'dosdump.c', 00H
	ORG $+2
$SG625	DB	'arena', 00H
	ORG $+2
_apszDOSDump DD	FLAT:$SG625
_apfnDOSDump DD	FLAT:_DOSDumpArena
_DATA	ENDS
PUBLIC	_DOSDumpCommand
EXTRN	_printf:NEAR
EXTRN	_istrtoken:NEAR
EXTRN	_nstrskip:NEAR
_DATA	SEGMENT
$SG632	DB	'DOS dump commands', 0aH, 09H, 'dos arena', 09H, 'dump me'
	DB	'mory arena', 0aH, 09H, 'dos dpb', 09H, 09H, 'dump drive param'
	DB	'eter blocks', 0aH, 00H
_DATA	ENDS
_TEXT	SEGMENT
; File dosdump.c
_pchCmd$ = 12
_i$ = -4
_DOSDumpCommand PROC NEAR
; Line 26
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	push	ebx
	push	esi
	push	edi
; Line 29
	mov	eax, DWORD PTR _pchCmd$[ebp]
	movsx	eax, BYTE PTR [eax]
	cmp	eax, 63					; 0000003fH
	jne	$L631
; Line 33
	push	OFFSET FLAT:$SG632
	call	_printf
	add	esp, 4
; Line 34
	mov	eax, 1
	jmp	$L629
; Line 36
$L631:
	mov	eax, DWORD PTR _pchCmd$[ebp]
	movsx	eax, BYTE PTR [eax]
	or	eax, eax
	je	$L633
; Line 37
	push	32					; 00000020H
	push	DWORD PTR _pchCmd$[ebp]
	call	_nstrskip
	add	esp, 8
	add	DWORD PTR _pchCmd$[ebp], eax
; Line 38
	push	1
	push	OFFSET FLAT:_apszDOSDump
	lea	eax, DWORD PTR _pchCmd$[ebp]
	push	eax
	call	_istrtoken
	add	esp, 12					; 0000000cH
	mov	DWORD PTR _i$[ebp], eax
; Line 39
	cmp	DWORD PTR _i$[ebp], 0
	jl	$L634
; Line 40
	mov	eax, DWORD PTR _i$[ebp]
	call	DWORD PTR _apfnDOSDump[eax*4]
; Line 41
	mov	eax, 1
	jmp	$L629
; Line 43
$L634:
; Line 44
$L633:
	sub	eax, eax
	jmp	$L629
; Line 45
$L629:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_DOSDumpCommand ENDP
_TEXT	ENDS
EXTRN	_nstrcpyn:NEAR
EXTRN	_flKeyEvent:DWORD
EXTRN	_pmbZero:DWORD
EXTRN	_nsanitize:NEAR
_DATA	SEGMENT
	ORG $+2
$SG644	DB	'DOS I/O segment  @%04X', 0aH, 00H
$SG645	DB	'I/O code segment @%04X:%04X (from %04X:%04X)', 0aH, 00H
	ORG $+2
$SG646	DB	'DOS data segment @%04X      (from %04X:%04X)', 0aH, 00H
	ORG $+2
$SG647	DB	'DOS code segment @%09X (from %04X:%04X)', 0aH, 00H
	ORG $+3
$SG648	DB	'DOS arena header @%04X      (from %04X:%04X)', 0aH, 00H
	ORG $+2
$SG653	DB	'MCB @%04X: Sig=%c Owner=%04X Size=%04X Name="%8s"', 0aH, 00H
	ORG $+1
$SG658	DB	'Unknown', 00H
$SG664	DB	'Buffers', 00H
$SG666	DB	'Device', 00H
	ORG $+1
$SG668	DB	'Files', 00H
	ORG $+2
$SG670	DB	'BDSs', 00H
	ORG $+3
$SG672	DB	'Lastdrive', 00H
	ORG $+2
$SG674	DB	'Stacks', 00H
	ORG $+1
$SG676	DB	'Install', 00H
$SG678	DB	'FCBs', 00H
	ORG $+3
$SG680	DB	'Drivemap', 00H
	ORG $+3
$SG681	DB	'SYSMCB @%04X Sig=%c Seg=%04X Size=%04X Name="%8s" (%s)', 0aH
	DB	00H
$SG684	DB	'Possible error in SYSMCBs (expected end @%04X, actual en'
	DB	'd @%04X)', 0aH, 00H
	ORG $+2
$SG687	DB	'Error in DOS arena @%09X', 0aH, 00H
_DATA	ENDS
_TEXT	SEGMENT
_pszDesc$ = -20
_szTmp$ = -12
_pmcb$ = -36
_pmcbSystem$ = -24
_pdos$ = -16
_pmsdos$ = -40
_pmcbPrev$ = -32
_pmcbNext$ = -28
_DOSDumpArena PROC NEAR
; Line 49
	push	ebp
	mov	ebp, esp
	sub	esp, 44					; 0000002cH
	push	ebx
	push	esi
	push	edi
; Line 57
	push	0
	push	112					; 00000070H
	push	OFFSET FLAT:$SG644
	call	_printf
	add	esp, 12					; 0000000cH
; Line 59
	mov	eax, DWORD PTR _pmbZero
	add	eax, 1792				; 00000700H
	mov	DWORD PTR _pdos$[ebp], eax
; Line 62
	push	286					; 0000011eH
	mov	eax, DWORD PTR _pdos$[ebp]
	sub	eax, DWORD PTR _pmbZero
	sar	eax, 4
	push	eax
	mov	eax, DWORD PTR _pdos$[ebp]
	movzx	eax, WORD PTR [eax+286]
	push	eax
	mov	eax, DWORD PTR _pdos$[ebp]
	movzx	eax, WORD PTR [eax+288]
	push	eax
	push	OFFSET FLAT:$SG645
	call	_printf
	add	esp, 20					; 00000014H
; Line 65
	push	3
	mov	eax, DWORD PTR _pdos$[ebp]
	sub	eax, DWORD PTR _pmbZero
	sar	eax, 4
	push	eax
	mov	eax, DWORD PTR _pdos$[ebp]
	movzx	eax, WORD PTR [eax+3]
	push	eax
	push	OFFSET FLAT:$SG646
	call	_printf
	add	esp, 16					; 00000010H
; Line 67
	mov	eax, DWORD PTR _pdos$[ebp]
	movzx	eax, WORD PTR [eax+3]
	shl	eax, 4
	add	eax, DWORD PTR _pmbZero
	mov	DWORD PTR _pmsdos$[ebp], eax
; Line 70
	push	3944					; 00000f68H
	mov	eax, DWORD PTR _pmsdos$[ebp]
	sub	eax, DWORD PTR _pmbZero
	sar	eax, 4
	push	eax
	mov	eax, DWORD PTR _pmsdos$[ebp]
	push	DWORD PTR [eax+3952]
	push	OFFSET FLAT:$SG647
	call	_printf
	add	esp, 16					; 00000010H
; Line 73
	push	36					; 00000024H
	mov	eax, DWORD PTR _pmsdos$[ebp]
	sub	eax, DWORD PTR _pmbZero
	sar	eax, 4
	push	eax
	mov	eax, DWORD PTR _pmsdos$[ebp]
	movzx	eax, WORD PTR [eax+36]
	push	eax
	push	OFFSET FLAT:$SG648
	call	_printf
	add	esp, 16					; 00000010H
; Line 75
	mov	eax, DWORD PTR _pmsdos$[ebp]
	movzx	eax, WORD PTR [eax+36]
	shl	eax, 4
	add	eax, DWORD PTR _pmbZero
	mov	DWORD PTR _pmcb$[ebp], eax
; Line 77
	and	DWORD PTR _flKeyEvent, -3		; fffffffdH
; Line 78
$L650:
	mov	eax, DWORD PTR _pmcb$[ebp]
	movzx	eax, BYTE PTR [eax]
	cmp	eax, 77					; 0000004dH
	je	$L652
	mov	eax, DWORD PTR _pmcb$[ebp]
	movzx	eax, BYTE PTR [eax]
	cmp	eax, 90					; 0000005aH
	jne	$L651
$L652:
; Line 81
	mov	eax, DWORD PTR _pmcb$[ebp]
	movzx	eax, WORD PTR [eax+3]
	shl	eax, 4
	add	eax, DWORD PTR _pmcb$[ebp]
	add	eax, 16					; 00000010H
	mov	DWORD PTR _pmcbNext$[ebp], eax
; Line 83
	push	8
	mov	eax, DWORD PTR _pmcb$[ebp]
	add	eax, 8
	push	eax
	lea	eax, DWORD PTR _szTmp$[ebp]
	push	eax
	call	_nstrcpyn
	add	esp, 12					; 0000000cH
; Line 84
	lea	eax, DWORD PTR _szTmp$[ebp]
	push	eax
	call	_nsanitize
	add	esp, 4
; Line 87
	lea	eax, DWORD PTR _szTmp$[ebp]
	push	eax
	mov	eax, DWORD PTR _pmcb$[ebp]
	movzx	eax, WORD PTR [eax+3]
	push	eax
	mov	eax, DWORD PTR _pmcb$[ebp]
	movzx	eax, WORD PTR [eax+1]
	push	eax
	mov	eax, DWORD PTR _pmcb$[ebp]
	movzx	eax, BYTE PTR [eax]
	push	eax
	mov	eax, DWORD PTR _pmcb$[ebp]
	sub	eax, DWORD PTR _pmbZero
	sar	eax, 4
	push	eax
	push	OFFSET FLAT:$SG653
	call	_printf
	add	esp, 24					; 00000018H
; Line 91
	mov	eax, DWORD PTR _pmcb$[ebp]
	movzx	eax, WORD PTR [eax+1]
	cmp	eax, 8
	jne	$L654
	mov	eax, DWORD PTR _pmcb$[ebp]
	movsx	eax, BYTE PTR [eax+8]
	cmp	eax, 83					; 00000053H
	jne	$L654
	mov	eax, DWORD PTR _pmcb$[ebp]
	movsx	eax, BYTE PTR [eax+9]
	cmp	eax, 68					; 00000044H
	jne	$L654
; Line 92
	mov	eax, DWORD PTR _pmcb$[ebp]
	add	eax, 16					; 00000010H
	mov	DWORD PTR _pmcbSystem$[ebp], eax
; Line 93
$L656:
	mov	eax, DWORD PTR _pmcbSystem$[ebp]
	movzx	eax, BYTE PTR [eax]
	or	eax, 32					; 00000020H
	movsx	eax, al
	cmp	eax, 97					; 00000061H
	jl	$L657
	mov	eax, DWORD PTR _pmcbSystem$[ebp]
	movzx	eax, BYTE PTR [eax]
	or	eax, 32					; 00000020H
	movsx	eax, al
	cmp	eax, 122				; 0000007aH
	jg	$L657
; Line 95
	mov	DWORD PTR _pszDesc$[ebp], OFFSET FLAT:$SG658
; Line 96
	mov	eax, DWORD PTR _pmcbSystem$[ebp]
	movzx	eax, BYTE PTR [eax]
	or	eax, 32					; 00000020H
	movsx	eax, al
	mov	DWORD PTR -44+[ebp], eax
	jmp	$L659
; Line 97
$L663:
; Line 99
	mov	DWORD PTR _pszDesc$[ebp], OFFSET FLAT:$SG664
; Line 100
	jmp	$L660
; Line 101
$L665:
; Line 102
	mov	DWORD PTR _pszDesc$[ebp], OFFSET FLAT:$SG666
; Line 103
	jmp	$L660
; Line 104
$L667:
; Line 105
	mov	DWORD PTR _pszDesc$[ebp], OFFSET FLAT:$SG668
; Line 106
	jmp	$L660
; Line 107
$L669:
; Line 108
	mov	DWORD PTR _pszDesc$[ebp], OFFSET FLAT:$SG670
; Line 109
	jmp	$L660
; Line 110
$L671:
; Line 111
	mov	DWORD PTR _pszDesc$[ebp], OFFSET FLAT:$SG672
; Line 112
	jmp	$L660
; Line 113
$L673:
; Line 114
	mov	DWORD PTR _pszDesc$[ebp], OFFSET FLAT:$SG674
; Line 115
	jmp	$L660
; Line 116
$L675:
; Line 117
	mov	DWORD PTR _pszDesc$[ebp], OFFSET FLAT:$SG676
; Line 118
	jmp	$L660
; Line 119
$L677:
; Line 120
	mov	DWORD PTR _pszDesc$[ebp], OFFSET FLAT:$SG678
; Line 121
	jmp	$L660
; Line 122
$L679:
; Line 123
	mov	DWORD PTR _pszDesc$[ebp], OFFSET FLAT:$SG680
; Line 124
	jmp	$L660
; Line 125
	jmp	$L660
$L659:
	sub	DWORD PTR -44+[ebp], 98			; 00000062H
	cmp	DWORD PTR -44+[ebp], 23			; 00000017H
	ja	$L660
	shl	DWORD PTR -44+[ebp], 2
	mov	eax, DWORD PTR -44+[ebp]
	jmp	DWORD PTR CS:$L688[eax]
$L688:
	DD	OFFSET FLAT:$L663
	DD	OFFSET FLAT:$L663
	DD	OFFSET FLAT:$L665
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L667
	DD	OFFSET FLAT:$L669
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L671
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L673
	DD	OFFSET FLAT:$L675
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L660
	DD	OFFSET FLAT:$L677
	DD	OFFSET FLAT:$L679
$L660:
; Line 127
	push	8
	mov	eax, DWORD PTR _pmcbSystem$[ebp]
	add	eax, 8
	push	eax
	lea	eax, DWORD PTR _szTmp$[ebp]
	push	eax
	call	_nstrcpyn
	add	esp, 12					; 0000000cH
; Line 128
	lea	eax, DWORD PTR _szTmp$[ebp]
	push	eax
	call	_nsanitize
	add	esp, 4
; Line 132
	push	DWORD PTR _pszDesc$[ebp]
	lea	eax, DWORD PTR _szTmp$[ebp]
	push	eax
	mov	eax, DWORD PTR _pmcbSystem$[ebp]
	movzx	eax, WORD PTR [eax+3]
	push	eax
	mov	eax, DWORD PTR _pmcbSystem$[ebp]
	movzx	eax, WORD PTR [eax+1]
	push	eax
	mov	eax, DWORD PTR _pmcbSystem$[ebp]
	movzx	eax, BYTE PTR [eax]
	push	eax
	mov	eax, DWORD PTR _pmcbSystem$[ebp]
	sub	eax, DWORD PTR _pmbZero
	sar	eax, 4
	push	eax
	push	OFFSET FLAT:$SG681
	call	_printf
	add	esp, 28					; 0000001cH
; Line 134
	mov	eax, DWORD PTR _pmcbSystem$[ebp]
	movzx	eax, WORD PTR [eax+3]
	inc	eax
	shl	eax, 4
	add	DWORD PTR _pmcbSystem$[ebp], eax
; Line 136
	mov	eax, DWORD PTR _pmcbNext$[ebp]
	cmp	DWORD PTR _pmcbSystem$[ebp], eax
	jb	$L682
; Line 137
	mov	eax, DWORD PTR _pmcbNext$[ebp]
	cmp	DWORD PTR _pmcbSystem$[ebp], eax
	je	$L683
; Line 139
	mov	eax, DWORD PTR _pmcbSystem$[ebp]
	sub	eax, DWORD PTR _pmbZero
	sar	eax, 4
	push	eax
	mov	eax, DWORD PTR _pmcbNext$[ebp]
	sub	eax, DWORD PTR _pmbZero
	sar	eax, 4
	push	eax
	push	OFFSET FLAT:$SG684
	call	_printf
	add	esp, 12					; 0000000cH
; Line 140
$L683:
	jmp	$L657
; Line 142
$L682:
	jmp	$L656
$L657:
; Line 146
$L654:
	mov	eax, DWORD PTR _pmcb$[ebp]
	movzx	eax, BYTE PTR [eax]
	cmp	eax, 90					; 0000005aH
	jne	$L685
; Line 147
	jmp	$L635
; Line 150
$L685:
	mov	eax, DWORD PTR _pmcb$[ebp]
	mov	DWORD PTR _pmcbPrev$[ebp], eax
; Line 151
	mov	eax, DWORD PTR _pmcbNext$[ebp]
	mov	DWORD PTR _pmcb$[ebp], eax
; Line 153
	test	BYTE PTR _flKeyEvent, 2
	je	$L686
; Line 154
	jmp	$L635
; Line 155
$L686:
	jmp	$L650
$L651:
; Line 156
	mov	eax, DWORD PTR _pmcb$[ebp]
	sub	eax, DWORD PTR _pmbZero
	sar	eax, 4
	movzx	eax, ax
	shl	eax, 16					; 00000010H
	mov	ecx, DWORD PTR _pmcb$[ebp]
	sub	ecx, DWORD PTR _pmbZero
	and	ecx, 15					; 0000000fH
	or	eax, ecx
	push	eax
	push	OFFSET FLAT:$SG687
	call	_printf
	add	esp, 8
; Line 157
$L635:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_DOSDumpArena ENDP
_TEXT	ENDS
END
