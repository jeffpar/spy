	TITLE	intparse.c
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
PUBLIC	_fnopLT
PUBLIC	_fnopLE
PUBLIC	_fnopGT
PUBLIC	_fnopGE
PUBLIC	_fnopEq
PUBLIC	_fnopNeq
PUBLIC	_fnopAnd
PUBLIC	_fnopXor
PUBLIC	_fnopOr
PUBLIC	_fnopLogAnd
PUBLIC	_fnopLogOr
PUBLIC	_fnCompress
PUBLIC	_avParse
PUBLIC	_aexop
PUBLIC	_stkOps
PUBLIC	_stkVals
PUBLIC	_szError
PUBLIC	_fnopDeref
PUBLIC	_fnopNot
PUBLIC	_fnopLogNot
PUBLIC	_fnopAdd
PUBLIC	_fnopSub
PUBLIC	_fnopMul
PUBLIC	_fnopDiv
PUBLIC	_fnopOpen
PUBLIC	_fnopClose
PUBLIC	_fnopShl
PUBLIC	_fnopShr
_DATA	SEGMENT
_szModule$S691 DB 'intparse.c', 00H
	ORG $+1
$SG717	DB	'compress', 00H
	ORG $+3
_avParse DD	FLAT:$SG717
	DD	00H
	DD	FLAT:_fnCompress
	ORG $+4
_aexop	DB	01H
	DB	05aH
	DD	FLAT:_fnopDeref
	DB	01H
	DB	05aH
	DD	FLAT:_fnopNot
	DB	01H
	DB	05aH
	DD	FLAT:_fnopLogNot
	DB	02H
	DB	046H
	DD	FLAT:_fnopAdd
	DB	02H
	DB	046H
	DD	FLAT:_fnopSub
	DB	02H
	DB	050H
	DD	FLAT:_fnopMul
	DB	02H
	DB	050H
	DD	FLAT:_fnopDiv
	DB	01H
	DB	0aH
	DD	FLAT:_fnopOpen
	DB	04H
	DB	063H
	DD	FLAT:_fnopClose
	DB	02H
	DB	03cH
	DD	FLAT:_fnopShl
	DB	02H
	DB	03cH
	DD	FLAT:_fnopShr
	DB	02H
	DB	032H
	DD	FLAT:_fnopLT
	DB	02H
	DB	032H
	DD	FLAT:_fnopLE
	DB	02H
	DB	032H
	DD	FLAT:_fnopGT
	DB	02H
	DB	032H
	DD	FLAT:_fnopGE
	DB	02H
	DB	028H
	DD	FLAT:_fnopEq
	DB	02H
	DB	028H
	DD	FLAT:_fnopNeq
	DB	02H
	DB	020H
	DD	FLAT:_fnopAnd
	DB	02H
	DB	01fH
	DD	FLAT:_fnopXor
	DB	02H
	DB	01eH
	DD	FLAT:_fnopOr
	DB	02H
	DB	015H
	DD	FLAT:_fnopLogAnd
	DB	02H
	DB	014H
	DD	FLAT:_fnopLogOr
	ORG $+4
_stkOps	DD	00H
	DD	014H
	ORG $+80
_stkVals DD	00H
	DD	014H
	ORG $+80
_szError DB	'Function not supported', 0aH, 00H
_DATA	ENDS
PUBLIC	_ParseInit
EXTRN	_nstrlen:NEAR
_TEXT	SEGMENT
; File intparse.c
_pv$ = -4
_ParseInit PROC NEAR
; Line 90
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	push	ebx
	push	esi
	push	edi
; Line 93
	mov	DWORD PTR _pv$[ebp], OFFSET FLAT:_avParse
	jmp	$L724
$L725:
	add	DWORD PTR _pv$[ebp], 12			; 0000000cH
$L724:
	mov	eax, OFFSET FLAT:_avParse
	add	eax, 12					; 0000000cH
	cmp	eax, DWORD PTR _pv$[ebp]
	jbe	$L726
; Line 94
	mov	eax, DWORD PTR _pv$[ebp]
	push	DWORD PTR [eax]
	call	_nstrlen
	add	esp, 4
	mov	ecx, DWORD PTR _pv$[ebp]
	mov	DWORD PTR [ecx+4], eax
; Line 95
	jmp	$L725
$L726:
; Line 96
$L722:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_ParseInit ENDP
_TEXT	ENDS
PUBLIC	_ParseCommand
EXTRN	_mystrcmpi:NEAR
_TEXT	SEGMENT
_pchCmd$ = 12
_pv$ = -4
_ParseCommand PROC NEAR
; Line 100
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	push	ebx
	push	esi
	push	edi
; Line 103
	cmp	DWORD PTR _avParse+4, 0
	jne	$L731
; Line 104
	call	_ParseInit
; Line 106
$L731:
	mov	DWORD PTR _pv$[ebp], OFFSET FLAT:_avParse
	jmp	$L732
$L733:
	add	DWORD PTR _pv$[ebp], 12			; 0000000cH
$L732:
	mov	eax, OFFSET FLAT:_avParse
	add	eax, 12					; 0000000cH
	cmp	eax, DWORD PTR _pv$[ebp]
	jbe	$L734
; Line 108
	mov	eax, DWORD PTR _pv$[ebp]
	push	DWORD PTR [eax]
	push	DWORD PTR _pchCmd$[ebp]
	call	_mystrcmpi
	add	esp, 8
	or	eax, eax
	jne	$L735
; Line 110
	mov	eax, DWORD PTR _pv$[ebp]
	call	DWORD PTR [eax+8]
; Line 116
	mov	eax, DWORD PTR _pv$[ebp]
	mov	eax, DWORD PTR [eax+4]
	jmp	$L729
; Line 118
$L735:
	jmp	$L733
$L734:
; Line 120
	sub	eax, eax
	jmp	$L729
; Line 121
$L729:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_ParseCommand ENDP
_TEXT	ENDS
PUBLIC	_ParseValue
PUBLIC	_ParseReg
PUBLIC	_ParsePush
PUBLIC	_ParsePop
PUBLIC	_ParsePushOp
PUBLIC	_ParseEvalOps
EXTRN	_printf:NEAR
EXTRN	_x86RegValue:NEAR
EXTRN	_sztodw:NEAR
EXTRN	_ardRegs:BYTE
EXTRN	_nstrskip:NEAR
_DATA	SEGMENT
$SG748	DB	'Expression operators', 0aH, 09H, '()', 09H, 'parentheses'
	DB	0aH, 09H, '||', 09H, 'logical OR', 0aH, 09H, '&&', 09H, 'logic'
	DB	'al AND', 0aH, 09H, '|', 09H, 'bitwise OR', 0aH, 09H, '^', 09H
	DB	'bitwise XOR', 0aH, 09H, '&', 09H, 'bitwise AND', 0aH, 09H, '='
	DB	'= !=', 09H, 'equal, not equal', 0aH, 09H, '< <=', 09H, 'less '
	DB	'than, less than or equal', 0aH, 09H, '> >=', 09H, 'greater th'
	DB	'an, greater than or equal', 0aH, 09H, '<< >>', 09H, 'shift le'
	DB	'ft, shift right', 0aH, 09H, '+ -', 09H, 'add, subtract', 0aH, 09H
	DB	'* /', 09H, 'multiply, divide', 0aH, 09H, '!', 09H, 'logical N'
	DB	'OT', 0aH, 09H, '~', 09H, 'bitwise NOT', 0aH, 09H, '@', 09H, 'd'
	DB	'ereference (assumes pointer to dword)', 0aH, 'Use ''?'' to ev'
	DB	'aluate 32-bit hex expressions (eg, ''? 200*9*50'')', 0aH, 00H
	ORG $+3
$SG798	DB	'Unknown symbol "%s"', 0aH, 00H
	ORG $+3
$SG807	DB	'Error in expression "%s"', 0aH, 00H
	ORG $+2
$SG810	DB	'Missing value', 0aH, 00H
	ORG $+1
$SG815	DB	'Missing operator', 0aH, 00H
_DATA	ENDS
_TEXT	SEGMENT
_pesf$ = 8
_psz$ = 12
_pdw$ = 20
_fMsg$ = 24
_i$ = -8
_dw$ = -4
_iLastToken$ = -16
_pszStart$ = -12
_ParseValue PROC NEAR
; Line 125
	push	ebp
	mov	ebp, esp
	sub	esp, 20					; 00000014H
	push	ebx
	push	esi
	push	edi
; Line 129
	mov	eax, DWORD PTR _psz$[ebp]
	mov	DWORD PTR _pszStart$[ebp], eax
; Line 131
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax]
	cmp	eax, 63					; 0000003fH
	jne	$L746
; Line 132
	cmp	DWORD PTR _fMsg$[ebp], 0
	je	$L747
; Line 150
	push	OFFSET FLAT:$SG748
	call	_printf
	add	esp, 4
; Line 151
$L747:
	sub	eax, eax
	jmp	$L741
; Line 160
$L746:
	mov	DWORD PTR _stkOps, 0
; Line 161
	mov	DWORD PTR _stkVals, 0
; Line 162
	mov	DWORD PTR _iLastToken$[ebp], 0
; Line 164
$L750:
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax]
	or	eax, eax
	je	$L751
; Line 169
	push	32					; 00000020H
	push	DWORD PTR _psz$[ebp]
	call	_nstrskip
	add	esp, 8
	add	DWORD PTR _psz$[ebp], eax
; Line 171
	push	DWORD PTR _psz$[ebp]
	call	_ParseReg
	add	esp, 4
	mov	DWORD PTR _i$[ebp], eax
	cmp	DWORD PTR _i$[ebp], 0
	je	$L752
; Line 172
	cmp	DWORD PTR _iLastToken$[ebp], 1
	jne	$L753
; Line 173
	jmp	$L751
; Line 174
$L753:
	mov	DWORD PTR _iLastToken$[ebp], 1
; Line 175
	dec	DWORD PTR _i$[ebp]
	push	DWORD PTR _i$[ebp]
	push	DWORD PTR _pesf$[ebp]
	call	_x86RegValue
	add	esp, 8
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L754
	sub	eax, eax
	jmp	$L741
	jmp	$L755
$L754:
$L755:
; Line 176
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	push	DWORD PTR _ardRegs[eax]
	call	_nstrlen
	add	esp, 4
	add	DWORD PTR _psz$[ebp], eax
; Line 178
	jmp	$L756
$L752:
	push	-1
	push	16					; 00000010H
	lea	eax, DWORD PTR _dw$[ebp]
	push	eax
	push	DWORD PTR _psz$[ebp]
	call	_sztodw
	add	esp, 16					; 00000010H
	mov	DWORD PTR _i$[ebp], eax
	cmp	DWORD PTR _i$[ebp], 0
	je	$L757
; Line 179
	cmp	DWORD PTR _iLastToken$[ebp], 1
	jne	$L758
; Line 180
	jmp	$L751
; Line 181
$L758:
	mov	DWORD PTR _iLastToken$[ebp], 1
; Line 182
	push	DWORD PTR _dw$[ebp]
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L759
	sub	eax, eax
	jmp	$L741
	jmp	$L760
$L759:
$L760:
; Line 183
	mov	eax, DWORD PTR _i$[ebp]
	add	DWORD PTR _psz$[ebp], eax
; Line 188
$L757:
$L756:
	push	32					; 00000020H
	push	DWORD PTR _psz$[ebp]
	call	_nstrskip
	add	esp, 8
	add	DWORD PTR _psz$[ebp], eax
; Line 190
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax]
	mov	DWORD PTR -20+[ebp], eax
	jmp	$L761
; Line 191
$L765:
; Line 192
	mov	DWORD PTR _i$[ebp], 0
; Line 193
	jmp	$L762
; Line 195
$L766:
; Line 196
	mov	DWORD PTR _i$[ebp], 1
; Line 197
	jmp	$L762
; Line 199
$L767:
; Line 200
	mov	DWORD PTR _i$[ebp], 3
; Line 201
	jmp	$L762
; Line 203
$L768:
; Line 204
	mov	DWORD PTR _i$[ebp], 4
; Line 205
	jmp	$L762
; Line 207
$L769:
; Line 208
	mov	DWORD PTR _i$[ebp], 5
; Line 209
	jmp	$L762
; Line 211
$L770:
; Line 212
	mov	DWORD PTR _i$[ebp], 6
; Line 213
	jmp	$L762
; Line 215
$L771:
; Line 216
	mov	DWORD PTR _i$[ebp], 7
; Line 217
	jmp	$L762
; Line 219
$L772:
; Line 220
	mov	DWORD PTR _i$[ebp], 8
; Line 221
	jmp	$L762
; Line 223
$L773:
; Line 224
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax+1]
	cmp	eax, 60					; 0000003cH
	jne	$L774
; Line 225
	inc	DWORD PTR _psz$[ebp]
; Line 226
	mov	DWORD PTR _i$[ebp], 9
; Line 228
	jmp	$L775
$L774:
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax+1]
	cmp	eax, 61					; 0000003dH
	jne	$L776
; Line 229
	inc	DWORD PTR _psz$[ebp]
; Line 230
	mov	DWORD PTR _i$[ebp], 12			; 0000000cH
; Line 232
	jmp	$L777
$L776:
; Line 233
	mov	DWORD PTR _i$[ebp], 11			; 0000000bH
$L777:
$L775:
; Line 234
	jmp	$L762
; Line 236
$L778:
; Line 237
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax+1]
	cmp	eax, 62					; 0000003eH
	jne	$L779
; Line 238
	inc	DWORD PTR _psz$[ebp]
; Line 239
	mov	DWORD PTR _i$[ebp], 10			; 0000000aH
; Line 241
	jmp	$L780
$L779:
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax+1]
	cmp	eax, 61					; 0000003dH
	jne	$L781
; Line 242
	inc	DWORD PTR _psz$[ebp]
; Line 243
	mov	DWORD PTR _i$[ebp], 14			; 0000000eH
; Line 245
	jmp	$L782
$L781:
; Line 246
	mov	DWORD PTR _i$[ebp], 13			; 0000000dH
$L782:
$L780:
; Line 247
	jmp	$L762
; Line 249
$L783:
; Line 250
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax+1]
	cmp	eax, 61					; 0000003dH
	jne	$L784
; Line 251
	inc	DWORD PTR _psz$[ebp]
; Line 252
	mov	DWORD PTR _i$[ebp], 15			; 0000000fH
; Line 254
$L784:
	jmp	$L762
; Line 256
$L785:
; Line 257
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax+1]
	cmp	eax, 61					; 0000003dH
	jne	$L786
; Line 258
	inc	DWORD PTR _psz$[ebp]
; Line 259
	mov	DWORD PTR _i$[ebp], 16			; 00000010H
; Line 261
	jmp	$L787
$L786:
; Line 262
	mov	DWORD PTR _i$[ebp], 2
$L787:
; Line 263
	jmp	$L762
; Line 265
$L788:
; Line 266
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax+1]
	cmp	eax, 38					; 00000026H
	jne	$L789
; Line 267
	inc	DWORD PTR _psz$[ebp]
; Line 268
	mov	DWORD PTR _i$[ebp], 20			; 00000014H
; Line 270
	jmp	$L790
$L789:
; Line 271
	mov	DWORD PTR _i$[ebp], 17			; 00000011H
$L790:
; Line 272
	jmp	$L762
; Line 274
$L791:
; Line 275
	mov	DWORD PTR _i$[ebp], 18			; 00000012H
; Line 276
	jmp	$L762
; Line 278
$L792:
; Line 279
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax+1]
	cmp	eax, 124				; 0000007cH
	jne	$L793
; Line 280
	inc	DWORD PTR _psz$[ebp]
; Line 281
	mov	DWORD PTR _i$[ebp], 21			; 00000015H
; Line 283
	jmp	$L794
$L793:
; Line 284
	mov	DWORD PTR _i$[ebp], 19			; 00000013H
$L794:
; Line 285
	jmp	$L762
; Line 287
$L795:
; Line 288
	cmp	DWORD PTR _iLastToken$[ebp], 2
	jne	$L796
; Line 289
	cmp	DWORD PTR _fMsg$[ebp], 0
	je	$L797
; Line 290
	push	DWORD PTR _psz$[ebp]
	push	OFFSET FLAT:$SG798
	call	_printf
	add	esp, 8
; Line 291
$L797:
	sub	eax, eax
	jmp	$L741
; Line 293
$L796:
$L799:
; Line 294
	jmp	$Process$800
; Line 295
	jmp	$L762
$L761:
	cmp	DWORD PTR -20+[ebp], 47			; 0000002fH
	jg	$L1059
	je	$L770
	cmp	DWORD PTR -20+[ebp], 41			; 00000029H
	jg	$L1060
	je	$L772
	cmp	DWORD PTR -20+[ebp], 0
	je	$Process$800
	cmp	DWORD PTR -20+[ebp], 33			; 00000021H
	je	$L785
	cmp	DWORD PTR -20+[ebp], 38			; 00000026H
	je	$L788
	cmp	DWORD PTR -20+[ebp], 40			; 00000028H
	je	$L771
	jmp	$L795
$L1060:
	cmp	DWORD PTR -20+[ebp], 42			; 0000002aH
	je	$L769
	cmp	DWORD PTR -20+[ebp], 43			; 0000002bH
	je	$L767
	cmp	DWORD PTR -20+[ebp], 45			; 0000002dH
	je	$L768
	jmp	$L795
$L1059:
	cmp	DWORD PTR -20+[ebp], 64			; 00000040H
	jg	$L1061
	je	$L765
	cmp	DWORD PTR -20+[ebp], 60			; 0000003cH
	je	$L773
	cmp	DWORD PTR -20+[ebp], 61			; 0000003dH
	je	$L783
	cmp	DWORD PTR -20+[ebp], 62			; 0000003eH
	je	$L778
	jmp	$L795
$L1061:
	cmp	DWORD PTR -20+[ebp], 94			; 0000005eH
	je	$L791
	cmp	DWORD PTR -20+[ebp], 124		; 0000007cH
	je	$L792
	cmp	DWORD PTR -20+[ebp], 126		; 0000007eH
	je	$L766
	jmp	$L795
$L762:
; Line 297
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	movzx	eax, BYTE PTR _aexop[eax]
	test	al, 1
	je	$L801
	cmp	DWORD PTR _iLastToken$[ebp], 1
	jne	$L801
; Line 298
	jmp	$Process$800
; Line 299
$L801:
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	movzx	eax, BYTE PTR _aexop[eax]
	test	al, 6
	je	$L802
	cmp	DWORD PTR _iLastToken$[ebp], 1
	je	$L802
; Line 300
	jmp	$Process$800
; Line 302
$L802:
	push	DWORD PTR _i$[ebp]
	call	_ParsePushOp
	add	esp, 4
; Line 304
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	movzx	eax, BYTE PTR _aexop[eax]
	test	al, 4
	je	$L803
; Line 305
	mov	DWORD PTR _iLastToken$[ebp], 1
; Line 306
$L803:
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	movzx	eax, BYTE PTR _aexop[eax]
	test	al, 3
	je	$L804
; Line 307
	mov	DWORD PTR _iLastToken$[ebp], 2
; Line 309
$L804:
	inc	DWORD PTR _psz$[ebp]
; Line 310
	jmp	$L750
$L751:
; Line 312
$Process$800:
; Line 314
	push	0
	call	_ParseEvalOps
	add	esp, 4
	or	eax, eax
	jne	$L805
; Line 315
	cmp	DWORD PTR _fMsg$[ebp], 0
	je	$L806
; Line 316
	push	DWORD PTR _pszStart$[ebp]
	push	OFFSET FLAT:$SG807
	call	_printf
	add	esp, 8
; Line 317
$L806:
	sub	eax, eax
	jmp	$L741
; Line 320
$L805:
	cmp	DWORD PTR _stkVals, 0
	jne	$L808
; Line 321
	cmp	DWORD PTR _fMsg$[ebp], 0
	je	$L809
; Line 322
	push	OFFSET FLAT:$SG810
	call	_printf
	add	esp, 4
; Line 323
$L809:
	sub	eax, eax
	jmp	$L741
; Line 326
$L808:
	push	DWORD PTR _pdw$[ebp]
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L811
	sub	eax, eax
	jmp	$L741
	jmp	$L812
$L811:
$L812:
; Line 328
	cmp	DWORD PTR _stkVals, 0
	je	$L813
; Line 329
	cmp	DWORD PTR _fMsg$[ebp], 0
	je	$L814
; Line 330
	push	OFFSET FLAT:$SG815
	call	_printf
	add	esp, 4
; Line 331
$L814:
	sub	eax, eax
	jmp	$L741
; Line 334
$L813:
	mov	eax, DWORD PTR _psz$[ebp]
	sub	eax, DWORD PTR _pszStart$[ebp]
	jmp	$L741
; Line 335
$L741:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_ParseValue ENDP
_psz$ = 8
_i$ = -4
_ParseReg PROC NEAR
; Line 339
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	push	ebx
	push	esi
	push	edi
; Line 342
	mov	DWORD PTR _i$[ebp], 0
	jmp	$L819
$L820:
	inc	DWORD PTR _i$[ebp]
$L819:
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	cmp	DWORD PTR _ardRegs[eax], 0
	je	$L821
; Line 343
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	push	DWORD PTR _ardRegs[eax]
	push	DWORD PTR _psz$[ebp]
	call	_mystrcmpi
	add	esp, 8
	or	eax, eax
	jne	$L822
; Line 344
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	push	DWORD PTR _ardRegs[eax]
	call	_nstrlen
	add	esp, 4
	mov	ecx, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [ecx+eax]
	or	eax, 32					; 00000020H
	movsx	eax, al
	cmp	eax, 97					; 00000061H
	jl	$L824
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	push	DWORD PTR _ardRegs[eax]
	call	_nstrlen
	add	esp, 4
	mov	ecx, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [ecx+eax]
	or	eax, 32					; 00000020H
	movsx	eax, al
	cmp	eax, 122				; 0000007aH
	jle	$L823
$L824:
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	push	DWORD PTR _ardRegs[eax]
	call	_nstrlen
	add	esp, 4
	mov	ecx, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [ecx+eax]
	cmp	eax, 48					; 00000030H
	jl	$L825
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	push	DWORD PTR _ardRegs[eax]
	call	_nstrlen
	add	esp, 4
	mov	ecx, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [ecx+eax]
	cmp	eax, 57					; 00000039H
	jle	$L823
$L825:
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	push	DWORD PTR _ardRegs[eax]
	call	_nstrlen
	add	esp, 4
	mov	ecx, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [ecx+eax]
	cmp	eax, 95					; 0000005fH
	je	$L823
	mov	eax, DWORD PTR _i$[ebp]
	imul	eax, 6
	push	DWORD PTR _ardRegs[eax]
	call	_nstrlen
	add	esp, 4
	mov	ecx, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [ecx+eax]
	cmp	eax, 36					; 00000024H
	je	$L823
; Line 345
	mov	eax, DWORD PTR _i$[ebp]
	inc	eax
	jmp	$L817
; Line 346
$L823:
$L822:
	jmp	$L820
$L821:
; Line 347
	sub	eax, eax
	jmp	$L817
; Line 348
$L817:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_ParseReg ENDP
_TEXT	ENDS
PUBLIC	_ParseRange
_TEXT	SEGMENT
_pesf$ = 8
_psz$ = 12
_pdwLower$ = 16
_pdwUpper$ = 20
_cch$ = -4
_ParseRange PROC NEAR
; Line 352
	push	ebp
	mov	ebp, esp
	sub	esp, 12					; 0000000cH
	push	ebx
	push	esi
	push	edi
; Line 355
	push	0
	push	DWORD PTR _pdwLower$[ebp]
	push	0
	push	DWORD PTR _psz$[ebp]
	push	DWORD PTR _pesf$[ebp]
	call	_ParseValue
	add	esp, 20					; 00000014H
	mov	DWORD PTR _cch$[ebp], eax
	cmp	DWORD PTR _cch$[ebp], 0
	je	$L832
; Line 356
	mov	eax, DWORD PTR _cch$[ebp]
	add	DWORD PTR _psz$[ebp], eax
; Line 357
	mov	eax, DWORD PTR _pdwLower$[ebp]
	mov	eax, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pdwUpper$[ebp]
	mov	DWORD PTR [ecx], eax
; Line 358
	mov	eax, DWORD PTR _psz$[ebp]
	mov	DWORD PTR -8+[ebp], eax
	inc	DWORD PTR _psz$[ebp]
	mov	eax, DWORD PTR -8+[ebp]
	movsx	eax, BYTE PTR [eax]
	cmp	eax, 46					; 0000002eH
	jne	$L833
	mov	eax, DWORD PTR _psz$[ebp]
	mov	DWORD PTR -12+[ebp], eax
	inc	DWORD PTR _psz$[ebp]
	mov	eax, DWORD PTR -12+[ebp]
	movsx	eax, BYTE PTR [eax]
	cmp	eax, 46					; 0000002eH
	jne	$L833
; Line 359
	push	0
	push	DWORD PTR _pdwUpper$[ebp]
	push	0
	push	DWORD PTR _psz$[ebp]
	push	DWORD PTR _pesf$[ebp]
	call	_ParseValue
	add	esp, 20					; 00000014H
	add	eax, 2
	add	DWORD PTR _cch$[ebp], eax
; Line 361
$L833:
; Line 362
$L832:
	mov	eax, DWORD PTR _cch$[ebp]
	jmp	$L830
; Line 363
$L830:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_ParseRange ENDP
_TEXT	ENDS
PUBLIC	_ParseAddr
EXTRN	_sel_Flat:WORD
_TEXT	SEGMENT
_pesf$ = 8
_psz$ = 12
_pselCode$ = 16
_poffCode$ = 20
_pselData$ = 24
_poffData$ = 28
_i$ = -8
_cch$ = -4
_flSelType$ = -12
_ParseAddr PROC NEAR
; Line 368
	push	ebp
	mov	ebp, esp
	sub	esp, 16					; 00000010H
	push	ebx
	push	esi
	push	edi
; Line 369
	mov	DWORD PTR _cch$[ebp], 0
; Line 372
	mov	DWORD PTR _flSelType$[ebp], 0
; Line 373
	mov	eax, DWORD PTR _pesf$[ebp]
	test	BYTE PTR [eax+110], 2
	je	$L844
; Line 374
	mov	DWORD PTR _flSelType$[ebp], 65536	; 00010000H
; Line 376
$L844:
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax]
	cmp	eax, 38					; 00000026H
	jne	$L845
; Line 377
	inc	DWORD PTR _psz$[ebp]
; Line 378
	inc	DWORD PTR _cch$[ebp]
; Line 379
	mov	DWORD PTR _flSelType$[ebp], 65536	; 00010000H
; Line 381
	jmp	$L846
$L845:
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax]
	cmp	eax, 35					; 00000023H
	jne	$L847
; Line 382
	inc	DWORD PTR _psz$[ebp]
; Line 383
	inc	DWORD PTR _cch$[ebp]
; Line 384
	mov	DWORD PTR _flSelType$[ebp], 0
; Line 386
	jmp	$L848
$L847:
	mov	eax, DWORD PTR _psz$[ebp]
	movsx	eax, BYTE PTR [eax]
	cmp	eax, 37					; 00000025H
	jne	$L849
; Line 387
	inc	DWORD PTR _psz$[ebp]
; Line 388
	inc	DWORD PTR _cch$[ebp]
; Line 389
	mov	DWORD PTR _flSelType$[ebp], 131072	; 00020000H
; Line 392
$L849:
$L848:
$L846:
	push	0
	push	DWORD PTR _poffCode$[ebp]
	push	DWORD PTR _pselCode$[ebp]
	push	DWORD PTR _psz$[ebp]
	push	DWORD PTR _pesf$[ebp]
	call	_ParseValue
	add	esp, 20					; 00000014H
	mov	DWORD PTR _i$[ebp], eax
	cmp	DWORD PTR _i$[ebp], 0
	je	$L850
; Line 393
	mov	eax, DWORD PTR _i$[ebp]
	add	DWORD PTR _cch$[ebp], eax
; Line 394
	mov	eax, DWORD PTR _i$[ebp]
	add	DWORD PTR _psz$[ebp], eax
; Line 395
	mov	eax, DWORD PTR _psz$[ebp]
	mov	DWORD PTR -16+[ebp], eax
	inc	DWORD PTR _psz$[ebp]
	mov	eax, DWORD PTR -16+[ebp]
	movsx	eax, BYTE PTR [eax]
	cmp	eax, 58					; 0000003aH
	jne	$L851
; Line 396
	inc	DWORD PTR _cch$[ebp]
; Line 397
	mov	eax, DWORD PTR _poffCode$[ebp]
	movzx	eax, WORD PTR [eax]
	or	eax, DWORD PTR _flSelType$[ebp]
	mov	ecx, DWORD PTR _pselCode$[ebp]
	mov	DWORD PTR [ecx], eax
; Line 398
	push	0
	push	DWORD PTR _poffCode$[ebp]
	push	DWORD PTR _pselCode$[ebp]
	push	DWORD PTR _psz$[ebp]
	push	DWORD PTR _pesf$[ebp]
	call	_ParseValue
	add	esp, 20					; 00000014H
	add	DWORD PTR _cch$[ebp], eax
; Line 400
	jmp	$L852
$L851:
	cmp	DWORD PTR _flSelType$[ebp], 131072	; 00020000H
	jne	$L853
; Line 406
	movzx	eax, WORD PTR _sel_Flat
	or	eax, DWORD PTR _flSelType$[ebp]
	mov	ecx, DWORD PTR _pselCode$[ebp]
	mov	DWORD PTR [ecx], eax
; Line 408
$L853:
$L852:
	cmp	DWORD PTR _poffData$[ebp], 0
	je	$L854
; Line 409
	mov	eax, DWORD PTR _poffCode$[ebp]
	mov	eax, DWORD PTR [eax]
	mov	ecx, DWORD PTR _poffData$[ebp]
	mov	DWORD PTR [ecx], eax
; Line 410
$L854:
	cmp	DWORD PTR _pselData$[ebp], 0
	je	$L855
; Line 411
	mov	eax, DWORD PTR _pselCode$[ebp]
	mov	eax, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pselData$[ebp]
	mov	DWORD PTR [ecx], eax
; Line 412
$L855:
; Line 413
$L850:
	mov	eax, DWORD PTR _cch$[ebp]
	jmp	$L840
; Line 414
$L840:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_ParseAddr ENDP
_pstk$ = 8
_th$ = 12
_ParsePush PROC NEAR
; Line 418
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi
; Line 419
	mov	eax, DWORD PTR _pstk$[ebp]
	mov	ecx, DWORD PTR _pstk$[ebp]
	mov	ecx, DWORD PTR [ecx+4]
	cmp	DWORD PTR [eax], ecx
	jne	$L859
; Line 420
	sub	eax, eax
	jmp	$L858
; Line 421
$L859:
	mov	eax, DWORD PTR _th$[ebp]
	mov	ecx, DWORD PTR _pstk$[ebp]
	mov	ecx, DWORD PTR [ecx]
	mov	edx, DWORD PTR _pstk$[ebp]
	mov	DWORD PTR [edx+ecx*4+8], eax
	mov	eax, DWORD PTR _pstk$[ebp]
	inc	DWORD PTR [eax]
; Line 422
	mov	eax, 1
	jmp	$L858
; Line 423
$L858:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_ParsePush ENDP
_pstk$ = 8
_pth$ = 12
_ParsePop PROC NEAR
; Line 427
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi
; Line 428
	mov	eax, DWORD PTR _pstk$[ebp]
	cmp	DWORD PTR [eax], 0
	jne	$L863
; Line 429
	sub	eax, eax
	jmp	$L862
; Line 430
$L863:
	mov	eax, DWORD PTR _pstk$[ebp]
	dec	DWORD PTR [eax]
	mov	eax, DWORD PTR _pstk$[ebp]
	mov	eax, DWORD PTR [eax]
	mov	ecx, DWORD PTR _pstk$[ebp]
	mov	eax, DWORD PTR [ecx+eax*4+8]
	mov	ecx, DWORD PTR _pth$[ebp]
	mov	DWORD PTR [ecx], eax
; Line 431
	mov	eax, 1
	jmp	$L862
; Line 432
$L862:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_ParsePop ENDP
_TEXT	ENDS
EXTRN	_szAssert:BYTE
_TEXT	SEGMENT
_iOp$ = 8
_ParsePushOp PROC NEAR
; Line 436
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi
; Line 437
	mov	eax, DWORD PTR _stkOps
	cmp	DWORD PTR _stkOps+4, eax
	jne	$L866
; Line 438
	sub	eax, eax
	jmp	$L865
; Line 440
$L866:
	cmp	DWORD PTR _iOp$[ebp], 22		; 00000016H
	jb	$L867
	push	440					; 000001b8H
	push	OFFSET FLAT:_szModule$S691
	push	OFFSET FLAT:_szAssert
	call	_printf
	add	esp, 12					; 0000000cH
$L867:
; Line 441
	mov	eax, DWORD PTR _iOp$[ebp]
	imul	eax, 6
	movzx	eax, BYTE PTR _aexop[eax]
	test	al, 1
	jne	$L868
; Line 442
	mov	eax, DWORD PTR _iOp$[ebp]
	imul	eax, 6
	mov	al, BYTE PTR _aexop[eax+1]
	push	eax
	call	_ParseEvalOps
	add	esp, 4
	or	eax, eax
	jne	$L869
; Line 443
	sub	eax, eax
	jmp	$L865
; Line 445
$L869:
$L868:
	mov	eax, DWORD PTR _iOp$[ebp]
	mov	ecx, DWORD PTR _stkOps
	mov	DWORD PTR _stkOps[ecx*4+8], eax
	inc	DWORD PTR _stkOps
; Line 446
	mov	eax, 1
	jmp	$L865
; Line 447
$L865:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_ParsePushOp ENDP
_bLevel$ = 8
_iOp$ = -4
_ParseEvalOps PROC NEAR
; Line 451
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	push	ebx
	push	esi
	push	edi
; Line 454
$L874:
	cmp	DWORD PTR _stkOps, 0
	je	$L875
; Line 455
	mov	eax, DWORD PTR _stkOps
	mov	eax, DWORD PTR _stkOps[eax*4+4]
	mov	DWORD PTR _iOp$[ebp], eax
; Line 456
	cmp	DWORD PTR _iOp$[ebp], 22		; 00000016H
	jb	$L876
	push	456					; 000001c8H
	push	OFFSET FLAT:_szModule$S691
	push	OFFSET FLAT:_szAssert
	call	_printf
	add	esp, 12					; 0000000cH
$L876:
; Line 457
	mov	eax, DWORD PTR _iOp$[ebp]
	imul	eax, 6
	movzx	eax, BYTE PTR _aexop[eax+1]
	movzx	ecx, BYTE PTR _bLevel$[ebp]
	cmp	eax, ecx
	jl	$L877
; Line 458
	lea	eax, DWORD PTR _iOp$[ebp]
	push	eax
	push	OFFSET FLAT:_stkOps
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L878
	sub	eax, eax
	jmp	$L871
	jmp	$L879
$L878:
$L879:
; Line 459
	cmp	DWORD PTR _iOp$[ebp], 22		; 00000016H
	jb	$L880
	push	459					; 000001cbH
	push	OFFSET FLAT:_szModule$S691
	push	OFFSET FLAT:_szAssert
	call	_printf
	add	esp, 12					; 0000000cH
$L880:
; Line 460
	mov	eax, DWORD PTR _iOp$[ebp]
	imul	eax, 6
	call	DWORD PTR _aexop[eax+2]
	or	eax, eax
	jne	$L881
; Line 461
	sub	eax, eax
	jmp	$L871
; Line 462
$L881:
; Line 463
	jmp	$L882
$L877:
; Line 464
	jmp	$L875
$L882:
; Line 465
	jmp	$L874
$L875:
; Line 466
	mov	eax, 1
	jmp	$L871
; Line 467
$L871:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_ParseEvalOps ENDP
_dw1$ = -4
_fnopDeref PROC NEAR
; Line 471
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	push	ebx
	push	esi
	push	edi
; Line 474
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L885
	sub	eax, eax
	jmp	$L883
	jmp	$L886
$L885:
$L886:
; Line 475
	mov	eax, DWORD PTR _dw1$[ebp]
	push	DWORD PTR [eax]
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L887
	sub	eax, eax
	jmp	$L883
	jmp	$L888
$L887:
$L888:
; Line 476
	mov	eax, 1
	jmp	$L883
; Line 477
$L883:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopDeref ENDP
_dw1$ = -4
_fnopNot PROC NEAR
; Line 481
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	push	ebx
	push	esi
	push	edi
; Line 484
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L891
	sub	eax, eax
	jmp	$L889
	jmp	$L892
$L891:
$L892:
; Line 485
	mov	eax, DWORD PTR _dw1$[ebp]
	not	eax
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L893
	sub	eax, eax
	jmp	$L889
	jmp	$L894
$L893:
$L894:
; Line 486
	mov	eax, 1
	jmp	$L889
; Line 487
$L889:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopNot ENDP
_dw1$ = -4
_fnopLogNot PROC NEAR
; Line 491
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	push	ebx
	push	esi
	push	edi
; Line 494
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L897
	sub	eax, eax
	jmp	$L895
	jmp	$L898
$L897:
$L898:
; Line 495
	cmp	DWORD PTR _dw1$[ebp], 1
	sbb	eax, eax
	neg	eax
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L899
	sub	eax, eax
	jmp	$L895
	jmp	$L900
$L899:
$L900:
; Line 496
	mov	eax, 1
	jmp	$L895
; Line 497
$L895:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopLogNot ENDP
_dw1$ = -8
_dw2$ = -4
_fnopAdd PROC NEAR
; Line 501
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 504
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L904
	sub	eax, eax
	jmp	$L901
	jmp	$L905
$L904:
$L905:
; Line 505
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L906
	sub	eax, eax
	jmp	$L901
	jmp	$L907
$L906:
$L907:
; Line 506
	mov	eax, DWORD PTR _dw2$[ebp]
	add	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L908
	sub	eax, eax
	jmp	$L901
	jmp	$L909
$L908:
$L909:
; Line 507
	mov	eax, 1
	jmp	$L901
; Line 508
$L901:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopAdd ENDP
_dw1$ = -8
_dw2$ = -4
_fnopSub PROC NEAR
; Line 512
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 515
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L913
	sub	eax, eax
	jmp	$L910
	jmp	$L914
$L913:
$L914:
; Line 516
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L915
	sub	eax, eax
	jmp	$L910
	jmp	$L916
$L915:
$L916:
; Line 517
	mov	eax, DWORD PTR _dw2$[ebp]
	sub	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L917
	sub	eax, eax
	jmp	$L910
	jmp	$L918
$L917:
$L918:
; Line 518
	mov	eax, 1
	jmp	$L910
; Line 519
$L910:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopSub ENDP
_dw1$ = -8
_dw2$ = -4
_fnopMul PROC NEAR
; Line 523
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 526
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L922
	sub	eax, eax
	jmp	$L919
	jmp	$L923
$L922:
$L923:
; Line 527
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L924
	sub	eax, eax
	jmp	$L919
	jmp	$L925
$L924:
$L925:
; Line 528
	mov	eax, DWORD PTR _dw2$[ebp]
	imul	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L926
	sub	eax, eax
	jmp	$L919
	jmp	$L927
$L926:
$L927:
; Line 529
	mov	eax, 1
	jmp	$L919
; Line 530
$L919:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopMul ENDP
_TEXT	ENDS
_DATA	SEGMENT
	ORG $+2
$SG934	DB	'Attempted division by zero', 0aH, 00H
_DATA	ENDS
_TEXT	SEGMENT
_dw1$ = -8
_dw2$ = -4
_fnopDiv PROC NEAR
; Line 534
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 537
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L931
	sub	eax, eax
	jmp	$L928
	jmp	$L932
$L931:
$L932:
; Line 538
	cmp	DWORD PTR _dw1$[ebp], 0
	jne	$L933
; Line 539
	push	OFFSET FLAT:$SG934
	call	_printf
	add	esp, 4
; Line 540
	sub	eax, eax
	jmp	$L928
; Line 542
$L933:
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L935
	sub	eax, eax
	jmp	$L928
	jmp	$L936
$L935:
$L936:
; Line 543
	mov	eax, DWORD PTR _dw2$[ebp]
	sub	edx, edx
	div	DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L937
	sub	eax, eax
	jmp	$L928
	jmp	$L938
$L937:
$L938:
; Line 544
	mov	eax, 1
	jmp	$L928
; Line 545
$L928:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopDiv ENDP
_fnopOpen PROC NEAR
; Line 549
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi
; Line 550
	mov	eax, 1
	jmp	$L939
; Line 551
$L939:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopOpen ENDP
_fnopClose PROC NEAR
; Line 555
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi
; Line 556
	push	10					; 0000000aH
	call	_ParseEvalOps
	add	esp, 4
	jmp	$L940
; Line 557
$L940:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopClose ENDP
_dw1$ = -8
_dw2$ = -4
_fnopShl PROC NEAR
; Line 561
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 564
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L944
	sub	eax, eax
	jmp	$L941
	jmp	$L945
$L944:
$L945:
; Line 565
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L946
	sub	eax, eax
	jmp	$L941
	jmp	$L947
$L946:
$L947:
; Line 566
	mov	eax, DWORD PTR _dw2$[ebp]
	mov	cl, BYTE PTR _dw1$[ebp]
	shl	eax, cl
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L948
	sub	eax, eax
	jmp	$L941
	jmp	$L949
$L948:
$L949:
; Line 567
	mov	eax, 1
	jmp	$L941
; Line 568
$L941:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopShl ENDP
_dw1$ = -8
_dw2$ = -4
_fnopShr PROC NEAR
; Line 572
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 575
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L953
	sub	eax, eax
	jmp	$L950
	jmp	$L954
$L953:
$L954:
; Line 576
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L955
	sub	eax, eax
	jmp	$L950
	jmp	$L956
$L955:
$L956:
; Line 577
	mov	eax, DWORD PTR _dw2$[ebp]
	mov	cl, BYTE PTR _dw1$[ebp]
	shr	eax, cl
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L957
	sub	eax, eax
	jmp	$L950
	jmp	$L958
$L957:
$L958:
; Line 578
	mov	eax, 1
	jmp	$L950
; Line 579
$L950:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopShr ENDP
_dw1$ = -8
_dw2$ = -4
_fnopLT	PROC NEAR
; Line 583
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 586
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L962
	sub	eax, eax
	jmp	$L959
	jmp	$L963
$L962:
$L963:
; Line 587
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L964
	sub	eax, eax
	jmp	$L959
	jmp	$L965
$L964:
$L965:
; Line 588
	mov	eax, DWORD PTR _dw1$[ebp]
	cmp	DWORD PTR _dw2$[ebp], eax
	mov	eax, 0
	setb	al
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L966
	sub	eax, eax
	jmp	$L959
	jmp	$L967
$L966:
$L967:
; Line 589
	mov	eax, 1
	jmp	$L959
; Line 590
$L959:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopLT	ENDP
_dw1$ = -8
_dw2$ = -4
_fnopLE	PROC NEAR
; Line 594
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 597
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L971
	sub	eax, eax
	jmp	$L968
	jmp	$L972
$L971:
$L972:
; Line 598
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L973
	sub	eax, eax
	jmp	$L968
	jmp	$L974
$L973:
$L974:
; Line 599
	mov	eax, DWORD PTR _dw1$[ebp]
	cmp	DWORD PTR _dw2$[ebp], eax
	mov	eax, 0
	setbe	al
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L975
	sub	eax, eax
	jmp	$L968
	jmp	$L976
$L975:
$L976:
; Line 600
	mov	eax, 1
	jmp	$L968
; Line 601
$L968:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopLE	ENDP
_dw1$ = -8
_dw2$ = -4
_fnopGT	PROC NEAR
; Line 605
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 608
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L980
	sub	eax, eax
	jmp	$L977
	jmp	$L981
$L980:
$L981:
; Line 609
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L982
	sub	eax, eax
	jmp	$L977
	jmp	$L983
$L982:
$L983:
; Line 610
	mov	eax, DWORD PTR _dw1$[ebp]
	cmp	DWORD PTR _dw2$[ebp], eax
	mov	eax, 0
	seta	al
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L984
	sub	eax, eax
	jmp	$L977
	jmp	$L985
$L984:
$L985:
; Line 611
	mov	eax, 1
	jmp	$L977
; Line 612
$L977:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopGT	ENDP
_dw1$ = -8
_dw2$ = -4
_fnopGE	PROC NEAR
; Line 616
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 619
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L989
	sub	eax, eax
	jmp	$L986
	jmp	$L990
$L989:
$L990:
; Line 620
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L991
	sub	eax, eax
	jmp	$L986
	jmp	$L992
$L991:
$L992:
; Line 621
	mov	eax, DWORD PTR _dw1$[ebp]
	cmp	DWORD PTR _dw2$[ebp], eax
	mov	eax, 0
	setae	al
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L993
	sub	eax, eax
	jmp	$L986
	jmp	$L994
$L993:
$L994:
; Line 622
	mov	eax, 1
	jmp	$L986
; Line 623
$L986:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopGE	ENDP
_dw1$ = -8
_dw2$ = -4
_fnopEq	PROC NEAR
; Line 627
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 630
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L998
	sub	eax, eax
	jmp	$L995
	jmp	$L999
$L998:
$L999:
; Line 631
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1000
	sub	eax, eax
	jmp	$L995
	jmp	$L1001
$L1000:
$L1001:
; Line 632
	mov	eax, DWORD PTR _dw2$[ebp]
	sub	eax, DWORD PTR _dw1$[ebp]
	cmp	eax, 1
	sbb	eax, eax
	neg	eax
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L1002
	sub	eax, eax
	jmp	$L995
	jmp	$L1003
$L1002:
$L1003:
; Line 633
	mov	eax, 1
	jmp	$L995
; Line 634
$L995:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopEq	ENDP
_dw1$ = -8
_dw2$ = -4
_fnopNeq PROC NEAR
; Line 638
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 641
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1007
	sub	eax, eax
	jmp	$L1004
	jmp	$L1008
$L1007:
$L1008:
; Line 642
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1009
	sub	eax, eax
	jmp	$L1004
	jmp	$L1010
$L1009:
$L1010:
; Line 643
	mov	eax, DWORD PTR _dw2$[ebp]
	sub	eax, DWORD PTR _dw1$[ebp]
	cmp	eax, 1
	sbb	eax, eax
	inc	eax
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L1011
	sub	eax, eax
	jmp	$L1004
	jmp	$L1012
$L1011:
$L1012:
; Line 644
	mov	eax, 1
	jmp	$L1004
; Line 645
$L1004:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopNeq ENDP
_dw1$ = -8
_dw2$ = -4
_fnopAnd PROC NEAR
; Line 649
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 652
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1016
	sub	eax, eax
	jmp	$L1013
	jmp	$L1017
$L1016:
$L1017:
; Line 653
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1018
	sub	eax, eax
	jmp	$L1013
	jmp	$L1019
$L1018:
$L1019:
; Line 654
	mov	eax, DWORD PTR _dw2$[ebp]
	and	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L1020
	sub	eax, eax
	jmp	$L1013
	jmp	$L1021
$L1020:
$L1021:
; Line 655
	mov	eax, 1
	jmp	$L1013
; Line 656
$L1013:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopAnd ENDP
_dw2$ = -4
_dw1$ = -8
_fnopXor PROC NEAR
; Line 660
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 663
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1025
	sub	eax, eax
	jmp	$L1022
	jmp	$L1026
$L1025:
$L1026:
; Line 664
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1027
	sub	eax, eax
	jmp	$L1022
	jmp	$L1028
$L1027:
$L1028:
; Line 665
	mov	eax, DWORD PTR _dw2$[ebp]
	xor	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L1029
	sub	eax, eax
	jmp	$L1022
	jmp	$L1030
$L1029:
$L1030:
; Line 666
	mov	eax, 1
	jmp	$L1022
; Line 667
$L1022:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopXor ENDP
_dw1$ = -8
_dw2$ = -4
_fnopOr	PROC NEAR
; Line 671
	push	ebp
	mov	ebp, esp
	sub	esp, 8
	push	ebx
	push	esi
	push	edi
; Line 674
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1034
	sub	eax, eax
	jmp	$L1031
	jmp	$L1035
$L1034:
$L1035:
; Line 675
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1036
	sub	eax, eax
	jmp	$L1031
	jmp	$L1037
$L1036:
$L1037:
; Line 676
	mov	eax, DWORD PTR _dw2$[ebp]
	or	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L1038
	sub	eax, eax
	jmp	$L1031
	jmp	$L1039
$L1038:
$L1039:
; Line 677
	mov	eax, 1
	jmp	$L1031
; Line 678
$L1031:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopOr	ENDP
_dw1$ = -8
_dw2$ = -4
_fnopLogAnd PROC NEAR
; Line 682
	push	ebp
	mov	ebp, esp
	sub	esp, 12					; 0000000cH
	push	ebx
	push	esi
	push	edi
; Line 685
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1043
	sub	eax, eax
	jmp	$L1040
	jmp	$L1044
$L1043:
$L1044:
; Line 686
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1045
	sub	eax, eax
	jmp	$L1040
	jmp	$L1046
$L1045:
$L1046:
; Line 687
	cmp	DWORD PTR _dw2$[ebp], 0
	je	$L1062
	cmp	DWORD PTR _dw1$[ebp], 0
	je	$L1062
	mov	DWORD PTR -12+[ebp], 1
	jmp	$L1063
$L1062:
	mov	DWORD PTR -12+[ebp], 0
$L1063:
	push	DWORD PTR -12+[ebp]
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L1047
	sub	eax, eax
	jmp	$L1040
	jmp	$L1048
$L1047:
$L1048:
; Line 688
	mov	eax, 1
	jmp	$L1040
; Line 689
$L1040:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopLogAnd ENDP
_dw1$ = -8
_dw2$ = -4
_fnopLogOr PROC NEAR
; Line 693
	push	ebp
	mov	ebp, esp
	sub	esp, 12					; 0000000cH
	push	ebx
	push	esi
	push	edi
; Line 696
	lea	eax, DWORD PTR _dw1$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1052
	sub	eax, eax
	jmp	$L1049
	jmp	$L1053
$L1052:
$L1053:
; Line 697
	lea	eax, DWORD PTR _dw2$[ebp]
	push	eax
	push	OFFSET FLAT:_stkVals
	call	_ParsePop
	add	esp, 8
	or	eax, eax
	jne	$L1054
	sub	eax, eax
	jmp	$L1049
	jmp	$L1055
$L1054:
$L1055:
; Line 698
	cmp	DWORD PTR _dw2$[ebp], 0
	jne	$L1066
	cmp	DWORD PTR _dw1$[ebp], 0
	je	$L1064
$L1066:
	mov	DWORD PTR -12+[ebp], 1
	jmp	$L1065
$L1064:
	mov	DWORD PTR -12+[ebp], 0
$L1065:
	push	DWORD PTR -12+[ebp]
	push	OFFSET FLAT:_stkVals
	call	_ParsePush
	add	esp, 8
	or	eax, eax
	jne	$L1056
	sub	eax, eax
	jmp	$L1049
	jmp	$L1057
$L1056:
$L1057:
; Line 699
	mov	eax, 1
	jmp	$L1049
; Line 700
$L1049:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnopLogOr ENDP
_fnCompress PROC NEAR
; Line 704
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi
; Line 705
	push	OFFSET FLAT:_szError
	call	_printf
	add	esp, 4
; Line 706
$L1058:
	pop	edi
	pop	esi
	pop	ebx
	leave
	ret	0
_fnCompress ENDP
_TEXT	ENDS
END
