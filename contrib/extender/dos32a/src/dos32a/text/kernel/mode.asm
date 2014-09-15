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

;=============================================================================
; Mode Switching Routines
;=============================================================================

;=============================================================================
; VCPI real mode to protected mode switch
;
;	AX = protected mode DS
;	CX = protected mode ES
;	DX = protected mode SS
;	EBX = protected mode stack pointer
;	ESI = protected mode target CS
;	EDI = protected mode target IP
;
	Align 4
v_rmtopmsw:
	pushf				; store FLAGS
	cli
	mov	ds,cs:kernel_code	; DS = _KERNEL
	pop	tempw0			; move FLAGS from stack to temp
	mov	tempw1,ax		; store AX (protected mode DS)
	mov	tempw2,si		; store SI (protected mode CS)
	mov	esi,vcpistrucaddx	; ESI = linear addx of VCPI structure
	mov	ax,0DE0Ch		; VCPI switch to protected mode
	int	67h
v_rmtopmswpm:
	mov	ss,dx			; load protected mode SS:ESP
	mov	esp,ebx
	mov	ds,cs:tempw1		; load protected mode DS
	mov	es,cx			; load protected mode ES
	xor	ax,ax
	mov	fs,ax			; load protected mode FS with NULL
	mov	gs,ax			; load protected mode GS with NULL
	pushfd				; store EFLAGS
	mov	ax,cs:tempw0		; move bits 0-11 of old FLAGS onto
	and	ah,0Fh			;  stack for IRETD
	mov	[esp],ax
	push	cs:tempd1		; store protected mode target CS
	push	edi			; store protected mode target EIP
	iretd				; go to targed addx in protected mode

;=============================================================================
; VCPI protected mode to real mode switch
;
;	AX = real mode DS
;	CX = real mode ES
;	DX = real mode SS
;	BX = real mode stack pointer
;	SI = real mode target CS
;	DI = real mode target IP
;
	Align 4
v_pmtormsw:
	pushf				; store FLAGS
	cli
	push	ax			; store AX (real mode DS)
	mov	ds,cs:selzero		; DS -> 0 (beginning of memory)
	movzx	ebx,bx			; clear high word of EBX, real mode SP
	mov	eax,cs:vcpiswitchstack	; EAX -> top of temporary switch stack
	movzx	edx,dx			; clear high word of EDX, real mode SS
	mov	dptr ds:[eax+32],0	; store real mode GS
	movzx	ecx,cx			; clear high word of ECX, real mode ES
	mov	dptr ds:[eax+28],0	; store real mode FS
	mov	ds:[eax+20],ecx		; store real mode ES
	pop	cx			; move real mode DS from protected
	mov	ds:[eax+24],ecx		;  mode stack to VCPI call stack
	mov	ds:[eax+16],edx		; store real mode SS
	mov	ds:[eax+12],ebx		; store real mode SP
	mov	dptr ds:[eax+4],_KERNEL	; store real mode CS
	mov	dptr ds:[eax],offs @@0	; store real mode IP
	pop	bx			; restore FLAGS from stack
	mov	ss,cs:selzero		; SS -> 0 (beginning of memory)
	mov	esp,eax			; ESP = stack ptr for VCPI call
	mov	ax,0DE0Ch		; VCPI switch to real mode (V86)
	call	fptr cs:vcpi_calleip
@@0:	push	bx			; store old FLAGS
	push	si 			; store target CS in real mode
	push	di 			; store target IP in real mode
	iret				; go to target addx in real mode


;=============================================================================
; XMS/RAW real mode to protected mode switch
;
;	AX = protected mode DS
;	CX = protected mode ES
;	DX = protected mode SS
;	EBX = protected mode stack pointer
;	ESI = protected mode target CS
;	EDI = protected mode target IP
;
	Align 4
xr_rmtopmsw:
	pushfd				; store EFLAGS
	cli
	push	ax			; store AX (protected mode DS)
	lidt	fptr cs:idtlimit	; load protected mode IDT
	lgdt	fptr cs:gdtlimit	; load protected mode GDT
	mov	eax,cr0			; switch to protected mode
	or	al,01h
	mov	cr0,eax
	db 0EAh				; JMP FAR PTR SELCODE:$+4
	dw $+4,SELCODE			;  (clear prefetch que)
	pop	ds			; load protected mode DS
	mov	es,cx			; load protected mode ES
	xor	ax,ax
	mov	fs,ax			; load protected mode FS with NULL
	mov	gs,ax			; load protected mode GS with NULL
	pop	eax
	mov	ss,dx			; load protected mode SS:ESP
	mov	esp,ebx
	and	ah,0BFh			; set NT=0 in old EFLAGS
	push	ax			; set current FLAGS
	popf
	push	eax			; store old EFLAGS
	push	esi			; store protected mode target CS
	push	edi			; store protected mode target EIP
	iretd				; go to target addx in protected mode

;=============================================================================
; XMS/RAW protected mode to real mode switch
;
;	AX = real mode DS
;	CX = real mode ES
;	DX = real mode SS
;	BX = real mode stack pointer
;	SI = real mode target CS
;	DI = real mode target IP
;
	Align 4
xr_pmtormsw:
	pushf				; store FLAGS
	cli
	push	ax			; store AX (real mode DS)
	mov	ds,cs:seldata		; DS -> 0 (beginning of memory)
	pop	tempw0			; move real mode DS from stack to temp
	pop	tempw1			; move FLAGS from stack to temp
	mov	ax,SELDATA		; load descriptors with real mode seg
	mov	ds,ax			;  attributes
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	mov	ss,ax			; load descriptor with real mode attr
	movzx	esp,bx			; load real mode SP, high word 0
	lidt	fptr rmidtlimit		; load real mode IDT
	mov	eax,cr0			; switch to real mode
	and	al,0FEh			; turn off protected mode
	mov	cr0,eax
	db 0EAh				; JMP FAR PTR _KERNEL:$+4
	dw $+4,_KERNEL			;  (clear prefetch que)
	mov	ss,dx			; load real mode SS
	mov	ds,cs:tempw0		; load real mode DS
	mov	es,cx			; load real mode ES
	xor	ax,ax
	mov	fs,ax			; load real mode FS with NULL
	mov	gs,ax			; load real mode GS with NULL
	push	cs:tempw1		; store old FLAGS
	push	si			; store real mode target CS
	push	di			; store real mode target IP
	iret				; go to target addx in real mode


;=============================================================================
vxr_saverestorepm:			; VCPI/XMS/RAW save/restore status
	db 66h				; no save/restore needed, 32bit RETF
vxr_saverestorerm:			; VCPI/XMS/RAW save/restore status
	retf				; no save/restore needed, 16bit RETF

