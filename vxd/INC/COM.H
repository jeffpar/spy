/*
 *  COM Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define COM1_PORT       0x03F8
#define COM2_PORT       0x02F8

#define COM_DAT         0x00
#define COM_IEN         0x01            // interrupt enable
#define COM_IER         0x02            // interrupt ID
#define COM_LCR         0x03            // line control registers
#define COM_MCR         0x04            // modem control register
#define COM_LSR         0x05            // line status register
#define COM_MSR         0x06            // modem status regiser
#define COM_DLL         0x00            // divisor latch least sig
#define COM_DLM         0x01            // divisor latch most sig

#define CLOCK_RATE      0x0001C200      // clock rate
#define LC_DLAB         0x80            // divisor latch access bit

#define BD_150          150
#define BD_300          300
#define BD_600          600
#define BD_1200         1200
#define BD_2400         2400
#define BD_4800         4800
#define BD_9600         9600
#define BD_19200        19200


#define PIC_COM1        PIC_MASTER
#define IRQ_COM1        0x04

#define PIC_COM2        PIC_MASTER
#define IRQ_COM2        0x03


// Function prototypes

BOOL    COMInit(INT iPort);
CHAR    COMInput(VOID);
VOID    COMOutput(CHAR chOut);
