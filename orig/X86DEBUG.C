/*
 *  X86DEBUG.C
 *  x86 Debugger Support
 *
 *  by Jeff Parsons 05-Jun-1992
 */


#include <all.h>
#include <com.h>
#include <sym.h>
#include <parse.h>

ASSERTMOD(x86debug.c);


/*
 * Public debugger variables
 */
INT     iIDTEntry = 0;
INT     iDebugEntry = 0;
DWORD   flTrace = TRACE_NONE;
DWORD   msDebugStart = 0;

/*
 * Private debugger variables
 */
LONG    lInsCount = 0;
INT     iSizeLast = 1;
DWORD   selCodeLast = 0, offCodeLast = 0;
DWORD   selDataLast = 0, offDataLast = 0;
INT     cbpdGlobal = 0;
BPD     abpdGlobal[MAX_BP] = { {0,0,0} };
PBPD    pbpdTempDisable = NULL;
PSZ     apszOptions[] = {"com", "idt", "vga"};
PSZ     apszIDTOptions[] = {"on", "off"};

/*
 * Dummy descriptor for v86-mode selectors
 */
CONST DES desV86 = {0xFFFF, 0, 0, SEGTYPE_PRESENT|SEGTYPE_DPL_3|SEGTYPE_DATA};

/*
 * Initialization table for LoadFrame/SaveFrame functions
 */
struct {
    int offSel;
    int offv86Sel;
} aoffSel[] = {OFFSETOF(ESF,CS), OFFSETOF(ESF,CS),
               OFFSETOF(ESF,DS), OFFSETOF(ESF,v86DS),
               OFFSETOF(ESF,ES), OFFSETOF(ESF,v86ES),
               OFFSETOF(ESF,FS), OFFSETOF(ESF,v86FS),
               OFFSETOF(ESF,GS), OFFSETOF(ESF,v86GS),
               OFFSETOF(ESF,SS), OFFSETOF(ESF,v86SS),
};

/*
 * The rest of the data in this module is for the disassembler
 */
PSZ apszMnemonic[MTOTAL] = {
    "???",    "aaa",	"aad",	  "aam",    "aas",    "adc",	"add",
    "and",    "arpl",   "as:",    "bound",  "bsf",    "bsr",    "bt",
    "btc",    "btr",	"bts",	  "call",   "cbw",    "clc",	"cld",
    "cli",    "clts",   "cmc",    "cmp",    "cmpsb",  "cmps",   "cs:",
    "cwd",    "daa",	"das",	  "dec",    "div",    "ds:",	"enter",
    "es:",    "esc",	"fadd",   "fbld",   "fbstp",  "fcom",	"fcomp",
    "fdiv",   "fdivr",	"fiadd",  "ficom",  "ficomp", "fidiv",	"fidivr",
    "fild",   "fimul",	"fist",   "fistp",  "fisub",  "fisubr", "fld",
    "fldcw",  "fldenv", "fmul",   "fnsave", "fnstcw", "fnstenv","fnstsw",
    "frstor", "fs:",	"fst",	  "fstp",   "fsub",   "fsubr",	"gbp",
    "gs:",    "hlt",	"idiv",   "imul",   "in",     "inc",	"ins",
    "int",    "int3",	"into",   "iret",   "jbe",    "jb",	"jcxz",
    "jecxz",  "jg",     "jge",    "jl",     "jle",    "jmp",    "jnb",
    "jnbe",   "jno",    "jnp",    "jns",    "jnz",    "jo",     "jp",
    "js",     "jz",     "lahf",   "lar",    "lds",    "lea",    "leave",
    "les",    "lfs",    "lgdt",   "lgs",    "lidt",   "lldt",   "lmsw",
    "lock",   "lodsb",  "lods",   "loop",   "loopnz", "loopz",  "lsl",
    "lss",    "ltr",    "mov",    "movsb",  "movs",   "movsx",  "movzx",
    "mul",    "neg",    "nop",    "not",    "or",     "os:",    "out",
    "outs",   "pop",    "popa",   "popf",   "push",   "pusha",  "pushf",
    "rcl",    "rcr",    "repnz",  "repz",   "ret",    "retf",   "rol",
    "ror",    "sahf",   "sar",    "sbb",    "scasb",  "scas",   "setbe",
    "setc",   "setg",   "setge",  "setl",   "setle",  "setnbe", "setnc",
    "setno",  "setnp",  "setns",  "setnz",  "seto",   "setp",   "sets",
    "setz",   "sgdt",   "shl",    "shld",   "shr",    "shrd",   "sidt",
    "sldt",   "smsw",   "ss:",    "stc",    "std",    "sti",    "stosb",
    "stos",   "str",    "sub",    "test",   "verr",   "verw",   "wait",
    "xchg",   "xlat",   "xor",
};


// The following register descriptor array is organized in groups of 8:
//
//     0-7: byte regs                   RD_BYTE
//    8-15: word regs                   RD_WORD
//   16-21: segment regs                RD_SEG
//   24-31: dword regs                  RD_DWORD
//      32: dword control regs          RD_CTL
//      40: dword debug regs            RD_DBG
//      48: dword test regs             RD_TST
//      56: flags                       RD_FLAGS
//
// Thus, the size of a register operand (ie, 1, 2 or 4), less 1, times 8,
// equals the index of the first register for that group.  The gap left by
// the absence of any 3-byte registers is filled, as you can see, with
// segment and other registers.

RD ardRegs[] = {
   {"al",   OFFSETOF(ESF,EAX),      1},     // &AL  @+0
   {"cl",   OFFSETOF(ESF,ECX),      1},     // &CL  @+1
   {"dl",   OFFSETOF(ESF,EDX),      1},     // &DL  @+2
   {"bl",   OFFSETOF(ESF,EBX),      1},     // &BL  @+3
   {"ah",   OFFSETOF(ESF,EAX)+1,    1},     // &AH  @+4
   {"ch",   OFFSETOF(ESF,ECX)+1,    1},     // &CH  @+5
   {"dh",   OFFSETOF(ESF,EDX)+1,    1},     // &DH  @+6
   {"bh",   OFFSETOF(ESF,EBX)+1,    1},     // &BH  @+7
   {"ax",   OFFSETOF(ESF,EAX),      2},     // &AX  @+8
   {"cx",   OFFSETOF(ESF,ECX),      2},     // &CX  @+9
   {"dx",   OFFSETOF(ESF,EDX),      2},     // &DX  @+10
   {"bx",   OFFSETOF(ESF,EBX),      2},     // &BX  @+11
   {"sp",   OFFSETOF(ESF,ESP),      2},     // &SP  @+12
   {"bp",   OFFSETOF(ESF,EBP),      2},     // &BP  @+13
   {"si",   OFFSETOF(ESF,ESI),      2},     // &SI  @+14
   {"di",   OFFSETOF(ESF,EDI),      2},     // &DI  @+15
   {"es",   OFFSETOF(ESF,ES),       3},     // &ES  @+16
   {"cs",   OFFSETOF(ESF,CS),       3},     // &CS  @+17
   {"ss",   OFFSETOF(ESF,SS),       3},     // &SS  @+18
   {"ds",   OFFSETOF(ESF,DS),       3},     // &DS  @+19
   {"fs",   OFFSETOF(ESF,FS),       3},     // &FS  @+20
   {"gs",   OFFSETOF(ESF,GS),       3},     // &GS  @+21
   {"ip",   OFFSETOF(ESF,EIP),      2},     // &IP  @+22
   {"eip",  OFFSETOF(ESF,EIP),      4},     // &EIP @+23
   {"eax",  OFFSETOF(ESF,EAX),      4},     // &EAX @+24
   {"ecx",  OFFSETOF(ESF,ECX),      4},     // &ECX @+25
   {"edx",  OFFSETOF(ESF,EDX),      4},     // &EDX @+26
   {"ebx",  OFFSETOF(ESF,EBX),      4},     // &EBX @+27
   {"esp",  OFFSETOF(ESF,ESP),      4},     // &ESP @+28
   {"ebp",  OFFSETOF(ESF,EBP),      4},     // &EBP @+29
   {"esi",  OFFSETOF(ESF,ESI),      4},     // &ESI @+30
   {"edi",  OFFSETOF(ESF,EDI),      4},     // &EDI @+31
   {"cr0",  OFFSETOF(ESF,CR0),      4},     //
   {"???",  0,                      0},     //
   {"cr2",  OFFSETOF(ESF,CR2),      4},     //
   {"cr3",  OFFSETOF(ESF,CR3),      4},     //
   {"???",  0,                      0},     //
   {"???",  0,                      0},     //
   {"???",  0,                      0},     //
   {"???",  0,                      0},     //
   {"dr0",  0,                      0},     //
   {"dr1",  0,                      0},     //
   {"dr2",  0,                      0},     //
   {"dr3",  0,                      0},     //
   {"???",  0,                      0},     //
   {"???",  0,                      0},     //
   {"dr6",  0,                      0},     //
   {"dr7",  0,                      0},     //
   {"???",  0,                      0},     //
   {"???",  0,                      0},     //
   {"???",  0,                      0},     //
   {"???",  0,                      0},     //
   {"???",  0,                      0},     //
   {"???",  0,                      0},     //
   {"tr6",  0,                      0},     //
   {"tr7",  0,                      0},     //
   {"flags",OFFSETOF(ESF,Flags),    4},     // Flags
   {NULL,   0,                      0},     // end of table marker
};


CHAR achScale[4] = {'1', '2', '4', '8'};


PSZ apszRM[8] = {
    "bx+si","bx+di","bp+si","bp+di","si",   "di",   "bp",   "bx",
};


ID aidBase[256] = {
 /*00*/ {M_ADD,   F_ADDB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_REG|TYPE_BYTE|TYPE_IN},
 /*01*/ {M_ADD,   F_ADDW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*02*/ {M_ADD,   F_ADDB,   TYPE_REG|TYPE_BYTE|TYPE_BOTH,   TYPE_MODRM|TYPE_BYTE|TYPE_IN},
 /*03*/ {M_ADD,   F_ADDW,   TYPE_REG|TYPE_WORDD|TYPE_BOTH,  TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*04*/ {M_ADD,   F_ADDB,   TYPE_AL|TYPE_BOTH,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*05*/ {M_ADD,   F_ADDW,   TYPE_AX|TYPE_BOTH,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*06*/ {M_PUSH,  F_PUSH,   TYPE_ES|TYPE_IN,		    TYPE_NONE},
 /*07*/ {M_POP,   F_POP,    TYPE_ES|TYPE_OUT,		    TYPE_NONE},

 /*08*/ {M_OR,	  F_ORB,    TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_REG|TYPE_BYTE|TYPE_IN},
 /*09*/ {M_OR,	  F_ORW,    TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*0A*/ {M_OR,	  F_ORB,    TYPE_REG|TYPE_BYTE|TYPE_BOTH,   TYPE_MODRM|TYPE_BYTE|TYPE_IN},
 /*0B*/ {M_OR,	  F_ORW,    TYPE_REG|TYPE_WORDD|TYPE_BOTH,  TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*0C*/ {M_OR,	  F_ORB,    TYPE_AL|TYPE_BOTH,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*0D*/ {M_OR,	  F_ORW,    TYPE_AX|TYPE_BOTH,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*0E*/ {M_PUSH,  F_PUSH,   TYPE_CS|TYPE_IN,		    TYPE_NONE},
 /*0F*/ {M_NONE,  F_NONE,   TYPE_NONE|TYPE_286},

 /*10*/ {M_ADC,   F_ADCB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_REG|TYPE_BYTE|TYPE_IN},
 /*11*/ {M_ADC,   F_ADCW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*12*/ {M_ADC,   F_ADCB,   TYPE_REG|TYPE_BYTE|TYPE_BOTH,   TYPE_MODRM|TYPE_BYTE|TYPE_IN},
 /*13*/ {M_ADC,   F_ADCW,   TYPE_REG|TYPE_WORDD|TYPE_BOTH,  TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*14*/ {M_ADC,   F_ADCB,   TYPE_AL|TYPE_BOTH,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*15*/ {M_ADC,   F_ADCW,   TYPE_AX|TYPE_BOTH,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*16*/ {M_PUSH,  F_PUSH,   TYPE_SS|TYPE_IN,		    TYPE_NONE},
 /*17*/ {M_POP,   F_POP,    TYPE_SS|TYPE_OUT,		    TYPE_NONE},

 /*18*/ {M_SBB,   F_SBBB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_REG|TYPE_BYTE|TYPE_IN},
 /*19*/ {M_SBB,   F_SBBW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*1A*/ {M_SBB,   F_SBBB,   TYPE_REG|TYPE_BYTE|TYPE_BOTH,   TYPE_MODRM|TYPE_BYTE|TYPE_IN},
 /*1B*/ {M_SBB,   F_SBBW,   TYPE_REG|TYPE_WORDD|TYPE_BOTH,  TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*1C*/ {M_SBB,   F_SBBB,   TYPE_AL|TYPE_BOTH,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*1D*/ {M_SBB,   F_SBBW,   TYPE_AX|TYPE_BOTH,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*1E*/ {M_PUSH,  F_PUSH,   TYPE_DS|TYPE_IN,		    TYPE_NONE},
 /*1F*/ {M_POP,   F_POP,    TYPE_DS|TYPE_OUT,		    TYPE_NONE},

 /*20*/ {M_AND,   F_ANDB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_REG|TYPE_BYTE|TYPE_IN},
 /*21*/ {M_AND,   F_ANDW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*22*/ {M_AND,   F_ANDB,   TYPE_REG|TYPE_BYTE|TYPE_BOTH,   TYPE_MODRM|TYPE_BYTE|TYPE_IN},
 /*23*/ {M_AND,   F_ANDW,   TYPE_REG|TYPE_WORDD|TYPE_BOTH,  TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*24*/ {M_AND,   F_ANDB,   TYPE_AL|TYPE_BOTH,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*25*/ {M_AND,   F_ANDW,   TYPE_AX|TYPE_BOTH,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*26*/ {M_ES,    F_ES,     TYPE_NONE|TYPE_OVERRIDE|OVERRIDE_ES},
 /*27*/ {M_DAA,   F_DAA,    TYPE_NONE},

 /*28*/ {M_SUB,   F_SUBB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_REG|TYPE_BYTE|TYPE_IN},
 /*29*/ {M_SUB,   F_SUBW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*2A*/ {M_SUB,   F_SUBB,   TYPE_REG|TYPE_BYTE|TYPE_BOTH,   TYPE_MODRM|TYPE_BYTE|TYPE_IN},
 /*2B*/ {M_SUB,   F_SUBW,   TYPE_REG|TYPE_WORDD|TYPE_BOTH,  TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*2C*/ {M_SUB,   F_SUBB,   TYPE_AL|TYPE_BOTH,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*2D*/ {M_SUB,   F_SUBW,   TYPE_AX|TYPE_BOTH,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*2E*/ {M_CS,    F_CS,     TYPE_NONE|TYPE_OVERRIDE|OVERRIDE_CS},
 /*2F*/ {M_DAS,   F_DAS,    TYPE_NONE},

 /*30*/ {M_XOR,   F_XORB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_REG|TYPE_BYTE|TYPE_IN},
 /*31*/ {M_XOR,   F_XORW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*32*/ {M_XOR,   F_XORB,   TYPE_REG|TYPE_BYTE|TYPE_BOTH,   TYPE_MODRM|TYPE_BYTE|TYPE_IN},
 /*33*/ {M_XOR,   F_XORW,   TYPE_REG|TYPE_WORDD|TYPE_BOTH,  TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*34*/ {M_XOR,   F_XORB,   TYPE_AL|TYPE_BOTH,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*35*/ {M_XOR,   F_XORW,   TYPE_AX|TYPE_BOTH,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*36*/ {M_SS,    F_SS,     TYPE_NONE|TYPE_OVERRIDE|OVERRIDE_SS},
 /*37*/ {M_AAA,   F_AAA,    TYPE_NONE},

 /*38*/ {M_CMP,   F_CMPB,   TYPE_MODRM|TYPE_BYTE|TYPE_IN,   TYPE_REG|TYPE_BYTE|TYPE_IN},
 /*39*/ {M_CMP,   F_CMPW,   TYPE_MODRM|TYPE_WORDD|TYPE_IN,  TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*3A*/ {M_CMP,   F_CMPB,   TYPE_REG|TYPE_BYTE|TYPE_IN,     TYPE_MODRM|TYPE_BYTE|TYPE_IN},
 /*3B*/ {M_CMP,   F_CMPW,   TYPE_REG|TYPE_WORDD|TYPE_IN,    TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*3C*/ {M_CMP,   F_CMPB,   TYPE_AL|TYPE_IN,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*3D*/ {M_CMP,   F_CMPW,   TYPE_AX|TYPE_IN,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*3E*/ {M_DS,    F_DS,     TYPE_NONE|TYPE_OVERRIDE|OVERRIDE_DS},
 /*3F*/ {M_AAS,   F_AAS,    TYPE_NONE},

 /*40*/ {M_INC,   F_INCW,   TYPE_AX|TYPE_BOTH,		    TYPE_NONE},
 /*41*/ {M_INC,   F_INCW,   TYPE_CX|TYPE_BOTH,		    TYPE_NONE},
 /*42*/ {M_INC,   F_INCW,   TYPE_DX|TYPE_BOTH,		    TYPE_NONE},
 /*43*/ {M_INC,   F_INCW,   TYPE_BX|TYPE_BOTH,		    TYPE_NONE},
 /*44*/ {M_INC,   F_INCW,   TYPE_SP|TYPE_BOTH,		    TYPE_NONE},
 /*45*/ {M_INC,   F_INCW,   TYPE_BP|TYPE_BOTH,		    TYPE_NONE},
 /*46*/ {M_INC,   F_INCW,   TYPE_SI|TYPE_BOTH,		    TYPE_NONE},
 /*47*/ {M_INC,   F_INCW,   TYPE_DI|TYPE_BOTH,		    TYPE_NONE},

 /*48*/ {M_DEC,   F_DECW,   TYPE_AX|TYPE_BOTH,		    TYPE_NONE},
 /*49*/ {M_DEC,   F_DECW,   TYPE_CX|TYPE_BOTH,		    TYPE_NONE},
 /*4A*/ {M_DEC,   F_DECW,   TYPE_DX|TYPE_BOTH,		    TYPE_NONE},
 /*4B*/ {M_DEC,   F_DECW,   TYPE_BX|TYPE_BOTH,		    TYPE_NONE},
 /*4C*/ {M_DEC,   F_DECW,   TYPE_SP|TYPE_BOTH,		    TYPE_NONE},
 /*4D*/ {M_DEC,   F_DECW,   TYPE_BP|TYPE_BOTH,		    TYPE_NONE},
 /*4E*/ {M_DEC,   F_DECW,   TYPE_SI|TYPE_BOTH,		    TYPE_NONE},
 /*4F*/ {M_DEC,   F_DECW,   TYPE_DI|TYPE_BOTH,		    TYPE_NONE},

 /*50*/ {M_PUSH,  F_PUSH,   TYPE_AX|TYPE_IN,		    TYPE_NONE},
 /*51*/ {M_PUSH,  F_PUSH,   TYPE_CX|TYPE_IN,		    TYPE_NONE},
 /*52*/ {M_PUSH,  F_PUSH,   TYPE_DX|TYPE_IN,		    TYPE_NONE},
 /*53*/ {M_PUSH,  F_PUSH,   TYPE_BX|TYPE_IN,		    TYPE_NONE},
 /*54*/ {M_PUSH,  F_PUSH,   TYPE_SP|TYPE_IN,		    TYPE_NONE},
 /*55*/ {M_PUSH,  F_PUSH,   TYPE_BP|TYPE_IN,		    TYPE_NONE},
 /*56*/ {M_PUSH,  F_PUSH,   TYPE_SI|TYPE_IN,		    TYPE_NONE},
 /*57*/ {M_PUSH,  F_PUSH,   TYPE_DI|TYPE_IN,		    TYPE_NONE},

 /*58*/ {M_POP,   F_POP,    TYPE_AX|TYPE_OUT,		    TYPE_NONE},
 /*59*/ {M_POP,   F_POP,    TYPE_CX|TYPE_OUT,		    TYPE_NONE},
 /*5A*/ {M_POP,   F_POP,    TYPE_DX|TYPE_OUT,		    TYPE_NONE},
 /*5B*/ {M_POP,   F_POP,    TYPE_BX|TYPE_OUT,		    TYPE_NONE},
 /*5C*/ {M_POP,   F_POP,    TYPE_SP|TYPE_OUT,		    TYPE_NONE},
 /*5D*/ {M_POP,   F_POP,    TYPE_BP|TYPE_OUT,		    TYPE_NONE},
 /*5E*/ {M_POP,   F_POP,    TYPE_SI|TYPE_OUT,		    TYPE_NONE},
 /*5F*/ {M_POP,   F_POP,    TYPE_DI|TYPE_OUT,		    TYPE_NONE},

 /*60*/ {M_PUSHA, F_PUSHA,  TYPE_NONE|TYPE_286},
 /*61*/ {M_POPA,  F_POPA,   TYPE_NONE|TYPE_286},
 /*62*/ {M_BOUND, F_BOUND,  TYPE_REG|TYPE_WORDD|TYPE_IN|TYPE_286,   TYPE_MODMEM|TYPE_WORDD2|TYPE_IN},
 /*63*/ {M_ARPL,  F_ARPL,   TYPE_MODRM|TYPE_WORD|TYPE_BOTH|TYPE_286,TYPE_REG|TYPE_WORD|TYPE_IN},
 /*64*/ {M_FS,    F_FS,     TYPE_NONE|TYPE_OVERRIDE|OVERRIDE_FS|TYPE_386},
 /*65*/ {M_GS,    F_GS,     TYPE_NONE|TYPE_OVERRIDE|OVERRIDE_GS|TYPE_386},
 /*66*/ {M_OS,    F_OS,     TYPE_NONE|TYPE_OVERRIDE|OVERRIDE_OS|TYPE_386},
 /*67*/ {M_AS,    F_AS,     TYPE_NONE|TYPE_OVERRIDE|OVERRIDE_AS|TYPE_386},

 /*68*/ {M_PUSH,  F_PUSH,   TYPE_IMM|TYPE_WORDD|TYPE_IN|TYPE_286,   TYPE_NONE},
 /*69*/ {M_IMUL,  F_IMULW,  TYPE_REG|TYPE_WORDD|TYPE_OUT|TYPE_286,  TYPE_MODRM|TYPE_WORDD|TYPE_IN,  TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*6A*/ {M_PUSH,  F_PUSH,   TYPE_IMM|TYPE_SBYTE|TYPE_IN|TYPE_286,   TYPE_NONE},
 /*6B*/ {M_IMUL,  F_IMULB,  TYPE_REG|TYPE_WORDD|TYPE_OUT|TYPE_286,  TYPE_MODRM|TYPE_WORDD|TYPE_IN,  TYPE_IMM|TYPE_SBYTE|TYPE_IN},
 /*6C*/ {M_INS,   F_INSB,   TYPE_ESDI|TYPE_BYTE|TYPE_OUT|TYPE_286,  TYPE_DX|TYPE_IN},
 /*6D*/ {M_INS,   F_INSW,   TYPE_ESDI|TYPE_WORDD|TYPE_OUT|TYPE_286, TYPE_DX|TYPE_IN},
 /*6E*/ {M_OUTS,  F_OUTSB,  TYPE_DX|TYPE_IN|TYPE_286,		    TYPE_DSSI|TYPE_BYTE|TYPE_IN},
 /*6F*/ {M_OUTS,  F_OUTSW,  TYPE_DX|TYPE_IN|TYPE_286,		    TYPE_DSSI|TYPE_WORDD|TYPE_IN},

 /*70*/ {M_JO,	  F_JO,     TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*71*/ {M_JNO,   F_JNO,    TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*72*/ {M_JB,	  F_JB,     TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*73*/ {M_JNB,   F_JNB,    TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*74*/ {M_JZ,	  F_JZ,     TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*75*/ {M_JNZ,   F_JNZ,    TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*76*/ {M_JBE,   F_JBE,    TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*77*/ {M_JNBE,  F_JNBE,   TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},

 /*78*/ {M_JS,	  F_JS,     TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*79*/ {M_JNS,   F_JNS,    TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*7A*/ {M_JP,	  F_JP,     TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*7B*/ {M_JNP,   F_JNP,    TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*7C*/ {M_JL,	  F_JL,     TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*7D*/ {M_JGE,   F_JGE,    TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*7E*/ {M_JLE,   F_JLE,    TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*7F*/ {M_JG,	  F_JG,     TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},

 /*80*/ {M_NONE,  F_GRP1B,  TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*81*/ {M_NONE,  F_GRP1W,  TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*82*/ {M_NONE,  F_GRP1B,  TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*83*/ {M_NONE,  F_GRP1WB, TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*84*/ {M_TEST,  F_TESTB,  TYPE_MODRM|TYPE_BYTE|TYPE_IN,   TYPE_REG|TYPE_BYTE|TYPE_IN},
 /*85*/ {M_TEST,  F_TESTW,  TYPE_MODRM|TYPE_WORDD|TYPE_IN,  TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*86*/ {M_XCHG,  F_XCHGB,  TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_REG|TYPE_BYTE|TYPE_BOTH},
 /*87*/ {M_XCHG,  F_XCHGW,  TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_BOTH},

 /*88*/ {M_MOV,   F_MOVB,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_REG|TYPE_BYTE|TYPE_IN},
 /*89*/ {M_MOV,   F_MOVW,   TYPE_MODRM|TYPE_WORDD|TYPE_OUT, TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*8A*/ {M_MOV,   F_MOVB,   TYPE_REG|TYPE_BYTE|TYPE_OUT,    TYPE_MODRM|TYPE_BYTE|TYPE_IN},
 /*8B*/ {M_MOV,   F_MOVW,   TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*8C*/ {M_MOV,   F_MOVW,   TYPE_MODRM|TYPE_WORD|TYPE_OUT,  TYPE_SEGREG|TYPE_WORD|TYPE_IN},
 /*8D*/ {M_LEA,   F_LEA,    TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODMEM|TYPE_WORDD},
 /*8E*/ {M_MOV,   F_MOVW,   TYPE_SEGREG|TYPE_WORD|TYPE_OUT, TYPE_MODRM|TYPE_WORD|TYPE_IN},
 /*8F*/ {M_POP,   F_POP,    TYPE_MODRM|TYPE_WORDD|TYPE_OUT, TYPE_NONE},

 /*90*/ {M_NOP,   F_NOP,    TYPE_NONE},
 /*91*/ {M_XCHG,  F_XCHGW,  TYPE_AX|TYPE_BOTH,		    TYPE_CX|TYPE_BOTH},
 /*92*/ {M_XCHG,  F_XCHGW,  TYPE_AX|TYPE_BOTH,		    TYPE_DX|TYPE_BOTH},
 /*93*/ {M_XCHG,  F_XCHGW,  TYPE_AX|TYPE_BOTH,		    TYPE_BX|TYPE_BOTH},
 /*94*/ {M_XCHG,  F_XCHGW,  TYPE_AX|TYPE_BOTH,		    TYPE_SP|TYPE_BOTH},
 /*95*/ {M_XCHG,  F_XCHGW,  TYPE_AX|TYPE_BOTH,		    TYPE_BP|TYPE_BOTH},
 /*96*/ {M_XCHG,  F_XCHGW,  TYPE_AX|TYPE_BOTH,		    TYPE_SI|TYPE_BOTH},
 /*97*/ {M_XCHG,  F_XCHGW,  TYPE_AX|TYPE_BOTH,		    TYPE_DI|TYPE_BOTH},

 /*98*/ {M_CBW,   F_CBW,    TYPE_NONE},
 /*99*/ {M_CWD,   F_CWD,    TYPE_NONE},
 /*9A*/ {M_CALL,  F_CALLF,  TYPE_IMM|TYPE_FARP|TYPE_IN},
 /*9B*/ {M_WAIT,  F_WAIT,   TYPE_NONE},
 /*9C*/ {M_PUSHF, F_PUSHF,  TYPE_NONE},
 /*9D*/ {M_POPF,  F_POPF,   TYPE_NONE},
 /*9E*/ {M_SAHF,  F_SAHF,   TYPE_NONE},
 /*9F*/ {M_LAHF,  F_LAHF,   TYPE_NONE},

 /*A0*/ {M_MOV,   F_MOVB,   TYPE_AL|TYPE_OUT,		    TYPE_IMMOFF|TYPE_BYTE|TYPE_IN},
 /*A1*/ {M_MOV,   F_MOVW,   TYPE_AX|TYPE_OUT,		    TYPE_IMMOFF|TYPE_WORDD|TYPE_IN},
 /*A2*/ {M_MOV,   F_MOVB,   TYPE_IMMOFF|TYPE_BYTE|TYPE_OUT, TYPE_AL|TYPE_IN},
 /*A3*/ {M_MOV,   F_MOVW,   TYPE_IMMOFF|TYPE_WORDD|TYPE_OUT,TYPE_AX|TYPE_IN},
 /*A4*/ {M_MOVSB, F_MOVSB,  TYPE_ESDI|TYPE_BYTE|TYPE_OUT,   TYPE_DSSI|TYPE_BYTE|TYPE_IN},
 /*A5*/ {M_MOVSW, F_MOVSW,  TYPE_ESDI|TYPE_WORDD|TYPE_OUT,  TYPE_DSSI|TYPE_WORDD|TYPE_IN},
 /*A6*/ {M_CMPSB, F_CMPSB,  TYPE_ESDI|TYPE_BYTE|TYPE_IN,    TYPE_DSSI|TYPE_BYTE|TYPE_IN},
 /*A7*/ {M_CMPSW, F_CMPSW,  TYPE_ESDI|TYPE_WORDD|TYPE_IN,   TYPE_DSSI|TYPE_WORDD|TYPE_IN},

 /*A8*/ {M_TEST,  F_TESTB,  TYPE_AL|TYPE_IN,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*A9*/ {M_TEST,  F_TESTW,  TYPE_AX|TYPE_IN,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*AA*/ {M_STOSB, F_STOSB,  TYPE_ESDI|TYPE_BYTE|TYPE_OUT,   TYPE_AL|TYPE_IN},
 /*AB*/ {M_STOSW, F_STOSW,  TYPE_ESDI|TYPE_WORDD|TYPE_OUT,  TYPE_AX|TYPE_IN},
 /*AC*/ {M_LODSB, F_LODSB,  TYPE_AL|TYPE_OUT,		    TYPE_DSSI|TYPE_BYTE|TYPE_IN},
 /*AD*/ {M_LODSW, F_LODSW,  TYPE_AX|TYPE_OUT,		    TYPE_DSSI|TYPE_WORDD|TYPE_IN},
 /*AE*/ {M_SCASB, F_SCASB,  TYPE_AL|TYPE_IN,		    TYPE_ESDI|TYPE_BYTE|TYPE_IN},
 /*AF*/ {M_SCASW, F_SCASW,  TYPE_AX|TYPE_IN,		    TYPE_ESDI|TYPE_WORDD|TYPE_IN},

 /*B0*/ {M_MOV,   F_MOVB,   TYPE_AL|TYPE_OUT,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*B1*/ {M_MOV,   F_MOVB,   TYPE_CL|TYPE_OUT,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*B2*/ {M_MOV,   F_MOVB,   TYPE_DL|TYPE_OUT,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*B3*/ {M_MOV,   F_MOVB,   TYPE_BL|TYPE_OUT,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*B4*/ {M_MOV,   F_MOVB,   TYPE_AH|TYPE_OUT,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*B5*/ {M_MOV,   F_MOVB,   TYPE_CH|TYPE_OUT,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*B6*/ {M_MOV,   F_MOVB,   TYPE_DH|TYPE_OUT,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*B7*/ {M_MOV,   F_MOVB,   TYPE_BH|TYPE_OUT,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},

 /*B8*/ {M_MOV,   F_MOVW,   TYPE_AX|TYPE_OUT,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*B9*/ {M_MOV,   F_MOVW,   TYPE_CX|TYPE_OUT,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*BA*/ {M_MOV,   F_MOVW,   TYPE_DX|TYPE_OUT,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*BB*/ {M_MOV,   F_MOVW,   TYPE_BX|TYPE_OUT,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*BC*/ {M_MOV,   F_MOVW,   TYPE_SP|TYPE_OUT,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*BD*/ {M_MOV,   F_MOVW,   TYPE_BP|TYPE_OUT,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*BE*/ {M_MOV,   F_MOVW,   TYPE_SI|TYPE_OUT,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},
 /*BF*/ {M_MOV,   F_MOVW,   TYPE_DI|TYPE_OUT,		    TYPE_IMM|TYPE_WORDD|TYPE_IN},

 /*C0*/ {M_NONE,  F_GRP2B,  TYPE_MODRM|TYPE_BYTE|TYPE_BOTH|TYPE_286, TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*C1*/ {M_NONE,  F_GRP2W,  TYPE_MODRM|TYPE_WORDD|TYPE_BOTH|TYPE_286,TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*C2*/ {M_RET,   F_RETNV,  TYPE_IMM|TYPE_WORD|TYPE_IN,     TYPE_NONE},
 /*C3*/ {M_RET,   F_RETN,   TYPE_NONE},
 /*C4*/ {M_LES,   F_LES,    TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODMEM|TYPE_FARP|TYPE_IN},
 /*C5*/ {M_LDS,   F_LDS,    TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODMEM|TYPE_FARP|TYPE_IN},
 /*C6*/ {M_MOV,   F_MOVB,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*C7*/ {M_MOV,   F_MOVW,   TYPE_MODRM|TYPE_WORDD|TYPE_OUT, TYPE_IMM|TYPE_WORDD|TYPE_IN},

 /*C8*/ {M_ENTER, F_ENTER,  TYPE_IMM|TYPE_WORD|TYPE_IN|TYPE_286,    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*C9*/ {M_LEAVE, F_LEAVE,  TYPE_NONE|TYPE_286},
 /*CA*/ {M_RETF,  F_RETFV,  TYPE_IMM|TYPE_WORD|TYPE_IN,     TYPE_NONE},
 /*CB*/ {M_RETF,  F_RETF,   TYPE_NONE},
 /*CC*/ {M_INT3,  F_INT3,   TYPE_NONE},
 /*CD*/ {M_INT,   F_INT,    TYPE_IMM|TYPE_BYTE|TYPE_IN,     TYPE_NONE},
 /*CE*/ {M_INTO,  F_INTO,   TYPE_NONE},
 /*CF*/ {M_IRET,  F_IRET,   TYPE_NONE},

 /*D0*/ {M_NONE,  F_GRP2B1, TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_ONE|TYPE_BYTE|TYPE_IN},
 /*D1*/ {M_NONE,  F_GRP2W1, TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_ONE|TYPE_BYTE|TYPE_IN},
 /*D2*/ {M_NONE,  F_GRP2BC, TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_CL|TYPE_IN},
 /*D3*/ {M_NONE,  F_GRP2WC, TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_CL|TYPE_IN},
 /*D4*/ {M_AAM,   F_AAM,    TYPE_NONE},
 /*D5*/ {M_AAD,   F_AAD,    TYPE_NONE},
 /*D6*/ {M_GBP,   F_GBP,    TYPE_NONE},
 /*D7*/ {M_XLAT,  F_XLAT,   TYPE_NONE},

 /*D8*/ {M_ESC,   F_ESC,    TYPE_NONE},
 /*D9*/ {M_ESC,   F_ESC,    TYPE_NONE},
 /*DA*/ {M_ESC,   F_ESC,    TYPE_NONE},
 /*DB*/ {M_ESC,   F_ESC,    TYPE_NONE},
 /*DC*/ {M_ESC,   F_ESC,    TYPE_NONE},
 /*DD*/ {M_ESC,   F_ESC,    TYPE_NONE},
 /*DE*/ {M_ESC,   F_ESC,    TYPE_NONE},
 /*DF*/ {M_ESC,   F_ESC,    TYPE_NONE},

 /*E0*/ {M_LOOPNZ,F_LOOPNZ, TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*E1*/ {M_LOOPZ, F_LOOPZ,  TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*E2*/ {M_LOOP,  F_LOOP,   TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*E3*/ {M_JCXZ,  F_JCXZ,   TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*E4*/ {M_IN,	  F_INB,    TYPE_AL|TYPE_OUT,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*E5*/ {M_IN,	  F_INW,    TYPE_AX|TYPE_OUT,		    TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*E6*/ {M_OUT,   F_OUTB,   TYPE_IMM|TYPE_BYTE|TYPE_IN,     TYPE_AL|TYPE_IN},
 /*E7*/ {M_OUT,   F_OUTW,   TYPE_IMM|TYPE_BYTE|TYPE_IN,     TYPE_AX|TYPE_IN},

 /*E8*/ {M_CALL,  F_CALL,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*E9*/ {M_JMP,   F_JMP,    TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*EA*/ {M_JMP,   F_JMPF,   TYPE_IMM|TYPE_FARP|TYPE_IN,     TYPE_NONE},
 /*EB*/ {M_JMP,   F_JMP,    TYPE_IMMREL|TYPE_BYTE|TYPE_IN,  TYPE_NONE},
 /*EC*/ {M_IN,	  F_INB,    TYPE_AL|TYPE_OUT,		    TYPE_DX|TYPE_IN},
 /*ED*/ {M_IN,	  F_INW,    TYPE_AX|TYPE_OUT,		    TYPE_DX|TYPE_IN},
 /*EE*/ {M_OUT,   F_OUTB,   TYPE_DX|TYPE_IN,		    TYPE_AL|TYPE_IN},
 /*EF*/ {M_OUT,   F_OUTW,   TYPE_DX|TYPE_IN,		    TYPE_AX|TYPE_IN},

 /*F0*/ {M_LOCK,  F_LOCK,   TYPE_NONE|TYPE_OVERRIDE|OVERRIDE_LOCK},
 /*F1*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*F2*/ {M_REPNZ, F_REPNZ,  TYPE_NONE|TYPE_OVERRIDE|OVERRIDE_REPNZ},
 /*F3*/ {M_REPZ,  F_REPZ,   TYPE_NONE|TYPE_OVERRIDE|OVERRIDE_REPZ},
 /*F4*/ {M_HLT,   F_HLT,    TYPE_NONE},
 /*F5*/ {M_CMC,   F_CMC,    TYPE_NONE},
 /*F6*/ {M_NONE,  F_GRP3B,  TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_NONE},
 /*F7*/ {M_NONE,  F_GRP3W,  TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_NONE},

 /*F8*/ {M_CLC,   F_CLC,    TYPE_NONE},
 /*F9*/ {M_STC,   F_STC,    TYPE_NONE},
 /*FA*/ {M_CLI,   F_CLI,    TYPE_NONE},
 /*FB*/ {M_STI,   F_STI,    TYPE_NONE},
 /*FC*/ {M_CLD,   F_CLD,    TYPE_NONE},
 /*FD*/ {M_STD,   F_STD,    TYPE_NONE},
 /*FE*/ {M_NONE,  F_GRP4,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH, TYPE_NONE},
 /*FF*/ {M_NONE,  F_GRP5,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_NONE},
};


ID aid0F[256] = {
 /*00*/ {M_NONE,  F_GRP6,   TYPE_MODRM|TYPE_WORD|TYPE_BOTH, TYPE_NONE},
 /*01*/ {M_NONE,  F_GRP7,   TYPE_MODRM|TYPE_WORD|TYPE_BOTH, TYPE_NONE},
 /*02*/ {M_LAR,   F_NONE,   TYPE_REG  |TYPE_WORDD|TYPE_OUT, TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*03*/ {M_LSL,   F_NONE,   TYPE_REG  |TYPE_WORDD|TYPE_OUT, TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*04*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*05*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*06*/ {M_CLTS,  F_NONE,   TYPE_NONE},
 /*07*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*08*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*09*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*0A*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*0B*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*0C*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*0D*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*0E*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*0F*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*10*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*11*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*12*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*13*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*14*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*15*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*16*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*17*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*18*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*19*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*1A*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*1B*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*1C*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*1D*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*1E*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*1F*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*20*/ {M_MOV,   F_NONE,   TYPE_MODREG|TYPE_DWORD|TYPE_OUT, TYPE_CTLREG|TYPE_DWORD|TYPE_IN},
 /*21*/ {M_MOV,   F_NONE,   TYPE_MODREG|TYPE_DWORD|TYPE_OUT, TYPE_DBGREG|TYPE_DWORD|TYPE_IN},
 /*22*/ {M_MOV,   F_NONE,   TYPE_CTLREG|TYPE_DWORD|TYPE_OUT, TYPE_MODREG|TYPE_DWORD|TYPE_IN},
 /*23*/ {M_MOV,   F_NONE,   TYPE_DBGREG|TYPE_DWORD|TYPE_OUT, TYPE_MODREG|TYPE_DWORD|TYPE_IN},
 /*24*/ {M_MOV,   F_NONE,   TYPE_MODREG|TYPE_DWORD|TYPE_OUT, TYPE_TSTREG|TYPE_DWORD|TYPE_IN},
 /*25*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*26*/ {M_MOV,   F_NONE,   TYPE_TSTREG|TYPE_DWORD|TYPE_OUT, TYPE_MODREG|TYPE_DWORD|TYPE_IN},
 /*27*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*28*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*29*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*2A*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*2B*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*2C*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*2D*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*2E*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*2F*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*30*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*31*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*32*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*33*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*34*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*35*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*36*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*37*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*38*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*39*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*3A*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*3B*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*3C*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*3D*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*3E*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*3F*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*40*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*41*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*42*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*43*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*44*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*45*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*46*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*47*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*48*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*49*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*4A*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*4B*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*4C*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*4D*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*4E*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*4F*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*50*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*51*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*52*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*53*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*54*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*55*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*56*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*57*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*58*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*59*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*5A*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*5B*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*5C*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*5D*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*5E*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*5F*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*60*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*61*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*62*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*63*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*64*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*65*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*66*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*67*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*68*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*69*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*6A*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*6B*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*6C*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*6D*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*6E*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*6F*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*70*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*71*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*72*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*73*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*74*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*75*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*76*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*77*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*78*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*79*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*7A*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*7B*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*7C*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*7D*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*7E*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*7F*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*80*/ {M_JO,    F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*81*/ {M_JNO,   F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*82*/ {M_JB,    F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*83*/ {M_JNB,   F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*84*/ {M_JZ,    F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*85*/ {M_JNZ,   F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*86*/ {M_JBE,   F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*87*/ {M_JNBE,  F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},

 /*88*/ {M_JS,    F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*89*/ {M_JNS,   F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*8A*/ {M_JP,    F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*8B*/ {M_JNP,   F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*8C*/ {M_JL,    F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*8D*/ {M_JGE,   F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*8E*/ {M_JLE,   F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},
 /*8F*/ {M_JG,    F_NONE,   TYPE_IMMREL|TYPE_WORDD|TYPE_IN, TYPE_NONE},

 /*90*/ {M_SETO,  F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*91*/ {M_SETNO, F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*92*/ {M_SETB,  F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*93*/ {M_SETNB, F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*94*/ {M_SETZ,  F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*95*/ {M_SETNZ, F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*96*/ {M_SETBE, F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*97*/ {M_SETNBE,F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},

 /*98*/ {M_SETS,  F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*99*/ {M_SETNS, F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*9A*/ {M_SETP,  F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*9B*/ {M_SETNP, F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*9C*/ {M_SETL,  F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*9D*/ {M_SETGE, F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*9E*/ {M_SETLE, F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},
 /*9F*/ {M_SETG,  F_NONE,   TYPE_MODRM|TYPE_BYTE|TYPE_OUT,  TYPE_NONE},

 /*A0*/ {M_PUSH,  F_NONE,   TYPE_FS|TYPE_IN,    TYPE_NONE},
 /*A1*/ {M_POP,   F_NONE,   TYPE_FS|TYPE_OUT,   TYPE_NONE},
 /*A2*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*A3*/ {M_BT,    F_NONE,   TYPE_MODRM|TYPE_WORDD|TYPE_IN,  TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*A4*/ {M_SHLD,  F_NONE,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN,   TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*A5*/ {M_SHLD,  F_NONE,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN,   TYPE_CL|TYPE_IN},
 /*A6*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*A7*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*A8*/ {M_PUSH,  F_NONE,   TYPE_GS|TYPE_IN,    TYPE_NONE},
 /*A9*/ {M_POP,   F_NONE,   TYPE_GS|TYPE_OUT,   TYPE_NONE},
 /*AA*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*AB*/ {M_BTS,   F_NONE,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*AC*/ {M_SHRD,  F_NONE,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN,   TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*AD*/ {M_SHRD,  F_NONE,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN,   TYPE_CL|TYPE_IN},
 /*AE*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*AF*/ {M_IMUL,  F_NONE,   TYPE_REG|TYPE_WORDD|TYPE_BOTH,  TYPE_MODRM|TYPE_WORDD|TYPE_IN},

 /*B0*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*B1*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*B2*/ {M_LSS,   F_NONE,   TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODMEM|TYPE_FARP|TYPE_IN},
 /*B3*/ {M_BTR,   F_NONE,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*B4*/ {M_LFS,   F_NONE,   TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODMEM|TYPE_FARP|TYPE_IN},
 /*B5*/ {M_LGS,   F_NONE,   TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODMEM|TYPE_FARP|TYPE_IN},
 /*B6*/ {M_MOVZX, F_NONE,   TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODRM|TYPE_BYTE|TYPE_IN},
 /*B7*/ {M_MOVZX, F_NONE,   TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODRM|TYPE_WORD|TYPE_IN},

 /*B8*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*B9*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*BA*/ {M_NONE,  F_GRP8,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_IMM|TYPE_BYTE|TYPE_IN},
 /*BB*/ {M_BTC,   F_NONE,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,TYPE_REG|TYPE_WORDD|TYPE_IN},
 /*BC*/ {M_BSF,   F_NONE,   TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*BD*/ {M_BSR,   F_NONE,   TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODRM|TYPE_WORDD|TYPE_IN},
 /*BE*/ {M_MOVSX, F_NONE,   TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODRM|TYPE_BYTE|TYPE_IN},
 /*BF*/ {M_MOVSX, F_NONE,   TYPE_REG|TYPE_WORDD|TYPE_OUT,   TYPE_MODRM|TYPE_WORD|TYPE_IN},
#ifdef UNUSED
 /*C0*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*C1*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*C2*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*C3*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*C4*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*C5*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*C6*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*C7*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*C8*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*C9*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*CA*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*CB*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*CC*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*CD*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*CE*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*CF*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*D0*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*D1*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*D2*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*D3*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*D4*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*D5*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*D6*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*D7*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*D8*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*D9*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*DA*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*DB*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*DC*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*DD*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*DE*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*DF*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*E0*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*E1*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*E2*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*E3*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*E4*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*E5*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*E6*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*E7*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*E8*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*E9*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*EA*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*EB*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*EC*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*ED*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*EE*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*EF*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*F0*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*F1*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*F2*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*F3*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*F4*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*F5*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*F6*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*F7*/ {M_NONE,  F_NONE,   TYPE_NONE},

 /*F8*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*F9*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*FA*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*FB*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*FC*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*FD*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*FE*/ {M_NONE,  F_NONE,   TYPE_NONE},
 /*FF*/ {M_NONE,  F_NONE,   TYPE_NONE},
#endif
};


ID aaidGroup[FGTOTAL][8] = {
  {	/*GRP1B*/
    {M_ADD,   F_ADDB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_OR,    F_ORB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_ADC,   F_ADCB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_SBB,   F_SBBB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_AND,   F_ANDB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_SUB,   F_SUBB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_XOR,   F_XORB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_CMP,   F_CMPB,	TYPE_MODRM|TYPE_BYTE|TYPE_IN,	    TYPE_IMM|TYPE_BYTE|TYPE_IN},
  }, {	/*GRP1W*/
    {M_ADD,   F_ADDW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_WORDD|TYPE_IN},
    {M_OR,    F_ORW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_WORDD|TYPE_IN},
    {M_ADC,   F_ADCW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_WORDD|TYPE_IN},
    {M_SBB,   F_SBBW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_WORDD|TYPE_IN},
    {M_AND,   F_ANDW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_WORDD|TYPE_IN},
    {M_SUB,   F_SUBW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_WORDD|TYPE_IN},
    {M_XOR,   F_XORW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_WORDD|TYPE_IN},
    {M_CMP,   F_CMPW,	TYPE_MODRM|TYPE_WORDD|TYPE_IN,	    TYPE_IMM|TYPE_WORDD|TYPE_IN},
  }, {	/*GRP1WB*/
    {M_ADD,   F_ADDW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_SBYTE|TYPE_IN},
    {M_OR,    F_ORW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_SBYTE|TYPE_IN},
    {M_ADC,   F_ADCW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_SBYTE|TYPE_IN},
    {M_SBB,   F_SBBW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_SBYTE|TYPE_IN},
    {M_AND,   F_ANDW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_SBYTE|TYPE_IN},
    {M_SUB,   F_SUBW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_SBYTE|TYPE_IN},
    {M_XOR,   F_XORW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_SBYTE|TYPE_IN},
    {M_CMP,   F_CMPW,	TYPE_MODRM|TYPE_WORDD|TYPE_IN,	    TYPE_IMM|TYPE_SBYTE|TYPE_IN},
  }, {	/*GRP2B*/
    {M_ROL,   F_ROLB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH|TYPE_286,    TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_ROR,   F_RORB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH|TYPE_286,    TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_RCL,   F_RCLB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH|TYPE_286,    TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_RCR,   F_RCRB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH|TYPE_286,    TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_SHL,   F_SHLB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH|TYPE_286,    TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_SHR,   F_SHRB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH|TYPE_286,    TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_NONE,  F_NONE,	TYPE_NONE},
    {M_SAR,   F_SARB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH|TYPE_286,    TYPE_IMM|TYPE_BYTE|TYPE_IN},
  }, {	/*GRP2W*/
    {M_ROL,   F_ROLW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH|TYPE_286,   TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_ROR,   F_RORW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH|TYPE_286,   TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_RCL,   F_RCLW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH|TYPE_286,   TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_RCR,   F_RCRW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH|TYPE_286,   TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_SHL,   F_SHLW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH|TYPE_286,   TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_SHR,   F_SHRW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH|TYPE_286,   TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_NONE,  F_NONE,	TYPE_NONE},
    {M_SAR,   F_SARW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH|TYPE_286,   TYPE_IMM|TYPE_BYTE|TYPE_IN},
  }, {	/*GRP2B1*/
    {M_ROL,   F_ROLB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_ONE|TYPE_BYTE|TYPE_IN},
    {M_ROR,   F_RORB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_ONE|TYPE_BYTE|TYPE_IN},
    {M_RCL,   F_RCLB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_ONE|TYPE_BYTE|TYPE_IN},
    {M_RCR,   F_RCRB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_ONE|TYPE_BYTE|TYPE_IN},
    {M_SHL,   F_SHLB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_ONE|TYPE_BYTE|TYPE_IN},
    {M_SHR,   F_SHRB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_ONE|TYPE_BYTE|TYPE_IN},
    {M_NONE,  F_NONE,	TYPE_NONE},
    {M_SAR,   F_SARB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_ONE|TYPE_BYTE|TYPE_IN},
  }, {	/*GRP2W1*/
    {M_ROL,   F_ROLW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_ONE|TYPE_BYTE|TYPE_IN},
    {M_ROR,   F_RORW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_ONE|TYPE_BYTE|TYPE_IN},
    {M_RCL,   F_RCLW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_ONE|TYPE_BYTE|TYPE_IN},
    {M_RCR,   F_RCRW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_ONE|TYPE_BYTE|TYPE_IN},
    {M_SHL,   F_SHLW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_ONE|TYPE_BYTE|TYPE_IN},
    {M_SHR,   F_SHRW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_ONE|TYPE_BYTE|TYPE_IN},
    {M_NONE,  F_NONE,	TYPE_NONE},
    {M_SAR,   F_SARW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_ONE|TYPE_BYTE|TYPE_IN},
  }, {	/*GRP2BC*/
    {M_ROL,   F_ROLB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_CL|TYPE_IN},
    {M_ROR,   F_RORB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_CL|TYPE_IN},
    {M_RCL,   F_RCLB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_CL|TYPE_IN},
    {M_RCR,   F_RCRB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_CL|TYPE_IN},
    {M_SHL,   F_SHLB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_CL|TYPE_IN},
    {M_SHR,   F_SHRB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_CL|TYPE_IN},
    {M_NONE,  F_NONE,   TYPE_NONE},
    {M_SAR,   F_SARB,   TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_CL|TYPE_IN},
  }, {  /*GRP2WC*/
    {M_ROL,   F_ROLW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_CL|TYPE_IN},
    {M_ROR,   F_RORW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_CL|TYPE_IN},
    {M_RCL,   F_RCLW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_CL|TYPE_IN},
    {M_RCR,   F_RCRW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_CL|TYPE_IN},
    {M_SHL,   F_SHLW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_CL|TYPE_IN},
    {M_SHR,   F_SHRW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_CL|TYPE_IN},
    {M_NONE,  F_NONE,   TYPE_NONE},
    {M_SAR,   F_SARW,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_CL|TYPE_IN},
  }, {	/*GRP3B*/
    {M_TEST,  F_TESTB,	TYPE_MODRM|TYPE_BYTE|TYPE_IN,	    TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_NONE,  F_NONE,	TYPE_NONE},
    {M_NOT,   F_NOTB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_NONE},
    {M_NEG,   F_NEGBB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_NONE},
    {M_MUL,   F_MULB,	TYPE_MODRM|TYPE_BYTE|TYPE_IN,	    TYPE_NONE},
    {M_IMUL,  F_IMULB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_NONE},
    {M_DIV,   F_DIVB,	TYPE_MODRM|TYPE_BYTE|TYPE_IN,	    TYPE_NONE},
    {M_IDIV,  F_IDIVB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_NONE},
  }, {	/*GRP3W*/
    {M_TEST,  F_TESTW,	TYPE_MODRM|TYPE_WORDD|TYPE_IN,	    TYPE_IMM|TYPE_WORDD|TYPE_IN},
    {M_NONE,  F_NONE,	TYPE_NONE},
    {M_NOT,   F_NOTW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_NONE},
    {M_NEG,   F_NEGBW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_NONE},
    {M_MUL,   F_MULW,	TYPE_MODRM|TYPE_WORDD|TYPE_IN,	    TYPE_NONE},
    {M_IMUL,  F_IMULW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_NONE},
    {M_DIV,   F_DIVW,	TYPE_MODRM|TYPE_WORDD|TYPE_IN,	    TYPE_NONE},
    {M_IDIV,  F_IDIVW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_NONE},
  }, {	/*GRP4*/
    {M_INC,   F_INCB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_NONE},
    {M_DEC,   F_DECB,	TYPE_MODRM|TYPE_BYTE|TYPE_BOTH,     TYPE_NONE},
    {M_NONE,  F_NONE,	TYPE_NONE},
    {M_NONE,  F_NONE,	TYPE_NONE},
    {M_NONE,  F_NONE,	TYPE_NONE},
    {M_NONE,  F_NONE,	TYPE_NONE},
    {M_NONE,  F_NONE,	TYPE_NONE},
    {M_NONE,  F_NONE,	TYPE_NONE},
  }, {	/*GRP5*/
    {M_INC,   F_INCW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_NONE},
    {M_DEC,   F_DECW,	TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_NONE},
    {M_CALL,  F_CALL,	TYPE_MODRM|TYPE_WORDD|TYPE_IN,	    TYPE_NONE},
    {M_CALL,  F_CALLF,  TYPE_MODMEM|TYPE_FARP|TYPE_IN,       TYPE_NONE},
    {M_JMP,   F_JMP,	TYPE_MODRM|TYPE_WORDD|TYPE_IN,	    TYPE_NONE},
    {M_JMP,   F_JMPF,   TYPE_MODMEM|TYPE_FARP|TYPE_IN,       TYPE_NONE},
    {M_PUSH,  F_PUSH,	TYPE_MODRM|TYPE_WORDD|TYPE_IN,	    TYPE_NONE},
    {M_NONE,  F_NONE,	TYPE_NONE},
  }, {  /*GRP6*/
    {M_SLDT,  F_NONE,   TYPE_MODRM|TYPE_WORD|TYPE_OUT,      TYPE_NONE},
    {M_STR,   F_NONE,   TYPE_MODRM|TYPE_WORD|TYPE_OUT,      TYPE_NONE},
    {M_LLDT,  F_NONE,   TYPE_MODRM|TYPE_WORD|TYPE_IN,       TYPE_NONE},
    {M_LTR,   F_NONE,   TYPE_MODRM|TYPE_WORD|TYPE_IN,       TYPE_NONE},
    {M_VERR,  F_NONE,   TYPE_MODRM|TYPE_WORD|TYPE_IN,       TYPE_NONE},
    {M_VERW,  F_NONE,   TYPE_MODRM|TYPE_WORD|TYPE_IN,       TYPE_NONE},
    {M_NONE,  F_NONE,   TYPE_NONE},
    {M_NONE,  F_NONE,	TYPE_NONE},
  }, {  /*GRP7*/
    {M_SGDT,  F_NONE,   TYPE_MODMEM|TYPE_DESC|TYPE_OUT,     TYPE_NONE},
    {M_SIDT,  F_NONE,   TYPE_MODMEM|TYPE_DESC|TYPE_OUT,     TYPE_NONE},
    {M_LGDT,  F_NONE,   TYPE_MODMEM|TYPE_DESC|TYPE_IN,      TYPE_NONE},
    {M_LIDT,  F_NONE,   TYPE_MODMEM|TYPE_DESC|TYPE_IN,      TYPE_NONE},
    {M_SMSW,  F_NONE,   TYPE_MODRM|TYPE_WORD|TYPE_OUT,      TYPE_NONE},
    {M_NONE,  F_NONE,   TYPE_NONE},
    {M_LMSW,  F_NONE,   TYPE_MODRM|TYPE_WORD|TYPE_IN,       TYPE_NONE},
    {M_NONE,  F_NONE,   TYPE_NONE},
  }, {  /*GRP8*/
    {M_NONE,  F_NONE,   TYPE_NONE},
    {M_NONE,  F_NONE,   TYPE_NONE},
    {M_NONE,  F_NONE,   TYPE_NONE},
    {M_NONE,  F_NONE,   TYPE_NONE},
    {M_BT,    F_NONE,   TYPE_MODRM|TYPE_WORDD|TYPE_IN,      TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_BTS,   F_NONE,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_BTR,   F_NONE,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_BYTE|TYPE_IN},
    {M_BTC,   F_NONE,   TYPE_MODRM|TYPE_WORDD|TYPE_BOTH,    TYPE_IMM|TYPE_BYTE|TYPE_IN},
  }
};


BYTE abTypeSize[TYPE_SIZE+1] = {
    0,  // TYPE_NONE    0x0000  //     (all other TYPE fields ignored)
    1,  // TYPE_BYTE    0x0001  // (b) byte, regardless of operand size
    1,  // TYPE_SBYTE   0x0002  //     same as above, but sign-extended
    2,  // TYPE_WORD    0x0003  // (w) word, regardless...
    2,  // TYPE_WORDD   0x0004  // (v) word or double-word, depending...
    4,  // TYPE_WORDD2  0x0005  // (a) two memory operands (BOUND only)
    4,  // TYPE_DWORD   0x0006  // (d) double-word, regardless...
    4,  // TYPE_FARP    0x0007  // (p) 32-bit or 48-bit pointer, depending...
    6,  // TYPE_DESC    0x0008  // (s) 6 byte pseudo-descriptor
};


PSZ apszIDTDesc[] = {
    "Divide",                   // IDT_DIVERROR    0x00
    "Trace",                    // IDT_DEBUG       0x01
    "NMI",                      // IDT_NMI         0x02
    "Breakpoint",               // IDT_BREAKPOINT  0x03
    "Overflow",                 // IDT_OVERFLOW    0x04
    "Bounds",                   // IDT_BOUNDS      0x05
    "Invalid opcode",           // IDT_INVOPCODE   0x06
    "No coprocessor",           // IDT_NOCP        0x07
    "Double fault",             // IDT_DBLFAULT    0x08
    NULL,                       // IDT_RESERVED09  0x09
    "TSS",                      // IDT_TSSFAULT    0x0A
    "Segment",                  // IDT_SEGFAULT    0x0B
    "Stack",                    // IDT_STACKFAULT  0x0C
    "General protection",       // IDT_GPFAULT     0x0D
    "Page",                     // IDT_PAGEFAULT   0x0E
    NULL,                       // IDT_RESERVED0F  0x0F
    "Coprocessor",              // IDT_CPFAULT     0x10
    NULL,                       // IDT_RESERVED11  0x11
    NULL,                       // IDT_RESERVED12  0x12
    NULL,                       // IDT_RESERVED13  0x13
    NULL,                       // IDT_RESERVED14  0x14
    NULL,                       // IDT_RESERVED15  0x15
    NULL,                       // IDT_RESERVED16  0x16
    NULL,                       // IDT_RESERVED17  0x17
    NULL,                       // IDT_RESERVED18  0x18
    NULL,                       // IDT_RESERVED19  0x19
    NULL,                       // IDT_RESERVED1A  0x1A
    NULL,                       // IDT_RESERVED1B  0x1B
    NULL,                       // IDT_RESERVED1C  0x1C
    NULL,                       // IDT_RESERVED1D  0x1D
    NULL,                       // IDT_RESERVED1E  0x1E
    NULL,                       // IDT_RESERVED1F  0x1F
    "Timer",                    // IDT_TMR         0x20
    "Keyboard",                 // IDT_KBD         0x21
    NULL,                       // IDT_SLAVE       0x22
    NULL,                       // IDT_COM2        0x23
    NULL,                       // IDT_COM1        0x24
    NULL,                       // IDT_LPT2        0x25
    NULL,                       // IDT_DISKETTE    0x26
    NULL,                       // IDT_LPT1        0x27
    NULL,                       // IDT_RTC         0x28
    NULL,                       // IDT_IRQ2        0x29
    NULL,                       // IDT_IRQ10       0x2A
    NULL,                       // IDT_IRQ11       0x2B
    NULL,                       // IDT_IRQ12       0x2C
    NULL,                       // IDT_CP          0x2D
    NULL,                       // IDT_DISK        0x2E
    NULL,                       // IDT_IRQ15       0x2F
};


VOID x86Trap(PESF pesf, INT flDebug, INT iNum)
{
    // Start out with the assumption that we're not going to re-execute
    // the instruction that caused this particular trap, but rather that
    // we're going to pass control on to the next guy in the chain

    flTrace &= ~TRACE_REEXEC;

    // Special software interrupt processing here

    if (flDebug & DEBUG_VINT) {
        if (!x86FindBP(pesf, BP_INUSE|BP_ENABLED|BP_INT, 0, iNum))
            return;
    }

    // If we hit an INT3 either via the IDT (IDT_BREAKPOINT) or via
    // instruction emulation (DEBUG_VINT3, as set by EmulateINT3), then
    // set the TRACE_REEXEC flag, because assuming it's my INT3, then
    // I'm going to want to re-execute that address (but of course if
    // I don't, or it's not my INT3, then I'm going to need some means
    // of clearing TRACE_REEXEC, to pass the interrupt on to the next guy)

    if (pesf->iTrap == IDT_BREAKPOINT) {
        pesf->EIP--;
        flTrace |= TRACE_REEXEC;
    }
    if (flDebug & DEBUG_VINT3)
        flTrace |= TRACE_REEXEC;

    if (pesf->iTrap == IDT_DEBUG) {
        if (!(flTrace & TRACE_INS))
            return;             // not *my* trace exception
        flTrace |= TRACE_REEXEC;
    }
    // pbpdTempDisable is used when we want to continue execution from an
    // address that contains a breakpoint; x86SetBP detects that condition
    // when breakpoints are reapplied, and sets the trace flag instead of
    // stuffing an INT3.  When we re-enter as a result of the trace, we call
    // x86SetBP again, which will actually store the INT3 this time because
    // the CS:EIP has changed, and then we'll immediately return back

    if (pbpdTempDisable) {
        x86SetBP(pesf, pbpdTempDisable);
        pbpdTempDisable = NULL;
        if (pesf->iTrap == IDT_DEBUG) {
            pesf->Flags &= ~FLAGS_TF;
            if (!(flTrace & TRACE_INS))
                return;
        }
    }

    // If this isn't a debugger exception, set TRACE_VERBOSE to insure
    // a full debugger dump

    if (!(flTrace & TRACE_REEXEC)) {
        flTrace &= ~TRACE_QUIET;
        flTrace |= TRACE_VERBOSE;
    }

    x86Debug(pesf, flDebug | DEBUG_TRAP);
}


VOID x86LoadFrame(register PESF pesf)
{
    INT i;
    register PDES pdes;

    // Initialize all seg regs

    _asm {
        mov     edx,[pesf]
        and     [edx]ESF.CS,0FFFFh
        mov     eax,[edx].saveDS
        and     eax,0FFFFh
        mov     [edx]ESF.DS,eax
        mov     eax,[edx].saveES
        and     eax,0FFFFh
        mov     [edx]ESF.ES,eax
        mov     ax,fs
        mov     [edx]ESF.FS,eax
        mov     ax,gs
        mov     [edx]ESF.GS,eax
        mov     ax,ss
        mov     [edx]ESF.SS,eax
        mov     eax,cr2
        mov     [edx]ESF.CR2,eax
        mov     eax,cr3
        mov     [edx]ESF.CR3,eax
    }

    // Set the SS:ESP fields from the v86 fields if we were
    // in v86-mode OR we didn't but there was still a ring transition

    pesf->ESP += sizeof(ISF);
    if ((pesf->Flags & FLAGS_V86) || (pesf->CS & SEL_RPL3)) {
        pesf->SS  = pesf->v86SS;
        pesf->ESP = pesf->v86ESP;
    }

    for (i=0; i<ARRAYSIZE(aoffSel); i++) {

        // Set each selector's SEL_V86 bit appropriately

        if (pesf->Flags & FLAGS_V86) {
            *(PDWORD)((INT)pesf+aoffSel[i].offSel) =
            *( PWORD)((INT)pesf+aoffSel[i].offv86Sel) | SEL_V86;
            continue;
        }

        // Set each selector's SEL_PROT32 bit appropriately

        if (pdes = x86SelDesc(*(PDWORD)((INT)pesf+aoffSel[i].offSel)))
            if (pdes->bLimit & SEGLIMIT_BIG)
                *(PDWORD)((INT)pesf+aoffSel[i].offSel) |= SEL_PROT32;
    }
}


VOID x86SaveFrame(register PESF pesf)
{
    if (pesf->Flags & FLAGS_V86) {
        pesf->CS &= ~SEL_V86;
        pesf->v86DS = (WORD)pesf->DS;
        pesf->v86ES = (WORD)pesf->ES;
        pesf->v86FS = (WORD)pesf->FS;
        pesf->v86GS = (WORD)pesf->GS;
        pesf->v86SS = (WORD)pesf->SS;
        pesf->v86ESP = pesf->ESP;
    }
    else {
        _asm {
        mov     edx,[pesf]
        mov     eax,[edx]ESF.DS
        mov     [edx].saveDS,eax
        mov     eax,[edx]ESF.ES
        mov     [edx].saveES,eax
        mov     eax,[edx]ESF.FS
        mov     fs,ax
        mov     eax,[edx]ESF.GS
        mov     gs,ax
    ;
    ;   If I interrupted ring 0, then SS:ESP can't be changed until
    ;   we're completely done with the current frame, and we still have
    ;   to wind back out -- BUGBUG!
    ;
    ;   If I instead interrupted some other ring, then the SS:ESP to
    ;   change is on the stack (in same spot as v86SS:v86ESP);  this is
    ;   indicated by the privilege of the interrupted CS.
    ;
        test    [edx]ESF.CS,SEL_RPL3
        jz      short Ring0             ; we interrupted ring 0, skip it
        mov     eax,[edx]ESF.SS
        mov     [edx]ESF.v86SS,eax
        mov     eax,[edx]ESF.ESP
        mov     [edx]ESF.v86ESP,eax
    Ring0:
        }
    }
#ifdef LATER

    // Logically, this is what I should do, but in actuality, I don't want
    // to cause any weird problems just yet....

    _asm {
        mov     edx,[pesf]
        mov     eax,[edx]ESF.CR2
        mov     cr2,eax
        mov     eax,[edx]ESF.CR3
        mov     cr3,eax
    }
#endif
}


CHAR x86AddrType(DWORD sel, BOOL fPrompt)
{
    if (sel & SEL_V86)
        if (fPrompt)
            return '-';
        else
            return '&';

    if (sel & SEL_PROT32)
        if (fPrompt)
            return '#';
        else
            return '%';

    return '#';
}


PDES x86SelDesc(DWORD sel)
{
    // v86 selectors are always valid to 64k, so return a pointer
    // to a dummy descriptor that says exactly that.

    if (sel & SEL_V86)
        return &desV86;

    // Mask off high garbage bits now

    sel = (WORD)sel;

    if (sel == 0)
        return NULL;

    if (sel & SEL_LDT)
        return NULL;

    if ((WORD)(sel & SEL_MASK) > dtrGDT.wLimit)
        return NULL;

    // Get ptr to descriptor

    return pGDT + (sel >> SEL_SHIFT);
}


BOOL x86IsGate(register PDES pdes)
{
    INT iType;

    if (!(pdes->bType & SEGTYPE_SEG)) {
        iType = pdes->bType & SEGTYPE_GATETYPE;
        if (iType >= SEGTYPE_CALLGATE && iType <= SEGTYPE_286TRAPGATE ||
            iType >= SEGTYPE_386CALLGATE) {

            return TRUE;
        }
    }
    return FALSE;
}


DWORD x86DescBase(register PDES pdes)
{
    if (x86IsGate(pdes))
        return ((PGATE)pdes)->wBase0_15 + (((PGATE)pdes)->wBase16_31 << 16);

    return pdes->wBase0_15 + (pdes->bBase16_23 << 16) + (pdes->bBase24_31 << 24);
}


/* x86DescSize - Return the size of a segment (which is the limit+1)
 *
 */
DWORD x86DescSize(register PDES pdes)
{
    DWORD dwLimit;

    if (!pdes)
        return 0;

    if (x86IsGate(pdes))
        return 0;

    dwLimit = pdes->wLimit + ((pdes->bLimit & 0x0F) << 16);
    if (pdes->bLimit & SEGLIMIT_GRANULAR) {

        // The extra "- 1" at the end is to prevent 0xffffffff from wrapping
        // around to zero and giving the impression of a bad segment;  it's a
        // hack, I know, but the side-effect (not being able to access the
        // last byte of a 4Gig segment) is the lesser of two evils right now...

        dwLimit = (dwLimit * PAGE_SIZE + PAGE_SIZE-1) - 1;
    }
    return dwLimit+1;
}


DWORD x86RegValue(PESF pesf, INT iReg)
{
    switch(ardRegs[iReg].cbReg) {
    case 1:
        return *((PBYTE)pesf + ardRegs[iReg].offReg);
    case 2:             // case 2 and 3 are bundled because
    case 3:             // we don't want selector flag bits in value
        return *(PWORD)((PBYTE)pesf + ardRegs[iReg].offReg);
    case 4:
        return *(PDWORD)((PBYTE)pesf + ardRegs[iReg].offReg);
    }
}


BYTE x86GetByte(DWORD sel, DWORD off)
{
    if (off >= x86DescSize(x86SelDesc(sel)))
        return 0xEE;

    if (sel & SEL_V86) {
        off = ((WORD)sel << 4) + off;
        sel = sel_Flat;
    }

    _asm {
        push    es
        mov     es,word ptr [sel]
        mov     edx,[off]
        movzx   eax,byte ptr es:[edx]   ; return byte in EAX
        pop     es
    }
}


WORD x86GetWord(DWORD sel, DWORD off)
{
    return x86GetByte(sel, off) | (x86GetByte(sel, off+1) << 8);
}


DWORD x86GetDWord(DWORD sel, DWORD off)
{
    return x86GetWord(sel, off) | (x86GetWord(sel, off+2) << 16);
}


INT x86GetBytes(DWORD sel, DWORD off, INT n, register PVOID p)
{
    INT i = 0;

    while (n-- > 0)
        *((PBYTE)p)++ = x86GetByte(sel, off + i++);

    return i;
}


VOID x86SetByte(DWORD sel, DWORD off, BYTE b)
{
    INT TypeSave = -1;
    register PDES pdes;

    pdes = x86SelDesc(sel);
    if (off >= x86DescSize(pdes))
        return;

    if (sel & SEL_V86) {
        off = ((WORD)sel << 4) + off;
        sel = sel_Flat;
    }
    _asm {
        push    es
        mov     es,word ptr [sel]
        mov     edx,[off]
        mov     al,[b]
        mov     byte ptr es:[edx],al    ; set byte in AL
        pop     es
    }
}


VOID x86SetWord(DWORD sel, DWORD off, WORD w)
{
    x86SetByte(sel, off++, (BYTE)w);
    x86SetByte(sel, off,   (BYTE)(w >> 8));
}


VOID x86TrapDump(register PESF pesf, INT flDebug)
{
    INT iTrap;
    PSZ pszTrapType;
    PSZ pszTrapDesc;
    PSZ pszSource;

    if (!(flDebug & DEBUG_TRAP))
        return;

    pszSource = "";
    if (flDebug & DEBUG_VMM)
        pszSource = "VMM ";

    pszTrapType = "exception";
    if (pesf->iTrap >= IDT_TMR)
        pszTrapType = "interrupt";

    pszTrapDesc = "Unknown";
    iTrap = pesf->iTrap;
    if (iTrap >= IDT_MASTER_BASE)
        iTrap -= IDT_OFFSET;
    if (iTrap < ARRAYSIZE(apszIDTDesc) && apszIDTDesc[iTrap])
        pszTrapDesc = apszIDTDesc[iTrap];

    printf("%s%s %s (%X)", pszSource, pszTrapDesc, pszTrapType, pesf->iTrap);

    if (pesf->iTrap >= IDT_TSSFAULT && pesf->iTrap <= IDT_PAGEFAULT)
        printf(" - %08x", pesf->iErrCode);

    printf("\n");
}


VOID x86RegDump(register PESF pesf)
{
    static PSZ apszFlags[] = {"nv", "OV",
                              "up", "DN",
                              "di", "EI",
                              "pl", "NG",
                              "nz", "ZR",
                              "na", "AC",
                              "po", "PE",
                              "nc", "CY"};

    printf("eax=%08x ebx=%08x ecx=%08x edx=%08x cr2=%08x  mm=%02x sm=%02x\n",
            pesf->EAX, pesf->EBX, pesf->ECX, pesf->EDX, pesf->CR2,
            pesf->dwIMRs & 0xFF, (pesf->dwIMRs >> 8) & 0xFF);

  //printf("eax=%08x ebx=%08x ecx=%08x edx=%08x cr2=%08x cr3=%08x\n",
  //        pesf->EAX, pesf->EBX, pesf->ECX, pesf->EDX, pesf->CR2, pesf->CR3);

    printf("ebp=%08x esp=%08x esi=%08x edi=%08x  ",
            pesf->EBP, pesf->ESP, pesf->ESI, pesf->EDI);

    printf("%s %s %s %s %s %s %s %s\n",
            apszFlags[IS_OVERFLOW(pesf->Flags)],
            apszFlags[IS_DIRECTION(pesf->Flags)+2],
            apszFlags[IS_INTERRUPT(pesf->Flags)+4],
            apszFlags[IS_SIGN(pesf->Flags)+6],
            apszFlags[IS_ZERO(pesf->Flags)+8],
            apszFlags[IS_AUXCARRY(pesf->Flags)+10],
            apszFlags[IS_PARITY(pesf->Flags)+12],
            apszFlags[IS_CARRY(pesf->Flags)+14]);

    printf("eip=%08x cs=%04x ss=%04x  ds=%04x es=%04x  fs=%04x gs=%04x  flgs=%08x\n",
            pesf->EIP,
            (DWORD)(WORD)pesf->CS,
            (DWORD)(WORD)pesf->SS,
            (DWORD)(WORD)pesf->DS,
            (DWORD)(WORD)pesf->ES,
            (DWORD)(WORD)pesf->FS,
            (DWORD)(WORD)pesf->GS, pesf->Flags);
}


INT x86InsDump(register PESF pesf, DWORD selCode, DWORD off)
{
    INT i, cb, cbPrint, cbAdj;
    CHAR szIns[MAXSZ], szData[MAXSZ];

    cb = x86Decode(pesf, selCode, off, szIns, szData, NULL, NULL);

    cbAdj = 0;
    if ((WORD)selCode == (WORD)sel_Text) {
        cbAdj = 4;
        cbPrint = printf("%%%08x ", off);
    } else
        cbPrint = printf("%04x:%08x ", (DWORD)(WORD)selCode, off);

    // Note the following loop also increments "off"

    for (i=0; i<cb && cbPrint<28-cbAdj; i++,cbPrint+=2)
        printf("%02x", (DWORD)x86GetByte(selCode, off++));

    while (cbPrint++ < 30-cbAdj)
        printf(" ");

    cbPrint += printf(szIns);

    while (cbPrint++ < 60-cbAdj)
        printf(" ");

    printf("%s\n", szData);

    // Remember the last code address dumped, for shorthand convenience

    selCodeLast = selCode;
    offCodeLast = off;

    return cb;
}


BOOL x86DescDump(register PDES pdesTbl, PDTR pdtr, PSZ pszTbl, PCHAR pchCmd, DWORD sel)
{
    // If there were no real arguments, then just provide a summary

    if (!*pchCmd) {
        sel = 0;
        flKeyEvent &= ~KEYEVENT_ABORT;
        while (x86DescDump(pdesTbl, pdtr, pszTbl, " ", sel++ << SEL_SHIFT))
            if (flKeyEvent & KEYEVENT_ABORT)
                break;
        return TRUE;
    }

    if ((WORD)(sel & SEL_MASK) > pdtr->wLimit)
        return FALSE;

    pdesTbl += (sel >> SEL_SHIFT);
    if (x86IsGate(pdesTbl)) {
        printf("%s %04x: sel:offset=%04x:%08x\n",
                pszTbl, sel >> SEL_SHIFT,
                ((PGATE)pdesTbl)->wSel, x86DescBase(pdesTbl));
    } else {
        printf("%s %04x: base=%08x limit=%08x (%s)\n",
                pszTbl, sel,
                x86DescBase(pdesTbl), x86DescSize(pdesTbl)-1,
                (pdesTbl->bLimit & SEGLIMIT_BIG)? "32-bit" : "16-bit");
    }
    return TRUE;
}


VOID x86MemDump(DWORD sel, DWORD off, INT fSize, INT cLines)
{
    INT i, j;
    BYTE abTmp[17];

    flKeyEvent &= ~KEYEVENT_ABORT;
    for (i=0; i<cLines; i++) {
        x86GetBytes(sel, off, 16, abTmp);
        printf("%04x:%08x ", (DWORD)(WORD)sel, off);
        if (fSize == 4) {
	    for (j=0; j<16; j+=8) {
                printf("  %c %c%02x%02x%02x%02x     %02x%02x%02x%02x",
                       j==8?'-':BS,
                       j==8?' ':BS,
                       abTmp[j+3], abTmp[j+2], abTmp[j+1], abTmp[j+0],
                       abTmp[j+7], abTmp[j+6], abTmp[j+5], abTmp[j+4]);
	    }
	}
        else if (fSize == 2) {
	    for (j=0; j<16; j+=8) {
                printf(" %c %02x%02x  %02x%02x  %02x%02x  %02x%02x",
                       j==8?'-':BS,
		       abTmp[j+1], abTmp[j+0], abTmp[j+3], abTmp[j+2],
		       abTmp[j+5], abTmp[j+4], abTmp[j+7], abTmp[j+6]);
	    }
	}
	else {
	    for (j=0; j<16; j+=8) {
		printf("%c%02x %02x %02x %02x %02x %02x %02x %02x",
		       j==8?'-':' ',
		       abTmp[j+0], abTmp[j+1], abTmp[j+2], abTmp[j+3],
		       abTmp[j+4], abTmp[j+5], abTmp[j+6], abTmp[j+7]);
	    }
	}
	for (j=0; j<16; j++) {
	    if (abTmp[j] < ' ' || abTmp[j] >= 127)
		abTmp[j] = '.';
	}
	abTmp[16] = 0;
        printf(" %s\n", abTmp);
        off += 16;
        if (flKeyEvent & KEYEVENT_ABORT)
            break;
    }

    // Remember the last data address dumped, for shorthand convenience

    selDataLast = sel;
    offDataLast = off;
}


VOID x86StackDump(PESF pesf, DWORD sel, DWORD off, INT cLines)
{
    INT i;
    DWORD dw;
    BOOL fSymbol;
    CHAR szSymbol[MAX_SYMLENGTH];

    flKeyEvent &= ~KEYEVENT_ABORT;
    for (i=0; i<cLines; i++) {
        dw = x86GetDWord(sel, off);
        printf("%04x:%08x  %08x  ", (DWORD)(WORD)sel, off, dw);
        SymAddress(szSymbol, pesf->CS, dw, 16, 8, &fSymbol);
        if (fSymbol)
            printf("%s", szSymbol);
        printf("\n");
        off += 4;
        if (flKeyEvent & KEYEVENT_ABORT)
            break;
    }

    // Remember the last data address dumped, for shorthand convenience

    selDataLast = sel;
    offDataLast = off;
}


INT x86Decode(PESF pesf, DWORD selCode, DWORD off,
              register PSZ pszIns, PSZ pszData, PBYTE pbOp, PBYTE pbModRM)
{
    BYTE bOp, bOp2, bModRM;
    INT cIns, cData, i, iFun;
    BOOL fModRM;
    register PID pid;
    PWORD pwType;
    INT iSegPrefix = -1;
    PDWORD pdw;
    DWORD dw, offCur, offSave;

    cIns = 0;                   // count # of chars in instruction part
    offCur = off;               // offCur - off will tell us how big it is

    pesf->defOverride = 0;
    pesf->defData = pesf->DS;
    pesf->defStack = pesf->SS;

  Refetch:
    bOp = x86GetByte(selCode, offCur);
    offCur++;
    bOp2 = x86GetByte(selCode, offCur);
    pid = aidBase + bOp;

    if (bOp == OP_0F) {
        offCur++;
        bOp = bOp2;

        // To save space, the OP_0F table is less than full size;  if
        // the 2nd byte of the 0Fh opcode is too large, then set it to another
        // invalid opcode that isn't too large, and the world will be happy

        if (bOp >= ARRAYSIZE(aid0F))
            bOp = OP_0F;        // BUGBUG -- Assumes 0F0Fh is invalid opcode
        pid = aid0F + bOp;
    }
    iFun = pid->bFunction;

    fModRM = FALSE;
    if (iFun >= FGSTART) {
	fModRM++;
        bModRM = x86GetByte(selCode, offCur++);
        i = REG(bModRM);
        pid = &aaidGroup[iFun-FGSTART][i];
        iFun = pid->bFunction;
    }

    if ((pid->wTypeDst & (TYPE_SIZE|TYPE_OVERRIDE)) == (TYPE_NONE|TYPE_OVERRIDE)) {

        // The main function of defOverride is to record operand and
        // address size overrides, but as a matter of consequence it records
        // all override prefixes encountered on this instruction

        pesf->defOverride |= (pid->wTypeDst & OVERRIDE_MASK);

        switch(bOp) {
        case OP_ES:
            iSegPrefix = REG_ES;
            pesf->defData = pesf->defStack = pesf->ES;
            break;
        case OP_CS:
            iSegPrefix = REG_CS;
            pesf->defData = pesf->defStack = pesf->CS;
            break;
        case OP_SS:
            iSegPrefix = REG_SS;
            pesf->defData = pesf->defStack = pesf->SS;
            break;
        case OP_DS:
            iSegPrefix = REG_DS;
            pesf->defData = pesf->defStack = pesf->DS;
            break;
        case OP_FS:
            iSegPrefix = REG_FS;
            pesf->defData = pesf->defStack = pesf->FS;
            break;
        case OP_GS:
            iSegPrefix = REG_GS;
            pesf->defData = pesf->defStack = pesf->GS;
            break;
        case OP_AS:
        case OP_OS:
            break;              // nothing more need be done for these
        default:
            if (pszIns) {
                i = nstrcpy(pszIns, apszMnemonic[pid->bMnemonic]);
                pszIns += i;
                *pszIns++ = ' ';
                *pszIns = 0;
                cIns += ++i;
            }
            break;
        }
        goto Refetch;
    }
    if (pszIns) {
        i = pid->bMnemonic;
        if (pid->bMnemonic == M_JCXZ)
            if (!(selCode & SEL_PROT32) == !!(pesf->defOverride & OVERRIDE_OS))
                i = M_JECXZ;
        i = nstrcpy(pszIns, apszMnemonic[i]);
        pszIns += i;
        cIns += i;
        pszIns += nstrcpyn(pszIns, "      ", max(8-cIns,1));
    }
    if (pszData)
        *pszData = 0;

    if (iFun == F_AAM || iFun == F_AAD) {
        offCur++;               // BUGBUG - skip extra opcode byte (0Ah)
    }                           // (not encoded as an operand to keep it hidden)

    if ((pid->wTypeDst & TYPE_TYPE) >= TYPE_MODRM ||
        (pid->wTypeSrc & TYPE_TYPE) >= TYPE_MODRM) {
	if (!fModRM++)
            bModRM = x86GetByte(selCode, offCur++);
    }

    cIns = cData = 0;
    pwType = &pid->wTypeDst;

    while (pwType <= &pid->wTypeSrc2) {

        WORD wType = *pwType++;

        if (wType & TYPE_SIZE) {
            if (pszIns && cIns) {
                *pszIns++ = ',';
                *pszIns = 0;
            }
            offSave = offCur;
            if (!x86DecodeOperand(wType, bModRM, &iSegPrefix,
                                  pesf, selCode, &offCur, pszIns)) {
                goto Exit;
            }
            if (pszIns)
                pszIns += nstrlen(pszIns);
            if (pszData) {
                pdw = &dw;
                if (x86CopyOperand(wType, bModRM, pesf, selCode, offSave, &i, &pdw) == 1) {
                    if (!cData)
                        *pszData++ = '(';
                    else
                        *pszData++ = ',';
                    switch(i) {
                    case 1:
                        dw = (DWORD)*(PBYTE)pdw;
                        if ((wType & TYPE_TYPE) == TYPE_IMM) {
                            *pszData++ = '\'';
                            *pszData++ = (CHAR)dw;
                            *pszData++ = '\'';
                            goto FinishOp;
                        }
                        break;
                    case 2:
                        dw = (DWORD)*(PWORD)pdw;
                        break;
                    default:
                        // This may seem redundant since we initialized pdw to
                        // &dw in the first place, but x86CopyOperand has the option
                        // of changing where pdw points (eg, to a reg in the ESF)
                        dw = *pdw;
                        break;
                    }
                    pszData += dwtosz(pszData, dw, -16, i*2);
                  FinishOp:
                    *pszData = ')';
                    *(pszData+1) = 0;
                    cData++;    // increment visual operand count
                }
            }
            cIns++;
        }
    }

#ifdef LATER
    if (pszIns) {

        // The problem with this code is that isn't ready to handle 32-bit
        // code segments yet....

        if (iFun == F_RETN || iFun == F_RETNV) {
            *pszIns++ = '(';
            pszIns += dwtosz(pszIns, x86GetWord(pesf->SS, pesf->ESP), -16, 4);
            *pszIns++ = ')';
            *pszIns = 0;
        }
        if (iFun == F_IRET || iFun == F_RETF || iFun == F_RETFV) {
            *pszIns++ = '(';
            pszIns += dwtosz(pszIns, x86GetDWord(pesf->SS, pesf->ESP), -16, 8);
            *pszIns++ = ')';
            *pszIns = 0;
        }

        // And the problem with this code is that it's part of the emulator....

        if (iFun == F_GBP) {
            PSZ pszOp;
            if (pszOp = GuestQueryGBP(selCode, off)) {
                *pszIns += '(';
                pszIns += nstrcpy(pszIns, pszOp);
                *pszIns++ = ')';
                *pszIns = 0;
            }
        }
    }
#endif

  Exit:
    if (pbOp)
        *pbOp = bOp;
    if (pbModRM)
        *pbModRM = bModRM;
    return offCur-off;
}


BOOL x86DecodeOperand(WORD wType, BYTE bModRM, PINT piSegPrefix, PESF pesf,
                      DWORD selCode, register PDWORD poffCur, register PSZ pszIns)
{
    DWORD dw[2];
    INT i, m, n, a, r, s, sib, w;

    i = wType & TYPE_SIZE;
    if (!(n = abTypeSize[i]))
        goto Error;             // unexpected type in table

    w = wType & TYPE_TYPE;
    if (!(selCode & SEL_PROT32) == !!(pesf->defOverride & OVERRIDE_OS)) {
        if (i == TYPE_WORDD)
            n *= 2;             // operand size is 4 bytes
        if (i == TYPE_FARP) {
            n = 6;              // operand size is 6 bytes (seg16:offset32)
            ASSERT(w == TYPE_IMM || w == TYPE_MODMEM);
        }
    }
    a = 2;
    if (!(selCode & SEL_PROT32) == !!(pesf->defOverride & OVERRIDE_AS))
        a = 4;                  // address size is 4 bytes

    if (w >= TYPE_REG)
        r = REG(bModRM);        // we'll be needing this for all TYPE_REG*

    dw[0] = dw[1] = 0;
    switch(w) {

    case TYPE_IMM:
        if (pszIns) {
            x86GetBytes(selCode, *poffCur, n, dw);
            if (i == TYPE_SBYTE)
                *pszIns++ = '+';
            if (n == 6) {
                pszIns +=
                dwtosz(pszIns, dw[1], -16, 4);
                *pszIns++ = ':';
                SymAddress(pszIns, pesf->defData, dw[0], -16, 8, NULL);
            }
            else
                SymAddress(pszIns, pesf->defData, dw[0], -16, n*2+(i==TYPE_FARP), NULL);
        }
        *poffCur += n;
	break;

    case TYPE_ONE:
        if (pszIns) {
            *pszIns++ = '1';
            *pszIns = 0;
        }
	break;

    case TYPE_IMMOFF:
        if (pszIns) {
            x86GetBytes(selCode, *poffCur, a, dw);
            if (*piSegPrefix >= 0) {
                pszIns += nstrcpy(pszIns, ardRegs[*piSegPrefix+RD_SEG].pszReg);
                *pszIns++ = ':';
                *piSegPrefix = -1;
            }
            *pszIns++ = '[';
            pszIns += SymAddress(pszIns, pesf->defData, dw[0], -16, a*2, NULL);
            *pszIns++ = ']';
            *pszIns = 0;
        }
        *poffCur += a;
	break;

    case TYPE_IMMREL:
        if (pszIns)
            x86GetBytes(selCode, *poffCur, n, dw);
        *poffCur += n;
        if (pszIns) {
            if (n == 1)
                dw[0] = SXBYTE(dw[0]);
            if (a == 4)
                dw[0] += *poffCur;
            else
                dw[0] = (WORD)(*poffCur + dw[0]);
            if (n == 1)
                dwtosz(pszIns, dw[0], -16, a*2);
            else
                SymAddress(pszIns, pesf->defData, dw[0], -16, a*2, NULL);
        }
	break;

    case TYPE_DSSI:
        if (pszIns) {
            if (*piSegPrefix >= 0) {
                pszIns += nstrcpy(pszIns, ardRegs[*piSegPrefix+RD_SEG].pszReg);
                *pszIns++ = ':';
                *piSegPrefix = -1;
            }
            if (a == 2)
                nstrcpy(pszIns, "[si]");
            else
                nstrcpy(pszIns, "[esi]");
        }
	break;

    case TYPE_ESDI:
        if (pszIns) {
            if (a == 2)
                nstrcpy(pszIns, "es:[di]");
            else
                nstrcpy(pszIns, "es:[edi]");
        }
	break;

    case TYPE_IMPREG:
        if (pszIns) {
            i = SHIFTRIGHT(wType,TYPE_IREG) + (n==4)*(RD_DWORD-RD_WORD);
            nstrcpy(pszIns, ardRegs[i].pszReg);
        }
	break;

    case TYPE_IMPSEG:
        if (pszIns) {
            i = SHIFTRIGHT(wType,TYPE_IREG);
            nstrcpy(pszIns, ardRegs[i+RD_SEG].pszReg);
        }
	break;

    case TYPE_MODMEM:
	if (MOD(bModRM) == MOD_REGISTER)
	    goto Error;
        // Goto MODRM after enforcing REG restriction
        goto ModRM;

    case TYPE_MODREG:
        if (MOD(bModRM) != MOD_REGISTER)
	    goto Error;
        // Fall into MODRM after enforcing REG restriction

    case TYPE_MODRM:
      ModRM:
	i = RM(bModRM);
	m = MOD(bModRM);
	if (m == MOD_REGISTER) {
            if (pszIns)
                pszIns += nstrcpy(pszIns, ardRegs[i+(n-1)*8].pszReg);
	    break;
	}
        if (pszIns) {
            if (*piSegPrefix >= 0) {
                pszIns += nstrcpy(pszIns, ardRegs[*piSegPrefix+RD_SEG].pszReg);
                *pszIns++ = ':';
                *piSegPrefix = -1;
            }
            *pszIns++ = '[';
            *pszIns = 0;
        }
        if (a == 2) {
            if (m == MOD_NODISP && i == RM_BP) {
                m = 2;
            } else if (pszIns) {
                pszIns += nstrcpy(pszIns, apszRM[i]);
                if (m > 0)
                    *pszIns++ = '+';
            }
        }
        else {
            if (i == RM_SIB) {
                sib = x86GetByte(selCode, *poffCur);
                (*poffCur)++;

                i = INDX(sib);

                // i == RM_SIB means no index register, in which case
                // the scale should also be 0, or else the effective addr
                // is undefined!  The only reason for i == RM_SIB is to
                // provide an encoding for [esp], [esp+d8], and [esp+d32],
                // which were stolen to provide RM_SIB in the first place.

                if (i != RM_SIB && pszIns) {

                    pszIns += nstrcpy(pszIns, ardRegs[i+RD_DWORD].pszReg);

                    // set scale (s) to either 0, 1, 2 or 3
                    s = SCALE(sib);
                    *pszIns++ = '*';
                    *pszIns++ = achScale[s];
                    *pszIns++ = '+';
                }
                i = BASE(sib);
            }
            if (m == MOD_NODISP && i == RM_EBP) {
                m = MOD_DISP16;
            } else if (pszIns) {
                pszIns += nstrcpy(pszIns, ardRegs[i+RD_DWORD].pszReg);
                if (m > 0)
                    *pszIns++ = '+';
            }
            if (m == 2)         // take care of the jump to double-word now
                m = 4;
        }
	if (m > 0) {
            if (pszIns) {
                x86GetBytes(selCode, *poffCur, m, dw);
                // BUGBUG pesf->defData needs to be set correctly for
                // EBP/ESP addressing;  see CopyOperand for an example
                pszIns += SymAddress(pszIns, pesf->defData, dw[0], -16, m*2, NULL);
            }
            *poffCur += m;
	}
        if (pszIns) {
            *pszIns++ = ']';
            *pszIns = 0;
        }
	break;

    case TYPE_REG:
        if (pszIns)
            nstrcpy(pszIns, ardRegs[r+(n-1)*8].pszReg);
	break;

    case TYPE_SEGREG:
        if (r > REG_GS)
            goto Error;         // not a valid segreg encoding
        if (pszIns)
            nstrcpy(pszIns, ardRegs[r+RD_SEG].pszReg);
	break;

    case TYPE_CTLREG:
        if (r != REG_CR0 && r != REG_CR2 && r != REG_CR3)
            goto Error;         // not a valid ctlreg encoding
        if (pszIns)
            nstrcpy(pszIns, ardRegs[r+RD_CTL].pszReg);
	break;

    case TYPE_DBGREG:
        if (r == 4 || r == 5)
            goto Error;         // not a valid dbgreg encoding
        if (pszIns)
            nstrcpy(pszIns, ardRegs[r+RD_DBG].pszReg);
	break;

    case TYPE_TSTREG:
        if (r != REG_TR6 && r != REG_TR7)
            goto Error;         // not a valid tstreg encoding
        if (pszIns)
            nstrcpy(pszIns, ardRegs[r+RD_TST].pszReg);
	break;

    default:
      Error:
        if (pszIns)
            nstrcpy(pszIns, "???");
	return FALSE;		// unexpected type in table
    }
    return TRUE;
}


BOOL x86CopyOperand(WORD wType, BYTE bModRM, PESF pesf,
                    DWORD selCode, DWORD offCur, PINT pSize, PPDWORD ppdw)
{
    BOOL fSuccess = 1;
    INT i, m, n, a, r, s, sib, w;
    DWORD dwEA, dwTmp;
    register PDWORD pdw = *ppdw;

    i = wType & TYPE_SIZE;
    n = abTypeSize[i];

#ifdef DEBUG
    if (!n)
	return FALSE;		// unsupported type in table
#endif
    w = wType & TYPE_TYPE;
    if (!(selCode & SEL_PROT32) == !!(pesf->defOverride & OVERRIDE_OS)) {
        if (i == TYPE_WORDD)
            n *= 2;             // operand size is 4 bytes
        if (i == TYPE_FARP) {
            n = 6;              // operand size is 6 bytes (seg16:offset32)
            ASSERT(w == TYPE_IMM || w == TYPE_MODMEM);
        }
    }
#ifndef LATER
    if (n == 6)                 // but callers are currently only set up to
        n = 4;                  // to handle a dword of data at most (BUGBUG)
#endif
    a = 2;
    if (!(selCode & SEL_PROT32) == !!(pesf->defOverride & OVERRIDE_AS))
        a = 4;                  // address size is 4 bytes

    if (w >= TYPE_REG)
        r = REG(bModRM);        // we'll be needing this for all TYPE_REG*

    *pdw = 0;
    switch(w) {

    case TYPE_IMM:
        x86GetBytes(selCode, offCur, n, pdw);
	if (i == TYPE_SBYTE)
            *pdw = SXBYTE(*pdw);
        if (n > 1 || !_isascii(*pdw))
            fSuccess++;         // still success, but not very interesting
	break;

    case TYPE_ONE:
        fSuccess++;             // still success, but not very interesting
        *(PBYTE)pdw = 1;
	break;

    case TYPE_IMMOFF:
        x86GetBytes(selCode, offCur, a, &dwEA);
        x86GetBytes(pesf->defData, dwEA, n, pdw);
	break;

    case TYPE_IMMREL:		// this type is always TYPE_IN
        fSuccess++;             // still success, but not very interesting
        x86GetBytes(selCode, offCur, n, pdw);
	if (n == 1)
            *pdw = SXBYTE(*pdw);
        *pdw += offCur + n;
        n = a;                  // return address size for all rel. imm.
	break;

    case TYPE_DSSI:
        if (a == 2)
            dwEA = (WORD)pesf->ESI;
        else
            dwEA = pesf->ESI;
        x86GetBytes(pesf->defData, dwEA, n, pdw);
	break;

    case TYPE_ESDI:
        if (a == 2)
            dwEA = (WORD)pesf->EDI;
        else
            dwEA = pesf->EDI;
        x86GetBytes(pesf->ES, dwEA, n, pdw);
	break;

    case TYPE_IMPREG:
        i = SHIFTRIGHT(wType,TYPE_IREG) + (n==4)*(RD_DWORD-RD_WORD);
        *ppdw = (PDWORD)((DWORD)pesf + ardRegs[i].offReg);
	break;

    case TYPE_IMPSEG:
        i = SHIFTRIGHT(wType,TYPE_IREG);
        *ppdw = (PDWORD)((DWORD)pesf + ardRegs[i+RD_SEG].offReg);
	break;

    case TYPE_MODMEM:
	if (MOD(bModRM) == MOD_REGISTER)
	    return FALSE;
	// Fall into MODRM after enforcing MEM restriction

    case TYPE_MODRM:
	i = RM(bModRM);
	m = MOD(bModRM);
	if (m == MOD_REGISTER) {
            *ppdw = (PDWORD)((DWORD)pesf + ardRegs[i+(n-1)*8].offReg);
	    break;
	}
        if (a == 2) {
            switch(i) {
            case RM_SI:
                dwEA = (WORD)pesf->ESI;
                break;
            case RM_DI:
                dwEA = (WORD)pesf->EDI;
                break;
            case RM_BP:
                if (m == MOD_NODISP) {
                    m = 2;
                    dwEA = 0;
                    break;
                }
                dwEA = (WORD)pesf->EBP;
                pesf->defData = pesf->defStack;
                break;
            case RM_BX:
                dwEA = (WORD)pesf->EBX;
                break;
            case RM_BXSI:
                dwEA = (WORD)((WORD)pesf->EBX+(WORD)pesf->ESI);
                break;
            case RM_BXDI:
                dwEA = (WORD)((WORD)pesf->EBX+(WORD)pesf->EDI);
                break;
            case RM_BPSI:
                dwEA = (WORD)((WORD)pesf->EBP+(WORD)pesf->ESI);
                pesf->defData = pesf->defStack;
                break;
            case RM_BPDI:
                dwEA = (WORD)((WORD)pesf->EBP+(WORD)pesf->EDI);
                pesf->defData = pesf->defStack;
                break;
            }
        } else {
            dwEA = 0;
            if (i == RM_SIB) {
                sib = x86GetByte(selCode, offCur);
                offCur++;

                i = INDX(sib);

                // i == RM_SIB means no index register, in which case
                // the scale should also be 0, or else the effective addr
                // is undefined!  The only reason for i == RM_SIB is to
                // provide an encoding for [esp], [esp+d8], and [esp+d32],
                // which were stolen to provide RM_SIB in the first place.

                if (i != RM_SIB) {

                    // set scale (s) to either 1, 2, 4 or 8
                    s = SCALEMULT(sib);

                    dwEA = *(PDWORD)((DWORD)pesf + ardRegs[i+RD_DWORD].offReg) * (DWORD)s;
                }
                i = BASE(sib);
            }
            switch(i) {
            case RM_EAX:
            case RM_ECX:
            case RM_EDX:
            case RM_EBX:
            case RM_ESI:
            case RM_EDI:
                dwEA += *(PDWORD)((DWORD)pesf + ardRegs[i+RD_DWORD].offReg);
                break;
            case RM_ESP:
                dwEA += pesf->ESP;
                pesf->defData = pesf->defStack;
                break;
            case RM_EBP:
                if (m == MOD_NODISP) {
                    m = MOD_DISP16;
                    break;
                }
                dwEA += pesf->EBP;
                pesf->defData = pesf->defStack;
                break;
            }
        }
        dwTmp = 0;
	switch(m) {
	case MOD_DISP8:
            x86GetBytes(selCode, offCur, 1, &dwTmp);
            dwEA += SXBYTE(dwTmp);
	    break;
	case MOD_DISP16:
            x86GetBytes(selCode, offCur, a, &dwTmp);
            dwEA += dwTmp;
	    break;
	}
        if (a == 2)
            dwEA = (WORD)dwEA;
        if (!(wType & TYPE_BOTH))   // if operand isn't input or output
            *pdw = dwEA;            // then it's LEA, so pretend this is it
        else
            x86GetBytes(pesf->defData, dwEA, n, pdw);
	break;

    case TYPE_REG:
        *ppdw = (PDWORD)((DWORD)pesf + ardRegs[r+(n-1)*8].offReg);
	break;

    case TYPE_SEGREG:
        if (r > REG_GS)
            return FALSE;       // not a valid segreg encoding
        *ppdw = (PDWORD)((DWORD)pesf + ardRegs[r+RD_SEG].offReg);
	break;

    case TYPE_CTLREG:
        if (r != REG_CR0 && r != REG_CR2 && r != REG_CR3)
            return FALSE;       // not a valid ctlreg encoding
        *ppdw = (PDWORD)((DWORD)pesf + ardRegs[r+RD_CTL].offReg);
	break;

    case TYPE_DBGREG:
        if (r == 4 || r == 5)
            return FALSE;       // not a valid dbgreg encoding
        *ppdw = (PDWORD)((DWORD)pesf + ardRegs[r+RD_DBG].offReg);
	break;

    case TYPE_TSTREG:
        if (r != REG_TR6 && r != REG_TR7)
            return FALSE;       // not a valid tstreg encoding
        *ppdw = (PDWORD)((DWORD)pesf + ardRegs[r+RD_TST].offReg);
	break;

    default:
	return FALSE;
    }
    *pSize = n;
    return fSuccess;
}


PBPD x86AllocBP(INT flBP, DWORD sel, DWORD off)
{
    INT i;
    PBPD pbpdCandidate = NULL;
    register PBPD pbpd = abpdGlobal;

    for (i=0; i<ARRAYSIZE(abpdGlobal); i++,pbpd++) {
        if ((pbpd->flBP & BP_INUSE) == 0 && !pbpdCandidate)
            pbpdCandidate = pbpd;
        if ((pbpd->flBP & flBP) == flBP) {
            if (pbpd->selBP == sel && pbpd->offBP == off) {
                pbpdCandidate = pbpd;
                break;
            }
        }
    }
    if (pbpdCandidate) {
        cbpdGlobal++;
        pbpd = pbpdCandidate;
        pbpd->flBP = BP_INUSE | BP_ENABLED | flBP;
        pbpd->selBP = sel;
        pbpd->offBP = off;
        pbpd->iReg = 0;
    }
    else
        printf("Out of breakpoints\n");
    return pbpdCandidate;
}


BOOL x86DisableBP(INT i)
{
    if (i < ARRAYSIZE(abpdGlobal)) {
        if (abpdGlobal[i].flBP & BP_INUSE) {
            if (abpdGlobal[i].flBP & BP_ENABLED) {
                abpdGlobal[i].flBP &= ~BP_ENABLED;
                printf("Breakpoint %d disabled\n", i);
            }
        }
        return TRUE;
    }
    return FALSE;
}


BOOL x86EnableBP(INT i)
{
    if (i < ARRAYSIZE(abpdGlobal)) {
        if (abpdGlobal[i].flBP & BP_INUSE) {
            if (!(abpdGlobal[i].flBP & BP_ENABLED)) {
                abpdGlobal[i].flBP |= BP_ENABLED;
                printf("Breakpoint %d enabled\n", i);
            }
        }
        return TRUE;
    }
    return FALSE;
}


BOOL x86FreeBP(INT i)
{
    if (i < ARRAYSIZE(abpdGlobal)) {
        if (abpdGlobal[i].flBP & BP_INUSE) {
            abpdGlobal[i].flBP &= ~BP_INUSE;
            cbpdGlobal--;
            if (!(abpdGlobal[i].flBP & BP_TEMPORARY))
                printf("Breakpoint %d cleared\n", i);
        }
        return TRUE;
    }
    return FALSE;
}


VOID x86ListBPs(PESF pesf)
{
    INT i;
    register PBPD pbpd = abpdGlobal;

    for (i=0; i<ARRAYSIZE(abpdGlobal); i++,pbpd++) {
        if (pbpd->flBP & BP_INUSE) {
            if (pbpd->flBP & BP_INT)
                printf("%2x:   INT:%08x", i, pbpd->offBP);
            else
                printf("%2x: %c%04x:%08x",
                        i, x86AddrType(pbpd->selBP, FALSE),
                        (DWORD)(WORD)pbpd->selBP, pbpd->offBP);
            if (!(pbpd->flBP & BP_ENABLED))
                printf(" OFF ");
            else if (pbpd->flBP & BP_TEMPORARY)
                printf(" TMP ");
            else
                printf("     ");
            if (pbpd->iReg)
                printf("(%s=%x)", ardRegs[pbpd->iReg-1].pszReg, pbpd->dwValue);
            printf("\n");
        }
    }
}


INT x86FindBP(PESF pesf, INT flBP, DWORD sel, DWORD off)
{
    INT i;
    register PBPD pbpd = abpdGlobal;

    for (i=0; i<ARRAYSIZE(abpdGlobal); i++,pbpd++) {
        if ((pbpd->flBP & flBP) == flBP)
            if (pbpd->selBP == sel && pbpd->offBP == off)
                if (!pbpd->iReg)
                    return i+1;
                else if (x86RegValue(pesf, pbpd->iReg-1) == pbpd->dwValue)
                    return i+1;
    }
    return 0;
}


/*  x86ApplyBPs - insert breakpoint opcodes into memory
 *
 *  ENTRY
 *      pesf -> register frame
 *
 *  EXIT
 *      None
 */

VOID x86ApplyBPs(PESF pesf)
{
    INT i;
    register PBPD pbpd = abpdGlobal;

    for (i=0; i<ARRAYSIZE(abpdGlobal); i++,pbpd++)
        x86SetBP(pesf, pbpd);
}


/*  x86SetBP - insert breakpoint opcode into memory
 *
 *  ENTRY
 *      pesf -> register frame
 *      pbpd -> breakpoint data structure
 *
 *  EXIT
 *      None
 *
 *  NOTES
 *      This is called with interrupts off, so with the exception of NMIs,
 *      we don't need to worry about the casual test-and-set treatment of the
 *      INPLACE bit.
 */

VOID x86SetBP(PESF pesf, register PBPD pbpd)
{
    if ((pbpd->flBP & (BP_INUSE|BP_ENABLED|BP_INPLACE)) == (BP_INUSE|BP_ENABLED)) {
        if (!pbpdTempDisable && pbpd->selBP == pesf->CS && pbpd->offBP == pesf->EIP) {
            pbpdTempDisable = pbpd;
            pesf->Flags |= FLAGS_TF;
        }
        else {
            pbpd->flBP &= ~0xFF;
            pbpd->flBP |= x86GetByte(pbpd->selBP, pbpd->offBP) | BP_INPLACE;
            x86SetByte(pbpd->selBP, pbpd->offBP, OP_INT3);
        }
    }
}


/*  x86RemoveBPs - remove breakpoint opcodes from memory
 *
 *  ENTRY
 *      None
 *
 *  EXIT
 *      None
 *
 *  NOTES
 *      This is called with interrupts off, so with the exception of NMIs,
 *      we don't need to worry about the casual test-and-clear treatment of the
 *      INPLACE bit.  Also note that I don't care about the ENABLED bit,
 *      because there might be occasions where a breakpoint has been internally
 *      disabled outside the debugger, but we still need to do the removal.
 */

VOID x86RemoveBPs(PESF pesf)
{
    INT i;
    register PBPD pbpd = abpdGlobal;

    for (i=0; i<ARRAYSIZE(abpdGlobal); i++,pbpd++) {
        if ((pbpd->flBP & (BP_INUSE|BP_INPLACE)) == (BP_INUSE|BP_INPLACE)) {
            pbpd->flBP &= ~BP_INPLACE;
            if (x86GetByte(pbpd->selBP, pbpd->offBP) == OP_INT3)
                x86SetByte(pbpd->selBP, pbpd->offBP, (BYTE)pbpd->flBP);
            if ((pbpd->flBP & BP_TEMPORARY) && pbpd->selBP == pesf->CS && pbpd->offBP == pesf->EIP)
                x86FreeBP(i);
        }
    }
}


VOID x86PortTrap(PBYTE piopm, WORD wPort)
{
    piopm[wPort/8] |= (1 << (wPort % 8));
}


VOID x86PortUntrap(PBYTE piopm, WORD wPort)
{
    piopm[wPort/8] &= ~(1 << (wPort % 8));
}


BOOL x86PortTrapped(PBYTE piopm, WORD wPort)
{
    // BUGBUG -- Technically, this routine should first
    // check wPort against the size of the IOPM, which is determined
    // by the limit of my TSS selector, and if it exceeds the limit,
    // then TRUE should be returned (since all ports beyond the IOPM's
    // limit default to trapping -- as if the corresponding bit was
    // set).  However, since my TSS is always the maximum size, this
    // issue can be ignored for now.

    return (piopm[wPort/8] & (1 << (wPort % 8))) != 0;
}


VOID x86DispTrapRange(DWORD dwFirst, DWORD dwLast)
{
    PSZ pszMsg;
    static CHAR szTrapMsg[]  = "     %04x trapped\n";
    static CHAR szTrapMsg2[] = "%04x-%04x trapped\n";

    pszMsg = szTrapMsg;
    if (dwFirst != dwLast-1)
        pszMsg = szTrapMsg2;
    printf(pszMsg, dwFirst, dwLast-1);
}


INT x86StepSize(register PESF pesf, DWORD selCode, DWORD off)
{
    INT i, n;
    BYTE bOp, bModRM;
    static BYTE abTransferOp[] = {
        OP_JO, OP_JNO, OP_JB, OP_JNB, OP_JZ, OP_JNZ,
        OP_JBE, OP_JNBE, OP_JS, OP_JNS, OP_JP, OP_JNP,
        OP_JL, OP_JGE ,OP_JLE, OP_JG, OP_RETNV, OP_RETN,
        OP_RETFV, OP_RETF, OP_IRET, OP_JCXZ, OP_JMPN, OP_JMPF, OP_JMPN8
    };

    n = x86Decode(pesf, selCode, off, NULL, NULL, &bOp, &bModRM);

    // I've determined the instruction size;  now determine
    // if it's a "transfer" that I should trace instead of step

    for (i=0; i<ARRAYSIZE(abTransferOp); i++)
	if (abTransferOp[i] == bOp)
	    return 0;

    if (bOp == OP_GRP5 &&
        (REG(bModRM) == OPGRP5_JMPNMEM || REG(bModRM) == OPGRP5_JMPFMEM))
	return 0;

    return n;
}


BOOL x86Trace(register PESF pesf, INT fl)
{
    INT i;
    BYTE bOp = x86GetByte(pesf->CS, pesf->EIP);

    pesf->Flags |= FLAGS_TF;
    flTrace |= TRACE_INS | TRACE_MASKINTS | fl;
    if (bOp == OP_ARPL && (pesf->CS & SEL_V86))
        flTrace &= ~TRACE_MASKINTS;         // can't trace ARPLs
                                            // in v86-mode like that
    if (flTrace & TRACE_MULTIPLE) {
        flKeyEvent &= ~KEYEVENT_ABORT;
    }
    if ((flTrace & TRACE_STEP) || bOp == OP_PUSHF) {
        if (i = x86StepSize(pesf, pesf->CS, pesf->EIP)) {
            pesf->Flags &= ~FLAGS_TF;
            if (bOp != OP_PUSHF && !(flTrace & TRACE_QUICK))
                flTrace &= ~TRACE_MASKINTS; // too risky when "stepping"
            if (!x86AllocBP(BP_TEMPORARY, pesf->CS, pesf->EIP+i)) {
                flTrace &= ~TRACE_FLAGS;
                return FALSE;
            }
        }
    }
    if (flTrace & TRACE_QUIET)
        flTrace &= ~TRACE_VERBOSE;

    if (flTrace & TRACE_VERBOSE)
        flTrace &= ~TRACE_QUIET;

    if (bOp == OP_INT3) {
        pesf->EIP++;
        if (flTrace & TRACE_VERBOSE)
            x86RegDump(pesf);
        x86InsDump(pesf, pesf->CS, pesf->EIP);
        flTrace &= ~TRACE_FLAGS;
        return FALSE;
    }
    return TRUE;
}


BOOL x86BPCommand(PESF pesf, PCHAR pchCmd, DWORD sel, DWORD off, PCHAR pchAddr, PCHAR pchArgs)
{
    CHAR ch;
    INT i, flBP = 0;
    register PBPD pbpd;

    ch = _tolower(*pchCmd++);

    switch(ch) {
    case 'c':
    case 'd':
    case 'e':
        if (*pchAddr == '*')
            break;
    case 'i':
    case 'p':
        if (pchArgs == pchAddr)
            return FALSE;
        break;
    }

    switch(ch) {
    case '?':
        printf("Breakpoint commands\n"
               "\tbc\tclear breakpoint\n"
               "\tbd\tdisable breakpoint\n"
               "\tbe\tenable breakpoint\n"
               "\tbl\tlist breakpoints\n"
               "\tbt\tset breakpoint (eg, BT 70:100)\n"
               "\tbi\tset int breakpoint (eg, BI 10 AH=0)\n"
        );
        break;

    case 'c':
        if (*pchAddr == '*') {
            for (i=0; i<MAX_BP; i++)
                x86FreeBP(i);
            break;
        }
        return x86FreeBP(off);

    case 'd':
        if (*pchAddr == '*') {
            for (i=0; i<MAX_BP; i++)
                x86DisableBP(i);
            break;
        }
        return x86DisableBP(off);

    case 'e':
        if (*pchAddr == '*') {
            for (i=0; i<MAX_BP; i++)
                x86EnableBP(i);
            break;
        }
        return x86EnableBP(off);

    case 'l':
        x86ListBPs(pesf);
        break;

    case 'i':
        sel = 0;
        flBP = BP_INT;
        // Fall into the BP case...

    case 'p':
    case 't':   // make "bt" an alias for "bp"
        pbpd = x86AllocBP(flBP, sel, off);
        if (!pbpd)
            return FALSE;
        if ((i = ParseReg(pchArgs)) && ardRegs[i-1].cbReg) {
            pchArgs += nstrlen(ardRegs[i-1].pszReg);
            if (*pchArgs++ != '=')
                return FALSE;
            pbpd->iReg = i;
            ParseValue(pesf, pchArgs, NULL, &pbpd->dwValue, TRUE);
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}


BOOL x86DumpCommand(PESF pesf, register PCHAR pchCmd, DWORD sel, DWORD off, PCHAR pchAddr, PCHAR pchArgs)
{
    INT cLines = 0;

    if (*pchCmd == '?') {
        printf("Dump commands\n"
               "\tdb\tdump bytes\n"
               "\tdw\tdump words\n"
               "\tdd\tdump dwords\n"
               "\tdg\tdump GDT info\n"
               "\tdi\tdump IDT info\n"
               "\tdtss\tdump TSS\n"
               "\tdiopm\tdump IOPM bitmap\n"
               "\tdos?\tDOS dump commands\n"
        );
        return TRUE;
    }
    if (mystrcmpi(pchCmd, "os") == 0)
        return DOSDumpCommand(pesf, pchCmd+2);

    // Postponed this arg check until after the non-conventional
    // dump commands were processed

    if (_tolower(*pchArgs) == 'l') {
        pchArgs++;
        pchArgs += ParseValue(pesf, pchArgs, NULL, &cLines, TRUE);
    }
    else if (*pchArgs && pchArgs != pchCmd)
        return FALSE;           // 'lxxx' is the only arg currently supported

    if (cLines == 0)
        cLines = 8;

    if (mystrcmpi(pchCmd, "tss") == 0)
        x86MemDump(sel_Data, (DWORD)pTSS, 4, cLines);

    else if (mystrcmpi(pchCmd, "iopm") == 0)
        x86MemDump(sel_Data, (DWORD)pTSS->abIOPM, 1, cLines);

    else {
        // Assorted single-character commands

        switch(_tolower(*pchCmd)) {

        case 'b':
            x86MemDump(sel, off, 1, cLines);
            iSizeLast = 1;
            break;
        case 'w':
            x86MemDump(sel, off, 2, cLines);
            iSizeLast = 2;
            break;
        case 'd':
            x86MemDump(sel, off, 4, cLines);
            iSizeLast = 4;
            break;
        case _tolower(NULL):
            x86MemDump(sel, off, iSizeLast, cLines);
            break;

        // For the DescDump calls, I need to pass "off" instead of "sel",
        // because the format of the commands is simply "d? xxxx", where xxxx
        // would normally be considered an offset.  This is also why DescDump
        // need not worry about checking for SEL_V86 -- it can't happen because
        // of the (WORD) casting.

        case 'g':
            x86DescDump(pGDT, &dtrGDT, "GDT", ++pchCmd, (WORD)off);
            break;

        case 'i':
            x86DescDump(pIDT, &dtrIDT, "IDT", ++pchCmd, (WORD)off);
            break;

        default:
            // Dump if address starts at pchCmd and seems to be valid
            if (pchAddr == pchCmd && pchAddr != pchArgs)
                x86MemDump(sel, off, iSizeLast, cLines);
            else
                return FALSE;
        }
    }
    return TRUE;
}


BOOL x86EditCommand(PESF pesf, PCHAR pchCmd, DWORD sel, DWORD off, PCHAR pchAddr, register PCHAR pchArgs)
{
    DWORD dw;
    INT i = 0, n;
    BOOL fWord = (_tolower(*pchCmd) == 'w');

    while (*pchArgs) {
        n = ParseValue(pesf, pchArgs, NULL, &dw, TRUE);
        if (n == 0)
            break;
        pchArgs += n;
	if (i++ == 8-4*fWord) {
	    i = 1;
	    printf("\n");
	}
	if (i == 1)
            printf("%04x:%08x ", (DWORD)(WORD)sel, off);
	if (fWord) {
            printf(" %04x..%04x ", x86GetWord(sel, off), (WORD)dw);
            x86SetWord(sel, off, (WORD)dw);
	}
	else {
            printf(" %02x..%02x", x86GetByte(sel, off), (BYTE)dw);
            x86SetByte(sel, off, (BYTE)dw);
	}
        off += 1+fWord;
        pchArgs += nstrskip(pchArgs, ' ');
    }
    printf("\n");
    return TRUE;
}


BOOL x86PortCommand(register PESF pesf, register PCHAR pchCmd, PCHAR pchOp, PBYTE piopm)
{
    CHAR ch;
    INT cch, iTotal;
    DWORD dwPort, dwFirst, dwLower, dwUpper;

    dwLower = 0;
    dwUpper = MAX_PORT;
    cch = ParseRange(pesf, pchOp, &dwLower, &dwUpper);
    if (dwUpper > MAX_PORT)
        dwUpper = MAX_PORT;

    switch(ch = _tolower(*pchCmd++)) {
    case '?':
        printf("Port commands\n"
               "\tpc\tuntrap port or range\n"
               "\tpd\ttoggle debugger notification\n"
               "\tpi\tinput from port\n"
               "\tpo\toutput to port\n"
               "\tpl\tlist trapped ports\n"
               "\tpt\ttrap port or range (eg, PP 40..43)\n"
        );
        break;

    case 'l':
        iTotal = 0;
        dwFirst = MAX_DWORD;
        for (dwPort=dwLower; dwPort<=dwUpper; dwPort++) {
            if (!x86PortTrapped(piopm, (WORD)dwPort)) {
                if (dwFirst != MAX_DWORD) {
                    x86DispTrapRange(dwFirst, dwPort);
                    iTotal += dwPort - dwFirst;
                    dwFirst = MAX_DWORD;
                }
            }
            else {
                if (dwFirst == MAX_DWORD)
                    dwFirst = dwPort;
            }
        }
        if (dwFirst != MAX_DWORD) {
            x86DispTrapRange(dwFirst, dwPort);
            iTotal += dwPort - dwFirst;
        }
        printf("%9d ports trapped\n", iTotal);
        break;

    case 'd':
        if (!(flTrace & TRACE_TRAPIO)) {
            flTrace |= TRACE_TRAPIO;
            printf("Port trap notification ON\n");
        }
        else {
            flTrace &= ~TRACE_TRAPIO;
            printf("Port trap notification OFF\n");
        }
        break;

    case 'c':
    case 'p':
    case 't':   // make "pt" an alias for "pp"
        // These commands require a single port, a range of ports
        // separated by a '-', or an asterisk (meaning: ALL ports)

        iTotal = 0;
        if (cch || *pchOp == '*') {
            for (dwPort=dwLower; dwPort<=dwUpper; dwPort++) {
                if (ch == 'c') {
                    if (x86PortTrapped(piopm, (WORD)dwPort)) {
                        iTotal++;
                        x86PortUntrap(piopm, (WORD)dwPort);
                    }
                }
                else {
                    if (!x86PortTrapped(piopm, (WORD)dwPort)) {
                        iTotal++;
                        x86PortTrap(piopm, (WORD)dwPort);
                    }
                }
            }
            printf("%9d modifications\n", iTotal);
        }
        else
            printf("Port or range required\n");
        break;

    default:
        return FALSE;
    }
    return TRUE;
}


BOOL x86VGACommand(register PESF pesf, register PCHAR pchCmd, PCHAR pchOp)
{
    INT i;
    CHAR ch;

    switch(_tolower(*pchCmd++)) {
    case '?':
        printf("VGA commands\n"
               "\tv\tview output\n"
               "\tvp\tdump plane buffers\n"
               "\tvr*\tdump register sets\n"
        );
        break;

    case _tolower(NULL):
        SaveVS(&vsMonitor, FALSE);
        RestoreVS(&vsVM);
        _getch();
        SaveVS(&vsVM, FALSE);
        RestoreVS(&vsMonitor);
        break;

    case 'p':
        i = PLANE0;
        if (*pchCmd >= '0' && *pchCmd <= '3')
            i = *pchCmd - '0';
        printf("Virtual plane buffer #%d:\n", i);
        x86MemDump(sel_Data, (DWORD)vsVM.pbPlane[i], 1, 8);
        break;

    // vr commands are for dumping the vga registers, as follows:
    //
    //  vr* - all of the following
    //  vrs - sequencer regs (@3C4,3C5)
    //  vrg - graphics data controller regs (@3CE,3CF)
    //  vra - attribute controller regs (@3C0,3C1)
    //  vrc - crt controller regs (@3D4,3D5 - if color mode anyway)
    //  vrd - DAC color regs (@3C7,3C8,3C9) -- not implemented yet

    case 'r':
        ch = _tolower(*pchCmd);
        if (!ch || ch == ' ')
            ch = '*';           // default to "*" if nothing specified
        if (ch == 's' || ch == '*') {
            printf("SEQ regs:  ");
            for (i=0; i<MAX_SEQREGS; i++)
                printf("%02x%c ", vsVM.aregSEQData[i], (BYTE)i==vsVM.regSEQIndx?'*':' ');
            printf("\n");
        }
        if (ch == 'g' || ch == '*') {
            printf("GDC regs:  ");
            for (i=0; i<MAX_GDCREGS; i++)
                printf("%02x%c ", vsVM.aregGDCData[i], (BYTE)i==vsVM.regGDCIndx?'*':' ');
            printf("\n");
        }
        if (ch == 'a' || ch == '*') {
            printf("ATC regs:  ");
            for (i=0; i<MAX_ATCREGS; i++) {
                if (i == 16)
                    printf("\n           ");
                printf("%02x%c ", vsVM.aregATCData[i], i==(vsVM.regATCIndx&~ATCPAL_ENABLE)?'*':' ');
            }
            printf("\n");
        }
        if (ch == 'c' || ch == '*') {
            printf("Misc reg:  %02x\n", vsVM.regMiscOut);
            printf("Feat reg:  %02x\n", vsVM.regFeature);
            printf("CRTC regs: ");
            for (i=0; i<MAX_CRTREGS; i++) {
                if (i > 0 && (i % 16) == 0)
                    printf("\n           ");
                printf("%02x%c ", vsVM.aregCRTData[i], (BYTE)i==vsVM.regCRTIndx?'*':' ');
            }
            printf("\n");
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}


INT x86Command(register PESF pesf, PCHAR pchCmd, PINT pflCommand)
{
    CHAR ch;
    INT i, j, fl;
    BOOL fBadAddr;
    DWORD sel, dw;
    DWORD selCode, offCode;
    DWORD selData, offData;
    register PCHAR pch;
    PCHAR pchAddr, pchArgs;

    *pflCommand = 0;
    pch = pchCmd;
    pch += nstrskip(pch, ' ');  // skip leading whitespace

    // Check for parser commands first

    i = ParseCommand(pesf, pch);
    if (i)
        return i;

    // Check for debugger commands next;
    // look for optional sel:off, since so many commands use it

    selCode = selCodeLast;
    offCode = offCodeLast;
    selData = selDataLast;
    offData = offDataLast;

    if (selCode == 0) {
        selCode = pesf->CS;
        offCode = pesf->EIP;
    }

    if (selData == 0)
        selData = pesf->DS;

    pchAddr = pch+1;
    while (_isalpha(*pchAddr))  // advance past the alpha command portion
        pchAddr++;
    pchAddr += nstrskip(pchAddr, ' ');

    fBadAddr = FALSE;
    if (*pchAddr == ':' || *pchAddr == '\0') {
        if (*(pch+2))           // we didn't find a well-separated address,
            pchAddr = pch+1;    // so back up to the second character again,
                                // provided there's a third character as well
    }
    pchArgs = pchAddr + ParseAddr(pesf, pchAddr, &selCode, &offCode, &selData, &offData);
    if (pchArgs == pchAddr) {
      //pchArgs = pch+1;
        if (*pchAddr)
            fBadAddr = TRUE;
    }
    pchArgs += nstrskip(pchArgs, ' ');

    switch(_tolower(*pch++)) {
    case 'b':
        if (!x86BPCommand(pesf, pch, selCode, offCode, pchAddr, pchArgs))
            goto CommandError;
        break;

    case 'd':
        if (!x86DumpCommand(pesf, pch, selData, offData, pchAddr, pchArgs))
            goto CommandError;
        break;

    case 'e':
        if (!x86EditCommand(pesf, pch, selData, offData, pchAddr, pchArgs))
            goto CommandError;
        break;

    case 'g':
        if (pchAddr != pchArgs)
            if (!x86AllocBP(BP_TEMPORARY, selCode, offCode))
                break;
        if (_tolower(*pch) == 't')
            flTrace &= ~TRACE_REEXEC;
        else if (fBadAddr)
            goto BadAddr;
        pesf->Flags &= ~FLAGS_TF;
        *pflCommand |= COMMAND_EXEC;
        break;

    case 'o':
        if (*pch == '?') {
            printf("Options\n"
                   "\tcom#\tuse COM# for output\n"
                   "\tidt\tenable IDT\n"
                   "\tvga\tuse VGA for output (default)\n"
            );
            break;
        }
        pchArgs = pch;
        switch(istrtoken(&pchArgs, apszOptions, ARRAYSIZE(apszOptions))) {
        case 0:             // "com"
            if (!COMInit(*pchArgs-'0'))
                printf("COM port not found\n");
            else if (!(vsMonitor.flVState & VSTATE_DISABLE)) {
                SaveVS(&vsMonitor, FALSE);
                RestoreVS(&vsVM);
                vsVM.flVState |= VSTATE_DISABLE;
                vsMonitor.flVState |= VSTATE_DISABLE;
                printf("COM mode enabled\n");
            }
            else
                printf("COM mode already enabled\n");
            break;
        case 1:             // "idt"
            switch(istrtoken(&pchArgs, apszIDTOptions, ARRAYSIZE(apszIDTOptions))) {
            case 0:         // "on"
                if (flTrace & TRACE_IDTOFF) {
                    flTrace &= ~TRACE_IDTOFF;
                    printf("IDT trapping enabled\n");
                }
                else
                    printf("IDT trapping already enabled\n");
                break;
            case 1:         // "off"
                if (!(flTrace & TRACE_IDTOFF)) {
                    flTrace |= TRACE_IDTOFF;
                    printf("IDT trapping disabled\n");
                }
                else
                    printf("IDT trapping already disabled\n");
                break;
            }
            break;
        case 2:             // "vga"
            if (vsMonitor.flVState & VSTATE_DISABLE) {
                vsVM.flVState &= ~VSTATE_DISABLE;
                vsMonitor.flVState &= ~VSTATE_DISABLE;
                SaveVS(&vsVM, FALSE);
                RestoreVS(&vsMonitor);
                printf("VGA mode restored\n");
            }
            else
                printf("VGA mode already restored\n");
            break;
        default:
            goto CommandError;
        }
        break;

    case 'p':
        // P commands are port commands, but P without any
        // arguments is treated as a TS command, due to tradition
        if (!*pch) {
            if (x86Trace(pesf, TRACE_STEP))
                *pflCommand |= COMMAND_EXEC;
            break;
        }
        if (!x86PortCommand(pesf, pch, pchAddr, pTSS->abIOPM))
            goto CommandError;
        break;

    case 'q':
        // if q! is specified, then trash SaveIVT to force warm boot;
        // as you can see, I have chosen to trash the divide-by-zero vector

        if (*pch == '!')
            pmbZero->rb.SaveIVT[IDT_DIVERROR] = 0;

        // HACK: Technically, I should be reinitializing the video state of
        // the VM, like with a RestoreVS(&vsVMPristine), but this will have to
        // do for now....

        _asm { cli }

        bDefColor = (CLR_WHITE << CLR_FGND_SHIFT) | (CLR_BLACK << CLR_BGND_SHIFT);
        _scroll(0,0,0,0,0);
        _setcursor(0,0,TRUE);
        _setvistop(0);

        Reboot();
        break;

    case 'r':
        pch += nstrskip(pch, ' ');
        if (*pch && *pch != ';') {
            if (!(i = ParseReg(pch))) {
                printf("Unknown register: \"%s\"\n", pch);
                break;
            }
            pch += nstrlen(ardRegs[--i].pszReg);
            if (ParseValue(pesf, pch, NULL, &dw, TRUE)) {
                if (ardRegs[i].cbReg == 3) {
                    if (pesf->Flags & FLAGS_V86)
                        dw |= SEL_V86;
                    if (!x86DescSize(x86SelDesc(dw))) {
                        printf("Not a valid selector: %04x\n", dw);
                        break;
                    }
                }
                switch(ardRegs[i].cbReg) {
                case 1:
                    *((PBYTE)pesf + ardRegs[i].offReg) = (BYTE)dw;
                    break;
                case 2:
                    *(PWORD)((PBYTE)pesf + ardRegs[i].offReg) = (WORD)dw;
                    break;
                case 3:
                case 4:
                    *(PDWORD)((PBYTE)pesf + ardRegs[i].offReg) = dw;
                    break;
                default:
                    printf("Unsupported register\n");
                    break;
                }
            }
            break;
        }
        else
            x86RegDump(pesf);
        // Fall into INSTRUCTION code from here

    case 'i':
        x86InsDump(pesf, pesf->CS, pesf->EIP);
        break;

    case 't':
        if (*pch == '?') {
            printf("Trace commands\n"
                   "\tt\ttrace one instruction\n"
                   "\ttg\ttrace until breakpoint\n"
                   "\tts\tstep one instruction\n"
                   "\ttq\ttrace quiet (no instructions)\n"
                   "\ttv\ttrace verbose (includes registers)\n"
            );
            break;
        }
        fl = 0;
        lInsCount = 0;
        while ((ch = _tolower(*pch++)) != _tolower(NULL)) {
            switch(ch) {
            case ';':
                goto EndTrace;
            case 's':
                fl |= TRACE_STEP;
                break;
            case 'f':
                fl |= TRACE_QUICK;
                break;
            case 'g':
                fl |= TRACE_MULTIPLE;
                break;
            case 'q':
                fl |= TRACE_QUIET;
                fl &= ~TRACE_VERBOSE;
                break;
            case 'v':
                fl |= TRACE_VERBOSE;
                fl &= ~TRACE_QUIET;
                break;
            default:
                goto CommandError;
            }
        }
      EndTrace:
        if (x86Trace(pesf, fl))
            *pflCommand |= COMMAND_EXEC;
        break;

    case 'u':
        if (fBadAddr)
            goto BadAddr;
        for (i=0,j=0; i<8; i++)
            j += x86InsDump(pesf, selCode, offCode+j);
        break;

    case 'v':
        if (!x86VGACommand(pesf, pch, pchAddr))
            goto CommandError;
        break;

    case ';':                   // no-op -- just a command separator
        break;

    case '?':
        if (!*pch) {
            printf("Debugger commands\n"
                   "\tb?\tbreakpoint commands\n"
                   "\td?\tdump commands\n"
                   "\te\tedit memory\n"
                   "\tg\tgo until breakpoint\n"
                   "\ti\tdisplay next instruction\n"
                   "\tl?\tlabel commands\n"
                   "\to?\toptions\n"
                   "\tp?\tport commands\n"
                   "\tq\tquit\n"
                   "\tr\tdisplay/edit registers\n"
                   "\tt?\ttrace commands\n"
                   "\tu\tdisassemble instructions\n"
                   "\tv?\tVGA commands\n"
                   "Use F12 to enter the debugger at any time\n"
            );
        }
        else {
            sel = 0;
            i = ParseValue(pesf, pch, &sel, &dw, TRUE);
            if (i) {
                printf("Value:  ");
                if (sel)
                    printf("%c%04x:", x86AddrType(sel,FALSE), (DWORD)(WORD)sel);
                printf("%08x  %d. \'%c\'\n", dw, dw, _isascii((BYTE)dw)?(BYTE)dw:' ');
                pch += i;
                if (*pch && *pch != ';')
                    printf("Unprocessed characters found \"%s\"\n", pch);
            }
        }
        break;

    default:

    CommandError:
        *pflCommand |= COMMAND_ERROR;
        break;
    }
    goto Exit;

    BadAddr:
        printf("Invalid address: %s\n", pchAddr);

    Exit:
    return nstrskipto(pchCmd, ';');
}


VOID x86Debug(PESF pesf, INT flDebug)
{
    CHAR achInput[MAXSZ];
    INT flCommand;
    PCHAR pchCmd;
    register PSZ pszInput;

    iDebugEntry++;
    if (flDebug & DEBUG_INT)
        iIDTEntry++;

    x86LoadFrame(pesf);

    if (iDebugEntry == 1) {
        if (!(flTrace & TRACE_MASKINTS)) {
            // On the first (DEBUG_INIT) call, the VM state has already
            // been saved and monitor state initialized, by the vidmgr module
            if (!(flDebug & DEBUG_INIT)) {
                SaveVS(&vsVM, FALSE);
                RestoreVS(&vsMonitor);
            }
        }
#ifdef DEBUG
        if (!(vsVM.flVState & VSTATE_DISABLE) && vsVM.cLocks != 0)
            printf("Warning: strange VM lock count: %d\n", vsVM.cLocks);
        if (!(vsMonitor.flVState & VSTATE_DISABLE) && vsMonitor.cLocks != -1)
            printf("Warning: strange Monitor lock count: %d\n", vsMonitor.cLocks);
#endif
    }
    x86RemoveBPs(pesf);

    _asm { sti }

    // If there is no TRACE_INS in progress, or there was but it was
    // a verbose trace, then do the full debugger dump

    if (!(flTrace & TRACE_INS) || (flTrace & TRACE_VERBOSE)) {
        x86TrapDump(pesf, flDebug);
        x86RegDump(pesf);
    }
    else if (!(flTrace & (TRACE_MULTIPLE | TRACE_QUIET | TRACE_VERBOSE)))
        _setcursor(rowCursor-1, colCursor, FALSE);

    if (!(flTrace & TRACE_QUIET))
        x86InsDump(pesf, pesf->CS, pesf->EIP);

#ifdef NMI_PROBE
    // Enable this if NMI to be used as a probe

    if (pesf->iTrap == IDT_NMI)
        goto Exit;
#endif

    // Return immediately if fast tracing enabled,
    // or if this is merely a "peeking" x86Debug() call

    if (flTrace & TRACE_MULTIPLE) {
        lInsCount++;
        if (!(flKeyEvent & KEYEVENT_ABORT))
            if (!x86FindBP(pesf, BP_INUSE|BP_ENABLED, pesf->CS, pesf->EIP))
                if (x86Trace(pesf, flTrace))
                    goto Exit;
        printf("total instructions: %d\n", lInsCount);
    }
    if (flDebug & DEBUG_PEEK)
        goto Exit;

    // For COMMAND_* flags returned by x86Command

    flCommand = 0;
    flTrace &= ~TRACE_FLAGS;

    while (!(flCommand & COMMAND_EXEC)) {

        printf("%c", x86AddrType(pesf->CS, TRUE));

        pszInput = achInput;
        ngets(pszInput);

        while (*pszInput) {
            pchCmd = pszInput;
            pszInput += x86Command(pesf, pchCmd, &flCommand);
            if (*pszInput)
                pszInput++;

            if (flCommand & COMMAND_EXEC)
                break;

            if (flCommand & COMMAND_ERROR) {
                printf("Command error: \"%s\"\n", pchCmd);
                break;
            }
        }
    }
  Exit:
    _asm { cli }

    x86ApplyBPs(pesf);

    if (iDebugEntry == 1) {
        if (!(flTrace & TRACE_MASKINTS)) {
            SaveVS(&vsMonitor, FALSE);
            RestoreVS(&vsVM);
        }
    }
    x86SaveFrame(pesf);

    if (flDebug & DEBUG_INT)
        iIDTEntry--;
    iDebugEntry--;
}
