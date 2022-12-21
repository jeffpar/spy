/*
 *  CMOS/RTC Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define TIMER_GETTIME           0x02    // sets CH==hours,CL==min,DH==sec (BCD)
#define TIMER_SETTIME           0x03    // same as above
                                        // and DL=1 if daylight savings time option

#define TIMER_GETDATE           0x04    // sets CH==cen(19|20),CL==yr,DH==mn,DL==day
#define TIMER_SETDATE           0x05    // same as above

#define TIMER_SETALRM           0x06    // sets CH==hours,CL==min,DH==sec (BCD)
#define TIMER_RESETALRM         0x07


#define PIC_RTC                 PIC_SLAVE
#define IRQ_RTC                 0x08


#define CMOSPORT_ADDR           0x70
#define CMOSPORT_DATA           0x71

#define CMOSADDR_NMION          0x00    // ORed with value written to CMOSPORT_ADDR
#define CMOSADDR_NMIOFF         0x80    // to control NMI

#define CMOS_SEC                0x00
#define CMOS_SECALARM           0x01
#define CMOS_MIN                0x02
#define CMOS_MINALARM           0x03
#define CMOS_HR                 0x04
#define CMOS_HRALARM            0x05
#define CMOS_WEEKDAY            0x06
#define CMOS_MONTHDAY           0x07
#define CMOS_MONTH              0x08
#define CMOS_YEAR               0x09
#define CMOS_STATUSA            0x0A
#define CMOS_STATUSB            0x0B
#define CMOS_STATUSC            0x0C
#define CMOS_STATUSD            0x0D

#define CMOS_DIAGNOSTIC         0x0E
#define CMOS_SHUTDOWN           0x0F

#define CMOS_DISKTYPEAB         0x10
#define CMOS_DISKTYPECD         0x12
#define CMOS_EQUIPMENT          0x14
#define CMOS_LOWMEMLO           0x15
#define CMOS_LOWMEMHI           0x16
#define CMOS_EXTMEMLO           0x17
#define CMOS_EXTMEMHI           0x18
#define CMOS_CHKSUMLO           0x2E
#define CMOS_CHKSUMHI           0x2F

#define CMOS_LAST               0x3F


#define CMOSSHUTDOWN_FARJMP     0x0A


/* ROM BIOS Data
 */

typedef struct _RTCDATA {       /* rtc */
    WORD    UserFlag;           // offset of user wait flag     0x0498
    WORD    UserFlagSeg;        // segment of user wait flag    0x049A
    WORD    RTCLow;             // low word of user wait flag   0x049C
    WORD    RTCHigh;            // high word of user wait flag  0x049E
    BYTE    RTCWaitFlag;        // wait active flag             0x04A0
} RTCDATA;
