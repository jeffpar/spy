/*
 *  x86 Debugger Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


/* Debugger input flags
 */
#define DEBUG_NONE      0x00000000      // normal dump and prompt
#define DEBUG_INIT      0x00000001      // initial call into debugger
#define DEBUG_TRAP      0x00000002      // dump trap info also
#define DEBUG_PEEK      0x00000004      // don't prompt, immediately return
#define DEBUG_VMM       0x00000008      // VMM fault notification
#define DEBUG_INT       0x00000010      // IDT fault notification
#define DEBUG_VINT      0x00010000      // virtual software interrupt occurred
#define DEBUG_VINT3     0x00020000      // virtual software interrupt #3 occurred
#define DEBUG_VIO       0x00040000      // virtual I/O instruction occurred


/* Debugger control flags
 */
#define TRACE_NONE      0x00000000      // no tracing in effect
#define TRACE_INS       0x00000001      // trace instruction
#define TRACE_MULTIPLE  0x00000002      // multi-trace enabled
#define TRACE_MASKINTS  0x00000004      // interrupt masking enabled
#define TRACE_REEXEC    0x00000008      // re-execute current instruction
#define TRACE_TRAPIO    0x00000010      // trap I/O instructions
#define TRACE_VERBOSE   0x00000020      // trace verbose mode (reg & ins dump)
#define TRACE_QUIET     0x00000040      // trace quietly (not even ins dump)
#define TRACE_STEP      0x00000080      // trace *over* instruction
#define TRACE_QUICK     0x00000100      // use w/TRACE_STEP to allow MASKINTS
#define TRACE_IDTOFF    0x00000200      // disable IDT trapping
#define TRACE_FLAGS     (TRACE_INS | TRACE_MULTIPLE | TRACE_MASKINTS | TRACE_VERBOSE | TRACE_QUIET | TRACE_STEP | TRACE_QUICK)


/* Mostly internal goop from here on out...
 */
#define CPU_86          0
#define CPU_186         1
#define CPU_286         2
#define CPU_386         3

#define COMMAND_NONE    0x00000000      //
#define COMMAND_EXEC    0x00000001      // begin instruction execution
#define COMMAND_ERROR   0x00000002      // unknown command


/* x86 opcodes (the really useful ones anyway)
 */
#define OP_UNKNOWN      0x00
#define OP_ADDMEMB      0x00
#define OP_ADDMEMW      0x01
#define OP_ADDREGB      0x02
#define OP_ADDREGW      0x03
#define OP_ADDALI       0x04
#define OP_ADDAXI       0x05
#define OP_PUSHES       0x06
#define OP_POPES        0x07
#define OP_ORMEMB       0x08
#define OP_ORMEMW       0x09
#define OP_ORREGB       0x0A
#define OP_ORREGW       0x0B
#define OP_ORALI        0x0C
#define OP_ORAXI        0x0D
#define OP_PUSHCS       0x0E
#define OP_0F           0x0F
#define OP_PUSHDS       0x1E
#define OP_POPDS        0x1F
#define OP_ANDMEMB      0x20
#define OP_ANDMEMW      0x21
#define OP_ANDREGB      0x22
#define OP_ANDREGW      0x23
#define OP_ANDALI       0x24
#define OP_ANDAXI       0x25
#define OP_ES           0x26
#define OP_DAA          0x27
#define OP_SUBMEMB      0x28
#define OP_SUBMEMW      0x29
#define OP_SUBREGB      0x2A
#define OP_SUBREGW      0x2B
#define OP_CS           0x2E
#define OP_SS           0x36
#define OP_CMPMEMB      0x38
#define OP_CMPMEMW      0x39
#define OP_CMPREGB      0x3A
#define OP_CMPREGW      0x3B
#define OP_DS           0x3E
#define OP_INCAX        0x40
#define OP_INCCX        0x41
#define OP_INCDX        0x42
#define OP_INCBX        0x43
#define OP_INCSP        0x44
#define OP_INCBP        0x45
#define OP_INCSI        0x46
#define OP_INCDI        0x47
#define OP_DECAX        0x48
#define OP_DECCX        0x49
#define OP_DECDX        0x4A
#define OP_DECBX        0x4B
#define OP_DECSP        0x4C
#define OP_DECBP        0x4D
#define OP_DECSI        0x4E
#define OP_DECDI        0x4F
#define OP_PUSHAX       0x50
#define OP_PUSHCX       0x51
#define OP_PUSHDX       0x52
#define OP_PUSHBX       0x53
#define OP_PUSHSP       0x54
#define OP_PUSHBP       0x55
#define OP_PUSHSI       0x56
#define OP_PUSHDI       0x57
#define OP_POPAX        0x58
#define OP_POPCX        0x59
#define OP_POPDX        0x5A
#define OP_POPBX        0x5B
#define OP_POPSP        0x5C
#define OP_POPBP        0x5D
#define OP_POPSI        0x5E
#define OP_POPDI        0x5F
#define OP_ARPL         0x63
#define OP_FS           0x64
#define OP_GS           0x65
#define OP_OS           0x66
#define OP_AS           0x67
#define OP_PUSHIW       0x68
#define OP_JO           0x70
#define OP_JNO          0x71
#define OP_JB           0x72
#define OP_JNB          0x73
#define OP_JZ           0x74
#define OP_JNZ          0x75
#define OP_JBE          0x76
#define OP_JNBE         0x77
#define OP_JS           0x78
#define OP_JNS          0x79
#define OP_JP           0x7A
#define OP_JNP          0x7B
#define OP_JL           0x7C
#define OP_JGE          0x7D
#define OP_JLE          0x7E
#define OP_JG           0x7F
#define OP_GRP1MEMB1    0x80
#define OP_GRP1MEMW2    0x81
#define OP_GRP1MEMB2    0x82    // effectively same as 0x80
#define OP_GRP1MEMW1    0x83
#define OP_TESTMEMB     0x84
#define OP_TESTMEMW     0x85
#define OP_XCHGMEMB     0x86
#define OP_XCHGMEMW     0x87
#define OP_MOVMEMB      0x88
#define OP_MOVMEMW      0x89
#define OP_MOVREGB      0x8A
#define OP_MOVREGW      0x8B
#define OP_MOVSEG2      0x8C
#define OP_LEA          0x8D
#define OP_MOV2SEG      0x8E
#define OP_POPMEM       0x8F
#define OP_NOP          0x90
#define OP_XCHGAXDX     0x92
#define OP_CBW          0x98
#define OP_CWD          0x99
#define OP_CALLF        0x9A
#define OP_PUSHF        0x9C
#define OP_POPF         0x9D
#define OP_MOVALOFF     0xA0
#define OP_MOVAXOFF     0xA1
#define OP_MOVSB        0xA4
#define OP_MOVSW        0xA5
#define OP_CMPSB        0xA6
#define OP_CMPSW        0xA7
#define OP_STOSB        0xAA
#define OP_STOSW        0xAB
#define OP_LODSB        0xAC
#define OP_LODSW        0xAD
#define OP_SCASB        0xAE
#define OP_SCASW        0xAF
#define OP_MOVAL        0xB0
#define OP_MOVCL        0xB1
#define OP_MOVDL        0xB2
#define OP_MOVBL        0xB3
#define OP_MOVAH        0xB4
#define OP_MOVCH        0xB5
#define OP_MOVDH        0xB6
#define OP_MOVBH        0xB7
#define OP_MOVAX        0xB8
#define OP_MOVCX        0xB9
#define OP_MOVDX        0xBA
#define OP_MOVBX        0xBB
#define OP_MOVSP        0xBC
#define OP_MOVBP        0xBD
#define OP_MOVSI        0xBE
#define OP_MOVDI        0xBF
#define OP_GRP2B        0xC0
#define OP_GRP2W        0xC1
#define OP_RETNV        0xC2
#define OP_RETN         0xC3
#define OP_LES          0xC4
#define OP_LDS          0xC5
#define OP_MOVMEMIB     0xC6
#define OP_MOVMEMIW     0xC7
#define OP_RETFV        0xCA
#define OP_RETF         0xCB
#define OP_INT3         0xCC
#define OP_INT          0xCD
#define OP_INTO         0xCE
#define OP_IRET         0xCF
#define OP_GRP2SB1      0xD0
#define OP_GRP2SW1      0xD1
#define OP_GRP2SBCL     0xD2
#define OP_GRP2SWCL     0xD3
#define OP_GBP          0xD6    // invalid opcode used for guest breakpoints
#define OP_XLAT         0xD7
#define OP_LOOPNZ       0xE0
#define OP_LOOPZ        0xE1
#define OP_LOOP         0xE2
#define OP_JCXZ         0xE3
#define OP_CALLN        0xE8
#define OP_JMPN         0xE9
#define OP_JMPF         0xEA
#define OP_JMPN8        0xEB
#define OP_LOCK         0xF0
#define OP_REPNZ        0xF2
#define OP_REPZ         0xF3
#define OP_REP          0xF3
#define OP_GRP3MEMIB    0xF6
#define OP_GRP3MEMIW    0xF7
#define OP_CLC          0xF8
#define OP_STC          0xF9
#define OP_CLI          0xFA
#define OP_STI          0xFB
#define OP_GRP4         0xFE
#define OP_GRP5         0xFF

#define OP0F_JO         0x80
#define OP0F_JNO        0x81
#define OP0F_JB         0x82
#define OP0F_JNB        0x83
#define OP0F_JZ         0x84
#define OP0F_JNZ        0x85
#define OP0F_JBE        0x86
#define OP0F_JNBE       0x87
#define OP0F_JS         0x88
#define OP0F_JNS        0x89
#define OP0F_JP         0x8A
#define OP0F_JNP        0x8B
#define OP0F_JL         0x8C
#define OP0F_JGE        0x8D
#define OP0F_JLE        0x8E
#define OP0F_JG         0x8F

#define OP0F_SHLDIB     0xA4
#define OP0F_SHRDIB     0xAC
#define OP0F_MOVZXB     0xB6
#define OP0F_MOVZXW     0xB7

#define OPGRP1_ADD      0x00
#define OPGRP1_OR       0x01
#define OPGRP1_ADC      0x02
#define OPGRP1_SBB      0x03
#define OPGRP1_AND      0x04
#define OPGRP1_SUB      0x05
#define OPGRP1_XOR      0x06
#define OPGRP1_CMP      0x07

#define OPGRP2_ROL      0x00
#define OPGRP2_ROR      0x01
#define OPGRP2_RCL      0x02
#define OPGRP2_RCR      0x03
#define OPGRP2_SHL      0x04
#define OPGRP2_SHR      0x05
#define OPGRP2_SAR      0x07

#define OPGRP3_TEST     0x00
#define OPGRP3_NOT      0x02
#define OPGRP3_NEG      0x03
#define OPGRP3_MUL      0x04
#define OPGRP3_IMUL     0x05
#define OPGRP3_DIV      0x06
#define OPGRP3_IDIV     0x07

#define OPGRP4_INCMEMB  0x00
#define OPGRP4_DECMEMB  0x01

#define OPGRP5_INCMEMW  0x00
#define OPGRP5_DECMEMW  0x01
#define OPGRP5_CALLNMEM 0x02
#define OPGRP5_CALLFMEM 0x03
#define OPGRP5_JMPNMEM  0x04
#define OPGRP5_JMPFMEM  0x05
#define OPGRP5_PUSHMEM  0x06



/* Mnemonic ordinals (indexes into apszMnemonic)
 */
#define M_NONE          0
#define M_AAA           1
#define M_AAD           2
#define M_AAM           3
#define M_AAS           4
#define M_ADC           5
#define M_ADD           6
#define M_AND           7
#define M_ARPL          8
#define M_AS            9
#define M_BOUND         10
#define M_BSF           11
#define M_BSR           12
#define M_BT            13
#define M_BTC           14
#define M_BTR           15
#define M_BTS           16
#define M_CALL          17
#define M_CBW           18
#define M_CLC           19
#define M_CLD           20
#define M_CLI           21
#define M_CLTS          22
#define M_CMC           23
#define M_CMP           24
#define M_CMPSB         25
#define M_CMPSW         26
#define M_CS            27
#define M_CWD           28
#define M_DAA           29
#define M_DAS           30
#define M_DEC           31
#define M_DIV           32
#define M_DS            33
#define M_ENTER         34
#define M_ES            35
#define M_ESC           36
#define M_FADD          37
#define M_FBLD          38
#define M_FBSTP         39
#define M_FCOM          40
#define M_FCOMP         41
#define M_FDIV          42
#define M_FDIVR         43
#define M_FIADD         44
#define M_FICOM         45
#define M_FICOMP        46
#define M_FIDIV         47
#define M_FIDIVR        48
#define M_FILD          49
#define M_FIMUL         50
#define M_FIST          51
#define M_FISTP         52
#define M_FISUB         53
#define M_FISUBR        54
#define M_FLD           55
#define M_FLDCW         56
#define M_FLDENV        57
#define M_FMUL          58
#define M_FNSAVE        59
#define M_FNSTCW        60
#define M_FNSTENV       61
#define M_FNSTSW        62
#define M_FRSTOR        63
#define M_FS            64
#define M_FST           65
#define M_FSTP          66
#define M_FSUB          67
#define M_FSUBR         68
#define M_GBP           69
#define M_GS            70
#define M_HLT           71
#define M_IDIV          72
#define M_IMUL          73
#define M_IN            74
#define M_INC           75
#define M_INS           76
#define M_INT           77
#define M_INT3          78
#define M_INTO          79
#define M_IRET          80
#define M_JBE           81
#define M_JB            82
#define M_JCXZ          83
#define M_JECXZ         84
#define M_JG            85
#define M_JGE           86
#define M_JL            87
#define M_JLE           88
#define M_JMP           89
#define M_JNB           90
#define M_JNBE          91
#define M_JNO           92
#define M_JNP           93
#define M_JNS           94
#define M_JNZ           95
#define M_JO            96
#define M_JP            97
#define M_JS            98
#define M_JZ            99
#define M_LAHF          100
#define M_LAR           101
#define M_LDS           102
#define M_LEA           103
#define M_LEAVE         104
#define M_LES           105
#define M_LFS           106
#define M_LGDT          107
#define M_LGS           108
#define M_LIDT          109
#define M_LLDT          110
#define M_LMSW          111
#define M_LOCK          112
#define M_LODSB         113
#define M_LODSW         114
#define M_LOOP          115
#define M_LOOPNZ        116
#define M_LOOPZ         117
#define M_LSL           118
#define M_LSS           119
#define M_LTR           120
#define M_MOV           121
#define M_MOVSB         122
#define M_MOVSW         123
#define M_MOVSX         124
#define M_MOVZX         125
#define M_MUL           126
#define M_NEG           127
#define M_NOP           128
#define M_NOT           129
#define M_OR            130
#define M_OS            131
#define M_OUT           132
#define M_OUTS          133
#define M_POP           134
#define M_POPA          135
#define M_POPF          136
#define M_PUSH          137
#define M_PUSHA         138
#define M_PUSHF         139
#define M_RCL           140
#define M_RCR           141
#define M_REPNZ         142
#define M_REPZ          143
#define M_RET           144
#define M_RETF          145
#define M_ROL           146
#define M_ROR           147
#define M_SAHF          148
#define M_SAR           149
#define M_SBB           150
#define M_SCASB         151
#define M_SCASW         152
#define M_SETBE         153
#define M_SETB          154
#define M_SETG          155
#define M_SETGE         156
#define M_SETL          157
#define M_SETLE         158
#define M_SETNBE        159
#define M_SETNB         160
#define M_SETNO         161
#define M_SETNP         162
#define M_SETNS         163
#define M_SETNZ         164
#define M_SETO          165
#define M_SETP          166
#define M_SETS          167
#define M_SETZ          156
#define M_SGDT          169
#define M_SHL           170
#define M_SHLD          171
#define M_SHR           172
#define M_SHRD          173
#define M_SIDT          174
#define M_SLDT          175
#define M_SMSW          176
#define M_SS            177
#define M_STC           178
#define M_STD           179
#define M_STI           180
#define M_STOSB         181
#define M_STOSW         182
#define M_STR           183
#define M_SUB           184
#define M_TEST          185
#define M_VERR          186
#define M_VERW          187
#define M_WAIT          188
#define M_XCHG          189
#define M_XLAT          190
#define M_XOR           191
#define MTOTAL          192


/* Function ordinals (indexes into apfnopGuest)
 */
#define F_NONE          0
#define F_AAA           1
#define F_AAD           2
#define F_AAM           3
#define F_AAS           4
#define F_ADCB          5
#define F_ADCW          6
#define F_ADDB          7
#define F_ADDW          8
#define F_ANDB          9
#define F_ANDW          10
#define F_ARPL          11
#define F_AS            12
#define F_BOUND         13
#define F_CALL          14
#define F_CALLF         15
#define F_CBW           16
#define F_CLC           17
#define F_CLD           18
#define F_CLI           19
#define F_CMC           20
#define F_CMPB          21
#define F_CMPSB         22
#define F_CMPSW         23
#define F_CMPW          24
#define F_CS            25
#define F_CWD           26
#define F_DAA           27
#define F_DAS           28
#define F_DECB          29
#define F_DECW          30
#define F_DIVB          31
#define F_DIVW          32
#define F_DS            33
#define F_ENTER         34
#define F_ES            35
#define F_ESC           36
#define F_FS            37
#define F_GBP           38
#define F_GS            39
#define F_HLT           40
#define F_IDIVB         41
#define F_IDIVW         42
#define F_IMULB         43
#define F_IMULW         44
#define F_INB           45
#define F_INCB          46
#define F_INCW          47
#define F_INSB          48
#define F_INSW          49
#define F_INT           50
#define F_INT3          51
#define F_INTO          52
#define F_INW           53
#define F_IRET          54
#define F_JB            55
#define F_JBE           56
#define F_JCXZ          57
#define F_JG            58
#define F_JGE           59
#define F_JL            60
#define F_JLE           61
#define F_JMP           62
#define F_JMPF          63
#define F_JNB           64
#define F_JNBE          65
#define F_JNO           66
#define F_JNP           67
#define F_JNS           68
#define F_JNZ           69
#define F_JO            70
#define F_JP            71
#define F_JS            72
#define F_JZ            73
#define F_LAHF          74
#define F_LDS           75
#define F_LEA           76
#define F_LEAVE         77
#define F_LES           78
#define F_LOCK          79
#define F_LODSB         80
#define F_LODSW         81
#define F_LOOP          82
#define F_LOOPNZ        83
#define F_LOOPZ         84
#define F_MOVB          85
#define F_MOVSB         86
#define F_MOVSW         87
#define F_MOVW          88
#define F_MULB          89
#define F_MULW          90
#define F_NEGBB         91
#define F_NEGBW         92
#define F_NOP           93
#define F_NOTB          94
#define F_NOTW          95
#define F_ORB           96
#define F_ORW           97
#define F_OS            98
#define F_OUTB          99
#define F_OUTSB         100
#define F_OUTSW         101
#define F_OUTW          102
#define F_POP           103
#define F_POPA          104
#define F_POPF          105
#define F_PUSH          106
#define F_PUSHA         107
#define F_PUSHF         108
#define F_RCLB          109
#define F_RCLW          110
#define F_RCRB          111
#define F_RCRW          112
#define F_REPNZ         113
#define F_REPZ          114
#define F_RETF          115
#define F_RETFV         116
#define F_RETN          117
#define F_RETNV         118
#define F_ROLB          119
#define F_ROLW          120
#define F_RORB          121
#define F_RORW          122
#define F_SAHF          123
#define F_SARB          124
#define F_SARW          125
#define F_SBBB          126
#define F_SBBW          127
#define F_SCASB         128
#define F_SCASW         129
#define F_SHLB          130
#define F_SHLW          131
#define F_SHRB          132
#define F_SHRW          133
#define F_SS            134
#define F_STC           135
#define F_STD           136
#define F_STI           137
#define F_STOSB         138
#define F_STOSW         139
#define F_SUBB          140
#define F_SUBW          141
#define F_TESTB         142
#define F_TESTW         143
#define F_WAIT          144
#define F_XCHGB         145
#define F_XCHGW         146
#define F_XLAT          147
#define F_XORB          148
#define F_XORW          149
#define FTOTAL          150

/* Special function groups
 */
#define FGSTART         FTOTAL
#define F_GRP1B        (FTOTAL+0)
#define F_GRP1W        (FTOTAL+1)
#define F_GRP1WB       (FTOTAL+2)
#define F_GRP2B        (FTOTAL+3)
#define F_GRP2W        (FTOTAL+4)
#define F_GRP2B1       (FTOTAL+5)
#define F_GRP2W1       (FTOTAL+6)
#define F_GRP2BC       (FTOTAL+7)
#define F_GRP2WC       (FTOTAL+8)
#define F_GRP3B        (FTOTAL+9)
#define F_GRP3W        (FTOTAL+10)
#define F_GRP4         (FTOTAL+11)
#define F_GRP5         (FTOTAL+12)
#define F_GRP6         (FTOTAL+13)
#define F_GRP7         (FTOTAL+14)
#define F_GRP8         (FTOTAL+15)
#define FGTOTAL         16


/* RD table offsets
 */
#define RD_BYTE         0       // byte regs
#define RD_WORD         8       // word regs
#define RD_SEG          16      // segment regs
#define RD_IP           22      //
#define RD_EIP          23      //
#define RD_DWORD        24      // dword regs
#define RD_CTL          32      // dword control regs
#define RD_DBG          40      // dword debug regs
#define RD_TST          48      // dword test regs
#define RD_FLAGS        56      // flags


/* ModRegRM masks and definitions
 */
#define REG_AL          0x00    // bits 0-2 are standard Reg encodings
#define REG_CL          0x01    // ('AND' with RMMASK to get the encoding)
#define REG_DL          0x02    //
#define REG_BL          0x03    //
#define REG_AH          0x04    //
#define REG_CH          0x05    //
#define REG_DH          0x06    //
#define REG_BH          0x07    //
#define REG_AX          0x08    //
#define REG_CX          0x09    //
#define REG_DX          0x0A    //
#define REG_BX          0x0B    //
#define REG_SP          0x0C    //
#define REG_BP          0x0D    //
#define REG_SI          0x0E    //
#define REG_DI          0x0F    //

#define REG_ES          0x00    //
#define REG_CS          0x01    //
#define REG_SS          0x02    //
#define REG_DS          0x03    //
#define REG_FS          0x04    //
#define REG_GS          0x05    //

#define REG_CR0         0x00    //
#define REG_CR2         0x02    //
#define REG_CR3         0x03    //

#define REG_DR0         0x00    //
#define REG_DR1         0x01    //
#define REG_DR2         0x02    //
#define REG_DR3         0x03    //
#define REG_DR6         0x06    //
#define REG_DR7         0x07    //

#define REG_TR6         0x06    //
#define REG_TR7         0x07    //

#define MOD_NODISP      0x00    // use RM below, no displacement
#define MOD_DISP8       0x01    // use RM below + 8-bit displacement
#define MOD_DISP16      0x02    // use RM below + 16-bit displacement
#define MOD_REGISTER    0x03    // use REG above

#define RM_BXSI         0x00    //
#define RM_BXDI         0x01    //
#define RM_BPSI         0x02    //
#define RM_BPDI         0x03    //
#define RM_SI           0x04    //
#define RM_DI           0x05    //
#define RM_BP           0x06    //
#define RM_IMMOFF       RM_BP   // only if MOD_NODISP
#define RM_BX           0x07    //

#define RM_EAX          0x00    //
#define RM_ECX          0x01    //
#define RM_EDX          0x02    //
#define RM_EBX          0x03    //
#define RM_ESP          0x04    //
#define RM_SIB          RM_ESP  //
#define RM_EBP          0x05    //
#define RM_IMMOFF32     RM_EBP  // only if MOD_NODISP
#define RM_ESI          0x06    //
#define RM_EDI          0x07    //

/* XLATOFF */
#define MODMASK         0xC0    // mod/reg/rm definitions
#define MODSHIFT        6       //
#define MOD(mrb)        (((mrb)&MODMASK)>>MODSHIFT)
#define REGMASK         0x38    //
#define REGSHIFT        3       //
#define REG(mrb)        (((mrb)&REGMASK)>>REGSHIFT)
#define RMMASK          0x07    //
#define RMSHIFT         0       //
#define RM(mrb)         (((mrb)&RMMASK)>>RMSHIFT)
#define MODREGRM(m,r,b) ((BYTE)((((m)<<MODSHIFT)&MODMASK) | \
                                (((r)<<REGSHIFT)&REGMASK) | \
                                (((b)<<RMSHIFT )&RMMASK )))

#define SCALEMASK       0xC0    // scale/indx/base definitions
#define SCALESHIFT      6       //
#define SCALE(sib)      (((sib)&SCALEMASK)>>SCALESHIFT)
#define SCALEMULT(sib)  (1 << SCALE(sib))
#define INDXMASK        0x38    //
#define INDXSHIFT       3       //
#define INDX(sib)       (((sib)&INDXMASK)>>INDXSHIFT)
#define BASEMASK        0x07    //
#define BASESHIFT       0       //
#define BASE(sib)       (((sib)&BASEMASK)>>BASESHIFT)
#define SCALEINDXBASE(s,i,b) ((BYTE)((((s)<<SCALESHIFT)&SCALEMASK) | \
                                     (((i)<< INDXSHIFT)&INDXMASK) | \
                                     (((b)<< BASESHIFT)&BASEMASK)))
/* XLATON */


/* Operand type descriptor masks and definitions
 *
 * Note that the letters in () in the comments refer to Intel's
 * nomenclature used in Appendix A of the 80386 Prog. Reference Manual.
 */
#define TYPE_SIZE       0x000F  // size field
#define TYPE_TYPE       0x00F0  // type field
#define TYPE_IREG       0x0F00  // implied register field
#define TYPE_OTHER      0xF000  // "other" field

// TYPE_SIZE values, describing the size of an operand.  Keep these
// constants in sync with the abTypeSize table.

#define TYPE_NONE       0x0000  //     (all other TYPE fields ignored)
#define TYPE_BYTE       0x0001  // (b) byte, regardless of operand size
#define TYPE_SBYTE      0x0002  //     same as above, but sign-extended
#define TYPE_WORD       0x0003  // (w) word, regardless...
#define TYPE_WORDD      0x0004  // (v) word or double-word, depending...
#define TYPE_WORDD2     0x0005  // (a) two memory operands (BOUND only)
#define TYPE_DWORD      0x0006  // (d) double-word, regardless...
#define TYPE_FARP       0x0007  // (p) 32-bit or 48-bit pointer, depending...
#define TYPE_DESC       0x0008  // (s) 6 byte pseudo-descriptor

// TYPE_TYPE values (assuming the previous field, TYPE_SIZE, is something
// other than TYPE_NONE).  Note that all values implying the presence of a
// ModRegRM byte are neatly arranged such that they are >= TYPE_MODRM...

#define TYPE_IMM        0x0000  // (I) immediate data
#define TYPE_ONE        0x0010  //     implicit 1 (eg, shifts/rotates)
#define TYPE_IMMOFF     0x0020  // (A) immediate offset
#define TYPE_IMMREL     0x0030  // (J) immediate relative
#define TYPE_DSSI       0x0040  // (X) memory addressed by DS:SI
#define TYPE_ESDI       0x0050  // (Y) memory addressed by ES:DI
#define TYPE_IMPREG     0x0060  //     implicit register in TYPE_IREG
#define TYPE_IMPSEG     0x0070  //     implicit seg. register in TYPE_IREG
#define TYPE_MODRM      0x0080  // (E) standard ModRM decoding
#define TYPE_MODMEM     0x0090  // (M) Mod CANNOT be MOD_REGISTER
#define TYPE_MODREG     0x00A0  // (R) Mod can ONLY be MOD_REGISTER
#define TYPE_REG        0x00B0  // (G) standard Reg decoding
#define TYPE_SEGREG     0x00C0  // (S) Reg specifies segment register
#define TYPE_CTLREG     0x00D0  // (C) Reg specifies control register
#define TYPE_DBGREG     0x00E0  // (D) Reg specifies debug register
#define TYPE_TSTREG     0x00F0  // (T) Reg specifies test register

// Special TYPE_TYPE values that only apply when TYPE_NONE is true.  For
// instructions that have no operands, these provide more information about
// the type of instruction instead.

#define TYPE_OVERRIDE   0x0010  // instruction prefix

/* Instruction prefix flags used in conjunction with TYPE_OVERRIDE
 */
#define OVERRIDE_MASK   0xFFE0
#define OVERRIDE_ES     0x0020  // 0x26
#define OVERRIDE_CS     0x0040  // 0x2E
#define OVERRIDE_SS     0x0080  // 0x36
#define OVERRIDE_DS     0x0100  // 0x3E
#define OVERRIDE_FS     0x0200  // 0x64
#define OVERRIDE_GS     0x0400  // 0x65
#define OVERRIDE_OS     0x0800  // 0x66
#define OVERRIDE_AS     0x1000  // 0x67
#define OVERRIDE_LOCK   0x2000  // 0xF0
#define OVERRIDE_REPNZ  0x4000  // 0xF2
#define OVERRIDE_REPZ   0x8000  // 0xF3
#define OVERRIDE_REP    (OVERRIDE_REPNZ | OVERRIDE_REPZ)

// TYPE_IREG (implied register) values, based on the REG_* constants.
// For convenience, they include TYPE_IMPREG or TYPE_IMPSEG as appropriate.

#define TYPE_AL        (REG_AL<<8|TYPE_IMPREG|TYPE_BYTE)
#define TYPE_CL        (REG_CL<<8|TYPE_IMPREG|TYPE_BYTE)
#define TYPE_DL        (REG_DL<<8|TYPE_IMPREG|TYPE_BYTE)
#define TYPE_BL        (REG_BL<<8|TYPE_IMPREG|TYPE_BYTE)
#define TYPE_AH        (REG_AH<<8|TYPE_IMPREG|TYPE_BYTE)
#define TYPE_CH        (REG_CH<<8|TYPE_IMPREG|TYPE_BYTE)
#define TYPE_DH        (REG_DH<<8|TYPE_IMPREG|TYPE_BYTE)
#define TYPE_BH        (REG_BH<<8|TYPE_IMPREG|TYPE_BYTE)
#define TYPE_AX        (REG_AX<<8|TYPE_IMPREG|TYPE_WORDD)
#define TYPE_CX        (REG_CX<<8|TYPE_IMPREG|TYPE_WORDD)
#define TYPE_DX        (REG_DX<<8|TYPE_IMPREG|TYPE_WORDD)
#define TYPE_BX        (REG_BX<<8|TYPE_IMPREG|TYPE_WORDD)
#define TYPE_SP        (REG_SP<<8|TYPE_IMPREG|TYPE_WORDD)
#define TYPE_BP        (REG_BP<<8|TYPE_IMPREG|TYPE_WORDD)
#define TYPE_SI        (REG_SI<<8|TYPE_IMPREG|TYPE_WORDD)
#define TYPE_DI        (REG_DI<<8|TYPE_IMPREG|TYPE_WORDD)
#define TYPE_ES        (REG_ES<<8|TYPE_IMPSEG|TYPE_WORD)
#define TYPE_CS        (REG_CS<<8|TYPE_IMPSEG|TYPE_WORD)
#define TYPE_SS        (REG_SS<<8|TYPE_IMPSEG|TYPE_WORD)
#define TYPE_DS        (REG_DS<<8|TYPE_IMPSEG|TYPE_WORD)
#define TYPE_FS        (REG_FS<<8|TYPE_IMPSEG|TYPE_WORD)
#define TYPE_GS        (REG_GS<<8|TYPE_IMPSEG|TYPE_WORD)

// TYPE_OTHER bit definitions (not used for much at this point;  mainly bits
// of interest to the emulator).

#define TYPE_IN         0x1000  // operand is input
#define TYPE_OUT        0x2000  // operand is output
#define TYPE_BOTH      (TYPE_IN|TYPE_OUT)
#define TYPE_86        (CPU_86  << 14)
#define TYPE_186       (CPU_186 << 14)
#define TYPE_286       (CPU_286 << 14)
#define TYPE_386       (CPU_386 << 14)

// Instruction descriptor structure

typedef struct _ID {            /* id */
    BYTE    bMnemonic;          // see M_* constants
    BYTE    bFunction;          // see F_* constants
    WORD    wTypeDst;           // see TYPE* constants
    WORD    wTypeSrc;           // ditto
    WORD    wTypeSrc2;          // ditto
#ifdef  DEBUG
    DWORD   dwHitCount;         // instruction hits counter
#endif
} ID;
typedef ID *PID;


// Register descriptor structure

typedef struct _RD {            /* rd */
    PSZ     pszReg;             // name of register
    BYTE    offReg;             // offset in ESF structure
    BYTE    cbReg;              // size of register (1,2,4; 3=>segreg)
} RD;
typedef RD *PRD;


#define MAX_BP          20

#define BP_INUSE        0x0100  // BP entry is in use
#define BP_ENABLED      0x0200  // BP entry is to be enabled
#define BP_TEMPORARY    0x0400  // BP entry is temporary (remove it when hit)
#define BP_INPLACE      0x0800  // BP opcode is in place
#define BP_INT          0x1000  // BP entry is an interrupt breakpoint

typedef struct _BPD {           /* bpd */
    DWORD   selBP;              // selector
    DWORD   offBP;              // offset, or interrupt #
    INT     iReg;               // index+1 into ardRegs[], 0 if none
    DWORD   dwValue;            // value of ardRegs[iReg] to match, if any
    INT     flBP;               // BP flags; low byte used for patched contents
} BPD;
typedef BPD *PBPD;


// x86debug.c

VOID    x86Trap(PESF pesf, INT flDebug, INT iNum);
VOID    x86LoadFrame(PESF pesf);
VOID    x86SaveFrame(PESF pesf);

CHAR    x86AddrType(DWORD sel, BOOL fPrompt);
PDES    x86SelDesc(DWORD sel);
BOOL    x86IsGate(PDES pdes);
DWORD   x86DescBase(PDES pdes);
DWORD   x86DescSize(PDES pdes);
DWORD   x86RegValue(PESF pesf, INT iReg);

BYTE    x86GetByte(DWORD sel, DWORD off);
WORD    x86GetWord(DWORD sel, DWORD off);
DWORD   x86GetDWord(DWORD sel, DWORD off);
INT     x86GetBytes(DWORD sel, DWORD off, INT n, PVOID p);
VOID    x86SetByte(DWORD sel, DWORD off, BYTE b);
VOID    x86SetWord(DWORD sel, DWORD off, WORD w);
VOID    x86SetDWord(DWORD sel, DWORD off, DWORD dw);

VOID    x86TrapDump(PESF pesf, INT flDebug);
VOID    x86RegDump(PESF pesf);
INT     x86InsDump(PESF pesf, DWORD sel, DWORD off);
BOOL    x86DescDump(PDES pdesTbl, PDTR pdtr, PSZ pszTbl, PCHAR pchCmd, DWORD sel);
VOID    x86MemDump(DWORD sel, DWORD off, INT fSize, INT cLines);
VOID    x86StackDump(PESF pesf, DWORD sel, DWORD off, INT cLines);
INT     x86Decode(PESF pesf, DWORD sel, DWORD off,
            PSZ pszIns, PSZ pszData, PBYTE pbOp, PBYTE pbModRM);
BOOL    x86DecodeOperand(WORD wType, BYTE bModRM, PINT piSegPrefix,
            PESF pesf, DWORD sel, PDWORD poffCur, PSZ pszIns);
BOOL    x86CopyOperand(WORD wType, BYTE bModRM, PESF pesf,
            DWORD sel, DWORD offCur, PINT pSize, PPDWORD ppdw);

PBPD    x86AllocBP(INT flBP, DWORD sel, DWORD off);
BOOL    x86FreeBP(INT i);
VOID    x86ListBPs(PESF pesf);
INT     x86FindBP(PESF pesf, INT flBP, DWORD sel, DWORD off);
VOID    x86ApplyBPs(PESF pesf);
VOID    x86SetBP(PESF pesf, PBPD pbpd);
VOID    x86RemoveBPs(PESF pesf);

VOID    x86PortTrap(PBYTE piopm, WORD wPort);
VOID    x86PortUntrap(PBYTE piopm, WORD wPort);
BOOL    x86PortTrapped(PBYTE piopm, WORD wPort);
VOID    x86DispTrapRange(DWORD dwFirst, DWORD dwLast);

BOOL    x86Trace(PESF pesf, INT fl);
BOOL    x86BPCommand(PESF pesf, PCHAR pchCmd, DWORD sel, DWORD off, PCHAR pchAddr, PCHAR pchArgs);
BOOL    x86DumpCommand(PESF pesf, PCHAR pchCmd, DWORD sel, DWORD off, PCHAR pchAddr, PCHAR pchArgs);
BOOL    x86EditCommand(PESF pesf, PCHAR pchCmd, DWORD sel, DWORD off, PCHAR pchAddr, PCHAR pchArgs);
BOOL    x86PortCommand(PESF pesf, PCHAR pchCmd, PCHAR pchOp, PBYTE piopm);
BOOL    x86VGACommand(PESF pesf, PCHAR pchCmd, PCHAR pchOp);
INT     x86Command(PESF pesf, PCHAR pchCommand, PINT pflCommand);

VOID    x86Debug(PESF pesf, INT flDebug);
