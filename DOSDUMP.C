/*
 *  DOSDUMP.C
 *  DOS Dump Support
 *
 *  by Jeff Parsons 20-Apr-1993
 */


#include <all.h>

ASSERTMOD(dosdump.c);


// Function prototypes

VOID    DOSDumpArena(VOID);


// Internal data structures

PSZ     apszDOSDump[] = {"arena"};
PFNVOID apfnDOSDump[] = {DOSDumpArena};


BOOL DOSDumpCommand(PESF pesf, PCHAR pchCmd)
{
    INT i;

    if (*pchCmd == '?') {
        printf("DOS dump commands\n"
               "\tdos arena\tdump memory arena\n"
               "\tdos dpb\t\tdump drive parameter blocks\n"
        );
        return TRUE;
    }
    if (*pchCmd) {
        pchCmd += nstrskip(pchCmd, ' ');
        i = istrtoken(&pchCmd, apszDOSDump, ARRAYSIZE(apszDOSDump));
        if (i >= 0) {
            (*apfnDOSDump[i])();
            return TRUE;
        }
    }
    return FALSE;
}


VOID DOSDumpArena()
{
    PDOS pdos;
    PMSDOS pmsdos;
    PMCB pmcbPrev, pmcbNext;
    PSZ pszDesc;
    CHAR szTmp[9];
    register PMCB pmcb, pmcbSystem;

    printf("DOS I/O segment  @%04X\n", SEG_DOSDATA, 0);

    pdos = SEGPTOLINP(SEG_DOSDATA,0);

    printf("I/O code segment @%04X:%04X (from %04X:%04X)\n",
            pdos->BiosCodeSeg, pdos->cdev_off, SEGLINP(pdos), OFFSETOF(DOS,cdev_off));

    printf("DOS data segment @%04X      (from %04X:%04X)\n",
            pdos->DosDataSeg, SEGLINP(pdos), OFFSETOF(DOS,DosDataSeg));

    pmsdos = SEGPTOLINP(pdos->DosDataSeg,0);

    printf("DOS code segment @%09X (from %04X:%04X)\n",
            pmsdos->DosIntTable[2], SEGLINP(pmsdos), OFFSETOF(MSDOS,DosIntTable));

    printf("DOS arena header @%04X      (from %04X:%04X)\n",
            pmsdos->arena_head, SEGLINP(pmsdos), OFFSETOF(MSDOS, arena_head));

    pmcb = SEGPTOLINP(pmsdos->arena_head,0);

    flKeyEvent &= ~KEYEVENT_ABORT;
    while (pmcb->signature == ARENA_SIG || pmcb->signature == ARENA_SIG_END) {

        // Get next MCB, too
        pmcbNext = pmcb + pmcb->size + 1;

        nstrcpyn(szTmp, pmcb->name, sizeof(szTmp)-1);
        nsanitize(szTmp);

        printf("MCB @%04X: Sig=%c Owner=%04X Size=%04X Name=\"%8s\"\n",
                SEGLINP(pmcb), pmcb->signature, pmcb->owner, pmcb->size, szTmp);

        // If this is a "system data" arena, then it should have a sub-arena

        if (pmcb->owner == 0x0008 && pmcb->name[0] == 'S' && pmcb->name[1] == 'D') {
            pmcbSystem = pmcb+1;
            while (_isalpha(pmcbSystem->signature)) {

                pszDesc = "Unknown";
                switch(_tolower(pmcbSystem->signature)) {
                case 'b':
                case 'c':
                    pszDesc = "Buffers";
                    break;
                case 'd':
                    pszDesc = "Device";
                    break;
                case 'f':
                    pszDesc = "Files";
                    break;
                case 'g':
                    pszDesc = "BDSs";
                    break;
                case 'l':
                    pszDesc = "Lastdrive";
                    break;
                case 's':
                    pszDesc = "Stacks";
                    break;
                case 't':
                    pszDesc = "Install";
                    break;
                case 'x':
                    pszDesc = "FCBs";
                    break;
                case 'y':
                    pszDesc = "Drivemap";
                    break;
                }

                nstrcpyn(szTmp, pmcbSystem->name, sizeof(szTmp)-1);
                nsanitize(szTmp);

                printf("SYSMCB @%04X Sig=%c Seg=%04X Size=%04X Name=\"%8s\" (%s)\n",
                        SEGLINP(pmcbSystem), pmcbSystem->signature,
                        pmcbSystem->owner, pmcbSystem->size, szTmp, pszDesc);

                pmcbSystem += pmcbSystem->size + 1;

                if (pmcbSystem >= pmcbNext) {
                    if (pmcbSystem != pmcbNext)
                        printf("Possible error in SYSMCBs (expected end @%04X, actual end @%04X)\n",
                                SEGLINP(pmcbNext), SEGLINP(pmcbSystem));
                    break;
                }
            }
        }

        // Check for exit condition
        if (pmcb->signature == ARENA_SIG_END)
            return;

        // Advance to next MCB
        pmcbPrev = pmcb;
        pmcb = pmcbNext;

        if (flKeyEvent & KEYEVENT_ABORT)
            return;
    }
    printf("Error in DOS arena @%09X\n", LINPTOADDR(pmcb));
}
