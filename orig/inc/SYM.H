/*
 *  Symbol Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define MAX_MODULES     20

#define MAX_SYMLENGTH   32      // this includes the NULL

#define MAX_SYMALLOC    4096


typedef struct _SYM {
    DWORD   off;                // offset for symbol
    PSZ     pszName;            // pointer to name of symbol
} SYM;
typedef SYM *PSYM;
typedef PSYM *PPSYM;

typedef struct _OBJ {
    INT     cSyms;              // ARRAYSIZE of SYMs
    PSYM    pSyms;              // pointer to array of SYMs
    PPSYM   ppSymsSorted;       // pointers to SYMs in sorted order
    WORD    wObjNum;            // type of object
    DWORD   sel;                // selector allocated to this object
    CHAR    achObjName[MAX_SYMLENGTH];
} OBJ;
typedef OBJ *POBJ;

typedef struct _MAP {
    INT     cObjs;              // ARRAYSIZE of OBJs
    POBJ    pObjs;              // pointer to array of OBJs
    INT     cConsts;            // ARRAYSIZE of constant SYMs
    PSYM    pConsts;            // pointer to array of constant SYMs
    CHAR    achModName[MAX_SYMLENGTH];
} MAP;
typedef MAP *PMAP;


// SYM file definitions

#define BFLAGS_16BIT    0x00
#define BFLAGS_32BIT    0x01
#define BFLAGS_ALPHATBL 0x02

typedef struct _MAPDEF {
    WORD    paraNextMap;
    BYTE    bFlags;
    BYTE    bRes1;
    WORD    wObjEntry;
    WORD    cConsts;
    WORD    offConsts;
    WORD    cObjs;
    WORD    paraNextObj;
    BYTE    bMaxSym;
    BYTE    cbModName;
} MAPDEF;
typedef MAPDEF *PMAPDEF;

typedef struct _OBJDEF {
    WORD    paraNextObj;
    WORD    cSyms;
    WORD    offSyms;
    WORD    wObjNum;
    WORD    wRes1;
    WORD    wRes2;
    WORD    wRes3;
    BYTE    bFlags;
    BYTE    bRes1;
    WORD    paraNextLine;
    WORD    wRes4;
    BYTE    cbObjName;
} OBJDEF;
typedef OBJDEF *POBJDEF;

typedef struct _SYMDEF16 {
    WORD    wValue;
    BYTE    cbSymName;
} SYMDEF16;
typedef SYMDEF16 *PSYMDEF16;

typedef struct _SYMDEF32 {
    DWORD   dwValue;
    BYTE    cbSymName;
} SYMDEF32;
typedef SYMDEF32 *PSYMDEF32;


// Function prototypes

#define SymAddress(pchOut, sel, off, iRadix, cchMin, pfSymbol) dwtosz(pchOut, off, iRadix, cchMin);
