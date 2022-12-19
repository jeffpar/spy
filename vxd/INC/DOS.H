/*
 *  DOS Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define SEG_DOSDATA                 0x0070

#define INT_DOSEXIT                 0x20
#define INT_DOS                     0x21

#define DOS_ABORT                   0x00    //
#define DOS_STD_CON_INPUT           0x01    // AL == char
#define DOS_STD_CON_OUTPUT          0x02    // DL == char
#define DOS_STD_AUX_INPUT           0x03    // AL == char
#define DOS_STD_AUX_OUTPUT          0x04    // DL == char
#define DOS_STD_PRINTER_OUTPUT      0x05    // DL == char
#define DOS_RAW_CON_IO              0x06    // DL == char (DL==0FFh => ZF==0 ==> AL==CHAR)
#define DOS_RAW_CON_INPUT           0x07    // AL == char
#define DOS_STD_CON_INPUT_NO_ECHO   0x08    // AL == char
#define DOS_STD_CON_STRING_OUTPUT   0x09    //
#define DOS_STD_CON_STRING_INPUT    0x0A    //
#define DOS_STD_CON_INPUT_STATUS    0x0B    //
#define DOS_STD_CON_INPUT_FLUSH     0x0C    //
#define DOS_DISK_RESET              0x0D    //
#define DOS_SET_DEFAULT_DRIVE       0x0E    //
#define DOS_FCB_OPEN                0x0F    //
#define DOS_FCB_CLOSE               0x10    //
#define DOS_DIR_SEARCH_FIRST        0x11    //
#define DOS_DIR_SEARCH_NEXT         0x12    //
#define DOS_FCB_DELETE              0x13    //
#define DOS_FCB_SEQ_READ            0x14    //
#define DOS_FCB_SEQ_WRITE           0x15    //
#define DOS_FCB_CREATE              0x16    //
#define DOS_FCB_RENAME              0x17    //
#define DOS_RESERVED18              0x18    //
#define DOS_GET_DEFAULT_DRIVE       0x19    //
#define DOS_SET_DMA                 0x1A    //
#define DOS_RESERVED1B              0x1B    //
#define DOS_RESERVED1C              0x1C    //
#define DOS_RESERVED1D              0x1D    //
#define DOS_RESERVED1E              0x1E    //
#define DOS_GET_DEFAULT_DPB         0x1F    // (UNDOCUMENTED)
#define DOS_RESERVED20              0x20    //
#define DOS_FCB_RANDOM_READ         0x21    //
#define DOS_FCB_RANDOM_WRITE        0x22    //
#define DOS_GET_FCB_FILE_LENGTH     0x23    //
#define DOS_GET_FCB_POSITION        0x24    //
#define DOS_SET_INTERRUPT_VECTOR    0x25    //
#define DOS_CREATE_PDB              0x26    //
#define DOS_FCB_RANDOM_READ_BLOCK   0x27    //
#define DOS_FCB_RANDOM_WRITE_BLOCK  0x28    //
#define DOS_PARSE_FILE_DESCRIPTOR   0x29    //
#define DOS_GET_DATE                0x2A    //
#define DOS_SET_DATE                0x2B    //
#define DOS_GET_TIME                0x2C    //
#define DOS_SET_TIME                0x2D    //
#define DOS_SET_VERIFY_ON_WRITE     0x2E    //
#define DOS_GET_DMA                 0x2F    //
#define DOS_GET_VERSION             0x30    //
#define DOS_KEEP_PROCESS            0x31    //
#define DOS_GET_DPB                 0x32    // (UNDOCUMENTED)
#define DOS_SET_CTRL_C_TRAPPING     0x33    //
#define DOS_GET_INDOS_FLAG          0x34    //
#define DOS_GET_INTERRUPT_VECTOR    0x35    //
#define DOS_GET_DRIVE_FREESPACE     0x36    //
#define DOS_CHAR_OPER               0x37    //
#define DOS_INTERNATIONAL_SUPPORT   0x38    //
#define DOS_MKDIR                   0x39    //
#define DOS_RMDIR                   0x3A    //
#define DOS_CHDIR                   0x3B    //
#define DOS_CREAT                   0x3C    //
#define DOS_OPEN                    0x3D    //
#define DOS_CLOSE                   0x3E    //
#define DOS_READ                    0x3F    //
#define DOS_WRITE                   0x40    //
#define DOS_UNLINK                  0x41    //
#define DOS_LSEEK                   0x42    //
#define DOS_CHMOD                   0x43    //
#define DOS_IOCTL                   0x44    //
#define DOS_XDUP                    0x45    //
#define DOS_XDUP2                   0x46    //
#define DOS_GETDIR                  0x47    //
#define DOS_ALLOC                   0x48    //
#define DOS_DEALLOC                 0x49    //
#define DOS_SETBLOCK                0x4A    //
#define DOS_EXEC                    0x4B    //
#define DOS_EXIT                    0x4C    //
#define DOS_WAIT                    0x4D    //
#define DOS_FIND_FIRST              0x4E    //
#define DOS_FIND_NEXT               0x4F    //
#define DOS_SET_CURRENT_PDB         0x50    // (UNDOCUMENTED)
#define DOS_GET_CURRENT_PDB         0x51    // (UNDOCUMENTED)
#define DOS_GET_SYSINIT_VARS        0x52    // (UNDOCUMENTED)
#define DOS_SET_DPB                 0x53    // (UNDOCUMENTED)
#define DOS_GET_VERIFY_ON_WRITE     0x54    //
#define DOS_DUP_PDB                 0x55    // (UNDOCUMENTED)
#define DOS_RENAME                  0x56    //
#define DOS_FILE_TIMES              0x57    // (Last 2.00 function)
#define DOS_RESERVED58              0x58    //
#define DOS_GET_EXTENDED_ERROR      0x59    //
#define DOS_CREATE_TEMP_FILE        0x5A    //
#define DOS_CREATE_NEW_FILE         0x5B    //
#define DOS_LOCK_OPER               0x5C    //
#define DOS_SERVER_CALL             0x5D    // (UNDOCUMENTED)
#define DOS_USER_OPER               0x5E    //
#define DOS_ASSIGN_OPER             0x5F    //
#define DOS_XNAMETRANS              0x60    //
#define DOS_JSA                     0x61    // (UNDOCUMENTED)
#define DOS_GET_CURRENT_PSP         0x62    // (UNDOCUMENTED)
#define DOS_HONGEUL                 0x63    // (UNDOCUMENTED)
#define DOS_SYSINITSLEAZE           0x64    // (UNDOCUMENTED)

#define DOSERR_BADFUNCTION          0x01
#define DOSERR_NOFILE               0x02
#define DOSERR_NOPATH               0x03
#define DOSERR_NOHANDLES            0x04
#define DOSERR_NOACCESS             0x05
#define DOSERR_BADHANDLE            0x06
#define DOSERR_BADARENA             0x07
#define DOSERR_NOMEMORY             0x08
#define DOSERR_BADADDRESS           0x09
#define DOSERR_BADENVIRONMENT       0x0A
#define DOSERR_BADFORMAT            0x0B
#define DOSERR_BADACCESS            0x0C
#define DOSERR_BADDATA              0x0D
#define DOSERR_BADUNIT              0x0E
#define DOSERR_BADDISK              0x0F
#define DOSERR_DELETECURDIR         0x10
#define DOSERR_NOTSAMEDEVICE        0x11
#define DOSERR_NOMOREFILES          0x12
#define DOSERR_WRITEPROTECT         0x13
#define DOSERR_UNKNOWNUNIT          0x14
#define DOSERR_DRIVENOTREADY        0x15
#define DOSERR_BADCOMMAND           0x16
#define DOSERR_DATA                 0x17
#define DOSERR_BADREQUEST           0x18
#define DOSERR_SEEK                 0x19
#define DOSERR_UNKNOWNMEDIA         0x1A
#define DOSERR_BADSECTOR            0x1B
#define DOSERR_NOPAPER              0x1C
#define DOSERR_WRITEFAULT           0x1D
#define DOSERR_READFAULT            0x1E
#define DOSERR_GENERALFAILURE       0x1F
#define DOSERR_NOTSHARED            0x20
#define DOSERR_LOCKED               0x21
#define DOSERR_DISKCHANGE           0x22

/*  Method codes for DOS_LSEEK
 */

#ifndef SEEK_SET
#define SEEK_SET                    0x00
#define SEEK_CUR                    0x01
#define SEEK_END                    0x02
#endif


/*  BIOS Parameter Block (BPB)
 */

typedef struct _BPB {           /* bpb */
    WORD    wBytesSector;       //
    BYTE    bSectorsCluster;    //
    WORD    wReservedSectors;   //
    BYTE    bFATs;              //
    WORD    wRootEntries;       //
    WORD    wTotalSectors;      //
    BYTE    bMediaDescriptor;   //
    WORD    wSectorsFAT;        //
    WORD    wSectorsTrack;      //
    WORD    wHeads;             //
    DWORD   dwHiddenSectors;    //
    DWORD   dwTotalSectors;     //
} BPB;


#define EXT_BOOT_SIGNATURE  41  // extended boot record signature

typedef struct _EBR {           /* ebr (extended boot record) */
    BYTE    abJmp[3];           //
    BYTE    achOEM[8];          //
    BPB     bpb;                //
    BYTE    bDrive;             //
    BYTE    bCurHD;             //
    BYTE    bSignature;         //
    DWORD   dwSerialNumber;     //
    BYTE    achVolume[11];      //
    BYTE    achSystemID[8];     //
} EBR;


/*  File Control Block (FCB)
 */

typedef struct _FCBHDR {        /* fcbhdr (for extended FCBs, size=07h) */
    BYTE    bExtFCBFlag;        // 00h (set by app to FFh)
    BYTE    abReserved[5];      // 01h
    BYTE    bDirAttr;           // 06h (set by app) one or more DIRATTR flags
} FCBHDR;
typedef FCBHDR *PFCBHDR;


typedef struct _FCB {           /* fcb (normal FCB structure, size=25h) */
    BYTE    chDrive;            // 00h (set by app)
    BYTE    achName[8];         // 01h (set by app)
    BYTE    achExtension[3];    // 09h (set by app)
    WORD    wBlock;             // 0Ch (also called EXTENT)
    WORD    wRecSize;           // 0Eh (set by app)
    DWORD   dwFileSize;         // 10h size of file in bytes
                                //     (high word is special in SEARCH calls)
    WORD    wDate;              // 14h modification date
    WORD    wTime;              // 16h modification time
    BYTE    abReserved[8];      // 18h (undoc?)
    BYTE    bRec;               // 20h current record #
    DWORD   dwRandRec[4];       // 21h (set by app) random record #
} FCB;
typedef FCB *PFCB;


typedef struct _EXTFCB {        /* extfcb (extended FCB structure, size=2Ch) */
    FCBHDR  fcbhdr;             //
    FCB     fcb;                //
} EXTFCB;
typedef EXTFCB *PEXTFCB;


typedef struct _MINFCB {        /* minfcb (minimal FCB structure, size=10h) */
    BYTE    chDrive;            //
    BYTE    achName[8];         // This is just a subset of above, for the
    BYTE    achExtension[3];    // definition of the PSP
    WORD    wBlock;             //
    WORD    wRecSize;           //
} MINFCB;
typedef MINFCB *PMINFCB;


/*  Memory Control Block (MCB) for DOS memory arena
 */

#define ARENA_OWNER_SYSTEM  0   // free block indication

#define ARENA_SIG        0x4D   // valid signature, not end of arena
#define ARENA_SIG_END    0x5A   // valid signature, last block in arena


typedef struct _MCB {           /* arena */
    BYTE    signature;          // ARENA_SIG or ARENA_SIG_END
    WORD    owner;              // owner of arena item
    WORD    size;               // size in paragraphs of item
    BYTE    reserved[3];        // reserved
    CHAR    name[8];            // owner file name
} MCB;
typedef MCB *PMCB;


/*  Program Segment Prefix (PSP), aka Process Data Block (PDB)
 *
 *  Notes
 *  -----
 *  intAbort    Since a TINY model application's stack is initialized
 *              by DOS with a ZERO, a near RET will invoke this INT 20h
 *
 *  abCPMCall   For CP/M compatibility, the WORD at abCPMCall[1] used to
 *              represent the number of bytes available in the TINY model
 *              segment, but DOS 5.0's DOS=HIGH support cannot support that
 *              interpretation.
 *
 *  maxJFNs     See INT 21/AH=67h to increase per-process handle table
 *
 *  lpJFNTable  See INT 21/AH=67h to increase per-process handle table
 */


#define MAX_PROCESSFILES        20

typedef struct _PSP {           /* psp */
    WORD    intAbort;           // 0000h INT 20h (CDh 20h)
    WORD    segNext;            // 0002h segment of next memory block
    BYTE    bUnused;            // 0004h (even according to UNDOC. DOS '91)
    BYTE    abCPMCall[5];       // 0005h far call to DOS for apps that "CALL 0005h"
    DWORD   vecTerminate;       // 000Ah copy of parent's INT 22h vector
    DWORD   vecCtrlC;           // 000Eh copy of parent's INT 23h vector
    DWORD   vecFatalAbort;      // 0012h copy of parent's INT 24h vector
    WORD    pspParent;          // 0016h PSP of parent process (undoc)
                                // 0018h array of JFNs (SFT IDs -undoc)
    BYTE    abJFN[MAX_PROCESSFILES];
    WORD    segEnviron;         // 002Ch segment of environment block
    DWORD   lpUserStack;        // 002Eh copy of SS:SP at INT 21h entry (undoc)
    WORD    maxJFNs;            // 0032h max JFNs (default == 20h - undoc)
    DWORD   lpJFNTable;         // 0034h ptr to table (default == PSP:0018h -undoc)
    DWORD   lpNextPDB;          // 0038h ptr to nested PDBs? (related to SHARE -undoc)
    BYTE    bInterCon;          // 003Ch flag for interim character input mode (undoc)
    BYTE    bAppendFlags;       // 003Dh BIT 0: set for TRUENAME reporting
    WORD    wNovell;            // 003Eh used by Novell redirector
    WORD    wDOSVersion;        // 0040h DOS version (possibly fake)
    WORD    wChain;             // 0042h (used by Windows KERNEL)
    WORD    wPartition;         // 0044h (no longer used by Windows KERNEL?)
    WORD    wNextPDB;           // 0046h (no longer used by Windows KERNEL?)
    DWORD   lpGlobalHeap;       // 0048h (used by Windows KERNEL)
    DWORD   lpEntryStack;       // 004Ch (no longer used by Windows KERNEL?)
    BYTE    abSysCall[3];       // 0050h INT 21h, RETF
    WORD    wUnused;            // 0053h (even according to UNDOC. DOS '91)
    FCBHDR  fcb1hdr;            // 0055h (allows FCB #1 to be an extended FCB)
    MINFCB  fcb1;               // 005Ch FCB #1
    MINFCB  fcb2;               // 006Ch FCB #2
    BYTE    abFCBData[4];       // 007Ch
    BYTE    cbCommand;          // 0080h (# of chars, excluding CR)
    BYTE    achCommand[127];    // 0081h command tail, ending with CR
} PSP;


/*  Directory Entry Definitions
 */

#define DIRATTR_READONLY        0x01
#define DIRATTR_HIDDEN          0x02
#define DIRATTR_SYSTEM          0x04
#define DIRATTR_VOLLABEL        0x08
#define DIRATTR_DIRECTORY       0x10
#define DIRATTR_ARCHIVE         0x20


/* DosOpen mode bits
 */

#define ACCESS_READ             0x00
#define ACCESS_WRITE            0x01
#define ACCESS_RW               0x02
#define ACCESS_EXEC             0x03
#define ACCESS_NODATE           0x04    // no Last Access date update (bit 3)

#define SHARE_COMP              0x00
#define SHARE_DENYALL           0x10
#define SHARE_DENYWRITE         0x20
#define SHARE_DENYREAD          0x30
#define SHARE_DENYNONE          0x40

#define SHARE_NOINHERIT         0x80


/* DosGetDir drive codes
 */

#define DRIVE_CURRENT           0x00


/*  Old Executable (OE) Format
 */

#define OLDEXE_SIG              0x5A4D


typedef struct _OLDEXE {        /* oe */
    WORD    wSig;               // "MZ"
    WORD    cbExtra;            // Length of image mod 512
    WORD    cBlocks;            // Length of image div 512
    WORD    cRelCnt;            // Size of relocation table
    WORD    cpHeader;           // Size of header div 16
    WORD    cpMinExtra;         // Minimum of extra memory div 16
    WORD    cpMaxExtra;         // Maximum of extra memory div 16
    WORD    offStack;           // Position of stack in image
    WORD    segStack;           // Initial stack ptr
    WORD    wCheckSum;          // Check sum
    WORD    segExec;            // Initial instruction ptr
    WORD    offExec;            // Position of code in image
    WORD    offRelocs;          // Offset of relocation table
    WORD    iOverlay;           // Overlay number
} OLDEXE;
typedef OLDEXE *POLDEXE;


typedef struct _DOS {           /* dos */
    WORD    harddrv_offset;
    BYTE    harddrv;
    WORD    DosDataSeg;
    BYTE    Bios_i2f[3];
    WORD    LowFlags;
    WORD    ROMStartAddr;
    BYTE    model_byte;
    BYTE    secondary_model_byte;
    DWORD   xms;
    DWORD   ptrsav;
    BYTE    res_dev_list[18];
    BYTE    auxdev[18];
    BYTE    prndev[18];
    BYTE    timdev[18];
    BYTE    dskdev[10];
    BYTE    drvmax;
    BYTE    keyrd_func;
    BYTE    com1dev[18];
    BYTE    lpt1dev[18];
    BYTE    lpt2dev[18];
    BYTE    lpt3dev[18];
    WORD    keysts_func;
    DWORD   OrigInt13;
    BYTE    com2dev[18];
    BYTE    com3dev[18];
    BYTE    com4dev[18];
    BYTE    tim_entry[5];
    BYTE    dsk_entry[5];
    DWORD   Start_BDS;
    DWORD   DiskSectorPtr;
    BYTE    RomVector10;
    DWORD   OldInt10;
    BYTE    RomVector13;
    DWORD   OldInt13;
    BYTE    RomVector15;
    DWORD   OldInt15;
    BYTE    RomVector19;
    DWORD   OldInt19;
    BYTE    RomVector1B;
    DWORD   OldInt1B;
    BYTE    RomVector4F;
    DWORD   OldInt4F;
    WORD    cdev_off;
    WORD    BiosCodeSeg;
} DOS;
typedef DOS *PDOS;


typedef struct MSDOS {          /* msdos */
    DWORD   DOSDATABEGIN;
    BYTE    DataVersion[10];
    WORD    MYNUM;
    WORD    FCBLRU;
    WORD    OpenLRU;
    DWORD   OEM_HANDLER;
    WORD    LeaveAddr;
    WORD    RetryCount;
    WORD    RetryLoop;
    DWORD   LastBuffer;
    WORD    CONTPOS;
    WORD    arena_head;
    DWORD   SYSINITVAR;
    DWORD   sft_addr;
    DWORD   BCLOCK;
    DWORD   BCON;
    WORD    MAXSEC;
    DWORD   BUFFHEAD;
    DWORD   CDSADDR;
    DWORD   sftFCB;
    WORD    KeepCount;
    BYTE    NUMIO;
    BYTE    CDSCOUNT;
    BYTE    NULDEV[18];
    BYTE    Splices;
    WORD    Special_Entries;
    DWORD   SetVerTbl;
    WORD    ChkCopyProt;
    WORD    A20OFF_PSP;
    WORD    BUFFERS_PARM1;
    WORD    BUFFERS_PARM2;
    BYTE    BOOTDRIVE;
    BYTE    DDMOVE;
    WORD    EXT_MEM_SIZE;
    DWORD   BufferQueue;
    WORD    DirtyBufferCount;
    DWORD   SC_CACHE_PTR;
    WORD    SC_CACHE_COUNT;
    BYTE    BuffInHMA;
    DWORD   LoMemBuff;
    WORD    CritCallTablePtr;
    BYTE    UNUSED_BYTE;
    WORD    CL0FATENTRY;
    BYTE    IoStatFail;
    BYTE    ALLOCMSAVE;
    BYTE    A20OFF_COUNT;
    BYTE    DOS_FLAG;
    WORD    UNPACK_OFFSET;
    BYTE    UMBFLAG;
    WORD    SAVE_AX;
    WORD    UMB_HEAD;
    WORD    START_ARENA;
    BYTE    JShare[60];
    BYTE    dummy_sftabl[301];
    BYTE    CARPOS;
    BYTE    STARTPOS;
    BYTE    INBUF[128];
    BYTE    CONBUF[131];
    BYTE    PFLAG;
    BYTE    VERFLG;
    BYTE    CharCo;
    BYTE    chSwitch;
    BYTE    AllocMethod;
    BYTE    fShare;
    BYTE    DIFFNAM;
    BYTE    MYNAME[16];
    BYTE    CritPatch[11];
    BYTE    ErrorMode;
    BYTE    INDOS;
    BYTE    WPErr;
    BYTE    EXTERR_LOCUS;
    WORD    EXTERR;
    BYTE    EXTERR_ACTION;
    BYTE    EXTERR_CLASS;
    DWORD   EXTERRPT;
    DWORD   DMAADD;
    WORD    CurrentPDB;
    WORD    ConC_spsave;
    WORD    exit_code;
    BYTE    CURDRV;
    BYTE    CNTCFLAG;
    BYTE    CPSWFLAG;
    BYTE    CPSWSAVE;
    WORD    USER_IN_AX;
    WORD    PROC_ID;
    WORD    USER_ID;
    WORD    FirstArena;
    WORD    BestArena;
    WORD    LastArena;
    WORD    EndMem;
    WORD    LASTENT;
    BYTE    FAILERR;
    BYTE    ALLOWED;
    BYTE    NoSetDir;
    BYTE    DidCTRLC;
    WORD    SpaceFlag;
    BYTE    DAY;
    BYTE    MONTH;
    WORD    YEAR;
    WORD    DAYCNT;
    BYTE    WEEKDAY;
    BYTE    CONSWAP;
    BYTE    IDLEINT;
    BYTE    fAborting;
    BYTE    DEVCALL[13];
    BYTE    CALLUNIT;
    BYTE    CALLRBYT;
    BYTE    CALLVIDM[3];
    WORD    CallBPB;
    WORD    CALLSSEC;
    DWORD   CALLVIDRW;
    DWORD   CALLNEWSC;
    DWORD   CALLDEVAD;
    BYTE    IOCall[13];
    BYTE    IORCHR;
    DWORD   IOXAD;
    WORD    IOSCNT;
    WORD    IOSSEC;
    WORD    DSKSTCALL;
    BYTE    DSKSTCOM;
    BYTE    DSKSTST[10];
    BYTE    DSKCHRET[5];
    DWORD   DSKSTCNT;
    BYTE    CreatePDB;
    BYTE    Lock_Buffer[9];
    BYTE    UserNum[3];
    BYTE    OEMNum;
    BYTE    TIMEBUF[6];
    WORD    DEVIOBUF;
    BYTE    OPENBUF[128];
    BYTE    RenBuf[128];
    BYTE    SEARCHBUF[53];
    BYTE    DummyCDS[88];
    BYTE    NAME1[12];
    BYTE    NAME2[13];
    BYTE    DESTSTART[7];
    BYTE    ATTRIB;
    BYTE    EXTFCB;
    BYTE    SATTRIB;
    BYTE    open_access;
    BYTE    FoundDel;
    BYTE    Found_dev;
    BYTE    fSplice;
    BYTE    fSharing;
    BYTE    SECCLUSPOS;
    BYTE    TRANS;
    BYTE    READOP;
    BYTE    THISDRV;
    BYTE    CLUSFAC;
    BYTE    CLUSSPLIT;
    BYTE    INSMODE;
    BYTE    cMeta;
    BYTE    VOLID;
    WORD    exit_type;
    BYTE    CREATING;
    BYTE    DELALL;
    DWORD   EXITHOLD;
    WORD    user_SP;
    WORD    user_SS;
    WORD    CONTSTK;
    DWORD   THISDPB;
    WORD    CLUSSAVE;
    DWORD   CLUSSEC;
    WORD    PREREAD;
    WORD    FATBYT;
    WORD    FATBYTE;
    DWORD   DEVPT;
    DWORD   THISSFT;
    DWORD   THISCDS;
    DWORD   THISFCB;
    WORD    SFN;
    WORD    JFN;
    DWORD   PJFN;
    WORD    WFP_START;
    WORD    REN_WFP;
    WORD    CURR_DIR_END;
    WORD    NEXTADD;
    WORD    LASTPOS;
    WORD    CLUSNUM;
    DWORD   DIRSEC;
    WORD    DIRSTART;
    DWORD   SECPOS;
    DWORD   VALSEC;
    WORD    BYTSECPOS;
    DWORD   BYTPOS;
    WORD    BYTCNT1;
    WORD    BYTCNT2;
    WORD    SECCNT;
    WORD    ENTFREE;
    WORD    ENTLAST;
    WORD    NXTCLUSNUM;
    DWORD   GROWCNT;
    DWORD   CURBUF;
    DWORD   CONSft;
    WORD    SaveBX;
    WORD    SaveDS;
    WORD    restore_tmp;
    WORD    NSS;
    WORD    NSP;
    WORD    EXTOPEN_FLAG;
    BYTE    EXTOPEN_ON;
    WORD    EXTOPEN_IO_MODE;
    WORD    SAVE_DI;
    WORD    SAVE_ES;
    WORD    SAVE_DX;
    WORD    SAVE_CX;
    WORD    SAVE_BX;
    WORD    SAVE_SI;
    WORD    SAVE_DS;
    WORD    HIGH_SECTOR;
    WORD    OffsetMagicPatch;
    BYTE    DISK_FULL;
    WORD    TEMP_VAR;
    WORD    TEMP_VAR2;
    BYTE    DrvErr;
    WORD    DOS34_FLAG;
    DWORD   NO_FILTER_PATH;
    DWORD   NO_FILTER_DPATH;
    WORD    AbsRdWr_SS;
    WORD    AbsRdWr_SP;
    BYTE    UU_Callback_flag;
    BYTE    RENAMEDMA[384];
    BYTE    AuxStack[384];
    BYTE    DskStack[384];
    BYTE    PRINTER_FLAG;
    BYTE    VOLCHNG_FLAG;
    BYTE    VIRTUAL_OPEN;
    BYTE    FSeek_drive;
    WORD    FSeek_firclus;
    WORD    FSeek_logclus;
    WORD    FSeek_logsave;
    WORD    TEMP_DOSLOC;
    BYTE    SWAP_END;
    BYTE    UCASE_TAB[130];
    BYTE    FILE_UCASE_TAB[130];
    BYTE    FILE_CHAR_TAB[48];
    BYTE    COLLATE_TAB[258];
    BYTE    DBCS_TAB[18];
    BYTE    MAP_CASE[18];
    BYTE    InterChar;
    BYTE    InterCon;
    BYTE    SaveCurFlg;
    WORD    TEMPSEG;
    BYTE    redir_patch;
    BYTE    DosHasHMA;
    WORD    fixexepatch;
    WORD    RationalPatchPtr;
    WORD    MSVERSION;
    BYTE    YRTAB[8];
    BYTE    MONTAB[12];
    BYTE    SysInitTable[8];
    BYTE    FastOpenTable[10];
    BYTE    FastFlg;
    BYTE    FastOpen_Ext_Info[11];
    BYTE    Dir_Info_Buff[32];
    WORD    Next_Element_Start;
    WORD    USER_SP_2F;
    BYTE    Packet_Temp[28];
    BYTE    SCAN_FLAG;
    WORD    DATE_FLAG;
    WORD    AbsDskErr;
    BYTE    NO_NAME_ID[11];
    BYTE    LOOKSIZ;
    BYTE    snuldev[6];
    BYTE    inuldev[62];
    BYTE    ERR_TABLE_21[128];
    BYTE    ERR_TABLE_24[80];
    BYTE    ErrMap24[16];
    WORD    SPECIAL_VERSION;
    WORD    OLD_FIRSTCLUS;
    WORD    exec_init_SP;
    WORD    exec_init_SS;
    WORD    exec_init_IP;
    WORD    exec_init_CS;
    WORD    exec_signature;
    WORD    exec_len_mod_512;
    WORD    exec_pages;
    WORD    exec_rle_count;
    WORD    exec_par_dir;
    WORD    exec_min_BSS;
    WORD    exec_max_BSS;
    WORD    exec_SS;
    WORD    exec_SP;
    WORD    exec_chksum;
    WORD    exec_IP;
    WORD    exec_CS;
    WORD    exec_rle_table;
    BYTE    Win386_Info[22];
    BYTE    Instance_Table[58];
    BYTE    Opt_Instance_Table[10];
    BYTE    Win386_DOSVars[14];
    BYTE    IsWin386;
    BYTE    DummyErrorModeCode[15];
    BYTE    DummyErrorModeCode2[10];
    BYTE    LocalSFT[5];
    DWORD   DosIntTable[8];             // 0, 20h, 21h, 25h, 26h, 27h, 2Fh, CALL 5
    WORD    SS_Save;                    // save user's stack segment (for EnsureA20ON)
    WORD    SP_Save;                    // save user's stack offset (for EnsureA20ON)
    BYTE    i0patch[5];
    BYTE    ldivov_jmp[5];
    BYTE    i20patch[5];
    BYTE    lquit_jmp[5];
    BYTE    lcommand[5];
    BYTE    lcommand_jmp[5];
    BYTE    i25patch[5];
    BYTE    labsdrd_jmp[5];
    BYTE    labsdwrt[5];
    BYTE    labsdwrt_jmp[5];
    BYTE    i27patch[5];
    BYTE    lstay_resident_jmp[5];
    BYTE    i2fpatch[5];
    BYTE    lint2f_jmp[5];
    BYTE    cpmpatch[5];
    BYTE    lcallentry_jmp[5];
    DWORD   DosRetAddr23;
    DWORD   DosRetAddr24;
    BYTE    LowInt23[20];
    BYTE    LowInt24[20];
    BYTE    LowInt28[6];
    BYTE    disa20_xfer[24];
    BYTE    disa20_iret[41];
    BYTE    lirett[13];
    BYTE    XMMcontrol[155];
    BYTE    RatBugCode[309];
    BYTE    COUNTRY_CDPG[142];
    BYTE    NLS_DATA[58];
    BYTE    CritSecCallTable[20];
    BYTE    Rational386PatchPtr[8];
    BYTE    sftabl[301];
    BYTE    DOSDATAEND;
} MSDOS;
typedef MSDOS *PMSDOS;


/* ASM

IFNDEF  WIN386

_DOSDATA segment at SEG_DOSDATA

DosData  DOS <>

_DOSDATA ends

ENDIF

*/


// dosdump.c

BOOL    DOSDumpCommand(PESF pesf, PCHAR pchCmd);


// dosserv.asm

INT     DosOpen(PSZ pszFile, INT flMode);
INT     DosRead(INT hFile, INT cbRead, PVOID pBuff);
INT     DosSeek(INT hFile, LONG lOffset, INT iMethod);
INT     DosClose(INT hFile);
INT     DosGetDir(INT iDrive, PSZ pszDir);
INT     DosSetDir(PSZ pszDir);
