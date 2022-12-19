/*
 *  PIC (8259A) Support
 *
 *  History
 *  -------
 *  Created: 05-Jun-1992 by Jeff Parsons
 */


#define PIC_MASTER              0x20
#define PIC_MASTER_IMR          0x21

#define PIC_SLAVE               0xA0
#define PIC_SLAVE_IMR           0xA1

#define PIC_EOI                 0x20    // PIC end-of-interrupt command


#define MASTER_BASE             IDT_MASTER_BASE
#define MASTER_BASE_DEF         0x08    // default (PC-compatible) base vector

#define SLAVE_BASE              IDT_SLAVE_BASE
#define SLAVE_BASE_DEF          0x70    // default (PC-compatible) base vector

#define IRQ_SLAVE               0x02


#define ICW1_ICW4               0x01
#define ICW1_SINGLE             0x02
#define ICW1_CASCADE            0x00
#define ICW1_INTERVAL4          0x04
#define ICW1_INTERVAL8          0x00
#define ICW1_LEVEL_TRIG         0x08
#define ICW1_EDGE_TRIG          0x00
#define ICW1_ICW                0x10
#ifndef PS2
#define ICW1			0x11
#else
#define ICW1			0x19
#endif

#define ICW4_8086_MODE          0x01
#define ICW4_8085_MODE          0x00
#define ICW4_AUTO_EOI           0x02
#define ICW4_NORM_EOI           0x00
#define ICW4_NON_BUF_MODE       0x00
#define ICW4_BUF_MODE_SLAVE     0x08
#define ICW4_BUF_MODE_MASTER    0x0C
#define ICW4_SPEC_FULLY_NESTED  0x10


/* ASM

ReadIRR macro   pic
        mov     al,0Ah          ;; write OCW
        out     pic,al
        IODelay
        in      al,pic          ;; read result
        movzx   eax,al
endm

ReadISR macro   pic
        mov     al,0Bh          ;; write OCW
        out     pic,al
        IODelay
        in      al,pic          ;; read result
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

*/
