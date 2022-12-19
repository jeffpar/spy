SEG_DOSDATA	EQU	0070H
INT_DOSEXIT	EQU	20H
INT_DOS	EQU	21H
DOS_ABORT	EQU	00H
DOS_STD_CON_INPUT	EQU	01H
DOS_STD_CON_OUTPUT	EQU	02H
DOS_STD_AUX_INPUT	EQU	03H
DOS_STD_AUX_OUTPUT	EQU	04H
DOS_STD_PRINTER_OUTPUT	EQU	05H
DOS_RAW_CON_IO	EQU	06H
DOS_RAW_CON_INPUT	EQU	07H
DOS_STD_CON_INPUT_NO_ECHO	EQU	08H
DOS_STD_CON_STRING_OUTPUT	EQU	09H
DOS_STD_CON_STRING_INPUT	EQU	0AH
DOS_STD_CON_INPUT_STATUS	EQU	0BH
DOS_STD_CON_INPUT_FLUSH	EQU	0CH
DOS_DISK_RESET	EQU	0DH
DOS_SET_DEFAULT_DRIVE	EQU	0EH
DOS_FCB_OPEN	EQU	0FH
DOS_FCB_CLOSE	EQU	10H
DOS_DIR_SEARCH_FIRST	EQU	11H
DOS_DIR_SEARCH_NEXT	EQU	12H
DOS_FCB_DELETE	EQU	13H
DOS_FCB_SEQ_READ	EQU	14H
DOS_FCB_SEQ_WRITE	EQU	15H
DOS_FCB_CREATE	EQU	16H
DOS_FCB_RENAME	EQU	17H
DOS_RESERVED18	EQU	18H
DOS_GET_DEFAULT_DRIVE	EQU	19H
DOS_SET_DMA	EQU	1AH
DOS_RESERVED1B	EQU	1BH
DOS_RESERVED1C	EQU	1CH
DOS_RESERVED1D	EQU	1DH
DOS_RESERVED1E	EQU	1EH
DOS_GET_DEFAULT_DPB	EQU	1FH
DOS_RESERVED20	EQU	20H
DOS_FCB_RANDOM_READ	EQU	21H
DOS_FCB_RANDOM_WRITE	EQU	22H
DOS_GET_FCB_FILE_LENGTH	EQU	23H
DOS_GET_FCB_POSITION	EQU	24H
DOS_SET_INTERRUPT_VECTOR	EQU	25H
DOS_CREATE_PDB	EQU	26H
DOS_FCB_RANDOM_READ_BLOCK	EQU	27H
DOS_FCB_RANDOM_WRITE_BLOCK	EQU	28H
DOS_PARSE_FILE_DESCRIPTOR	EQU	29H
DOS_GET_DATE	EQU	2AH
DOS_SET_DATE	EQU	2BH
DOS_GET_TIME	EQU	2CH
DOS_SET_TIME	EQU	2DH
DOS_SET_VERIFY_ON_WRITE	EQU	2EH
DOS_GET_DMA	EQU	2FH
DOS_GET_VERSION	EQU	30H
DOS_KEEP_PROCESS	EQU	31H
DOS_GET_DPB	EQU	32H
DOS_SET_CTRL_C_TRAPPING	EQU	33H
DOS_GET_INDOS_FLAG	EQU	34H
DOS_GET_INTERRUPT_VECTOR	EQU	35H
DOS_GET_DRIVE_FREESPACE	EQU	36H
DOS_CHAR_OPER	EQU	37H
DOS_INTERNATIONAL_SUPPORT	EQU	38H
DOS_MKDIR	EQU	39H
DOS_RMDIR	EQU	3AH
DOS_CHDIR	EQU	3BH
DOS_CREAT	EQU	3CH
DOS_OPEN	EQU	3DH
DOS_CLOSE	EQU	3EH
DOS_READ	EQU	3FH
DOS_WRITE	EQU	40H
DOS_UNLINK	EQU	41H
DOS_LSEEK	EQU	42H
DOS_CHMOD	EQU	43H
DOS_IOCTL	EQU	44H
DOS_XDUP	EQU	45H
DOS_XDUP2	EQU	46H
DOS_GETDIR	EQU	47H
DOS_ALLOC	EQU	48H
DOS_DEALLOC	EQU	49H
DOS_SETBLOCK	EQU	4AH
DOS_EXEC	EQU	4BH
DOS_EXIT	EQU	4CH
DOS_WAIT	EQU	4DH
DOS_FIND_FIRST	EQU	4EH
DOS_FIND_NEXT	EQU	4FH
DOS_SET_CURRENT_PDB	EQU	50H
DOS_GET_CURRENT_PDB	EQU	51H
DOS_GET_SYSINIT_VARS	EQU	52H
DOS_SET_DPB	EQU	53H
DOS_GET_VERIFY_ON_WRITE	EQU	54H
DOS_DUP_PDB	EQU	55H
DOS_RENAME	EQU	56H
DOS_FILE_TIMES	EQU	57H
DOS_RESERVED58	EQU	58H
DOS_GET_EXTENDED_ERROR	EQU	59H
DOS_CREATE_TEMP_FILE	EQU	5AH
DOS_CREATE_NEW_FILE	EQU	5BH
DOS_LOCK_OPER	EQU	5CH
DOS_SERVER_CALL	EQU	5DH
DOS_USER_OPER	EQU	5EH
DOS_ASSIGN_OPER	EQU	5FH
DOS_XNAMETRANS	EQU	60H
DOS_JSA	EQU	61H
DOS_GET_CURRENT_PSP	EQU	62H
DOS_HONGEUL	EQU	63H
DOS_SYSINITSLEAZE	EQU	64H
DOSERR_BADFUNCTION	EQU	01H
DOSERR_NOFILE	EQU	02H
DOSERR_NOPATH	EQU	03H
DOSERR_NOHANDLES	EQU	04H
DOSERR_NOACCESS	EQU	05H
DOSERR_BADHANDLE	EQU	06H
DOSERR_BADARENA	EQU	07H
DOSERR_NOMEMORY	EQU	08H
DOSERR_BADADDRESS	EQU	09H
DOSERR_BADENVIRONMENT	EQU	0AH
DOSERR_BADFORMAT	EQU	0BH
DOSERR_BADACCESS	EQU	0CH
DOSERR_BADDATA	EQU	0DH
DOSERR_BADUNIT	EQU	0EH
DOSERR_BADDISK	EQU	0FH
DOSERR_DELETECURDIR	EQU	10H
DOSERR_NOTSAMEDEVICE	EQU	11H
DOSERR_NOMOREFILES	EQU	12H
DOSERR_WRITEPROTECT	EQU	13H
DOSERR_UNKNOWNUNIT	EQU	14H
DOSERR_DRIVENOTREADY	EQU	15H
DOSERR_BADCOMMAND	EQU	16H
DOSERR_DATA	EQU	17H
DOSERR_BADREQUEST	EQU	18H
DOSERR_SEEK	EQU	19H
DOSERR_UNKNOWNMEDIA	EQU	1AH
DOSERR_BADSECTOR	EQU	1BH
DOSERR_NOPAPER	EQU	1CH
DOSERR_WRITEFAULT	EQU	1DH
DOSERR_READFAULT	EQU	1EH
DOSERR_GENERALFAILURE	EQU	1FH
DOSERR_NOTSHARED	EQU	20H
DOSERR_LOCKED	EQU	21H
DOSERR_DISKCHANGE	EQU	22H
ifndef SEEK_SET
SEEK_SET	EQU	00H
SEEK_CUR	EQU	01H
SEEK_END	EQU	02H
endif

BPB	STRUC
bpb_wBytesSector	DW	?
bpb_bSectorsCluster	DB	?
bpb_wReservedSectors	DW	?
bpb_bFATs	DB	?
bpb_wRootEntries	DW	?
bpb_wTotalSectors	DW	?
bpb_bMediaDescriptor	DB	?
bpb_wSectorsFAT	DW	?
bpb_wSectorsTrack	DW	?
bpb_wHeads	DW	?
bpb_dwHiddenSectors	DD	?
bpb_dwTotalSectors	DD	?
BPB	ENDS
EXT_BOOT_SIGNATURE	EQU	41

EBR	STRUC
ebr_abJmp	DB	3 DUP (?)
ebr_achOEM	DB	8 DUP (?)
ebr_bpb	DB	SIZE BPB DUP (?)
ebr_bDrive	DB	?
ebr_bCurHD	DB	?
ebr_bSignature	DB	?
ebr_dwSerialNumber	DD	?
ebr_achVolume	DB	11 DUP (?)
ebr_achSystemID	DB	8 DUP (?)
EBR	ENDS

FCBHDR	STRUC
fcbhdr_bExtFCBFlag	DB	?
fcbhdr_abReserved	DB	5 DUP (?)
fcbhdr_bDirAttr	DB	?
FCBHDR	ENDS

FCB	STRUC
fcb_chDrive	DB	?
fcb_achName	DB	8 DUP (?)
fcb_achExtension	DB	3 DUP (?)
fcb_wBlock	DW	?
fcb_wRecSize	DW	?
fcb_dwFileSize	DD	?
fcb_wDate	DW	?
fcb_wTime	DW	?
fcb_abReserved	DB	8 DUP (?)
fcb_bRec	DB	?
fcb_dwRandRec	DD	4 DUP (?)
FCB	ENDS

EXTFCB	STRUC
extfcb_fcbhdr	DB	SIZE FCBHDR DUP (?)
extfcb_fcb	DB	SIZE FCB DUP (?)
EXTFCB	ENDS

MINFCB	STRUC
minfcb_chDrive	DB	?
minfcb_achName	DB	8 DUP (?)
minfcb_achExtension	DB	3 DUP (?)
minfcb_wBlock	DW	?
minfcb_wRecSize	DW	?
MINFCB	ENDS
ARENA_OWNER_SYSTEM	EQU	0
ARENA_SIG	EQU	4DH
ARENA_SIG_END	EQU	5AH

MCB	STRUC
arena_signature	DB	?
arena_owner	DW	?
arena_size	DW	?
arena_reserved	DB	3 DUP (?)
arena_name	DB	8 DUP (?)
MCB	ENDS
MAX_PROCESSFILES	EQU	20

PSP	STRUC
psp_intAbort	DW	?
psp_segNext	DW	?
psp_bUnused	DB	?
psp_abCPMCall	DB	5 DUP (?)
psp_vecTerminate	DD	?
psp_vecCtrlC	DD	?
psp_vecFatalAbort	DD	?
psp_pspParent	DW	?
psp_abJFN	DB	MAX_PROCESSFILES DUP (?)
psp_segEnviron	DW	?
psp_lpUserStack	DD	?
psp_maxJFNs	DW	?
psp_lpJFNTable	DD	?
psp_lpNextPDB	DD	?
psp_bInterCon	DB	?
psp_bAppendFlags	DB	?
psp_wNovell	DW	?
psp_wDOSVersion	DW	?
psp_wChain	DW	?
psp_wPartition	DW	?
psp_wNextPDB	DW	?
psp_lpGlobalHeap	DD	?
psp_lpEntryStack	DD	?
psp_abSysCall	DB	3 DUP (?)
psp_wUnused	DW	?
psp_fcb1hdr	DB	SIZE FCBHDR DUP (?)
psp_fcb1	DB	SIZE MINFCB DUP (?)
psp_fcb2	DB	SIZE MINFCB DUP (?)
psp_abFCBData	DB	4 DUP (?)
psp_cbCommand	DB	?
psp_achCommand	DB	127 DUP (?)
PSP	ENDS
DIRATTR_READONLY	EQU	01H
DIRATTR_HIDDEN	EQU	02H
DIRATTR_SYSTEM	EQU	04H
DIRATTR_VOLLABEL	EQU	08H
DIRATTR_DIRECTORY	EQU	10H
DIRATTR_ARCHIVE	EQU	20H
ACCESS_READ	EQU	00H
ACCESS_WRITE	EQU	01H
ACCESS_RW	EQU	02H
ACCESS_EXEC	EQU	03H
ACCESS_NODATE	EQU	04H
SHARE_COMP	EQU	00H
SHARE_DENYALL	EQU	10H
SHARE_DENYWRITE	EQU	20H
SHARE_DENYREAD	EQU	30H
SHARE_DENYNONE	EQU	40H
SHARE_NOINHERIT	EQU	80H
DRIVE_CURRENT	EQU	00H
OLDEXE_SIG	EQU	5A4DH

OLDEXE	STRUC
oe_wSig	DW	?
oe_cbExtra	DW	?
oe_cBlocks	DW	?
oe_cRelCnt	DW	?
oe_cpHeader	DW	?
oe_cpMinExtra	DW	?
oe_cpMaxExtra	DW	?
oe_offStack	DW	?
oe_segStack	DW	?
oe_wCheckSum	DW	?
oe_segExec	DW	?
oe_offExec	DW	?
oe_offRelocs	DW	?
oe_iOverlay	DW	?
OLDEXE	ENDS

DOS	STRUC
dos_harddrv_offset	DW	?
dos_harddrv	DB	?
dos_DosDataSeg	DW	?
dos_Bios_i2f	DB	3 DUP (?)
dos_LowFlags	DW	?
dos_ROMStartAddr	DW	?
dos_model_byte	DB	?
dos_secondary_model_byte	DB	?
dos_xms	DD	?
dos_ptrsav	DD	?
dos_res_dev_list	DB	18 DUP (?)
dos_auxdev	DB	18 DUP (?)
dos_prndev	DB	18 DUP (?)
dos_timdev	DB	18 DUP (?)
dos_dskdev	DB	10 DUP (?)
dos_drvmax	DB	?
dos_keyrd_func	DB	?
dos_com1dev	DB	18 DUP (?)
dos_lpt1dev	DB	18 DUP (?)
dos_lpt2dev	DB	18 DUP (?)
dos_lpt3dev	DB	18 DUP (?)
dos_keysts_func	DW	?
dos_OrigInt13	DD	?
dos_com2dev	DB	18 DUP (?)
dos_com3dev	DB	18 DUP (?)
dos_com4dev	DB	18 DUP (?)
dos_tim_entry	DB	5 DUP (?)
dos_dsk_entry	DB	5 DUP (?)
dos_Start_BDS	DD	?
dos_DiskSectorPtr	DD	?
dos_RomVector10	DB	?
dos_OldInt10	DD	?
dos_RomVector13	DB	?
dos_OldInt13	DD	?
dos_RomVector15	DB	?
dos_OldInt15	DD	?
dos_RomVector19	DB	?
dos_OldInt19	DD	?
dos_RomVector1B	DB	?
dos_OldInt1B	DD	?
dos_RomVector4F	DB	?
dos_OldInt4F	DD	?
dos_cdev_off	DW	?
dos_BiosCodeSeg	DW	?
DOS	ENDS

MSDOS	STRUC
msdos_DOSDATABEGIN	DD	?
msdos_DataVersion	DB	10 DUP (?)
msdos_MYNUM	DW	?
msdos_FCBLRU	DW	?
msdos_OpenLRU	DW	?
msdos_OEM_HANDLER	DD	?
msdos_LeaveAddr	DW	?
msdos_RetryCount	DW	?
msdos_RetryLoop	DW	?
msdos_LastBuffer	DD	?
msdos_CONTPOS	DW	?
msdos_arena_head	DW	?
msdos_SYSINITVAR	DD	?
msdos_sft_addr	DD	?
msdos_BCLOCK	DD	?
msdos_BCON	DD	?
msdos_MAXSEC	DW	?
msdos_BUFFHEAD	DD	?
msdos_CDSADDR	DD	?
msdos_sftFCB	DD	?
msdos_KeepCount	DW	?
msdos_NUMIO	DB	?
msdos_CDSCOUNT	DB	?
msdos_NULDEV	DB	18 DUP (?)
msdos_Splices	DB	?
msdos_Special_Entries	DW	?
msdos_SetVerTbl	DD	?
msdos_ChkCopyProt	DW	?
msdos_A20OFF_PSP	DW	?
msdos_BUFFERS_PARM1	DW	?
msdos_BUFFERS_PARM2	DW	?
msdos_BOOTDRIVE	DB	?
msdos_DDMOVE	DB	?
msdos_EXT_MEM_SIZE	DW	?
msdos_BufferQueue	DD	?
msdos_DirtyBufferCount	DW	?
msdos_SC_CACHE_PTR	DD	?
msdos_SC_CACHE_COUNT	DW	?
msdos_BuffInHMA	DB	?
msdos_LoMemBuff	DD	?
msdos_CritCallTablePtr	DW	?
msdos_UNUSED_BYTE	DB	?
msdos_CL0FATENTRY	DW	?
msdos_IoStatFail	DB	?
msdos_ALLOCMSAVE	DB	?
msdos_A20OFF_COUNT	DB	?
msdos_DOS_FLAG	DB	?
msdos_UNPACK_OFFSET	DW	?
msdos_UMBFLAG	DB	?
msdos_SAVE_AX	DW	?
msdos_UMB_HEAD	DW	?
msdos_START_ARENA	DW	?
msdos_JShare	DB	60 DUP (?)
msdos_dummy_sftabl	DB	301 DUP (?)
msdos_CARPOS	DB	?
msdos_STARTPOS	DB	?
msdos_INBUF	DB	128 DUP (?)
msdos_CONBUF	DB	131 DUP (?)
msdos_PFLAG	DB	?
msdos_VERFLG	DB	?
msdos_CharCo	DB	?
msdos_chSwitch	DB	?
msdos_AllocMethod	DB	?
msdos_fShare	DB	?
msdos_DIFFNAM	DB	?
msdos_MYNAME	DB	16 DUP (?)
msdos_CritPatch	DB	11 DUP (?)
msdos_ErrorMode	DB	?
msdos_INDOS	DB	?
msdos_WPErr	DB	?
msdos_EXTERR_LOCUS	DB	?
msdos_EXTERR	DW	?
msdos_EXTERR_ACTION	DB	?
msdos_EXTERR_CLASS	DB	?
msdos_EXTERRPT	DD	?
msdos_DMAADD	DD	?
msdos_CurrentPDB	DW	?
msdos_ConC_spsave	DW	?
msdos_exit_code	DW	?
msdos_CURDRV	DB	?
msdos_CNTCFLAG	DB	?
msdos_CPSWFLAG	DB	?
msdos_CPSWSAVE	DB	?
msdos_USER_IN_AX	DW	?
msdos_PROC_ID	DW	?
msdos_USER_ID	DW	?
msdos_FirstArena	DW	?
msdos_BestArena	DW	?
msdos_LastArena	DW	?
msdos_EndMem	DW	?
msdos_LASTENT	DW	?
msdos_FAILERR	DB	?
msdos_ALLOWED	DB	?
msdos_NoSetDir	DB	?
msdos_DidCTRLC	DB	?
msdos_SpaceFlag	DW	?
msdos_DAY	DB	?
msdos_MONTH	DB	?
msdos_YEAR	DW	?
msdos_DAYCNT	DW	?
msdos_WEEKDAY	DB	?
msdos_CONSWAP	DB	?
msdos_IDLEINT	DB	?
msdos_fAborting	DB	?
msdos_DEVCALL	DB	13 DUP (?)
msdos_CALLUNIT	DB	?
msdos_CALLRBYT	DB	?
msdos_CALLVIDM	DB	3 DUP (?)
msdos_CallBPB	DW	?
msdos_CALLSSEC	DW	?
msdos_CALLVIDRW	DD	?
msdos_CALLNEWSC	DD	?
msdos_CALLDEVAD	DD	?
msdos_IOCall	DB	13 DUP (?)
msdos_IORCHR	DB	?
msdos_IOXAD	DD	?
msdos_IOSCNT	DW	?
msdos_IOSSEC	DW	?
msdos_DSKSTCALL	DW	?
msdos_DSKSTCOM	DB	?
msdos_DSKSTST	DB	10 DUP (?)
msdos_DSKCHRET	DB	5 DUP (?)
msdos_DSKSTCNT	DD	?
msdos_CreatePDB	DB	?
msdos_Lock_Buffer	DB	9 DUP (?)
msdos_UserNum	DB	3 DUP (?)
msdos_OEMNum	DB	?
msdos_TIMEBUF	DB	6 DUP (?)
msdos_DEVIOBUF	DW	?
msdos_OPENBUF	DB	128 DUP (?)
msdos_RenBuf	DB	128 DUP (?)
msdos_SEARCHBUF	DB	53 DUP (?)
msdos_DummyCDS	DB	88 DUP (?)
msdos_NAME1	DB	12 DUP (?)
msdos_NAME2	DB	13 DUP (?)
msdos_DESTSTART	DB	7 DUP (?)
msdos_ATTRIB	DB	?
msdos_EXTFCB	DB	?
msdos_SATTRIB	DB	?
msdos_open_access	DB	?
msdos_FoundDel	DB	?
msdos_Found_dev	DB	?
msdos_fSplice	DB	?
msdos_fSharing	DB	?
msdos_SECCLUSPOS	DB	?
msdos_TRANS	DB	?
msdos_READOP	DB	?
msdos_THISDRV	DB	?
msdos_CLUSFAC	DB	?
msdos_CLUSSPLIT	DB	?
msdos_INSMODE	DB	?
msdos_cMeta	DB	?
msdos_VOLID	DB	?
msdos_exit_type	DW	?
msdos_CREATING	DB	?
msdos_DELALL	DB	?
msdos_EXITHOLD	DD	?
msdos_user_SP	DW	?
msdos_user_SS	DW	?
msdos_CONTSTK	DW	?
msdos_THISDPB	DD	?
msdos_CLUSSAVE	DW	?
msdos_CLUSSEC	DD	?
msdos_PREREAD	DW	?
msdos_FATBYT	DW	?
msdos_FATBYTE	DW	?
msdos_DEVPT	DD	?
msdos_THISSFT	DD	?
msdos_THISCDS	DD	?
msdos_THISFCB	DD	?
msdos_SFN	DW	?
msdos_JFN	DW	?
msdos_PJFN	DD	?
msdos_WFP_START	DW	?
msdos_REN_WFP	DW	?
msdos_CURR_DIR_END	DW	?
msdos_NEXTADD	DW	?
msdos_LASTPOS	DW	?
msdos_CLUSNUM	DW	?
msdos_DIRSEC	DD	?
msdos_DIRSTART	DW	?
msdos_SECPOS	DD	?
msdos_VALSEC	DD	?
msdos_BYTSECPOS	DW	?
msdos_BYTPOS	DD	?
msdos_BYTCNT1	DW	?
msdos_BYTCNT2	DW	?
msdos_SECCNT	DW	?
msdos_ENTFREE	DW	?
msdos_ENTLAST	DW	?
msdos_NXTCLUSNUM	DW	?
msdos_GROWCNT	DD	?
msdos_CURBUF	DD	?
msdos_CONSft	DD	?
msdos_SaveBX	DW	?
msdos_SaveDS	DW	?
msdos_restore_tmp	DW	?
msdos_NSS	DW	?
msdos_NSP	DW	?
msdos_EXTOPEN_FLAG	DW	?
msdos_EXTOPEN_ON	DB	?
msdos_EXTOPEN_IO_MODE	DW	?
msdos_SAVE_DI	DW	?
msdos_SAVE_ES	DW	?
msdos_SAVE_DX	DW	?
msdos_SAVE_CX	DW	?
msdos_SAVE_BX	DW	?
msdos_SAVE_SI	DW	?
msdos_SAVE_DS	DW	?
msdos_HIGH_SECTOR	DW	?
msdos_OffsetMagicPatch	DW	?
msdos_DISK_FULL	DB	?
msdos_TEMP_VAR	DW	?
msdos_TEMP_VAR2	DW	?
msdos_DrvErr	DB	?
msdos_DOS34_FLAG	DW	?
msdos_NO_FILTER_PATH	DD	?
msdos_NO_FILTER_DPATH	DD	?
msdos_AbsRdWr_SS	DW	?
msdos_AbsRdWr_SP	DW	?
msdos_UU_Callback_flag	DB	?
msdos_RENAMEDMA	DB	384 DUP (?)
msdos_AuxStack	DB	384 DUP (?)
msdos_DskStack	DB	384 DUP (?)
msdos_PRINTER_FLAG	DB	?
msdos_VOLCHNG_FLAG	DB	?
msdos_VIRTUAL_OPEN	DB	?
msdos_FSeek_drive	DB	?
msdos_FSeek_firclus	DW	?
msdos_FSeek_logclus	DW	?
msdos_FSeek_logsave	DW	?
msdos_TEMP_DOSLOC	DW	?
msdos_SWAP_END	DB	?
msdos_UCASE_TAB	DB	130 DUP (?)
msdos_FILE_UCASE_TAB	DB	130 DUP (?)
msdos_FILE_CHAR_TAB	DB	48 DUP (?)
msdos_COLLATE_TAB	DB	258 DUP (?)
msdos_DBCS_TAB	DB	18 DUP (?)
msdos_MAP_CASE	DB	18 DUP (?)
msdos_InterChar	DB	?
msdos_InterCon	DB	?
msdos_SaveCurFlg	DB	?
msdos_TEMPSEG	DW	?
msdos_redir_patch	DB	?
msdos_DosHasHMA	DB	?
msdos_fixexepatch	DW	?
msdos_RationalPatchPtr	DW	?
msdos_MSVERSION	DW	?
msdos_YRTAB	DB	8 DUP (?)
msdos_MONTAB	DB	12 DUP (?)
msdos_SysInitTable	DB	8 DUP (?)
msdos_FastOpenTable	DB	10 DUP (?)
msdos_FastFlg	DB	?
msdos_FastOpen_Ext_Info	DB	11 DUP (?)
msdos_Dir_Info_Buff	DB	32 DUP (?)
msdos_Next_Element_Start	DW	?
msdos_USER_SP_2F	DW	?
msdos_Packet_Temp	DB	28 DUP (?)
msdos_SCAN_FLAG	DB	?
msdos_DATE_FLAG	DW	?
msdos_AbsDskErr	DW	?
msdos_NO_NAME_ID	DB	11 DUP (?)
msdos_LOOKSIZ	DB	?
msdos_snuldev	DB	6 DUP (?)
msdos_inuldev	DB	62 DUP (?)
msdos_ERR_TABLE_21	DB	128 DUP (?)
msdos_ERR_TABLE_24	DB	80 DUP (?)
msdos_ErrMap24	DB	16 DUP (?)
msdos_SPECIAL_VERSION	DW	?
msdos_OLD_FIRSTCLUS	DW	?
msdos_exec_init_SP	DW	?
msdos_exec_init_SS	DW	?
msdos_exec_init_IP	DW	?
msdos_exec_init_CS	DW	?
msdos_exec_signature	DW	?
msdos_exec_len_mod_512	DW	?
msdos_exec_pages	DW	?
msdos_exec_rle_count	DW	?
msdos_exec_par_dir	DW	?
msdos_exec_min_BSS	DW	?
msdos_exec_max_BSS	DW	?
msdos_exec_SS	DW	?
msdos_exec_SP	DW	?
msdos_exec_chksum	DW	?
msdos_exec_IP	DW	?
msdos_exec_CS	DW	?
msdos_exec_rle_table	DW	?
msdos_Win386_Info	DB	22 DUP (?)
msdos_Instance_Table	DB	58 DUP (?)
msdos_Opt_Instance_Table	DB	10 DUP (?)
msdos_Win386_DOSVars	DB	14 DUP (?)
msdos_IsWin386	DB	?
msdos_DummyErrorModeCode	DB	15 DUP (?)
msdos_DummyErrorModeCode2	DB	10 DUP (?)
msdos_LocalSFT	DB	5 DUP (?)
msdos_DosIntTable	DD	8 DUP (?)
msdos_SS_Save	DW	?
msdos_SP_Save	DW	?
msdos_i0patch	DB	5 DUP (?)
msdos_ldivov_jmp	DB	5 DUP (?)
msdos_i20patch	DB	5 DUP (?)
msdos_lquit_jmp	DB	5 DUP (?)
msdos_lcommand	DB	5 DUP (?)
msdos_lcommand_jmp	DB	5 DUP (?)
msdos_i25patch	DB	5 DUP (?)
msdos_labsdrd_jmp	DB	5 DUP (?)
msdos_labsdwrt	DB	5 DUP (?)
msdos_labsdwrt_jmp	DB	5 DUP (?)
msdos_i27patch	DB	5 DUP (?)
msdos_lstay_resident_jmp	DB	5 DUP (?)
msdos_i2fpatch	DB	5 DUP (?)
msdos_lint2f_jmp	DB	5 DUP (?)
msdos_cpmpatch	DB	5 DUP (?)
msdos_lcallentry_jmp	DB	5 DUP (?)
msdos_DosRetAddr23	DD	?
msdos_DosRetAddr24	DD	?
msdos_LowInt23	DB	20 DUP (?)
msdos_LowInt24	DB	20 DUP (?)
msdos_LowInt28	DB	6 DUP (?)
msdos_disa20_xfer	DB	24 DUP (?)
msdos_disa20_iret	DB	41 DUP (?)
msdos_lirett	DB	13 DUP (?)
msdos_XMMcontrol	DB	155 DUP (?)
msdos_RatBugCode	DB	309 DUP (?)
msdos_COUNTRY_CDPG	DB	142 DUP (?)
msdos_NLS_DATA	DB	58 DUP (?)
msdos_CritSecCallTable	DB	20 DUP (?)
msdos_Rational386PatchPtr	DB	8 DUP (?)
msdos_sftabl	DB	301 DUP (?)
msdos_DOSDATAEND	DB	?
MSDOS	ENDS

IFNDEF  WIN386

_DOSDATA segment at SEG_DOSDATA

DosData  DOS <>

_DOSDATA ends

ENDIF


