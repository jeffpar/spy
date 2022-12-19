/*
 *  ROM BIOS Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#include <pic.h>
#include <timer.h>
#include <kbd.h>
#include <disk.h>
#include <cmos.h>
#include <video.h>


#define SEG_ROMDATA             0x0000

#define MB_ADDR                 0x00000000
#define MB_SIZE                 sizeof(MB)

#define DOSMEM_SIZE            (0x000A0000 - 0x00000700)
#define ROMMEM_SIZE            (0x00100000 - 0x000C0000)
#define WRAPMEM_SIZE            0x0000FFF0

#define INT_DIVERROR            IDT_DIVERROR    // 0x00
#define INT_DEBUG               IDT_DEBUG       // 0x01
#define INT_NMI                 IDT_NMI         // 0x02
#define INT_BREAKPOINT          IDT_BREAKPOINT  // 0x03
#define INT_OVERFLOW            IDT_OVERFLOW    // 0x04
#define INT_PRINTSCREEN         IDT_BOUNDS      // 0x05
#define INT_INVOPCODE           IDT_INVOPCODE   // 0x06
#define INT_NOCP                IDT_NOCP        // 0x07

#define INT_TMR                 0x08
#define INT_KBD                 0x09
#define INT_SLAVE               0x0A
#define INT_COM2                0x0B
#define INT_COM1                0x0C
#define INT_LPT2                0x0D
#define INT_DISKETTE            0x0E
#define INT_LPT1                0x0F

#define INT_RTC                 0x70
#define INT_IRQ2                0x71
#define INT_IRQ10               0x72
#define INT_IRQ11               0x73
#define INT_IRQ12               0x74
#define INT_CP                  0x75
#define INT_DISK                0x76
#define INT_IRQ15               0x77

#define INT_EQUIPMENTCHECK      0x11
#define INT_MEMORYCHECK         0x12
#define INT_COMIO               0x14
#define INT_SYSSERV             0x15    // System Services interrupt
#define INT_PRINTERIO           0x17
#define INT_ROMBASIC            0x18
#define INT_REBOOT              0x19

#define RESETFLAG_WARM          0x1234


/*  Int 15h Equates (AH values)
 */
#define INT15AH_KEYINTERCEPT    0x4F
#define INT15AH_KEYSYSREQ       0x85
#define INT15AH_BMOVE           0x87
#define INT15AH_EXTMEM          0x88
#define INT15AH_PMODE           0x89
#define INT15AH_DEVBUSY         0x90
#define INT15AH_INTCOMPLETE     0x91


/*  Int 15h Equates (AL values)
 */
#define INT15AL_DISKETTE        0x01
#define INT15AL_KEYBOARD        0x02


/* MB Data Areas
 *
 * The following regions are instanced by DOS386:
 *
 *  0x0000: 0x400 bytes
 *  0x0400: 0x008 bytes
 *  0x0410: 0x002 bytes
 *  0x0413: 0x002 bytes
 *  0x0415: 0x028 bytes
 *  0x0449: 0x01E bytes
 *  0x0471: 0x001 bytes
 *  0x047C: 0x004 bytes
 *  0x0480: 0x004 bytes
 *  0x0484: 0x007 bytes
 *  0x0496: 0x00B bytes
 *  0x04A8: 0x004 bytes
 *  0x0500: 0x002 bytes
 *  0x0504: 0x001 bytes
 *  0x050E: 0x014 bytes
 */

#define EQUIP_FLOPPY            0x00001 // machine has a floppy
#define EQUIP_X87               0x00002 // X87=1 if coprocessor installed
#define EQUIP_VIDEOMASK         0x00030 // video configuration bits
#define EQUIP_COLOR40VIDEO      0x00010 //
#define EQUIP_COLOR80VIDEO      0x00020 //
#define EQUIP_MONOVIDEO         0x00030 //
#define EQUIP_FLOPPYMASK        0x000C0 // # floppies-1 (if IPLDISK=1)
#define EQUIP_COMMASK           0x00E00 // # COM ports
#define EQUIP_PRINTERMASK       0x0C000 // # LPT ports


typedef struct _RB {            /* rb */
    DWORD   IVT[256];           // real-mode IVT                0x0000 (WIN-instanced)
    WORD    RS232Base[4];       // addresses of RS232 adapters  0x0400 (WIN-instanced)
    WORD    PrinterBase[4];     // addresses of printers        0x0408
    WORD    EquipFlag;          // installed hardware           0x0410 (WIN-instanced)
    BYTE    MfgTest;            // initialization flag          0x0412
    WORD    MemorySize;         // memory size in Kbytes        0x0413 (WIN-instanced)
    BYTE    MfgErrFlag;         // scratchpad for manufacturing 0x0415 (WIN-instanced)
    BYTE    MfgErrCode;         // error codes                  0x0416 (WIN-instanced)
//  A bug in Windows 3.x: the byte at 0x043D fails to get instanced
    KBDDATA kbd;                // keyboard data                0x0417 (WIN-instanced)
    DSKDATA dsk;                // disk data                    0x043E
    VIDDATA vid;                // video data                   0x0449 (WIN-instanced)
    DWORD   IOROMInit;          // shutdown vector address      0x0467
    BYTE    IntrFlag;           // flag to indicate interrupt   0x046B
    TMRDATA tmr;                // timer data                   0x046C
    BYTE    BIOSBreak;          // bit 7=1 if break key hit     0x0471 (WIN-instanced)
    WORD    ResetFlag;          // warm boot flag               0x0472
    HDSKDTA hdsk;               // hard disk data               0x0474
    BYTE    PrintTimOut[4];     // parallel port timeouts       0x0478
    BYTE    RS232TimOut[4];     // serial port timeouts         0x047C (WIN-instanced)
    WORD    KBBufferStart;      // keyboard buffer pointers     0x0480 (WIN-instanced)
    WORD    KBBufferEnd;        //                              0x0482 (WIN-instanced)
    VGADATA vga;                // EGA/VGA video data           0x0484 (WIN-instanced)
    XDSKDTA xdsk;               // extended disk data           0x048B
    BYTE    KBFlag3;            // extended keyboard data       0x0496 (WIN-instanced)
    BYTE    KBFlag2;            //                              0x0497 (WIN-instanced)
    RTCDATA rtc;                // real-time clock data         0x0498 (WIN-instanced)
    BYTE    RB1[95];            //                              0x04A1
//  DOS zeros a whole word @ 0x0500
//  In addition, DOS initialization stores the first sector of the root of
//  the boot drive at 0x0500 through 0x06FF, and references the cluster nos.
//  of the first two directory entries (IO.SYS and MSDOS.SYS) directly, at
//  offsets 0x051A and 0x053A, respectively;  as a result, the segment of
//  ResModePtr (below) would not default to zero if MSDOS.SYS had a last access
//  date its directory entry (@ 0x0520 + 0x12) -- fixed in DOS7
    BYTE    PrtScStatus;        // print-screen status          0x0500 (DOS-instanced)
    BYTE    PrtScStatusPlus1;   //                              0x0501 (DOS-instanced)
    BYTE    RB2[2];             //                              0x0502
//  DOS zeros a whole word @ 0x0504
    BYTE    SingleDrive;        // DOS single-drive status      0x0504 (WIN-instanced)
    BYTE    RB3[9];             //                              0x0505
    BYTE    ROMBasic[20];       // ROM Basic data               0x050E (DOS-instanced)
//  QBASIC uses the dword @ 0x0512 to save original INT 1Ch vector
    DSKPARM DisketteParms;      // DOS diskette parameters      0x0522
    BYTE    RB4[3];             //                              0x052D
//  Another bug in Windows 3.x: ResModePtr was not instanced, so installing the
//  resident portion of MODE in one VM would confuse a MODE command in another VM
    DWORD   ResModePtr;         // MODE's resident code ptr     0x0530 (DOS7-zeroed)
    DWORD   ZeroForIBM;         // DOS7-zeroed (for IBM)        0x0534 (DOS7-instanced)
    BYTE    RB5[8];             //                              0x0538
    DWORD   SaveIVT[46];        // original vectors    (DOS7)   0x0540
    BYTE    SaveMasterIMR;      // original master IMR (DOS7)   0x05F8
    BYTE    SaveSlaveIMR;       // original slave IMR  (DOS7)   0x05F9
    WORD    SaveReserved[2];    // reserved            (DOS7)   0x05FA
    WORD    SaveCheckSum;       // checksum of DOS7 vars        0x05FE
    BYTE    ScratchStack[256];  // temporarily used by BIOS     0x0600
} RB;                           // start of BIOS/DOS memory     0x0700
typedef RB *PRB;


/* XLATOFF */
typedef struct _MB {            /* mb */
    RB      rb;
    BYTE    abDOSMem[DOSMEM_SIZE];
    BYTE    abGrphMem[VIDMEM_GRPHSIZE];
    BYTE    abMonoMem[VIDMEM_MONOSIZE];
    BYTE    abColrMem[VIDMEM_COLRSIZE];
    BYTE    abROMMem[ROMMEM_SIZE];
    BYTE    abWrapMem[WRAPMEM_SIZE];
} MB;
typedef MB *PMB;
/* XLATON */


/*  Model byte equates (located at F000:FFFE in actual ROM)
 */
#define ROMMODEL_PC1            0x0FF   // PC-1, Expansion, Old PC2 (PC)
#define ROMMODEL_PC2            0x0FE   // PC-2, Newer              (XT)
#define ROMMODEL_JR             0x0FD   // HALO                     (JR)
#define ROMMODEL_AT             0x0FC   // Salmon (PC-3)            (AT)


/*  ROM format
 */
#define ROMHDR_SIGNATURE        0xAA55
#define ROMHDR_INCR             2048
#define BLOCKSIZE               512
#define BLOCKSPERPAGE           (PAGESIZE / BLOCKSIZE)

typedef struct _ROMHDR {        /* rh */
    WORD romhdr_wSignature;     // should be ROMHDR_SIGNATURE
    BYTE romhdr_bnBlocks;       // # of ROM blocks
    BYTE romhdr_abFiller[ROMHDR_INCR-3];
} ROMHDR;
typedef ROMHDR *PROMHDR;


#define PPI                     0x61
#define PPO                     0x61

#define PPO_MASK_IOCHECK        0x04    // disable system board MEM parity check
#define PPO_MASK_PCHECK         0x08    // disable IO "channel" parity check

#define PPI_IOCHECK             0x40    // IO "channel" parity bit
#define PPI_PCHECK              0x80    // system board MEM parity bit


/* ASM

        .errnz  size RB NE 700h

IFNDEF  WIN386

_ROMDATA segment at SEG_ROMDATA

RomData  RB  <>

_ROMDATA ends

ENDIF


SumVecs macro
        mov     si,rb_SaveIVT           ; chksum the old vector info
        mov     cx,46*2+1               ; # words to sum
        sub     ax,ax                   ; AX == sum
init_vecsum:
        add     ax,[si]
        add     si,2
        loop    init_vecsum
        cmp     ax,[si+4]               ; chksum ok?
endm

CopyVecs macro
        mov     di,rb_IVT               ; copy vectors back into the IVT
        mov     si,rb_SaveIVT
        mov     cx,20h*2
        rep     movsw                   ; copy vectors 00h-1Fh
        mov     di,INT_HDISKDISKETTE*4
        mov     cx,4*2
        rep     movsw                   ; copy vectors 40h-43h
        add     di,2*4
        movsw                           ; copy vector 46h
        movsw
        add     di,3*4
        movsw                           ; copy vector 4Ah
        movsw
        mov     di,INT_RTC*4
        mov     cx,8*2
        rep     movsw                   ; copy vectors 70h-77h
;
;   Also blow away some other vectors, so that certain
;   software doesn't think it's already loaded (eg, EMM386)
;
        sub     ax,ax
        mov     di,INT_EMS*4
        stosw
        stosw
endm

SumVecs386 macro
        mov     esi,rb_SaveIVT          ; chksum the old vector info
        mov     ecx,46*2+1              ; # words to sum
        sub     ax,ax                   ; AX == sum
init_vecsum:
        add     ax,[esi]
        add     esi,2
        loop    init_vecsum
        cmp     ax,[esi+4]              ; chksum ok?
endm

CopyVecs386 macro
        mov     edi,rb_IVT              ; copy vectors back into the IVT
        mov     esi,rb_SaveIVT
        mov     ecx,20h
        rep     movsd                   ; copy vectors 00h-1Fh
        mov     edi,INT_HDISKDISKETTE*4
        mov     ecx,4
        rep     movsd                   ; copy vectors 40h-43h
        add     edi,2*4
        movsd                           ; copy vector 46h
        add     edi,3*4
        movsd                           ; copy vector 4Ah
        mov     edi,INT_RTC*4
        mov     ecx,8
        rep     movsd                   ; copy vectors 70h-77h
;
;   Also blow away some other vectors, so that certain
;   software doesn't think it's already loaded (eg, EMM386)
;
        sub     eax,eax
        mov     edi,INT_EMS*4
        stosd
endm

ResetCPU macro  method
        IFDEF   SETSYSFLAG
        mov     al,KBDCMD_READ_CB       ; this bit of code attempts to insure
        out     KBDPORT_CMD,al          ; that the SysFlag bit in the 8042's
        jmp     $+2                     ; command byte is set, but that appears
@@:     in      al,KBDPORT_STATUS       ; to be its default state, so let's
        test    al,KBDSTATUS_OUTPUT     ; not muck with it if we don't have to...
        jz      @B
        jmp     $+2
        in      al,KBDPORT_DATA
        or      al,KBDCB_SYSFLAG
        push    ax
        call    WaitKbd
        mov     al,KBDCMD_WRITE_CB
        out     KBDPORT_CMD,al
        call    WaitKbd
        pop     ax
        jmp     $+2
        out     KBDPORT_DATA,al
        ENDIF   ;SETSYSFLAG

        IFNB    <method>
        mov     al,CMOSADDR_NMIOFF or CMOS_SHUTDOWN
        out     CMOSPORT_ADDR,al        ;
        jmp     $+2                     ;
        jmp     $+2                     ;
        mov     al,method
        out     CMOSPORT_DATA,al        ; set the CMOS shutdown byte
        ENDIF

        IFDEF   WAITKBD
@@:     in      al,KBDPORT_STATUS
        test    al,KBDSTATUS_INPUT
        jnz     @B
        ENDIF

        mov     al,KBDCMD_CPURESET      ;
        out     KBDPORT_CMD,al          ; reset CPU

        hlt                             ; just sit here until it does
        jmp     $-1                     ; don't let NMIs skip by either
                                        ; (not that I'm expecting any!)
endm

EnableNMI macro
        mov     al,CMOSADDR_NMION+CMOS_STATUSD
        out     CMOSPORT_ADDR,al
        jmp     $+2
        jmp     $+2
        in      al,CMOSPORT_DATA
endm

ResetNMI macro
	push	eax
        mov     al,CMOSADDR_NMIOFF+CMOS_STATUSD
        out     CMOSPORT_ADDR,al
        jmp     $+2
        jmp     $+2
        in      al,CMOSPORT_DATA

        in      al,PPI
        jmp     $+2
        jmp     $+2
        or      al,PPO_MASK_IOCHECK or PPO_MASK_PCHECK
        out     PPO,al                  ; Disable I/O check
        jmp     $+2
        jmp     $+2
        and     al,NOT (PPO_MASK_IOCHECK or PPO_MASK_PCHECK)
        out     PPO, al                 ; Enable I/O check
        jmp     $+2
        jmp     $+2

        mov     al,CMOSADDR_NMION+CMOS_STATUSD
        out     CMOSPORT_ADDR,al
        jmp     $+2
        jmp     $+2
        in      al,CMOSPORT_DATA
        pop     eax
endm

*/
