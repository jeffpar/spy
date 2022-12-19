        page    ,132
        title   DOSSERV.ASM

;
;   DOSSERV.ASM
;   DOS Services
;
;   by Jeff Parsons 30-Apr-93
;


        include all.inc


OPEN_DATA

        extrn   _iIDTEntry:dword
        extrn   _pDebugStack:dword
        extrn   _pDebugStackTop:dword

CLOSE_DATA


OPEN_CODE


ifdef   WIN386

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   DosOpen
;
;   ENTRY
;       pszFile -> filename
;       flMode == access and sharing mode flags
;
;   EXIT
;       DOS file handle, or negative error code
;
;   USES
;       Only EAX, ECX and EDX
;
;   NOTES
;       This function uses real-mode 60:0 through 60:FF as a translation
;       buffer.
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

BeginProc DosOpen, CCall

        ArgVar  pszFile,DWORD
        ArgVar  flMode,DWORD

        call    SaveState               ; EBX -> client registers
                                        ; does not return if VMM re-entered
        mov     eax,[_pmbZero]
        lea     eax,[eax].rb_ScratchStack
        cCall   _nstrcpy,<eax,pszFile>

        mov     [ebx].Client_DS,(rb_ScratchStack shr SEG_SHIFT)
        mov     [ebx].Client_DX,(rb_ScratchStack and 000Fh)
        mov     eax,[flMode]
        mov     ah,DOS_OPEN
        mov     [ebx].Client_AX,AX
        call    NestedDOSInt
        movzx   eax,[ebx].Client_AX
        test    [ebx].Client_Flags,FLAGS_CF
        jz      short open_success
        neg     eax                     ; negify return code to indicate error

open_success:
        jmp     RestoreState

EndProc DosOpen,NOCHECK


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   DosRead
;
;   ENTRY
;       hFile  == DOS file handle
;       cbRead == # of bytes to read
;       pBuff  -> buffer
;
;   EXIT
;       # of bytes read, or negative error code
;
;   USES
;       Only EAX, ECX and EDX
;
;   NOTES
;       This function uses real-mode 60:0 through 60:FF as a translation
;       buffer.
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

BeginProc DosRead, CCall

        ArgVar  hFile,DWORD
        ArgVar  cbRead,DWORD
        ArgVar  pBuff,DWORD

        call    SaveState               ; EBX -> client registers
                                        ; does not return if VMM re-entered
        mov     eax,[hFile]
        mov     [ebx].Client_BX,ax
        mov     [ebx].Client_DS,(rb_ScratchStack shr SEG_SHIFT)
        mov     [ebx].Client_DX,(rb_ScratchStack and 000Fh)

        mov     edi,[pBuff]             ; EDI -> current output
        mov     esi,[cbRead]            ; ESI == # bytes
        mov     edx,esi
        shr     edx,8                   ; EDX == # of 256-byte chunks
        or      edx,edx                 ; any whole chunks?
        jz      short read_last_chunk   ; no

read_loop:
        mov     ecx,256                 ; # bytes to read
read_last:
        mov     [ebx].Client_CX,cx
        mov     [ebx].Client_AH,DOS_READ
        call    NestedDOSInt
        movzx   eax,[ebx].Client_AX
        test    [ebx].Client_Flags,FLAGS_CF
        jz      short read_copy
        neg     eax                     ; negify return code to indicate error
        jmp     short read_finish

read_copy:
        push    ecx                     ; save requested for THIS read
        push    edx                     ; save chunk count
        mov     edx,[_pmbZero]
        lea     edx,[edx].rb_ScratchStack
        cCall   _nmemcpy,<edi,edx,eax>
        pop     edx                     ; restore chunk count
        pop     ecx                     ; restore requested for THIS read

        add     edi,eax                 ; update current output pos.
        cmp     eax,ecx                 ; did we read all we requested?
        jb      short read_success      ; no, we're all done

        dec     edx                     ; decrement chunk total
        jnz     read_loop               ; continue reading chunks

read_last_chunk:
        and     esi,256-1               ; any fractional 256 to do?
        jz      short read_success      ; no
        mov     ecx,esi                 ; set size to fractional remainder
        sub     esi,esi                 ; total remaining size is zero now
        inc     edx                     ; pretend it's one more chunk
        jmp     read_last               ; go!

read_success:
        sub     edi,[pBuff]             ; subtract original pos. from current
        mov     eax,edi                 ; EAX == # of bytes read

read_finish:                            ; result is in EAX
        jmp     RestoreState

EndProc DosRead,NOCHECK


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   DosSeek
;
;   ENTRY
;       hFile == DOS file handle
;       lOffset == offset to seek to
;       iMethod == seek method (0=beg, 1=cur, 2=end)
;
;   EXIT
;       0, or negative error code
;
;   USES
;       Only EAX, ECX and EDX
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

BeginProc DosSeek, CCall

        ArgVar  hFile,DWORD
        ArgVar  lOffset,DWORD
        ArgVar  iMethod,DWORD

        call    SaveState               ; EBX -> client registers
                                        ; does not return if VMM re-entered
        mov     eax,[hFile]
        mov     [ebx].Client_BX,ax
        mov     eax,[lOffset]
        mov     [ebx].Client_DX,ax
        shr     eax,16
        mov     [ebx].Client_CX,ax      ; Client CX:DX == lOffset
        mov     al,byte ptr [iMethod]
        mov     ah,DOS_LSEEK
        mov     [ebx].Client_AX,ax
        call    NestedDOSInt
        mov     ax,[ebx].Client_DX      ; default to success
        shl     eax,16
        mov     ax,[ebx].Client_AX
        test    [ebx].Client_Flags,FLAGS_CF
        jz      short seek_success
        movzx   eax,[ebx].Client_AX
        neg     eax                     ; negify return code to indicate error

seek_success:
        jmp     RestoreState

EndProc DosSeek,NOCHECK


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   DosClose
;
;   ENTRY
;       hFile == DOS file handle
;
;   EXIT
;       0, or negative error code
;
;   USES
;       Only EAX, ECX and EDX
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

BeginProc DosClose, CCall

        ArgVar  hFile,DWORD

        call    SaveState               ; EBX -> client registers
                                        ; does not return if VMM re-entered
        mov     eax,[hFile]
        mov     [ebx].Client_BX,ax
        mov     [ebx].Client_AH,DOS_CLOSE
        call    NestedDOSInt
        sub     eax,eax                 ; default to success
        test    [ebx].Client_Flags,FLAGS_CF
        jz      short close_success
        movzx   eax,[ebx].Client_AX
        neg     eax                     ; negify return code to indicate error

close_success:
        jmp     RestoreState

EndProc DosClose,NOCHECK


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   DosGetDir
;
;   ENTRY
;       iDrive == drive code
;       pszDir -> buffer for directory (64 bytes or more)
;
;   EXIT
;       0, or negative error code
;
;   USES
;       Only EAX, ECX and EDX
;
;   NOTES
;       This function uses real-mode 60:0 through 60:FF as a translation
;       buffer.
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

BeginProc DosGetDir, CCall

        ArgVar  iDrive,DWORD
        ArgVar  pszDir,DWORD

        call    SaveState               ; EBX -> client registers
                                        ; does not return if VMM re-entered
        mov     eax,[iDrive]
        mov     [ebx].Client_DL,al
        mov     [ebx].Client_DS,(rb_ScratchStack shr SEG_SHIFT)
        mov     [ebx].Client_SI,(rb_ScratchStack and 000Fh)
        mov     [ebx].Client_AH,DOS_GETDIR
        call    NestedDOSInt
        movzx   eax,[ebx].Client_AX
        neg     eax                     ; default to error
        test    [ebx].Client_Flags,FLAGS_CF
        jnz     short getdir_exit       ; yup, there was an error

        mov     eax,[_pmbZero]
        lea     eax,[eax].rb_ScratchStack
        cCall   _nstrcpy,<pszDir,eax>

        sub     eax,eax                 ; return success

getdir_exit:
        jmp     RestoreState

EndProc DosGetDir,NOCHECK


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   DosSetDir
;
;   ENTRY
;       pszDir -> buffer for directory (including drive:)
;
;   EXIT
;       0, or negative error code
;
;   USES
;       Only EAX, ECX and EDX
;
;   NOTES
;       This function uses real-mode 60:0 through 60:FF as a translation
;       buffer.
;
        assume  cs:CODE_SEG,ds:DATA_SEG,es:DATA_SEG,ss:DATA_SEG

BeginProc DosSetDir, CCall

        ArgVar  pszDir,DWORD

        call    SaveState               ; EBX -> client registers
                                        ; does not return if VMM re-entered
        mov     eax,[_pmbZero]
        lea     eax,[eax].rb_ScratchStack
        cCall   _nstrcpy,<eax,pszDir>

        mov     [ebx].Client_DS,(rb_ScratchStack shr SEG_SHIFT)
        mov     [ebx].Client_DX,(rb_ScratchStack and 000Fh)
        mov     [ebx].Client_AH,DOS_CHDIR
        call    NestedDOSInt
        sub     eax,eax                 ; default to success
        test    [ebx].Client_Flags,FLAGS_CF
        jz      short setdir_success
        movzx   eax,[ebx].Client_AX
        neg     eax                     ; negify return code to indicate error

setdir_success:
        jmp     RestoreState

EndProc DosSetDir,NOCHECK


BeginProc SaveState

        mov     eax,-1
        pop     edx                     ; EDX == return address
        cmp     [_iIDTEntry],0
        jne     short state_exit
        VMMCall Get_VMM_Reenter_Count   ; ECX == # of times re-entered
        jecxz   state_ok                ; we're ok
state_exit:
        ret                             ; otherwise, return error (-1 for now)

state_ok:
        push    ebp
        mov     ebp,esp
        SaveReg <ebx,esi,edi>
        VMMCall Get_Crit_Status_No_Block
        or      ecx,ecx
        jnz     short state_abort       ; critical section is owned, get out
        sub     esp,size Client_Reg_Struc
        mov     edi,esp                 ; EDI -> client state buffer
        VMMCall Save_Client_State
        VMMCall Begin_Nest_Exec
        mov     ebx,[ebx].CB_Client_Pointer
        jmp     edx                     ; return with EBX -> client registers

EndProc SaveState,NOCHECK


BeginProc NestedDOSInt

        push    eax
        push    edx
        lea     edx,[esp-100h]          ; BUGBUG -- 100h a good slop factor???
        cmp     edx,[_pDebugStack]
        jb      short off_stack
        cmp     edx,[_pDebugStackTop]
        jb      short on_stack
off_stack:
        mov     edx,[_pDebugStackTop]
on_stack:
        xchg    [_pDebugStackTop],edx
        mov     eax,INT_DOS
        VMMCall Exec_Int
        mov     [_pDebugStackTop],edx
        pop     edx
        pop     eax
        ret

EndProc   NestedDOSInt,NOCHECK


BeginProc RestoreState

        VMMCall End_Nest_Exec
        mov     esi,esp                 ; ESI -> client state buffer
        VMMCall Restore_Client_State
        add     esp,size Client_Reg_Struc
state_abort label near
        RestoreReg <edi,esi,ebx>
        pop     ebp
        ret

EndProc RestoreState,NOCHECK


endif  ;WIN386

CLOSE_CODE

        end
