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

;=============================================================================
int15h:	cmp ah,88h			; Real mode INT 15h
	jz @@0				; if func: 88h, need to process
	jmp cs:oldint15h		; no, go on to old INT 15h handler
@@0:	pushf				; call old int 15h handler
	call cs:oldint15h
	sub ax,cs:mem_used		; adjust AX by extended memory used
	push bp
	mov bp,sp
	and byte ptr [bp+6],0FEh	; clear carry flag on stack for IRET
	pop bp
	iret				; return with new AX extended memory


;=============================================================================
int21h:	cmp cs:id32_tsrmode_id,0	; if we are TSR, ignore 0FF88h calls
	jnz @@1
	cmp ax,0FF88h
	jz int21h_call
	cmp cs:id32_spawned_id,0	; check if it is the mother process
	jnz @@1				; if not, chain to the previous handlr
	cmp ah,4Ch			; Real mode INT 21h
	jz int21h_exit
	cmp ah,4Bh
	jz int21h_spawn
	cmp ah,31h
	jz int21h_tsr
@@1:	jmp cs:oldint21h		; go to original real mode INT 21h


;-----------------------------------------------------------------------------
int21h_exit:
	cli
	mov bp,ax			; preserve AX=exit code
	mov ax,SELDATA			; DS selector for protected mode
	mov cx,ax
	mov dx,SELZERO			; SS selector = zero selector
	mov ebx,cs:pmstacktop		; EBX = ESP for protected mode
	mov si,SELCODE			; target protected mode CS:EIP
	mov edi,offs @@1
	jmp cs:rmtopmswrout		; switch to protected mode
@@1:	mov ax,bp			; restore AX=exit code
	jmp int21

;-----------------------------------------------------------------------------
int21h_spawn:
	mov cs:id32_spawned_id,1	; disable memory deallocation
	pushf
	call cs:oldint21h
	mov cs:id32_spawned_id,0
	pop bx cx dx			; pop IP, CS, FLG
	push cx bx			; push CS, IP
	retf

;-----------------------------------------------------------------------------
int21h_call:				; DOS/32A functional call (real mode)
	mov eax,'ID32'			; EAX = "ID32"
	movzx ebx,cs:client_version	; EBX = client version
	mov ecx,cs:mem_free		; ECX = size of free memory
	mov edx,cs:mem_ptr		; EDX = base of free memory
	mov si,cs:id32_process_id	; SI(hi) = previous process id
	shl esi,16
	movzx si,cs:pagetables		; SI(lo) = pagetables allocated
	mov edi,cs:pagetablefree	; EDI = base of free pagetable space
	iret

;-----------------------------------------------------------------------------
int21h_tsr:
	mov cs:id32_tsrmode_id,1	; indicate that we are going TSR
	jmp cs:oldint21h




;=============================================================================
intold_save:
	pop bp
	mov ax,ds:[esi+04h]		; update FLAGS in structure
	mov es:[edi+20h],ax
	mov eax,ds:[esi+00h]		; update CS:IP in structure
	mov es:[edi+2Ah],eax
	push es edi
	jmp bp

intold_restore:
	pop edi es
	add es:[edi+2Eh],ax
	iretd



;-----------------------------------------------------------------------------
int1Bh:	call intold_save
	int 1Bh				; call protected mode INT 1Bh
	mov ax,6
	jmp intold_restore

;-----------------------------------------------------------------------------
int1Ch:	call intold_save
	push ds
	mov ds,cs:selzero		; restore default INT 1Ch
	mov eax,cs:oldint1Ch
	mov ds:[4*1Ch],eax
	pop ds
	int 1Ch				; call protected mode INT 1Ch
	push ds
	mov ds,cs:selzero		; restore callback INT 1Ch
	mov eax,cs:newint1Ch
	mov ds:[4*1Ch],eax
	pop ds
	mov ax,6
	jmp intold_restore

;-----------------------------------------------------------------------------
int23h:	call intold_save
	clc
	mov ebp,esp
	int 23h				; call protected mode INT 23h
	mov esp,ebp
	setc ah
	mov al,es:[edi+20h]
	and al,0FEh
	add al,ah
	mov es:[edi+20h],al
	mov ax,4
	jmp intold_restore

;-----------------------------------------------------------------------------
int24h:	call intold_save
	push edi
	push dword ptr ds:[esi+1Ah]	; CS, FLG
	push dword ptr ds:[esi+16h]	; ES, IP
	push dword ptr ds:[esi+12h]	; BP, DS
	push dword ptr ds:[esi+0Eh]	; SI, DI
	push dword ptr ds:[esi+0Ah]	; CX, DX
	push dword ptr ds:[esi+06h]	; AX, BX
	mov ax,word ptr es:[edi+1Ch]	; get register AX
	mov bp,word ptr es:[edi+08h]	; get register BP
	mov si,word ptr es:[edi+04h]	; get register SI
	mov di,word ptr es:[edi+00h]	; get register DI
	int 24h				; call protected mode INT 24h
	add esp,18h
	pop edi
	mov es:[edi+1Ch],al
	mov ax,6
	jmp intold_restore



;=============================================================================
; Enable/Disable A20 Line
;=============================================================================
enable_A20:				; hardware enable gate A20
	pushf
	cli
	call enablea20test		; is A20 already enabled?
	setz al				; AL = 00h (OFF),  AL = 01h (ON)
	mov A20_state,al
	jz @@done			; if yes, done
	in al,92h			; PS/2 A20 enable
	or al,02h
	jmp short $+2
	jmp short $+2
	jmp short $+2
	out 92h,al
	call enablea20test
	jz @@done
	call enablea20kbwait		; AT A20 enable
	jnz @@f0
	mov al,0D1h
	out 64h,al
	call enablea20kbwait
	jnz @@f0
	mov al,0DFh
	out 60h,al
	call enablea20kbwait
@@f0:	mov cx,800h			; wait for A20 to enable do 800h tries
@@l0:	call enablea20test		; is A20 enabled?
	jz @@done			; if yes, done
	in al,40h			; get current tick counter
	jmp short $+2
	jmp short $+2
	jmp short $+2
	in al,40h
	mov ah,al
@@l1:	in al,40h			; wait a single tick
	jmp short $+2
	jmp short $+2
	jmp short $+2
	in al,40h
	cmp al,ah
	je @@l1
	loop @@l0			; loop for another try
	popf
	stc				; error: set carry flag
	ret
@@done:	popf
	clc				; success: clear carry
	ret


;-----------------------------------------------------------------------------
comment~
disable_A20:				; hardware disable gate A20
	pushf
	cli
	mov al,cs:A20_state		; check if A20 was enabled from
	test al,al			;  the beginning
	jnz @@done			; yes, do not disable it, jump
	call disablea20test		; is A20 already disabled?
	jnz @@done			; if yes, done
	in al,92h			; PS/2 A20 disable
	and al,0FDh
	jmp short $+2
	jmp short $+2
	jmp short $+2
	out 92h,al
	call disablea20test
	jnz @@done
	call enablea20kbwait		; AT A20 disable
	jnz @@f0
	mov al,0D1h
	out 64h,al
	call enablea20kbwait
	jnz @@f0
	mov al,0DDh
	out 60h,al
	call enablea20kbwait
@@f0:	mov cx,800h			; wait for A20 to disable, 800h tries
@@l0:	call disablea20test		; is A20 disabled?
	jnz @@done			; if yes, done
	in al,40h			; get current tick counter
	jmp short $+2
	jmp short $+2
	jmp short $+2
	in al,40h
	mov ah,al
@@l1:	in al,40h			; wait a single tick
	jmp short $+2
	jmp short $+2
	jmp short $+2
	in al,40h
	cmp al,ah
	je @@l1
	loop @@l0			; loop for another try
	popf
	stc				; error: set carry flag
	ret
@@done:	popf
	clc				; success: clear carry
	ret
~

enablea20kbwait:			; wait for safe to write to 8042
	xor cx,cx
@@l0:	jmp short $+2
	jmp short $+2
	jmp short $+2
	in al,64h			; read 8042 status
	test al,2			; buffer full?
	loopnz @@l0			; if yes, loop
	ret

enablea20test:				; test for enabled A20
	push fs gs
	xor ax,ax			; set A20 test segments 0 and 0ffffh
	mov fs,ax
	dec ax
	mov gs,ax
	mov al,fs:[0000h]		; get byte from 0:0
	mov ah,al			; preserve old byte
	not al				; modify byte
	xchg al,gs:[0010h]		; put modified byte to 0ffffh:10h
	cmp ah,fs:[0000h]		; set zero if byte at 0:0 not modified
	mov gs:[0010h],al		; put back old byte at 0ffffh:10h
	pop gs fs
	ret				; return, zero if A20 enabled

comment~
disablea20test:				; test for disabled A20
	push ds esi edi
	mov ds,cs:selzero
	xor esi,esi
	mov edi,100000h
	mov al,[esi]
	mov ah,al
	not al
	xchg al,[edi]
	cmp ah,[esi]
	mov [edi],al
	pop edi esi ds
	ret
~



;=============================================================================
critical_error_rm:			; error in Protected Mode
	cli				; not enough space on real mode stack
	mov ds,cs:seldata
	mov es,selzero
	mov eax,oldint1Ch		; restore INT 1Ch
	mov es:[4*1Ch],eax
	mov ax,rmstacktop2		; restore RM stack
	mov rmstacktop,ax
	mov ax,0081h			; AX = error code
	jmp dword ptr client_call

critical_error_pm:			; error in Real mode
	cli				; not enough space on prot. mode stack
	mov ds,cs:kernel_code
	xor ax,ax
	mov es,ax
	mov eax,oldint1Ch
	mov es:[4*1Ch],eax
	mov eax,pmstacktop2
	mov pmstacktop,eax
	mov ebx,eax			; EBX = ESP for protected mode
	mov ax,SELDATA			; DS selector for protected mode
	mov cx,ax
	mov dx,SELZERO			; SS selector = zero selector
	mov si,SELCODE			; target protected mode CS:EIP
	mov edi,offs @@0
	jmp rmtopmswrout		; switch to protected mode
@@0:	mov ax,0082h			; AX = error code
	jmp dword ptr client_call

