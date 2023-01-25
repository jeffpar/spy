/*
 *  PRINTF.C
 *  printf() and other routines
 *
 *  by Jeff Parsons 05-Jun-1992
 */


#include <all.h>
#include <com.h>


// Private function prototypes

PBYTE   _plogicalvram(PVS pvs);


/*  printf - display string on screen
 *
 *  ENTRY
 *      pszFmt -> format control string
 *
 *  EXIT
 *      # of characters displayed
 */

INT printf(PSZ pszFmt, ...)
{
    CHAR ch;
    INT c, i, n, t;
    register PCHAR pch;
    register PBYTE pbDst;

    n = (nstrlen(pszFmt) + MAXSZ*3) & ~0x03;    // BUGBUG - arbitrary
    _asm {
        sub     esp,[n]         ; allocate space off the stack
        mov     [pch],esp
    }
    n = vsprintf(pch, pszFmt, (PINT)&pszFmt + 1);

    while (*pch) {
        if (pvsMonitor->flVState & VSTATE_DISABLE) {
            if (*pch == LF)
                COMOutput(CR);
            COMOutput(*pch++);
            continue;
        }
        t = 0;
        switch(ch = *pch++) {
        case BELL:
            continue;
        case BS:
            if (colCursor)
                colCursor--;
            else if (rowCursor) {
                rowCursor--;
                colCursor = colCursorMax-1;
            }
            continue;
        case TAB:
            t = 8 - (colCursor % 8);
            ch = ' ';
            break;
        case CR:
            colCursor = 0;
            continue;
        case LF:
            colCursor = colCursorMax;
            break;
        default:
            t = 1;
            break;
        }
        do {
            c = min(t, colCursorMax - colCursor);
            if (t) {
                pbDst = _plogicalvram(pvsMonitor);
                pbDst += (rowCursor * colCursorMax + colCursor) * 2;
                colCursor += c;
                for (i=c; i>0; i--)
                    *pbDst++ = ch, *pbDst++ = bDefColor;
            }
            if (colCursor >= colCursorMax) {
                colCursor = 0;
                rowCursor++;
            }
            if (rowCursor >= rowCursorMax) {
                rowCursor = rowCursorMax - 1;
                _scroll(0, 0, colCursorMax, rowCursorMax, 1);
            }
        } while (t -= c);
    }
    _setcursor(rowCursor, colCursor, FALSE);
    return n;
}


VOID _putch(CHAR ch)
{
    printf("%c", ch);
}


/*  _scroll - scroll rectangle on screen
 *
 *  ENTRY
 *      see below;
 *      pass all zero parameters to scroll entire screen clear
 *
 *  EXIT
 *      none
 */

VOID _scroll(INT left, INT top, INT right, INT bottom, INT nLines)
{
    register PBYTE pbSrc, pbDst;
    INT i, cLines, cbLine, cbAdj;

    if (pvsMonitor->flVState & VSTATE_DISABLE)
        return;

    // Do bounds checking

    if (right == 0)
        right = colCursorMax-1;
    if (bottom == 0)
        bottom = rowCursorMax-1;

    left    = max(left, 0);
    top     = max(top, 0);
    right   = min(right, colCursorMax-1);
    bottom  = min(bottom, rowCursorMax-1);

    if (nLines == 0)
        nLines = rowCursorMax;

    if (nLines > 0) {
        cLines = bottom-top+1;
        nLines = min(nLines, cLines);

        cbLine = (right - left + 1) * 2;
        cbAdj  = colCursorMax * 2 - cbLine;

        pbDst = _plogicalvram(pvsMonitor);
        pbDst += (top * colCursorMax + left) * 2;
        pbSrc = pbDst + nLines * colCursorMax * 2;

        if (cbAdj) {
            while (cLines-- > nLines) {
                nmemcpy(pbDst, pbSrc, cbLine);
                pbDst += cbLine+cbAdj;  pbSrc += cbLine+cbAdj;
            }
        } else {
            i = nmemcpy(pbDst, pbSrc, cbLine*(cLines-nLines));
            pbDst += i;  pbSrc += i;
        }
        for (cLines=0; cLines<nLines; cLines++) {
            for (i=cbLine/2; i; i--)
                *pbDst++ = ' ', *pbDst++ = bDefColor;
            pbDst += cbAdj;
        }
    }
}


/*  _setcursor - set physical cursor position
 *
 *  ENTRY
 *      row - cursor row (where 0 is the topmost row)
 *      col - cursor column (where 0 is the leftmost column)
 *      fSync - TRUE if ROM BIOS data should be synced with cursor
 *
 *  EXIT
 *      none
 */

VOID _setcursor(INT row, INT col, INT fSync)
{
    INT offset;

    if (pvsMonitor->flVState & VSTATE_DISABLE)
        return;

    if (row >= 0 && col >= 0) {
        rowCursor = row;
        colCursor = col;
    }
    offset = rowCursor * colCursorMax + colCursor;

    // Sync the hardware

    if (pvsMonitor->cLocks >= 0) {
        pvsMonitor->aregCRTData[REG_CRTCURLOCHI] = (BYTE)(offset >> 8);
        pvsMonitor->aregCRTData[REG_CRTCURLOCLO] = (BYTE)(offset & 0xFF);
    } else {
        _outpw(pvsMonitor->wPortCRTIndx, (WORD)(REG_CRTCURLOCHI | (offset & 0xFF00)));
        _outpw(pvsMonitor->wPortCRTIndx, (WORD)(REG_CRTCURLOCLO | (offset & 0xFF) << 8));
    }

    // Sync the BIOS data area (this is a holdover of BIOS emulation support)

    if (fSync) {
        pmbZero->rb.vid.CursorPos[pmbZero->rb.vid.ActivePage].col = (BYTE)colCursor;
        pmbZero->rb.vid.CursorPos[pmbZero->rb.vid.ActivePage].row = (BYTE)rowCursor;
    }
}


PBYTE _plogicalvram(register PVS pvs)
{
    PBYTE pbDst;

    if (pvs->cLocks >= 0)
        pbDst = pvsMonitor->pbPlane[PLANE0];
    else {
        pbDst = pVRAM+(VIDMEM_MONOADDR-VIDMEM_GRPHADDR);
        if ((pvs->aregGDCData[REG_GDCMISC] & GDCMISC_ADDRMASK) == GDCMISC_32K_B8000)
            pbDst += VIDMEM_MONOSIZE;
    }
    return pbDst;
}


#ifdef MULTIPAGE

/*  _setvistop - set first visible row
 *
 *  ENTRY
 *      top - first visible row
 *
 *  EXIT
 *      none
 */

VOID _setvistop(INT row)
{
    INT offset;

    if (pvsMonitor->flVState & VSTATE_DISABLE)
        return;

    if (row != rowScreenVis) {

        rowScreenVis = row;

        offset = row * colCursorMax;

        // Sync the hardware

        if (pvsMonitor->cLocks >= 0) {
            pvsMonitor->aregCRTData[REG_CRTSTARTADDRHI] = (BYTE)(offset >> 8);
            pvsMonitor->aregCRTData[REG_CRTSTARTADDRLO] = (BYTE)(offset & 0xFF);
        } else {
            _outpw(pvsMonitor->wPortCRTIndx, (WORD)(REG_CRTSTARTADDRHI | (offset & 0xFF00)));
            _outpw(pvsMonitor->wPortCRTIndx, (WORD)(REG_CRTSTARTADDRLO | (offset & 0xFF) << 8));
        }
    }
}


/*  _resetvistop - reset first visible row
 *
 *  ENTRY
 *      none
 *
 *  EXIT
 *      none
 */

VOID _resetvistop()
{
    _setvistop(rowScreenMax);
}


/*  _decvistop - decrement first visible row
 *
 *  ENTRY
 *      none
 *
 *  EXIT
 *      none
 */

VOID _decvistop()
{
    if (rowScreenVis)
        _setvistop(rowScreenVis-1);
}


/*  _incvistop - increment first visible row
 *
 *  ENTRY
 *      none
 *
 *  EXIT
 *      none
 */

VOID _incvistop()
{
    if (rowScreenVis < rowScreenMax)
        _setvistop(rowScreenVis+1);
}

#endif
