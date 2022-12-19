/*
 *  INTPARSE.C
 *  Interpreter Parser
 *
 *  by Jeff Parsons 25-Apr-1993
 */


#include <all.h>
#include <sym.h>
#include <parse.h>

ASSERTMOD(intparse.c);


extern RD ardRegs[];


// Internal verb function prototypes

#ifdef WIN386
#include <mrci.h>
#endif

BOOL fnopDeref(VOID);
BOOL fnopNot(VOID);
BOOL fnopLogNot(VOID);
BOOL fnopAdd(VOID);
BOOL fnopSub(VOID);
BOOL fnopMul(VOID);
BOOL fnopDiv(VOID);
BOOL fnopOpen(VOID);
BOOL fnopClose(VOID);
BOOL fnopShl(VOID);
BOOL fnopShr(VOID);
BOOL fnopLT(VOID);
BOOL fnopLE(VOID);
BOOL fnopGT(VOID);
BOOL fnopGE(VOID);
BOOL fnopEq(VOID);
BOOL fnopNeq(VOID);
BOOL fnopAnd(VOID);
BOOL fnopXor(VOID);
BOOL fnopOr(VOID);
BOOL fnopLogAnd(VOID);
BOOL fnopLogOr(VOID);

VOID fnCompress(VOID);


// Internal tables

VERB avParse[] = {
    {"compress",        0,  fnCompress},
};

// Expression operations; a higher level setting indicates
// a higher priority (ie, a tighter binding of operator with operand(s))

EXOP aexop[] = {
    {EXFLG_PREFIX,  EXLVL_DEREF,    fnopDeref}, // EXOP_DEREF
    {EXFLG_PREFIX,  EXLVL_NOT,      fnopNot},   // EXOP_NOT
    {EXFLG_PREFIX,  EXLVL_LOGNOT,   fnopLogNot},// EXOP_LOGNOT
    {EXFLG_INFIX,   EXLVL_ADD,      fnopAdd},   // EXOP_ADD
    {EXFLG_INFIX,   EXLVL_SUB,      fnopSub},   // EXOP_SUB
    {EXFLG_INFIX,   EXLVL_MUL,      fnopMul},   // EXOP_MUL
    {EXFLG_INFIX,   EXLVL_DIV,      fnopDiv},   // EXOP_DIV
    {EXFLG_PREFIX,  EXLVL_OPEN,     fnopOpen},  // EXOP_OPEN
    {EXFLG_POSTFIX, EXLVL_CLOSE,    fnopClose}, // EXOP_CLOSE
    {EXFLG_INFIX,   EXLVL_SHL,      fnopShl},   // EXOP_SHL
    {EXFLG_INFIX,   EXLVL_SHR,      fnopShr},   // EXOP_SHR
    {EXFLG_INFIX,   EXLVL_LT,       fnopLT},    // EXOP_LT
    {EXFLG_INFIX,   EXLVL_LE,       fnopLE},    // EXOP_LE
    {EXFLG_INFIX,   EXLVL_GT,       fnopGT},    // EXOP_GT
    {EXFLG_INFIX,   EXLVL_GE,       fnopGE},    // EXOP_GE
    {EXFLG_INFIX,   EXLVL_EQ,       fnopEq},    // EXOP_EQ
    {EXFLG_INFIX,   EXLVL_NEQ,      fnopNeq},   // EXOP_NEQ
    {EXFLG_INFIX,   EXLVL_AND,      fnopAnd},   // EXOP_AND
    {EXFLG_INFIX,   EXLVL_XOR,      fnopXor},   // EXOP_XOR
    {EXFLG_INFIX,   EXLVL_OR,       fnopOr},    // EXOP_OR
    {EXFLG_INFIX,   EXLVL_LOGAND,   fnopLogAnd},// EXOP_LOGAND
    {EXFLG_INFIX,   EXLVL_LOGOR,    fnopLogOr}, // EXOP_LOGOR
};

STACK stkOps  = {0, MAX_THINGS};
STACK stkVals = {0, MAX_THINGS};


CHAR szError[] = "Function not supported\n";



VOID ParseInit()
{
    register PVERB pv;

    for (pv=avParse; pv<avParse+ARRAYSIZE(avParse); pv++) {
        pv->cbVerb = nstrlen(pv->pszVerb);
    }
}


INT ParseCommand(PESF pesf, register PCHAR pchCmd)
{
    register PVERB pv;

    if (!avParse[0].cbVerb)
        ParseInit();

    for (pv=avParse; pv<avParse+ARRAYSIZE(avParse); pv++) {

        if (mystrcmpi(pchCmd, pv->pszVerb) == 0) {

            (*pv->pfnVerb)();

            // Normally, we would continue processing, but for now, just return

            // pchCmd += pv->cbVerb;

            return pv->cbVerb;
        }
    }
    // It wasn't something we recognized, so return 0 chars processed
    return 0;
}


INT ParseValue(PESF pesf, register PSZ psz, PDWORD psel, PDWORD pdw, BOOL fMsg)
{
    INT i;
    DWORD dw;
    INT iLastToken;
    PSZ pszStart = psz;

    if (*psz == '?') {
        if (fMsg)
            printf("Expression symbols\n"
                   "\t()\tparentheses\n"
                   "\t||\tlogical OR\n"
                   "\t&&\tlogical AND\n"
                   "\t|\tbitwise OR\n"
                   "\t^\tbitwise XOR\n"
                   "\t&\tbitwise AND\n"
                   "\t== !=\tequal, not equal\n"
                   "\t< <=\tless than, less than or equal\n"
                   "\t> >=\tgreater than, greater than or equal\n"
                   "\t<< >>\tshift left, shift right\n"
                   "\t+ -\tadd, subtract\n"
                   "\t* /\tmultiply, divide\n"
                   "\t!\tlogical NOT\n"
                   "\t~\tbitwise NOT\n"
                   "\t@\tdereference (assumes pointer to dword)\n"
            );
        return 0;
    }

    // iLastToken encodes the current state as follows:
    //
    //  0 - no state, expecting symbol or prefix op
    //  1 - symbol or postfix op encountered, expecting infix or postfix op
    //  2 - infix or prefix op encountered, expecting symbol or prefix op

    RESETOPS();                 // reset the ops (operations) stack
    RESETVALS();                // reset the vals (values) stack
    iLastToken = 0;

    while (*psz) {

        // Begin the value-parsing process by expecting a value;
        // currently, numerics must begin with a digit (0-9) and be hex

        psz += nstrskip(psz, ' ');

        if (i = ParseReg(psz)) {
            if (iLastToken == 1)
                break;          // sorry, we were expecting an op
            iLastToken = 1;
            PUSHVAL(x86RegValue(pesf, --i));
            psz += nstrlen(ardRegs[i].pszReg);
        }
#ifdef WIN386
        else if (_issymbol(*psz) && !_isdigit(*psz) && (i = SymFindSymbol(psz, psel, &dw))) {
            if (iLastToken == 1)
                break;          // sorry, we were expecting an op
            iLastToken = 1;
            PUSHVAL(dw);
            psz += min(i,nstrlen(psz));
        }
#endif
        else if (i = sztodw(psz, &dw, 16, -1)) {
            if (iLastToken == 1)
                break;          // sorry, we were expecting an op
            iLastToken = 1;
            PUSHVAL(dw);
            psz += i;
        }

        // Next, check for an op (after skipping whitespace)

        psz += nstrskip(psz, ' ');

        switch(*psz) {
        case '@':
            i = EXOP_DEREF;
            break;

        case '~':
            i = EXOP_NOT;
            break;

        case '+':
            i = EXOP_ADD;
            break;

        case '-':
            i = EXOP_SUB;
            break;

        case '*':
            i = EXOP_MUL;
            break;

        case '/':
            i = EXOP_DIV;
            break;

        case '(':
            i = EXOP_OPEN;
            break;

        case ')':
            i = EXOP_CLOSE;
            break;

        case '<':
            if (*(psz+1) == '<') {
                psz++;
                i = EXOP_SHL;
            }
            else if (*(psz+1) == '=') {
                psz++;
                i = EXOP_LE;
            }
            else
                i = EXOP_LT;
            break;

        case '>':
            if (*(psz+1) == '>') {
                psz++;
                i = EXOP_SHR;
            }
            else if (*(psz+1) == '=') {
                psz++;
                i = EXOP_GE;
            }
            else
                i = EXOP_GT;
            break;

        case '=':
            if (*(psz+1) == '=') {
                psz++;
                i = EXOP_EQ;
            }
            break;

        case '!':
            if (*(psz+1) == '=') {
                psz++;
                i = EXOP_NEQ;
            }
            else
                i = EXOP_LOGNOT;
            break;

        case '&':
            if (*(psz+1) == '&') {
                psz++;
                i = EXOP_LOGAND;
            }
            else
                i = EXOP_AND;
            break;

        case '^':
            i = EXOP_XOR;
            break;

        case '|':
            if (*(psz+1) == '|') {
                psz++;
                i = EXOP_LOGOR;
            }
            else
                i = EXOP_OR;
            break;

        default:
            if (iLastToken == 2) {
                if (fMsg)
                    printf("Unknown symbol \"%s\"\n", psz);
                return 0;           // unknown value/symbol
            }
        case NULL:
            goto Process;
        }

        if ((aexop[i].bFlags & EXFLG_PREFIX) && iLastToken == 1)
            goto Process;
        if ((aexop[i].bFlags & (EXFLG_POSTFIX | EXFLG_INFIX)) && iLastToken != 1)
            goto Process;

        PUSHOP(i);

        if (aexop[i].bFlags & EXFLG_POSTFIX)
            iLastToken = 1;
        if (aexop[i].bFlags & (EXFLG_PREFIX | EXFLG_INFIX))
            iLastToken = 2;

        psz++;
    }

    Process:

    if (!ParseEvalOps(EXLVL_ALL)) {
        if (fMsg)
            printf("Error in expression \"%s\"\n", pszStart);
        return 0;
    }

    if (!VALEXISTS()) {
        if (fMsg)
            printf("Missing value\n");
        return 0;               // hmmm, nothing on the value stack
    }

    POPVAL(*pdw);               // pop the (hopefully) last value

    if (VALEXISTS()) {
        if (fMsg)
            printf("Missing operator\n");
        return 0;               // hmmm, there's still things on the stack
    }

    return psz - pszStart;      // simple calculation to return chars processed
}


INT ParseReg(PSZ psz)
{
    INT i;

    for (i=0; ardRegs[i].pszReg; i++) {
        if (!mystrcmpi(psz, ardRegs[i].pszReg))
            if (!_issymbol(*(psz+nstrlen(ardRegs[i].pszReg))))
                return i+1;
    }
    return 0;
}


INT ParseRange(PESF pesf, register PSZ psz, PDWORD pdwLower, PDWORD pdwUpper)
{
    INT cch;

    if (cch = ParseValue(pesf, psz, NULL, pdwLower, FALSE)) {
        psz += cch;
        *pdwUpper = *pdwLower;
        if (*psz++ == '.' && *psz++ == '.') {
            cch += 2 + ParseValue(pesf, psz, NULL, pdwUpper, FALSE);
        }
    }
    return cch;
}


INT ParseAddr(PESF pesf, register PSZ psz,
              PDWORD pselCode, PDWORD poffCode, PDWORD pselData, PDWORD poffData)
{
    INT i, cch = 0;
    DWORD flSelType;

    flSelType = SEL_PROT;
    if (pesf->Flags & FLAGS_V86)
        flSelType = SEL_V86;

    if (*psz == '&') {
        psz++;
        cch++;
        flSelType = SEL_V86;
    }
    else if (*psz == '#') {
        psz++;
        cch++;
        flSelType = SEL_PROT;
    }
    else if (*psz == '%') {
        psz++;
        cch++;
        flSelType = SEL_PROT32;
    }

    if (i = ParseValue(pesf, psz, pselCode, poffCode, FALSE)) {
        cch += i;
        psz += i;
        if (*psz++ == ':') {
            cch++;
            *pselCode = (WORD)*poffCode | flSelType;
            cch += ParseValue(pesf, psz, pselCode, poffCode, FALSE);
        }
        else if (flSelType == SEL_PROT32) {

            // We get here when there's no explicit selector BUT
            // there was a % override, which means we should switch to
            // the flat selector

            *pselCode = sel_Flat | flSelType;
        }
        if (poffData)
            *poffData = *poffCode;
        if (pselData)
            *pselData = *pselCode;
    }
    return cch;
}


BOOL ParsePush(register PSTACK pstk, THING th)
{
    if (pstk->iTop == pstk->iMax)
        return FALSE;           // stack is full
    pstk->ath[pstk->iTop++] = th;
}


BOOL ParsePop(register PSTACK pstk, PTHING pth)
{
    if (pstk->iTop == 0)
        return FALSE;           // stack is empty
    *pth = pstk->ath[--pstk->iTop];
}


BOOL ParsePushOp(INT iOp)
{
    if (stkOps.iTop == stkOps.iMax)
        return FALSE;           // stack is full

    ASSERT(iOp < ARRAYSIZE(aexop));
    if (!(aexop[iOp].bFlags & EXFLG_PREFIX))
        if (!ParseEvalOps(aexop[iOp].bLevel))
            return FALSE;

    stkOps.ath[stkOps.iTop++] = iOp;
}


BOOL ParseEvalOps(BYTE bLevel)
{
    INT iOp;

    while (OPEXISTS()) {
        iOp = stkOps.ath[stkOps.iTop-1];
        ASSERT(iOp < ARRAYSIZE(aexop));
        if (aexop[iOp].bLevel >= bLevel) {
            POPOP(iOp);
            ASSERT(iOp < ARRAYSIZE(aexop));
            if (!(*aexop[iOp].pfnexop)())
                return FALSE;   // propagate any errors
        }
        else
            break;
    }
    return TRUE;
}


BOOL fnopDeref()
{
    DWORD dw1;

    POPVAL(dw1);
    PUSHVAL(*(PDWORD)dw1);
    return TRUE;
}


BOOL fnopNot()
{
    DWORD dw1;

    POPVAL(dw1);
    PUSHVAL(~dw1);
    return TRUE;
}


BOOL fnopLogNot()
{
    DWORD dw1;

    POPVAL(dw1);
    PUSHVAL(!dw1);
    return TRUE;
}


BOOL fnopAdd()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2+dw1);
    return TRUE;
}


BOOL fnopSub()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2-dw1);
    return TRUE;
}


BOOL fnopMul()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2*dw1);
    return TRUE;
}


BOOL fnopDiv()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    if (dw1 == 0) {
        printf("Attempted division by zero\n");
        return FALSE;
    }
    POPVAL(dw2);
    PUSHVAL(dw2/dw1);
    return TRUE;
}


BOOL fnopOpen()
{
    return TRUE;
}


BOOL fnopClose()
{
    return ParseEvalOps(EXLVL_OPEN);
}


BOOL fnopShl()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 << dw1);
    return TRUE;
}


BOOL fnopShr()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 >> dw1);
    return TRUE;
}


BOOL fnopLT()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 < dw1);
    return TRUE;
}


BOOL fnopLE()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 <= dw1);
    return TRUE;
}


BOOL fnopGT()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 > dw1);
    return TRUE;
}


BOOL fnopGE()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 >= dw1);
    return TRUE;
}


BOOL fnopEq()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 == dw1);
    return TRUE;
}


BOOL fnopNeq()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 != dw1);
    return TRUE;
}


BOOL fnopAnd()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 & dw1);
    return TRUE;
}


BOOL fnopXor()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 ^ dw1);
    return TRUE;
}


BOOL fnopOr()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 | dw1);
    return TRUE;
}


BOOL fnopLogAnd()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 && dw1);
    return TRUE;
}


BOOL fnopLogOr()
{
    DWORD dw1, dw2;

    POPVAL(dw1);
    POPVAL(dw2);
    PUSHVAL(dw2 || dw1);
    return TRUE;
}


VOID fnCompress()
{
#ifdef WIN386
    INT hFile, iResult;
    LONG cbSrcTotal, cbDstTotal;
    INT cbSrc, cbDst, cbCopy;
    CHAR achFile[MAXSZ];
    static CHAR achSrc[8192], achDst[8192/8*9+5], achCopy[8192];

    while (TRUE) {

        printf("Filename: ");
        ngets(achFile);
        if (!achFile[0])
            return;

        hFile = DosOpen(achFile, ACCESS_READ | SHARE_COMP);
        if (hFile < 0) {
            printf("Error opening \"%s\" (%d)\n", achFile, hFile);
            continue;
        }

        cbSrcTotal = cbDstTotal = 0;
        while (TRUE) {
            cbSrc = DosRead(hFile, sizeof(achSrc), achSrc);
            if (cbSrc < 0) {
                printf("Error reading \"%s\" (%d)\n", achFile, cbSrc);
                break;
            }
            if (cbSrc == 0)
                break;

            ASSERT(cbSrc <= sizeof(achSrc));
            cbSrcTotal += cbSrc;

            cbDst = MRCI32_Compress(achSrc, cbSrc, achDst, sizeof(achDst));
            if (!cbDst) {
                _asm mov [iResult],edx
                printf("Compression error on %d bytes (%d)\n", cbSrc, iResult);
                cbDstTotal += cbSrc;
            } else {

                ASSERT(cbDst < cbSrc);
                cbDstTotal += cbDst;

                printf("Compressed %d bytes to %d bytes\n", cbSrc, cbDst);
                cbCopy = MRCI32_Decompress(achDst, achCopy, cbSrc);
                if (!cbCopy) {
                    _asm mov [iResult],edx
                    printf("Decompression error (%d)\n", iResult);
                } else {
                    if (cbCopy != cbSrc || (iResult = mymemcmp(achSrc, achCopy, cbSrc)) != cbSrc)
                        printf("Decompression mismatch @%d\n", iResult);
                }
            }
        }
        if (cbSrc >= 0)
            printf("Compressed %d total bytes to %d, %d%% of original size\n",
                    cbSrcTotal, cbDstTotal, (cbDstTotal*100)/cbSrcTotal);

        iResult = DosClose(hFile);
        if (iResult != 0)
            printf("Error closing \"%s\" (%d)\n", achFile, iResult);
    }
#else
    printf(szError);
#endif
}
