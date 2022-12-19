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
    // Remap the first free PTEs to be an alias to the first meg (MB)
    // BUGBUG -- the physical pages beneath are currently being "lost"

    SetPages(pLinFree, MB_ADDR, NUM_PAGES(MB_SIZE));

    // Initialize ptr to 1st meg, via alias we just created with SetPages()

    pmbZero = MemAlloc(SIZE_PAGES(MB_SIZE));
}


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


PVOID MemAlloc(INT cbSize)
{
    PVOID p;

    p = pFree;
    pFree += cbSize;
    pLinFree += cbSize;

    return p;
}


BOOL MemFree(PVOID p, INT cbSize)
{
}
