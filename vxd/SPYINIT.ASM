        page    ,132
        title   SPYINIT.ASM

;
;   SPY VxD
;
;   SPYINIT.ASM
;   SPY VxD initialization
;
;   by Jeff Parsons 31-Jan-1993
;

	.386p

        __acrtused  equ 1
        public  __acrtused

        include all.inc
        include vxdldr.inc


SPY_Device_ID   equ     1FFh    ; highest MS-reserved device ID
SPY_Init_Order  equ     DEBUG_INIT_ORDER

        Declare_Virtual_Device    SPY, 4, 0, \
                                  SPY_Control, SPY_Device_ID, SPY_Init_Order, \
                                ; SPY_V86_APIs, SPY_Prot_APIs


OPEN_DATA
;
;   Stuff for vxddump.c
;
        extrn   _NumberOfStaticVXDs:dword
        extrn   _pStaticVXDLocationArray:dword


        public  _dtrIDT
_dtrIDT DTR     <?, ?>          ; contents of current IDTR

        public  _dtrGDT
_dtrGDT DTR     <?, ?>          ; contents of current GDTR

        public  _dtrLDT
_dtrLDT DTR     <?, ?>          ; contents of simulated LDTR

        public  _sel_LDT
_sel_LDT  dw    ?               ; current LDT selector

        public  _sel_TSS
_sel_TSS  dw    ?               ; current TSS selector

        public  _sel_Flat, _sel_Text, _sel_Data
_sel_Flat dw    ?               ; current flat selector
_sel_Text dw    ?               ; current text selector
_sel_Data dw    ?               ; current data selector

        public  _pIDT
_pIDT   dd      ?               ; pointer to current IDT

        public  _pGDT
_pGDT   dd      ?               ; pointer to current GDT

        public  _pLDT
_pLDT   dd      ?               ; pointer to current LDT

        public  _pTSS
_pTSS   dd      ?               ; pointer to current TSS

	ifdef   DEBUG
	public  _szAssert
_szAssert   db  "Assertion failure in module %s, line %d", LF, NULL
	endif

CLOSE_DATA


OPEN_CODE

        extrn   _Main:near

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_Control
;
;   This is the SPY VxD's control procedure.
;
;   ENTRY:
;	EAX = Control call ID
;
;   EXIT:
;	If carry clear then
;	    Successful
;	else
;	    Control call failed
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;

BeginProc SPY_Control

        Control_Dispatch Sys_Critical_Init, SPY_Critical_Init
        Control_Dispatch Device_Init,       SPY_Device_Init

        clc                     ; ignore other control calls
	ret

EndProc SPY_Control


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_RegInit
;
;   ENTRY:
;       None
;
;   EXIT:
;       None
;
;   USES:
;       None
;

BeginProc SPY_RegInit, Async_Service, No_Prolog
;
;   Read current IDTR and GDTR registers, and then create 32-bit
;   pointers to their respective IDT and GDT structures
;
        cld
        sidt    [_dtrIDT]
        sgdt    [_dtrGDT]
        sldt    [_sel_LDT]
        str     [_sel_TSS]
        mov     [_sel_Flat],ds
        mov     [_sel_Text],cs
        mov     [_sel_Data],ds

        push    eax
        push    ecx
        push    edx

        mov     edx,[_dtrIDT].dtr_dwBase
        mov     ax,cs
        cmp     ax,[edx+IDT_KBD*size GATE].gate_wSel
        je      short SPY_KbdIDTOk
        int     3

SPY_KbdIDTOk:
        mov     [_pIDT],edx

        mov     edx,[_dtrGDT].dtr_dwBase
        mov     [_pGDT],edx

        movzx   eax,[_sel_LDT]
        test    al,NOT SEL_MASK
        jz      short SPY_LDTOk
        int     3

SPY_LDTOk:
        mov     cx,[edx+eax].des_wLimit
        mov     [_dtrLDT].dtr_wLimit,cx
        mov     cl,[edx+eax].des_bBase24_31
        rol     ecx,8
        mov     cl,[edx+eax].des_bBase16_23
        rol     ecx,16
        mov     cx,[edx+eax].des_wBase0_15
        mov     [_dtrLDT].dtr_dwBase,ecx
        mov     [_pLDT],ecx

        movzx   eax,[_sel_TSS]
        test    al,NOT SEL_MASK
        jz      short SPY_TSSOk
        int     3

SPY_TSSOk:
        mov     cl,[edx+eax].des_bBase24_31
        rol     ecx,8
        mov     cl,[edx+eax].des_bBase16_23
        rol     ecx,16
        mov     cx,[edx+eax].des_wBase0_15
        mov     [_pTSS],ecx

        pop     edx
        pop     ecx
        pop     eax
	ret

EndProc SPY_RegInit


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   _Reboot
;
;   ENTRY:
;       None
;
;   EXIT:
;       None
;
;   USES:
;       All
;

BeginProc _Reboot

        extrn   _DrainKbd:near

	ifdef	KBDSCAN3

        extrn   _WaitKbdInput:near, _WaitKbdOutput:near

        call    _WaitKbdInput       ; wait for ready state
        mov     al,KEYCMD_SETSCANSET
        out     KBDPORT_DATA,al     ; tell keyboard to enable new scan set
        call    _WaitKbdOutput      ; wait for acknowledgement
        in      al,KBDPORT_DATA     ; get it
        mov     al,1                ; select scan set #1 (default set)
        out     KBDPORT_DATA,al     ;
        call    _WaitKbdOutput      ; wait for acknowledgement

	endif  ;KBDSCAN3

        cli
        SumVecs386                  ; vector chksum ok?
        jne     reboot_vecbad       ; no
        CopyVecs386                 ; copy vectors back into the IVT

        ResetPICs                   ; reset PICS for real-mode operation
        call    _DrainKbd
;
;   Do some rudimentary reinitialization of the ROM BIOS data area
;
        mov     ds:[rb_vid.vid_CRTMode],VMODE_CO80

        mov     ds:[rb_IOROMInit],00700000h
        mov     edi,700h            ; where to dump our "pre-boot" code
        mov     esi,OFFSET32 reboot_code
        mov     ecx,reboot_code_end - reboot_code
        rep     movsb

        ResetCPU CMOSSHUTDOWN_FARJMP

reboot_code label byte
        db      OP_MOVAX, 70h, 00h  ; mov ax,0070h
        mov     ss,eax              ; mov ss,ax
        db      OP_MOVSP, 00h, 00h  ; mov sp,0000h
        int     19h                 ; int 19h
reboot_code_end label byte

reboot_vecbad:
        mov     ds:[rb_ResetFlag],RESETFLAG_WARM
        ResetCPU                    ; vector info is hosed, just warm boot

EndProc _Reboot


;
;   Stuff for vxddump.c
;

BeginProc _GetDeviceList, No_Prolog
        VXDCall VXDLDR_GetDeviceList
        ret
EndProc   _GetDeviceList


CLOSE_CODE


OPEN_ICODE

        extrn   SPY_KBDInit:near
        extrn   SPY_VMMInit:near
        extrn   SPY_INTInit:near

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_Critical_Init
;
;   ENTRY:
;       None
;
;   EXIT:
;       Carry clear (always works)
;
;   USES:
;       All
;

BeginProc SPY_Critical_Init

        call    SPY_RegInit
        call    SPY_VMMInit         ; sets carry accordingly

	ret

EndProc SPY_Critical_Init


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_Device_Init
;
;   ENTRY:
;	EBX = System VM Handle
;
;   EXIT:
;       Carry clear (always works)
;
;   USES:
;       All
;

BeginProc SPY_Device_Init

        call    SPY_RegInit
        call    SPY_KBDInit
;
;   Stuff for vxddump.c
;
        VMMCall VMM_GetVxDLocationList
        mov     [_pStaticVXDLocationArray], eax
        mov     [_NumberOfStaticVXDs], edx
;
;   Force the int 20 dynalink call to be resolved now rather
;   than at debug time.
;
        call    _GetDeviceList
;
;   Create a dummy ESF frame for the call to _Main;  it's useful
;   to enter the debugger for the first time this way, because then
;   the unhooked IDT can be examined if need be.  All you need to do is
;   a "t" to regain control at the "ret" below with all hooks in place.
;
;   One of the problem(s) with this approach, however, is that the IMRs
;   that are normally saved on the frame (see SPY_Debug, the normal
;   entry into the debugger) are not correct.  Again, after the first "t"
;   or "g" command, that's no longer an issue.  And of course, there's
;   the familiar problem of ESP not really changing, which is just not
;   worth trying to support at this point either.
;
        clc
        pushfd                      ; flags, with carry pre-cleared
        push    cs                  ; CS
        push    OFFSET32 sdinit_complete
        push    0                   ; iErrCode  (dummy)
        push    IDT_DEBUG           ; iTrap     (dummy)
        pusha                       ; regs
        push    ds                  ; saveDS
        push    es                  ; saveES
        sub     esp,esf_saveES      ; reserve space for the rest
        mov     [esp].esf_hVM,ebx   ; save system VM handle
        call    _Main               ; dispatch to C code
        add     esp,esf_saveES      ; throw away the rest
        call    SPY_INTInit         ; hook the IDT like crazy now
        pop     es                  ;
        pop     ds                  ;
        popa                        ;
        add     esp,isf_EIP         ; throw away iTrap and iErrCode
        iretd                       ; if trace flag was turned on, honor it

sdinit_complete:
	ret

EndProc SPY_Device_Init


CLOSE_ICODE


        end
