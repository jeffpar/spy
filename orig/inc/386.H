/*
 *  386 Processor Definitions
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define FLAGS_CF        0x00000001
#define FLAGS_PF        0x00000004
#define FLAGS_AF        0x00000010
#define FLAGS_ZF        0x00000040
#define FLAGS_SF        0x00000080
#define FLAGS_TF        0x00000100
#define FLAGS_IF        0x00000200
#define FLAGS_DF        0x00000400
#define FLAGS_OF        0x00000800
#define FLAGS_IOPL      0x00003000
#define FLAGS_IOPL0     0x00000000
#define FLAGS_IOPL1     0x00001000
#define FLAGS_IOPL2     0x00002000
#define FLAGS_IOPL3     0x00003000
#define FLAGS_NT        0x00004000
#define FLAGS_BIT15     0x00008000
#define FLAGS_RF        0x00010000
#define FLAGS_V86       0x00020000

#define IS_CARRY(f)     (((f) & FLAGS_CF) != 0)
#define IS_PARITY(f)    (((f) & FLAGS_PF) != 0)
#define IS_AUXCARRY(f)  (((f) & FLAGS_AF) != 0)
#define IS_ZERO(f)      (((f) & FLAGS_ZF) != 0)
#define IS_SIGN(f)      (((f) & FLAGS_SF) != 0)
#define IS_TRACE(f)     (((f) & FLAGS_TF) != 0)
#define IS_INTERRUPT(f) (((f) & FLAGS_IF) != 0)
#define IS_DIRECTION(f) (((f) & FLAGS_DF) != 0)
#define IS_OVERFLOW(f)  (((f) & FLAGS_OF) != 0)


#define CR0_PE          0x00000001      // protection enable
#define CR0_MP          0x00000002      // math present (for WAIT)
#define CR0_EM          0x00000004      // coprocessor emulation
#define CR0_TS          0x00000008      // task switched
#define CR0_ET          0x00000010      // coprocessor extension type (287/387)
#define CR0_PG          0x80000000      // paging enable


#define SEG_SHIFT       4               // shift factor for linear conversion
#define SEGPTOLINP(s,o) ((PVOID)((PBYTE)pmbZero+((s)<<SEG_SHIFT)+(o)))
#define OFFLINP(l)      (((INT)(l)-(INT)pmbZero)&0xF)
#define SEGLINP(l)      (((INT)(l)-(INT)pmbZero)>>SEG_SHIFT)
#define LINPTOADDR(l)   MAKEDWORD(OFFLINP(l),SEGLINP(l))

#define SEL_RPL0        0x0000
#define SEL_RPL1        0x0001
#define SEL_RPL2        0x0002
#define SEL_RPL3        0x0003
#define SEL_MASK        0xFFF8
#define SEL_SHIFT       3
#define SEL_LDT         0x0004

#define SEL_V86         0x00010000      // my own little hacks...
#define SEL_PROT        0x00000000
#define SEL_PROT32      0x00020000


/* Equates for Type field in segment and gate descriptors
 */

#define SEGTYPE_ACC             0x01    // accessed
#define SEGTYPE_RW              0x02    // readable if code, writable if data
#define SEGTYPE_CE              0x04    // conforming if code, expand-down if data
#define SEGTYPE_TYPE            0x18
#define SEGTYPE_SEG             0x10    // segment...
#define SEGTYPE_GATE            0x00    // ...versus gate
#define SEGTYPE_GATETYPE        0x0F
#define SEGTYPE_CODE            0x18    // code segment...
#define SEGTYPE_DATA            0x10    // ...versus data segment
#define SEGTYPE_DPL             0x60
#define SEGTYPE_DPL_0           0x00
#define SEGTYPE_DPL_1           0x20
#define SEGTYPE_DPL_2           0x40
#define SEGTYPE_DPL_3           0x60
#define SEGTYPE_PRESENT         0x80

#define SEGTYPE_0               0x00
#define SEGTYPE_286TSS          0x01
#define SEGTYPE_LDT             0x02
#define SEGTYPE_286TSSBUSY      0x03
#define SEGTYPE_CALLGATE        0x04
#define SEGTYPE_TASKGATE        0x05
#define SEGTYPE_286INTGATE      0x06
#define SEGTYPE_286TRAPGATE     0x07
#define SEGTYPE_8               0x08
#define SEGTYPE_386TSS          0x09
#define SEGTYPE_A               0x0A
#define SEGTYPE_386TSSBUSY      0x0B
#define SEGTYPE_386CALLGATE     0x0C
#define SEGTYPE_D               0x0D
#define SEGTYPE_386INTGATE      0x0E
#define SEGTYPE_386TRAPGATE     0x0F

#define SEGLIMIT_BIG            0x40
#define SEGLIMIT_GRANULAR       0x80

#define SEG_386TSS              (SEGTYPE_PRESENT|SEGTYPE_DPL_0|SEGTYPE_386TSS)

#define SEG_CODE                (SEGTYPE_PRESENT|SEGTYPE_DPL_0|SEGTYPE_CODE|SEGTYPE_RW)
#define SEG_DATA                (SEGTYPE_PRESENT|SEGTYPE_DPL_0|SEGTYPE_DATA|SEGTYPE_RW)

typedef struct _DES {           /* des */
    WORD    wLimit;             // maximum valid offset
    WORD    wBase0_15;
    BYTE    bBase16_23;
    BYTE    bType;
    BYTE    bLimit;             // low 4 bits are bits 16-19 of wLimit
    BYTE    bBase24_31;
} DES;
typedef DES *PDES;


typedef struct _GATE {          /* gate */
    WORD    wBase0_15;
    WORD    wSel;
    BYTE    bReserved;
    BYTE    bType;
    WORD    wBase16_31;
} GATE;
typedef GATE *PGATE;


/* Descriptor Table Register format
 * (ie, the data used to load GDTR, LDTR and IDTR registers)
 */

typedef struct _DTR {           /* dtr */
    WORD    wLimit;
    DWORD   dwBase;
} DTR;
typedef DTR *PDTR;


#define MAX_PORT    MAX_WORD
#define SIZE_IOPM   (MAX_PORT/8+1)


/* Task State Segment format
 * (ie, the data whose selector is loaded in the TR Task Register)
 */

typedef struct _TSS {           /* tss */
    DWORD   selBackLink;
    DWORD   ESP0;
    DWORD   SS0;
    DWORD   ESP1;
    DWORD   SS1;
    DWORD   ESP2;
    DWORD   SS2;
    DWORD   CR3;                // initialize to PDIR physical address
    DWORD   EIP;
    DWORD   Flags;
    DWORD   EAX;
    DWORD   ECX;
    DWORD   EDX;
    DWORD   EBX;
    DWORD   ESP;
    DWORD   EBP;
    DWORD   ESI;
    DWORD   EDI;
    DWORD   ES;
    DWORD   CS;
    DWORD   SS;
    DWORD   DS;
    DWORD   FS;
    DWORD   GS;
    DWORD   selLDT;             // initialize to ZERO
    WORD    wTSSFlags;          // initialize to ZERO (all but bit 0 reserved)
    WORD    offIOPM;            // initialize to offsetof(abIOPM[0])
    BYTE    abIOPM[SIZE_IOPM];  // 8K is large enough for all possible 16-bit ports
} TSS;
typedef TSS *PTSS;


/* Paging equates
 */

#define PAGE_SIZE               4096
#define PAGE_SHIFT              12
#define PAGE_INDEX(p)           ((DWORD)(p) >> PAGE_SHIFT)
#define PAGE_FRAME(p)           ((DWORD)(p) & PG_FRAME)
#define PAGE_OFFSET(p)          ((DWORD)(p) & 0xFFF)
#define PAGE_DIR(p)             ((DWORD)(p) >> (PAGE_SHIFT+10))
#define PAGE_TBL(p)             (((DWORD)(p) >> PAGE_SHIFT) & 0x3FF)
#define NUM_PAGES(cb)           ((cb+PAGE_SIZE-1)/PAGE_SIZE)
#define SIZE_PAGES(cb)          (NUM_PAGES(cb) * PAGE_SIZE)

#define PAGE_ENTRIES            (PAGE_SIZE/4)

#define PG_FRAME                0xFFFFF000
#define PG_AVAIL                0x00000E00
#define PG_RESERVED             0x00000198
#define PG_DIRTY                0x00000040
#define PG_ACCESSED             0x00000020
#define PG_USER                 0x00000004
#define PG_WRITE                0x00000002
#define PG_PRESENT              0x00000001

#define PG_FRAMEINC             0x00001000

typedef DWORD *PTE;


/* Hard-coded interrupt vector definitions
 */

#define IDT_DIVERROR            0x00
#define IDT_DEBUG               0x01
#define IDT_NMI                 0x02
#define IDT_BREAKPOINT          0x03
#define IDT_OVERFLOW            0x04
#define IDT_BOUNDS              0x05
#define IDT_INVOPCODE           0x06
#define IDT_NOCP                0x07
#define IDT_DBLFAULT            0x08
#define IDT_RESERVED09          0x09
#define IDT_TSSFAULT            0x0A
#define IDT_SEGFAULT            0x0B
#define IDT_STACKFAULT          0x0C
#define IDT_GPFAULT             0x0D
#define IDT_PAGEFAULT           0x0E
#define IDT_RESERVED0F          0x0F
#define IDT_CPFAULT             0x10
#define IDT_RESERVED11          0x11
#define IDT_RESERVED12          0x12
#define IDT_RESERVED13          0x13
#define IDT_RESERVED14          0x14
#define IDT_RESERVED15          0x15
#define IDT_RESERVED16          0x16
#define IDT_RESERVED17          0x17
#define IDT_RESERVED18          0x18
#define IDT_RESERVED19          0x19
#define IDT_RESERVED1A          0x1A
#define IDT_RESERVED1B          0x1B
#define IDT_RESERVED1C          0x1C
#define IDT_RESERVED1D          0x1D
#define IDT_RESERVED1E          0x1E
#define IDT_RESERVED1F          0x1F

#define IDT_VMM                 0x20
#define IDT_DEBUGREQ            0x41


/* Soft-coded interrupt vector definitions
 */

#define IDT_OFFSET              0x00

#define IDT_MASTER_BASE        (0x20+IDT_OFFSET)
#define IDT_TMR                (0x20+IDT_OFFSET)
#define IDT_KBD                (0x21+IDT_OFFSET)
#define IDT_SLAVE              (0x22+IDT_OFFSET)
#define IDT_COM2               (0x23+IDT_OFFSET)
#define IDT_COM1               (0x24+IDT_OFFSET)
#define IDT_LPT2               (0x25+IDT_OFFSET)
#define IDT_DISKETTE           (0x26+IDT_OFFSET)
#define IDT_LPT1               (0x27+IDT_OFFSET)

#define IDT_SLAVE_BASE         (0x28+IDT_OFFSET)
#define IDT_RTC                (0x28+IDT_OFFSET)
#define IDT_IRQ2               (0x29+IDT_OFFSET)
#define IDT_IRQ10              (0x2A+IDT_OFFSET)
#define IDT_IRQ11              (0x2B+IDT_OFFSET)
#define IDT_IRQ12              (0x2C+IDT_OFFSET)
#define IDT_CP                 (0x2D+IDT_OFFSET)
#define IDT_DISK               (0x2E+IDT_OFFSET)
#define IDT_IRQ15              (0x2F+IDT_OFFSET)


/* Total of hard-coded and soft-coded entries
 */

#define IDT_ENTRIES            (0x30+IDT_OFFSET)
#define IDT_SIZE               (IDT_ENTRIES * sizeof(DES))


/* Exception stack frame (also used for interrupts, with dummy iErrCode)
 */

typedef struct _ESF {           /* esf */
    DWORD   hVM;
    DWORD   defData;
    DWORD   defStack;
    DWORD   defOverride;
    DWORD   dwIMRs;
    DWORD   CR3;
    DWORD   CR2;
    DWORD   CR0;
    DWORD   SS;
    DWORD   GS;
    DWORD   FS;
    DWORD   ES;
    DWORD   DS;
    DWORD   saveES;
    DWORD   saveDS;
    DWORD   EDI;
    DWORD   ESI;
    DWORD   EBP;
    DWORD   ESP;
    DWORD   EBX;
    DWORD   EDX;
    DWORD   ECX;
    DWORD   EAX;                // top of PUSHA frame
    DWORD   iTrap;              // trap/interrupt #
    DWORD   iErrCode;           // error code (sometimes simulated)
    DWORD   EIP;
    DWORD   CS;
    DWORD   Flags;
    DWORD   v86ESP;
    DWORD   v86SS;
    DWORD   v86ES;
    DWORD   v86DS;
    DWORD   v86FS;
    DWORD   v86GS;
} ESF;
typedef ESF *PESF;


/* Interrupt stack frame (bare-bones)
 */

typedef struct _ISF {           /* isf */
    DWORD   iTrap;              // interrupt #
    DWORD   iErrCode;           // error code (not used, always 0)
    DWORD   EIP;
    DWORD   CS;
    DWORD   Flags;
} ISF;
typedef ISF *PISF;


/* v86-mode interrupt return frame
 */

typedef struct _VIRF {          /* virf */
    WORD    IP;
    WORD    CS;
    WORD    Flags;
} VIRF;
typedef VIRF *PVIRF;


/* ASM

Check286 macro
        pushf                   ;; save current flags
        sub     ax,ax
        push    ax
        popf
        pushf
        pop     ax
        popf                    ;; restore current flags
        and     ax,FLAGS_IOPL or FLAGS_NT or FLAGS_BIT15
        cmp     ax,FLAGS_IOPL or FLAGS_NT or FLAGS_BIT15
endm

Check386 macro
        pushf
        mov     ax,FLAGS_IOPL or FLAGS_NT
        push    ax
        popf
        pushf
        pop     ax
        popf
        and     ax,FLAGS_IOPL or FLAGS_NT
endm

IODelay macro
        jmp     $+2
        jmp     $+2
endm

*/
