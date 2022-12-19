/*
 *  Debugging Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


/* ASM

ScrOff  macro
        mov     dx,3DAh         ;; assume color VGA
        in      al,dx           ;; reset attribute controller
        mov     dx,3C0h         ;; point to attribute controller
        sub     al,al           ;; 0 index disables, 20h re-enables
        out     dx,al           ;; screen should be blank now
endm

DispSz  macro   szMsg,r,c,n     ;; destroys AL, ECX, EDX, ESI, EDI
        local   l1,l2,l3,l4
        ifnb    <n>
        mov     edx,n
        endif
        mov     esi,offset szMsg
        mov     edi,[_pmbZero]
        add     edi,mb_abTextMem+(r-1)*160+(c-1)*2
l1:     lodsb
        or      al,al
        jz      l2
        stosb
        inc     edi             ;; skip attr bytes in video memory
        jmp     l1
l2:
        ifnb    <n>
        mov     ecx,8
l3:     rol     edx,4
        mov     al,dl
        and     al,0Fh
        add     al,'0'
        cmp     al,'9'
        jbe     l4
        add     al,'A'-'0'-10
l4:     stosb
        inc     edi
        loop    l3
        endif
endm

*/
