/*
 *  Timer (8254) Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define INT_TIMER               0x1A


#define TIMER_GETTICKS          0x00    // sets CX==high, DX==low, AL!=0 if rollover
#define TIMER_SETTICKS          0x01    // uses above


#define PIC_TMR                 PIC_MASTER
#define IRQ_TMR                 0x00


/* ROM BIOS Data
 */

typedef struct _TMRDATA {       /* tmr */
    WORD    TimerLow;           // low word of timer count      0x046C
    WORD    TimerHigh;          // high word of timer count     0x046E
    BYTE    TimerOvfl;          // roll-over since last read    0x0470
} TMRDATA;
