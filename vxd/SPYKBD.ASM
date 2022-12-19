	page    ,132
        title   SPYKBD.ASM

;
;   SPY VxD
;
;   SPYKBD.ASM
;   SPY VxD keyboard handling
;
;   by Jeff Parsons 31-Jan-1993
;


        include all.inc
        include vkd.inc                 ; hook VKD_Filter_Keyboard_Input


OPEN_DATA

        extrn   _iDebugEntry:dword

        public  _pfnPrev_VKD_Filter_Keyboard_Input
_pfnPrev_VKD_Filter_Keyboard_Input  dd  0

        public  _pfnPrev_KBDInterrupt
_pfnPrev_KBDInterrupt  dd  0

        public  _flKeyEvent
_flKeyEvent dd  0

	public  _abKeyBuff
_abKeyBuff  db  KEYBUFF_MAX dup(?)      ; don't insert goop between
					; abKeyBuff and pbKeyHead; the latter
					; marks the end of the former
	public  _pbKeyHead, _pbKeyTail
_pbKeyHead  dd  offset _abKeyBuff       ; advanced at task-time
_pbKeyTail  dd  offset _abKeyBuff       ; advanced at interrupt-time


	public  _flKeyShift
_flKeyShift dd  0                       ; very similar to the BIOS flags


	public  _abXlatNorm
_abXlatNorm label   byte
	db      00h, ESCAPE,'1',  '2',  '3',  '4',  '5',  '6'   ; 00-07h
	db      '7',  '8',  '9',  '0',  '-',  '=',   BS,  TAB   ; 08-0Fh
	db      'q',  'w',  'e',  'r',  't',  'y',  'u',  'i'   ; 10-17h
        db      'o',  'p',  '[',  ']',   CR,  00h,  'a',  's'   ; 18-1Fh
	db      'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';'   ; 20-27h
	db      "'",  '`',  00h,  '\',  'z',  'x',  'c',  'v'   ; 28-2Fh
	db      'b',  'n',  'm',  ',',  '.',  '/',  00h,  '*'   ; 30-37h
        db      00h,  ' ',  00h,  00h,  CTRLA,CTRLK,00h,  00h   ; 38-3Fh
        db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  CTRLB ; 40-47h
        db      CTRLK,CTRLU,'-',  CTRLH,00h,  CTRLL,'+',  CTRLE ; 48-4Fh
        db      CTRLJ,CTRLD,00h,  00h,  00h,  00h,  00h,  00h   ; 50-57h
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 58-5Fh
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 60-67h
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 68-6Fh
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 70-77h
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 78-7Fh

        public  _abXlatCtrl
_abXlatCtrl label  byte
        db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 00-07h
        db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 08-0Fh
        db      CTRLQ,CTRLW,CTRLE,CTRLR,CTRLT,CTRLY,CTRLU,CTRLI ; 10-17h
        db      CTRLO,CTRLP,00h,  00h,  00h,  00h,  CTRLA,CTRLS ; 18-1Fh
        db      CTRLD,CTRLF,CTRLG,CTRLH,CTRLJ,CTRLK,CTRLL,00h   ; 20-27h
        db      00h,  00h,  00h,  00h,  CTRLZ,CTRLX,CTRLC,CTRLV ; 28-2Fh
        db      CTRLB,CTRLN,CTRLM,00h,  00h,  00h,  00h,  00h   ; 30-37h
        db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 38-3Fh
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 40-47h
        db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 48-4Fh
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 50-57h
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 58-5Fh
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 60-67h
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 68-6Fh
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 70-77h
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 78-7Fh

	public  _abXlatShift
_abXlatShift label  byte
	db      00h, ESCAPE,'!',  '@',  '#',  '$',  '%',  '^'   ; 00-07h
	db      '&',  '*',  '(',  ')',  '_',  '+',   BS,  TAB   ; 08-0Fh
	db      'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I'   ; 10-17h
        db      'O',  'P',  '{',  '}',   CR,  00h,  'A',  'S'   ; 18-1Fh
	db      'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':'   ; 20-27h
	db      '"',  '~',  00h,  '|',  'Z',  'X',  'C',  'V'   ; 28-2Fh
	db      'B',  'N',  'M',  '<',  '>',  '?',  00h,  '*'   ; 30-37h
	db      00h,  ' ',  00h,  00h,  00h,  00h,  00h,  00h   ; 38-3Fh
        db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  CTRLB ; 40-47h
        db      CTRLK,CTRLU,'-',  CTRLH,00h,  CTRLL,'+',  CTRLE ; 48-4Fh
        db      CTRLJ,CTRLD,00h,  00h,  00h,  00h,  00h,  00h   ; 50-57h
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 58-5Fh
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 60-67h
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 68-6Fh
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 70-77h
	db      00h,  00h,  00h,  00h,  00h,  00h,  00h,  00h   ; 78-7Fh

CLOSE_DATA


OPEN_ICODE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_KBDInit - Initialize KBDHook
;
;   ENTRY
;       None
;
;   EXIT
;       Carry clear if success, set if not
;
;   USES
;       None
;
BeginProc   SPY_KBDInit
        push    eax
        push    edx
        push    esi
;
;   Set fast typematic rate
;
	in      al,PIC_MASTER_IMR
        mov     ah,al
        or      al,(1 shl IRQ_KBD)  ; temporarily mask kbd interrupts
        out     PIC_MASTER_IMR,al

        mov     al,KEYCMD_SETTYPEMATIC
        out     KBDPORT_DATA,al     ; tell keyboard to set new typematic rate
        call    _WaitKbdOutput      ; wait for acknowledgement
        in      al,KBDPORT_DATA     ; get it
        mov     al,TYPEMATIC_FAST   ; select rate (bits 0-4), delay (bits 5-6)
        out     KBDPORT_DATA,al     ;
        call    _WaitKbdOutput      ; wait for acknowledgement
        in      al,KBDPORT_DATA     ; get it

        mov     al,ah
        out     PIC_MASTER_IMR,al   ; restore kbd interrupt mask
;
;   Hook the keyboard interrupt
;
        mov     edx,[_pIDT]
        mov     ax,[edx+IDT_KBD*size GATE].gate_wBase16_31
        rol     eax,16
        mov     ax,[edx+IDT_KBD*size GATE].gate_wBase0_15
        mov     [_pfnPrev_KBDInterrupt],eax

        mov     eax,OFFSET32 SPY_KBDInterrupt
        mov     [edx+IDT_KBD*size GATE].gate_wBase0_15,ax
        rol     eax,16
        mov     [edx+IDT_KBD*size GATE].gate_wBase16_31,ax
;
;   Hook VKD's filter service, so the interrupt handler knows when to kick in
;
        mov     eax,@@VKD_Filter_Keyboard_Input
        mov     esi,OFFSET32 SPYHook_VKD_Filter_Keyboard_Input
        VMMCall Hook_Device_Service

        mov     [_pfnPrev_VKD_Filter_Keyboard_Input],esi

        pop     esi
        pop     edx
        pop     eax
        ret

EndProc SPY_KBDInit


CLOSE_ICODE


OPEN_CODE

        extrn   SPY_Debug:near          ; in spytrap.asm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPYHook_VKD_Filter_Keyboard_Input - VKD interrupt hook
;
;   ENTRY
;       CL == scan code
;
;   EXIT
;       Carry clear -> continue processing, CL == (new) scan code
;
;   USES
;       None
;
BeginProc SPYHook_VKD_Filter_Keyboard_Input, Async_Service, No_Prolog, Hook_Proc, _pfnPrev_VKD_Filter_Keyboard_Input

        cmp     cl,KBDSCAN_F12          ; F12 key?
        jne     short kbdhook_passthru  ; no
;
;   Our interrupt handler will take care of everything from here
;
        or      [_flKeyEvent],KEYEVENT_DEBUGGER
        stc                             ; eat key
        ret

kbdhook_passthru:
;
;   But before we pass thru, keep an eye on shift-state keys so that
;   our shift state guy (_flKeyShift) doesn't get out of sync with reality
;
        mov     al,cl
        call    CheckShift              ; check that scan code in AL
        jmp     [_pfnPrev_VKD_Filter_Keyboard_Input]    ; (we don't care what the result was)

EndProc SPYHook_VKD_Filter_Keyboard_Input


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_KBDInterrupt
;
;   ENTRY
;       None
;
;   EXIT
;       None
;
;   USES
;       None
;
        assume  cs:CODE_SEG,ds:nothing,es:nothing,ss:DATA_SEG

        public  SPY_KBDInterrupt
SPY_KBDInterrupt  proc near
	push    0                       ; dummy iErrCode
	push    IDT_KBD                 ; iTrap

	pusha
	push    ds
	push    es
        sub     esp,esf_saveES          ; reserve space for the rest
	mov     ebp,esp                 ; EBP -> ESF

        mov     eax,ss
        mov     ds,eax
        mov     es,eax
        assume  ds:DATA_SEG, es:DATA_SEG

        test    [_flKeyEvent],KEYEVENT_DEBUGGER
        jnz     short kbd_readit

        cmp     [_iDebugEntry],0
        je      short kbd_passthru

kbd_readit:
	in      al,KBDPORT_DATA         ; read scan code

        btr     [_flKeyEvent],KEYEVENT_DEBUGGER_BIT
        jnc     short kbd_bufferit

        push    DEBUG_INT               ; yes, break into debugger immediately
        call    SPY_Debug               ; call debugger router
	jmp     kbd_return              ; terminate this interrupt

kbd_passthru:
        mov     eax,[_pfnPrev_KBDInterrupt]
        mov     [ebp].esf_iErrCode,eax  ; set iErrCode to next handler
	add     esp,esf_saveES          ; throw away the rest
	pop     es
	pop     ds
	popa
        add     esp,4                   ; throw away iTrap but not iErrCode
        ret                             ; return through iErrCode

kbd_bufferit:
        call    CheckShift              ; is AL shift key or break code?
        jz      short kbd_eoi           ; yes

	mov     edi,[_pbKeyTail]        ;
	mov     esi,edi                 ;
	inc     edi                     ;
	cmp     edi,offset _pbKeyHead   ;
	jb      short kbd_tailok        ;
	mov     edi,offset _abKeyBuff   ;
kbd_tailok:                             ;
	cmp     edi,[_pbKeyHead]        ; will this overflow the buffer?
	je      short kbd_eoi           ; yes

	mov     ebx,offset _abXlatNorm  ;
	test    byte ptr [_flKeyShift],KBFLAG_SHIFTMASK
        jz      short kbd_xlatctrl      ;
	mov     ebx,offset _abXlatShift ;
        jmp     short kbd_xlatnorm      ;
kbd_xlatctrl:
        test    byte ptr [_flKeyShift],KBFLAG_LCTRL
        jz      short kbd_xlatnorm      ;
        mov     ebx,offset _abXlatCtrl  ;
kbd_xlatnorm:
	xlat                            ;
        cmp     al,CTRLC                ; abort sequence?
        je      short kbd_abort         ; yes
        cmp     al,ESCAPE               ; abort sequence?
        jne     short kbd_save          ; no
kbd_abort:                              ; yes
        or      [_flKeyEvent],KEYEVENT_ABORT
        jmp     short kbd_eoi
kbd_save:
	mov     [esi],al                ; save the code received
	mov     [_pbKeyTail],edi        ; and update the tail pointer

kbd_eoi:
        mov     al,PIC_EOI
        out     PIC_MASTER,al

kbd_return:
	add     esp,esf_saveES          ; throw away the rest
	pop     es
	pop     ds
	popa
	add     esp,isf_EIP             ; throw away iTrap and iErrCode
	iretd
SPY_KBDInterrupt endp


        public  CheckShift
CheckShift proc near
        mov     ah,al                   ; save original scan code in AH
	and     al,NOT KBDSCAN_BREAKPREFIX
	cmp     al,KBDSCAN_RSHIFT       ; check for shift keys
        jne     short cs_notrs          ;
	mov     al,KBFLAG_RSHIFT        ;
        jmp     short cs_shift          ;
cs_notrs:
	cmp     al,KBDSCAN_LSHIFT       ;
        jne     short cs_notls          ;
        mov     al,KBFLAG_LSHIFT        ;
        jmp     short cs_shift          ;
cs_notls:
        cmp     al,KBDSCAN_LCTRL        ;
        jne     short cs_notshift       ;
        mov     al,KBFLAG_LCTRL         ;
cs_shift:
	test    ah,KBDSCAN_BREAKPREFIX  ; is this a shift make or break?
        jnz     short cs_shiftbreak     ; break
	or      byte ptr [_flKeyShift],al
        sub     al,al                   ; set Z flag
        ret                             ; bye-bye
cs_shiftbreak:
	not     al                      ;
	and     byte ptr [_flKeyShift],al
        sub     al,al                   ; set Z flag
        ret                             ; bye-bye
cs_notshift:
        and     ah,KBDSCAN_BREAKPREFIX  ;
        cmp     ah,KBDSCAN_BREAKPREFIX  ; return Z flag set if break key
        ret
CheckShift endp


        public  _DrainKbd
_DrainKbd proc  near
        pushfd
        cli
        push    eax
        in      al,KBDPORT_STATUS
        test    al,KBDSTATUS_OUTPUT
        jz      short drain_done
drain_loop:
        in      al,KBDPORT_DATA
        call    _WaitKbdOutput
        jnz     drain_loop
drain_done:
        pop     eax
        popfd
	ret
_DrainKbd       endp

        public  _EnableKbd
_EnableKbd      proc near
        push    eax
        call    _WaitKbdInput
	mov     al,KBDCMD_ENABLE_KBD
        out     KBDPORT_CMD,al
        pop     eax
	ret
_EnableKbd      endp

        public  _DisableKbd
_DisableKbd     proc near
        push    eax
        call    _WaitKbdInput
	mov     al,KBDCMD_DISABLE_KBD
	out     KBDPORT_CMD,al
        call    _WaitKbdInput
        pop     eax
	ret
_DisableKbd     endp

        public  _DrainMse
_DrainMse proc  near
        pushfd
        cli
        push    eax
        in      al,KBDPORT_STATUS
        test    al,KBDSTATUS_OUTPUT
        jz      short drain_done
drain_loop:
        test    al,KBDSTATUS_AUXDATA
        jz      short drain_done
        in      al,KBDPORT_DATA
        call    _WaitKbdOutput
        jnz     drain_loop
drain_done:
        pop     eax
        popfd
	ret
_DrainMse       endp

        public  _EnableMse
_EnableMse      proc near
        push    eax
        call    _WaitKbdInput
        mov     al,KBDCMD_MSE
        out     KBDPORT_CMD,al
        call    _WaitKbdInput
        call    _WaitKbdOutput
        mov     al,KBDMSE_ENABLE
        out     KBDPORT_DATA,al
        pop     eax
	ret
_EnableMse      endp

        public  _DisableMse
_DisableMse     proc near
        push    eax
        call    _WaitKbdInput
        mov     al,KBDCMD_MSE
        out     KBDPORT_CMD,al
        call    _WaitKbdInput
        call    _WaitKbdOutput
        mov     al,KBDMSE_DISABLE
        out     KBDPORT_DATA,al
        pop     eax
	ret
_DisableMse     endp

        public  _WaitKbdInput
_WaitKbdInput   proc near
        push    ecx
        mov     ecx,20000h
wait_loop:
	in      al,KBDPORT_STATUS
	test    al,KBDSTATUS_INPUT
        loopnz  wait_loop
        pop     ecx
	ret
_WaitKbdInput   endp

        public  _WaitKbdOutput
_WaitKbdOutput  proc near
        push    ecx
	mov     ecx,20000h
wait_loop:
	in      al,KBDPORT_STATUS
	test    al,KBDSTATUS_OUTPUT
        loopz   wait_loop
        pop     ecx
	ret
_WaitKbdOutput  endp


CLOSE_CODE


	end
