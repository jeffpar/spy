
ScrOff  macro
        mov     dx,3DAh
        in      al,dx
        mov     dx,3C0h
        sub     al,al
        out     dx,al
endm

DispSz  macro   szMsg,r,c,n
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
        inc     edi
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


