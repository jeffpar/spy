/*
 *  Types and constants
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


/* ASM
OPTION  SCOPED
 */

#ifndef NOBASEDEFS
#define NULL            0
#define FALSE           0
#define TRUE            (!FALSE)
#endif
#define K               1024

/* XLATOFF */
#define FAR             // here for portability, might not use it

#undef  CONST
#define CONST           // empty define because I don't want to deal with
                        // compiler complaints right now, but I do want to
                        // flag R/O data as such
/* XLATON */

#define MAX_WORD        0xFFFF
#define MAX_DWORD       0xFFFFFFFF


#ifndef NOBASEDEFS
typedef void            VOID;
typedef char            CHAR;
typedef unsigned char   BYTE;
typedef unsigned char   UCHAR;
typedef long            INT;
typedef long            BOOL;
typedef unsigned long   UINT;
typedef short		SHORT;
typedef unsigned short  WORD;
typedef unsigned short  USHORT;
typedef long		LONG;
typedef unsigned long   DWORD;
typedef unsigned long   ULONG;
#endif
typedef signed char     SBYTE;
typedef unsigned short  PORT;
typedef long            FLAGS;


#ifndef NOBASEDEFS
typedef CHAR             SZ[];
typedef VOID        FAR *PVOID;
typedef CHAR        FAR *PCHAR;
typedef CHAR        FAR *PSZ;
typedef BYTE        FAR *PBYTE;
typedef INT         FAR *PINT;
typedef BOOL        FAR *PBOOL;
typedef UINT        FAR *PUINT;
typedef SHORT       FAR *PSHORT;
typedef WORD        FAR *PWORD;
typedef USHORT      FAR *PUSHORT;
typedef LONG        FAR *PLONG;
typedef DWORD       FAR *PDWORD;
typedef ULONG       FAR *PULONG;
#endif
typedef CHAR        FAR *PSTR;
typedef PCHAR       FAR *PPCHAR;
typedef PBYTE       FAR *PPBYTE;
typedef PWORD       FAR *PPWORD;
typedef PUSHORT     FAR *PPUSHORT;
typedef PDWORD      FAR *PPDWORD;
typedef PULONG      FAR *PPULONG;
typedef PSZ         FAR *PPSZ;
typedef VOID (FAR *PFNVOID)(VOID);


typedef struct _BYTES {         /* b */
    BYTE    lo;
    BYTE    hi;
} BYTES;


typedef struct _WORDS {         /* w */
    WORD    lo;
    WORD    hi;
} WORDS;


typedef struct _RECT {          /* rc */
    INT     left;
    INT     top;
    INT     right;
    INT     bottom;
} RECT;
