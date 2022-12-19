/*
 *  WIN386 Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define LOGOP_INB       1
#define LOGOP_INW       2
#define LOGOP_OUTB      3
#define LOGOP_OUTW      4


#define MAX_LOG_ENTRIES 1024

typedef struct _LOGENTRY {
    DWORD   hVM;                // VM handle
    DWORD   off;                // offset of event
    WORD    sel;                // selector of event
    BYTE    iOp;                // see LOGOP_*
    DWORD   dwDst;              // dest. operand
    DWORD   dwSrc;              // source operand
    DWORD   msTimeStamp;        // timestamp of event
} LOGENTRY;

typedef LOGENTRY *PLOGENTRY;


typedef struct _LOG {
    PSZ     pszDesc;
    INT     iNext;              // if iNext == iLast, log is empty
    INT     iLast;
    INT     cEntries;
    INT     iMaximum;
    PLOGENTRY ple;
} LOG;

typedef LOG *PLOG;


#define OBJNUM_LOCKED   1
#define OBJNUM_INIT     2
#define OBJNUM_PAGED    3
#define OBJNUM_STATIC   4

#define OBJNAME_LOCKED  "_LGROUP"
#define OBJNAME_INIT    "_IGROUP"
#define OBJNAME_PAGED   "_PGROUP"
#define OBJNAME_STATIC  "_SGROUP"

typedef struct _VXDADDR {
    DWORD  vxd_ObjAddr;
    DWORD  vxd_ObjType;
    struct VxD_Desc_Block *vxd_pDDB;
} VXDADDR;

typedef VXDADDR *PVXDADDR;


typedef BOOL (FAR *PFNDUMP)(PESF pesf, PCHAR pchCmd);


// VMMDUMP function prototypes

BOOL    VMMDumpIO(PESF pesf, PCHAR pchCmd);
BOOL    VMMDumpVMs(PESF pesf, PCHAR pchCmd);
BOOL    VMMDumpVMRegs(PESF pesf, PCHAR pchCmd);
BOOL    VMMWatchSwitch(PESF pesf, PCHAR pchCmd);
BOOL    VMMDumpLog(PLOG plog, BOOL fLastFirst);

VOID    VMMDumpSpecificVMRegs(DWORD hVM);

VOID    LogInALnn(PESF pesf);
VOID    LogInAXnn(PESF pesf);
VOID    LogOutnnAL(PESF pesf);
VOID    LogOutnnAX(PESF pesf);
VOID    LogInALDX(PESF pesf);
VOID    LogInAXDX(PESF pesf);
VOID    LogOutDXAL(PESF pesf);
VOID    LogOutDXAX(PESF pesf);
VOID    LogAppend(PLOG plog, DWORD hVM,
                  DWORD sel, DWORD off,
                  INT iOp, DWORD dwDst, DWORD dwSrc);

BOOL    IsValidVMHandle(PDWORD phVM);


// VXDDUMP function prototypes

struct  DeviceInfo *GetDeviceList(VOID);

BOOL    VxDDump(PESF pesf, PCHAR pchCmd);
BOOL    VxDFindAddress(DWORD dwValue,
                       PSZ pszModName,
                       PPSZ ppszObj, PDWORD pdstObj,
                       PPSZ ppszSym, PDWORD pdstSym);

BOOL    VxDFindDynamicAddress(DWORD addr, PVXDADDR pvxd);
BOOL    VxDFindStaticAddress(DWORD addr, PVXDADDR pvxd);

DWORD   VxDFindObject(PSZ pszModule, WORD wObjNum);
DWORD   VxDFindDynamicObject(PSZ pszModule, WORD wObjNum);
DWORD   VxDFindStaticObject(PSZ pszModule, WORD wObjNum);

PSZ     VxDObjDesc(DWORD Type);
WORD    VxDObjNum(DWORD Type);
WORD    VxDObjNumFromName(PSZ pszObjName);
