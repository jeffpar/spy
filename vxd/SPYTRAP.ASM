	page    ,132
        title   SPYTRAP.ASM

;
;   SPY VxD
;
;   SPYTRAP.ASM
;   SPY VxD trap handlers
;
;   by Jeff Parsons 31-Jan-1993
;


	include all.inc
        include debugsys.inc

OPEN_CODE

        extrn   _LogInALnn:near
        extrn   _LogInAXnn:near
        extrn   _LogOutnnAL:near
        extrn   _LogOutnnAX:near
        extrn   _LogInALDX:near
        extrn   _LogInAXDX:near
        extrn   _LogOutDXAL:near
        extrn   _LogOutDXAX:near

CLOSE_CODE


OPEN_DATA

        extrn   _flTrace:dword
        extrn   _pDebugStack:dword
        extrn   _pDebugStackTop:dword
        extrn   _hVMSwitch:dword


HookEntry struc
    HookServ    dd  ?
    HookHandler dd  ?
HookEntry ends

;
;   There are dependencies on the order of these hook entries;
;   these equates document them.
;
HOOK_OUT_DEBUG_CHR              equ 0

        public  HookTbl
HookTbl HookEntry   <@@Out_Debug_Chr,   OFFSET32 SPYHOOK_Out_Debug_Chr>
;;;     HookEntry   <@@In_Debug_Chr,    OFFSET32 SPYHOOK_In_Debug_Chr>
;;;     HookEntry   <@@Is_Debug_Chr,    OFFSET32 SPYHOOK_Is_Debug_Chr>
        dd          -1


;
; Bit Values for IDTFlags
;
TRAP_IDT            equ 01h     ; trap even if segment differs from current CS

TrapEntry struc
    IDTNum      db  ?
    IDTFlags    db  ?
    INTSegment  dw  ?
    INTHandler  dd  ?
    VMMHandler  dd  ?
TrapEntry ends

        public  TrapTbl
TrapTbl TrapEntry   <IDT_DIVERROR,  0, 0, OFFSET32 SPY_INTDivError,   OFFSET32 SPY_VMMDivError>
        TrapEntry   <IDT_DEBUG,     0, 0, OFFSET32 SPY_INTDebug,      OFFSET32 SPY_VMMDebug>
        TrapEntry   <IDT_NMI,       0, 0, OFFSET32 SPY_INTNMI,        OFFSET32 SPY_VMMNMI>
        TrapEntry   <IDT_BREAKPOINT,0, 0, OFFSET32 SPY_INTBreakPoint, OFFSET32 SPY_VMMBreakPoint>
        TrapEntry   <IDT_OVERFLOW,  0, 0, OFFSET32 SPY_INTOverflow,   OFFSET32 SPY_VMMOverflow>
        TrapEntry   <IDT_BOUNDS,    0, 0, OFFSET32 SPY_INTBounds,     OFFSET32 SPY_VMMBounds>
        TrapEntry   <IDT_INVOPCODE, 0, 0, OFFSET32 SPY_INTInvOpCode,  OFFSET32 SPY_VMMInvOpCode>
        TrapEntry   <IDT_NOCP,      0, 0, OFFSET32 SPY_INTNoCP,       OFFSET32 SPY_VMMNoCP>
        TrapEntry   <IDT_DBLFAULT,  0, 0, OFFSET32 SPY_INTDblFault,   OFFSET32 SPY_VMMDblFault>
        TrapEntry   <IDT_RESERVED09,0, 0, 0,                          0>
        TrapEntry   <IDT_TSSFAULT,  0, 0, OFFSET32 SPY_INTTSSFault,   OFFSET32 SPY_VMMTSSFault>
        TrapEntry   <IDT_SEGFAULT,  0, 0, OFFSET32 SPY_INTSegFault,   OFFSET32 SPY_VMMSegFault>
        TrapEntry   <IDT_STACKFAULT,0, 0, OFFSET32 SPY_INTStackFault, OFFSET32 SPY_VMMStackFault>
        TrapEntry   <IDT_GPFAULT,   0, 0, OFFSET32 SPY_INTGPFault,    OFFSET32 SPY_VMMGPFault>
        TrapEntry   <IDT_PAGEFAULT, 0, 0, OFFSET32 SPY_INTPageFault,  OFFSET32 SPY_VMMPageFault>
        TrapEntry   <IDT_RESERVED0F,0, 0, 0,                          0>
        TrapEntry   <IDT_CPFAULT,   0, 0, OFFSET32 SPY_INTCPFault,    OFFSET32 SPY_VMMCPFault>
        TrapEntry   <11h,           0, 0, 0,                          0>
        TrapEntry   <12h,           0, 0, 0,                          0>
        TrapEntry   <13h,           0, 0, 0,                          0>
        TrapEntry   <14h,           0, 0, 0,                          0>
        TrapEntry   <15h,           0, 0, 0,                          0>
        TrapEntry   <16h,           0, 0, 0,                          0>
        TrapEntry   <17h,           0, 0, 0,                          0>
        TrapEntry   <18h,           0, 0, 0,                          0>
        TrapEntry   <19h,           0, 0, 0,                          0>
        TrapEntry   <1Ah,           0, 0, 0,                          0>
        TrapEntry   <1Bh,           0, 0, 0,                          0>
        TrapEntry   <1Ch,           0, 0, 0,                          0>
        TrapEntry   <1Dh,           0, 0, 0,                          0>
        TrapEntry   <1Eh,           0, 0, 0,                          0>
        TrapEntry   <1Fh,           0, 0, 0,                          0>
        TrapEntry   <20h,           0, 0, 0,                          0>
        TrapEntry   <21h,           0, 0, 0,                          0>
        TrapEntry   <22h,           0, 0, 0,                          0>
        TrapEntry   <23h,           0, 0, 0,                          0>
        TrapEntry   <24h,           0, 0, 0,                          0>
        TrapEntry   <25h,           0, 0, 0,                          0>
        TrapEntry   <26h,           0, 0, 0,                          0>
        TrapEntry   <27h,           0, 0, 0,                          0>
        TrapEntry   <28h,           0, 0, 0,                          0>
        TrapEntry   <29h,           0, 0, 0,                          0>
        TrapEntry   <2Ah,           0, 0, 0,                          0>
        TrapEntry   <2Bh,           0, 0, 0,                          0>
        TrapEntry   <2Ch,           0, 0, 0,                          0>
        TrapEntry   <2Dh,           0, 0, 0,                          0>
        TrapEntry   <2Eh,           0, 0, 0,                          0>
        TrapEntry   <2Fh,           0, 0, 0,                          0>
        TrapEntry   <30h,           0, 0, 0,                          0>
        TrapEntry   <31h,           0, 0, 0,                          0>
        TrapEntry   <32h,           0, 0, 0,                          0>
        TrapEntry   <33h,           0, 0, 0,                          0>
        TrapEntry   <34h,           0, 0, 0,                          0>
        TrapEntry   <35h,           0, 0, 0,                          0>
        TrapEntry   <36h,           0, 0, 0,                          0>
        TrapEntry   <37h,           0, 0, 0,                          0>
        TrapEntry   <38h,           0, 0, 0,                          0>
        TrapEntry   <39h,           0, 0, 0,                          0>
        TrapEntry   <3Ah,           0, 0, 0,                          0>
        TrapEntry   <3Bh,           0, 0, 0,                          0>
        TrapEntry   <3Ch,           0, 0, 0,                          0>
        TrapEntry   <3Dh,           0, 0, 0,                          0>
        TrapEntry   <3Eh,           0, 0, 0,                          0>
        TrapEntry   <3Fh,           0, 0, 0,                          0>
        TrapEntry   <40h,           0, 0, 0,                          0>
        TrapEntry   <IDT_DEBUGREQ,  1, 0, OFFSET32 SPY_INTDebugReq,   0>
        TrapEntry   <42h,           0, 0, 0,                          0>
        TrapEntry   <43h,           0, 0, 0,                          0>
        TrapEntry   <44h,           0, 0, 0,                          0>
        TrapEntry   <45h,           0, 0, 0,                          0>
        TrapEntry   <46h,           0, 0, 0,                          0>
        TrapEntry   <47h,           0, 0, 0,                          0>
        TrapEntry   <48h,           0, 0, 0,                          0>
        TrapEntry   <49h,           0, 0, 0,                          0>
        TrapEntry   <4Ah,           0, 0, 0,                          0>
        TrapEntry   <4Bh,           0, 0, 0,                          0>
        TrapEntry   <4Ch,           0, 0, 0,                          0>
        TrapEntry   <4Dh,           0, 0, 0,                          0>
        TrapEntry   <4Eh,           0, 0, 0,                          0>
        TrapEntry   <4Fh,           0, 0, 0,                          0>
        TrapEntry   <50h,           0, 0, 0,                          0>
        TrapEntry   <51h,           0, 0, 0,                          0>
        TrapEntry   <52h,           0, 0, 0,                          0>
        TrapEntry   <53h,           0, 0, 0,                          0>
        TrapEntry   <54h,           0, 0, 0,                          0>
        TrapEntry   <55h,           0, 0, 0,                          0>
        TrapEntry   <56h,           0, 0, 0,                          0>
        TrapEntry   <57h,           0, 0, 0,                          0>
        TrapEntry   <58h,           0, 0, 0,                          0>
        TrapEntry   <59h,           0, 0, 0,                          0>
        TrapEntry   <5Ah,           0, 0, 0,                          0>
        TrapEntry   <5Bh,           0, 0, 0,                          0>
        TrapEntry   <5Ch,           0, 0, 0,                          0>
        TrapEntry   <5Dh,           0, 0, 0,                          0>
        TrapEntry   <5Eh,           0, 0, 0,                          0>
        TrapEntry   <5Fh,           0, 0, 0,                          0>
        db          -1

        public  INTFaultCounts
INTFaultCounts label dword
        dd      0   ;IDT_DIVERROR
        dd      0   ;IDT_DEBUG
        dd      0   ;IDT_NMI
        dd      0   ;IDT_BREAKPOINT
        dd      0   ;IDT_OVERFLOW
        dd      0   ;IDT_BOUNDS
        dd      0   ;IDT_INVOPCODE
        dd      0   ;IDT_NOCP
        dd      0   ;IDT_DBLFAULT
        dd      0   ;IDT_RESERVED09
        dd      0   ;IDT_TSSFAULT
        dd      0   ;IDT_SEGFAULT
        dd      0   ;IDT_STACKFAULT
        dd      0   ;IDT_GPFAULT
        dd      0   ;IDT_PAGEFAULT
        dd      0   ;IDT_RESERVED0F
        dd      0   ;IDT_CPFAULT

        public  SavedIMRs
SavedIMRs       dd  ?

        public  _afnLogging
_afnLogging label dword
	dd      0                       ; 00h   ADD
	dd      0                       ; 01h   ADD
	dd      0                       ; 02h   ADD
	dd      0                       ; 03h   ADD
	dd      0                       ; 04h   ADD
	dd      0                       ; 05h   ADD
	dd      0                       ; 06h   PUSH
	dd      0                       ; 07h   POP
	dd      0                       ; 08h   OR
	dd      0                       ; 09h   OR
	dd      0                       ; 0Ah   OR
	dd      0                       ; 0Bh   OR
	dd      0                       ; 0Ch   OR
	dd      0                       ; 0Dh   OR
	dd      0                       ; 0Eh   PUSH
	dd      0                       ; 0Fh
	dd      0                       ; 10h   ADC
	dd      0                       ; 11h   ADC
	dd      0                       ; 12h   ADC
	dd      0                       ; 13h   ADC
	dd      0                       ; 14h   ADC
	dd      0                       ; 15h   ADC
	dd      0                       ; 16h   PUSH
	dd      0                       ; 17h   POP
	dd      0                       ; 18h   SBB
	dd      0                       ; 19h   SBB
	dd      0                       ; 1Ah   SBB
	dd      0                       ; 1Bh   SBB
	dd      0                       ; 1Ch   SBB
	dd      0                       ; 1Dh   SBB
	dd      0                       ; 1Eh   PUSH
	dd      0                       ; 1Fh   POP
	dd      0                       ; 20h   AND
	dd      0                       ; 21h   AND
	dd      0                       ; 22h   AND
	dd      0                       ; 23h   AND
	dd      0                       ; 24h   AND
	dd      0                       ; 25h   AND
	dd      0                       ; 26h   ES
	dd      0                       ; 27h   DAA
	dd      0                       ; 28h   SUB
	dd      0                       ; 29h   SUB
	dd      0                       ; 2Ah   SUB
	dd      0                       ; 2Bh   SUB
	dd      0                       ; 2Ch   SUB
	dd      0                       ; 2Dh   SUB
	dd      0                       ; 2Eh   CS
	dd      0                       ; 2Fh   DAS
	dd      0                       ; 30h   XOR
	dd      0                       ; 31h   XOR
	dd      0                       ; 32h   XOR
	dd      0                       ; 33h   XOR
	dd      0                       ; 34h   XOR
	dd      0                       ; 35h   XOR
	dd      0                       ; 36h   SS
	dd      0                       ; 37h   AAA
	dd      0                       ; 38h   CMP
	dd      0                       ; 39h   CMP
	dd      0                       ; 3Ah   CMP
	dd      0                       ; 3Bh   CMP
	dd      0                       ; 3Ch   CMP
	dd      0                       ; 3Dh   CMP
	dd      0                       ; 3Eh   DS
	dd      0                       ; 3Fh   AAS
	dd      0                       ; 40h   INC
	dd      0                       ; 41h   INC
	dd      0                       ; 42h   INC
	dd      0                       ; 43h   INC
	dd      0                       ; 44h   INC
	dd      0                       ; 45h   INC
	dd      0                       ; 46h   INC
	dd      0                       ; 47h   INC
	dd      0                       ; 48h   DEC
	dd      0                       ; 49h   DEC
	dd      0                       ; 4Ah   DEC
	dd      0                       ; 4Bh   DEC
	dd      0                       ; 4Ch   DEC
	dd      0                       ; 4Dh   DEC
	dd      0                       ; 4Eh   DEC
	dd      0                       ; 4Fh   DEC
	dd      0                       ; 50h   PUSH
	dd      0                       ; 51h   PUSH
	dd      0                       ; 52h   PUSH
	dd      0                       ; 53h   PUSH
	dd      0                       ; 54h   PUSH
	dd      0                       ; 55h   PUSH
	dd      0                       ; 56h   PUSH
	dd      0                       ; 57h   PUSH
	dd      0                       ; 58h   POP
	dd      0                       ; 59h   POP
	dd      0                       ; 5Ah   POP
	dd      0                       ; 5Bh   POP
	dd      0                       ; 5Ch   POP
	dd      0                       ; 5Dh   POP
	dd      0                       ; 5Eh   POP
	dd      0                       ; 5Fh   POP
	dd      0                       ; 60h   PUSHA
	dd      0                       ; 61h   POPA
	dd      0                       ; 62h   BOUND
	dd      0                       ; 63h   ARPL
	dd      0                       ; 64h   FS
	dd      0                       ; 65h   GS
	dd      0                       ; 66h   OSIZE
	dd      0                       ; 67h   ASIZE
	dd      0                       ; 68h   PUSH
	dd      0                       ; 69h   IMUL
	dd      0                       ; 6Ah   PUSH
	dd      0                       ; 6Bh   IMUL
	dd      0                       ; 6Ch   INS
	dd      0                       ; 6Dh   INS
	dd      0                       ; 6Eh   OUTS
	dd      0                       ; 6Fh   OUTS
	dd      0                       ; 70h   JO
	dd      0                       ; 71h   JNO
	dd      0                       ; 72h   JB
	dd      0                       ; 73h   JNB
	dd      0                       ; 74h   JZ
	dd      0                       ; 75h   JNZ
	dd      0                       ; 76h   JBE
	dd      0                       ; 77h   JNBE
	dd      0                       ; 78h   JS
	dd      0                       ; 79h   JNS
	dd      0                       ; 7Ah   JP
	dd      0                       ; 7Bh   JNP
	dd      0                       ; 7Ch   JL
	dd      0                       ; 7Dh   JGE
	dd      0                       ; 7Eh   JLE
	dd      0                       ; 7Fh   JG
	dd      0                       ; 80h
	dd      0                       ; 81h
	dd      0                       ; 82h
	dd      0                       ; 83h
	dd      0                       ; 84h   TEST
	dd      0                       ; 85h   TEST
	dd      0                       ; 86h   XCHG
	dd      0                       ; 87h   XCHG
	dd      0                       ; 88h   MOV
	dd      0                       ; 89h   MOV
	dd      0                       ; 8Ah   MOV
	dd      0                       ; 8Bh   MOV
	dd      0                       ; 8Ch   MOV
	dd      0                       ; 8Dh   LEA
	dd      0                       ; 8Eh   MOV
	dd      0                       ; 8Fh   POP
	dd      0                       ; 90h   NOP
	dd      0                       ; 91h   XCHG
	dd      0                       ; 92h   XCHG
	dd      0                       ; 93h   XCHG
	dd      0                       ; 94h   XCHG
	dd      0                       ; 95h   XCHG
	dd      0                       ; 96h   XCHG
	dd      0                       ; 97h   XCHG
	dd      0                       ; 98h   CBW
	dd      0                       ; 99h   CWD
	dd      0                       ; 9Ah   CALL
	dd      0                       ; 9Bh   WAIT
	dd      0                       ; 9Ch   PUSHF
	dd      0                       ; 9Dh   POPF
	dd      0                       ; 9Eh   SAHF
	dd      0                       ; 9Fh   LAHF
	dd      0                       ; A0h   MOV
	dd      0                       ; A1h   MOV
	dd      0                       ; A2h   MOV
	dd      0                       ; A3h   MOV
	dd      0                       ; A4h   MOVSB
	dd      0                       ; A5h   MOVSW
	dd      0                       ; A6h   CMPSB
	dd      0                       ; A7h   CMPSW
	dd      0                       ; A8h   TEST
	dd      0                       ; A9h   TEST
	dd      0                       ; AAh   STOSB
	dd      0                       ; ABh   STOSW
	dd      0                       ; ACh   LODSB
	dd      0                       ; ADh   LODSW
	dd      0                       ; AEh   SCASB
	dd      0                       ; AFh   SCASW
	dd      0                       ; B0h   MOV
	dd      0                       ; B1h   MOV
	dd      0                       ; B2h   MOV
	dd      0                       ; B3h   MOV
	dd      0                       ; B4h   MOV
	dd      0                       ; B5h   MOV
	dd      0                       ; B6h   MOV
	dd      0                       ; B7h   MOV
	dd      0                       ; B8h   MOV
	dd      0                       ; B9h   MOV
	dd      0                       ; BAh   MOV
	dd      0                       ; BBh   MOV
	dd      0                       ; BCh   MOV
	dd      0                       ; BDh   MOV
	dd      0                       ; BEh   MOV
	dd      0                       ; BFh   MOV
	dd      0                       ; C0h
	dd      0                       ; C1h
	dd      0                       ; C2h   RET
	dd      0                       ; C3h   RET
	dd      0                       ; C4h   LES
	dd      0                       ; C5h   LDS
	dd      0                       ; C6h   MOV
	dd      0                       ; C7h   MOV
	dd      0                       ; C8h   ENTER
	dd      0                       ; C9h   LEAVE
	dd      0                       ; CAh   RETF
	dd      0                       ; CBh   RETF
        dd      0                       ; CCh   INT3
        dd      0                       ; CDh   INT
	dd      0                       ; CEh   INTO
	dd      0                       ; CFh   IRET
	dd      0                       ; D0h
	dd      0                       ; D1h
	dd      0                       ; D2h
	dd      0                       ; D3h
	dd      0                       ; D4h   AAM
	dd      0                       ; D5h   AAD
	dd      0                       ; D6h   GBP
	dd      0                       ; D7h   XLAT
	dd      0                       ; D8h   ESC
	dd      0                       ; D9h   ESC
	dd      0                       ; DAh   ESC
	dd      0                       ; DBh   ESC
	dd      0                       ; DCh   ESC
	dd      0                       ; DDh   ESC
	dd      0                       ; DEh   ESC
	dd      0                       ; DFh   ESC
	dd      0                       ; E0h   LOOPNZ
	dd      0                       ; E1h   LOOPZ
	dd      0                       ; E2h   LOOP
	dd      0                       ; E3h   JCXZ
        dd      _LogInALnn              ; E4h   IN
        dd      _LogInAXnn              ; E5h   IN
        dd      _LogOutnnAL             ; E6h   OUT
        dd      _LogOutnnAX             ; E7h   OUT
	dd      0                       ; E8h   CALL
	dd      0                       ; E9h   JMP
	dd      0                       ; EAh   JMP
	dd      0                       ; EBh   JMP
        dd      _LogInALDX              ; ECh   IN AL,DX
        dd      _LogInAXDX              ; EDh   IN AX,DX
        dd      _LogOutDXAL             ; EEh   OUT DX,AL
        dd      _LogOutDXAX             ; EFh   OUT DX,AX
	dd      0                       ; F0h   LOCK
	dd      0                       ; F1h
	dd      0                       ; F2h   REPNZ
	dd      0                       ; F3h   REPZ
	dd      0                       ; F4h   HLT
	dd      0                       ; F5h   CMC
	dd      0                       ; F6h
	dd      0                       ; F7h
	dd      0                       ; F8h   CLC
	dd      0                       ; F9h   STC
	dd      0                       ; FAh   CLI
	dd      0                       ; FBh   STI
	dd      0                       ; FCh   CLD
	dd      0                       ; FDh   STD
	dd      0                       ; FEh
	dd      0                       ; FFh

CLOSE_DATA


OPEN_ICODE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_VMMInit
;
;   ENTRY
;       None
;
;   EXIT
;       None
;
;   USES
;       Any
;

BeginProc SPY_VMMInit

        mov     edi,OFFSET32 TrapTbl

init_vmm:
        movsx   eax,[edi].IDTNum
        or      eax,eax                 ; end of list?
        js      short init_done         ; yes

        mov     esi,[edi].VMMHandler
        or      esi,esi                 ; is there a VMM handler?
        jz      short init_loop         ; no

        cmp     eax,IDT_NMI
        je      short init_nmi
        VMMCall Hook_VMM_Fault
        jmp     short init_save
init_nmi:
        VMMCall Hook_NMI_Event
init_save:                              ; save previous fault handler
        mov     [edi].VMMHandler,esi

init_loop:
        add     edi,size TrapEntry
        jmp     short init_vmm

init_done:
        ret                             ; carry is already clear (from above)

EndProc SPY_VMMInit


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_INTInit
;
;   ENTRY
;       None
;
;   EXIT
;       None
;
;   USES
;       Any
;

BeginProc SPY_INTInit

        mov     edi,OFFSET32 TrapTbl

init_ints:
        movsx   eax,[edi].IDTNum
        or      eax,eax                 ; end of list?
        js      short init_done         ; yes

        cmp     [edi].INTHandler,0
        je      short init_loop         ; no

        mov     edx,[_pIDT]             ; yes, so make sure it and we
        mov     cx,cs                   ; are in the same code segment
        cmp     cx,[edx+eax*size GATE].gate_wSel
        je      short init_ok
        test    [edi].IDTFlags,TRAP_IDT
        jz      short init_loop         ; don't hook it
        xchg    cx,[edx+eax*size GATE].gate_wSel
        mov     [edi].INTSegment,cx     ; INTSegment is non-zero now

init_ok:
        mov     cx,[edx+eax*size GATE].gate_wBase16_31
        rol     ecx,16
        mov     cx,[edx+eax*size GATE].gate_wBase0_15
        xchg    [edi].INTHandler,ecx

        mov     [edx+eax*size GATE].gate_wBase0_15,cx
        rol     ecx,16
        mov     [edx+eax*size GATE].gate_wBase16_31,cx

init_loop:
        add     edi,size TrapEntry
        jmp     short init_ints

init_done:
        mov     edi,OFFSET32 HookTbl

init_hooks:
        mov     eax,[edi].HookServ
        cmp     eax,-1
        je      short init_exit
        mov     esi,[edi].HookHandler
        VMMCall Hook_Device_Service
        mov     [edi].HookHandler,esi
        add     edi,size HookEntry
        jmp     init_hooks

init_exit:
        mov     esi,OFFSET32 SPYHOOK_Task_Switch
        VMMCall Call_When_Task_Switched

        clc
        ret

EndProc SPY_INTInit

CLOSE_ICODE


OPEN_CODE

        extrn   SPY_RegInit:near        ; in spyinit.asm
	extrn   _x86Trap:near           ; in x86debug.c

;
;   Entry conditions for all VMM fault handlers are:
;
;       EBX == current VM
;       EBP -> VMM stack frame
;
;   Before we can dispatch to our debugger, we need to create a new
;   frame, call, and then reflect any changes back to the original frame.
;

        public  SPY_VMMDivError
SPY_VMMDivError     label   near
        mov     al,IDT_DIVERROR
        jmp     short SPY_VMMFault

        public  SPY_VMMDebug
SPY_VMMDebug        label   near
        mov     al,IDT_DEBUG
        jmp     short SPY_VMMFault

        public  SPY_VMMNMI
SPY_VMMNMI          label   near
        mov     al,IDT_NMI
        jmp     short SPY_VMMFault

        public  SPY_VMMBreakPoint
SPY_VMMBreakPoint   label   near
        mov     al,IDT_BREAKPOINT
        jmp     short SPY_VMMFault

        public  SPY_VMMOverflow
SPY_VMMOverflow     label   near
        mov     al,IDT_OVERFLOW
        jmp     short SPY_VMMFault

        public  SPY_VMMBounds
SPY_VMMBounds       label   near
        mov     al,IDT_BOUNDS
        jmp     short SPY_VMMFault

        public  SPY_VMMInvOpCode
SPY_VMMInvOpCode    label   near
        mov     al,IDT_INVOPCODE
        jmp     short SPY_VMMFault

        public  SPY_VMMNoCP
SPY_VMMNoCP         label   near
        mov     al,IDT_NOCP
        jmp     short SPY_VMMFault

        public  SPY_VMMDblFault
SPY_VMMDblFault     label   near
        mov     al,IDT_DBLFAULT
        jmp     short SPY_VMMFault

        public  SPY_VMMTSSFault
SPY_VMMTSSFault     label   near
        mov     al,IDT_TSSFAULT
        jmp     short SPY_VMMFault

        public  SPY_VMMSegFault
SPY_VMMSegFault     label   near
        mov     al,IDT_SEGFAULT
        jmp     short SPY_VMMFault

        public  SPY_VMMStackFault
SPY_VMMStackFault   label   near
        mov     al,IDT_STACKFAULT
        jmp     short SPY_VMMFault

        public  SPY_VMMGPFault
SPY_VMMGPFault      label   near
        mov     al,IDT_GPFAULT
        jmp     short SPY_VMMFault

        public  SPY_VMMPageFault
SPY_VMMPageFault    label   near
        mov     al,IDT_PAGEFAULT
        jmp     short SPY_VMMFault

        public  SPY_VMMCPFault
SPY_VMMCPFault      label   near
        mov     al,IDT_CPFAULT
        jmp     short SPY_VMMFault


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_VMMFault
;
;   ENTRY
;       AL  == fault #
;       EBX == current VM
;       EBP -> VMM stack frame
;
;   EXIT
;       None
;
;   USES
;       All but EBP
;

        public  SPY_VMMFault
SPY_VMMFault proc   near
;
;   Before we go the whole nine yards with this, see if it's a GP fault
;   in our own get or set byte function, and if so, skip over the instruction
;
        cmp     al,IDT_GPFAULT
        jne     short vmmf_notsegfault

        cmp     [ebp].Client_EIP,OFFSET32 SPY_SegRead
        jne     short vmmf_notsegread
        mov     [ebp].Client_EIP,OFFSET32 SPY_SegReadSkip
        ret                             ; all done

vmmf_notsegread:
        cmp     [ebp].Client_EIP,OFFSET32 SPY_SegWrite
        jne     short vmmf_notsegfault
        mov     [ebp].Client_EIP,OFFSET32 SPY_SegWriteSkip
        ret                             ; all done

vmmf_notsegfault:
;
;   Ditto for page faults...
;
        cmp     al,IDT_PAGEFAULT
        jne     short vmmf_notpagefault

        cmp     [ebp].Client_EIP,OFFSET32 SPY_PageRead
        jne     short vmmf_notpageread
        mov     [ebp].Client_EIP,OFFSET32 SPY_PageReadSkip
        ret                             ; all done

vmmf_notpageread:
        cmp     [ebp].Client_EIP,OFFSET32 SPY_PageWrite
        jne     short vmmf_notpagefault
        mov     [ebp].Client_EIP,OFFSET32 SPY_PageWriteSkip
        ret                             ; all done
;
;   Go the whole nine yards...
;
vmmf_notpagefault:

        call    SPY_DebugVMM

        test    [_flTrace],TRACE_REEXEC ; is re-exec set?
        jnz     short @F                ; yes, assume we fixed it up
        .errnz  size TrapEntry NE 12
        lea     ecx,[eax*4]
        mov     ecx,[TrapTbl][ecx+eax*8].VMMHandler
        jecxz   short @F
        push    ecx
@@:
        ret                             ; pretend we processed the fault

SPY_VMMFault endp


;
;   Entry conditions for all IDT fault handlers are:
;
;       CS == CODE_SEG
;       SS == DATA_SEG
;
;   and that's it!
;

        public  SPY_INTDivError
SPY_INTDivError     label   near
	push    0                       ; dummy iErrCode
        push    IDT_DIVERROR            ; iTrap
        jmp     short SPY_IDTFault

        public  SPY_INTDebug
SPY_INTDebug        label   near
        push    0
        push    IDT_DEBUG
        jmp     short SPY_IDTFault

        public  SPY_INTNMI
SPY_INTNMI          label   near
	ResetNMI
        push    0
        push    IDT_NMI
        jmp     short SPY_IDTFault

        public  SPY_INTBreakPoint
SPY_INTBreakPoint   label   near
        push    0
        push    IDT_BREAKPOINT
        jmp     short SPY_IDTFault

        public  SPY_INTOverflow
SPY_INTOverflow     label   near
        push    0
        push    IDT_OVERFLOW
        jmp     short SPY_IDTFault

        public  SPY_INTBounds
SPY_INTBounds       label   near
        push    0
        push    IDT_BOUNDS
        jmp     short SPY_IDTFault

        public  SPY_INTInvOpCode
SPY_INTInvOpCode    label   near
        push    0
        push    IDT_INVOPCODE
        jmp     short SPY_IDTFault

        public  SPY_INTNoCP
SPY_INTNoCP         label   near
        push    0
        push    IDT_NOCP
        jmp     short SPY_IDTFault

        public  SPY_INTDblFault
SPY_INTDblFault     label   near
        push    IDT_DBLFAULT
        jmp     short SPY_IDTFault

        public  SPY_INTTSSFault
SPY_INTTSSFault     label   near
        push    IDT_TSSFAULT
        jmp     short SPY_IDTFault

        public  SPY_INTSegFault
SPY_INTSegFault     label   near
        push    IDT_SEGFAULT
        jmp     short SPY_IDTFault

        public  SPY_INTStackFault
SPY_INTStackFault   label   near
        push    IDT_STACKFAULT
        jmp     short SPY_IDTFault

        public  SPY_INTGPFault
SPY_INTGPFault      label   near
        push    IDT_GPFAULT
        jmp     short SPY_IDTFault

        public  SPY_INTPageFault
SPY_INTPageFault    label   near
        push    IDT_PAGEFAULT
        jmp     short SPY_IDTFault

        public  SPY_INTCPFault
SPY_INTCPFault      label   near
        push    IDT_CPFAULT
        jmp     short SPY_IDTFault

        public  SPY_INTDebugReq
SPY_INTDebugReq     label   near
        push    0
        push    IDT_DEBUGREQ
        jmp     short SPY_IDTFault


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_IDTFault
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

        public  SPY_IDTFault
SPY_IDTFault  proc near

        cld
	pusha
	push    ds
	push    es
        sub     esp,esf_saveES          ; reserve space for the rest
	mov     ebp,esp                 ; EBP -> ESF
        mov     [ebp].esf_hVM,0         ; we don't know the hVM yet

        mov     eax,ss
        mov     ds,eax
        mov     es,eax
        assume  ds:DATA_SEG, es:DATA_SEG

        mov     eax,[ebp].esf_iTrap

        cmp     al,IDT_DEBUGREQ         ; debugger request?
        jne     short intf_chkidt       ; no

        call    SPY_DebugReq
        jnc     short intf_reexec       ; we handled it, so just return
        jmp     short intf_passthru     ; we didn't handle it, so pass it on

intf_chkidt:
        test    [_flTrace],TRACE_IDTOFF
        jnz     short intf_passthru

        cmp     al,IDT_DEBUG            ; trace interrupt?
        je      short intf_debugger     ; yes

        cmp     al,IDT_BREAKPOINT       ; breakpoint interrupt?
        jne     short intf_passthru     ; yes

intf_debugger:
        push    DEBUG_INT
        call    SPY_Debug               ; route to debugger
        test    [_flTrace],TRACE_REEXEC
        jz      short intf_passthru

intf_reexec:
        add     esp,esf_saveES
	pop     es
	pop     ds
	popa
        add     esp,isf_EIP             ; throw away both iTrap and iErrCode
        iretd

intf_passthru:
        cmp     al,IDT_GPFAULT          ; interesting VM event?
        jne     short intf_skiplog      ; no

	test    [ebp].esf_Flags,FLAGS_V86
        jz      short intf_skiplog      ; it didn't happen in a VM!

	mov     ebx,[_pmbZero]
        movzx   esi,word ptr [ebp].esf_CS
	shl     esi,4
	add     esi,[ebp].esf_EIP
	movzx   edi,byte ptr [ebx+esi]  ; EDI == opcode byte
        mov     ecx,_afnLogging[edi*4]
        jecxz   short intf_skiplog      ; no logging function exists

        test    [_flTrace],TRACE_TRAPIO ; BUGBUG -- I must first determine
        jz      short intf_notrap       ; whether or not this is an I/O ins.
        push    DEBUG_INT or DEBUG_VIO  ; it just so happens that currently
        call    SPY_Debug               ; all I log are I/O instructions....
        test    [_flTrace],TRACE_REEXEC
        jnz     short intf_reexec

intf_notrap:
        push    eax                     ; protect EAX from Logxxxx
        call    SPY_SwitchStacks        ; need I say more?
        VMMCall Get_Cur_VM_Handle       ; EBX == current VM handle
        mov     [ebp].esf_hVM,ebx       ; save it in the frame
        cCall   ecx,<ebp>               ; call Logxxxx(pesf)
        pop     esp                     ; this complements SPY_SwitchStacks
        pop     eax                     ; restore EAX

intf_skiplog:                           ; EAX must still contain IDT #
        cmp     al,IDT_CPFAULT          ; in range of interest?
        ja      short intf_skipcount    ; no
        inc     [INTFaultCounts][eax*4] ; record pass-thru interrupt activity

intf_skipcount:
        .errnz  size TrapEntry NE 12
        lea     edx,[eax*4]
        movzx   ecx,[TrapTbl][edx+eax*8].INTSegment
        mov     edx,[TrapTbl][edx+eax*8].INTHandler
;
;   Warning: this code currently cannot handle passing on an
;   interrupt that generated an error code to a different segment;
;   for the inter-segment return, I use both iErrCode and iTrap.
;
        jecxz   short intf_sameseg      ; no problem
        mov     [ebp].esf_iTrap,edx     ; set iTrap to offset
        mov     [ebp].esf_iErrCode,ecx  ; set iErrCode to segment
	add     esp,esf_saveES          ; throw away the rest
	pop     es
	pop     ds
	popa
        retf

intf_sameseg:
        cmp     al,IDT_DBLFAULT
        jae     short intf_donteat

        mov     [ebp].esf_iErrCode,edx  ; set iErrCode to next handler
	add     esp,esf_saveES          ; throw away the rest
	pop     es
	pop     ds
	popa
        add     esp,isf_iErrCode        ; throw away iTrap but not iErrCode
        ret                             ; return through iErrCode

intf_donteat:
        mov     [ebp].esf_iTrap,edx     ; set iTrap to next handler
	add     esp,esf_saveES          ; throw away the rest
	pop     es
	pop     ds
	popa
        ret                             ; return through iTrap

SPY_IDTFault endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_Debug
;
;   ENTRY
;       EBP -> ESF
;       DEBUG_* flags on stack
;       Interrupts off
;
;   EXIT
;       None
;
;   USES
;       None
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

        public  SPY_Debug
SPY_Debug proc near
	push    eax                     ; save scratch regs
	push    edx                     ;

	in      al,PIC_MASTER_IMR
	mov     dl,al                   ; save master interrupt mask in DL
        mov     al,not (1 shl IRQ_KBD)
	IODelay
        out     PIC_MASTER_IMR,al       ; enable only keyboard ints
        in      al,PIC_SLAVE_IMR
	mov     dh,al                   ; save slave interrupt mask in DH
	mov     al,0FFh
	IODelay
        out     PIC_SLAVE_IMR,al

        test    [_flTrace],TRACE_MASKINTS
        jz      short no_mask_ints
        mov     edx,[SavedIMRs]
no_mask_ints:
        mov     [ebp].esf_dwIMRs,edx    ; EDX == saved IMRs

	ReadISR PIC_MASTER              ; EAX == master ISR register

        push    eax
	test    al,1 shl IRQ_TMR        ; timer interrupt in service?
        jz      short no_tmr_int        ; no
	mov     al,PIC_EOI              ; yes, we have to issue EOI to
        IODelay                         ; allow further timer interrupts
        out     PIC_MASTER,al           ;
no_tmr_int:
        pop     eax

        test    al,1 shl IRQ_KBD        ; previous interrupt in service?
        jz      short no_kbd_int        ; no
        mov     al,PIC_EOI              ; yes, we have to issue EOI to
	IODelay                         ; allow further keyboard interrupts
	out     PIC_MASTER,al           ;
no_kbd_int:                             ;

	push    ecx                     ; save C-trashable regs
        mov     ecx,[esp+10h]           ; ECX == flDebug pushed by caller
        call    SPY_SwitchStacks        ; need I say more?
	push    edi                     ; push nn
        push    ecx                     ; push flDebug
	push    ebp                     ; push PESF
	call    _x86Trap                ; call debugger
	add     esp,12                  ; clean up the stack
        pop     esp                     ; this complements SPY_SwitchStacks
        pop     ecx                     ; restore C-trashable regs

        mov     edx,[ebp].esf_dwIMRs
        test    [_flTrace],TRACE_MASKINTS
        jz      short no_mask_ints2
        mov     [SavedIMRs],edx
        sub     edx,edx
        dec     edx                     ; mask everything (EDX == -1)
no_mask_ints2:

	mov     al,dh                   ; restore slave interrupt mask
	out     PIC_SLAVE_IMR,al        ; done!
	mov     al,dl                   ; restore master interrupt mask
	out     PIC_MASTER_IMR,al       ; done!

	pop     edx                     ; restore scratch regs
        pop     eax                     ;
					; throw away DEBUG_* flags
	ret     4                       ; because this is a PASCAL function
SPY_Debug endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_DebugReq
;
;   Intercepts IDT_DEBUGREQ (INT 41h) requests (see debugsys.inc for list)
;
;   ENTRY
;       EAX == 41h
;       EBP -> ESF
;       Interrupts off
;
;   EXIT
;       Carry clear if processed, set if not (and EAX must still be iTrap)
;
;   USES
;       Any
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

        public  SPY_DebugReq
SPY_DebugReq proc near
        push    eax
        call    SPY_SwitchStacks        ; need I say more?

        mov     eax,[ebp].esf_EAX

        cmp     eax,DS_Out_Char
        jne     short sdr_ignore
        cCall   __putch,<edx>
        jmp     short sdr_processed

sdr_ignore:
        stc
        jmp     short sdr_exit

sdr_processed:
        clc
sdr_exit:
        pop     esp                     ; this complements SPY_SwitchStacks
        pop     eax
        ret
SPY_DebugReq endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPYHook_In_Debug_Chr
;
;   ENTRY
;       None
;
;   EXIT
;       AL == char, ZF set if Ctrl-C or Esc
;
;   USES
;       EAX, Flags
;
BeginProc SPYHook_In_Debug_Chr, Async_Service, No_Prolog

        push    ecx
        push    edx
        cCall   __getch
        pop     edx
        pop     ecx
        cmp     al,ESCAPE               ; kbd services map both to the same
        ret

EndProc SPYHook_In_Debug_Chr


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPYHook_Is_Debug_Chr
;
;   ENTRY
;       None
;
;   EXIT
;       ZF set if no char, else AL == char
;
;   USES
;       EAX, Flags
;
BeginProc SPYHook_Is_Debug_Chr, Async_Service, No_Prolog

        push    ecx
        push    edx
        cCall   __kbhit
        pop     edx
        pop     ecx
        or      al,al
        ret

EndProc SPYHook_Is_Debug_Chr


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPYHook_Out_Debug_Chr
;
;   ENTRY
;       AL == char
;
;   EXIT
;       None
;
;   USES
;       None
;
BeginProc SPYHook_Out_Debug_Chr, Async_Service, No_Prolog

        push    eax
        push    ecx
        push    edx
        cCall   __putch,<eax>
        pop     edx
        pop     ecx
        pop     eax

        ret
;
;   The problem with passing this call through as well is that if we're
;   running on a DEBUG system and our attempt to hook Out_Debug_Chr
;   results in a debug assertion being printed *after* Hook_Device_Service
;   has set up our hook but *before* it has returned to us its own
;   address, we'll end up calling ourselves endlessly.  One solution is
;   to have separate fields in the HookTbl for my function/their function
;   addresses, but that's a big waste to solve such a stupid problem.
;
;;;     jmp     [HookTbl+HOOK_OUT_DEBUG_CHR*size HookEntry].HookHandler

EndProc SPYHook_Out_Debug_Chr


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPYHook_Task_Switch
;
;   ENTRY
;       EAX == Old VM
;       EBX == New VM
;
;   EXIT
;       None
;
;   USES
;       None
;
BeginProc SPYHook_Task_Switch, High_Freq

        cmp     [_hVMSwitch],0          ; is task-switch watchpoint enabled?
        jne     short ts_check          ; yes
        ret                             ; no, just return (in-line for speed)
ts_check:
        cmp     ebx,[_hVMSwitch]        ; match?
        jne     short ts_return         ; no
        mov     [_hVMSwitch],0          ; clear it (this is a one-shot thing)

        push    ebp
        mov     ebp,[ebx].CB_Client_Pointer
        mov     al,-1                   ; BUGBUG -- Need #defines for these events
        call    SPY_DebugVMM            ; route to debugger
        pop     ebp

ts_return:
        ret

EndProc SPYHook_Task_Switch


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_SwitchStacks
;
;   Switches to private stack as appropriate.  To restore previous stack
;   context, you don't need to call a function.  Just pop esp.
;
        public  SPY_SwitchStacks
SPY_SwitchStacks proc near

        call    SPY_RegInit             ; this is also a good idea...
        push    eax
        lea     eax,[esp+8]             ; EAX -> old stack
        cmp     eax,[_pDebugStack]      ; already running on my stack?
        jb      short switch            ; no, switch
        cmp     eax,[_pDebugStackTop]   ; already running on my stack?
        jb      short no_switch         ; yes, don't switch
switch:
        mov     esp,[_pDebugStackTop]   ; load new ESP
        push    eax                     ; save old stack ptr on new stack
        push    [eax-4]                 ; where to return
        xchg    eax,[eax-8]             ; get back EAX as pushed above
        ret                             ; return, with no regs modified

no_switch:
        xchg    eax,[esp+4]             ; EAX == ret address
        xchg    eax,[esp]               ; EAX is restored, ret address moved
        ret                             ; return, with no regs modified

SPY_SwitchStacks endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   SPY_DebugVMM
;
;   Creates the standard SPY frame from the standard VMM frame (EBP).
;   AL contains the fault # to stuff in the frame, and EBX contains the
;   current VM handle.
;
;   Uses EDX and EDI.
;
        public  SPY_DebugVMM
SPY_DebugVMM    proc near

        push    dword ptr [ebp].Client_GS
        push    dword ptr [ebp].Client_FS
        push    dword ptr [ebp].Client_DS
        push    dword ptr [ebp].Client_ES
        push    dword ptr [ebp].Client_SS
        push    [ebp].Client_ESP
        push    [ebp].Client_EFlags
        push    dword ptr [ebp].Client_CS
        push    [ebp].Client_EIP
        push    [ebp].Client_Error
        movzx   eax,al
        push    eax                     ; push iTrap
        push    [ebp].Client_EAX
        push    [ebp].Client_ECX
        push    [ebp].Client_EDX
        push    [ebp].Client_EBX
        mov     edx,[ebp].Client_res0   ; BUGBUG -- this is very dependent
        sub     edx,4                   ; on the fact that VMM's frame is just
        push    edx                     ; like mine, except for iTrap
        push    [ebp].Client_EBP
        push    [ebp].Client_ESI
        push    [ebp].Client_EDI
	push    ds
	push    es
	sub     esp,esf_saveES          ; reserve space for the rest

        mov     edi,ebp                 ; preserve EBP in non-volatile reg.
	mov     ebp,esp                 ; EBP -> ESF
        mov     [ebp].esf_hVM,ebx       ; save current VM from EBX
        push    DEBUG_VMM
        call    SPY_Debug               ; route to debugger
        mov     ebp,edi                 ; restore EBP from non-volatile reg.

	add     esp,esf_saveES          ; throw away the rest
	pop     es
	pop     ds
        pop     [ebp].Client_EDI
        pop     [ebp].Client_ESI
        pop     [ebp].Client_EBP
        pop     [ebp].Client_res0       ; BUGBUG -- this doesn't change ESP!
        pop     [ebp].Client_EBX
        pop     [ebp].Client_EDX
        pop     [ebp].Client_ECX
        pop     [ebp].Client_EAX
        add     esp,isf_EIP             ; throw away iTrap and iErrCode
        pop     [ebp].Client_EIP
        pop     dword ptr [ebp].Client_CS
        pop     [ebp].Client_EFlags
        pop     [ebp].Client_ESP
        pop     dword ptr [ebp].Client_SS
        pop     dword ptr [ebp].Client_ES
        pop     dword ptr [ebp].Client_DS
        pop     dword ptr [ebp].Client_FS
        pop     dword ptr [ebp].Client_GS
        ret

SPY_DebugVMM endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   _TrapGetByte
;   _TrapSetByte
;
;   Central routines for reading/writing random memory, so that if a fault
;   occurs, SPY knows how to fix itself.
;
;
        public  SPY_SegRead
        public  SPY_PageRead
        public  SPY_PageReadSkip
        public  SPY_SegReadSkip

        public  SPY_SegWrite
        public  SPY_PageWrite
        public  SPY_PageWriteSkip
        public  SPY_SegWriteSkip


        public  _TrapGetByte            ; sel = [ebp+8], off = [ebp+12]
_TrapGetByte proc near
        push    ebp
        mov     ebp,esp

        push    es
        mov     eax,0EEh                ; return this in case we fault
SPY_SegRead  label near
        mov     es,[ebp+8]
        mov     edx,[ebp+12]
SPY_PageRead label near
        movzx   eax,byte ptr es:[edx]   ; return byte in AL (clear the rest to be safe)
SPY_PageReadSkip label near
SPY_SegReadSkip  label near
        pop     es

        pop     ebp
        ret
_TrapGetByte endp


        public  _TrapSetByte            ; sel = [ebp+8], off = [ebp+12], b = [ebp+16]
_TrapSetByte proc near
        push    ebp
        mov     ebp,esp

        push    es
SPY_SegWrite  label near
        mov     es,[ebp+8]
        mov     edx,[ebp+12]
        mov     al,[ebp+16]
SPY_PageWrite label near
        mov     byte ptr es:[edx],al    ; set byte in AL
SPY_PageWriteSkip label near
SPY_SegWriteSkip  label near
        pop     es

        pop     ebp
        ret
_TrapSetByte endp


CLOSE_CODE


	end
