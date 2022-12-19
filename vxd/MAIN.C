/*
 *  MAIN.C
 *  Main protected-mode module
 *
 *  by Jeff Parsons 05-Jun-1992
 */


#include <all.h>
#include <sym.h>

ASSERTMOD(main.c);


#ifdef WIN386
CHAR szTitle[] = "SPY system monitor/debugger\n"
                 "VxD version 0.1 by Jeff Parsons, June 5, 1992\n\n";
#else
CHAR szTitle[] = "386 Monitor v0.1 by Jeff Parsons, June 5, 1992\n\n";
#endif


VOID Main(ESF esf)
{
    // Initialize the memory manager

    InitMemMgr();

    // Initialize the video manager (MUST be done after memmgr!)

    InitVideoMgr();

#ifndef WIN386
    // The linker doesn't fixup BSS data correctly in .COM file version,
    // so assert that the size of any BSS or c_common data is nil.

    ASSERT(cbBSSData == 0);
#endif

    printf(szTitle);

#ifdef WIN386
    SymLoadInit();
#endif

    x86Debug(&esf, DEBUG_INIT);
}
