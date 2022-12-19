/*
 *  Globals
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define MULTIPAGE


extern  DTR     dtrIDT;
extern  PDES    pIDT;

extern  DTR     dtrGDT;
extern  PDES    pGDT;

#ifdef  WIN386
extern  DTR     dtrLDT;
extern  PDES    pLDT;
extern  WORD    sel_LDT, sel_TSS;
#endif

extern  PTSS    pTSS;

extern  WORD    sel_Flat, sel_Text, sel_Data;

#ifndef WIN386
extern  PTSS    pPhysTSS;
extern  PBYTE   pPhysData;
extern  DWORD   cbPhysData;
#ifdef  DEBUG
extern  DWORD   cbBSSData;
#endif
extern  PTE     pPhysPgDir, pPgDir;
extern  PTE     pPhysPgTbl, pPgTbl;
extern  PBYTE   pPhysFree, pLinFree, pFree;
#endif

extern  INT     flKeyEvent;


// memmgr.c globals

extern  PMB     pmbZero;

// vidmgr.c globals

extern  PBYTE   pVRAM;
extern  VSTATE  vsMonitor, vsVM;

extern  INT rowCursor, colCursor;
extern  INT rowCursorMax, colCursorMax;

#ifdef  MULTIPAGE
extern  INT rowScreenVis, rowScreenMax;
#endif

extern  BYTE bDefColor;

// x86debug.c globals

extern  DWORD   flTrace;
extern  DWORD   msDebugStart;


/* ASM
        IFDEF   WIN386

        CODE_SEG equ <FLAT>
        DATA_SEG equ <FLAT>

OPEN_ICODE macro
VxD_ICODE_SEG
endm

CLOSE_ICODE macro
VxD_ICODE_ENDS
endm

OPEN_CODE macro
VxD_CODE_SEG
endm

CLOSE_CODE macro
VxD_CODE_ENDS
endm

OPEN_IDATA macro
VxD_IDATA_SEG
endm

CLOSE_IDATA macro
VxD_IDATA_ENDS
endm

OPEN_DATA macro
VxD_DATA_SEG
endm

CLOSE_DATA macro
VxD_DATA_ENDS
endm

        ELSE

        CODE_SEG equ <_TEXT>
        DATA_SEG equ <_DATA>

OPEN_CODE macro
CODE_SEG  segment dword use32 public 'CODE'
endm

CLOSE_CODE macro
CODE_SEG   ends
endm

OPEN_DATA macro
DATA_SEG  segment dword use32 public 'DATA'
endm

CLOSE_DATA macro
DATA_SEG   ends
endm

        ENDIF   ;WIN386


        IFNDEF  __acrtused

        IFNDEF  WIN386
        .386p
        extrn   SEL_FLAT:abs
        extrn   SEL_TEXT:abs,SEL_DATA:abs,SEL_TSS:abs
        ENDIF

        OPEN_CODE
        CLOSE_CODE

        OPEN_DATA

        IFNDEF  WIN386
        extrn   _pPhysTSS:dword
        extrn   _pPhysData:dword
        extrn   _cbPhysData:dword
        extrn   _pPhysPgDir:dword, _pPgDir:dword
        extrn   _pPhysPgTbl:dword, _pPgTbl:dword
        extrn   _pPhysFree:dword, _pLinFree:dword, _pFree:dword
        ENDIF

        extrn   _dtrIDT:DTR, _pIDT:dword
        extrn   _dtrGDT:DTR, _pGDT:dword
        IFDEF   WIN386
        extrn   _dtrLDT:DTR, _pLDT:dword
        extrn   _sel_LDT:word, _sel_TSS:word
        ENDIF
        extrn   _pTSS:dword
        extrn   _sel_Flat:word, _sel_Text:word, _sel_Data:word

; memmgr.c globals

        extrn   _pmbZero:dword

; vidmgr.c globals

        extrn   _pVRAM:dword
        extrn   _vsMonitor:VSTATE, _vsVM:VSTATE

; x86debug.c globals

        extrn   _flTrace:dword

        CLOSE_DATA

        ENDIF
*/


/*
 * MASM functions
 */

VOID    Reboot(VOID);
VOID    DrainKbd(VOID);
VOID    EnableKbd(VOID);
VOID    DisableKbd(VOID);
VOID    DrainMse(VOID);
VOID    EnableMse(VOID);
VOID    DisableMse(VOID);
BYTE    GetByte(DWORD sel, DWORD off);
VOID    SetByte(DWORD sel, DWORD off, BYTE b);


/*
 * memmgr.c functions
 */

VOID    InitMemMgr(VOID);       // initialization-only function!
#ifndef WIN386
VOID    SetPages(PVOID pLinear, DWORD dwPhysical, INT nPages);
#endif
PVOID   MemAlloc(INT cbSize);
BOOL    MemFree(PVOID p, INT cbSize);


/*
 * vidmgr.c functions
 */

VOID    InitVideoMgr(VOID);     // initialization-only function!
VOID    InitVS(PVS pvs);        // initialization-only function!


/*
 * vidsave.asm functions
 */

VOID    SaveVS(PVS pvs, BOOL fEnable);
VOID    RestoreVS(PVS pvs);
VOID    WaitRetrace(PVS pvs, BYTE bRetrace);
#ifdef WIN386                   // initialization-only functions!
VOID    SetMode(PVS pvs, INT flMode);
VOID    EnableMode(PVS pvs);
#endif


#ifdef WIN386

/*
 * VMM functions (that really should be in VMM.H)
 */

DWORD VXDINLINE
Get_Cur_VM_Handle(VOID)
{
    VMMCall(Get_Cur_VM_Handle)
    _asm xchg eax,ebx
}

DWORD VXDINLINE
Get_Sys_VM_Handle(VOID)
{
    VMMCall(Get_Sys_VM_Handle)
    _asm xchg eax,ebx
}

BOOL VXDINLINE
Test_Cur_VM_Handle(DWORD hVM)
{
    _asm mov ebx,[hVM]
    VMMCall(Test_Cur_VM_Handle)
    _asm {
         mov eax,0
         jnz short lbl1
         inc eax
        lbl1:
    }
}

DWORD VXDINLINE
Get_Next_VM_Handle(DWORD hVM)
{
    _asm mov ebx,[hVM]
    VMMCall(Get_Next_VM_Handle)
    _asm xchg eax,ebx
}

DWORD VXDINLINE
Get_Last_Updated_System_Time(VOID)
{
    Touch_Register(eax)
    VMMCall(Get_Last_Updated_System_Time)
}

DWORD VXDINLINE
Get_System_Time(VOID)
{
    Touch_Register(eax)
    VMMCall(Get_System_Time)
}

BOOL VXDINLINE
Get_VMM_Reenter_Count(VOID)
{
    VMMCall(Get_VMM_Reenter_Count)
    _asm xchg eax,ecx
}

DWORD VXDINLINE
Get_Crit_Status_No_Block(VOID)
{
    Touch_Register(ecx)
    VMMCall(Get_Crit_Status_No_Block)
    _asm {
         sub eax,eax
         or  ecx,ecx    // can't get the damn compiler to do jecxz...
         jz  short lbl2
         mov eax,ebx
        lbl2:
    }
}

PVOID VXDINLINE 
_MapPhysToLinear(DWORD physAddr,
                 DWORD nBytes,
                 DWORD flags)
{
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    _asm push flags
    _asm push nBytes
    _asm push physAddr
    VMMCall(_MapPhysToLinear)
    _asm add  esp, 12
}

PVOID VXDINLINE 
_PageAllocate(DWORD nPages,
              DWORD flType,
              DWORD hVM,
              DWORD flAlign,
              DWORD minPhys,
              DWORD maxPhys,
              PDWORD pPhys,
              DWORD flags)
{
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    _asm push flags
    _asm push pPhys
    _asm push maxPhys
    _asm push minPhys
    _asm push flAlign
    _asm push hVM
    _asm push flType
    _asm push nPages
    VMMCall(_PageAllocate)
    _asm add  esp, 32
#ifdef WIN31COMPAT
    _asm _emit 0x92     // xchg eax,edx broken
#endif
}

BOOL VXDINLINE
_PageFree(PVOID p,
          DWORD flags)
{
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    _asm push flags
    _asm push p
    VMMCall(_PageFree)
    _asm add  esp, 8
}

#endif
