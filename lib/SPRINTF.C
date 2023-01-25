/*
 *  SPRINTF.C
 *  sprintf() and other routines
 *
 *  by Jeff Parsons 05-Jun-1992
 */


#include <types.h>
#include <lib.h>


/*  sprintf - convert integer to string using specified radix
 *
 *  ENTRY
 *      pchOut -> output buffer
 *      pszFmt -> format control string
 *
 *  EXIT
 *      # of digits
 *
 *  NOTES
 *      Only base 10 (unsigned decimal) and 16 (hexadecimal) supported
 */

INT sprintf(PCHAR pchOut, PSZ pszFmt, ...)
{
    return vsprintf(pchOut, pszFmt, (PINT)&pszFmt + 1);
}


/*  vsprintf - convert integer to string using specified radix
 *
 *  ENTRY
 *      pchOut -> output buffer
 *      pszFmt -> format control string
 *      pArgs  -> arguments, if any
 *
 *  EXIT
 *      # of digits
 *
 *  NOTES
 *      Only base 10 (unsigned decimal) and 16 (hexadecimal) supported
 */

INT vsprintf(register PCHAR pchOut, register PSZ pszFmt, PVOID pArgs)
{
    INT n;
    LONG lMin;
    PSZ psz;
    CHAR ch;
    PCHAR pchSave = pchOut;
    BOOL fBlankPad, fRepeat;

    while (*pszFmt) {
        if (*pszFmt == '%') {

            pszFmt++;
            lMin = -1;
            fBlankPad = 1;
            fRepeat = FALSE;

            if (*pszFmt == '#') {
                pszFmt++;
                fRepeat = TRUE;
            }

            if (*pszFmt == '0')
                fBlankPad = -1;

            if (_isdigit(*pszFmt))
                pszFmt += sztodw(pszFmt, (PDWORD)&lMin, 10, -1);

            if (*pszFmt == '*') {
                pszFmt++;
                lMin = *((PINT)pArgs)++;
            }

            switch(ch = *pszFmt++) {
            case 'c':
                if (lMin--)
                    *pchOut++ = *(PSZ)pArgs;
                while (lMin-- > 0)  // BUGBUG -- yes, I want chars left-justified
                    if (!fRepeat)
                        *pchOut++ = ' ';
                    else
                        *pchOut++ = *(PSZ)pArgs;
                ((PINT)pArgs)++;
                break;
            case 'd':
                pchOut += dwtosz(pchOut, *((PDWORD)pArgs)++, 10*fBlankPad, (INT)lMin);
                break;
            case 's':
                if (!fRepeat) {
                    n = nstrcpyn(pchOut, *(PPSZ)pArgs, (INT)lMin);
                    pchOut += n;
                    if (n < (INT)lMin) {
                        n = (INT)lMin - n;
                        while (n--) // BUGBUG -- yes, I want chars left-justified
                            *pchOut++ = ' ';
                    }
                }
                else {
                    lMin = abs(lMin);
                    while (lMin--)
                        pchOut += nstrcpy(pchOut, *(PPSZ)pArgs);
                }
                ((PPSZ)pArgs)++;
                break;
            case 'x':
            case 'X':
                psz = pchOut;
                pchOut += dwtosz(pchOut, *((PDWORD)pArgs)++, 16*fBlankPad, (INT)lMin);
                if (ch == 'X')
                    nstrupr(psz);
                break;
            default:
                *pchOut++ = ch;
                break;
            }
        }
        else
            *pchOut++ = *pszFmt++;
    }
    *pchOut = '\0';
    return pchOut - pchSave;
}


/*  dwtosz - convert dword to string using specified radix
 *
 *  ENTRY
 *      pchOut -> output buffer
 *      dw == dword
 *      iRadix == conversion radix (negative for zero-pad)
 *      cchMin == total # of chars to return (negative for variable)
 *
 *      if cchMin == 9, then separation is provided between the upper
 *      and lower 4 digits of a hex conversion, in the form of a ':'
 *
 *  EXIT
 *      # of digits processed
 *
 *  NOTES
 *      Only base 10 (unsigned decimal) and 16 (hexadecimal) supported
 */

INT dwtosz(register PCHAR pchOut, DWORD dw, INT iRadix, INT cchMin)
{
    DWORD d, q;
    CHAR fSep = 0;
    INT ch, chFill, cSkip, fSig, n = 0;

    fSig = FALSE;
    chFill = ' ';
    if (iRadix < 0) {
        chFill = '0';
        iRadix = -iRadix;
    }
    if (cchMin == 9 && iRadix == 16) {
        cchMin--;
        fSep = ':';
    }
    d = 0x10000000;
    cSkip = 8 - cchMin;
    if (iRadix == 10) {
        d = 1000000000;
        cSkip = 10 - cchMin;
        if ((LONG)dw < 0) {
            dw = (DWORD)(-(LONG)dw);
            *pchOut++ = '-';
            cchMin--;
            n++;
        }
    }
    while (cchMin != 0 && d != 0) {
        ch = 0;
        q = (dw / d);
        dw -= q * d;
        d /= iRadix;
        if (fSig || q != 0 || d == 0) {
            fSig = TRUE;
            ch = q + '0';
            if (q >= 10)
                ch = q - 10 + 'a';
        }
        else if (cSkip > 0)
            cSkip--;
        else
            ch = chFill;
        if (ch) {
            *pchOut++ = (CHAR)ch;
            cchMin--;
            if (fSep && cchMin == 4) {
                *pchOut++ = fSep;
                n++;
            }
            n++;
        }
    }
    *pchOut = '\0';
    return n;
}


/*  sztodw - convert string to dword using specified radix
 *
 *  ENTRY
 *      psz -> string
 *      pdw -> dword
 *      iRadix == conversion radix
 *      cchMax == maximum # of digits to convert (-1 for no limit)
 *
 *  EXIT
 *      # of digits processed
 *
 *  NOTES
 *      Only base 10 (unsigned decimal) and base 16 (hexadecimal) supported
 */

INT sztodw(register PSZ pszIn, PDWORD pdw, INT iRadix, INT cchMax)
{
    DWORD d, dw;
    INT ch, n = 0;

    dw = 0;
    while (*pszIn && cchMax--) {
        ch = _tolower(*pszIn);
        if (_isdigit(ch))
            d = ch - '0';
        else if (ch >= 'a' && ch <= 'f' && iRadix == 16)
            d = ch - 'a' + 10;
        else
            break;
        n++;
        pszIn++;
        dw = (dw * iRadix) + d;
    }
    *pdw = dw;
    return n;
}
