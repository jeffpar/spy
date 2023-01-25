/*
 *  STRING.C
 *  nstrcpy() and other routines
 *
 *  by Jeff Parsons 05-Jun-1992
 */


#include <types.h>
#include <lib.h>


/*  nstrlen - return length of input string
 *
 *  ENTRY
 *      pszIn  -> input string
 *
 *  EXIT
 *      # of chars (terminating NULL is not considered)
 */

INT nstrlen(register PSZ pszIn)
{
    register INT cch = 0;

    while (*pszIn++)
        cch++;
    return cch;
}


/*  nstrskip - return number of characters to advance past 'char'
 *
 *  ENTRY
 *      pszIn  -> input string
 *
 *  EXIT
 *      # of chars (terminating NULL is not considered)
 */

INT nstrskip(register PSZ pszIn, INT ch)
{
    register INT cch = 0;

    while (*pszIn && *pszIn == ch)
        cch++, pszIn++;
    return cch;
}


/*  nstrskipto - return number of characters to advance to 'char' or NULL
 *
 *  ENTRY
 *      pszIn  -> input string
 *      ch == char to find
 *
 *  EXIT
 *      # of chars (terminating NULL is not considered)
 */

INT nstrskipto(register PSZ pszIn, INT ch)
{
    register INT cch = 0;

    while (*pszIn && *pszIn != ch)
        cch++, pszIn++;
    return cch;
}


/*  nstrskipbackto - return number of characters to back up to 'char' or NULL
 *
 *  ENTRY
 *      pszIn  -> input string
 *      ch == char to find
 *
 *  EXIT
 *      # of chars (terminating NULL is not considered)
 */

INT nstrskipbackto(register PSZ pszIn, INT ch)
{
    register INT cch = 0;

    while (*pszIn && *pszIn != ch)
        cch++, pszIn--;
    return cch;
}


/*  strcmp - compare string1 to string2
 *
 *  ENTRY
 *      psz1 -> string1
 *      psz2 -> string2
 *
 *  EXIT
 *      0 if match
 *     -1 if string1 < string2
 *      1 if string1 > string2
 */

INT strcmp(register PSZ psz1, register PSZ psz2)
{
    while (*psz1 && *psz2) {
        if (*psz1 > *psz2)
            return 1;
        else if (*psz1 < *psz2)
            return -1;
        psz1++;
        psz2++;
    }
    return !*psz2 - !*psz1;
}


/*  mystrcmpi - compare string1 to string2, case insensitively
 *
 *  ENTRY
 *      psz1 -> string1
 *      psz2 -> string2
 *
 *  EXIT
 *      0 if match
 *     -1 if string1 < string2
 *      1 if string1 > string2
 *
 *  NOTES
 *      A deliberate peculiarity of this function is that if the
 *      end of psz2 is reached and all characters have thus far matched,
 *      then a match is returned;  this relieves us from having to zero-
 *      terminate the first string (psz1).
 */

INT mystrcmpi(register PSZ psz1, register PSZ psz2)
{
    INT ch1, ch2;

    while (*psz1 && *psz2) {
        ch1 = tolower(*psz1);
        ch2 = tolower(*psz2);
        if (ch1 > ch2)
            return 1;
        else if (ch1 < ch2)
            return -1;
        psz1++;
        psz2++;
    }
    if (*psz2 == '\0')
        return 0;
    return !*psz2 - !*psz1;
}


/*  nstrcpy - copy input string to output buffer, returning count
 *
 *  ENTRY
 *      pchOut -> output buffer
 *      pszIn  -> input string (NULL ptr is ok too)
 *
 *  EXIT
 *      # of chars copied (terminating NULL is not considered)
 */

INT nstrcpy(register PCHAR pchOut, register PSZ pszIn)
{
    INT cch = 0;

    if (pszIn) {
        while (*pszIn) {
            *pchOut++ = *pszIn++;
            cch++;
        }
    }
    *pchOut = '\0';
    return cch;
}


/*  nstrcpyn - copy limited input string to output buffer, returning count
 *
 *  ENTRY
 *      pchOut -> output buffer
 *      pszIn  -> input string (NULL ptr is ok too)
 *      nMax   == maximum # of input characters to copy (-1 for unlimited)
 *
 *  EXIT
 *      # of chars copied (terminating NULL is not considered)
 */

INT nstrcpyn(register PCHAR pchOut, register PSZ pszIn, INT nMax)
{
    INT cch = 0;

    if (pszIn) {
        if (nMax == -1)
            nMax = MAXSZ;
        while (nMax-- > 0 && *pszIn) {
            *pchOut++ = *pszIn++;
            cch++;
        }
    }
    *pchOut = '\0';
    return cch;
}


/*  nstrcat - append input string to output buffer, returning count
 *
 *  ENTRY
 *      pchOut -> output buffer
 *      pszIn  -> input string
 *
 *  EXIT
 *      # of chars appended (terminating NULL is not considered)
 */

INT nstrcat(PSZ pszOut, PSZ pszIn)
{
    pszOut += nstrlen(pszOut);

    return nstrcpy(pszOut, pszIn);
}


/*  nstrcatn - append limited input string to output buffer, returning count
 *
 *  ENTRY
 *      pchOut -> output buffer
 *      pszIn  -> input string
 *      nMax   == maximum # of input characters to append
 *
 *  EXIT
 *      # of chars appended (terminating NULL is not considered)
 */

INT nstrcatn(PSZ pszOut, PSZ pszIn, INT nMax)
{
    pszOut += nstrlen(pszOut);

    return nstrcpyn(pszOut, pszIn, nMax);
}


/*  nstrupr - upper-case input string, returning count
 *
 *  ENTRY
 *      pszIn  -> input string
 *
 *  EXIT
 *      # of chars processed (terminating NULL is not considered)
 */

INT nstrupr(register PSZ pszIn)
{
    INT cch = 0;

    while (*pszIn) {
        cch++;
        *pszIn = toupper(*pszIn);
        pszIn++;
    }
    return cch;
}


/*  nmemcpy - copy input buffer to output buffer
 *
 *  ENTRY
 *      pOut -> output buffer
 *      pIn  -> input buffer
 *      nMax == # of bytes to copy
 *
 *  EXIT
 *      # of bytes copied
 */

INT nmemcpy(register PVOID pOut, register PVOID pIn, INT nMax)
{
    _asm {
        push    esi
        push    edi
        mov     eax,[nMax]      ; set return code
        mov     ecx,eax
        shr     ecx,2
        mov     esi,[pIn]
        mov     edi,[pOut]
        rep     movsd
        mov     ecx,[nMax]
        and     ecx,03h
        rep     movsb
        pop     edi
        pop     esi
    }
}


/*  nmemset - set output buffer to specified value
 *
 *  ENTRY
 *      pOut -> output buffer
 *      c    == value to set
 *      nMax == # of bytes to set
 *
 *  EXIT
 *      # of bytes set
 */

INT nmemset(register PVOID pOut, INT c, INT nMax)
{
    _asm {
        push    edi
        mov     eax,[c]
        mov     ecx,[nMax]
        mov     edi,[pOut]
        rep     stosb
        pop     edi
        mov     eax,[nMax]      ; set return code
    }
}


/*  mymemcmp - compare input buffer to output buffer
 *
 *  ENTRY
 *      pb1 -> buffer #1
 *      pb2 -> buffer #2
 *      nMax == buffer size
 *
 *  EXIT
 *      offset of difference, nMax if none
 */

INT mymemcmp(register PBYTE pb1, register PBYTE pb2, INT nMax)
{
    INT i = 0;

    while (nMax-- > 0) {
        if (*pb1++ != *pb2++)
            break;
        i++;
    }
    return i;
}


/*  nsanitize - convert all non-printable chars to '.'
 *
 *  ENTRY
 *      psz -> string
 *
 *  EXIT
 *      # of chars processed
 */

INT nsanitize(register PSZ psz)
{
    PSZ pszStart = psz;

    while (*psz) {
        if (!_isascii(*psz))
            *psz = '.';
        psz++;
    }
    return psz - pszStart;
}


/*  istrtoken - return index of matching token, or -1 if none
 *
 *  ENTRY
 *      ppszStr -> string
 *      ppszToken -> token array
 *      cpszToken == number of tokens in array
 *
 *  EXIT
 *      index of token, -1 if none;  pszStr updated as appropriate
 */
INT istrtoken(PPSZ ppszStr, PPSZ ppszToken, INT cpszToken)
{
    register PPSZ ppsz = ppszToken;

    *ppszStr += nstrskip(*ppszStr, ' ');
    while (cpszToken-- > 0) {
        if (mystrcmpi(*ppszStr, *ppsz) == 0) {
            *ppszStr += nstrlen(*ppsz);
            *ppszStr += nstrskip(*ppszStr, ' ');
            return ppsz - ppszToken;
        }
        ppsz++;
    }
    return -1;
}
