/*
 *  VMMDUMP.C
 *  VMM Dump Support
 *
 *  by Jeff Parsons 20-Apr-1993
 */


#include <all.h>
#include <parse.h>
#include <win386.h>

ASSERTMOD(vmmdump.c);


// Globals

DWORD   hVMSwitch = 0;

LOG     logIO = {"I/O", 0, 0, 0, MAX_LOG_ENTRIES};
DWORD   hVMLog = 0;             // 0 means all VMs
PBYTE   piopmLog = NULL;        // pointer to dummy IOPM for tracking logging changes (if any)
CHAR    szNoLog[] = "Log buffer not allocated\n";

PSZ apszLogOp[] = {
    "???",                      // 0
    "INB",                      // LOGOP_INB
    "INW",                      // LOGOP_INW
    "OUTB",                     // LOGOP_OUTB
    "OUTW",                     // LOGOP_OUTW
};


// Internal data structures

PSZ     apszVMMDump[]   = {"io", "l", "r", "sw"};
PFNDUMP apfnVMMDump[]   = {VMMDumpIO, VMMDumpVMs, VMMDumpVMRegs, VMMWatchSwitch};

PSZ     apszVMMDumpIO[] = {"on", "off", "list", "log", "nolog", "vm", "clear", "rev"};


BOOL VMMDumpCommand(PESF pesf, PCHAR pchCmd)
{
    INT i;

    if (*pchCmd == '?') {
        printf("VM dump commands\n"
               "\tvmio?\t\tI/O logging commands\n"
               "\tvml\t\tvirtual machine info\n"
               "\tvmr\t\tcurrent virtual machine regs\n"
               "\tvmsw\t\tset vm task-switch watchpoint\n"
        );
        return TRUE;
    }
    if (*pchCmd) {
        pchCmd += nstrskip(pchCmd, ' ');
        i = istrtoken(&pchCmd, apszVMMDump, ARRAYSIZE(apszVMMDump));
        if (i >= 0)
            return (*apfnVMMDump[i])(pesf, pchCmd);
    }
    return FALSE;
}


BOOL VMMDumpIO(PESF pesf, PCHAR pchCmd)
{
    register PBYTE pb;

    if (*pchCmd == '?') {
        printf("VM I/O commands\n"
               "\tvmio on\t\tallocate log buffer\n"
               "\tvmio off\tfree log buffer\n"
               "\tvmio\t\tdisplay all log info\n"
               "\tvmio rev\tdisplay info in reverse order\n"
               "\tvmio clear\treset log info\n"
               "\tvmio list\tlist logged ports\n"
               "\tvmio log\tlog port or range\n"
               "\tvmio nolog\tdisable logging for port or range\n"
               "\tvmio vm\t\trestrict logging to specified vm\n"
        );
        return TRUE;
    }

    switch(istrtoken(&pchCmd, apszVMMDumpIO, ARRAYSIZE(apszVMMDumpIO))) {
    case 0:             // "on"
        if (logIO.ple) {
            printf("Log buffer already allocated\n");
            break;
        }
        logIO.ple = MemAlloc(logIO.iMaximum*sizeof(LOGENTRY));
        if (!logIO.ple)
            printf("Cannot allocate log buffer\n");
        else {
            logIO.iNext = logIO.iLast = logIO.cEntries = 0;
            if (piopmLog = MemAlloc(SIZE_IOPM))
                nmemset(piopmLog, 0xFF, SIZE_IOPM); // log all by default
        }
        break;
    case 1:             // "off"
        if (!logIO.ple) {
            printf(szNoLog);
            break;
        }
        if (!MemFree(logIO.ple, logIO.iMaximum*sizeof(LOGENTRY)))
            printf("Cannot free log buffer\n");
        else {
            logIO.ple = NULL;
        }
        if (piopmLog)
            MemFree(piopmLog, SIZE_IOPM);
        break;
    case 2:             // "ports"
        if (piopmLog)
            return x86PortCommand(pesf, "l", pchCmd, piopmLog);
        printf(szNoLog);
        break;
    case 3:             // "log"
        if (piopmLog)
            return x86PortCommand(pesf, "t", pchCmd, piopmLog);
        printf(szNoLog);
        break;
    case 4:             // "nolog"
        if (piopmLog)
            return x86PortCommand(pesf, "c", pchCmd, piopmLog);
        printf(szNoLog);
        break;
    case 5:             // "vm"
        if (*pchCmd) {
            if (ParseValue(pesf, pchCmd, NULL, &hVMLog, TRUE)) {
                if (!hVMLog)
                    printf("Logging enabled for all VMs\n");
                else if (IsValidVMHandle(&hVMLog))
                    printf("Logging restricted to VM %08x\n", hVMLog);
            }
        }
        break;
    case 6:             // "clear"
        logIO.iNext = logIO.iLast = logIO.cEntries = 0;
        break;
    case 7:             // "last"
        return VMMDumpLog(&logIO, TRUE);
    default:
        if (!*pchCmd)
            return VMMDumpLog(&logIO, FALSE);
        return FALSE;
    }
    return TRUE;
}


BOOL VMMDumpVMs(PESF pesf, PCHAR pchCmd)
{
    DWORD hVM;

    hVM = Get_Cur_VM_Handle();
    flKeyEvent &= ~KEYEVENT_ABORT;
    do {
        VMMDumpSpecificVMRegs(hVM);
        if (flKeyEvent & KEYEVENT_ABORT)
            break;
        printf("\n");
        hVM = Get_Next_VM_Handle(hVM);
    } while (!Test_Cur_VM_Handle(hVM));

    return TRUE;
}


BOOL VMMDumpVMRegs(PESF pesf, PCHAR pchCmd)
{
    VMMDumpSpecificVMRegs(Get_Cur_VM_Handle());

    return TRUE;
}


BOOL VMMWatchSwitch(PESF pesf, PCHAR pchCmd)
{
    if (ParseValue(pesf, pchCmd, NULL, &hVMSwitch, TRUE)) {
        if (IsValidVMHandle(&hVMSwitch))
            printf("One-shot task-switch watchpoint for VM %08x enabled\n", hVMSwitch);
    }
    return TRUE;
}


BOOL VMMDumpLog(register PLOG plog, BOOL fLastFirst)
{
    INT iNext, iLast, cLines;
    INT sec, secFrac;
    register PLOGENTRY ple;

    if (!plog->ple) {
        printf("%s %s", plog->pszDesc, szNoLog);
        return TRUE;
    }

    if (fLastFirst) {
        iNext = plog->iNext;
        iLast = plog->iLast;
    } else {
        iNext = plog->iLast;
        iLast = plog->iNext;
    }

    printf("%s log entries: %d%s\n",
            plog->pszDesc, plog->cEntries,
            (plog->cEntries == plog->iMaximum-1)? " (maximum)":NULL);

    if (plog->cEntries == 0)
        return TRUE;

    cLines = 0;
    flKeyEvent &= ~KEYEVENT_ABORT;
    while (iNext != iLast) {

        if (fLastFirst) {
            if (iNext == 0)
                iNext = plog->iMaximum;
            iNext--;
        }

        ple = plog->ple + iNext;

        sec = (ple->msTimeStamp - msDebugStart)/1000;
        secFrac = (ple->msTimeStamp - msDebugStart) - sec*1000;
        secFrac = (secFrac*100)/1000;

        printf("%04x:%08x  %7s %04x,%04x   VM=%08x  time=%d.%02d\n",
                ple->sel,
                ple->off,
                apszLogOp[ple->iOp],
                ple->dwDst,
                ple->dwSrc,
                ple->hVM,
                sec,
                secFrac
        );

        if (!fLastFirst) {
            iNext++;
            if (iNext == plog->iMaximum)
                iNext = 0;
        }

        if (++cLines == 25) {
            _getch();
            cLines = 0;
        }
        if (flKeyEvent & KEYEVENT_ABORT)
            break;
    }
    return TRUE;
}


VOID VMMDumpSpecificVMRegs(DWORD hVM)
{
    PSZ pszDesc;
    ESF esfDummy;
    struct cb_s *pvmcb;
    register struct Client_Reg_Struc *pcrs;

    pvmcb = (struct cb_s *)hVM;

    pcrs = (struct Client_Reg_Struc *)pvmcb->CB_Client_Pointer;

    esfDummy.Flags  = pcrs->Client_EFlags;
    esfDummy.CS     = pcrs->Client_CS;
    esfDummy.EIP    = pcrs->Client_EIP;
    esfDummy.EAX    = pcrs->Client_EAX;
    esfDummy.ECX    = pcrs->Client_ECX;
    esfDummy.EDX    = pcrs->Client_EDX;
    esfDummy.EBX    = pcrs->Client_EBX;
    esfDummy.ESP    = pcrs->Client_ESP;
    esfDummy.EBP    = pcrs->Client_EBP;
    esfDummy.ESI    = pcrs->Client_ESI;
    esfDummy.EDI    = pcrs->Client_EDI;
    esfDummy.DS     = pcrs->Client_DS;
    esfDummy.ES     = pcrs->Client_ES;
    esfDummy.FS     = pcrs->Client_FS;
    esfDummy.GS     = pcrs->Client_GS;
    esfDummy.SS     = pcrs->Client_SS;
    esfDummy.CR0    = 0xffffffff;
    esfDummy.CR2    = 0xffffffff;
    esfDummy.CR3    = 0xffffffff;
    esfDummy.dwIMRs = 0xffffffff;
    esfDummy.hVM    = hVM;

    if (esfDummy.Flags & FLAGS_V86)
        esfDummy.CS |= SEL_V86;

    pszDesc = NULL;
    if (hVM == Get_Sys_VM_Handle())
        pszDesc = "(SYSTEM)";
    if (hVM == Get_Cur_VM_Handle())
        pszDesc = "(CURRENT)";

    printf("VM %08x %s\n", hVM, pszDesc);
    x86RegDump(&esfDummy);
    x86InsDump(&esfDummy, esfDummy.CS, esfDummy.EIP);
}


VOID LogInALnn(register PESF pesf)
{
    LogAppend(&logIO, pesf->hVM,
              pesf->CS, pesf->EIP, LOGOP_INB,
              x86GetByte(pesf->CS | SEL_V86, pesf->EIP+1), (BYTE)pesf->EAX);
}


VOID LogInAXnn(register PESF pesf)
{
    LogAppend(&logIO, pesf->hVM,
              pesf->CS, pesf->EIP, LOGOP_INW,
              x86GetByte(pesf->CS | SEL_V86, pesf->EIP+1), (WORD)pesf->EAX);
}


VOID LogOutnnAL(register PESF pesf)
{
    LogAppend(&logIO, pesf->hVM,
              pesf->CS, pesf->EIP, LOGOP_OUTB,
              x86GetByte(pesf->CS | SEL_V86, pesf->EIP+1), (BYTE)pesf->EAX);
}


VOID LogOutnnAX(register PESF pesf)
{
    LogAppend(&logIO, pesf->hVM,
              pesf->CS, pesf->EIP, LOGOP_OUTW,
              x86GetByte(pesf->CS | SEL_V86, pesf->EIP+1), (WORD)pesf->EAX);
}


VOID LogInALDX(register PESF pesf)
{
    LogAppend(&logIO, pesf->hVM,
              pesf->CS, pesf->EIP, LOGOP_INB,
              (WORD)pesf->EDX, (BYTE)pesf->EAX);
}


VOID LogInAXDX(register PESF pesf)
{
    LogAppend(&logIO, pesf->hVM,
              pesf->CS, pesf->EIP, LOGOP_INW,
              (WORD)pesf->EDX, (WORD)pesf->EAX);
}


VOID LogOutDXAL(register PESF pesf)
{
    LogAppend(&logIO, pesf->hVM,
              pesf->CS, pesf->EIP, LOGOP_OUTB,
              (WORD)pesf->EDX, (BYTE)pesf->EAX);
}


VOID LogOutDXAX(register PESF pesf)
{
    LogAppend(&logIO, pesf->hVM,
              pesf->CS, pesf->EIP, LOGOP_OUTW,
              (WORD)pesf->EDX, (WORD)pesf->EAX);
}


VOID LogAppend(register PLOG plog, DWORD hVM,
               DWORD sel, DWORD off,
               INT iOp, DWORD dwDst, DWORD dwSrc)
{
    register PLOGENTRY ple;

    if (!plog->ple)     // logging not enabled (no buffer)
        return;

    if (hVMLog && hVM != hVMLog)
        return;         // logging restricted to that VM only

    if (piopmLog)
        if (!x86PortTrapped(piopmLog, (WORD)dwDst))
            return;     // logging disabled for this port

    ple = plog->ple + plog->iNext++;

    if (plog->iNext == plog->iMaximum)
        plog->iNext = 0;

    plog->cEntries++;
    if (plog->iNext == plog->iLast) {
        plog->iLast++;
        if (plog->iLast == plog->iMaximum)
            plog->iLast = 0;
        plog->cEntries--;
    }

    ple->hVM = hVM;
    ple->off = off;
    ple->sel = sel;
    ple->iOp = iOp;
    ple->dwDst = dwDst;
    ple->dwSrc = dwSrc;
    ple->msTimeStamp = Get_Last_Updated_System_Time();
}


BOOL IsValidVMHandle(PDWORD phVM)
{
    DWORD hVM;

    hVM = Get_Cur_VM_Handle();
    do {
        if (*phVM == hVM)
            return TRUE;
        hVM = Get_Next_VM_Handle(hVM);
    } while (!Test_Cur_VM_Handle(hVM));

    *phVM = 0;
    return FALSE;
}
