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
; TSR.ASM
; An exapmle to show off how to install and run a protected mode TSR under
; DOS/32 Advanced DOS Extender. Please note that the TSR support is limited,
; for example you cannot remove the TSR once it has been installed.
;
; Also note the importance of the correct configuration of the TSR, espesially
; its memory requirements. See the "tsr.d32" configuration supplied with this
; example (use the SS utility).
;
; Apparently the TSRs will function when running under Clean, XMS and VCPI
; system softwares. When an external DPMI host is present in the system,
; all will depend on how well it "emulates" DOS/32A built-in DPMI.
;
; A problem that can be encountered under XMS: programs run after the TSR
; has been installed that manipulate A20 gate will cause the system to crash.
; DOS/32 Advanced, when callbacking IRQs/callbacks from real mode will not
; re-enable the A20 line (for speed's sake), and if a program run after the
; TSR has been installed (even temporarily) disables the A20 gate, the DOS
; Extender will not be able to callback the interrupt to the correct address
; due to memory wrap-around. The solutions are:
;  1) do not run programs that behave badly after the TSR has been installed -
;     - no one should disable A20 line since it controls the access to the
;     HMA (among other things) where the DOS's kernel is usually loaded high.
;
;  2) if nr. 1 is not possible, use Object Loading Scheme #3 to load the
;     TSR into DOS conventional memory (as does this example). This is the
;     safest way to ensure that even if someone disables A20 line the
;     callbacks will still hit the correct destination addresses.
;
; This program will install protected mode IRQ 0 (timer) and IRQ 1 (keyboard)
; and will switch to the real mode by issuing INT 21h, AH=31h, placing itself
; into TSR mode. Whenever hardware interrupts that have been hooked in the
; protected mode occur (that will happen in the real mode), they will be
; callbacked by the DOS Extender to the protected mode and the protected
; mode handlers will service the interrupts. The example also shows how to
; provide an API point to the TSR by hooking one of the unused real mode
; interrupt vectors (INT 80h) and callbacking it to the protected mode.
;
;*****************************************************************************

API_INT	equ 80h
SCR_PTR equ 0B8000h


	.386p
	.MODEL flat

DGROUP	group _DATA,STACK		; Tasm will complain about STACK here

.CODE
;=============================================================================
start:
	push	ds			; make ES = DS
	pop	es
	mov	_sel_ds,ds		; store DS for later use

	call	check_if_resident	; check if we are already installed
	jc	@not_resident		; if not, proceed with installation

	mov	eax,'1234'		; API function "1234"
	int	API_INT			; access TSR API

	mov	ax,0FF80h		; say "TSR already installed"
	mov	edx,offset _msg5
	int	21h

	mov	ax,4C00h		; return to DOS
	int	21h


@not_resident:
	cli				; disable interrupts

	call	get_default_irqs	; retrieve default IRQ vectors
	call	install_tsr_irqs	; install our IRQ 0 and 1 handlers
	call	install_api_int		; install TSR API callback

	mov	ax,0FF80h		; say "TSR is now installed"
	mov	edx,offset _msg3
	int	21h

	sti				; enable interrupts

	mov	ah,31h			; go TSR function
	mov	al,00h			; return code for DOS
	mov	dx,0000h		; ignored by DOS/32A
	int	21h			; GO!





;*****************************************************************************
check_if_resident:
	mov	bl,API_INT		; get real mode INT 80h vector by
	mov	ax,0200h		;  using DPMI function 0200h
	int	31h
	mov	_old_int80h_seg,cx	; store for later use
	mov	_old_int80h_off,dx
	mov	ax,cx			; check if vector == 0
	or	ax,dx
	jz	@not_installed		; if yes, our TSR isn't installed
	movzx	eax,cx			; calculate linear addr of INT 80h
	shl	eax,4			;  vector (CS<<4 + IP)
	movzx	edx,dx
	add	eax,edx
	cmp	byte ptr ds:[eax],0CFh	; check if vector points to IRET
	jz	@not_installed		; if yes, our TSR isn't installed
	clc				; TSR is installed, return with
	ret				;  carry flag clear

@not_installed:
	stc				; TSR isn't installed, return with
	ret				;  carry flag set



;=============================================================================
get_default_irqs:
	mov	bl,08h			; get default INT 08h (IRQ 0 timer)
	mov	ax,0204h		; use DPMI function 0204h
	int	31h
	mov	_old_int08h_sel,cx	; store INT 08h selector
	mov	_old_int08h_off,edx	; store INT 08h offset

	mov	bl,09h			; get default INT 09h (IRQ 1 keyboard)
	mov	ax,0204h
	int	31h
	mov	_old_int09h_sel,cx	; store INT 09h selector
	mov	_old_int09h_off,edx	; store INT 09h offset

	ret



;=============================================================================
install_tsr_irqs:
	mov	bl,08h			; set new INT 08h (IRQ 0 timer)
	mov	cx,cs
	mov	edx,offset int08h
	mov	ax,0205h		; use DPMI function 0205h
	int	31h
	mov	ax,0FF80h		; say "IRQ 0 installed"
	mov	edx,offset _msg1
	int	21h

	mov	bl,09h			; set new INT 09h (IRQ 1 keyboard)
	mov	cx,cs
	mov	edx,offset int09h
	mov	ax,0205h
	int	31h
	mov	ax,0FF80h		; say "IRQ 1 installed"
	mov	edx,offset _msg2
	int	21h

	ret



;=============================================================================
install_api_int:
	push	ds			; preserve DS

	push	cs				; make DS = CS
	pop	ds
	mov	ax,0303h			; use DPMI function 0303h
	mov	esi,offset int80h		; DS:ESI = ptr to callback
	mov	edi,offset _int80h_struct	; ES:EDI = ptr to structure
	int	31h				; allocate callback

	mov	bl,API_INT		; install callback as realmode INT 80h
	mov	ax,0201h		; use DPMI function 0201h
	int	31h

	pop	ds
	ret





;*****************************************************************************
; Actual protected mode IRQ 0 handler
;
; Notes:
; 1) all the IRQ handlers in protected mode are called with interrupts
;    disabled
;
; 2) if the interrupt occurs in protected mode, it will be executed on the
;    current protected mode stack (typically your application's 32-bit stack)
;    if the interrupt occurs in the real mode, it will be callbacked to the
;    protected mode and executed on a virual stack temporary allocated by
;    the present DPMI host
;
; 3) no assumptions should be made about the contents of the DS, ES, SS etc
;    registers, especially when the IRQ is callbacked from the real mode
;    the SS will NOT be == DS; the best way to access your data segment in
;    protected mode interrupt handler is to reload the needed selectors
;    through the CS (eg mov ds,cs:_my_ds)
;
; 4) the IRQ handler _may_ switch to an alternative stack, but must return on
;    the same stack frame it was called on by using IRETD instruction
;
; 5) if you plan to enable interrupts inside IRQ handler in protected mode,
;    you must be aware of that the handler may be reentered at any time,
;    especially when chaining to the default IRQ handlers, since the default
;    real mode IRQ handlers have the habit of reenabling interrupts
;
int08h:	pushad				; store registers
	push	ds			; store current DS
	mov	ds,cs:_sel_ds		; reload DS with the proper selector
	inc	dword ptr _tick_counter	; increment counter

	mov	edx,SCR_PTR		; EDX points 1st VGA video page
	mov	al,[edx]		; write a character in the upper
	cmp	al,'A'			;  left corner of the screen
	jae	@int08h_l1		; this short routine will limit the
	mov	al,'A'-1		;  displayed characters to "A".."Z"
@int08h_l1:
	cmp	al,'Z'
	jbe	@int08h_l2
	mov	al,'A'-1
@int08h_l2:
	inc	al
	mov	ah,_char_color
	mov	[edx],ax		; write a colored character

	pushfd				; chain to the default INT 08h handler
	call	fword ptr _old_int08h_off

	pop	ds			; restore DS
	popad				; restore registers
	iretd				; return from interrupt



;*****************************************************************************
; Actual protected mode IRQ 1 handler
;
;See notes above
;
int09h:	pushad				; store registers
	push	ds			; store current DS
	mov	ds,cs:_sel_ds		; reload DS with the proper selector
	inc	byte ptr _char_color	; change the color of the character

	pushfd				; chain to the default INT 09h handler
	call	fword ptr _old_int09h_off

	pop	ds			; restore DS
	popad				; restore registers
	iretd				; return from interrupt



;*****************************************************************************
; Actual protected mode TSR API handler
;
;Notes:
; 1) protected mode callbacks are called with interrupts disabled
;
; 2) the protected mode callbacks are executed on a virtual stack temporarily
;    allocated by the present DPMI host
;
; 3) the contents of general registers are undefined when the callback is
;    entered, except for DS:ESI and ES:EDI registers which point to the
;    real mode stack frame and to protected mode register structure respective
;
; 4) the callback handler _may_ switch to an alternative stack, but it must
;    return on the same stack frame on which it was called with DS:ESI and
;    ES:EDI registers preserved
;
; 5) don't even think about enabling interrupts in a callback handler, it
;    will give you lots of pain and next to no gain at all...
;
int80h:	mov	eax,ds:[esi]		   ; get CS:IP from real mode stack
	mov	dword ptr es:[edi+2Ah],eax ; put CS:IP into prot. mode stack
	add	dword ptr es:[edi+2Eh],06h ; remove FLG and CS:IP from stack

	pushad				; store all registers
	push	ds			; store DS
	mov	ds,cs:_sel_ds		; reload DS with the proper selector

	mov	eax,es:[edi+1Ch]	; get EAX from structure
	cmp	eax,'1234'		; check if it's our function number
	jnz	@int80h_l1		; if not, exit

	mov	ax,0FF80h		; say "TSR API accessed"
	mov	ebx,_tick_counter
	mov	edx,offset _msg4
	int	21h

@int80h_l1:
	pop	ds			; restore DS
	popad				; restore registers
	iretd				; exit callback




.DATA
;=============================================================================
_msg1	db '+ INT 08h (IRQ 0 - timer) has been installed',13,10,0
_msg2	db '+ INT 09h (IRQ 1 - keyboard) has been installed',13,10,0
_msg3	db '+ Protected Mode TSR has been installed',13,10,0
_msg4	db '+ TSR API has been accessed, _tick_counter=%l',13,10,0
_msg5	db '+ Protected Mode TSR already installed',13,10,0

_sel_ds		dw 0

_char_color	db 0Fh	; color of the character displayed on the screen
_tick_counter	dd 0	; counts timer ticks; very amusing,... and useless

_old_int08h_off dd 0	; pointers to default interrupt handlers
_old_int08h_sel	dw 0,0
_old_int09h_off dd 0
_old_int09h_sel	dw 0,0
_old_int80h_off dw 0
_old_int80h_seg	dw 0

_int80h_struct	db 32h dup(0)	; structure for callback at INT 80h



.STACK
;=============================================================================
;	db	1000h dup(?)


end	start
