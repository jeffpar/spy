/*
 *  Video Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define INT_VIDEO               0x10
#define INT_VIDEOPARMS          0x1D
#define INT_VIDEOGRAPH          0x1F


#define VIDEO_RESET             0x00    // reset mode
#define VIDEO_SETCURSORTYPE     0x01    // set cursor type
#define VIDEO_SETCURSORPOS      0x02
#define VIDEO_QUERYCURSORPOS    0x03
#define VIDEO_QUERYLPEN         0x04
#define VIDEO_SETACTIVEPAGE     0x05
#define VIDEO_SCROLLUP          0x06
#define VIDEO_SCROLLDOWN        0x07
#define VIDEO_READCHARATTR      0x08
#define VIDEO_WRITECHARATTR     0x09
#define VIDEO_WRITECHAR         0x0A
#define VIDEO_SETPALETTE        0x0B
#define VIDEO_WRITEPIXEL        0x0C
#define VIDEO_READPIXEL         0x0D
#define VIDEO_WRITETTY          0x0E
#define VIDEO_QUERYMODE         0x0F
#define VIDEO_EGASETPALETTE     0x10
#define VIDEO_EGASELECTFONT     0x11
#define VIDEO_EGASELECTMISC     0x12
#define VIDEO_EGAWRITESTRING    0x13
#define VIDEO_VGADISPLAYCODES   0x1A
#define VIDEO_VGAQUERYSTATE     0x1B
#define VIDEO_VGASAVERESTORE    0x1C


#define VMODE_BW40              0x00
#define VMODE_CO40              0x01
#define VMODE_BW80              0x02
#define VMODE_CO80              0x03
#define VMODE_CO320X200X4       0x04
#define VMODE_BW320X200X4       0x05
#define VMODE_CO640X200X2       0x06
#define VMODE_MONO80            0x07
#define VMODE_PCJR1             0x08
#define VMODE_PCJR2             0x09
#define VMODE_PCJR3             0x0A
#define VMODE_COLRFONT          0x0B
#define VMODE_MONOFONT          0x0C
#define VMODE_CO320X200X16      0x0D
#define VMODE_CO640X200X16      0x0E
#define VMODE_MONO640X350X4     0x0F
#define VMODE_CO640X350X16      0x10
#define VMODE_CO640X480X2       0x11
#define VMODE_CO640X480X16      0x12
#define VMODE_CO320X200X256     0x13

// VMODE flags for SetMode() only

#define VMODE_50LINES           0x10000


#define VIDMEM_GRPHADDR         0xA0000
#define VIDMEM_GRPHSIZE         0x10000

#define VIDMEM_MONOADDR         0xB0000
#define VIDMEM_MONOSIZE         0x08000

#define VIDMEM_COLRADDR 	0xB8000
#define VIDMEM_COLRSIZE 	0x08000

#define PLANE0                  0
#define PLANE1                  1
#define PLANE2                  2
#define PLANE3                  3
#define MAX_PLANES              4
#define MAX_PLANESIZE           0x10000

#define MAX_RTRCLOOP            0x4000  // maximum # iterations to wait for retrace

// Colors (traditional attribute settings)

#define CLR_BLACK               0x00
#define CLR_BLUE                0x01
#define CLR_GREEN               0x02
#define CLR_CYAN                0x03
#define CLR_RED                 0x04
#define CLR_MAGENTA             0x05
#define CLR_BROWN               0x06
#define CLR_WHITE               0x07
#define CLR_FGND_SHIFT          0x00

#define CLR_GRAY                0x08
#define CLR_BBLUE               0x09
#define CLR_BGREEN              0x0A
#define CLR_BCYAN               0x0B
#define CLR_BRED                0x0C
#define CLR_BMAGENTA            0x0D
#define CLR_YELLOW              0x0E
#define CLR_BWHITE              0x0F
#define CLR_BGND_SHIFT          0x04


/*  Hardware ports
 *
 *  All are single-register ports unless otherwise specified;
 *  All single-register ports are W/O unless VGA or otherwise specified
 */
#define PORT_MONOCRTINDX        0x3B4   //
#define PORT_MONOCRTDATA        0x3B5   // multi-register port
#define PORT_MONOMODE           0x3B8   // usually shadowed at CRTModeSet
#define PORT_MONOLPENSET        0x3B9   //
#define PORT_MONOSTATUS1        0x3BA   // (R/O)
#define PORT_MONOFEATURE        0x3BA   // (W/O)
#define PORT_MONOLPENRESET      0x3BB   //
#define PORT_ATC0               0x3C0   //
#define PORT_ATC1               0x3C1   // on VGA, distinct from ATC0 and R/O
#define PORT_STATUS0            0x3C2   // (R/O)
#define PORT_MISCOUT            0x3C2   // reset sequencer before writing
#define PORT_VGAENABLE          0x3C3   //
#define PORT_SEQINDX            0x3C4   //
#define PORT_SEQDATA            0x3C5   // multi-register port
#define PORT_VGADACMASK         0x3C6   // initialized to 0FFh, leave alone
#define PORT_VGADACREAD         0x3C7   // state (R), index to read (W)
#define PORT_VGADACWRITE        0x3C8   // Index to write (R/W)
#define PORT_VGADACDATA         0x3C9   // multi-register port
#define PORT_GDCPOS2            0x3CA   // on VGA, R/O (returns FEATURE)
#define PORT_GDCPOS1            0x3CC   // on VGA, R/O (returns MISCOUT)
#define PORT_GDCINDX            0x3CE   //
#define PORT_GDCDATA            0x3CF   // multi-register port
#define PORT_COLRCRTINDX        0x3D4   //
#define PORT_COLRCRTDATA        0x3D5   // multi-register port
#define PORT_CGAMODE            0x3D8   // usually shadowed at CRTModeSet
#define PORT_CGACOLOR           0x3D9   // usually shadowed at CRTPalette
#define PORT_COLRSTATUS1        0x3DA   // (R/O)
#define PORT_COLRFEATURE        0x3DA   // (W/O)
#define PORT_CGALPENRESET       0x3DB   //
#define PORT_CGALPENSET         0x3DC   //


/*  Registers indexed via multi-register ports
 *
 *  All are W/O unless VGA or otherwise specified
 */
#define REG_ATCPALETTE          0x00    // 00h-0Fh are palette registers
#define REG_ATCMODECTRL         0x10    // Compaq VGA: wait for vert retrace
#define REG_ATCOVERSCAN         0x11    //
#define REG_ATCPLANEENABLE      0x12    //
#define REG_ATCPELPANNING       0x13    //
#define REG_ATCVGAREG           0x14    //
#define TOTAL_EGA_ATCREGS       0x14    //
#define TOTAL_IBM_ATCREGS       0x15    //
#define TOTAL_TOSHIBA_ATCREGS   0x15    //

#define REG_SEQRESET            0x00    //
#define REG_SEQCLKMODE          0x01    // reset sequencer before writing
#define REG_SEQMAPMASK          0x02    //
#define REG_SEQCHARMAPSEL       0x03    //
#define REG_SEQMEMMODE          0x04    // reset sequencer before writing
#define TOTAL_IBM_SEQREGS       0x05    //
#define TOTAL_TOSHIBA_SEQREGS   0x05    //

#define REG_GDCSR               0x00    //
#define REG_GDCESR              0x01    //
#define REG_GDCCLRCMP           0x02    //
#define REG_GDCROTATE           0x03    //
#define REG_GDCREADMAP          0x04    //
#define REG_GDCMODE             0x05    //
#define REG_GDCMISC             0x06    // reset sequencer before writing
#define REG_GDCDONTCARE         0x07    //
#define REG_GDCBITMASK          0x08    //
#define TOTAL_IBM_GDCREGS       0x09    //
#define TOTAL_TOSHIBA_GDCREGS   0x10    //

#define REG_CRTHORZTOTAL        0x00    //
#define REG_CRTHORZDSPEND       0x01    //
#define REG_CRTHORZSTBLNK       0x02    //
#define REG_CRTHORZENDBLNK      0x03    //
#define REG_CRTHORZSTRTRC       0x04    //
#define REG_CRTHORZENDRTRC      0x05    //
#define REG_CRTVERTTOTAL        0x06    //
#define REG_CRTOVERFLOW         0x07    //
#define REG_CRTPRESETROW        0x08    //
#define REG_CRTMAXSCAN          0x09    //
#define REG_CRTCURSTSCAN        0x0A    //
#define REG_CRTCURENDSCAN       0x0B    //
#define REG_CRTSTARTADDRHI      0x0C    // R/W (W/O on CGA???)
#define REG_CRTSTARTADDRLO      0x0D    // R/W (W/O on CGA???)
#define REG_CRTCURLOCHI         0x0E    // R/W
#define REG_CRTCURLOCLO         0x0F    // R/W
#define REG_CRTVERTSTRTRC       0x10    //
#define REG_CRTLPENHI           0x10    // R/O (dual-purpose register index)
#define REG_CRTVERTENDRTRC      0x11    //
#define REG_CRTLPENLO           0x11    // R/O (dual-purpose register index)
#define REG_CRTVERTDSPEND       0x12    //
#define REG_TSENG_CRTLPEN2      0x12    // R/O (dual-purpose register index)
#define REG_CRTOFFSET           0x13    //
#define REG_CRTUNDLINELOC       0x14    //
#define REG_CRTVERTSTBLNK       0x15    //
#define REG_CRTVERTENDBLNK      0x16    //
#define REG_CRTMODECTRL         0x17    //
#define REG_CRTLINECMP          0x18    //
#define TOTAL_CGA_CRTREGS       0x13    //
#define TOTAL_IBM_CRTREGS       0x19    //
#define TOTAL_TOSHIBA_CRTREGS   0x35    //

#define TOTAL_DACREGS           256     // on VGA, there are 256 color registers
#define TOTAL_IBM_DACREGS       256
#define TOTAL_TOSHIBA_DACREGS   256

#define MAX_ATCREGS             TOTAL_TOSHIBA_ATCREGS
#define MAX_SEQREGS             TOTAL_TOSHIBA_SEQREGS
#define MAX_GDCREGS             TOTAL_TOSHIBA_GDCREGS
#define MAX_CRTREGS             TOTAL_TOSHIBA_CRTREGS
#define MAX_DACREGS             TOTAL_TOSHIBA_DACREGS


/* Individual bits for registers
 */

#define ATCPAL_ADDRMASK         0x01F
#define ATCPAL_ENABLE           0x020

#define SEQRESET_ASYNC          0x001
#define SEQRESET_SYNC           0x002

#define SEQCLK_8DOTS            0x001   // reset sequencer before writing
#define SEQCLK_CYCLES           0x002
#define SEQCLK_SHIFTLOAD        0x004
#define SEQCLK_DOTCLOCK         0x008   // reset sequencer before writing
#define SEQCLK_VGASHIFT4        0x010
#define SEQCLK_VGASCREENOFF     0x020

#define SEQMAP_PLANE0           0x001
#define SEQMAP_PLANE1           0x002
#define SEQMAP_PLANE2           0x004
#define SEQMAP_PLANE3           0x008
#define SEQMAP_ALL              (SEQMAP_PLANE0|SEQMAP_PLANE1| \
                                 SEQMAP_PLANE2|SEQMAP_PLANE3)

#define SEQMEM_ALPHA            0x001
#define SEQMEM_EXTENDED         0x002
#define SEQMEM_ODDEVENSAME      0x004   // otherwise, even->0/2, odd->1/3
#define SEQMEM_CHAIN4           0x008   // low 2 bits select plane
#define SEQMEM_ALL              (SEQMEM_ALPHA|SEQMEM_EXTENDED|SEQMEM_ODDEVENSAME|SEQMEM_CHAIN4)

#define GDCSR_PLANE0            0x001
#define GDCSR_PLANE1            0x002
#define GDCSR_PLANE2            0x004
#define GDCSR_PLANE3            0x008
#define GDCSR_ALL               (GDCSR_PLANE0|GDCSR_PLANE1| \
                                 GDCSR_PLANE2|GDCSR_PLANE3)
#define GDCESR_ALL              GDCSR_ALL

#define GDCCLRCMP_ALL           0x00F

#define GDCROTATE_COUNT         0x007
#define GDCROTATE_AND           0x008
#define GDCROTATE_OR            0x010
#define GDCROTATE_XOR           0x018
#define GDCROTATE_NOFUNC        0x000
#define GDCROTATE_ALL           (GDCROTATE_COUNT|GDCROTATE_AND|GDCROTATE_OR| \
                                 GDCROTATE_XOR)

#define GDCREADMAP_ALL          (PLANE0|PLANE1|PLANE2|PLANE3)

#define GDCMODE_WRITE0          0x000
#define GDCMODE_WRITE1          0x001
#define GDCMODE_WRITE2          0x002
#define GDCMODE_WRITE3          0x003   // only on a VGA
#define GDCMODE_TEST            0x004
#define GDCMODE_READ0           0x000
#define GDCMODE_READ1           0x008
#define GDCMODE_ODDEVENDIFF     0x010
#define GDCMODE_ODDEVENBITS     0x020
#define GDCMODE_256COLOR        0x040   // only on a VGA
#define GDCMODE_ALL             (GDCMODE_WRITE3|GDCMODE_TEST|GDCMODE_READ1| \
                                 GDCMODE_ODDEVENDIFF|GDCMODE_ODDEVENBITS|GDCMODE_256COLOR)

#define GDCMISC_NOCHARGEN       0x001
#define GDCMISC_ODDTOEVEN       0x002
#define GDCMISC_64K_A0000       0x004
#define GDCMISC_32K_B0000       0x008
#define GDCMISC_32K_B8000       0x00C
#define GDCMISC_128K_A0000      0x000
#define GDCMISC_ADDRMASK        0x00C
#define GDCMISC_ALL             (GDCMISC_NOCHARGEN|GDCMISC_ODDTOEVEN|GDCMISC_32K_B8000)

#define GDCDONTCARE_ALL         0x00F

#define GDCBITMASK_ALL          0x0FF

#define CRTOVFL_VERTTOTAL       0x001
#define CRTOVFL_VERTDSPEND      0x002
#define CRTOVFL_VERTSTRTRC      0x004
#define CRTOVFL_VERTSTBLNK      0x008
#define CRTOVFL_LINECMP         0x010
#define CRTOVFL_CURLOC          0x020

#define CRTMAXSCAN_200CONV      0x080   // VGA bit for 200->400 conversion

#define CRTVERTEND_WRITEPROT    0x080

#define CRTMCTRL_COMPAT         0x001   // CRT compatibility mode (8K disp.)


#define MISCOUT_COLRPORTS       0x001   // select 3Dx (or possibly 2Dx) ports
#define MISCOUT_ENABLERAM       0x002
#define MISCOUT_CLKSELECT       0x00C   // reset sequencer before writing
#define MISCOUT_DISABLEDRV      0x010
#define MISCOUT_HIGHPAGE        0x020
#define MISCOUT_NEGHORZRTRC     0x040
#define MISCOUT_NEGVERTRTRC     0x080

#define STATUS1_HORZRTRC        0x001   // very frequent, but very short
#define STATUS1_LPENSTROBE      0x002
#define STATUS1_LPENSWITCH      0x004
#define STATUS1_VERTRTRC        0x008   // less frequent, but much longer
#define STATUS1_ATCOUTPUT1      0x010
#define STATUS1_ATCOUTPUT2      0x020


#define MONOMODE_HIRES          0x001   // must always be set
#define MONOMODE_ENABLE         0x008
#define MONOMODE_BLINK          0x020

#define CGAMODE_80X25           0x001
#define CGAMODE_320X200         0x002
#define CGAMODE_BW              0x004
#define CGAMODE_ENABLE          0x008
#define CGAMODE_640X200         0x010
#define CGAMODE_BLINK           0x020

#define CGACOLOR_BORDER         0x00F   // border in modes 0/1/2/3, bgnd in 4/5
#define CGACOLOR_INTENSE        0x010
#define CGACOLOR_SET            0x020



/* VIDEO_EGASELECTFONT subfunctions
 */

#define VFONT_8X8               0x12    // load 8x8 font (after mode change)
#define VFONT_GETPTR            0x30    //


/* ROM BIOS Data
 */

typedef struct _CURPOS {        /* cpos */
    BYTE    col;                // column
    BYTE    row;                // row
} CURPOS;


typedef struct _VIDDATA {       /* vid */
    BYTE    CRTMode;            // current crt mode             0x0449
    WORD    CRTCols;            // number of columns on screen  0x044A
    WORD    CRTLen;             // length of regen in bytes     0x044C
    WORD    CRTStart;           // page offset in frame buffer  0x044E
    CURPOS  CursorPos[8];       // cursor for each of 8 pages   0x0450
    WORD    CursorMode;         // current cursor mode setting  0x0460
    BYTE    ActivePage;         // current page displayed       0x0462
    WORD    Addr6845;           // base address of active card  0x0463
    BYTE    CRTModeSet;         // setting of the 3x8 register  0x0465
    BYTE    CRTPalette;         // palette setting (color card) 0x0466
} VIDDATA;


#define EGAINFO_NOEMULATE       0x01    // (controls cursor emulation)
#define EGAINFO_EGAMONO         0x02    //
#define EGAINFO_WAITRETRC       0x04    //
#define EGAINFO_EGAINACTIVE     0x08    //
#define EGAINFO_MEMMASK         0x60    // (00=64k, 01=128k, 10=192k, 11=256k)
#define EGAINFO_DONTCLEAR       0x80    // (don't clear screen on resets)

#define EGASW_SW1OFF            0x01
#define EGASW_SW2OFF            0x02
#define EGASW_SW3OFF            0x04
#define EGASW_SW4OFF            0x08
#define EGASW_FEATUREBITS       0xF0

#define VGAFLAGS_VGAACTIVE      0x01
#define VGAFLAGS_GRAYSCALES     0x02
#define VGAFLAGS_VGAMONO        0x04
#define VGAFLAGS_NODEFPALETTE   0x08
#define VGAFLAGS_DSPSWITCHING   0x40

#define VGAFLAGS_350LINES       0x00
#define VGAFLAGS_400LINES       0x10
#define VGAFLAGS_200LINES       0x80
#define VGAFLAGS_RESRVDLINES    0x90


typedef struct _VGADATA {       /* vga */
    BYTE    CRTRows;            // current crt rows-1           0x0484
    WORD    CRTPoints;          // font point size              0x0485
    BYTE    EGAInfo;            // flags                        0x0487
    BYTE    EGASwitches;        // switches                     0x0488
    BYTE    VGAFlags;           // VGA flags                    0x0489
    BYTE    DCCIndex;           // display combination code     0x048A
} VGADATA;


/*  ATC (Attribute Controller) flip-flop states
 *
 *  These are the states that the ATC on an EGA/VGA can be in.  They have
 *  been selected not only because they are complementary, but also because
 *  they can be OR'ed with a index into the virtual ATC data (aregATCData),
 *  to produce a properly adjusted index (I rely on the fact that regATCIndx
 *  precedes aregATCData by 1 byte).
 */

#define ATC_DATA        0       // ready for data byte
#define ATC_INDEX       -1      // ready for index byte


/*  DAC (Digital-to-Analog Color-lookup) states
 *
 *  These are the states that the DAC hardware on a VGA can be in.
 */

#define DAC_DATA0       0x0000  // ready first 1st byte of data
#define DAC_DATA1       0x0001  // ready first 2nd byte of data
#define DAC_DATA2       0x0002  // ready first 3rd byte of data
#define DAC_DATAMASK    0x0003
#define DAC_READ        0x0300  // read mode (high byte simulates state reg.)
#define DAC_WRITE       0x0000  // write mode (high byte simulates state reg.)
#define DAC_RWMASK      0x0300


/*  DAC color register structure
 *
 *  This structure contains the 3x6 bits (total of 18 bits) that make up
 *  the DAC information for one color register.
 */

typedef struct _DACREG {        /* dac */
    BYTE    bRed;
    BYTE    bGreen;
    BYTE    bBlue;
} DACREG;
typedef DACREG *PDACREG;


/*  Video state structure
 */

#define VSTATE_NONE             0x0000
#define VSTATE_SWAP             0x0001
#define VSTATE_DISABLE          0x0002

typedef struct _VSTATE {        /* vs */
    INT     cLocks;                     // -1 => no locks
    INT     flVState;                   // VSTATE flags, if any

    INT     cbPlane[MAX_PLANES];        // size of plane buffers
    PBYTE   pbPlane[MAX_PLANES];        // pointers to plane buffers

    WORD    wPortCRTIndx;		// PORT_MONOCRTINDX or PORT_COLRCRTINDX
    WORD    wPortCRTData;		// PORT_MONOCRTDATA or PORT_COLRCRTDATA
    WORD    wPortStatus1;		// PORT_MONOSTATUS1 or PORT_COLRSTATUS1
    WORD    wPortFeature;		// PORT_MONOFEATURE or PORT_COLRFEATURE

    BYTE    regCRTIndx;                 // shadows PORT_MONOCRTINDX/COLRCRTINDX
    BYTE   aregCRTData[MAX_CRTREGS];    // shadows PORT_MONOCTRDATA/COLRCRTDATA[]
    INT     stateStatus1;               // state of STATUS1 register
                                        // (also see abNextStatusState)
    BYTE    regFeature;                 // shadows PORT_MONOFEATURE/COLRFEATURE
    BYTE    regMiscOut;                 // shadows PORT_MISCOUT
    BYTE    regATCIndx;                 // shadows PORT_ATC0
    BYTE   aregATCData[MAX_ATCREGS];    // shadows PORT_ATC1[MAX_ATCREGS]
    BYTE    regSEQIndx;                 // shadows PORT_SEQINDX
    BYTE   aregSEQData[MAX_SEQREGS];    // shadows PORT_SEQDATA[MAX_SEQREGS]
    BYTE    regGDCPos1;                 // shadows PORT_GDCPOS1
    BYTE    regGDCPos2;                 // shadows PORT_GDCPOS2
    BYTE    regGDCIndx;                 // shadows PORT_GDCINDX
    BYTE   aregGDCData[MAX_GDCREGS];    // shadows PORT_GDCDATA[MAX_GDCREGS]
    INT     stateATC;                   // state of ATC controller

    BYTE    regDACMask;                 // shadows PORT_VGADACMASK
    BYTE    regDACIndx;                 // shadows PORT_VGADACREAD/VGADACWRITE
    DACREG adacDACData[MAX_DACREGS];    // shadows PORT_VGADACDATA[MAX_DACREGS]
    DACREG  dacDACDataCur;              // current DAC data register
    INT     stateDAC;                   // state of VGA's DAC

    BYTE    bLatches[MAX_PLANES];       // storage for latches
} VSTATE;
typedef VSTATE *PVS;
