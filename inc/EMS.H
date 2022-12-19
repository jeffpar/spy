/*
 *  EMS Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define INT_EMS                 0x67


/* ASM

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

*/
