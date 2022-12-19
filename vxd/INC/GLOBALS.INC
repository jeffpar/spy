MULTIPAGE	EQU	1
ifdef  WIN386
endif
ifndef WIN386
ifdef  DEBUG
endif
endif
ifdef  MULTIPAGE
endif
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

        ENDIF


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



        extrn   _pmbZero:dword



        extrn   _pVRAM:dword
        extrn   _vsMonitor:VSTATE, _vsVM:VSTATE



        extrn   _flTrace:dword

        CLOSE_DATA

        ENDIF

ifndef WIN386
endif
ifdef WIN386
endif
ifdef WIN386
ifdef WIN31COMPAT
endif
endif
