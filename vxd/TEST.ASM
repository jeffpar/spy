        title   386 instruction tests

        org     100h
        assume  cs:code, ds:code, es:code, ss:code

code    segment

main    proc    near
        .386p
        mov     ebx,10h
        mov     ecx,100h
        test    [ebx+ecx*4+12345678h],edx
        mov     cr0,eax
        mov     cr0,ecx
        mov     eax,cr0
        mov     ecx,cr0
        mov     dr0,eax
        mov     dr0,ecx
        mov     eax,dr0
        mov     ecx,dr0
        mov     tr7,eax
        mov     tr7,ecx
        mov     eax,tr7
        mov     ecx,tr7
        .8086
        ret
main    endp

code    ends

        end     main
