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
; INT1CH.ASM
; This program will show how DOS/32 Advanced callbacks INT 1Ch from real mode
; to protected mode when a protected mode handler for it is installed.
; When INT 16h (wait for key) is issued, the DOS Extender will switch the
; processor to real mode. Each time a real mode INT 1Ch is encountered, it
; will be automatically callbacked by the DOS Extender to the protected mode.
; Other interrupts that are callbacked from real mode to protected mode when
; a protected mode interrupt for them is installed are: IRQ 0-7, IRQ 8-15,
; INT 1Bh, INT 1Ch, INT 23h and INT 24h. Simply install a protected mode
; handler for these interrupts as shown in example below, and the DOS Extender
; will automatically allocate a callback for it.
;
; A word of warning: callbacks used for IRQs are special callbacks. They
; don't pass the general registers across mode switches for speed's sake.
; Therefore you cannot pass a value in a register across mode switch when
; using IRQ callbacks. If you are writing a program which requires this,
; use DPMI function 0303h - Allocate Real Mode Callback, which will do the
; job.
;
; Note that real mode INT 1Ch will be callbacked to protected mode only
; when and if you install a protected mode interrupt for it. Otherwise, the
; real mode INT 1Ch will be processed by default handler in real mode. This
; is done so to reduce the number of unneccesary mode switches.
;
; Problem: DOS/32 Advanced DOS Extender built-in DPMI will not be able to
; trap down and report exceptions that come from an interrupt handler on
; *OLDER* MACHINES! When this program was tested on a 386DX-25 it simply
; crashed when an exception was forced in the interrupt handler. It seems
; that one must send an EOI to the interrupt controller before checking for
; the exception in the DPMI kernel. However, on a 486 and Pentium, which had
; newer interrupt controllers the exception was trapped and reported without
; any problems... Never mind...
;
;*****************************************************************************

	.386p
	.MODEL flat

DGROUP	group _DATA,STACK		; Tasm will complain about STACK here

.CODE
;=============================================================================
start:
	mov	_sel_ds,ds		; store DS for later use

	mov	ah,09h			; show message1
	mov	edx,offset _msg1	; "installing interrupt"
	int	21h

	mov	bl,1Ch			; get default INT 1Ch
	mov	ax,0204h
	int	31h
	mov	_old_int1Ch_sel,cx	; store INT 1Ch selector
	mov	_old_int1Ch_off,edx	; store INT 1Ch offset

	mov	bl,1Ch			; set new INT 1Ch
	mov	cx,cs
	mov	edx,offset int1Ch
	mov	ax,0205h
	int	31h

	mov	ah,09h			; show message2
	mov	edx,offset _msg2	; "interrupt is active"
	int	21h
	mov	byte ptr _int1Ch_active,1	; enable custom INT 1Ch

	xor	ax,ax			; wait for a key (from real mode)
	int	16h			; Note: switch to real mode

	mov	byte ptr _int1Ch_active,0	; disable custom INT 1Ch
	mov	ah,09h			; show message3
	mov	edx,offset _msg3	; "deinstalling interrupt"
	int	21h

	mov	bl,1Ch			; restore default INT 1Ch
	mov	cx,_old_int1Ch_sel
	mov	edx,_old_int1Ch_off
	mov	ax,0205h
	int	31h

	mov	ax,4C00h		; return to DOS with error code 00h
	int	21h




;-----------------------------------------------------------------------------
int1Ch:	pushad				; store registers
	push	ds			; store current DS
	mov	ds,cs:_sel_ds		; I love zero-based flat memory model

;	mov	al,20h			; send EOI to the interrupt controller
;	out	20h,al			; try it if the next line doesn't work
;	mov	cs:[0],al		; try this one on your machine!!!

	xor	al,al
	cmp	al,_int1Ch_active	; if (_int1Ch_active==0)
	jz	@@done			; then jump
	mov	ah,09h			; else show string1
	mov	edx,offset _str1
	int	21h			; Note: nested calls (RM->PM->RM)

	mov	al,_str1+1		; ajust string
	mov	ah,"\"
	cmp	al,"-"
	jz	@@1
	mov	ah,"|"
	cmp	al,"\"
	jz	@@1
	mov	ah,"/"
	cmp	al,"|"
	jz	@@1
	mov	ah,"-"
	cmp	al,"/"
	jz	@@1

@@1:	mov	_str1+1,ah
@@done:	pop	ds			; restore DS
	popad				; restore registers
	iretd				; return from interrupt



.DATA
;=============================================================================
_msg1	db 0Dh,0Ah,'Installing protected mode INT 1Ch',0Dh,0Ah,'$'
_msg2	db 'Protected mode INT 1Ch is active: $'
_msg3	db 0Dh,0Ah,'Deinstalling protected mode INT 1Ch',0Dh,0Ah,'$'

_str1	db '[-]',08h,08h,08h,'$'

_sel_ds		dw 0
_int1Ch_active	db 0
_old_int1Ch_sel	dw 0
_old_int1Ch_off dd 0


.STACK
;=============================================================================
	db	1000h dup(?)


end	start
