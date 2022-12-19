/*
 *  GETS.C
 *  gets() and other routines
 *
 *  by Jeff Parsons 05-Jun-1992
 */


#include <all.h>


extern  BYTE    abKeyBuff[KEYBUFF_MAX];
extern  PBYTE   pbKeyHead;      // advanced at task-time
extern  PBYTE   pbKeyTail;      // advanced at interrupt-time
extern  INT     iDebugEntry;

INT     iHistFree = 0;
INT     iHistCurr = 0;
CHAR    achHist[MAXSZ*MAXHIST] = {0};


/*  _getch - input character
 *
 *  ENTRY
 *      None
 *
 *  EXIT
 *      character
 */

CHAR _getch()
{
    CHAR ch;

    iDebugEntry++;
    do {
        flKeyEvent &= ~KEYEVENT_ABORT;
        while (pbKeyHead == pbKeyTail) {
            DrainMse();
            if (flKeyEvent & KEYEVENT_ABORT) {
                ch = ESCAPE;
                goto Leave;
            }
        }
        _asm { cli }

        ch = *pbKeyHead++;
        if (pbKeyHead == (PBYTE)&pbKeyHead)
            pbKeyHead = abKeyBuff;

        _asm { sti }

      Leave:
#ifdef MULTIPAGE
        if (ch == CTRLU) {
            _decvistop();
            ch = 0;
        }

        if (ch == CTRLD) {
            _incvistop();
            ch = 0;
        }
        if (ch != 0)
            _resetvistop();
#endif

    } while (ch == 0);

    iDebugEntry--;
    return ch;
}


/*  _kbhit - return TRUE if key exists
 *
 *  ENTRY
 *      None
 *
 *  EXIT
 *      TRUE if key exists, FALSE if not
 */

BOOL _kbhit()
{
    if (pbKeyHead != pbKeyTail)
        return _getch();
    return FALSE;
}


/*  ngets - input string
 *
 *  ENTRY
 *      pchIn -> input buffer
 *
 *  EXIT
 *      # of characters, not including terminating NULL
 */

INT ngets(PCHAR pchIn)
{
    CHAR ch;
    INT i, n = 0;
    register PSZ psz = pchIn;

    while ((ch = _getch()) != CR) {
        if (ch == CTRLA) {
            i = getcurrhistory(psz, n);
            n += i;
            psz += i;
	    break;
	}
        if (ch == CTRLJ) {
            i = getcurrhistory(psz, n);
            if (i == 0)
                i = getnexthistory(psz, n);
            n += i;
            psz += i;
            continue;
	}
        if (ch == CTRLK) {
            i = getcurrhistory(psz, n);
            if (i == 0)
                i = getprevhistory(psz, n);
            n += i;
            psz += i;
            continue;
	}
	i = (n > 0);
        if (ch == ESCAPE) {
	    i = n;
	    ch = BS;
	}
	if (ch == BS) {
            printf("%#*s", i, "\b \b");
            n -= i;
            psz -= i;
	}
	i = 1;
        if (ch == TAB) {
	    ch = ' ';
            i = 8 - (colCursor % 8);
        }
	if (ch < ' ')
	    continue;
        while (i-- && n < MAXSZ-1) {
	    n++;
            *psz++ = ch;
            printf("%c", (INT)ch);
	}
    }
    *psz = '\0';
    printf("\n");

    addhistory(pchIn);

    return n;
}


VOID addhistory(PSZ psz)
{
    INT cb;

    if (cb = nstrlen(psz)) {

        // Don't append new line if it matches current line

        if (strcmp(psz, achHist+iHistCurr) == 0)
            return;

        cb++;                   // size of incoming string plus NULL
        while (iHistFree + cb > sizeof(achHist))
            removehistory();    // remove from top until we have room at bottom
        nstrcpy(achHist+iHistFree, psz);
        iHistCurr = iHistFree;
        iHistFree += cb;
    }
}


INT getcurrhistory(PSZ psz, INT i)
{
    INT n = 0;

    if (iHistCurr != iHistFree) {
        if (i < nstrlen(achHist+iHistCurr)) {
            n = nstrcpy(psz, achHist+iHistCurr+i);
            printf(psz);
        }
    }
    return n;
}


INT getnexthistory(PSZ psz, INT i)
{
    INT n = 0;

    if (iHistCurr != iHistFree) {
        iHistCurr += nstrskipto(achHist+iHistCurr, NULL) + 1;
        printf("%#*s", i, "\b \b");
        n = getcurrhistory(psz-i, 0) - i;
    }
    return n;
}


INT getprevhistory(PSZ psz, INT i)
{
    INT n = 0;

    if (iHistCurr != 0) {
        iHistCurr -= nstrskipbackto(achHist+iHistCurr-2, NULL) + 1;
        printf("%#*s", i, "\b \b");
        n = getcurrhistory(psz-i, 0) - i;
    }
    return n;
}


VOID removehistory()
{
    INT cb;

    cb = nstrlen(achHist) + 1;  // size of first string (to be removed)
    nmemcpy(achHist, achHist+cb, iHistFree-cb);
    iHistFree -= cb;            // string removed
    if (iHistCurr)
        iHistCurr -= cb;        // current position moved, too
}
