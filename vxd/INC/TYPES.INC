OPTION  SCOPED

ifndef NOBASEDEFS
NULL	EQU	0
FALSE	EQU	0
TRUE	EQU	(0 EQ FALSE)
endif
K	EQU	1024
MAX_WORD	EQU	0FFFFH
MAX_DWORD	EQU	0FFFFFFFFH
ifndef NOBASEDEFS
endif
ifndef NOBASEDEFS
endif

BYTES	STRUC
b_lo	DB	?
b_hi	DB	?
BYTES	ENDS

WORDS	STRUC
w_lo	DW	?
w_hi	DW	?
WORDS	ENDS

RECT	STRUC
rc_left	DD	?
rc_top	DD	?
rc_right	DD	?
rc_bottom	DD	?
RECT	ENDS
