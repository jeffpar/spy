/*
 *  VXDDUMP.C
 *  VxD Dump Support
 *
 *  by Jeff Parsons 20-Apr-1993
 */


#define INC_VMMSYS
#include <all.h>
#include <sym.h>
#include <parse.h>
#include <win386.h>

ASSERTMOD(vxddump.c);


// Globals

DWORD   NumberOfStaticVXDs = 0;
struct  Device_Location_List *pStaticVXDLocationArray = NULL;


// Internal data structures

PSZ     apszVxDDump[]   = {"list"};
PFNDUMP apfnVxDDump[]   = {VxDDump};

PSZ     apszObjName[]   = {OBJNAME_LOCKED, OBJNAME_INIT, OBJNAME_PAGED, OBJNAME_STATIC};


BOOL VxDDumpCommand(PESF pesf, PCHAR pchCmd)
{
    INT i;

    if (*pchCmd == '?') {
        printf("VxD dump commands\n"
               "\tvxdlist\t\tVxD list\n"
        );
        return TRUE;
    }
    if (*pchCmd) {
        pchCmd += nstrskip(pchCmd, ' ');
        i = istrtoken(&pchCmd, apszVxDDump, ARRAYSIZE(apszVxDDump));
        if (i >= 0)
            return (*apfnVxDDump[i])(pesf, pchCmd);
    }
    return FALSE;
}


BOOL VxDDump(PESF pesf, PCHAR pchCmd)
{
    return TRUE;
}


BOOL VxDFindAddress(DWORD dwValue,
                   PSZ pszModName,
                   PPSZ ppszObj, PDWORD pdstObj,
                   PPSZ ppszSym, PDWORD pdstSym)
{
    INT i;
    CHAR ch;
    BOOL fNoChars;
    VXDADDR vxd;
    register PVXDADDR pvxd = &vxd;
    struct VxD_Desc_Block *pddb;


    if (!NumberOfStaticVXDs)
        return FALSE;           // too early in boot process

    if (!VxDFindDynamicAddress(dwValue, pvxd)) {
        if (!VxDFindStaticAddress(dwValue, pvxd)) {
            return FALSE;
        }
    }
    pddb = pvxd->vxd_pDDB;

    // Copy the VXD name, removing space padding at the end

    fNoChars = TRUE;
    for (i=7; i>=0; i--) {
        ch = pddb->DDB_Name[i];
        if( ch == ' ' && fNoChars ) {
            ch = '\0';
        } else {
            fNoChars = FALSE;
        }
        pszModName[i] = ch;
    }
    pszModName[8] = '\0';

    *ppszObj = VxDObjDesc(pvxd->vxd_ObjType);
    *pdstObj = dwValue - pvxd->vxd_ObjAddr;
    *ppszSym = SymFindAddress(pszModName,
                              VxDObjNum(pvxd->vxd_ObjType), *pdstObj, pdstSym);

    return TRUE;
}


BOOL VxDFindDynamicAddress(DWORD dwValue, register PVXDADDR pvxd)
{
    INT i;
    struct DeviceInfo *pdi;
    struct ObjectInfo *poi;

    for (pdi=GetDeviceList(); pdi; pdi=pdi->DI_Next) {

        for (i=0, poi=pdi->DI_ObjInfo; i<pdi->DI_ObjCount; i++,poi++) {

            if (dwValue >= poi->OI_LinearAddress &&
                dwValue < (poi->OI_LinearAddress + poi->OI_Size)) {
                pvxd->vxd_ObjAddr = poi->OI_LinearAddress;
                pvxd->vxd_ObjType = poi->OI_ObjType;
                pvxd->vxd_pDDB = pdi->DI_DDB;
                return TRUE;
            }
        }
    }
    return FALSE;
}


BOOL VxDFindStaticAddress(DWORD dwValue, register PVXDADDR pvxd)
{
    INT i, j;
    struct Device_Location_List  *pdl;
    struct ObjectLocation        *pol;

    for (i=0,pdl=pStaticVXDLocationArray; i<NumberOfStaticVXDs; i++) {

        for (j=0; j<pdl->DLL_NumObjects; j++) {

            pol = &(pdl->DLL_ObjLocation[j]);

            if (dwValue >= pol->OL_LinearAddr &&
                dwValue < (pol->OL_LinearAddr + pol->OL_Size)) {
                pvxd->vxd_ObjAddr = pol->OL_LinearAddr;
                pvxd->vxd_ObjType = pol->OL_ObjType;
                pvxd->vxd_pDDB = (struct VxD_Desc_Block *)(pdl->DLL_DDB);
                return TRUE;
            }
        }
        // Next device location structure follows the previous data array

        pdl = (struct Device_Location_List *)&(pdl->DLL_ObjLocation[pdl->DLL_NumObjects]);
    }
    return FALSE;
}


DWORD VxDFindObject(PSZ pszModName, WORD wObjNum)
{
    DWORD dwValue;

    dwValue = VxDFindDynamicObject(pszModName, wObjNum);
    if (dwValue == -1) {
        dwValue = VxDFindStaticObject(pszModName, wObjNum);
        if (dwValue == -1)
            dwValue = 0;
    }
    return dwValue;
}


DWORD VxDFindDynamicObject(PSZ pszModName, WORD wObjNum)
{
    INT i;
    struct DeviceInfo *pdi;
    struct ObjectInfo *poi;

    for (pdi=GetDeviceList(); pdi; pdi=pdi->DI_Next) {
      //printf("vxdFDO: comparing %8s to %s\n", pdi->DI_DDB->DDB_Name, pszModName);
        if (mystrcmpi(pdi->DI_DDB->DDB_Name,
                      pszModName) == 0) {
            if (!wObjNum)       // if we're just interested in whether or
                return 0;       // not the module already exists, return 0
            for (i=0, poi=pdi->DI_ObjInfo; i<pdi->DI_ObjCount; i++,poi++) {
              //printf("vxdFDO: comparing %d to %d\n", VxDObjNum(poi->OI_ObjType), wObjNum);
                if (VxDObjNum(poi->OI_ObjType) == wObjNum)
                    return poi->OI_LinearAddress;
            }
        }
    }
    return -1;
}


DWORD VxDFindStaticObject(PSZ pszModName, WORD wObjNum)
{
    INT i, j;
    struct Device_Location_List  *pdl;
    struct ObjectLocation        *pol;

    for (i=0,pdl=pStaticVXDLocationArray; i<NumberOfStaticVXDs; i++) {
      //printf("vxdFSO: comparing %8s to %s\n", ((struct VxD_Desc_Block *)pdl->DLL_DDB)->DDB_Name, pszModName);
        if (mystrcmpi(((struct VxD_Desc_Block *)pdl->DLL_DDB)->DDB_Name,
                      pszModName) == 0) {
            if (!wObjNum)       // if we're just interested in whether or
                return 0;       // not the module already exists, return 0
            for (j=0; j<pdl->DLL_NumObjects; j++) {
                pol = &(pdl->DLL_ObjLocation[j]);
              //printf("vxdFSO: comparing %d to %d\n", VxDObjNum(pol->OL_ObjType), wObjNum);
                if (VxDObjNum(pol->OL_ObjType) == wObjNum)
                    return pol->OL_LinearAddr;
            }
        }
        // Next device location structure follows the previous data array
        pdl = (struct Device_Location_List  *)&(pdl->DLL_ObjLocation[pdl->DLL_NumObjects]);
    }
    return -1;
}


PSZ VxDObjDesc(DWORD Type)
{
    switch(Type) {
    case LCODE_OBJ:
        return "Locked Code";
    case LDATA_OBJ:
        return "Locked Data";
    case PCODE_OBJ:
        return "Pageable Code";
    case PDATA_OBJ:
        return "Pageable Data";
    case SCODE_OBJ:
        return "Static Code";
    case SDATA_OBJ:
        return "Static Data";
    case CODE16_OBJ:
        return "Code 16";
    case ICODE_OBJ:
        return "Init Code";
    case IDATA_OBJ:
        return "Init Data";
    case ICODE16_OBJ:
        return "Init Code 16";
    }
    return "Unknown";
}


WORD VxDObjNum(DWORD Type)
{
    switch(Type) {
    case LCODE_OBJ:
        return OBJNUM_LOCKED;
    case LDATA_OBJ:
        return OBJNUM_LOCKED;
    case PCODE_OBJ:
        return OBJNUM_PAGED;
    case PDATA_OBJ:
        return OBJNUM_PAGED;
    case SCODE_OBJ:
        return OBJNUM_STATIC;
    case SDATA_OBJ:
        return OBJNUM_STATIC;
    case CODE16_OBJ:
        return 0;
    case ICODE_OBJ:
        return OBJNUM_INIT;
    case IDATA_OBJ:
        return OBJNUM_INIT;
    case ICODE16_OBJ:
        return 0;
    }
    return 0;
}


WORD VxDObjNumFromName(PSZ pszObjName)
{
    pszObjName += nstrskipto(pszObjName, '_');

    return istrtoken(&pszObjName, apszObjName, ARRAYSIZE(apszObjName)) + 1;
}
