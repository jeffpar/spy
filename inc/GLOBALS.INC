MULTIPAGE	EQU	1
ifdef  DEBUG
endif
ifdef  MULTIPAGE
endif
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



        extrn   _pmbZero:dword



        extrn   _pVRAM:dword
        extrn   _pvsMonitor:dword, _pvsVM:dword



        extrn   _flTrace:dword

        CLOSE_DATA

        ENDIF

