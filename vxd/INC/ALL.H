/*
 *  Global include file
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


/* ASM
.xlist
ifdef   WIN386
.386p
endif
 */

#ifdef  WIN386
/* XLATOFF */
#include <basedef.h>
/* XLATON */
#ifndef INC_VMMSYS
#include <vmm.h>
#else
#include <vmmsys.h>
#include <vxdldr.h>
#endif
#endif

#include <types.h>
#include <386.h>
#include <387.h>
#include <bios.h>
#include <dos.h>
#include <ems.h>
#include <debug.h>

#include <globals.h>

#include <x86.h>

#include <lib.h>

/* ASM
.list
 */
