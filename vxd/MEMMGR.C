/*
 *  MEMMGR.C
 *  Memory manager
 *
 *  by Jeff Parsons 24-Jul-1992
 */


#include <all.h>

ASSERTMOD(memmgr.c);


PMB     pmbZero = 0;    // pointer to first megabyte (current VM)
#ifdef  WIN386
PDWORD  pDebugStack = NULL, pDebugStackTop = NULL;
#endif


#ifdef WIN386
#pragma VxD_ICODE_SEG
#endif


VOID InitMemMgr()
{
#ifndef WIN386

    // Remap the first free PTEs to be an alias to the first meg (MB)
    // BUGBUG -- the physical pages beneath are currently being "lost"

    SetPages(pLinFree, MB_ADDR, NUM_PAGES(MB_SIZE));

    // Initialize ptr to 1st meg, via alias we just created with SetPages()

    pmbZero = MemAlloc(SIZE_PAGES(MB_SIZE));

#else

    // Allocate a stack for the debugger to switch to, to avoid eating too
    // much of the VMM's ring 0 stack

    if (pDebugStack = MemAlloc(MAXSTACK))
        pDebugStackTop = pDebugStack + MAXSTACK/sizeof(DWORD);

#endif
}


#ifdef WIN386
#pragma VxD_LOCKED_CODE_SEG
#endif


#ifndef WIN386

VOID SetPages(PVOID pLinear, DWORD dwPhysical, INT nPages)
{
    register PTE pte;

    ASSERT(PAGE_OFFSET(dwPhysical) == 0);

    pte = pPgTbl + PAGE_INDEX(pLinear);

    while (nPages--) {
        *pte++ = dwPhysical | PG_WRITE | PG_PRESENT;
        dwPhysical += PAGE_SIZE;
    }
}

#endif


PVOID MemAlloc(INT cbSize)
{
#ifndef WIN386

    PVOID p;

    p = pFree;
    pFree += cbSize;
    pLinFree += cbSize;

    return p;

#else

    if (Get_VMM_Reenter_Count())
        return NULL;

    if (cbSize < PAGE_SIZE || cbSize & (PAGE_SIZE-1)) {

        return _HeapAllocate(cbSize, HEAPZEROINIT);

    }
    else {

        return _PageAllocate(cbSize >> PAGE_SHIFT,
                             PG_SYS, 0, 0, 0, 0, NULL,
                             PAGEFIXED | PAGEZEROINIT);
    }
#endif
}


BOOL MemFree(PVOID p, INT cbSize)
{
#ifdef WIN386

    if (Get_VMM_Reenter_Count())
        return FALSE;

    if (cbSize < PAGE_SIZE || cbSize & (PAGE_SIZE-1)) {

        return _HeapFree(p, 0);         // returns TRUE if successful

    }
    else {

        return _PageFree(p, 0);         // returns TRUE if successful

    }

#endif
}
