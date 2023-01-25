/*
 *  MEMMGR.C
 *  Memory manager
 *
 *  by Jeff Parsons 24-Jul-1992
 */


#include <all.h>

ASSERTMOD(memmgr.c);


PMB     pmbZero = 0;    // pointer to first megabyte (current VM)


VOID InitMemMgr()
{
    // Remap the first free PTEs to be an alias to the first meg (MB).

    DWORD dwPhys = SetPages(pLinFree, MB_ADDR, NUM_PAGES(MB_SIZE));

    // Since all our PTEs were initially created with linear == physical,
    // verify that the starting address of the physical memory underlying
    // the PTEs we just modified is equal to pLinFree.

    ASSERT(dwPhys == (DWORD)pLinFree);

    // Next, to avoid losing access to that physical memory, we'll set the
    // *next* meg of PTEs to that address.
    //
    // Yes, this means any physical memory underlying *those* PTEs will no
    // longer be accessible.  But this app is very stupid: it doesn't query
    // the amount of physical memory, it simply requires that you have at
    // LEAST 2Mb (since it will never need more than that).
    //
    // So we just want to make sure we don't waste any of that first 2Mb.

    SetPages(pLinFree + MB_SIZE, dwPhys, NUM_PAGES(MB_SIZE));

    // Initialize ptr to 1st meg, via alias we just created with SetPages()

    pmbZero = (PMB)MemAlloc(SIZE_PAGES(MB_SIZE));
}


DWORD SetPages(PVOID pLinear, DWORD dwPhysical, INT nPages)
{
    INT iPage;
    DWORD dwPhys;
    register PTE pte;

    ASSERT(PAGE_OFFSET(dwPhysical) == 0);

    iPage = PAGE_INDEX(pLinear);
    pte = pPgTbl + iPage;
    dwPhys = *pte & PG_FRAME;

    while (nPages-- && iPage++ < PAGE_ENTRIES) {
        *pte++ = dwPhysical | PG_WRITE | PG_PRESENT;
        dwPhysical += PAGE_SIZE;
    }
    return dwPhys;
}


PVOID MemAlloc(INT cbSize)
{
    PVOID p = NULL;

    if (cbSize) {
        p = pFree;
        pFree += cbSize;
        pLinFree += cbSize;
    }
    return p;
}


BOOL MemFree(PVOID p, INT cbSize)
{
    return TRUE;        // BUGBUG: Unimplemented
}
