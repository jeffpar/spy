INCLUDE pic.INC
INCLUDE timer.INC
INCLUDE kbd.INC
INCLUDE disk.INC
INCLUDE cmos.INC
INCLUDE video.INC
SEG_ROMDATA	EQU	0000H
MB_ADDR	EQU	00000000H
MB_SIZE	EQU	SIZE MB
DOSMEM_SIZE	EQU	(000A0000H-00000700H)
ROMMEM_SIZE	EQU	(00100000H-000C0000H)
WRAPMEM_SIZE	EQU	0000FFF0H
INT_DIVERROR	EQU	<IDT_DIVERROR>
INT_DEBUG	EQU	<IDT_DEBUG>
INT_NMI	EQU	<IDT_NMI>
INT_BREAKPOINT	EQU	<IDT_BREAKPOINT>
INT_OVERFLOW	EQU	<IDT_OVERFLOW>
INT_PRINTSCREEN	EQU	<IDT_BOUNDS>
INT_INVOPCODE	EQU	<IDT_INVOPCODE>
INT_NOCP	EQU	<IDT_NOCP>
INT_TMR	EQU	08H
INT_KBD	EQU	09H
INT_SLAVE	EQU	0AH
INT_COM2	EQU	0BH
INT_COM1	EQU	0CH
INT_LPT2	EQU	0DH
INT_DISKETTE	EQU	0EH
INT_LPT1	EQU	0FH
INT_RTC	EQU	70H
INT_IRQ2	EQU	71H
INT_IRQ10	EQU	72H
INT_IRQ11	EQU	73H
INT_IRQ12	EQU	74H
INT_CP	EQU	75H
INT_DISK	EQU	76H
INT_IRQ15	EQU	77H
INT_EQUIPMENTCHECK	EQU	11H
INT_MEMORYCHECK	EQU	12H
INT_COMIO	EQU	14H
INT_SYSSERV	EQU	15H
INT_PRINTERIO	EQU	17H
INT_ROMBASIC	EQU	18H
INT_REBOOT	EQU	19H
RESETFLAG_WARM	EQU	1234H
INT15AH_KEYINTERCEPT	EQU	4FH
INT15AH_KEYSYSREQ	EQU	85H
INT15AH_BMOVE	EQU	87H
INT15AH_EXTMEM	EQU	88H
INT15AH_PMODE	EQU	89H
INT15AH_DEVBUSY	EQU	90H
INT15AH_INTCOMPLETE	EQU	91H
INT15AL_DISKETTE	EQU	01H
INT15AL_KEYBOARD	EQU	02H
EQUIP_FLOPPY	EQU	00001H
EQUIP_X87	EQU	00002H
EQUIP_VIDEOMASK	EQU	00030H
EQUIP_COLOR40VIDEO	EQU	00010H
EQUIP_COLOR80VIDEO	EQU	00020H
EQUIP_MONOVIDEO	EQU	00030H
EQUIP_FLOPPYMASK	EQU	000C0H
EQUIP_COMMASK	EQU	00E00H
EQUIP_PRINTERMASK	EQU	0C000H

RB	STRUC
rb_IVT	DD	256 DUP (?)
rb_RS232Base	DW	4 DUP (?)
rb_PrinterBase	DW	4 DUP (?)
rb_EquipFlag	DW	?
rb_MfgTest	DB	?
rb_MemorySize	DW	?
rb_MfgErrFlag	DB	?
rb_MfgErrCode	DB	?
rb_kbd	DB	SIZE KBDDATA DUP (?)
rb_dsk	DB	SIZE DSKDATA DUP (?)
rb_vid	DB	SIZE VIDDATA DUP (?)
rb_IOROMInit	DD	?
rb_IntrFlag	DB	?
rb_tmr	DB	SIZE TMRDATA DUP (?)
rb_BIOSBreak	DB	?
rb_ResetFlag	DW	?
rb_hdsk	DB	SIZE HDSKDTA DUP (?)
rb_PrintTimOut	DB	4 DUP (?)
rb_RS232TimOut	DB	4 DUP (?)
rb_KBBufferStart	DW	?
rb_KBBufferEnd	DW	?
rb_vga	DB	SIZE VGADATA DUP (?)
rb_xdsk	DB	SIZE XDSKDTA DUP (?)
rb_KBFlag3	DB	?
rb_KBFlag2	DB	?
rb_rtc	DB	SIZE RTCDATA DUP (?)
rb_RB1	DB	95 DUP (?)
rb_PrtScStatus	DB	?
rb_PrtScStatusPlus1	DB	?
rb_RB2	DB	2 DUP (?)
rb_SingleDrive	DB	?
rb_RB3	DB	9 DUP (?)
rb_ROMBasic	DB	20 DUP (?)
rb_DisketteParms	DB	SIZE DSKPARM DUP (?)
rb_RB4	DB	3 DUP (?)
rb_ResModePtr	DD	?
rb_ZeroForIBM	DD	?
rb_RB5	DB	8 DUP (?)
rb_SaveIVT	DD	46 DUP (?)
rb_SaveMasterIMR	DB	?
rb_SaveSlaveIMR	DB	?
rb_SaveReserved	DW	2 DUP (?)
rb_SaveCheckSum	DW	?
rb_ScratchStack	DB	256 DUP (?)
RB	ENDS
ROMMODEL_PC1	EQU	0FFH
ROMMODEL_PC2	EQU	0FEH
ROMMODEL_JR	EQU	0FDH
ROMMODEL_AT	EQU	0FCH
ROMHDR_SIGNATURE	EQU	0AA55H
ROMHDR_INCR	EQU	2048
BLOCKSIZE	EQU	512
BLOCKSPERPAGE	EQU	<(PAGESIZE/BLOCKSIZE)>

ROMHDR	STRUC
rh_romhdr_wSignature	DW	?
rh_romhdr_bnBlocks	DB	?
rh_romhdr_abFiller	DB	ROMHDR_INCR-3 DUP (?)
ROMHDR	ENDS
PPI	EQU	61H
PPO	EQU	61H
PPO_MASK_IOCHECK	EQU	04H
PPO_MASK_PCHECK	EQU	08H
PPI_IOCHECK	EQU	40H
PPI_PCHECK	EQU	80H

        .errnz  size RB NE 700h

IFNDEF  WIN386

_ROMDATA segment at SEG_ROMDATA

RomData  RB  <>

_ROMDATA ends

ENDIF


SumVecs macro
        mov     si,rb_SaveIVT
        mov     cx,46*2+1
        sub     ax,ax
init_vecsum:
        add     ax,[si]
        add     si,2
        loop    init_vecsum
        cmp     ax,[si+4]
endm

CopyVecs macro
        mov     di,rb_IVT
        mov     si,rb_SaveIVT
        mov     cx,20h*2
        rep     movsw
        mov     di,INT_HDISKDISKETTE*4
        mov     cx,4*2
        rep     movsw
        add     di,2*4
        movsw
        movsw
        add     di,3*4
        movsw
        movsw
        mov     di,INT_RTC*4
        mov     cx,8*2
        rep     movsw




        sub     ax,ax
        mov     di,INT_EMS*4
        stosw
        stosw
endm

SumVecs386 macro
        mov     esi,rb_SaveIVT
        mov     ecx,46*2+1
        sub     ax,ax
init_vecsum:
        add     ax,[esi]
        add     esi,2
        loop    init_vecsum
        cmp     ax,[esi+4]
endm

CopyVecs386 macro
        mov     edi,rb_IVT
        mov     esi,rb_SaveIVT
        mov     ecx,20h
        rep     movsd
        mov     edi,INT_HDISKDISKETTE*4
        mov     ecx,4
        rep     movsd
        add     edi,2*4
        movsd
        add     edi,3*4
        movsd
        mov     edi,INT_RTC*4
        mov     ecx,8
        rep     movsd




        sub     eax,eax
        mov     edi,INT_EMS*4
        stosd
endm

ResetCPU macro  method
        IFDEF   SETSYSFLAG
        mov     al,KBDCMD_READ_CB
        out     KBDPORT_CMD,al
        jmp     $+2
@@:     in      al,KBDPORT_STATUS
        test    al,KBDSTATUS_OUTPUT
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
        ENDIF

        IFNB    <method>
        mov     al,CMOSADDR_NMIOFF or CMOS_SHUTDOWN
        out     CMOSPORT_ADDR,al
        jmp     $+2
        jmp     $+2
        mov     al,method
        out     CMOSPORT_DATA,al
        ENDIF

        IFDEF   WAITKBD
@@:     in      al,KBDPORT_STATUS
        test    al,KBDSTATUS_INPUT
        jnz     @B
        ENDIF

        mov     al,KBDCMD_CPURESET
        out     KBDPORT_CMD,al

        hlt
        jmp     $-1

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
        out     PPO,al
        jmp     $+2
        jmp     $+2
        and     al,NOT (PPO_MASK_IOCHECK or PPO_MASK_PCHECK)
        out     PPO, al
        jmp     $+2
        jmp     $+2

        mov     al,CMOSADDR_NMION+CMOS_STATUSD
        out     CMOSPORT_ADDR,al
        jmp     $+2
        jmp     $+2
        in      al,CMOSPORT_DATA
        pop     eax
endm


