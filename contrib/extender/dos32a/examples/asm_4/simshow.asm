;
; Copyright (C) 1996-2002 Supernar Systems, Ltd. All rights reserved.
;
; Redistribution  and  use  in source and  binary  forms, with or without
; modification,  are permitted provided that the following conditions are
; met:
;
; 1.  Redistributions  of  source code  must  retain  the above copyright
; notice, this list of conditions and the following disclaimer.
;
; 2.  Redistributions  in binary form  must reproduce the above copyright
; notice,  this  list of conditions and  the  following disclaimer in the
; documentation and/or other materials provided with the distribution.
;
; 3. The end-user documentation included with the redistribution, if any,
; must include the following acknowledgment:
;
; "This product uses DOS/32 Advanced DOS Extender technology."
;
; Alternately,  this acknowledgment may appear in the software itself, if
; and wherever such third-party acknowledgments normally appear.
;
; 4.  Products derived from this software  may not be called "DOS/32A" or
; "DOS/32 Advanced".
;
; THIS  SOFTWARE AND DOCUMENTATION IS PROVIDED  "AS IS" AND ANY EXPRESSED
; OR  IMPLIED  WARRANTIES,  INCLUDING, BUT  NOT  LIMITED  TO, THE IMPLIED
; WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED.  IN  NO  EVENT SHALL THE  AUTHORS  OR  COPYRIGHT HOLDERS BE
; LIABLE  FOR  ANY DIRECT, INDIRECT,  INCIDENTAL,  SPECIAL, EXEMPLARY, OR
; CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF
; SUBSTITUTE  GOODS  OR  SERVICES;  LOSS OF  USE,  DATA,  OR  PROFITS; OR
; BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
; WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
; OTHERWISE)  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
; ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;

;*****************************************************************************
;
; SIMSHOW.ASM
; This program will show how DOS/32 Advanced reports unexpected interrupts
; issued to emulate IRQs. Interrupts in range INT 08h-0Fh will be reported
; as software interrups. This feature can be disabled using SUNSYS Setup
; Program, option Trap and Report Software INTs. If the user has installed
; a custom exception handler, it will be executed instead of reporting an
; error. If this feature is disabled, the software interrupts issued to
; emulate IRQs will be sent to the appropriate protected mode IRQ handlers
; if those are installed. Otherwise an exception will be reported (if the
; user has installed a custom exception handler it will be executed).
; NOTE: this program will not work correctly under external DPMI.
;
;*****************************************************************************

	.386p
	.MODEL flat

DGROUP	group _DATA,STACK		; Tasm will complain about STACK here

.CODE
;=============================================================================
start:	push	ds			; make ES=DS
	pop	es
	mov	_sel_ds,ds

	mov	ax,0FF8Ah		; check if DOS/32 Advanced is present
	int	21h			; and get ptr to DPMI Kernel config
	cmp	eax,'ID32'
	jz	@@1
	mov	ax,4CFFh		; if not present, exit
	int	21h

@@1:	mov	ax,0204h		; get protected mode IRQ 0 vector
	mov	bl,08h
	int	31h
	mov	_irq0_sel,cx		; save selector and offset
	mov	_irq0_offs,edx

	cli
	mov	ax,0203h		; install handler for exception #8
	mov	bl,08h
	mov	cx,cs
	mov	edx,offset exc0
	int	31h
	mov	ax,0205h		; install custom IRQ 0
	mov	bl,08h
	mov	cx,cs
	mov	edx,offset irq0
	int	31h

	sti				; enable hardware interrupts
@@2:	cmp	byte ptr _wait,0	; wait until IRQ 0 is triggered
	jz	@@2			;  by a hardware interrupt
	cli				; disable hardware interrupts
;
; Note: hardware interrupts are disabled all the way down to INT 21h/AH=4Ch
;
	mov	al,fs:[esi+00h]		; get DPMI Kernel config bits
	and	al,11101111b		; turn off Soft-INT trapping
	mov	fs:[esi+00h],al		; modify DPMI Kernel config
	int	08h			; now issue a software INT 8
;
; Note: interrupt above will be sent to IRQ 0 handler
;
	mov	al,fs:[esi+00h]		; get DPMI Kernel config bits
	or	al,00010000b		; turn on Soft-INT trapping
	mov	fs:[esi+00h],al		; modify DPMI Kernel config
	int	08h			; issue a software interrupt INT 8
;
; Note: interrupt above will be sent to exception #8 handler
;
	mov	ax,0205h		; restore default IRQ 0
	mov	bl,08h
	mov	cx,_irq0_sel
	mov	edx,_irq0_offs
	int	31h

	mov	ax,4C00h		; return to DOS with error code 00h
	int	21h

;
; irq0:	This subroutine handles both software and hardware interrupts INT 08h.
;	The first time this subroutibe is called, it will show message _msg1,
;	the second time _msg2.
;	However, arghhhh... why the hell am I bothering... it works all right.
;	This is not really important anyway.
;
;-----------------------------------------------------------------------------
irq0:	pushad
	mov	ds,cs:_sel_ds

	mov	ah,09h
	test	word ptr [esp+28h],0200h	; check IF status on stack
	mov	edx,offset _msg1		; if IF=1, interrupts are on
	jnz	irq0_1
	mov	edx,offset _msg2		; if IF=0, interrupts are off
irq0_1:	int	21h

	inc	byte ptr _wait			; increment counter
	mov	al,20h				; send EOI
	out	20h,al
	popad
	iretd

;-----------------------------------------------------------------------------
exc0:	pushad					; exception handler
	mov	ds,cs:_sel_ds
	mov	ah,09h
	mov	edx,offset _msg3
	int	21h
	popad
	iretd					; Note: IRETD, *NOT* RETF!





.DATA
;=============================================================================
_wait	db 0
_sel_ds	dw 0

_irq0_sel	dw 0
_irq0_offs	dd 0

_msg1	db '  IRQ 0 was triggered by a Hardware Interrupt',0Dh,0Ah,'$'
_msg2	db '  IRQ 0 was triggered by a Software Interrupt',0Dh,0Ah,'$'
_msg3	db '  EXCEPTION #8 was triggered by a Software Interrupt',0Dh,0Ah,'$'



.STACK
;=============================================================================
	db	1000h dup(?)


end	start
