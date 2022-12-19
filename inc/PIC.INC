PIC_MASTER	EQU	20H
PIC_MASTER_IMR	EQU	21H
PIC_SLAVE	EQU	0A0H
PIC_SLAVE_IMR	EQU	0A1H
PIC_EOI	EQU	20H
MASTER_BASE	EQU	<IDT_MASTER_BASE>
MASTER_BASE_DEF	EQU	08H
SLAVE_BASE	EQU	<IDT_SLAVE_BASE>
SLAVE_BASE_DEF	EQU	70H
IRQ_SLAVE	EQU	02H
ICW1_ICW4	EQU	01H
ICW1_SINGLE	EQU	02H
ICW1_CASCADE	EQU	00H
ICW1_INTERVAL4	EQU	04H
ICW1_INTERVAL8	EQU	00H
ICW1_LEVEL_TRIG	EQU	08H
ICW1_EDGE_TRIG	EQU	00H
ICW1_ICW	EQU	10H
ifndef PS2
ICW1	EQU	11H
else
ICW1	EQU	19H
endif
ICW4_8086_MODE	EQU	01H
ICW4_8085_MODE	EQU	00H
ICW4_AUTO_EOI	EQU	02H
ICW4_NORM_EOI	EQU	00H
ICW4_NON_BUF_MODE	EQU	00H
ICW4_BUF_MODE_SLAVE	EQU	08H
ICW4_BUF_MODE_MASTER	EQU	0CH
ICW4_SPEC_FULLY_NESTED	EQU	10H

ReadIRR macro   pic
        mov     al,0Ah
        out     pic,al
        IODelay
        in      al,pic
        movzx   eax,al
endm

ReadISR macro   pic
        mov     al,0Bh
        out     pic,al
        IODelay
        in      al,pic
        movzx   eax,al
endm

ReadIMR macro   pic
        in      al,pic+1
        movzx   eax,al
endm

SetPICs macro   irqs
        local   master_imr,slave_imr
        master_imr = 0
        slave_imr = 0
        irp     x,<irqs>
        if      x LT IRQ_RTC
        master_imr = master_imr or 1 shl x
        else
        slave_imr = slave_imr or 1 shl x
        endif
        endm

	mov	al,ICW1
        out     PIC_MASTER,al
        mov     al,MASTER_BASE
        IODelay
        out     PIC_MASTER_IMR,al
        mov     al,1 shl IRQ_SLAVE
        IODelay
        out     PIC_MASTER_IMR,al
        mov     al,ICW4_8086_MODE or ICW4_NORM_EOI or ICW4_NON_BUF_MODE
        IODelay
        out     PIC_MASTER_IMR,al
        IODelay
        mov     al,NOT master_imr
        out     PIC_MASTER_IMR,al

	mov	al,ICW1
        out     PIC_SLAVE,al
        mov     al,SLAVE_BASE
        IODelay
        out     PIC_SLAVE_IMR,al
        mov     al,IRQ_SLAVE
        IODelay
        out     PIC_SLAVE_IMR,al
        mov     al,ICW4_8086_MODE or ICW4_NORM_EOI or ICW4_NON_BUF_MODE
        IODelay
        out     PIC_SLAVE_IMR,al
        mov     al,NOT slave_imr
        IODelay
        out     PIC_SLAVE_IMR,al
endm

ResetPICs macro
	mov	al,ICW1
        out     PIC_MASTER,al
        mov     al,MASTER_BASE_DEF
        IODelay
        out     PIC_MASTER_IMR,al
        mov     al,1 shl IRQ_SLAVE
        IODelay
        out     PIC_MASTER_IMR,al
        mov     al,ICW4_8086_MODE or ICW4_NORM_EOI or ICW4_NON_BUF_MODE
        IODelay
        out     PIC_MASTER_IMR,al
        mov     al,ds:[rb_SaveMasterIMR]
        IODelay
        out     PIC_MASTER_IMR,al

	mov	al,ICW1
        out     PIC_SLAVE,al
        mov     al,SLAVE_BASE_DEF
        IODelay
        out     PIC_SLAVE_IMR,al
        mov     al,IRQ_SLAVE
        IODelay
        out     PIC_SLAVE_IMR,al
        mov     al,ICW4_8086_MODE or ICW4_NORM_EOI or ICW4_NON_BUF_MODE
        IODelay
        out     PIC_SLAVE_IMR,al
        mov     al,ds:[rb_SaveSlaveIMR]
        IODelay
        out     PIC_SLAVE_IMR,al
endm


