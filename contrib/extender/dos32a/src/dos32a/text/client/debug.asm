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

PushState


.386p
;=============================================================================
; IN:	AL = interrupt number / extended error number
;			interrupt	exception
;			----------------------------
;	SS:[ESP] =	 AX	+0	 AX	+0
;			 -		 ec	+2
;			 EIP	+2	 EIP	+6
;			 xx:CS	+6	 xx:CS	+10
;			 EFL	+10	 EFL	+14
;
;=============================================================================
critical_handler:
	cli				; immediately disable interrupts
	cld				; clear direction flag
	cmp	esp,40h			; check if low on stack
	jae	critical_handler_start	; if not, proceed
	lss	esp,fword ptr cs:_sel_esp

critical_handler_start:
	push	ax
	mov	ax,ds
	mov	ds,cs:_sel_ds		; get system's DS
	mov	_int_temp,ax		; store application's DS
	mov	al,20h
	out	20h,al			; send EOI to the interrupt controller
	out	0A0h,al
	mov	ax,_pic_mask		; restore PIC mask
	out	0A1h,al
	mov	al,ah
	out	21h,al
	pop	ax
	mov	word ptr _int_number,ax	; store INT number and Last INT number
	cmp	al,08h			; if INT 00h-07h, no errcode
	jb	@@1
	cmp	al,09h			; if INT 09h, no errcode
	jz	@@1
	cmp	al,0Fh			; if INT 0Fh, no errcode
	jz	@@1
	ja	@@next			; if extended error, goto next handler

	push	ds			; check for software INT
	push	esi
	lar	si,[esp+6+6]		; get CS: rights assuming no errcode
	jnz	@@0			; if CS: not valid, jump
	verr	word ptr [esp+6+6]	; verify selector read access
	jnz	@@0			; if non-readable, jump
	not	si			; invert Present bit
	test	si,8000h		; test Present bit
	jnz	@@0			; if segment was not present, jump
	mov	ah,al
	mov	al,0CDh			; AX = ##h, 0CDh (INT ##h instruction)
	mov	ds,[esp+6+6]
	mov	esi,[esp+6+2]
	cmp	ax,[esi-2]
@@0:	pop	esi
	pop	ds
	jz	@@1			; it is software interrupt, jump

; Exception with error code
;---------------------------
	pop	ax			; restore application's AX
	pop	_int_errcode		; remove error_code from stack
	mov	_int_erravail,1
	jmp	@@2

; Exception without error code
;------------------------------
@@1:	pop	ax			; restore application's AX
	mov	_int_erravail,0


;=============================================================================
; now make stack frame look like this:	(relative to EBP)
;	GS	-50
;	FS	-48
;	SS	-46
;	ES	-44
;	PUSHAD	-12
;	EIP	-8
;	xx:CS	-4
;	EFL	-0
;-----------------------------------------------------------------------------
@@2:	pushad
	push	_int_temp		; push application's DS
	push	es ss fs gs

	xor	eax,eax
	mov	dr7,eax			; reset any breakpoints
	mov	_int_ss,ss		; store application's SS:ESP
	mov	_int_esp,esp
	mov	ax,ss
	cmp	ax,_sel_ss		; call from DOS/32A segment?
	mov	edi,esp
	jz	@@2a			; yes, do not copy stack

	lea	esi,[esp+36h]		; SS:ESI=source on stack
	les	edi,fword ptr _sel_esp	; ES:EDI=destination new stack
	sub	edi,2			; adjust EDI (1st dec stk, then push)
	mov	ecx,1Ch
	std
	rep	movs word ptr es:[edi],ss:[esi]	; move pushed regs to main stk
	add	edi,2
	cld
	mov	ss,cs:_sel_ss		; switch to system's SS:ESP
	lea	esp,[edi-2]		; align ESP on 4 bytes boundary

@@2a:	lea	ebp,[edi+32h]
	add	dword ptr [ebp-28],0Ch	; adjust application's ESP by 12 bytes

	mov	al,_int_number
	cmp	al,1
	jz	trap_point
	cmp	al,3
	jz	break_point

	push	ax
	call	restore_pit
	call	tone
	call	restore_inttab
	call	scr_on
	pop	ax
	mov	dx,offs errmsg3		; 'DOS/32A run-time:'
	push	6001h			; manually push error code
	call	prints
	pop	bx

	xor	bx,bx
	mov	bl,al
	add	bx,bx
	push	dword ptr [ebp-08h]	; show EIP
	push	word ptr [ebp-04h]	; show CS:
	push	word ptr i_msg[bx]	; int name
	push	ax			; int number

	mov	dx,offs excmsgE		; assume 'exception'
	mov	bx,[ebp-04h]		; get CS:
	lar	cx,bx			; check if valid selector
	jnz	@@2b
	mov	es,bx
	mov	ah,al
	mov	al,0CDh
	mov	esi,[ebp-08h]		; get EIP
	cmp	ax,es:[esi-2]		; check at CS:[EIP] for INT instruct.
	jnz	@@2b
	mov	dx,offs excmsgI		; 'unexpected interrupt'
	mov	ax,offs excmsgS		; replace name by 'software interrupt'
	mov	[esp+2],ax

@@2b:	push	dx
	mov	dx,offs excmsg1		; main string
	call	prints
	add	sp,12

	mov	ax,0006h		; get base of application's CS:
	mov	bx,[ebp-04h]
	int	31h
	shl	ecx,16
	mov	cx,dx			; ECX=CS's Base
	mov	eax,[ebp-08h]		; EAX=offset of faulting instruction
	call	check_faultsel		; check if fault occured in app's seg
	jc	@@3			; if not, show linear address

	push	eax		;******** calculate unrelocated addr
	push	si
	mov	dx,offs excmsg3		; 'unrelocated...'
	jmp	@@4

@@3:	push	eax
	push	0000h
	mov	dx,offs excmsg2		; 'linear...'
@@4:	call	prints
	add	sp,6

	cmp	_int_erravail,0		; if error code present, show it
	jz	@@5
	push	_int_errcode		; show error code
	mov	dx,offs excmsg4
	call	prints
	pop	eax

@@5:	call	printcr
	test	_misc_byte2,00010000b
	jz	@@6
	mov	dx,offs excmsgC
	push	_process_id
	push	offs start
	call	prints
	pop	eax

@@6:	call	show_status
@@done:	mov	al,-1			; exit with error code 0FFh
	jmp	exit386

;
; extended error handler
;
@@next:	lss	esp,fword ptr cs:_sel_esp
	push	ax
	call	restore_pit
	call	tone			; handle extended errors
	call	restore_inttab
	call	scr_on
	pop	ax
	mov	dx,ax

	mov	ax,6002h
	cmp	dl,81h		; RM virtual stack overflow
	jz	@@x1

	mov	al,03h
	cmp	dl,82h		; PM virtual stack overflow
	jz	@@x1

	rol	esi,16
	mov	di,si
	shr	esi,16
	mov	al,04h
	cmp	dl,83h		; extended memory check failure
	jz	@@x1

	mov	al,05h
	cmp	dl,84h		; DOS/4G API call failure
	jz	@@x1

;	mov	al,06h
;	cmp	dl,85h		; invalid selector limit failure
;	mov	si,bp
;	jz	@@x1

	mov	al,00h
	movzx	si,byte ptr _int_number
@@x1:	call	report_error
	jmp	@@done



;-----------------------------------------------------------------------------
trap_point:
	mov	eax,dr6			; check if breakpoint on one of DRs
	and	al,0Fh
	jnz	null_protect
	mov	dx,offs excmsg5
	jmp	brkpnt_common
null_protect:
	call	tone
	call	scr_on
	xor	eax,eax
	mov	dr6,eax
	mov	dx,offs nulmsg0
	jmp	brkpnt_common
break_point:
	call	tone
	call	scr_on
	mov	dx,offs excmsg6
brkpnt_common:
	push	dx
	mov	ax,0006h		; get base of application's CS:
	mov	bx,[ebp-04h]
	int	31h
	shl	ecx,16
	mov	cx,dx			; ECX=CS's Base
	mov	eax,[ebp-08h]		; EAX=offset of faulting instruction
	call	check_faultsel		; check if fault occured in app's seg
	pop	dx
	jc	@@1			; if not, show linear address

	push	eax		;******** calculate unrelocated addr
	push	si
	push	offs excmsgB		; unrelocated
	jmp	@@2

@@1:	push	eax
	push	0000h
	push	offs excmsgA		; linear

@@2:	push	dword ptr [ebp-08h]	; show EIP
	push	word ptr [ebp-04h]	; show CS:
	call	prints
	add	sp,14
	call	show_status
	mov	dx,offs excmsg7
	call	prints

@@loop:	mov	bl,09h
	mov	ax,0200h
	int	31h
	push	cx dx
	push	ds
	mov	ds,_sel_ss
	mov	si,STACKSIZE*16+9*4
	mov	dx,ds:[si+0]
	mov	cx,ds:[si+2]
	pop	ds
	mov	ax,0201h
	int	31h
	sti
	xor	ax,ax
	int	16h
	cli
	pop	dx cx
	push	ax
	mov	bl,09h
	mov	ax,0201h
	int	31h
	pop	ax
	and	al,0DFh
	cmp	al,'A'
	jz	@@ab
	cmp	al,'P'
	jz	@@pr
	cmp	al,'T'
	jz	@@tr
	mov	ax,0400h
	mov	cx,0100h
	call	beep
	jmp	@@loop
@@ab:	call	restore_pit
	call	printc
	call	printcr
	mov	al,-1
	jmp	exit386
@@pr:	call	printc
	call	printcr
	cli
	lss	esp,fword ptr _int_esp
	pop	gs fs ss es ds
	popad
	and	byte ptr ss:[esp+9],0FEh
	iretd
@@tr:	call	printc
	call	printcr
	cli
	lss	esp,fword ptr _int_esp
	pop	gs fs ss es ds
	popad
	or	byte ptr ss:[esp+9],001h
	iretd




;=============================================================================
show_status:
	call	@show_status_1
	mov	ax,[ebp-4]
	call	@show_status_2
	mov	ebx,[ebp]			; show EFLAGS
	call	@show_status_3

	lar	ax,[ebp-4]			; check if CS: is accessible
	jnz	@@1
	verr 	word ptr [ebp-4]
	jnz	@@1
	mov	fs,[ebp-4]			; show instructions at CS:EIP
	mov	ebx,[ebp-8]
	call	@show_status_4
	jmp	@@2
@@1:	mov	dx,offs selmsg1			; invalid CS:
	push	word ptr [ebp-4]		; show heximal CS:
	call	prints
	pop	ax
	mov	dx,offs dbgmsgA			; show 'Invalid Selector'
	call	prints
@@2:	call	printcr
	lar	ax,[ebp-46]			; check if SS: is accessible
	jnz	@@3
	mov	es,[ebp-46]			; load ES:ESI with stack ptr
	mov	esi,[ebp-28]
	jmp	@@4
@@3:	mov	es,_sel_ds			; load ES:ESI with dummy ptr
	xor	esi,esi				; that will not trigger errors

@@4:	push	dword ptr es:[esi+0]		; STK[0]
	mov	al,_int_number+1
	push	ax				; Last INT
	push	dword ptr [ebp-36]		; EAX
	push	dword ptr [ebp-12]		; ESI
	mov	dx,offs dbgmsg4
	call	prints
	add	sp,14

	push	dword ptr es:[esi+4]		; STK[4]
	mov	eax,cr0
	push	eax				; CR0
	push	dword ptr [ebp-40]		; EBX
	push	dword ptr [ebp-24]		; EDI
	mov	dx,offs dbgmsg5
	call	prints
	add	sp,16

	push	dword ptr es:[esi+8]		; STK[8]
	mov	eax,cr2
	push	eax				; CR2
	push	dword ptr [ebp-32]		; ECX
	push	dword ptr [ebp-16]		; EBP
	mov	dx,offs dbgmsg6
	call	prints
	add	sp,16

	push	dword ptr es:[esi+12]		; STK[12]
	mov	eax,cr3
	push	eax				; CR3
	push	dword ptr [ebp-28]		; EDX
	push	dword ptr [ebp-20]		; ESP
	mov	dx,offs dbgmsg7
	call	prints
	add	sp,16

	mov	dx,offs selmsg1
	mov	ax,[ebp-4]			; CS: selector
	call	show_selector
	mov	dx,offs selmsg2
	mov	ax,[ebp-42]			; DS: selector
	call	show_selector
	mov	dx,offs selmsg3
	mov	ax,[ebp-44]			; ES: selector
	call	show_selector
	mov	dx,offs selmsg4
	mov	ax,[ebp-46]			; SS: selector
	call	show_selector
	mov	dx,offs selmsg5
	mov	ax,[ebp-48]			; FS: selector
	call	show_selector
	mov	dx,offs selmsg6
	mov	ax,[ebp-50]			; GS: selector
	jmp	show_selector


;-----------------------------------------------------------------------------
show_selector:
	push	ax
	call	prints
	pop	ax
	test	ax,ax				; check if NULL selector
	jnz	@@1
	mov	dx,offs dbgmsgB
	call	prints
	jmp	printcr

@@1:	sub	sp,8				; get selector info
	mov	es,_sel_ss
	mov	edi,esp
	mov	bx,ax
	mov	ax,000Bh
	int	31h
	jnc	@@2				; branch if selector is OK
	add	sp,8				; invalid selector
	mov	dx,offs dbgmsgA
	call	prints
	jmp	printcr

@@2:	mov	ax,word ptr es:[edi+5]		; explain selector
	mov	dx,ax
	push	ax				; Acc

	shr	al,1
	and	ax,07h
	push	ax				; Type

	mov	cl,dh
	shr	cl,6
	and	cl,1
	mov	ax,16
	shl	ax,cl
	push	ax				; Seg: USE16/USE32

	mov	al,dl
	shr	al,3
	and	eax,01h
	lea	eax,selmsg8[eax*4+eax]
	push	ax				; Seg: DATA/CODE

	mov	al,dh
	shr	al,7
	and	eax,01h
	lea	eax,selmsg9[eax+eax*4]
	push	ax				; Granularity

	mov	al,dh
	and	eax,0Fh
	shl	eax,16
	mov	ax,word ptr es:[edi+0]
	push	eax				; Limit
	mov	ah,byte ptr es:[edi+7]
	mov	al,byte ptr es:[edi+4]
	shl	eax,16
	mov	ax,word ptr es:[edi+2]
	push	eax				; Base
	mov	dx,offs selmsg0
	call	prints
	add	sp,26
	ret



;-----------------------------------------------------------------------------
check_faultsel:
	add	eax,ecx			; linear addr = CS_base+EIP_offset
	push	ecx
	mov	ecx,_app_num_objects
	jcxz	@@2
@@1:	cmp	bx,_app_buf_allocsel[ecx*2]
	jz	@@3
	loop	@@1
@@2:	stc
	jmp	@@done
@@3:	sub	eax,_app_buf_allocbase[ecx*4]
	mov	si,cx
	clc
@@done:	pop	ecx
	ret




.8086
;=============================================================================
tone:	push	ax cx
	mov	ax,0500h
	mov	cx,0110h
	call	beep
	mov	ax,0300h
	call	beep
	pop	cx ax
	ret

;=============================================================================
beep:	test	cs:_misc_byte,00000010b
	jz	@@done
	push	cx dx ax		; AX=frequency, CX=time
	mov	al,0B6h			; set frequency
	out	43h,al
	pop	ax
	out	42h,al			; fLow
	mov	al,ah
	out	42h,al			; fHigh
	in	al,61h			; beep on
	or	al,03h
	out	61h,al
@@loop:	in	al,40h
	in	al,40h
	mov	ah,al
@@0:	in	al,40h
	in	al,40h
	cmp	ah,al
	je	@@0
	loop	@@loop
	in	al,61h			; beep off
	and	al,not 03h
	out	61h,al
	pop	dx cx
@@done:	ret


;=============================================================================
scr_on:	push	ax bx cx dx
	test	cs:_misc_byte,01000000b
	jz	@@1
	mov	ax,0003h
	int	10h			; set default videomode
@@1:	mov	dx,03C4h		; turn on VGA screen
	mov	al,01
	out	dx,al
	inc	dx
	in	al,dx
	and	al,0DFh
	out	dx,al
	pop	dx cx bx ax
	ret




.386p
;=============================================================================
_exc00:	push	00h
	jmp	_exc_common
_exc06:	push	06h
	jmp	_exc_common
_exc0D:	push	0Dh
	jmp	_exc_common
_exc0E:	push	0Eh


_exc_common:
	cli
	cld
	pushad				; push regs on application's stack
	mov	ax,ss
	mov	edx,esp
	lss	esp,fword ptr cs:_sel_esp	; switch to internal 16bit stk
	push	ds es fs gs		; now push the segment regs

	mov	es,ax
	movzx	ebx,ax
	lar	ebx,ebx
	test	ebx,00400000h
	mov	ebp,edx
	jnz	@@0
	movzx	ebp,dx
@@0:	mov	ds,cs:_sel_ds

	call	restore_pit
	call	tone
	call	scr_on

	mov	dx,offs errmsg3		; 'DOS/32A run-time:'
	push	6001h			; manually push error code
	call	prints
	pop	ax

	mov	ax,es:[ebp+20h]
	mov	bx,ax
	add	bx,bx
	push	dword ptr es:[ebp+2Eh]	; show EIP
	push	word ptr es:[ebp+32h]	; show CS:
	push	word ptr i_msg[bx]	; int name
	push	ax			; int number
	push	offs excmsgE		; "exception"
	mov	dx,offs excmsg1		; main string
	call	prints
	add	sp,12

	mov	ax,0006h		; get base of application's CS:
	mov	bx,es:[ebp+32h]
	int	31h
	shl	ecx,16
	mov	cx,dx			; ECX=CS's Base
	mov	eax,es:[ebp+2Eh]	; EAX=offset of faulting instruction
	call	check_faultsel		; check if fault occured in app's seg
	jc	@@3			; if not, show linear address

	push	eax		;******** calculate unrelocated addr
	push	si
	mov	dx,offs excmsg3		; 'unrelocated...'
	jmp	@@4

@@3:	push	eax
	push	0
	mov	dx,offs excmsg2		; 'linear...'
@@4:	call	prints
	add	sp,6

	cmp	byte ptr es:[ebp+20h],08h ; if error code present, show it
	jb	@@5
	push	dword ptr es:[ebp+2Ah]	; show error code
	mov	dx,offs excmsg4
	call	prints
	pop	eax
@@5:	call	printcr

	test	_misc_byte2,00010000b
	jz	show_statusx
	mov	dx,offs excmsgC
	push	_process_id
	push	offs start
	call	prints
	pop	eax

show_statusx:
	call	@show_status_1
	mov	ax,es:[ebp+32h]
	call	@show_status_2
	mov	ebx,es:[ebp+36h]		; show EFLAGS
	call	@show_status_3

	lar	ax,es:[ebp+32h]			; check if CS: is accessible
	jnz	@@1
	verr 	word ptr es:[ebp+32h]
	jnz	@@1
	mov	fs,es:[ebp+32h]			; show instructions at CS:EIP
	mov	ebx,es:[ebp+2Eh]
	call	@show_status_4
	jmp	@@2
@@1:	mov	dx,offs selmsg1			; invalid CS:
	push	word ptr es:[ebp+32h]		; show heximal CS:
	call	prints
	pop	ax
	mov	dx,offs dbgmsgA			; show 'Invalid Selector'
	call	prints
@@2:	call	printcr
	lar	ax,es:[ebp+3Eh]			; check if SS: is accessible
	jnz	@@3
	mov	fs,es:[ebp+3Eh]			; load ES:ESI with stack ptr
	mov	esi,es:[ebp+3Ah]
	jmp	@@4
@@3:	mov	fs,_sel_ds			; load ES:ESI with dummy ptr
	xor	esi,esi				; that will not trigger errors

@@4:	push	dword ptr fs:[esi+0]		; STK[0]
	push	0				; Last INT
	push	dword ptr es:[ebp+04h]		; ESI
	push	dword ptr es:[ebp+1Ch]		; EAX
	mov	dx,offs dbgmsg4
	call	prints
	add	sp,14

	push	dword ptr fs:[esi+4]		; STK[4]
	push	large 0				; CR0
	push	dword ptr es:[ebp+00h]		; EDI
	push	dword ptr es:[ebp+10h]		; EBX
	mov	dx,offs dbgmsg5
	call	prints
	add	sp,16

	push	dword ptr fs:[esi+8]		; STK[8]
	push	large 0				; CR2
	push	dword ptr es:[ebp+08h]		; EBP
	push	dword ptr es:[ebp+18h]		; ECX
	mov	dx,offs dbgmsg6
	call	prints
	add	sp,16

	push	dword ptr fs:[esi+12]		; STK[12]
	push	large 0				; CR3
	push	dword ptr es:[ebp+3Ah]		; ESP
	push	dword ptr es:[ebp+14h]		; EDX
	mov	dx,offs dbgmsg7
	call	prints
	add	sp,16

	mov	ax,es
	mov	fs,ax
	mov	dx,offs selmsg1
	mov	ax,fs:[ebp+32h]			; CS: selector
	call	show_selector
	mov	dx,offs selmsg2
	mov	ax,[esp+06h]			; DS: selector
	call	show_selector
	mov	dx,offs selmsg3
	mov	ax,[esp+04h]			; ES: selector
	call	show_selector
	mov	dx,offs selmsg4
	mov	ax,fs:[ebp+3Eh]			; SS: selector
	call	show_selector
	mov	dx,offs selmsg5
	mov	ax,[esp+02h]			; FS: selector
	call	show_selector
	mov	dx,offs selmsg6
	mov	ax,[esp+00h]			; GS: selector
	call	show_selector

	mov	al,-1
	jmp	exit386



;=============================================================================
@show_status_1:
	mov	al,'='
	mov	cx,72
@@0:	call	printc
	loop	@@0
	ret

;-----------------------------------------------------------------------------
@show_status_2:
	cmp	ax,0008h		; note: kernel selector hardcoded!
	mov	dx,offs selmsgX		; exception came from "KERNEL"
	jz	@@0
	cmp	ax,_sel_cs
	mov	dx,offs selmsgY		; exception came from "CLIENT"
	jz	@@0
	cmp	ax,_sel32_cs
	mov	dx,offs selmsgZ		; exception came from "APP/32"
	jz	@@0
	mov	dx,offs selmsgW		; where do all they come from?
@@0:	call	prints
	jmp	printcr


;-----------------------------------------------------------------------------
@show_status_3:
	mov	esi,ebx
	mov	cl,8
	xor	eax,eax
	xor	edx,edx
@@0:	shr	bl,1
	rcr	eax,4
	shr	bh,1
	rcr	edx,4
	loop	@@0
	push	eax
	push	edx
	push	esi
	mov	dx,offs dbgmsg1
	call	prints
	add	sp,12
	ret

;-----------------------------------------------------------------------------
@show_status_4:
	mov	cl,10
@@0:	mov	al,fs:[ebx+9]
	push	ax
	dec	ebx
	loop	@@0
	mov	dx,offs dbgmsg2
	call	prints
	add	sp,10*2
	ret


PopState
