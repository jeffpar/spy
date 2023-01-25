/*
 *  MAIN.C
 *  Main protected-mode module
 *
 *  by Jeff Parsons 05-Jun-1992
 */


#include <all.h>
#include <sym.h>

ASSERTMOD(main.c);


CHAR szTitle[] = "386 Monitor v0.1 by Jeff Parsons, June 5, 1992\n\n";


VOID Main(ESF esf)
{
    // Initialize the memory manager

    InitMemMgr();

    // Initialize the video manager (MUST be done after memmgr!)

    InitVideoMgr();

    // The linker doesn't fixup BSS data correctly in .COM file version,
    // so assert that the size of any BSS or c_common data is nil.

    ASSERT(cbBSSData == 0);

    printf(szTitle);

    x86Debug(&esf, DEBUG_INIT);
}
