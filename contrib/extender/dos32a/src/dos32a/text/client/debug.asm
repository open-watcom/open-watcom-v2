;
; Copyright (C) 1996-2006 by Narech K. All rights reserved.
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
; IN:	AL = interrupt number (if applicable)
;	AH = kernel error code:
;
;		81h = unhandled interrupt/exception
;		82h = out of real-mode virtual stacks
;		83h = out of protected-mode virtual stacks
;		84h = extended memory blocks have been corrupted
;		85h = DOS/4G API calls not supported
;
;=============================================================================
critical_handler:
	cli				; immediately disable interrupts
	cld				; clear direction flag
	mov	ds,cs:_sel_ds		; restore client DS
	lss	esp,fptr _sel_esp	; restore client SS:ESP

	push	ax			; push error code
	mov	al,20h			; send EOI codes to PICs
	out	0A0h,al
	out	020h,al

	mov	ax,_pic_mask		; restore PIC masks
	out	0A1h,al
	mov	al,ah
	out	021h,al

	call	restore_pit
	call	restore_inttab
	call	tone
	call	scr_on
	pop	dx			; DH = code, DL = int number

	mov	ax,6001h		; error 6001: "unexpected interrupt"
	cmp	dh,81h
	jz	@@arg_dl

	mov	al,02h			; error 6002: "out of real-mode virtual stacks"
	cmp	dh,82h
	jz	@@report

	mov	al,03h			; error 6003: "out of protected-mode virtual stacks"
	cmp	dh,83h
	jz	@@report

	mov	al,04h			; error 6004: "extended memory blocks have been corrupted"
	cmp	dh,84h
	jz	@@arg_esi

	mov	al,05h			; error 6005: "DOS/4G API calls not supported"
	cmp	dh,85h
	jz	@@report

	mov	al,00h			; error 6000: "unknown error code"

@@arg_dl:
	movzx	si,dl			; load SI with DL
	jmp	@@report

@@arg_esi:
	rol	esi,16			; convert ESI to DI:SI
	mov	di,si
	shr	esi,16

@@report:
	call	report_error
	mov	al,0FFh			; exit with error code 0FFh
	jmp	exit386




;=============================================================================
eh00:	push	00h
	jmp	eh_common
eh01:	push	01h
	jmp	eh_debug
eh02:	push	02h
	jmp	eh_common
eh03:	push	03h
	jmp	eh_debug
eh04:	push	04h
	jmp	eh_common
eh05:	push	05h
	jmp	eh_common
eh06:	push	06h
	jmp	eh_common
eh07:	push	07h
	jmp	eh_common
eh08:	push	08h
	jmp	eh_common
eh09:	push	09h
	jmp	eh_common
eh0A:	push	0Ah
	jmp	eh_common
eh0B:	push	0Bh
	jmp	eh_common
eh0C:	push	0Ch
	jmp	eh_common
eh0D:	push	0Dh
	jmp	eh_common
eh0E:	push	0Eh
	jmp	eh_common


;=============================================================================
; Machine state at the point of exception:
;
;
; Stack Frame relative to ms_frame_ ss:esp
;
;	00:SS	+1Ch
;	 ESP	+18h
;	 EFL	+14h
;	00:CS	+10h
;	 EIP	+0Ch
;	 ec	+08h
; Ret:	00:CS	+04h
; Ret:	 EIP	+00h
;
;--------------------
; Stack Frame relative to ss:esp at the point of eh_reporter
;
;	DS	+26h
;	ES	+24h
;	FS	+22h
;	GS	+20h
;	EAX	+1Ch
;	ECX	+18h
;	EDX	+14h
;	EBX	+10h
;	ESP	+0Ch
;	EBP	+08h
;	ESI	+04h
;	EDI	+00h
;
eh_debug:
	cmp	cs:_sys_type,3
	jz	@@0

	push	eax
	mov	eax,dr6				; check if breakpoint on one of DRs
	and	al,0Fh
	pop	eax
	jnz	eh_common			; if yes, jump

@@0:	add	esp,2				; pop INT #
	and	bptr [esp+15h],0FEh		; reset trap flag
	db 66h					; do 32bit far ret
	retf


;=============================================================================
;eh_debug_continue:
;	popad
;	pop	gs fs es ds
;	lss	esp,fptr cs:ms_frame_esp	; restore app's SS:ESP
;	and	bptr [esp+15h],0FEh		; reset trap flag
;	db 66h					; do 32bit far ret
;	retf

;=============================================================================
;eh_debug_trap:
;	popad
;	pop	gs fs es ds
;	lss	esp,fptr cs:ms_frame_esp	; restore app's SS:ESP
;	or	bptr [esp+15h],01h		; set trap flag
;	db 66h					; do 32bit far ret
;	retf


;=============================================================================
eh_common:
	cli
	cld

	push	ax				; save AX
	mov	ax,ds				; AX = app's DS
	mov	ds,cs:_sel_ds			; load DS with client's data selector
	mov	ms_reg_ds,ax			; machine state: save DS
	pop	ax				; restore AX

	pop	ms_int				; machine state: save INT #

	mov	ms_frame_ss,ss			; machine state: save eh frame SS
	mov	ms_frame_esp,esp		; machine state: save eh frame ESP

	lss	esp,fptr _sel_esp		; switch to internal stack

	push	ms_reg_ds
	push	es
	push	fs
	push	gs
	pushad

	mov	es,ms_frame_ss
	mov	ebp,ms_frame_esp

	mov	eax,es:[ebp+08h]
	mov	ms_ec,eax			; machine state: save error code
	mov	eax,es:[ebp+14h]
	mov	ms_eflags,eax			; machine state: save EFLAGS
;
; make FS:ESI = app's CS:EIP
; make GS:EDI = app's SS:ESP
;
	mov	ax,es:[ebp+10h]			; load app CS
	call	check_selector			; validate app's CS
	mov	fs,ax				; FS = app CS

	mov	ax,es:[ebp+1Ch]			; load app SS
	call	check_selector			; validate app's SS
	mov	gs,ax				; GS = app SS

	mov	esi,es:[ebp+0Ch]		; load app EIP
	mov	edi,es:[ebp+18h]		; load app ESP
;
; copy CRx regs
;
	xor	eax,eax
	mov	ms_reg_cr0,eax			; machine state: reset CR0
	mov	ms_reg_cr2,eax			; machine state: reset CR2
	mov	ms_reg_cr3,eax			; machine state: reset CR3
	mov	ms_reg_dr6,eax			; machine state: reset DR6

	cmp	_sys_type,3
	jz	@@1
	mov	eax,cr0
	mov	ms_reg_cr0,eax			; machine state: save CR0
	mov	eax,cr2
	mov	ms_reg_cr2,eax			; machine state: save CR2
	mov	eax,cr3
	mov	ms_reg_cr3,eax			; machine state: save CR3
	mov	eax,dr6
	mov	ms_reg_dr6,eax			; machine state: save DR6

@@1:	mov	ebp,esp

	mov	al,20h				; send EOI to PICs
	out	0A0h,al
	out	020h,al

	mov	ax,_pic_mask			; restore PIC masks
	out	0A1h,al
	mov	al,ah
	out	021h,al

eh_reporter:
	call	restore_pit
	call	restore_inttab
	call	tone
	call	scr_on

	call	dump_description
	call	dump_description2

	push	_process_id
	push	offs start
	mov	dx,offs excmsgC
	call	prints
	add	sp,4

	call	dump_origin			; dump origin of exception
	call	dump_eflags			; dump EFLAGS
	call	dump_opcodes			; dump op-codes @ CS:EIP

	call	dump_regs			; dump regs, CRs, stack
	call	dump_selectors			; dump selectors

	mov	al,-1
	jmp	exit386





;=============================================================================
dump_description:
	mov	eax,ms_reg_dr6			; check if this is a breakpoint on one of DRs
	and	al,0Fh
	jz	@@0				; if not, jump

	call	check_faultsel			; check if CS is known code selector
	push	esi				; show EIP
	push	fs				; show CS
	mov	dx,offs nulmsg0			; "Null-pointer protection..."
	call	prints
	add	sp,6
	ret

@@0:	push	6001h				; manually push error code
	mov	dx,offs errmsg3			; 'DOS/32A run-time:'
	call	prints
	pop	ax				; remove arg from stack

	mov	ebx,-2				; FIXME: can crash
	call	loadl_cs_eip			; load 4 bytes at CS:[EIP-2]

	mov	bl,0CDh				; check for INT # opcode
	mov	bh,bptr ms_int
	cmp	ax,bx

	mov	cx,offs excmsgE			; "exception"
	jnz	@@1
	mov	cx,offs excmsgI			; "unexpected interrupt"

@@1:	mov	ax,ms_int			; machine state: get INT #
	mov	bx,ax
	add	bx,bx

	push	esi				; show EIP
	push	fs				; show CS
	push	wptr i_msg[bx]			; int name
	push	ax				; int number
	push	cx				; "exception"
	mov	dx,offs excmsg1			; main string
	call	prints
	add	sp,12

	ret


;=============================================================================
dump_description2:
	call	check_faultsel			; check if CS is known code selector
	push	eax				; addr
	push	cx				; object #
	mov	ax,offs excmsgA			; "Linear"
	jc	@@1
	mov	ax,offs excmsgB			; "Unrelocated"
@@1:	push	ax
	mov	dx,offs excmsg2			; "...crash address..."
	call	prints
	add	sp,8

	mov	ax,ms_int
	cmp	al,08h				; if error code present, show it
	jb	@@3
	cmp	al,09h
	je	@@2
	cmp	al,0Eh
	ja	@@3

@@2:	push	ms_ec				; display error code
	mov	dx,offs excmsg4
	call	prints
	add	sp,4

@@3:	call	printcr

	ret


;=============================================================================
; In:	FS = app's CS
;
dump_origin:
	mov	al,'='
	mov	cx,72
@@0:	call	printc
	loop	@@0

	mov	ax,fs
	cmp	ax,0008h		; note: kernel selector hardcoded!
	mov	dx,offs selmsgX		; exception came from "KERNEL"
	jz	@@1
	cmp	ax,_sel_cs
	mov	dx,offs selmsgY		; exception came from "CLIENT"
	jz	@@1
	cmp	ax,_sel32_cs
	mov	dx,offs selmsgZ		; exception came from "APP/32"
	jz	@@1
	mov	dx,offs selmsgW		; where do all they come from?
@@1:	call	prints
	jmp	printcr


;=============================================================================
dump_eflags:
	mov	cl,8
	mov	ebx,ms_eflags
	xor	eax,eax
	xor	edx,edx
@@0:	shr	bl,1
	rcr	eax,4
	shr	bh,1
	rcr	edx,4
	loop	@@0
	push	eax
	push	edx
	push	ms_eflags
	mov	dx,offs dbgmsg1
	call	prints
	add	sp,12
	ret


;=============================================================================
; In:	FS:ESI = app's CS:EIP
;
dump_opcodes:
	mov	cl,10
	mov	ebx,9
@@0:	call	loadb_cs_eip
	push	ax
	dec	ebx
	loop	@@0
	mov	dx,offs dbgmsg2
	call	prints
	add	sp,10*2
	jmp	printcr


;=============================================================================
; In:	SS:EBP = local stack frame
;	FS:ESI = app's CS:EIP
;	GS:EDI = app's SS:ESP
;
dump_regs:
	xor	ebx,ebx
	call	loadl_ss_esp
	push	eax				; STK[0]
	push	ms_reg_dr6			; DR6
	push	dptr [ebp+04h]			; ESI
	push	dptr [ebp+1Ch]			; EAX
	mov	dx,offs dbgmsg4
	call	prints
	add	sp,16

	add	bx,4
	call	loadl_ss_esp
	push	eax				; STK[4]
	push	ms_reg_cr0			; CR0
	push	dptr [ebp+00h]			; EDI
	push	dptr [ebp+10h]			; EBX
	mov	dx,offs dbgmsg5
	call	prints
	add	sp,16

	add	bx,4
	call	loadl_ss_esp
	push	eax				; STK[8]
	push	ms_reg_cr2			; CR2
	push	dptr [ebp+08h]			; EBP
	push	dptr [ebp+18h]			; ECX
	mov	dx,offs dbgmsg6
	call	prints
	add	sp,16

	add	bx,4
	call	loadl_ss_esp
	push	eax				; STK[12]
	push	ms_reg_cr3			; CR3
	push	edi				; ESP
	push	dptr [ebp+14h]			; EDX
	mov	dx,offs dbgmsg7
	call	prints
	add	sp,16

	ret


;=============================================================================
; In:	SS:EBP = local stack frame
;	FS:ESI = app's CS:EIP
;	GS:EDI = app's SS:ESP
;
dump_selectors:
	mov	dx,offs selmsg1
	mov	ax,fs				; CS selector
	call	describe_selector

	mov	dx,offs selmsg2
	mov	ax,[ebp+26h]			; DS selector
	call	describe_selector

	mov	dx,offs selmsg3
	mov	ax,[ebp+24h]			; ES selector
	call	describe_selector

	mov	dx,offs selmsg4
	mov	ax,gs				; SS selector
	call	describe_selector

	mov	dx,offs selmsg5
	mov	ax,[ebp+22h]			; FS selector
	call	describe_selector

	mov	dx,offs selmsg6
	mov	ax,[ebp+20h]			; GS selector
	call	describe_selector

	ret





;=============================================================================
; In:	AX = selector
;
describe_selector:
	push	ax
	call	prints
	pop	ax
	test	ax,ax				; check if NULL selector
	jnz	@@1
	mov	dx,offs dbgmsgB
	call	prints
	jmp	printcr

@@1:	mov	bx,ax
	sub	sp,8				; get selector info
	push	ss
	pop	es
	mov	edi,esp
	mov	ax,000Bh
	int	31h
	jnc	@@2				; branch if selector is OK
	add	sp,8				; invalid selector
	mov	dx,offs dbgmsgA
	call	prints
	jmp	printcr

@@2:	mov	ax,wptr es:[edi+5]		; describe selector
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
	mov	ax,wptr es:[edi+0]
	push	eax				; Limit
	mov	ah,bptr es:[edi+7]
	mov	al,bptr es:[edi+4]
	shl	eax,16
	mov	ax,wptr es:[edi+2]
	push	eax				; Base
	mov	dx,offs selmsg0
	call	prints
	add	sp,26
	ret


;=============================================================================
; In:	FS:ESI = app CS:EIP
;
; Out: 	if FS = known selector:
;		CF clear
;		CX:EAX = N:unrelocated_addr
;	else
;		CF set
;		CX:EAX = 0:linear_addr
check_faultsel:
	mov	ax,6				; get base of app's CS
	mov	bx,fs
	int	31h
	shl	ecx,16
	mov	cx,dx				; ECX = base of app's CS
	lea	eax,[ecx+esi]			; EAX = linear addr (eg CS.base + EIP)
	mov	ecx,_app_num_objects
	jcxz	@@err
@@0:	cmp	bx,_app_buf_allocsel[ecx*2]
	jz	@@ok
	loop	@@0
@@err:	stc
	ret
@@ok:	sub	eax,_app_buf_allocbase[ecx*4]	; EAX = unrelocated addr
	clc
	ret


;=============================================================================
; In:	AX = selector
;
; Out:	if valid selector AX.out == AX.in
;	else AX = 0 (Null selector)
;
check_selector:
	lar	bx,ax			; get access rights
	jnz	@@0			; if sel not a valid selector, jump
	verr	ax			; verify selector read access
	jnz	@@0			; if non-readable, jump
	not	bx			; invert Present bit
	test	bh,80h			; test Present bit
	jnz	@@0			; if segment not present, jump
	ret
@@0:	xor	ax,ax			; return Null selector
	ret


;=============================================================================
; In:	FS:ESI = app's CS:EIP
;	EBX = offset
;
; Out:	AL
;
loadb_cs_eip:
	mov	ax,fs
	test	ax,ax
	jnz	@@0
	xor	eax,eax
	ret
@@0:	movzx	eax,bptr fs:[esi+ebx]
	ret

;=============================================================================
; In:	FS:ESI = app's CS:EIP
;	EBX = offset
;
; Out:	EAX
;
loadl_cs_eip:
	mov	ax,fs
	test	ax,ax
	jnz	@@0
	xor	eax,eax
	ret
@@0:	mov	eax,fs:[esi+ebx]
	ret

;=============================================================================
; In:	GS:EDI = app's SS:ESP
;	EBX = offset
;
; Out:	EAX
;
loadl_ss_esp:
	mov	ax,gs
	test	ax,ax
	jnz	@@0
	xor	eax,eax
	ret
@@0:	mov	eax,gs:[edi+ebx]
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
	jnz	@@0
	ret

@@0:	push	cx dx ax		; AX=frequency, CX=time
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
@@1:	in	al,40h
	in	al,40h
	cmp	ah,al
	je	@@1
	loop	@@loop
	in	al,61h			; beep off
	and	al,not 03h
	out	61h,al
	pop	dx cx
	ret

;=============================================================================
scr_on:	push	ax bx cx dx
	test	cs:_misc_byte,01000000b
	jz	@@0

	mov	ax,0003h
	int	10h			; set default videomode

@@0:	mov	dx,03C4h		; turn on VGA screen
	mov	al,01
	out	dx,al
	inc	dx
	in	al,dx
	and	al,0DFh
	out	dx,al
	pop	dx cx bx ax
	ret


PopState

