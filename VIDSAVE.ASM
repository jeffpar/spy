	page	,132
	title	VIDSAVE.ASM

;
;   VIDSAVE.ASM
;   Video save/restore code
;
;   by Jeff Parsons 25-Jul-1992
;


	include all.inc


RESETSEQ_CLKMODE equ 1
RESETSEQ_MISCOUT equ 1


OPEN_CODE


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   _SaveVS
;
;   ENTRY
;	pvs -> video state
;	fEnable == TRUE to enable video afterward
;
;   EXIT
;	video state saved
;
;   USES
;	Only EAX, ECX and EDX
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	_SaveVS
_SaveVS proc	near

	push	ebx
	push	esi
	push	edi
	mov	ebx,[esp+10h]	; EBX == pvs

	test	[ebx].vs_flVState,VSTATE_DISABLE
	jnz	save_exit	; do absolutely nothing

	add	[ebx].vs_cLocks,1
	jnc	save_exit

	mov	[ebx].vs_wPortCRTIndx,PORT_MONOCRTINDX
	mov	[ebx].vs_wPortCRTData,PORT_MONOCRTDATA
	mov	[ebx].vs_wPortStatus1,PORT_MONOSTATUS1
	mov	[ebx].vs_wPortFeature,PORT_MONOFEATURE

	mov	dx,PORT_GDCPOS1 ; return MISCOUT port on read (VGA only!)
	in	al,dx		;
	test	al,MISCOUT_COLRPORTS
	jz	short not_color ;

	mov	[ebx].vs_wPortCRTIndx,PORT_COLRCRTINDX
	mov	[ebx].vs_wPortCRTData,PORT_COLRCRTDATA
	mov	[ebx].vs_wPortStatus1,PORT_COLRSTATUS1
	mov	[ebx].vs_wPortFeature,PORT_COLRFEATURE

not_color:
	mov	dx,[ebx].vs_wPortStatus1
	in	al,dx		; reset ATC state
	IODelay			;

	sub	ecx,ecx		; ECX == ATC reg index
	rol	edx,16		;
	mov	dx,PORT_ATC0	;
	in	al,dx		; save the index setting
	mov	[ebx].vs_regATCIndx,al
	IODelay			;

atc_loop:
	mov	al,cl		;
	out	dx,al		; select register to read (screen off!)
	IODelay			;
	inc	edx		;
	in	al,dx		; read it
	dec	edx		; save the register setting
	mov	[ebx].vs_aregATCData[ecx],al
	IODelay			;
	rol	edx,16		;
	in	al,dx		; reset ATC state again
	rol	edx,16		;
	inc	ecx		;
	cmp	cl,MAX_ATCREGS	;
	jb	atc_loop	;

	mov	dx,PORT_GDCPOS1 ; VGA's R/O port for PORT_MISCOUT
	in	al,dx		;
	mov	[ebx].vs_regMiscOut,al

	sub	ecx,ecx		; ECX == SEQ reg index
	mov	dx,PORT_SEQINDX ;
	in	al,dx		; save the index setting
	mov	[ebx].vs_regSEQIndx,al

seq_loop:
	mov	al,cl		;
	out	dx,al		; select register to read
	IODelay			;
	inc	edx		;
	in	al,dx		; read it
	dec	edx		; save the register setting
	mov	[ebx].vs_aregSEQData[ecx],al
	inc	ecx		;
	cmp	cl,MAX_SEQREGS	;
	jb	seq_loop	;

	mov	al,[ebx].vs_regSEQIndx
	out	dx,al		; restore the SEQ index setting
	IODelay			;

	mov	dx,PORT_VGADACMASK
	in	al,dx		; save DAC mask register
	mov	[ebx].vs_regDACMask,al
	IODelay			;

	sub	eax,eax		;
	mov	dx,PORT_VGADACREAD
	in	al,dx		; save DAC I/O state
	shl	eax,8		;
	mov	[ebx].vs_stateDAC,eax
	IODelay			;

	mov	dx,PORT_VGADACWRITE
	in	al,dx		; save DAC index
	mov	[ebx].vs_regDACIndx,al
	IODelay			;

	sub	ecx,ecx		; ECX == DAC counter
	mov	dx,PORT_VGADACREAD
	mov	al,0		;
	out	dx,al		; select the starting index (0)
	mov	dx,PORT_VGADACDATA
	IODelay			;

dac_loop:
	in	al,dx		; read RED
	mov	[ebx].vs_adacDACData[ecx].dac_bRed,al
	IODelay			;
	in	al,dx		; read GREEN
	mov	[ebx].vs_adacDACData[ecx].dac_bGreen,al
	IODelay			;
	in	al,dx		; read BLUE
	mov	[ebx].vs_adacDACData[ecx].dac_bBlue,al
	IODelay			;
	add	ecx,3		;
	cmp	ecx,MAX_DACREGS*3
	jb	dac_loop	;

	mov	dx,PORT_VGADACREAD
	mov	eax,[ebx].vs_stateDAC
	and	ah,03h		;
	cmp	ah,03h		; was the last write to the read reg?
	je	short dac_readme; yes
	mov	dx,PORT_VGADACWRITE
dac_readme:
	mov	al,[ebx].vs_regDACIndx
	out	dx,al		;
	IODelay			;

	sub	ecx,ecx		; ECX == GDC reg index
	mov	dx,PORT_GDCINDX ;
	in	al,dx		; save the index setting
	mov	[ebx].vs_regGDCIndx,al

gdc_loop:
	mov	al,cl		;
	out	dx,al		; select register to read
	IODelay			;
	inc	dx		;
	in	al,dx		; read it
	dec	dx		; save the register setting
	mov	[ebx].vs_aregGDCData[ecx],al
	inc	ecx		;
	cmp	cl,MAX_GDCREGS	;
	jb	gdc_loop	;

	mov	al,[ebx].vs_regGDCIndx
	out	dx,al		; restore the index setting

	sub	ecx,ecx		; ECX == CRT reg index
	mov	dx,[ebx].vs_wPortCRTIndx
	in	al,dx		; save the index setting
	mov	[ebx].vs_regCRTIndx,al

crt_loop:
	mov	al,cl		;
	out	dx,al		; select register to read
	IODelay			;
	inc	dx		;
	in	al,dx		; read it
	dec	dx		; save the register setting
	mov	[ebx].vs_aregCRTData[ecx],al
	inc	ecx		;
	cmp	cl,MAX_CRTREGS	;
	jb	crt_loop	;

	mov	al,[ebx].vs_regCRTIndx
	out	dx,al		; restore the CRT index setting

	mov	dx,PORT_GDCPOS2 ; VGA's R/O port for PORT_xxxxFEATURE
	in	al,dx		; save the feature register
	mov	[ebx].vs_regFeature,al

	sub	al,al		; save all planes
	call	SavePlanes
	call	RestorePlaneMode

	cmp	dword ptr [esp+14h],0
	je	short save_exit ; fEnable is FALSE, leave screen off

	mov	dx,PORT_ATC0	;
	mov	al,[ebx].vs_regATCIndx
	out	dx,al		; restore ATC index setting (screen on!)

save_exit:
	pop	edi
	pop	esi
	pop	ebx
	ret

_SaveVS endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   _RestoreVS
;
;   ENTRY
;	pvs -> video state
;
;   EXIT
;	video state restored
;
;   USES
;	Only EAX, ECX and EDX
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	_RestoreVS
_RestoreVS proc	   near

	push	ebx
	push	esi
	push	edi
	mov	ebx,[esp+10h]	; EBX == pvs

	test	[ebx].vs_flVState,VSTATE_DISABLE
	jnz	restore_exit	; do absolutely nothing

	sub	[ebx].vs_cLocks,1
	jnc	restore_exit

	mov	dx,[ebx].vs_wPortStatus1
	in	al,dx		; reset ATC state
	IODelay			;

	sub	ecx,ecx		; ECX == ATC reg index
	mov	dx,PORT_ATC0	;

atc_loop:
	IFDEF	WAITVERT_ATCMODE
	cmp	cl,REG_ATCMODECTRL
	jne	short atc_cont	;
	push	STATUS1_VERTRTRC;
	push	ebx		;
	call	_WaitRetrace	;
	add	esp,8		;
atc_cont:
	ENDIF  ;WAITVERT_ATCMODE

	mov	al,cl		;
	out	dx,al		; select register to write (screen off!)
	IODelay			;
	mov	al,[ebx].vs_aregATCData[ecx]
	out	dx,al		; write it
	inc	ecx		;
	cmp	cl,MAX_ATCREGS	;
	jb	atc_loop	;

	sub	ecx,ecx		; ECX == SEQ reg index
	mov	dx,PORT_SEQINDX ;

seq_loop:
	IFDEF	RESETSEQ_CLKMODE
	sub	eax,eax		;
	cmp	cl,REG_SEQCLKMODE
	IFDEF	RESETSEQ_MEMMODE
	je	short seq_reset ;
	cmp	cl,REG_SEQMEMMODE
	ENDIF
	jne	short seq_cont	;
seq_reset:
	pushf			;
	cli			;
	mov	ax,REG_SEQRESET + SEQRESET_ASYNC SHL 8
	out	dx,ax		;
	IODelay			;
	IFDEF	RESETSEQ_MISCOUT
	mov	dx,PORT_MISCOUT ;
	mov	al,[ebx].vs_regMiscOut
	out	dx,al		;
	mov	dx,PORT_SEQINDX ;
	IODelay			;
	ENDIF
seq_cont:
	push	eax		;
	ENDIF  ;RESETSEQ_CLKMODE

	mov	al,cl		;
	mov	ah,[ebx].vs_aregSEQData[ecx]
	out	dx,ax		; select register to write
	IODelay			;

	IFDEF	RESETSEQ_CLKMODE
	pop	eax		;
	or	ah,ah		; did we reset the sequencer?
	jz	short seq_next	; no
	mov	al,REG_SEQRESET ;
	mov	ah,[ebx].vs_aregSEQData[REG_SEQRESET]
	out	dx,ax		; re-enable sequencer
	popf			;
seq_next:
	ENDIF  ;RESETSEQ_CLKMODE

	inc	ecx		;
	cmp	cl,MAX_SEQREGS	;
	jb	seq_loop	;

	mov	al,[ebx].vs_regSEQIndx
	out	dx,al		; restore the SEQ index setting
	IODelay			;

	mov	dx,PORT_VGADACMASK
	mov	al,[ebx].vs_regDACMask
	out	dx,al		; restore DAC mask register
	IODelay			;

	sub	ecx,ecx		; ECX == DAC counter
	mov	dx,PORT_VGADACWRITE
	mov	al,0		;
	out	dx,al		; select the starting index (0)
	mov	dx,PORT_VGADACDATA
	IODelay			;

dac_loop:
	mov	al,[ebx].vs_adacDACData[ecx].dac_bRed
	out	dx,al		; write RED
	IODelay			;
	mov	al,[ebx].vs_adacDACData[ecx].dac_bGreen
	out	dx,al		; write GREEN
	IODelay			;
	mov	al,[ebx].vs_adacDACData[ecx].dac_bBlue
	out	dx,al		; write BLUE
	IODelay			;
	add	ecx,3		;
	cmp	ecx,MAX_DACREGS*3
	jb	dac_loop	;

	mov	dx,PORT_VGADACREAD
	mov	eax,[ebx].vs_stateDAC
	and	ah,03h		;
	cmp	ah,03h		; was the last write to the read reg?
	je	short dac_readme; yes
	mov	dx,PORT_VGADACWRITE
dac_readme:
	mov	al,[ebx].vs_regDACIndx
	out	dx,al		;
	IODelay			;

	sub	ecx,ecx		; ECX == GDC reg index
	mov	dx,PORT_GDCINDX ;

gdc_loop:
	IFDEF	RESETSEQ_GDCMISC
	cmp	cl,REG_GDCMISC	;
	jne	short gdc_cont	;
	push	STATUS1_VERTRTRC;
	push	ebx		;
	call	_WaitRetrace	;
	add	esp,8		;
	pushf			;
	cli			;
	mov	dx,PORT_SEQINDX ;
	mov	ax,REG_SEQRESET + SEQRESET_ASYNC SHL 8
	out	dx,ax		;
	IODelay			;
	mov	dx,PORT_GDCINDX ;
gdc_cont:
	ENDIF  ;RESETSEQ_GDCMISC

	mov	al,cl		;
	mov	ah,[ebx].vs_aregGDCData[ecx]
	out	dx,ax		; select register to write
	IODelay			;

	IFDEF	RESETSEQ_GDCMISC
	cmp	cl,REG_GDCMISC	; did we reset the sequencer?
	je	short gdc_next	; no
	mov	dx,PORT_SEQINDX ;
	mov	al,REG_SEQRESET ;
	mov	ah,[ebx].vs_aregSEQData[REG_SEQRESET]
	out	dx,ax		; re-enable sequencer
	mov	dx,PORT_GDCINDX ;
	popf			;
gdc_next:
	ENDIF  ;RESETSEQ_GDCMISC

	inc	ecx		;
	cmp	cl,MAX_GDCREGS	;
	jb	gdc_loop	;

	mov	al,[ebx].vs_regGDCIndx
	out	dx,al		; restore the GDC index setting

	sub	ecx,ecx		; ECX == CRT reg index
	mov	dx,[ebx].vs_wPortCRTIndx
	mov	al,REG_CRTVERTENDRTRC
	mov	ah,[ebx].vs_aregCRTData[REG_CRTVERTENDRTRC]
	and	ah,not CRTVERTEND_WRITEPROT
	out	dx,ax		; insure write-protect is off
	IODelay

crt_loop:
	mov	al,cl		;
	mov	ah,[ebx].vs_aregCRTData[ecx]
	out	dx,ax		; select register to write
	IODelay			;
	inc	ecx		;
	cmp	cl,MAX_CRTREGS	;
	jb	crt_loop	;

	mov	al,[ebx].vs_regCRTIndx
	out	dx,al		; restore the CRT index setting

	mov	dx,[ebx].vs_wPortFeature
	mov	al,[ebx].vs_regFeature
	out	dx,al		; restore the feature register

	sub	al,al		; restore all planes
	call	RestorePlanes
	call	RestorePlaneMode

	mov	dx,PORT_ATC0	;
	mov	al,[ebx].vs_regATCIndx
	out	dx,al		; restore ATC index setting (screen on!)

restore_exit:
	pop	edi
	pop	esi
	pop	ebx
	ret

_RestoreVS endp


	IFDEF	WAITVERT_ATCMODE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   _WaitRetrace
;
;   ENTRY
;	pvs -> video state
;	bRetrace == retrace bit mask
;
;   EXIT
;	retrace interval active (provided no interrupts)
;
;   USES
;	Only EAX
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	_WaitRetrace
_WaitRetrace proc near

	push	ecx
	push	edx

	mov	ah,[esp+10h]	; get bRetrace
	mov	ecx,[esp+0Ch]	; get pvs
	mov	dx,[ecx].vs_wPortStatus1

	mov	ecx,MAX_RTRCLOOP
wait_low:
	in	al,dx
	test	al,ah
	loopnz	wait_low

	mov	ecx,MAX_RTRCLOOP
wait_high:
	in	al,dx
;;;	sti
	test	al,ah
;;;	cli
	loopz	wait_high

wait_high_done:
	pop	edx
	pop	ecx
	ret

_WaitRetrace endp

	ENDIF  ;WAITVERT_ATCMODE


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   SetPlaneMode
;
;   ENTRY
;	CL  == plane # (ordinal form)
;
;   EXIT
;	Memory mode set
;
;   USES
;	Only AX, DX
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	SetPlaneMode
SetPlaneMode proc near

	mov	dx,PORT_SEQINDX
	mov	ah,1
	shl	ah,cl
	mov	al,REG_SEQMAPMASK
	out	dx,ax
	IODelay
	mov	ax,REG_SEQMEMMODE or (SEQMEM_EXTENDED or SEQMEM_ODDEVENSAME) shl 8
	out	dx,ax
	IODelay
	mov	dx,PORT_GDCINDX
	mov	ax,REG_GDCSR or (00h) shl 8
	out	dx,ax
	IODelay
	mov	ax,REG_GDCESR or (00h) shl 8
	out	dx,ax
	IODelay
	mov	ax,REG_GDCCLRCMP or (00h) shl 8
	out	dx,ax
	IODelay
	mov	ax,REG_GDCROTATE or (00h) shl 8
	out	dx,ax
	IODelay
	mov	ah,cl
	mov	al,REG_GDCREADMAP
	out	dx,ax
	IODelay
	mov	ax,REG_GDCMODE or (00h) shl 8
	out	dx,ax
	IODelay
	mov	ax,REG_GDCMISC or (GDCMISC_64K_A0000) shl 8
	out	dx,ax
	IODelay
	mov	ax,REG_GDCDONTCARE or (00h) shl 8
	out	dx,ax
	IODelay
	mov	ax,REG_GDCBITMASK or (0FFh) shl 8
	out	dx,ax
	IODelay
	ret

SetPlaneMode endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   SavePlanes
;
;   ENTRY
;	EBX == pvs
;
;   EXIT
;	Plane memory saved
;
;   USES
;	EAX, ECX, EDX
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	SavePlanes
SavePlanes proc near

	push	esi
	push	edi

	cmp	al,2
	je	short save_plane2
;
;   Time to save the planes;  try to keep the logic simple:
;
;   If the GDCMISC_NOCHARGEN bit is set in REG_GDCMISC, then assume it
;   is a graphics mode and save all the planes, up to maximums we have
;   allocated;	otherwise, save a max of 32K of PLANE0 and 8K of PLANE2
;   (ie, assume conventional text mode)
;
	test	[ebx].vs_aregGDCData[REG_GDCMISC],GDCMISC_NOCHARGEN
	jz	short save_text

	sub	ecx,ecx		; ECX == plane #
save_plane:
	call	SetPlaneMode	; enable plane at VIDMEM_GRPHADDR (A0000h)
	mov	esi,[_pVRAM]
	mov	edi,[ebx].vs_pbPlane[ecx*4]
	push	ecx
	mov	ecx,[ebx].vs_cbPlane[ecx*4]
	shr	ecx,2		; BUGBUG -- assumes size is whole dwords
	rep	movsd		; save plane
	pop	ecx
	inc	ecx
	cmp	cl,MAX_PLANES
	jb	save_plane
	jmp	short save_finish

save_text:
	call	GetVRAMAddress	; EAX == VRAM address, EDX == size
	mov	esi,eax
	mov	edi,[ebx].vs_pbPlane[PLANE0*4]
	mov	ecx,[ebx].vs_cbPlane[PLANE0*4]
	cmp	ecx,edx
	jbe	short save_text2
	mov	ecx,edx
save_text2:
	shr	ecx,2		; BUGBUG -- assumes size is whole dwords
	test	[ebx].vs_flVState,VSTATE_SWAP
	jz	short save_text4
save_text3:
	mov	eax,[esi]
	xchg	[edi],eax
	add	edi,4
	mov	[esi],eax
	add	esi,4
	loop	save_text3
	jmp	short save_plane2
save_text4:
	rep	movsd		; save plane 0
save_plane2:
	mov	cl,PLANE2
	call	SetPlaneMode
	mov	esi,[_pVRAM]
	mov	edi,[ebx].vs_pbPlane[PLANE2*4]
	mov	ecx,[ebx].vs_cbPlane[PLANE2*4]
	shr	ecx,2		; BUGBUG -- assumes size is whole dwords
	test	[ebx].vs_flVState,VSTATE_SWAP
	jz	short save_text8
save_text7:
	mov	eax,[esi]
	xchg	[edi],eax
	add	edi,4
	mov	[esi],eax
	add	esi,4
	loop	save_text7
	jmp	short save_finish
save_text8:
	rep	movsd		; save plane 2
save_finish:

	pop	edi
	pop	esi
	ret

SavePlanes endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   RestorePlanes
;
;   ENTRY
;	EBX == pvs
;
;   EXIT
;	Plane memory restored
;
;   USES
;	EAX, ECX, EDX
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	RestorePlanes
RestorePlanes proc near

	push	esi
	push	edi

	cmp	al,2
	je	short restore_plane2
;
;   Time to restore the planes;	 try to keep the logic simple:
;
;   If the GDCMISC_NOCHARGEN bit is set in REG_GDCMISC, then assume it
;   is a graphics mode and restore all the planes, up to maximums we have
;   allocated;	otherwise, restore a max of 32K of PLANE0 and 8K of PLANE2
;   (ie, assume conventional text mode)
;
	test	[ebx].vs_aregGDCData[REG_GDCMISC],GDCMISC_NOCHARGEN
	jz	short restore_text

	sub	ecx,ecx		; ECX == plane #
restore_plane:
	call	SetPlaneMode	; enable plane at VIDMEM_GRPHADDR (A0000h)
	mov	edi,[_pVRAM]
	mov	esi,[ebx].vs_pbPlane[ecx*4]
	push	ecx
	mov	ecx,[ebx].vs_cbPlane[ecx*4]
	shr	ecx,2		; BUGBUG -- assumes size is whole dwords
	rep	movsd		; restore plane
	pop	ecx
	inc	ecx
	cmp	cl,MAX_PLANES
	jb	restore_plane
	jmp	short restore_finish

restore_text:
	call	GetVRAMAddress	; EAX == VRAM address, EDX == size
	mov	edi,eax
	mov	esi,[ebx].vs_pbPlane[PLANE0*4]
	mov	ecx,[ebx].vs_cbPlane[PLANE0*4]
	cmp	ecx,edx
	jbe	short restore_text2
	mov	ecx,edx
restore_text2:
	shr	ecx,2		; BUGBUG -- assumes size is whole dwords
	test	[ebx].vs_flVState,VSTATE_SWAP
	jz	short restore_text4
restore_text3:
	mov	eax,[esi]
	xchg	[edi],eax
	add	edi,4
	mov	[esi],eax
	add	esi,4
	loop	restore_text3
	jmp	short restore_plane2
restore_text4:
	rep	movsd		; restore plane 0
restore_plane2:
	mov	cl,PLANE2
	call	SetPlaneMode
	mov	edi,[_pVRAM]
	mov	esi,[ebx].vs_pbPlane[PLANE2*4]
	mov	ecx,[ebx].vs_cbPlane[PLANE2*4]
	shr	ecx,2		; BUGBUG -- assumes size is whole dwords
	test	[ebx].vs_flVState,VSTATE_SWAP
	jz	short restore_text8
restore_text7:
	mov	eax,[esi]
	xchg	[edi],eax
	add	edi,4
	mov	[esi],eax
	add	esi,4
	loop	restore_text7
	jmp	short restore_finish
restore_text8:
	rep	movsd		; restore plane 2
restore_finish:

	pop	edi
	pop	esi
	ret

RestorePlanes endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   RestorePlaneMode
;
;   ENTRY
;	EBX == pvs
;
;   EXIT
;	Memory mode restored
;
;   USES
;	Only AX, DX
;
	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

	public	RestorePlaneMode
RestorePlaneMode proc near

	mov	dx,PORT_SEQINDX
	mov	al,REG_SEQMAPMASK
	mov	ah,[ebx].vs_aregSEQData[REG_SEQMAPMASK]
	out	dx,ax
	IODelay
	mov	al,REG_SEQMEMMODE
	mov	ah,[ebx].vs_aregSEQData[REG_SEQMEMMODE]
	out	dx,ax
	IODelay
	mov	al,[ebx].vs_regSEQIndx
	out	dx,al
	IODelay
	mov	dx,PORT_GDCINDX
	mov	al,REG_GDCSR
	mov	ah,[ebx].vs_aregGDCData[REG_GDCSR]
	out	dx,ax
	IODelay
	mov	al,REG_GDCESR
	mov	ah,[ebx].vs_aregGDCData[REG_GDCESR]
	out	dx,ax
	IODelay
	mov	al,REG_GDCCLRCMP
	mov	ah,[ebx].vs_aregGDCData[REG_GDCCLRCMP]
	out	dx,ax
	IODelay
	mov	al,REG_GDCROTATE
	mov	ah,[ebx].vs_aregGDCData[REG_GDCROTATE]
	out	dx,ax
	IODelay
	mov	al,REG_GDCREADMAP
	mov	ah,[ebx].vs_aregGDCData[REG_GDCREADMAP]
	out	dx,ax
	IODelay
	mov	al,REG_GDCMODE
	mov	ah,[ebx].vs_aregGDCData[REG_GDCMODE]
	out	dx,ax
	IODelay
	mov	al,REG_GDCMISC
	mov	ah,[ebx].vs_aregGDCData[REG_GDCMISC]
	out	dx,ax
	IODelay
	mov	al,REG_GDCDONTCARE
	mov	ah,[ebx].vs_aregGDCData[REG_GDCDONTCARE]
	out	dx,ax
	IODelay
	mov	al,REG_GDCBITMASK
	mov	ah,[ebx].vs_aregGDCData[REG_GDCBITMASK]
	out	dx,ax
	IODelay
	mov	al,[ebx].vs_regGDCIndx
	out	dx,al
	ret

RestorePlaneMode endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   GetVRAMAddress
;
;   ENTRY
;	EBX == pvs
;
;   EXIT
;	EAX == linear address of physical video memory, EDX == size in bytes
;
;   USES
;	Only EAX, EDX
;
;   The latter "assume" is used so that GetVRAMAddress can be called at
;   interrupt-time by TMRInterrupt
;
;	assume	cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG
	assume	cs:CODE_SEG,ds:nothing, es:nothing, ss:DATA_SEG

	public	GetVRAMAddress
GetVRAMAddress	proc near

	mov	al,[ebx].vs_aregGDCData[REG_GDCMISC]
	and	al,GDCMISC_ADDRMASK

	cmp	al,GDCMISC_64K_A0000
	jne	short not_A0000
	mov	eax,[_pVRAM]	; VIDMEM_GRPHADDR
	mov	edx,VIDMEM_GRPHSIZE
	ret
not_A0000:
	cmp	al,GDCMISC_32K_B0000
	jne	short not_B0000
	mov	eax,[_pVRAM]	; VIDMEM_MONOADDR
	add	eax,VIDMEM_MONOADDR - VIDMEM_GRPHADDR
	mov	edx,VIDMEM_MONOSIZE
	ret
not_B0000:
	cmp	al,GDCMISC_32K_B8000
	jne	short not_B8000
	mov	eax,[_pVRAM]	; VIDMEM_COLRADDR
	add	eax,VIDMEM_COLRADDR - VIDMEM_GRPHADDR
	mov	edx,VIDMEM_COLRSIZE
	ret
not_B8000:
	; It must be GDCMISC_128K_A0000
	mov	eax,[_pVRAM]	; VIDMEM_GRPHADDR
	mov	edx,VIDMEM_GRPHSIZE*2
	ret

GetVRAMAddress	endp


CLOSE_CODE


	end
