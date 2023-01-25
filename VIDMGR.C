/*
 *  VIDMGR.C
 *  Video management functions
 *
 *  by Jeff Parsons 21-Jul-1992
 */


#include <all.h>

ASSERTMOD(vidmgr.c);


PBYTE   pVRAM = (PBYTE)VIDMEM_GRPHADDR; // pointer to physical VRAM

VSTATE  vsVM = {-1, VSTATE_NONE, {64*K, 64*K, 64*K, 64*K}};
VSTATE  vsMonitor = {-1, VSTATE_NONE, {32*K, 0, 8*K, 0}};

// I use "row" and "col" prefixes instead of "x" and "y" to denote
// 0-relative cell coordinates that increase from top to bottom and
// left to right, as opposed to bottom to top and left to right.

INT rowCursor = 0, colCursor = 0;
INT rowCursorMax = 0, colCursorMax = 0;

#ifdef MULTIPAGE
INT rowScreenVis = 0, rowScreenMax = 0;
#endif

#ifndef NOTPRETTY
BYTE bDefColor = (CLR_BWHITE << CLR_FGND_SHIFT) | (CLR_BLUE  << CLR_BGND_SHIFT);
#else
BYTE bDefColor = (CLR_WHITE  << CLR_FGND_SHIFT) | (CLR_BLACK << CLR_BGND_SHIFT);
#endif


VOID InitVideoMgr()
{
    pVRAM = pmbZero->abGrphMem;

    InitVS(&vsVM);
    InitVS(&vsMonitor);

    SaveVS(&vsVM, TRUE);

    // BUGBUG -- Currently, the video mgr allows the monitor to inherit
    // the current video state, the assumption being we're already in text
    // mode, with the number of lines, font, etc, already set to what the
    // user wants to use with the debugger.

    // Initialize screen dimension and cursor position info

    colCursorMax = pmbZero->rb.vid.CRTCols;
#ifndef MULTIPAGE
    rowCursorMax = pmbZero->rb.vga.CRTRows+1;
    colCursor = pmbZero->rb.vid.CursorPos[pmbZero->rb.vid.ActivePage].col;
    rowCursor = pmbZero->rb.vid.CursorPos[pmbZero->rb.vid.ActivePage].row;
#else
    rowCursorMax = VIDMEM_COLRSIZE/(colCursorMax*2);
    rowScreenMax = rowCursorMax - (pmbZero->rb.vga.CRTRows+1);
    _setvistop(rowScreenMax);
    colCursor = 0;
    rowCursor = rowScreenMax;
#endif

#ifdef MULTIPAGE
    _scroll(0,0,0,0,0);         // if we changed modes, then clean the mess up
#endif
}


VOID InitVS(register PVS pvs)
{
    INT i;

    for (i=0; i<MAX_PLANES; i++) {
        pvs->cbPlane[i] = 64 * 1024;
        pvs->pbPlane[i] = MemAlloc(pvs->cbPlane[i]);
    }

    // BUGBUG: Some fake h/w state initialization;
    // more accurate initialization takes place in SaveVS()

    pvs->wPortCRTIndx = PORT_MONOCRTINDX;
    pvs->wPortCRTData = PORT_MONOCRTDATA;
    pvs->wPortStatus1 = PORT_MONOSTATUS1;
    pvs->wPortFeature = PORT_MONOFEATURE;
    pvs->aregGDCData[REG_GDCMISC] = GDCMISC_32K_B0000;

    if (!(pmbZero->rb.vga.EGAInfo & EGAINFO_EGAMONO)) {
        pvs->regMiscOut |= MISCOUT_COLRPORTS;
        pvs->wPortCRTIndx = PORT_COLRCRTINDX;
        pvs->wPortCRTData = PORT_COLRCRTDATA;
        pvs->wPortStatus1 = PORT_COLRSTATUS1;
        pvs->wPortFeature = PORT_COLRFEATURE;
        pvs->aregGDCData[REG_GDCMISC] = GDCMISC_32K_B8000;
    }
    pvs->aregSEQData[REG_SEQRESET] = SEQRESET_ASYNC | SEQRESET_SYNC;

    pvs->stateATC = ATC_INDEX;                  // Init ATC to INDEX state
}
