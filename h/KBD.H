/*
 *  Keyboard (8042) Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define CTRLA           0x01
#define CTRLB           0x02
#define CTRLC           0x03
#define CTRLD           0x04
#define CTRLE           0x05
#define CTRLF           0x06
#define CTRLG           0x07
#define CTRLH           0x08
#define CTRLI           0x09
#define CTRLJ           0x0A
#define CTRLK           0x0B
#define CTRLL           0x0C
#define CTRLM           0x0D
#define CTRLN           0x0E
#define CTRLO           0x0F
#define CTRLP           0x10
#define CTRLQ           0x11
#define CTRLR           0x12
#define CTRLS           0x13
#define CTRLT           0x14
#define CTRLU           0x15
#define CTRLV           0x16
#define CTRLW           0x17
#define CTRLX           0x18
#define CTRLY           0x19
#define CTRLZ           0x1A
#define BELL            CTRLG
#define BS              CTRLH
#define TAB             CTRLI
#define LF              CTRLJ
#define CR              CTRLM
#define ESCAPE          0x1B


#define INT_KEY                 0x16


#define KEY_READ                0x00
#define KEY_PEEK                0x01
#define KEY_QUERYSHIFT          0x02
#define KEY_SETDELAYS           0x03
#define KEY_WRITECHAR           0x05
#define KEY_READEXTCHAR         0x10
#define KEY_PEEKEXTCHAR         0x11
#define KEY_QUERYEXTSHIFT       0x12
#define KEY_READ122CHAR         0x20
#define KEY_PEEK122CHAR         0x21
#define KEY_QUERY122SHIFT       0x22


#define PIC_KBD                 PIC_MASTER
#define IRQ_KBD                 0x01


#define KBDPORT_DATA            0x60
#define KBDPORT_CMD             0x64    // write-only
#define KBDPORT_STATUS          0x64    // read-only

#define KBDSTATUS_OUTPUT        0x01    // output buffer full
#define KBDSTATUS_INPUT         0x02    // input buffer full
#define KBDSTATUS_AUXDATA       0x20    // aux (eg, PS/2 mouse) data available

#define KBDCMD_READ_CB          0x20    // read command byte (CB)
#define KBDCMD_WRITE_CB         0x60    // write command byte (CB)
#define KBDCMD_DISABLE_KBD      0xAD    // disable keyboard
#define KBDCMD_ENABLE_KBD       0xAE    // enable keyboard
#define KBDCMD_READ_OPORT       0xD0    // read output port (OP)
#define KBDCMD_WRITE_OPORT      0xD1    // write output port (OP)
#define KBDCMD_MSE              0xD4    // PS/2-style mouse command
#define KBDCMD_CPURESET         0xFE

#define KBDMSE_ENABLE           0xF4    // enable PS/2-style mouse
#define KBDMSE_DISABLE          0xF5    // disable PS/2-style mouse

#define KBDCB_SYSFLAG           0x04    // system flag in controller command byte (CB)

#define KBDOPORT_A20ON          0xDF    // A20 flag in controller output port (OP)


#define KEYCMD_SETSCANSET       0xF0
#define KEYCMD_SETTYPEMATIC     0xF3
#define TYPEMATIC_FAST          0x00
#define TYPEMATIC_DEFAULT       0x2B


// Flags for flKeyEvent status

#define KEYEVENT_DEBUGGER       0x01    // debug entry pending
#define KEYEVENT_DEBUGGER_BIT   0
#define KEYEVENT_ABORT          0x02    // abort sequence detected


#define KEYBUFF_MAX             128     // maximum length of kbd buffer


#ifndef  KBDSCAN3

/* Scan codes for scan set #1
 */

#define KBDSCAN_1               0x02
#define KBDSCAN_BACKSPACE       0x0E
#define KBDSCAN_ENTER           0x1C
#define KBDSCAN_LCTRL           0x1D
#define KBDSCAN_A               0x1E
#define KBDSCAN_LSHIFT          0x2A
#define KBDSCAN_RSHIFT          0x36
#define KBDSCAN_F1              0x3B
#define KBDSCAN_F2              0x3C
#define KBDSCAN_F3              0x3D
#define KBDSCAN_F4              0x3E
#define KBDSCAN_F5              0x3F
#define KBDSCAN_F6              0x40
#define KBDSCAN_F7              0x41
#define KBDSCAN_F8              0x42
#define KBDSCAN_F9              0x43
#define KBDSCAN_F10             0x44
#define KBDSCAN_SCRLOCK         0x46
#define	KBDSCAN_SYSREQ			0x54
#define KBDSCAN_F11             0x57
#define KBDSCAN_F12             0x58
#define KBDSCAN_BREAKPREFIX     0x80
#define KBDSCAN_PAUSE           0xE1

#else   // KBDSCAN3

/* Scan codes for scan set #3
 */

#define KBDSCAN_1               0x16
#define KBDSCAN_A               0x1C
#define KBDSCAN_ENTER           0x5A
#define KBDSCAN_F12             0x5E
#define KBDSCAN_SCRLOCK         0x5F
#define KBDSCAN_PAUSE           0x62
#define KBDSCAN_BACKSPACE       0x66
#define KBDSCAN_BREAKPREFIX     0xF0

#endif  // KBDSCAN3


/* ROM BIOS Data
 */

#define KBFLAG_RSHIFT           0x01
#define KBFLAG_LSHIFT           0x02
#define KBFLAG_SHIFTMASK        0x03
#define KBFLAG_LCTRL            0x04
#define KBFLAG_LALT             0x08
#define KBFLAG_SCROLLLOCK       0x10
#define KBFLAG_NUMLOCK          0x20
#define KBFLAG_CAPSLOCK         0x40
#define KBFLAG_INSERT           0x80

#define KBFLAG1_RCTRL           0x01        // right CTRL down
#define KBFLAG1_RALT            0x02        // right ALT down
#define KBFLAG1_SYSSHIFT        0x04        // SysReq pressed and held
#define KBFLAG1_HOLDSTATE       0x08        // Ctrl-NumLock has been pressed
#define KBFLAG1_SCROLLDOWN      0x10
#define KBFLAG1_NUMDOWN         0x20
#define KBFLAG1_CAPSDOWN        0x40
#define KBFLAG1_INSDOWN         0x80


typedef struct _KBDDATA {       /* kbd */
    BYTE    KBFlag;
    BYTE    KBFlag1;            // SECOND BYTE OF KEYBOARD STATUS
    BYTE    AltInput;           // STORAGE FOR ALTERNATE KEYPAD ENTRY
    WORD    BufferHead;         // POINTER TO HEAD OF KEYBOARD BUFFER
    WORD    BufferTail;         // POINTER TO TAIL OF KEYBOARD BUFFER
    WORD    KBBuffer[16];       // ROOM FOR 15 ENTRIES
} KBDDATA;
