/*
 *  Disk Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define INT_DISKIO              0x13

#define INT_DISKETTEPARMS       0x1E
#define INT_HDISKDISKETTE       0x40
#define INT_HDISK0PARMS         0x41
#define INT_HDISK1PARMS         0x46


#define DISKIO_RESET            0x00
#define DISKIO_DISKSTATUS       0x01
#define DISKIO_READSECTORS      0x02
#define DISKIO_WRITESECTORS     0x03
#define DISKIO_VERIFYSECTORS    0x04
#define DISKIO_FORMATTRACK      0x05
#define DISKIO_QUERYDRVPARMS    0x08
#define DISKIO_DISKSEEK         0x0C
#define DISKIO_QUERYDASDTYPE    0x15
#define DISKIO_QUERYCHANGE      0x16
#define DISKIO_SETDISKTYPE      0x17
#define DISKIO_SETMEDIATYPE     0x18


/* Int 13h Status Equates (returned in AH)
 */

#define DISKSTATUS_SUCCESS      0x00    // successful completion
#define DISKSTATUS_BADCMD       0x01    // bad command
#define DISKSTATUS_BADADDRMARK  0x02    // address mark not found
#define DISKSTATUS_WRTPROTECT   0x03    // write on write-protected disk
#define DISKSTATUS_RECNOTFOUND  0x04    // sector not found
#define DISKSTATUS_BADRESET     0x05    // reset failed (HD)
#define DISKSTATUS_MEDIACHANGE  0x06    // media changed
#define DISKSTATUS_INITFAIL     0x07    // parm. act. failed (HD)
#define DISKSTATUS_BADDMA       0x08    // DMA overrun
#define DISKSTATUS_DMABOUNDARY  0x09    // DMA across 64K boundary
#define DISKSTATUS_BADSECTOR    0x0A    // bad sector detected (HD)
#define DISKSTATUS_BADTRACK     0x0B    // bad track detected (HD)
#define DISKSTATUS_BADMEDIATYPE 0X0C    // unsupported track (HD)
#define DISKSTATUS_BADFMTSECNUM 0X0D    // bad # of sectors on format (HD)
#define DISKSTATUS_ADDRMARKDET  0x0E    // ctrl data addr mark detected (HD
#define DISKSTATUS_DMAARBERR    0x0F    // DMA arbitration error (HD)
#define DISKSTATUS_BADCRCECC    0x10    // bad CRC/ECC
#define DISKSTATUS_DATACORRECT  0x11    // data ECC corrected
#define DISKSTATUS_BADCNTLR     0x20    // controller failure
#define DISKSTATUS_BADSEEK      0x40    // seek failed
#define DISKSTATUS_TIMEOUT      0x80    // time out
#define DISKSTATUS_DRVNOTREADY  0xAA    // drive not ready (HD)
#define DISKSTATUS_UNDEFERR     0xBB    // undefined error (HD)
#define DISKSTATUS_WRITEFAULT   0xCC    // write fault (HD)
#define DISKSTATUS_STATUSERROR  0xE0    // status register error (HD)
#define DISKSTATUS_SENSEFAIL    0xFF    // sense operation failed (HD)


/* ROM BIOS Data
 */

#define SEEK_DRV0NORECAL        0x01
#define SEEK_DRV1NORECAL        0x02
#define SEEK_DRV2NORECAL        0x04
#define SEEK_DRV3NORECAL        0x08
#define SEEK_DRVINTERRUPT       0x80

#define MOTOR_DRV0RUNNING       0x01
#define MOTOR_DRV1RUNNING       0x02
#define MOTOR_DRV2RUNNING       0x04
#define MOTOR_DRV3RUNNING       0x08
#define MOTOR_DRVSELECT         0x30
#define MOTOR_WRTOPERATION      0x80


typedef struct _DSKDATA {       /* dsk */
    BYTE    SeekStatus;         // DRIVE RECALIBRATION STATUS
                                //   BIT 3-0 = DRIVE 3-0 NEEDS RECAL
                                //   BEFORE NEXT SEEK IF BIT IS = 0
    BYTE    MotorStatus;        // MOTOR STATUS
                                //   BIT 3-0 = DRIVE 3-0 IS CURRENTLY
                                //   RUNNING
                                // BIT 7 = CURRENT OPERATION IS A WRITE,
                                //   REQUIRES DELAY
    BYTE    MotorCount;         // TIME OUT COUNTER FOR DRIVE TURN OFF
    BYTE    DisketteStatus;     // RETURN CODE STATUS BYTE
    BYTE    NECStatus[7];       // STATUS BYTES FROM NEC
} DSKDATA;


typedef struct _HDSKDTA {       /* hdsk */
    BYTE    DiskStatus1;
    BYTE    HFNum;
    BYTE    ControlByte;
    BYTE    PortOff;
} HDSKDTA;


typedef struct _XDSKDTA {       /* xdsk */
    BYTE    LastRate;           // LAST DATA RATE SELECTED
    BYTE    HFStatus;           // STATUS REGISTER
    BYTE    HFError;            // ERROR REGISTER
    BYTE    HFIntFlags;         // HARD FILE INTERRUPT FLAG
    BYTE    HFCntrl;            // COMBO HARD FILE/FLOPPY CARD BIT 0=1
    BYTE    DskState[4];        //
    BYTE    DskTrk[2];          //
} XDSKDTA;


typedef struct _DSKPARM {       /* dp */
    BYTE    specify_1;          //
    BYTE    specify_2;          //
    BYTE    motor_wait;         // wait till motor off
    BYTE    sector_size;        // bytes/sector (2 = 512)
    BYTE    eot;                // sectors per track (max)
    BYTE    rw_gap;             // read write gap
    BYTE    dtl;                //
    BYTE    format_gap;         // format gap length
    BYTE    fill;               // format fill byte
    BYTE    head_settle;        // head settle time (msec)
    BYTE    motor_start;        // motor start delay
} DSKPARM;
typedef DSKPARM *PDSKPARM;      /* pdp */


typedef struct _HDSKPARM {      /* hp */
    WORD    wMaxCylinders;      // maximum number of cylinders
    BYTE    bMaxHeads;          // maximum number of heads
    WORD    wReserve1;          // reserved (not used)
    WORD    wWritePrecompCyl;   // starting write precompensation cyl.
    BYTE    bMaxECCDataBurstLen;// maximum ECC data burst length
    BYTE    bControl;           // control byte
    BYTE    abReserve2[3];      // reserved (not used)
    WORD    wLandingZone;       // landing zone for head parking
    BYTE    bSectorsPerTrack;   // number of sectors per track
    BYTE    bReserve3;          // reserved for future use
} HDSKPARM;
typedef HDSKPARM *PHDSKPARM;    /* php */
