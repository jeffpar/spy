	page    ,132
        title   COMIO.ASM

;
;   COMIO.ASM
;   COM input/output services
;
;   by Jeff Parsons 31-Jan-1993
;


        include all.inc
        include com.inc


OPEN_DATA

        public  _COMPort
_COMPort    dd  0

CLOSE_DATA


OPEN_CODE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   _COMInit
;
;   ENTRY
;       iPort == COM port # to use
;
;   EXIT
;       EAX == TRUE if successful, FALSE if not
;
;   USES
;       Only EAX, ECX, EDX (C standard)
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

        public  _COMInit
_COMInit    proc near

        sub     eax,eax                 ; EAX == FALSE
        mov     edx,[esp+4]             ; EDX == COM port # (1 or 2)
        dec     edx                     ; convert to 0 or 1
        cmp     edx,1                   ; out of range?
        ja      com_exit                ; yup

        mov     ecx,[_pmbZero]
        movzx   edx,[ecx].rb_RS232Base[edx*2]
        or      edx,edx                 ; port exist?
        jz      short com_exit          ; nope
        mov     [_COMPort],edx          ; yup, save it

        mov     ecx,BD_9600             ; (ecx) = new baud rate
        mov     eax,CLOCK_RATE
        cdq                             ; (edx:eax) = clock rate
        div     ecx
        mov     ecx,eax                 ; (ecx) = clock rate / baud rate
;
;   Set divisor-latch access bit (DLAB) in line control reg.
;
        mov     edx,[_COMPort]

	add     dl,COM_IER
	xor     al,al
	out     dx,al
	IODelay
        add     dl,COM_LCR-COM_IER      ; (edx) -> LCR
	mov     al,LC_DLAB              ; turn on DLAB
	out     dx,al
	IODelay
;
;   Set divisor-latch value.
;   (dx) -> COM_LCR (Line Control Register)
;
        add     dl,COM_DLL-COM_LCR      ; (edx) -> LSB of baud latch
;       in      al,dx
;       cmp     al,cl
;       je      short com_lsbdone
;       IODelay
	mov     al,cl                   ; (al) = divisor latch LSB
	out     dx,al                   ; set LSB of baud latch
	IODelay

com_lsbdone:
        inc     dx
;       in      al,dx
;       cmp     al,cl
;       je      short com_msbdone
;       IODelay
	mov     al,ch                   ; (al) = divisor latch MSB
	out     dx,al
	IODelay

com_msbdone:
;
;   State of LCR is same as initcom in IBMBIO. (LCR = 3)
;   (dx) -> COM_DLM
;
        add     dl,COM_LCR-COM_DLM      ; (edx) -> LCR
	mov     al,3                    ; (al) = same mode as in IBMBIO
	out     dx,al
	IODelay
;
;   Enable the FIFO on a 16550A
;
IFDEF LATER
        add     dl,COM_IER - COM_LCR
        mov     al,00000111b
        out     dx,al
        IODelay
        in      al,dx
        IODelay
        test    al,10000000b
        jz      short com_nofifo        ; No FIFO
        test    al,01000000b
        jnz     short com_done          ; 16550A
com_nofifo:
        xor     al,al                   ; 16550 or no FIFO
        out     dx,al
ENDIF

com_done:
        mov     eax,TRUE                ; return TRUE

com_exit:
        ret

_COMInit    endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   _COMInput
;
;   ENTRY
;       None
;
;   EXIT
;       EAX == character, 0 if none
;
;   USES
;       Only EAX, ECX, EDX (C standard)
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

        public  _COMInput
_COMInput   proc near

        sub     eax,eax
        mov     edx,[_COMPort]
        or      edx,edx
        jz      short com_exit

        add     dl,COM_LSR
com_wait:
        in      al,dx
	IODelay
	and     al,1
        jz      short com_exit

        sub     dl,COM_LSR-COM_DAT
        in      al,dx
        and     al,7Fh

com_exit:
        ret

_COMInput   endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   _COMOutput
;
;   ENTRY
;       chOut == character to output
;
;   EXIT
;       EAX == TRUE if successful, FALSE if not
;
;   USES
;       Only EAX, ECX, EDX (C standard)
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

        public  _COMOutput
_COMOutput  proc near

        mov     edx,[_COMPort]
        or      edx,edx
        jz      short com_exit

        add     dl,COM_LSR
com_wait:
        in      al,dx
	IODelay
	test    al,020h
        jz      short com_wait

        sub     dl,COM_LSR-COM_DAT
        mov     al,[esp+4]
	out     dx,al

com_exit:
        ret

_COMOutput  endp


CLOSE_CODE


	end
