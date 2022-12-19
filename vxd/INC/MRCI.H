/****************************************************************************
*  MRCI.H - Microsoft Real-Time Compression Interface definitions
*
*  Microsoft Confidential
*  Copyright (C) Microsoft Corporation 1992-1993
*  All Rights Reserved.
*
*  History:
*   18-Jul-92 bens      Initial version.
*   25-Jul-92 billkru   Adapted for use by MS-Flash & ROMDOS environment.
*
*  VxD History:
*   21-Apr-93 jeffpar   Adapted for MRCI32.386, converted to .H file, and
*                       moved to DOS\INC directory.
*****************************************************************************/

#include <mult.h>


/*
 *  MRCI interrupt number (2Fh)
 */
#define intMRCI         INT_MULT
/*
 *  intMRCI AX for detecting MRCI server (4A12h)
 */
#define mrciDETECT      ((I2F_MULTIPURPOSE << 8) | I2FMULT_MRCI)
/*
 *  intMRCI AX for back-patch (4A13h)
 */
#define mrciBackPatch   ((I2F_MULTIPURPOSE << 8) | I2FMULT_MRCIBackPatch)


#define intMRCIROM      0x1A    // ROM MRCI interrupt number
#define mrciDETECTROM   0xB001  // ROM intMRCI AX for detecting MRCI server


/****************************************************************************
*  sigOLD_CX, sigOLD_DX, sigNEW_CX, sigNEW_DX - MRCI Server detection
*
*  These values are used to verify that the response from issuing
*  intMRCI is coming from an MRCI server, and not some other piece
*  of code.
*
*  The *old* values are passed on the mrciQUERY call, and the server
*  must change CX:DX to the *new* values, so that the caller can
*  trust that the MRCI server was responding, and not some other
*  interrupt hooker.
*
*  The 16-bit Server uses this code sequence to transform CX:DX:
*
*           ;------------- entry: cx='ab'  dx='cd'
*           xchg    ch,cl       ; cx='ba'  dx='cd'
*           xchg    dh,dl       ; cx='ba'  dx='dc'
*           xchg    dx,cx       ; cx='dc'  dx='ba'
*
*  The 32-bit Server uses this code sequence to transform EDX:
*
*           ;------------- entry: edx='abcd'
*           xchg    dh,dl       ; edx='abdc'
*           rol     edx,16      ; edx='dcab'
*           xchg    dh,dl       ; edx='dcba'
*****************************************************************************/

#define sigOLD_CX   0x524D      // 'MR'
#define sigOLD_DX   0x4943      // 'CI'
#define sigOLD_EDX  0x4943524D  // 'MRCI'

#define sigNEW_CX   0x4349      // 'IC'
#define sigNEW_DX   0x4D52      // 'RM'
#define sigNEW_EDX  0x4D524349  // 'ICRM'


/****************************************************************************
*    mcXXXX - flag values passed to MRCI operations
*
*       MRCCompress and MRCDecompress take a flag to indicate whether the
*       client is a *system* component (and hence may call with InDOS
*       set), or an *application*.
*
*       If a system client calls MRCCompress/MRCDecompress with mcSYSTEM,
*       then it is the clients responsibility to ensure that one of the
*       following is true:
*           1)  InDOS is set
*           2)  The call is made at interrupt time.
*
*       FAILING TO FOLLOW THE ABOVE RULE WILL LIKELY RESULT IN A SYSTEM
*       HANG AND LOSS OF USER DATA.
*****************************************************************************/

#define mcAPPLICATION   0       // Client is an application
#define mcSYSTEM        1       // Client is a file system driver


/****************************************************************************
*  MRCI Server Busy flag values
*****************************************************************************/

#define MRCI_SERVER_IDLE    0


/****************************************************************************
*   MRCINFO - MRC Information data structure
*
*   A pointer to an MRCINFO structure is returned from MRCQuery, and sent
*   on MRCNotifyLoad.  This structure contains information on the MRCI
*   server and its capabilities.
*****************************************************************************/

typedef struct MRCINFO {
    DWORD mi_lVendor;           // A 4-byte vendor ID.
                                // Microsoft's vendor ID is "MSFT".
                                //
    WORD  mi_wVendorVersion;    // Version number of the MRC server.
                                // High byte is major number, low byte is minor.
                                // EXAMPLES: v3.20 = 0314h, v10.01 = 0A01h
                                //
    WORD  mi_wMRCIVersion;      // Version number of the MRCI supported by
                                // this server.
                                //
    DWORD mi_pfnOperate;        // Far pointer of the server compression entry
                                // point.
                                //
    WORD  mi_flCapability;      // Bit field of server capabilities
                                // See micapXXX for bit definitions
                                //
    WORD  mi_flHWAssist;        // Bit field of hardware assisted
                                // server capabilities.  One-to-one
                                // correspondance with mi_flCapability
                                // bits.  A bit set in this field
                                // indicates the corresponding
                                // capability is hardware assisted.
                                //
    WORD  mi_cbMax;             // Maximum number of bytes that the compression
                                // services provider can compress or decompress.
                                // Requests to compress or decompress buffers in
                                // excess of this length will fail.
                                // All MRCI servers must support at least
                                // 8192 byte (8Kb) blocks.
} MRCINFO;
typedef MRCINFO *PMRCINFO;


/****************************************************************************
*    micapXXXXX - bit flags for MRCINFO.mi_flCapabilities
*
*       These define both the capabilities of the Server, and also double
*       as *operation* codes passed to the mi_pfnOperate entry point in
*       the server.
*                       111111
*                       5432109876543210
*****************************************************************************/

#define micapNONE           0x0000  // No capabilities
#define micapSTANDARD       0x0001  // Standard compress
#define micapDECOMPRESS     0x0002  // Standard compress
#define micapUPDATE         0x0004  // Update compress
#define micapSUPER          0x0008  // SuperCompress(tm)
#define micapSUPERUPDATE    0x0010  // Update SuperCompress(tm)
#define micapINCDECOMP      0x0020  // Incremental Decompress
#define micapMRCI32         0x0040  // Server interface is 32-bit
                                    // Remaining bits (7..14) are RESERVED and must be 0
#define micapREADONLY       0x8000  // MRCINFO structure is read-only
#define micapDEINSTALL      -1      // Server deinstall service


/****************************************************************************
*  MRCREQUEST - MRC compress/decompress Request packet
*
*  This structure is used to pass parameters to the server for
*  compress/decompress operations.
*
*  General Notes
*  -------------
*  1) <mr_pbSrc,mr_cbSrc> and <mr_pbDst,mr_cbDst> MUST NOT OVERLAP!
*
*  2) The safest practice is for mr_cbSrc and mr_cbDst to be identical
*     (except for decompress where mr_cbSrc is ignored and mr_cbDst is
*      the count of bytes to uncompress).
*
*  Details on Structure Members
*  ----------------------------
*  mr_pbSrc
*    This points to the *source* buffer.
*
*    On a *compress* operation, the contents of this buffer are
*    *uncompressed* data.
*
*    On an *update* operation, the contents of this buffer starting
*    at offset mr_offUpdate is valid, uncompressed data.  The contents
*    from the start of the buffer to the byte just prior to
*    mr_OffUpdate are ignored.
*
*    On a *decompress* operation, the contents of this buffer are
*    *compressed* data.
*
*  mr_cbSrc
*    This is the size of the *source* buffer.
*
*    This is the amount of data to be compressed or decompressed.
*    NOTE: For a decompress operation, this value may be ignored
*          by many implentations, since the MRC Format encoding has
*          a special code to indicate the end of the compressed data.
*
*  mr_offUpdate
*    NOTE: This is used for Update(Super)Compress only.
*
*    On INPUT, this is the offset (same segment as mr_pbSrc) in the
*    *source* buffer where the changed uncompressed data starts.
*
*    On OUTPUT, this is the offset (same segment as mr_pbDst) in the
*    *destination* buffer where the changed compressed data starts.
*
*  mr_pbDst
*    This points to the *destination* buffer.
*
*    On a *compress* operation, this buffer receives the *compressed*
*    result of the operation.
*
*    On an *update* operation, the contents of this buffer MUST be
*    the compressed representation of the uncompressed data in the
*    *source* buffer.  After the operation is completed, this buffer
*    receives the *compressed* result of the operation.
*
*    On a *decompress* operation, this buffer receives the
*    *uncompressed* result of the operation.
*
*  mr_cbDst
*    On a *compress* operation and on INPUT, this is the size of
*    the *destination* buffer.  If, during the operation, the
*    Server would overflow this buffer length, then the operation
*    fails and the server returns the error MRCI_ERROR_BUFFER_OVERFLOW.
*
*    On a *decompress* operation and on INPUT, this is what the
*    *decompressed* size should be.  If, during the operation, the
*    Server would overflow this length, then the operation fails
*    and the server returns the error MRCI_ERROR_BUFFER_OVERFLOW.
*
*    On OUTPUT, the Server updates this field with the actual size
*    of the resulting compressed/uncompressed data.
*
*  mr_cbChunk
*    This is information that the Server compress routines can use to
*    "early out" of the compression as early as possible.
*
*    Valid values are 1 (client is interested in savings as small as
*    1 byte) to 32767.  DblSpace passes 512, and MS-Flash passes 1.
*
*    This field is most easily explained by giving an example:
*
*    Example:
*      DblSpace does space allocation in chunks of 512 bytes (the
*      common sector size on a disk).
*
*      The compression server can use this information for two
*      optimizations:
*
*      1) If the Server cannot compress the uncompressed data
*         enough to save at least 512 bytes, then the data is
*         *incompressible* as far as DblSpace is concerned, even
*         if it could be compressed to save fewer than 512 bytes.
*
*      2) While compressing, if the Server gets to a point where
*         the remaining uncompressed data is of such a length that
*         it can be encoded simply (without table lookups, etc.)
*         and not cross a 512 byte boundary, then the Server can
*         do the simple encoding.
*
*      It is likely that these optimizations will be hard to perform
*      quickly in software, but it is possible that hardware can do
*      these optimizations without any performance loss.
*****************************************************************************/

typedef struct MRCREQUEST {
    DWORD mr_pbSrc;             // Pointer to source buffer
    WORD  mr_cbSrc;             // Size of source buffer, in bytes
    WORD  mr_offUpdate;         // For UpdateCompress only: offset of (source) destination
                                // buffer where changed (un)compressed data starts.
    DWORD mr_pbDst;             // Pointer to destination buffer
    WORD  mr_cbDst;             // Size of destination buffer, in bytes
    WORD  mr_cbChunk;           // Client compressed data storage chunk size (see above!)
    DWORD mr_dwIncDecomp;       // Incremental Decompression state
} MRCREQUEST;
typedef MRCREQUEST *PMRCREQUEST;


/****************************************************************************
*   MRCI_ERROR_XXX definitions
*
*   Error codes returned from a request
*****************************************************************************/

#define MRCI_ERROR_NONE             0   // No error
#define MRCI_ERROR_NOT_SUPPORTED    1   // Unsupported operation requested
#define MRCI_ERROR_BUSY             2   // Server is busy
#define MRCI_ERROR_BUFFER_OVERFLOW  3   // Destination buffer too small
#define MRCI_ERROR_NOT_COMPRESSIBLE 4   // Data could not be compressed
#define MRCI_ERROR_BAD_MRC_FORMAT   5   // Compressed data format is bad


/***************************************************************************
*   DblSpace MRCI Server constants
****************************************************************************/

#define MRCIVERSION     0x0100          // MRCI version supported by this server
#define MYVERSION       0x0100          // Version of this server/driver

#define MYVENDOR        0x5446534D      // "MSFT" (reversed for byte ordering)
#define MYVENDORhi      0x5446          // High word of MYVENDOR
#define MYVENDORlo      0x534D          // Low word of MYVENDOR

/*
 * Max buffer size set to 32k to avoid problems with compression code and
 * buffer offsets near 0E000h.  See comments in rcomp.asm for details.
 */
#define MYCBMAX         (32*1024)       // Maximum compression buffer size

/*
 * This server's capabilities.
 */
#define micapMINE       (micapSTANDARD | micapDECOMPRESS | micapINCDECOMP | micapMRCI32)

#define MYHWASSIST      0               // We have no hardware assist


/***************************************************************************
*  MRCI32 VxD Support
****************************************************************************/

//  Equates for version number

#define MRCI32_MAJOR_VER    0x04
#define MRCI32_MINOR_VER    0x00

#define MRCI32_DEVICE_ID    0x0040
#define MRCI32_INIT_ORDER   UNDEFINED_INIT_ORDER


/*XLATOFF*/
#define MRCI32_Service  Declare_Service
/*XLATON*/

/*MACROS*/
Begin_Service_Table(MRCI32, VxD)

MRCI32_Service          (MRCI32_Get_Version, LOCAL)
MRCI32_Service          (MRCI32_Server, LOCAL)
MRCI32_Service          (MRCI32_Compress, LOCAL)
MRCI32_Service          (MRCI32_Decompress, LOCAL)

End_Service_Table(MRCI32, VxD)

/*ENDMACROS*/

/*XLATOFF*/
#pragma warning (disable:4035)		// turn off no return code warning

INT VXDINLINE MRCI32_Get_Version()
{
    VxDCall(MRCI32_Get_Version)
}

INT VXDINLINE MRCI32_Compress(PVOID pSrc, INT cbSrc, PVOID pDst, INT cbDst)
{
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    _asm push pSrc
    _asm push cbSrc
    _asm push pDst
    _asm push cbDst
    VxDCall(MRCI32_Compress)
}

INT VXDINLINE MRCI32_Decompress(PVOID pSrc, PVOID pDst, INT cbDst)
{
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    _asm push pSrc
    _asm push pDst
    _asm push cbDst
    VxDCall(MRCI32_Decompress)
}

#pragma warning (default:4035)		// turn on no return code warning
/*XLATON*/
