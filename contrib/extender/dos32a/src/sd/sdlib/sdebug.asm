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

		.486p
		.MODEL flat
		LOCALS
		NOJUMPS


PUBLIC	Debug_, _Debug, Debug
PUBLIC	Debug_Init_, _Debug_Init, Debug_Init
include	stddef.inc


.CODE
	Align	4
Debug_:
_Debug:
Debug:
@debug@	db	0C3h		; RET
	xchg	eax,[esp]
	mov	ds:_temp,eax
	xchg	eax,[esp]
	lea	esp,[esp+4]	; remove caller from stack, don't change flags
	pushfd			; simulate an INT (push eflags)
	cli
	push	cs
	push	_temp		; push original caller
	jmp	_int03		; execute INT 3 handler






;=============================================================================
	Align 4
Debug_Init_:				; needed to initialize debugger's INTs
_Debug_Init:
Debug_Init:
	pushfd
	pushad
	push	cs ds es ss fs gs
	cli
	cld

	mov	ax,001Fh
	int	33h
	cli
	cld

	push	es ds			; check for DOS/32 Advanced
	push	cs
	pop	ds
	mov	esi,offs __idstring
	mov	ax,0A00h
	int	31h
	pop	ds
	mov	__api_off,edi
	mov	__api_sel,es
	pop	es
	jnc	@@0
	jmp	@@done
__idstring	db 'SUNSYS DOS/32A',0

@@0:	push	ds
	pop	es
	mov	ds:__datasel,ds		; set up selectors
	mov	ds:__zerosel,ds		; DOS/32A defines DS=ES=CS=0
	mov	ds:__savedstack,esp
	add	ds:__savedstack,56

	mov	al,06h
	call	fword ptr __api_off
	mov	__kernel_codesel,bx
	mov	__kernel_datasel,cx
	mov	__kernel_zerosel,dx
	mov	__kernel_codeseg,si
	mov	__client_codesel,di

	mov	ax,0400h
	int	31h
	mov	__cputype,cl
	mov	ax,0E00h
	int	31h
	shr	al,4
	mov	__fputype,al

	mov	edi,offs _registers
	lea	esi,[esp+52]
	mov	ecx,14
@@l1:	mov	eax,[esi]		; copy general & segment regs into buf
	sub	esi,4
	mov	[edi],eax
	add	edi,4
	loop	@@l1
	mov	eax,cr0
	mov	_cr0,eax
	mov	eax,cr2
	mov	_cr2,eax
	mov	eax,cr3
	mov	_cr3,eax
	mov	eax,[esp+60]
	mov	_eip,eax
	mov	eax,[esp+56]
	mov	_efl,eax
	mov	esi,offs _registers	; copy new regs into old regs
	mov	edi,offs _old_registers
	mov	ecx,19
	rep	movsd
	call	clearkeytab

	mov	ax,0204h		; get IRQ vectors
	mov	bl,0Ah
	int	31h
	mov	_old_intAsel,ecx
	mov	_old_intAoff,edx
	mov	ax,0204h
	mov	bl,0Bh
	int	31h
	mov	_old_intBsel,ecx
	mov	_old_intBoff,edx
	mov	ax,0204h
	mov	bl,0Ch
	int	31h
	mov	_old_intCsel,ecx
	mov	_old_intCoff,edx
	mov	ax,0204h
	mov	bl,0Dh
	int	31h
	mov	_old_intDsel,ecx
	mov	_old_intDoff,edx
	mov	ax,0204h
	mov	bl,0Eh
	int	31h
	mov	_old_intEsel,ecx
	mov	_old_intEoff,edx

	mov	ax,0203h		; install INT handlers
	mov	cx,cs
	mov	bl,0
	mov	edx,offs _int00		; divide by zero
	int	31h
	mov	bl,1
	mov	edx,offs _int01		; INT 01h (trap)
	int	31h
	mov	bl,3
	mov	edx,offs _int03		; INT 03h (breakpnt)
	int	31h
	mov	bl,4
	mov	edx,offs _int04		; INTO
	int	31h
	mov	bl,5
	mov	edx,offs _int05		; bound
	int	31h
	mov	bl,6
	mov	edx,offs _int06		; invalid op
	int	31h
	mov	bl,0Ah
	mov	edx,offs _int0A
	int	31h
	mov	bl,0Bh
	mov	edx,offs _int0B
	int	31h
	mov	bl,0Ch
	mov	edx,offs _int0C
	int	31h
	mov	bl,0Dh
	mov	edx,offs _int0D
	int	31h
	mov	bl,0Eh
	mov	edx,offs _int0E
	int	31h

@@dpmi:	clr	eax			; initialize DEBUG REGISTERS
	mov	dr0,eax		; clear DR0-DR7: linear addresses
	mov	dr1,eax
	mov	dr2,eax
	mov	dr3,eax
	mov	dr6,eax		; clear DR6: bits T,S,D and breakpoint matches
	mov	dr7,eax		; setup DR7: disable all breakpoints
	mov	_dr0,eax
	mov	_dr1,eax
	mov	_dr2,eax
	mov	_dr3,eax
	mov	_dr7,eax
	xor	wptr _dr7,0300h
	mov	old_video_mode,ax
	mov	_data_addr,eax
	mov	_stack_addr,eax
	mov	_current_window,eax
	mov	eax,_eip		; setup variables
	mov	_code_addr,eax
	mov	_data_override,3
	mov	_cpu_xpos,1
	mov	_cpu_ypos,1
	mov	_cpu_xptr,0
	mov	_mem_xpos,14
	mov	_mem_ypos,39
	mov	_stk_xpos,62
	mov	_stk_ypos,30
	mov	_reg_xpos,1
	mov	_reg_ypos,32
	mov	_reg_ldat,12
	mov	_reg_xptr,0
	mov	__buffer,offset __screen2
	mov	_system_block,offset __screen
	mov	_vgastate_buf,offset __vgastatebuf
	mov	_svgastate_buf,offset __svgastatebuf
	mov	_vgastate_set,0			; set no VGA save/restore
	mov	_svgastate_set,0		; set no SVGA save/restore

	mov	ax,1C00h
	mov	cx,0007h
	int	10h
	cmp	al,1Ch
	jnz	@@verr
	cmp	ebx,4096/64
	ja	@@verr
	mov	_vgastate_set,1

	mov	ax,4F04h
	mov	cx,000Fh
	mov	dx,0000h
	int	10h
	cmp	ax,004Fh
	jnz	@@verr
	cmp	ebx,4096/64
	ja	@@verr
	mov	_svgastate_set,1

@@verr:	mov	byte ptr ds:@debug@,90h	; replace RET with NOP


@@done:	pop	gs fs ss es ds		; return to caller
	add	esp,4
	sti
	popad
	popfd
	ret




;-----------------------------------------------------------------------------
_exit:	cli
	cld
	clr	eax
	mov	dr6,eax
	mov	dr7,eax			; reset hardware breakpoints
	mov	ds,cs:__datasel
	mov	es,cs:__datasel
	mov	ss,cs:__datasel
	mov	esp,cs:__savedstack
	and	old_video_mode,7F7Fh
	cmp	old_video_mode,03h
	jnz	@@l1
	call	restore_video		; raw exit to DOS
	call	restore_state
	mov	ax,4C00h
	int	21h
@@l1:	mov	ax,0003h
	int	10h
	call	restore_state
	mov	ax,4C00h
	int	21h

;-----------------------------------------------------------------------------
_abort:	cli
	cld
	clr	eax
	mov	dr6,eax
	mov	dr7,eax			; reset hardware breakpoints
	mov	ds,cs:__datasel
	mov	es,cs:__datasel
	mov	ss,cs:__datasel
	mov	esp,cs:__savedstack
	mov	ax,0003h
	int	10h
	mov	ax,4CFFh
	int	21h


	evendata
__datasel	dw 0
__zerosel	dw 0
__savedstack	dd 0



.DATA
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

__cputype	db 0
__fputype	db 0
__buffer	dd 0
_obj_selector	dw 0
_obj_address	dd 0

.DATA?
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

__screen	db 0C000h dup(?)	; allocate memory for user screen
__screen2	db 10000h dup(?)
__vgastatebuf	db 4096 dup(?)
__svgastatebuf	db 4096 dup(?)

include	sdebug.inc




end
