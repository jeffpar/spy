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

extern  PTSS    pTSS;

extern  WORD    sel_Flat, sel_Text, sel_Data;

extern  PTSS    pPhysTSS;
extern  PBYTE   pPhysData;
extern  DWORD   cbPhysData;
#ifdef  DEBUG
extern  DWORD   cbBSSData;
#endif
extern  PTE     pPhysPgDir, pPgDir;
extern  PTE     pPhysPgTbl, pPgTbl;
extern  PBYTE   pPhysFree, pLinFree, pFree;

extern  INT     flKeyEvent;


// memmgr.c globals

extern  PMB     pmbZero;

// vidmgr.c globals

extern  PBYTE   pVRAM;
extern  PVS     pvsMonitor, pvsVM;

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

        IFNDEF  __acrtused

        .386p
        extrn   SEL_FLAT:abs
        extrn   SEL_TEXT:abs,SEL_DATA:abs,SEL_TSS:abs

        OPEN_CODE
        CLOSE_CODE

        OPEN_DATA

        extrn   _pPhysTSS:dword
        extrn   _pPhysData:dword
        extrn   _cbPhysData:dword
        extrn   _pPhysPgDir:dword, _pPgDir:dword
        extrn   _pPhysPgTbl:dword, _pPgTbl:dword
        extrn   _pPhysFree:dword, _pLinFree:dword, _pFree:dword

        extrn   _dtrIDT:DTR, _pIDT:dword
        extrn   _dtrGDT:DTR, _pGDT:dword
        extrn   _pTSS:dword
        extrn   _sel_Flat:word, _sel_Text:word, _sel_Data:word

; memmgr.c globals

        extrn   _pmbZero:dword

; vidmgr.c globals

        extrn   _pVRAM:dword
        extrn   _pvsMonitor:dword, _pvsVM:dword

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

VOID    InitMemMgr(VOID);                               // initialization-only function!
DWORD   SetPages(PVOID pLinear, DWORD dwPhysical, INT nPages);
PVOID   MemAlloc(INT cbSize);
BOOL    MemFree(PVOID p, INT cbSize);


/*
 * vidmgr.c functions
 */

VOID    InitVideoMgr(VOID);                             // initialization-only function!
PVS     InitVS(INT cb1, INT cb2, INT cb3, INT cb4);     // initialization-only function!


/*
 * vidsave.asm functions
 */

VOID    SaveVS(PVS pvs, BOOL fEnable);
VOID    RestoreVS(PVS pvs);
VOID    WaitRetrace(PVS pvs, BYTE bRetrace);
