/*
 *  Library include file
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define MAXSZ           79
#define MAXHIST         20      // max # of MAXSZ lines of history

#define MAXSTACK        4096

#define LC_BIT          0x20

#define _tolower(c)     ((CHAR)((c) | LC_BIT))
#define _toupper(c)     ((CHAR)((c) & ~LC_BIT))

#define _isdigit(c)     ((c) >= '0' && (c) <= '9')
#define _ishex(c)       ((c) >= '0' && (c) <= '9' || _tolower(c) >= 'a' && _tolower(c) <= 'f')
#define _islower(c)     ((c) >= 'a' && (c) <= 'z')
#define _isupper(c)     ((c) >= 'A' && (c) <= 'Z')
#define _isalpha(c)     (_tolower(c) >= 'a' && _tolower(c) <= 'z')
#define _isascii(c)     ((c) >= 0x20 && (c) < 0x7F)
#define _issymbol(c)    (_isalpha(c) || _isdigit(c) || (c) == '_' || (c) == '$')

#define toupper(c)      (_islower(c)? _toupper(c) : (c))
#define tolower(c)      (_isupper(c)? _tolower(c) : (c))

#define max(a,b)	(((a) > (b)) ? (a) : (b))
#define min(a,b)	(((a) < (b)) ? (a) : (b))

#define abs(a)          ((a) < 0 ? -(a) : (a))


/* XLATOFF */

#define SXBYTE(i)	((LONG)(SBYTE)(i))
#define SXWORD(i)	((LONG)(SHORT)(i))
#define SXSHORT(i)	((LONG)(SHORT)(i))
#define ZXBYTE(i)	((ULONG)(BYTE)(i))
#define ZXWORD(i)	((ULONG)(USHORT)(i))
#define ZXSHORT(i)	((ULONG)(USHORT)(i))

#define MAKEDWORD(l,h)  ((LONG)(((WORD)(l)) | (((DWORD)((WORD)(h))) << 16)))

#define Z2(m)		((m)&1?0:(m)&2?1:2)
#define Z4(m)		((m)&3?Z2(m):Z2((m)>>2)+2)
#define Z8(m)		((m)&15?Z4(m):Z4((m)>>4)+4)
#define Z16(m)		((m)&255?Z8(m):Z8((m)>>8)+8)
#define Z32(m)		((m)&65535?Z16(m):Z16((m)>>16)+16)
#define SHIFTLEFT(i,m)	(((i)<<Z32(m))&(m))
#define SHIFTRIGHT(i,m) (((i)&(m))>>Z32(m))

#undef  OFFSETOF
#define OFFSETOF(t,f)   ((INT)&(((t *)0)->f))
#define ARRAYSIZE(a)    ((sizeof a)/(sizeof a[0]))


#ifndef DEBUG
#define ASSERT(exp)
#define ASSERTMOD(mod)
#else
extern  CHAR szAssert[];        // defined in 386trap.asm
#define ASSERT(exp)     if (!(exp)) printf(szAssert, szModule, __LINE__);
#define ASSERTMOD(mod)  static CHAR szModule[] = __FILE__
#endif


BYTE    _inp(WORD port);
WORD    _inpw(WORD port);
VOID    _outp(WORD port, BYTE data);
VOID    _outpw(WORD port, WORD data);
#pragma intrinsic(_inp,_inpw,_outp,_outpw)

/* XLATON */


// printf.c

extern  INT rowCursor, colCursor;
extern  INT rowCursorMax, colCursorMax;

INT     printf(PSZ pszFmt, ...);
VOID    _putch(CHAR ch);
VOID    _scroll(INT left, INT top, INT right, INT bottom, INT nLines);
VOID    _setcursor(INT row, INT col, INT fSync);
#ifdef MULTIPAGE
VOID    _setvistop(INT row);
VOID    _resetvistop(VOID);
VOID    _decvistop(VOID);
VOID    _incvistop(VOID);
#endif


// sprintf.c

INT     sprintf(PCHAR pchOut, PSZ pszFmt, ...);
INT     vsprintf(PCHAR pchOut, PSZ pszFmt, PVOID pArgs);
INT     dwtosz(PCHAR pchOut, DWORD dw, INT iRadix, INT cchMin);
INT     sztodw(PSZ pszIn, PDWORD pdw, INT iRadix, INT cchMax);


// gets.c

CHAR    _getch(VOID);
BOOL    _kbhit(VOID);
INT     ngets(PCHAR pchIn);
VOID    addhistory(PSZ psz);
INT     getcurrhistory(PSZ psz, INT i);
INT     getnexthistory(PSZ psz, INT i);
INT     getprevhistory(PSZ psz, INT i);
VOID    removehistory(VOID);


// string.c

INT     nstrlen(PSZ pszIn);
INT     nstrskip(PSZ pszIn, INT ch);
INT     nstrskipto(PSZ pszIn, INT ch);
INT     nstrskipbackto(PSZ pszIn, INT ch);
INT     strcmp(PSZ psz1, PSZ psz2);
INT     mystrcmpi(PSZ psz1, PSZ psz2);
INT     nstrcpy(PCHAR pchOut, PSZ pszIn);
INT     nstrcpyn(PCHAR pchOut, PSZ pszIn, INT nMax);
INT     nstrcat(PSZ pszOut, PSZ pszIn);
INT     nstrcatn(PSZ pszOut, PSZ pszIn, INT nMax);
INT     nstrupr(PSZ pszIn);
INT     nmemcpy(PVOID pOut, PVOID pIn, INT nMax);
INT     nmemset(PVOID pOut, INT c, INT nMax);
INT     mymemcmp(PBYTE pb1, PBYTE pb2, INT nMax);
INT     nsanitize(PSZ psz);
INT     istrtoken(PPSZ ppszStr, PPSZ ppszToken, INT cpszToken);
