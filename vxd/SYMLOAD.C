/*
 *  SYMLOAD.C
 *  Symbol loading support
 *
 *  by Jeff Parsons 20-Apr-1993
 */


#include <all.h>
#include <sym.h>

ASSERTMOD(symload.c);


// Globals for SymAlloc

PCHAR   pchSymTop = NULL;
PCHAR   pchSymCur = NULL;
INT     cchSymCur = MAX_SYMALLOC;


// Globals for SymOpen

PMAP    apmapModules[MAX_MODULES] = {NULL};

CHAR    szDOS386[] = "DOS386";
PSZ     apszSymAutoVxDLoad[] = {"spy.sym", "dos386.sym"};


VOID SymLoadInit()
{
    INT i;
    PDOS pdos;
    PMSDOS pmsdos;
    CHAR achFile[MAXSZ];

    i = DosGetDir(DRIVE_CURRENT, achFile);
    if (i == 0) {

        printf("Current directory is \\%s\n", achFile);

        SymOpen("io.sym", "bios_init", FALSE);
        SymOpen("msdos.sym", NULL, FALSE);

        for (i=0; i<ARRAYSIZE(apszSymAutoVxDLoad); i++)
            SymOpen(apszSymAutoVxDLoad[i], NULL, FALSE);

        // The following bits of deduction are copied from dosdump.c.
        // These fake loadobject calls are required because the objects
        // are already loaded (and if you didn't know that, you have no
        // business worrying your head about this code, do you?)

        SymLoadObject("io", "datagrp", 0, SEG_DOSDATA | SEL_V86);

        pdos = SEGPTOLINP(SEG_DOSDATA,0);

        SymLoadObject("io", "bios_code", 0, pdos->BiosCodeSeg | SEL_V86);

        SymLoadObject("msdos", "dosgrp", 0, pdos->DosDataSeg | SEL_V86);

        pmsdos = SEGPTOLINP(pdos->DosDataSeg,0);

        SymLoadObject("msdos", "doscode", 0, HIWORD(pmsdos->DosIntTable[2]) | SEL_V86);
    }
    else
        printf("Error querying current directory (%d)\n", i);
}


PSZ SymAlloc(INT cb)
{
    PCHAR pch;

    if (cb == 0)
        return NULL;            // don't need any real space

    if (cchSymCur + cb > MAX_SYMALLOC) {
        if (!(pch = MemAlloc(MAX_SYMALLOC)))
            return NULL;        // can't allocate more space
        if (pchSymCur)
            *(PDWORD)pchSymCur = (DWORD)pch;
        pchSymCur = pch;
        cchSymCur = sizeof(PDWORD);
    }

    if (!pchSymTop)             // remember the first allocation
        pchSymTop = pchSymCur;

    pch = pchSymCur + cchSymCur;
    cchSymCur += cb;

    return pch;
}


VOID SymListMaps(PCHAR pchCmd)
{
    INT i, j, cObjs;
    PDWORD pdw;
    register PMAP pmap;
    register POBJ pobj;
    CHAR szLastObj[MAXSZ];

    for (i=0; i<ARRAYSIZE(apmapModules); i++) {
        if (pmap = apmapModules[i]) {
            if (mystrcmpi(pmap->achModName, szDOS386) == 0) {
                cObjs = 0;
                szLastObj[0] = '\0';
                for (j=0,pobj=pmap->pObjs; j<pmap->cObjs; j++,pobj++,cObjs++) {
                    if (szLastObj[0] && mystrcmpi(pobj->achObjName, szLastObj)) {
                        printf("%8s  %2d objects\n", szLastObj, cObjs);
                        cObjs = 0;
                    }
                    nstrcpy(szLastObj, pobj->achObjName);
                    szLastObj[nstrskipto(szLastObj, '_')] = '\0';
                }
                printf("%8s  %2d objects\n", szLastObj, cObjs);
            }
            else
                printf("%8s  %2d objects\n",
                        pmap->achModName, pmap->cObjs);
              //printf("%8s  %2d objects, %2d constants\n",
              //        pmap->achModName, pmap->cObjs, pmap->cConsts);
        }
    }
    i = 0;
    if (pdw = (PDWORD)pchSymTop) {
        i = 1;
        while (pdw = (PDWORD)*pdw)
            i++;
    }
    printf("%d sym blocks (%d bytes/block, %d bytes in last block)\n",
            i, MAX_SYMALLOC, cchSymCur);
}


/*  SymFindAddress - Given address, find symbol
 *
 *  ENTRY
 *      pszModName -> module name (optional; if not provided, then
 *                    wObjNum must be a selector)
 *      wObjNum    == object/segment number
 *      dstObj     == distance into object/segment of symbol to find
 *      pdstSym    -> where to return distance from nearest symbol
 *  EXIT
 *      pointer to (nearest) symbol name, or NULL if none
 */

PSZ SymFindAddress(PSZ pszModName, DWORD dwObjNum, DWORD dstObj, PDWORD pdstSym)
{
    INT i, j, k;
    BOOL fDOS386;
    register PMAP pmap;
    register POBJ pobj;
    register PSYM psym;

    *pdstSym = 0;

    for (i=0; i<ARRAYSIZE(apmapModules); i++) {

        if (pmap = apmapModules[i]) {

            // If the module name for the map we found is DOS386, then
            // search all the objects associated with DOS386 that contain
            // that module name at the front of the object name.

            fDOS386 = FALSE;
            if (pszModName) {
                if (mystrcmpi(pmap->achModName, szDOS386) == 0)
                    fDOS386 = TRUE;
                else if (mystrcmpi(pmap->achModName, pszModName) != 0)
                    continue;
            }
            for (j=0,pobj=pmap->pObjs; j<pmap->cObjs; j++,pobj++) {

                if (fDOS386) {
                    if (mystrcmpi(pobj->achObjName, pszModName) != 0)
                        continue;
                }
                if (!pszModName) {
                    if (pobj->sel != dwObjNum)
                        continue;
                }
                else {
                    if (pobj->wObjNum != (WORD)dwObjNum)
                        continue;
                }

                // BUGBUG -- The sym array is sorted by offset,
                //           so I should binarily search for dstObj -JP

                for (k=0,psym=pobj->pSyms; k<pobj->cSyms; k++,psym++) {

                    if (psym->off == dstObj) {
                        *pdstSym = 0;
                        return psym->pszName;
                    }
                    else if (psym->off > dstObj && k) {
                        psym--;
                        *pdstSym = dstObj - psym->off;
                        return psym->pszName;
                    }
                }
            }
        }
    }
    return NULL;
}


INT SymFindSymbol(PSZ pszName, PDWORD psel, PDWORD poff)
{
    INT i, j, k;
    register PMAP pmap;
    register POBJ pobj;
    register PSYM psym;
    PSZ pszModule;
    CHAR szModule[MAX_SYMLENGTH];

    for (i=0; i<ARRAYSIZE(apmapModules); i++) {

        if (pmap = apmapModules[i]) {

          //printf("searching module %s\n", pmap->achModName);
            for (j=0,pobj=pmap->pObjs; j<pmap->cObjs; j++,pobj++) {

                k = SymNearestSymbol(pobj, pszName, TRUE);
                if (k >= 0 && k < pobj->cSyms) {
                    psym = pobj->ppSymsSorted[k];
                    if (mystrcmpi(pszName, psym->pszName) == 0) {
                        if (psel)
                            *psel = pobj->sel;
                        *poff = psym->off;
                        pszModule = pmap->achModName;
                        // BUGBUG -- Only bother if the sel is flat
                        if (mystrcmpi(pmap->achModName, szDOS386) == 0) {
                            pszModule = szModule;
                            nstrcpyn(szModule,
                                     pobj->achObjName,
                                     nstrskipto(pobj->achObjName, '_'));
                        }
                        *poff += VxDFindObject(pszModule, pobj->wObjNum);
                        // BUGBUG -- End of bugbug
                        return nstrlen(psym->pszName);
                    }
                }
            }
        }
    }
    return 0;
}


BOOL SymReadSymbol(INT hFile, BYTE bFlags, register PSYM psym)
{
    INT cb;
    SYMDEF16 sym16;
    SYMDEF32 sym32;
    CHAR achTemp[MAXSZ];

    psym->off = 0;
    psym->pszName = NULL;

    if (bFlags & BFLAGS_32BIT) {
        if (DosRead(hFile, sizeof(sym32), &sym32) != sizeof(sym32))
            return FALSE;
        cb = sym32.cbSymName;
        psym->off = sym32.dwValue;
    }
    else {
        if (DosRead(hFile, sizeof(sym16), &sym16) != sizeof(sym16))
            return FALSE;
        cb = sym16.cbSymName;
        psym->off = sym16.wValue;
    }

    if (cb >= sizeof(achTemp))          // BUGBUG: cb >= sizeof(achTemp)
        return FALSE;                   // may or may not be an error (ie,
                                        // could just be a really long symbol)
    achTemp[0] = '\0';
    if (cb) {
        cb = DosRead(hFile, cb, achTemp);
        if (cb < 0)
            return FALSE;               // not enough data is definitely an error
        achTemp[cb] = '\0';
    }
    if (cb >= MAX_SYMLENGTH)
        cb = MAX_SYMLENGTH-1;

    if (psym->pszName = SymAlloc(cb+1)) // +1 for the NULL
        nstrcpyn(psym->pszName, achTemp, cb);

    return TRUE;
}


VOID SymSortSymbol(POBJ pobj, register PSYM psym)
{
    INT i, j;

    j = SymNearestSymbol(pobj, psym->pszName, FALSE);
    if (j < 0)
        j = 0;
    for (i=pobj->cSyms-1; i>=j; i--) {
        pobj->ppSymsSorted[i+1] = pobj->ppSymsSorted[i];
    }
    pobj->ppSymsSorted[j] = psym;
}


INT SymNearestSymbol(POBJ pobj, PSZ pszName, BOOL fExact)
{
    INT iMin, iMax, iCur, iResult;

    iCur = 0;
    iMin = 0;
    iMax = pobj->cSyms-1;

    while (iMin <= iMax) {

        iCur = (iMin+iMax)/2;

        if (!fExact)
            iResult = mystrcmpi(pobj->ppSymsSorted[iCur]->pszName, pszName);
        else
            iResult = mystrcmpi(pszName, pobj->ppSymsSorted[iCur]->pszName) * -1;

        if (iResult == 0)
            break;

        if (iMin == iMax) {
            if (iResult < 0)
                iCur++;
            break;
        }

        if (iResult < 0)
            iMin = iCur+1;
        else
            iMax = iCur-1;
    }
    return iCur;
}


BOOL SymOpen(PSZ pszFile, PSZ pszObjIgnore, BOOL fMsg)
{
    WORD w, sel;
    INT hFile;
    LONG off;
    MAPDEF md;
    OBJDEF od;
    INT i, cb, cObjs;
    DWORD offNextObj;
    CHAR achName[MAXSZ];
    register PMAP pmap;
    register POBJ pobj;
    register PSYM psym;

    flKeyEvent &= ~KEYEVENT_ABORT;

    pszFile += nstrskip(pszFile, ' ');
    hFile = DosOpen(pszFile, ACCESS_READ | SHARE_COMP);
    if (hFile < 0) {
        nstrcpy(achName, "system\\");
        nstrcat(achName, pszFile);
        hFile = DosOpen(achName, ACCESS_READ | SHARE_COMP);
    }
    if (hFile < 0) {
        nstrcpy(achName, "\\");
        nstrcat(achName, pszFile);
        hFile = DosOpen(achName, ACCESS_READ | SHARE_COMP);
    }
    if (hFile < 0) {
        if (fMsg)
            printf("Error opening \"%s\" (%d)\n", pszFile, hFile);
        return FALSE;
    }

    cb = DosRead(hFile, sizeof(MAPDEF), &md);
    if (cb != sizeof(MAPDEF)) {
        if (fMsg)
            printf("Error reading MAPDEF (%d)", cb);
        goto CloseFile;
    }

    // Let us digress for a moment from the excitement of reading the
    // SYM file to setting up some internal data structures;  specifically,
    // allocating an entry from apmapModules, allocating a MAP structure
    // whose address we will record in that entry, and then copying the
    // relevant info from the MAPDEF we just read to aforementioned MAP
    // structure, and finally, allocating an array of OBJ structures.

    for (i=0; i<ARRAYSIZE(apmapModules); i++)   // search for slot
        if (!apmapModules[i])
            break;

    if (i == ARRAYSIZE(apmapModules)) {         // no free slot
        printf("Too many SYM files open");
        goto CloseFile;
    }

    if (!(pmap = MemAlloc(sizeof(MAP)))) {      // no free memory???
        printf("Cannot allocate MAP structure");
        goto CloseFile;
    }
    apmapModules[i] = pmap;                     // record that puppy

    pmap->cObjs = md.cObjs;
    pmap->pObjs = NULL;
    pmap->cConsts = md.cConsts;
    pmap->pConsts = NULL;
    pmap->achModName[0] = '\0';

    if (md.cbModName) {
        if (md.cbModName >= sizeof(pmap->achModName)) {
            if (fMsg)
                printf("Unexpected error reading module name");
            goto CloseFile;
        }
        DosRead(hFile, md.cbModName, pmap->achModName);
        pmap->achModName[md.cbModName] = '\0';
    }

    sel = 0;
    if (VxDFindObject(pmap->achModName, 0) == 0)
        sel = sel_Text | SEL_PROT32;

    if (pmap->cConsts) {

        if (!(pmap->pConsts = MemAlloc(sizeof(SYM)*pmap->cConsts))) {
            printf("Cannot allocate constant SYM array");
            goto CloseFile;
        }

        // I don't understand offConsts, it's not always right;
        // continuing to read past the MAPDEF seems to work better -JP
        //
        // off = DosSeek(hFile, md.offConsts, SEEK_SET);
        // if (off < 0) {
        //     if (fMsg)
        //         printf("Cannot seek to constants table");
        //     goto CloseFile;
        // }

        psym = pmap->pConsts;   // point to 1st sym in the constant SYM array

        while (md.cConsts-- && !(flKeyEvent & KEYEVENT_ABORT)) {

            if (!SymReadSymbol(hFile, md.bFlags, psym)) {
               if (fMsg)
                   printf("Unexpected error reading constant");
                goto CloseFile;
            }
            if (flKeyEvent & KEYEVENT_ABORT)
                break;
            psym++;
        }
    }

    if (!(cObjs = pmap->cObjs)) // I don't expect this to ever happen,
        goto CloseFile;         // but it is still a logical check to make

    if (!(pmap->pObjs = MemAlloc(sizeof(OBJ)*pmap->cObjs))) {
        printf("Cannot allocate OBJ array");
        goto CloseFile;
    }

    pobj = pmap->pObjs;         // point to 1st obj in the OBJ array
    offNextObj = md.paraNextObj << 4;

    while (offNextObj && cObjs-- && !(flKeyEvent & KEYEVENT_ABORT)) {

        off = DosSeek(hFile, offNextObj, SEEK_SET);
        if (off < 0) {
            if (fMsg)
                printf("\nCannot seek to next OBJDEF\n");
            goto CloseFile;
        }
        cb = DosRead(hFile, sizeof(OBJDEF), &od);
        if (cb != sizeof(OBJDEF)) {
            if (fMsg)
                printf("\nError reading OBJDEF (%d)\n", cb);
            goto CloseFile;
        }

        // Let us digress again from the excitement of reading the SYM
        // file to setting up more internal data structures;  specifically,
        // initializing the current OBJ entry (pobj), and allocating an
        // array of SYM structures.

        pobj->cSyms = 0;
        pobj->wObjNum = od.wObjNum;
        pobj->sel = sel;

        if (od.cbObjName) {
            if (od.cbObjName >= sizeof(pobj->achObjName)) {
                if (fMsg)
                    printf("\nUnexpected error reading object name\n");
                goto CloseFile;
            }
            DosRead(hFile, od.cbObjName, pobj->achObjName);
            pobj->achObjName[od.cbObjName] = '\0';

            // Check for VxD objects that have already been loaded

            if (pobj->sel & SEL_PROT32) {
                w = VxDObjNumFromName(pobj->achObjName);
                if (w) {
                  // This is a bogus assertion that simply happens to be true
                  // most of time, because most VxDs have locked AND init AND
                  // pageable code/data
                  //
                  // if (w != pobj->wObjNum)
                  //    printf("\nWarning: object number (%d) differs from %s (%d)\n",
                  //            pobj->wObjNum, pobj->achObjName, w);
                    pobj->wObjNum = w;
                }
                else
                    od.cSyms = 0;       // BUGBUG -- blow off syms for
            }                           //           unrecognized VxD objects

            // Check for objects we've been told to ignore

            if (pszObjIgnore) {
                if (mystrcmpi(pobj->achObjName, pszObjIgnore) == 0) {
                    od.cSyms = 0;       // blow off syms for this object
                }
            }
        }

        printf("Loading module %8s  object %16s  ",
                pmap->achModName, pobj->achObjName);

        if (od.cSyms) {

            if (!(pobj->pSyms = MemAlloc(sizeof(SYM)*od.cSyms))) {
                printf("\nCannot allocate SYM array\n");
                goto CloseFile;
            }

            if (!(pobj->ppSymsSorted = MemAlloc(sizeof(PSYM)*od.cSyms))) {
                printf("\nCannot allocate PSYM array\n");
                goto CloseFile;
            }

            // I don't understand offSyms, it's not always right;
            // continuing to read past the OBJDEF seems to work better -JP
            //
            // off = DosSeek(hFile, od.offSyms, SEEK_SET);
            // if (off < 0) {
            //     if (fMsg)
            //         printf("\nCannot seek to symbols table\n");
            //     goto CloseFile;
            // }

            psym = pobj->pSyms;         // point to 1st sym in the SYM array
            while (od.cSyms-- && !(flKeyEvent & KEYEVENT_ABORT)) {

                if (!SymReadSymbol(hFile, od.bFlags, psym)) {
                    if (fMsg)
                        printf("\nUnexpected error reading symbol\n");
                    goto CloseFile;
                }
                SymSortSymbol(pobj, psym);
                pobj->cSyms++;
                psym++;
            }
          //for (i=0; i<pobj->cSyms; i++)
          //    printf("  %s\n", pobj->ppSymsSorted[i]->pszName);
          //_getch();
        }
        printf("%4d symbols\r", pobj->cSyms);

        // Get ready for next OBJDEF iteration, if any
        pobj++;
        offNextObj = od.paraNextObj << 4;
    }

  CloseFile:
    printf("\n");
    DosClose(hFile);

    return TRUE;
}


/*  SymAddress - convert address to string using specified radix
 *
 *  ENTRY
 *      pchOut -> output buffer
 *      sel == dword
 *      off == dword
 *      iRadix == conversion radix (negative for zero-pad)
 *      cchMin == total # of chars to return (negative for variable)
 *      pfSymbol -> boolean (pointer may be NULL)
 *
 *      if cchMin == 9, then separation is provided between the upper
 *      and lower 4 digits of a hex conversion, in the form of a ':'
 *
 *  EXIT
 *      # of digits processed
 *      *pfSymbol will be TRUE if symbol was found, FALSE if not
 *
 *  NOTES
 *      Only base 10 (unsigned decimal) and 16 (hexadecimal) supported
 */

INT SymAddress(register PCHAR pchOut, DWORD sel, DWORD off, INT iRadix, INT cchMin, PBOOL pfSymbol)
{
    PCHAR pch = pchOut;
    PSZ pszSym;
    PSZ pszObj;
    CHAR szModName[MAX_SYMLENGTH];
    DWORD dwValue, dstObj, dstSym;

    if (pfSymbol)
        *pfSymbol = FALSE;

    if (sel & SEL_PROT32) {

        if (cchMin != 8 && !pfSymbol)
            goto CopyValue;

        if (!VxDFindAddress(off, szModName, &pszObj, &dstObj, &pszSym, &dstSym))
            goto CopyValue;

        if (!pszSym) {
            if (cchMin == 8)
                goto CopyValue;

            pch += sprintf(pch, "%s (%s)", szModName, pszObj);
            if (dstObj)
                pch += sprintf(pch, "+%x  ", dstObj);
        }
    }
    else {
        if (!SymFindAddress(NULL, sel, off, &dstSym))
            goto CopyValue;
    }

    if (pfSymbol)
        *pfSymbol = TRUE;

    pch += nstrcpy(pch, pszSym);
    if (dstSym) {
        *pch++ = '+';
        pch += dwtosz(pch, dstSym, iRadix, -1);
    }
    return pch - pchOut;

  CopyValue:
    return dwtosz(pch, off, iRadix, cchMin);
}


BOOL SymDumpAddress(PESF pesf, PCHAR pchCmd)
{
    DWORD sel, off;
    BOOL fSymbol;
    CHAR szSymbol[MAXSZ];

    sel = pesf->CS;
    if (!*pchCmd)
        off = pesf->EIP;
    else if (!ParseValue(pesf, pchCmd, &sel, &off, TRUE))
        return TRUE;

    printf("%04x:%08x  ", (DWORD)(WORD)sel, off);

    SymAddress(szSymbol, sel, off, 16, 8, &fSymbol);

    if (!fSymbol)
        printf("unknown\n");
    else
        printf("%s\n", szSymbol);

    return TRUE;
}


/*  SymLoadObject - Record location of symbolized object
 *
 *  ENTRY
 *      pszModName -> module name (required)
 *      pszObjName -> object name (required if object number unknown)
 *      wObjNum    == object number (if 0, pszObjName is used, otherwise
 *                    pszObjName is ignored)
 *      sel        == segment/selector where object is now located
 *  EXIT
 *      Nothing
 */

VOID SymLoadObject(PSZ pszModName, PSZ pszObjName, WORD wObjNum, DWORD sel)
{
    INT i, j;
    register PMAP pmap;
    register POBJ pobj;

    for (i=0; i<ARRAYSIZE(apmapModules); i++) {

        if (pmap = apmapModules[i]) {

            if (mystrcmpi(pmap->achModName, pszModName) != 0)
                continue;

            for (j=0,pobj=pmap->pObjs; j<pmap->cObjs; j++,pobj++) {

                if (pszObjName) {
                    if (mystrcmpi(pobj->achObjName, pszObjName) != 0)
                        continue;
                }
                else {
                    if (pobj->wObjNum != wObjNum)
                        continue;
                }
                pobj->sel = sel;
                printf("Object %16s in module %8s loaded at %c%04x\n",
                        pobj->achObjName,
                        pmap->achModName,
                        x86AddrType(sel, FALSE), (WORD)sel);
              //break;          // there should only be one such object, no?
            }
        }
    }
}
