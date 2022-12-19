        page    ,132
        title   V86INT.ASM

;
;   V86INT.ASM
;   V86-mode interrupt support
;
;   by Jeff Parsons 05-Jun-1992
;   with a little help from Intel's 386 Programmer's Reference Manual :)
;


        include all.inc


OPEN_CODE

        extrn   Debugger:near           ; in 386trap.asm


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   EmulateINT
;
;   ENTRY
;       EBX -> megabyte alias (eg, pmbZero)
;       ESI -> current instruction
;       EBP -> ESF with valid v86* fields
;       Interrupts disabled (so don't dink around!)
;
;   EXIT
;       Nothing in particular
;
;   USES
;       Anything it pleases
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

        public  EmulateINT
EmulateINT      proc near

        movzx   edi,byte ptr [ebx+esi+1]; EDI == nn byte of the INT nn
        push    DEBUG_VINT
        call    Debugger
        test    [_flTrace],TRACE_REEXEC
        jnz     short EmExit

        add     word ptr [ebp].esf_EIP,2; advance instruction pointer now
;
;   BUGBUG: Until I implement support for VM stubs and VM breakpoints,
;   I have to have these hacks to fake out certain calls.  Other than
;   being slow and ugly, this code also suffers from being incompatible,
;   because the v86-mode hook chain, if any, doesn't get a chance to run.
;
;   Hack #1: INT 15h, function AH=88h (Read Extended Memory Size) needs to
;   return 0 so that no one will try switching to protected-mode to access
;   it.
;
        cmp     edi,INT_SYSSERV         ; INT 15h? (System Services)
        jne     short SimulateINT       ; nope
        cmp     byte ptr [ebp].esf_EAX+1,INT15AH_EXTMEM
        jne     short SimulateINT       ;
        mov     word ptr [ebp].esf_EAX,0; return 0Kb extended memory size
EmExit:
        ret                             ; fake emulation complete!


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   EmulateINT3
;
;   ENTRY
;       EBX -> megabyte alias (eg, pmbZero)
;       ESI -> current instruction
;       EBP -> ESF with valid v86* fields
;       Interrupts disabled (so don't dink around!)
;
;   EXIT
;       Nothing in particular
;
;   USES
;       Anything it pleases
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

        public  EmulateINT3
EmulateINT3 label near

        push    DEBUG_VINT3
        call    Debugger
        test    [_flTrace],TRACE_REEXEC
        jnz     short EmExit

        inc     word ptr [ebp].esf_EIP  ; advance instruction pointer now

        ; Fall into SimulateINT


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   SimulateINT
;
;   ENTRY
;       EBX -> megabyte alias (eg, pmbZero)
;       EDI == IVT interrupt #
;       EBP -> ESF with valid v86* fields
;       Interrupts disabled (so don't dink around!)
;
        public  SimulateINT
SimulateINT label near

        mov     eax,[ebp].esf_v86ESP    ;
        sub     ax,size VIRF            ; check for stack wrap
        jb      short intem_wrap        ; yup, it's going to wrap; what a pain

intem_cont:
        mov     [ebp].esf_v86ESP,eax    ; update VM's SP

        movzx   esi,word ptr [ebp].esf_v86SS
        shl     esi,4                   ;
        add     esi,eax                 ; ESI -> place to put VIRF

        mov     eax,[ebp].esf_EIP       ; create VIRF now
        mov     [ebx+esi].virf_IP,ax    ;
        mov     eax,[ebp].esf_CS        ;
        mov     [ebx+esi].virf_CS,ax    ;
        mov     eax,[ebp].esf_Flags     ;
        mov     [ebx+esi].virf_Flags,ax ; VIRF creation complete

        IFDEF   DEBUG
        and     eax,NOT (FLAGS_IF)
        cmp     [ebp].esf_iTrap,IDT_MASTER_BASE
        jb      short @F
        and     eax,NOT (FLAGS_TF)      ; on h/w int simulation, clear TF too
@@:
        ELSE
        and     eax,NOT (FLAGS_IF or FLAGS_TF)
        ENDIF
        mov     [ebp].esf_Flags,eax     ;

        mov     eax,[ebx+edi*4]         ; EAX == IVT vector address
        mov     word ptr [ebp].esf_EIP,ax
        ror     eax,16                  ;
        mov     word ptr [ebp].esf_CS,ax; CS:IP updated

        ret                             ; return to GPFault in 386trap.asm

intem_wrap:
        cmp     ax,-(size VIRF)         ; determine if initial SP was zero
        je      intem_cont              ; yes, it was

        int     3                       ; BUGBUG
        jmp     intem_cont              ;

EmulateINT      endp


CLOSE_CODE


        end
