INT_EMS	EQU	67H

EMSOff  macro
        push    ax
        pushf
        cli
        mov     al,0Fh
        out     84h,al
        mov     al,0
        out     85h,al
        jmp     $+2
        popf
        pop     ax
endm


