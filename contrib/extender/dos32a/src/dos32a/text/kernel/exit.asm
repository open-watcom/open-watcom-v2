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
; Exit Protected Mode
;=============================================================================

int21:	cmp ah,4Ch			; watch for INT 21h AH=4Ch
	je @@0
	jmp int_matrix+4*21h		; go to INT 21h redirection
@@0:	cli
	cld
	push ax				; preserve AX error code
	mov ds,cs:seldata
	mov es,selzero
	mov eax,oldcr0
	mov cr0,eax

	mov eax,oldint1Bh		; restore critical INTs
	mov es:[4*1Bh],eax
	mov eax,oldint1Ch
	mov es:[4*1Ch],eax
	mov eax,oldint21h
	mov es:[4*21h],eax
	mov eax,oldint23h
	mov es:[4*23h],eax
	mov eax,oldint24h
	mov es:[4*24h],eax
	mov eax,oldint2Fh
	mov es:[4*2Fh],eax
	call mem_dealloc		; dealloc memory for current process
	movzx bx,pmodetype		; appropriate exit routine
	add bx,bx
	call @@exit[bx]			; call cleanup routine on exit
	pop ax				; restore AX
@@done:	jmp int_matrix+4*21h		; go to INT 21h redirection

@@exit	dw r_exit
	dw x_exit
	dw v_exit
	dw d_exit



;=============================================================================
mem_dealloc:
	cmp id32_process_id,0		; no need in freeing memory when
	jz @@done			;  only one process is running
	mov esi,mem_ptr
	mov eax,mem_free
	or eax,esi			; check if memory was allocated
	jz @@done			; if not, we are done
@@1:	mov eax,es:[esi+04h]
	mov edx,es:[esi+08h]
	btr eax,31
	cmp edx,dptr id32_process_id	; block allocated from this process
	jnz @@2				; if not, jump
	mov es:[esi+04h],eax		; otherwise set this block as free
@@2:	lea esi,[esi+eax+10h]		; get ptr to next memory block
	cmp esi,mem_top			; check if at top of memory
	jb @@1				; if not, loop
	push ds es
	pop ds
	call int31_linkfreeblocks
	pop ds
@@done:	ret


;=============================================================================
r_exit:	mov eax,oldint15h		; put back old INT 15h handler
	mov es:[4*15h],eax
	ret

;=============================================================================
x_exit:	call xms_dealloc
	mov ah,A20_state		; reset A20 gate to initial state
	and ah,1
	xor ah,1			; calculate appropriate function num
	add ah,3
	jmp xms_call_pm

;=============================================================================
v_exit:	call vcpi_dealloc
	;jmp xms_dealloc


;-----------------------------------------------------------------------------
xms_dealloc:				; XMS deallocate memory
	mov dx,xms_handle		; check if memory was allocated
	test dx,dx
	jz d_exit			; if not, done
@@0:	mov ah,0Dh			; unlock XMS memory
	call xms_call_pm
	mov ah,0Ah			; free XMS memory
xms_call_pm:
	push ss
	pop es
	sub esp,32h
	mov edi,esp
	xor ecx,ecx
	mov [esp+14h],dx		; DX
	mov [esp+1Ch],ax		; AX
	mov eax,dword ptr xms_call	; real mode CS:IP
	mov [esp+20h],cx		; clear flags
	mov [esp+2Eh],ecx		; clear SS:SP
	mov [esp+2Ah],eax		; put in structure
	xor bx,bx
	mov ax,0301h
	int 31h
	add esp,32h
d_exit:	ret

vcpi_dealloc:				; VCPI deallocate memory
	mov cx,vcpi_allocmem		; check if memory was allocated
	mov esi,pagetablefree
	jcxz @@2			; if no memory was allocated, done
@@1:	mov edx,es:[esi]
	add esi,4
	and dx,0F000h
	mov ax,0DE05h
	call fword ptr vcpi_calleip
	loop @@1
	mov eax,vcpi_cr3		; reload CR3 to flush page cache
	mov cr3,eax
@@2:	ret


