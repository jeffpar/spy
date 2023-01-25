/*
 *  Parser Support
 *
 *  History
 *  -------
 *  Created: 25-Apr-1993 by Jeff Parsons
 */


typedef VOID (FAR *PFNVERB)(VOID);


typedef struct _VERB {
    CHAR   *pszVerb;
    INT     cbVerb;
    PFNVERB pfnVerb;
} VERB;

typedef VERB *PVERB;


/* Stack-related stuff
 *
 * In one of these stacks, iTop is always the index of the next
 * free slot in aThings, unless it is equal to iMax, which means the
 * stack is full.  Push operations increment iTop, pop operations
 * decrement it.  iTop of zero means the stack is empty.
 */
typedef INT THING;
typedef THING *PTHING;

#define MAX_THINGS  20

typedef struct _STACK {
    INT     iTop;
    INT     iMax;
    THING   ath[MAX_THINGS];
} STACK;

typedef STACK *PSTACK;


#define EXOP_DEREF      0
#define EXOP_NOT        1
#define EXOP_LOGNOT     2
#define EXOP_ADD        3
#define EXOP_SUB        4
#define EXOP_MUL        5
#define EXOP_DIV        6
#define EXOP_OPEN       7
#define EXOP_CLOSE      8
#define EXOP_SHL        9
#define EXOP_SHR        10
#define EXOP_LT         11
#define EXOP_LE         12
#define EXOP_GT         13
#define EXOP_GE         14
#define EXOP_EQ         15
#define EXOP_NEQ        16
#define EXOP_AND        17
#define EXOP_XOR        18
#define EXOP_OR         19
#define EXOP_LOGAND     20
#define EXOP_LOGOR      21

#define EXLVL_ALL       0
#define EXLVL_OPEN      10
#define EXLVL_LOGOR     20
#define EXLVL_LOGAND    21
#define EXLVL_OR        30
#define EXLVL_XOR       31
#define EXLVL_AND       32
#define EXLVL_EQ        40
#define EXLVL_NEQ       40
#define EXLVL_LT        50
#define EXLVL_LE        50
#define EXLVL_GT        50
#define EXLVL_GE        50
#define EXLVL_SHL       60
#define EXLVL_SHR       60
#define EXLVL_ADD       70
#define EXLVL_SUB       70
#define EXLVL_MUL       80
#define EXLVL_DIV       80
#define EXLVL_LOGNOT    90
#define EXLVL_NOT       90
#define EXLVL_DEREF     90
#define EXLVL_CLOSE     99

#define EXFLG_PREFIX    0x01
#define EXFLG_INFIX     0x02
#define EXFLG_POSTFIX   0x04

typedef BOOL (FAR *PFNEXOP)(VOID);

typedef struct _EXOP {
    BYTE    bFlags;
    BYTE    bLevel;
    PFNEXOP pfnexop;
} EXOP;

typedef EXOP *PEXOP;


#define RESETOPS()      stkOps.iTop = 0
#define PUSHOP(op)      ParsePushOp(op)
#define POPOP(op)       if (!ParsePop(&stkOps, (PTHING)&(op))) return 0; else;
#define OPEXISTS()      stkOps.iTop

#define RESETVALS()     stkVals.iTop = 0
#define PUSHVAL(val)    if (!ParsePush(&stkVals, (THING)(val))) return 0; else;
#define POPVAL(val)     if (!ParsePop(&stkVals, (PTHING)&(val))) return 0; else;
#define VALEXISTS()     stkVals.iTop


// Function prototypes

VOID    ParseInit(VOID);
INT     ParseCommand(PESF pesf, PCHAR pchCmd);

INT     ParseValue(PESF pesf, PSZ psz, PDWORD psel, PDWORD pdw, BOOL fMsg);
INT     ParseReg(PSZ psz);
INT     ParseRange(PESF pesf, PSZ psz, PDWORD pdwLower, PDWORD pdwUpper);
INT     ParseAddr(PESF pesf, PSZ psz,
                  PDWORD pselCode, PDWORD poffCode, PDWORD pselData, PDWORD poffData);

BOOL    ParsePush(PSTACK pstk, THING th);
BOOL    ParsePop(PSTACK pstk, PTHING pth);

BOOL    ParsePushOp(INT iOp);
BOOL    ParseEvalOps(BYTE bLevel);
