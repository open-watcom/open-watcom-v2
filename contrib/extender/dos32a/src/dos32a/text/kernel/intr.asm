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

; interrupt tables
;=============================================================================
	Align 16
int_matrix:				; INT redirectors
	rept	256			; 256 INTs
	push	ax
	call	near ptr int_main
	endm
;=============================================================================
std_matrix:				; Standard IRQ redirectors
	rept	16			; 16 IRQs
	push	ax
	call	near ptr irq_standard
	endm
;=============================================================================
back_matrix:				; Real mode IRQ callbacks
	rept	16			; 16 IRQs
	push	ax
	call	near ptr irq_callback
	endm
;=============================================================================
exc_matrix:				; Exceptions
	rept	16			; 16 EXCs
	push	ax
	call	near ptr exc_handler
	endm





;=============================================================================
; Out of Real-Mode virtual stack space handler: handles virtual stack
; overflows which may occur during a switch from PM to RM.
;
critical_error_rm:
	cli
	mov	ds,cs:seldata		; DS points to kernel
	mov	es,selzero		; ES points to zero

	mov	eax,oldint1Ch		; restore INT 1Ch
	mov	es:[4*1Ch],eax

	mov	ax,rmstacktop2		; restore RM virtual stack
	mov	rmstacktop,ax		;  to a safe value

	mov	ax,8200h		; AX = kernel error code
	jmp	dptr client_call	; enter client's run-time error handler

;=============================================================================
; Out of Protected-Mode virtual stack space handler: handles virtual stack
; overflows which may occur during a switch from RM to PM.
;
critical_error_pm:
	cli
	xor	ax,ax
	mov	ds,cs:kernel_code	; DS points to kernel
	mov	es,ax			; ES points to zero

	mov	eax,oldint1Ch		; restore INT 1Ch
	mov	es:[4*1Ch],eax

	mov	eax,pmstacktop2		; restore PM virtual stack
	mov	pmstacktop,eax		;  to a safe value

	mov	ebx,eax			; EBX = ESP for protected mode
	mov	ax,SELDATA		; DS selector for protected mode
	mov	cx,ax
	mov	dx,SELZERO		; SS selector = zero selector
	mov	si,SELCODE		; target protected mode CS:EIP
	mov	edi,offs @@0
	jmp	rmtopmswrout		; switch to protected mode

@@0:	mov	ax,8300h		; AX = kernel error code
	jmp	dptr client_call	; enter client's run-time error handler





;=============================================================================
; INT Redirector
;
; Default interrupt handler: reflects interrupts issued in protected mode
; (and for which no user-defined handler is installed) to real-mode.
;
int_main:
	cli				; disable interrupts

	pop	ax			; get caller address in AX
	sub	ax,offs int_matrix+1	; calculate INT number
	shr	ax,2			; now AX = INT ##h

	pushad
	push	ds es fs gs
	mov	ds,cs:seldata

	inc	_pc_intpmtorm		; increment INT PM->RM counter
	mov	@@N,al			; modify code with interrupt number

	mov	dx,rmstacktop		; DX = SS for real mode redirection
	mov	bx,rmstacklen		; get size of real mode stack
	movzx	esi,dx			; EBP -> top of real mode stack
	sub	dx,bx			; adjust DX to next stack location
	shl	esi,4
	cmp	dx,rmstackbase		; exceeded real mode stack space?
	jb	critical_error_rm	; if yes, critical error
	mov	rmstacktop,dx		; update ptr for possible reenterancy
	shl	bx,4			; set real mode SP to top of stack

	mov	es,selzero		; copy registers from protected mode
	mov	ds,selzero		; DS -> 0 (beginning of memory)
	lea	edi,[esi-26h]
	mov	ecx,8
	mov	[esi-2],ss		; store SS:ESP on real mode stack
	mov	[esi-6],esp
	lea	esi,[esp+8]
	cld
	rep	movs dptr es:[edi],ss:[esi]
	mov	ax,[esp+28h]		; move AX to real mode stack frame
	mov	[edi-04h],ax
	mov	si,_KERNEL		; real mode target CS:IP
	mov	di,offs @@0
	sub	bx,26h			; adjust real mode SP for stored vars
	db 66h				; JMP DWORD PTR, as in 32bit offset,
	jmp	wptr cs:pmtormswrout	;  not seg:16bit offset

@@0:	popad				; load regs with int call values
	db 0CDh				; issue INT ##h in real-mode
@@N	db 000h

	pushad				; store registers on stack
	pushf				; store flags on stack
	cli				; disable interrupts
	xor	eax,eax			; EAX = linear ptr to SS
	mov	ebp,eax
	mov	ax,ss
	shl	eax,4
	mov	bp,sp			; EBP = SP
	mov	ebx,[bp+22h]		; get protected mode SS:ESP from stack
	mov	dx,[bp+26h]
	add	ebp,eax			; EBP -> stored regs on stack
	mov	ax,SELZERO		; DS selector value for protected mode
	mov	cx,SELDATA		; ES selector value for protected mode
	mov	si,SELCODE		; target CS:EIP in protected mode
	mov	edi,offs @@1
	jmp	cs:rmtopmswrout		; go back to protected mode

@@1:	inc	es:_pc_intrmtopm	; increment INT RM->PM counter

	mov	ax,es:rmstacklen	; restore top of real mode stack
	add	es:rmstacktop,ax

	mov	ax,ds:[ebp]		; move return FLAGS from real mode
	and	ax,08D5h		;  stack to protected mode stack
	mov	dx,[esp+32h]
	and	dx,not 08D5h
	or	ax,dx
	mov	[esp+32h],ax
	mov	eax,ebp
	mov	edi,[eax+2]		; restore return registers from real
	mov	esi,[eax+6]		;  mode stack
	mov	ebp,[eax+10]
	mov	ebx,[eax+18]
	mov	edx,[eax+22]
	mov	ecx,[eax+26]
	mov	eax,[eax+30]

	pop	gs fs es ds		; restore segment regs
	add	esp,22h			; skip old registers
	iretd				;**no pop AX





;=============================================================================
; irq_tester is called only for interrupts INT 08..0Fh, only when
; master PIC is mapped onto those INTs
;
irq_tester:				; redirection for IRQs mapped on INT 08..0Fh
	mov	al,0Bh			; query master interrupt controller
	out	20h,al
	in	al,20h
	test	al,al
	jz	irq_fail		; if no pending IRQ, jump

	mov	ax,[esp]		; get caller address from stack
	sub	ax,offs int_matrix+1	; calculate INT number
	shr	ax,2			; AX = INT number
	mov	[esp],ax		; save INT number

	and	al,07h			; AX = IRQ number
	bt	cs:irqset_pm,ax		; check if user handler is installed
	pop	ax			; restore INT number
	jnc	irq_down		; if not, proceed

	and	al,07h			; AX = IRQ number
	shl	ax,3
	xchg	ax,bx
	sub	esp,6			; alloc 6 bytes on stack
	mov	[esp+4],ax		; put BX on stack
	mov	ax,wptr cs:irqtab_pm[bx+0]	; get target offset loword(EIP)
	mov	[esp+0],ax
	mov	ax,wptr cs:irqtab_pm[bx+2]	; get target offset hiword(EIP)
	mov	[esp+2],ax
	mov	bx,wptr cs:irqtab_pm[bx+4]	; get target selector CS
	xchg	bx,[esp+4]		; put CS and restore BX
	mov	ax,[esp+6]		; restore AX
	db 66h				; do 32bit far ret to the
	retf				;  appropriate interrupt handler


;=============================================================================
; irq_normal is called for IRQs that do not need to be tested against
; CPU exceptions
;
irq_normal:				; Standard IRQ handler that will send
	pop	ax			; all the IRQs that have not been
	sub	ax,offs int_matrix+1	; hooked in protected mode to real
	shr	ax,2			; mode

;-----------------------------------------------------------------------------
; reflects an IRQ to the real-mode handler
;
irq_down:
	pushad
	push	ds es fs gs
	mov	ds,cs:seldata

	inc	_pc_irqpmtorm		; increment IRQ PM->RM counter

	movzx	eax,al			; EAX = interrupt number

	mov	dx,rmstacktop		; DX = SS for real mode redirection
	mov	bx,rmstacklen		; get size of real mode stack
	movzx	esi,dx			; ESI -> top of real mode stack
	sub	dx,bx			; adjust DX to next stack location
	shl	esi,4
	cmp	dx,rmstackbase		; exceeded real mode stack space?
	jb	critical_error_rm	; if yes, critical error
	mov	rmstacktop,dx		; update ptr for possible reenterancy
	shl	bx,4			; set real mode SP to top of stack

	mov	ds,selzero		; DS -> 0 (beginning of memory)
	mov	edi,[eax*4]		; get real mode interrupt CS:IP
	mov	[esi-2],ss		; store SS: on real mode stack
	mov	[esi-6],esp		; store ESP on real mode stack
	mov	dptr [esi-10],_KERNEL	; set target FLAGS and CS on RM stack
	mov	wptr [esi-12],offs @irq	; set target IP on RM stack
	shld	esi,edi,16
	sub	bx,12			; adjust real mode SP for stored vars
	db 66h				; JMP DWORD PTR, as in 32bit offset,
	jmp	wptr cs:pmtormswrout	;  not seg:16bit offset





;=============================================================================
; Exception Handler
;
; We get here when:
;	+ an interrupt in range INT 00..0Fh occurs
;	+ irq_tester function determines that there is no pending IRQ
;	  in the interrupt controller
;
irq_fail:
	pop	ax			; get call address
	sub	ax,offs int_matrix+1	; calculate INT ##
	shr	ax,2
;
; invoke a user-defined exception handler
;
	cmp	al,08h			; exceptions 08h, 0A..0Eh push error codes
	jb	@@2
	cmp	al,09h
	je	@@2
	cmp	al,0Eh
	ja	@@2
;
; rearrange stack frame: have exception code
;
@@1:	sub	esp,16h
	mov	[esp+08h],eax		; save hiword(EAX)

	movzx	eax,ax
	mov	eax,cs:exctab_pm[eax*8+0]
	mov	[esp+00h],eax		; target EIP

	movzx	eax,wptr [esp+08h]
	mov	eax,cs:exctab_pm[eax*8+4]
	mov	[esp+04h],eax		; target CS

	mov	ax,[esp+16h]		; restore AX
	mov	[esp+08h],ax		; save loword(EAX)

	mov	ax,cs
	mov	[esp+0Ch],eax		; ret CS

	mov	eax,[esp+18h]
	mov	[esp+10h],eax		; EC
	mov	eax,[esp+1Ch]
	mov	[esp+14h],eax		; EIP
	mov	eax,[esp+20h]
	mov	[esp+18h],eax		; CS
	mov	eax,[esp+24h]
	mov	[esp+1Ch],eax		; EFL

	jmp	@@run
;
; rearrange stack frame: no exception code
;
@@2:	sub	esp,1Ah
	mov	[esp+08h],eax		; save hiword(EAX)

	movzx	eax,ax
	mov	eax,cs:exctab_pm[eax*8+0]
	mov	[esp+00h],eax		; target EIP

	movzx	eax,wptr [esp+08h]
	mov	eax,cs:exctab_pm[eax*8+4]
	mov	[esp+04h],eax		; target CS

	mov	ax,[esp+1Ah]		; restore AX
	mov	[esp+08h],ax		; save loword(EAX)

	mov	ax,cs
	mov	[esp+0Ch],eax		; ret CS

	xor	eax,eax
	mov	[esp+10h],eax		; EC
	mov	eax,[esp+1Ch]
	mov	[esp+14h],eax		; EIP
	mov	eax,[esp+20h]
	mov	[esp+18h],eax		; CS
	mov	eax,[esp+24h]
	mov	[esp+1Ch],eax		; EFL

@@run:	lea	eax,[esp+28h]
	mov	[esp+20h],eax		; ESP

	mov	ax,ss
	movzx	eax,ax
	mov	[esp+24h],eax		; SS

	mov	ax,offs @@ret		; ret EIP
	xchg	eax,[esp+08h]		; restore EAX

	db 66h				; do 32bit far ret to the
	retf				; appropriate exception handler


; exception handler epilogue
; note:	we don't reload the stack but simply ignore SS:ESP values on eh's stack frame
@@ret:	mov	[esp],eax
	mov	eax,[esp+0Ch]
	mov	[esp+14h],eax
	mov	eax,[esp+08h]
	mov	[esp+10h],eax
	mov	eax,[esp+04h]
	mov	[esp+0Ch],eax
	mov	eax,[esp]
	add	esp,0Ch
	iretd


;=============================================================================
; Invoke built-in exception handler
;
exc_handler:
	pop	ax
	sub	ax,offs exc_matrix+1
	shr	ax,2

	mov	ah,81h			; kernel run-time error 81h
	jmp	dptr cs:client_call	; jump to built-in exception handler






;=============================================================================
; Standard IRQ Handler
;
; This is a built-in IRQ handler. It is returned by INT 31h, AX=0204h
; for BL values identifying IRQs for which no user-defined handler had been
; installed. When a user-defined IRQ handler chains, the call goes here.
;
irq_standard:				; Standard IRQ handler that will send
	cli
	pop	ax			; by default all the IRQs from
	sub	ax,offs std_matrix+1	; protected mode to real mode
	shr	ax,2

	pushad
	push	ds es fs gs
	mov	ds,cs:seldata

	inc	_pc_irqpmtorm		; increment IRQ PM->RM counter

	movzx	eax,al			; EAX = IRQ number

	mov	dx,rmstacktop		; DX = SS for real mode redirection
	mov	bx,rmstacklen		; get size of real mode stack
	movzx	esi,dx			; ESI -> top of real mode stack
	sub	dx,bx			; adjust DX to next stack location
	shl	esi,4
	cmp	dx,rmstackbase		; exceeded real mode stack space?
	jb	critical_error_rm	; if yes, critical error
	mov	rmstacktop,dx		; update ptr for possible reenterancy
	shl	bx,4			; set real mode SP to top of stack

	mov	edi,irqtab_rm[eax*4]	; get real mode interrupt CS:IP
	mov	ds,selzero		; DS -> 0 (beginning of memory)
	mov	[esi-2],ss		; store SS: on real mode stack
	mov	[esi-6],esp		; store ESP on real mode stack
	mov	dptr [esi-10],_KERNEL	; set target FLAGS and CS on RM stack
	mov	wptr [esi-12],offs @irq	; set target IP on RM stack
	shld	esi,edi,16
	sub	bx,12			; adjust real mode SP for stored vars
	db 66h				; JMP DWORD PTR, as in 32bit offset,
	jmp	wptr cs:pmtormswrout	;  not seg:16bit offset

@irq:	cli
	mov	ax,SELDATA		; DS selector value for protected mode
	mov	cx,ax			; ES selector value for protected mode
	pop	ebx			; get protected mode SS:ESP from stack
	pop	dx
	mov	si,SELCODE		; target CS:EIP in protected mode
	mov	edi,offs @@2
	jmp	cs:rmtopmswrout		; go back to protected mode

@@2:	inc	_pc_irqrmtopm		; increment IRQ RM->PM counter

	mov	ax,rmstacklen		; restore top of real mode stack
	add	rmstacktop,ax

	pop	gs fs es ds		; restore all registers
	popad
	pop	ax			; restore original AX
	iretd








; IRQ Callback
;
; Used by IRQs in real mode to process protected mode IRQ handlers.
;
;=============================================================================
irq_callback:
	cli
	pop	ax
	sub	ax,offs back_matrix+1
	shr	ax,2			; AX = IRQ number

	pushad
	push	ds es fs gs
	mov	ds,cs:kernel_code

	inc	_pc_irqcbrmtopm		; increment IRQCallback RM->PM counter
	mov	temp_int,al

	mov	edx,pmstacktop		; EDX = ESP for protected mode
	mov	ebx,edx
	sub	edx,pmstacklen
	cmp	edx,pmstackbase		; exceeded protected mode stack space?
	jb	critical_error_pm	; if yes, critical error
	mov	pmstacktop,edx		; update ptr for possible reenterancy

	mov	bp,ss			; save real mode SS:SP in EBP
	shl	ebp,16
	mov	bp,sp
	mov	si,irqcallbackptr	; save ESP across mode switches
	mov	dptr @callback_data[si+100h],esp
	add	irqcallbackptr,4

	mov	ax,SELCODE		; prot. mode DS
	mov	cx,SELZERO		; prot. mode ES
	mov	dx,cx			; prot. mode SS
	mov	si,ax			; prot. mode CS
	mov	edi,offs @@0		; prot. mode EIP
	jmp	rmtopmswrout		; switch to protected mode

@@0:	movzx	bx,temp_int
	shl	bx,3			; BX = pointer to interrupt ##h
	pushfd				; set return eflags
	push	large SELCODE		; set return CS on PM stack
	push	large offs @@1		; set return EIP on PM stack
	db 66h
	jmp	dptr irqtab_pm[bx]	; go to prot. mode interrupt handler

@@1:	mov	ax,_KERNEL		; AX = real mode DS
	mov	si,ax			; SI = real mode CS
	mov	di,offs @@2		; DI = real mode IP
	mov	bx,bp			; BX = real mode SP
	shr	ebp,16
	mov	dx,bp			; DX = real mode SS
	db 66h
	jmp	wptr cs:pmtormswrout	; switch to real mode

@@2:	inc	_pc_irqcbpmtorm		; increment IRQCallback PM->RM counter

	mov	eax,pmstacklen
	add	pmstacktop,eax

	sub	irqcallbackptr,4	; restore original ESP
	mov	si,irqcallbackptr
	mov	esp,dptr @callback_data[si+100h]

	pop	gs fs es ds
	popad
	pop	ax
	iret				; return from IRQ callback








;=============================================================================
; Real mode callback actual code:
;
;##	pushad
;##	push	0			; if 0, callback is free
;##	push	large 0
;##	mov	cx,0			; load CX with callers ES
;##	push	large
;##	jmp	far ptr ?:?
;
callback:				; real mode callback handler
	mov	ax,sp			; preserve SS:SP for callback
	push	ss
	push	ax
	push	gs fs ds es		; preserve real mode regs for callback
	pushf				; preserve FLAGS for callback
	cli
	push	cs
	pop	ds

	inc	_pc_cbrmtopm		; increment Callback RM->PM counter

	mov	ebp,pmstacktop		; EBP = ESP for protected mode
	mov	ebx,ebp			; set EBX to next stack location
	sub	ebx,pmstacklen
	mov	pmstacktop,ebx		; update ptr for possible reenterancy
	cmp	ebx,pmstackbase		; exceeded protected mode stack space?
	jb	critical_error_pm	; if yes, critical error

	xor	eax,eax			; EAX = base address of SS
	mov	ebx,eax
	mov	ax,ss
	shl	eax,4
	mov	bx,sp			; EBX = current linear SS:SP
	add	ebx,eax

	mov	es,ds:gdtseg		; set for protected mode callback DS
	or	eax,92000000h		;  base address in GDT
	mov	es:[SELCALLBACK+2],eax
	mov	ax,SELZERO		; DS selector for protected mode
	mov	dx,ax			; SS selector = DS selector
	mov	si,SELCODE		; target protected mode CS:EIP
	mov	edi,offs @@0
	jmp	rmtopmswrout		; go to protected mode

@@0:	mov	edi,[esp+14]		; EDI -> register structure from stack
	lea	esi,[esp+24]		; copy general registers from stack
	mov	ecx,8			;  to register structure
	cld
	rep	movs dptr es:[edi],ds:[esi]

	mov	esi,esp			; copy FLAGS, ES, DS, FG, and GS
	movs	wptr es:[edi],ds:[esi]
	movs	dptr es:[edi],ds:[esi]
	movs	dptr es:[edi],ds:[esi]
	lods	dptr ds:[esi]		; EAX = real mode SS:SP from stack
	add	ax,42			; adjust SP for stuff on stack
	mov	es:[edi+4],eax		; put in register structure
	mov	ds,cs:selcallback	; DS = callback DS selector
	sub	edi,42			; EDI -> register structure
	movzx	esi,ax			; ESI = old real mode SP
	xchg	esp,ebp			; ESP = protected mode stack
	pushfd				; push flags for IRETD from callback
	push	large cs		; push 32bit CS for IRETD
	push	large offs @@1		; push 32bit EIP for IRETD
	movzx	eax,wptr [ebp+22]	; EAX = target CS of callback
	push	eax			; push 32bit CS for RETF to callback
	push	dptr [ebp+18]		; push 32bit EIP for retf
	db 66h				; 32bit RETF to callback
	retf

@@1:	cli
	push	es			; DS:ESI = register structure
	pop	ds
	mov	esi,edi
	mov	es,cs:selzero		; ES -> 0 (beginning of memory)
	movzx	ebx,wptr [esi+2Eh]	; EBX = real mode SP from structure
	movzx	edx,wptr [esi+30h]	; EDX = real mode SS from structure
	sub	bx,42			; subtract size of vars to be put
	mov	ebp,[esi+0Ch]		; EBP = pushed ESP from real mode
	mov	bp,bx			; EBP = old high & new low word of ESP
	lea	edi,[edx*4]		; EDI -> real mode base of stack
	lea	edi,[edi*4+ebx]		;  of vars to be stored
	mov	ecx,8			; copy general registers to stack
	cld
	rep	movs dptr es:[edi],ds:[esi]
	mov	eax,[esi+6]		; EAX = return FS and GS for real mode
	mov	es:[edi],eax		; store on real mode stack for return
	mov	eax,[esi+10]		; EAX = return CS:IP for real mode
	mov	es:[edi+4],eax		; store on real mode stack for return
	mov	ax,[esi]		; AX = return FLAGS for real mode
	mov	es:[edi+8],ax		; store on real mode stack for return
	mov	ax,[esi+4]		; AX = return DS for real mode
	mov	cx,[esi+2]		; CX = return ES for real mode
	mov	si,_KERNEL		; real mode target CS:IP
	mov	di,offs @@2
	db 66h				; JMP DWORD PTR, as in 32bit offset,
	jmp	wptr cs:pmtormswrout	;  not seg:16bit offset

@@2:	inc	cs:_pc_cbpmtorm		; increment Callback PM->RM counter

	mov	esp,ebp			; restore total ESP, old high word
	mov	eax,cs:pmstacklen	; restore top of protected mode stack
	add	cs:pmstacktop,eax

	popad				; get callback return general regs
	pop	fs gs			; get callback return FS and GS values
	iret				; go to callback return CS:IP

