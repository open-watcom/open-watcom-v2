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
_int33:	cld
	push	ds es
	pushad

	cmp	ax,0009h
	jz	@__0009h
	cmp	ax,000Ch
	jz	@__000Ch
	cmp	ax,0014h
	jz	@__0014h
	cmp	ax,0016h
	jz	@__0016h
	cmp	ax,0017h
	jz	@__0017h
	cmp	ax,0018h
	jz	@__0018h
	cmp	ax,0019h
	jz	@__0019h
	cmp	ax,0020h
	jz	@__0020h

	popad
	pop	es ds
	db	66h
	jmp	cs:_int33_ip




;=============================================================================
; Define Graphics Cursor
; In:	BX = column of cursor hotspot in bitmap
;	CX = row of cursor hotspot in bitmap
;	ES:EDX = mask bitmap
;
@__0009h:
	push	es
	pop	ds
	sub	esp,32h
	mov	ebp,esp
	mov	esi,edx
	mov	es,cs:_sel_zero
	movzx	edi,cs:_seg_mus
	mov	word ptr [ebp+1Ch],ax
	mov	word ptr [ebp+18h],cx
	mov	word ptr [ebp+10h],bx
	mov	word ptr [ebp+22h],di
	mov	word ptr [ebp+14h],0
	shl	edi,4
	mov	ecx,10h
	rep	movs dword ptr es:[edi],[esi]
	call	int33h
	add	esp,32h
	jmp	@__ok



;=============================================================================
; Define Interrupt Subroutine
; In:	CX = call mask
;	ES:EDX = far pointer to routine
;
@__000Ch:
	call	_mus_int_def
	jmp	@__ok


;=============================================================================
; Exchange Defined Interrupt Subroutine
; In:	CX = call mask
;	ES:EDX = far pointer to routine
; Out:	CX = call mask
;	ES:EDX = far pointer to routine
;
@__0014h:
	mov	si,word ptr cs:_mus_sel
	mov	edi,dword ptr cs:_mus_off
	call	_mus_int_def
	mov	[esp+14h],edi
	mov	[esp+20h],si
	jmp	@__ok


;=============================================================================
; Define Interrupt Subroutine
; In:	CX = call mask
;	ES:EDX = far pointer to routine
;
@__0018h:
	call	_mus_int_def
	mov	[esp+1Ch],eax
	jmp	@__ok


;=============================================================================
; Return Defined Interrupt Subroutine
; In:	CX = call mask
; Out:	BX:EDX = far pointer to routine
;
@__0019h:
	mov	ax,word ptr cs:_mus_sel
	mov	edx,dword ptr cs:_mus_off
	mov	[esp+14h],edx
	mov	[esp+10h],ax
	jmp	@__ok



;-----------------------------------------------------------------------------
_mus_int_def:
	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+1Ch],ax
	mov	[ebp+18h],cx
	mov	ds,cs:_sel_ds
	xor	eax,eax
	mov	_mus_off,edx
	mov	_mus_sel,es
	mov	ax,es
	or	eax,edx
	jz	@@1
	mov	ax,_seg_ds
	mov	dx,offs _mus_int_rm
@@1:	mov	word ptr [ebp+14h],dx
	mov	word ptr [ebp+22h],ax
	cli
	call	int33h
	movzx	eax,word ptr [ebp+1Ch]
	add	esp,32h
	sti
	ret

_mus_int_rm:
	cmp	cs:_mus_data,0
	mov	cs:_mus_data,1
	jnz	@@1
	jmp	dword ptr cs:_mus_backoff
@@1:	retf

_mus_int_pm:
	cld
	push	ss esp
	pushad
	push	ds es
	xor	eax,eax
	mov	ax,ss
	lar	eax,eax
	test	eax,00400000h
	jnz	@@1
	movzx	esp,sp
@@1:	mov	ax,cs:_seg_ds
	mov	word ptr es:[edi+2Ah],offs @@done
	mov	word ptr es:[edi+2Ch],ax
	movzx	eax,word ptr es:[edi+1Ch]
	movzx	ecx,word ptr es:[edi+18h]
	movzx	edx,word ptr es:[edi+14h]
	movzx	ebx,word ptr es:[edi+10h]
	movzx	esi,word ptr es:[edi+04h]
	movzx	edi,word ptr es:[edi+00h]
	call	fword ptr cs:_mus_off
	pop	es ds
	popad
	lss	esp,[esp]
	add	esp,2
	iretd
@@done:	mov	cs:_mus_data,0
	retf



;=============================================================================
; Save Driver State
; In:	BX = size of buffer
;	ES:EDX = buffer for driver state
;
@__0016h:
	sub	esp,32h
	mov	ebp,esp
	mov	edi,edx
	mov	[ebp+1Ch],ax
	mov	ax,cs:_seg_buf
	mov	word ptr [ebp+22h],ax
	mov	word ptr [ebp+14h],0
	call	int33h
	mov	ds,cs:_sel_ds
	mov	esi,_lobufbase
	mov	ecx,_mus_size
	rep	movs byte ptr es:[edi],[esi]
	add	esp,32h
	jmp	@__ok


;=============================================================================
; Restore Driver State
; In:	BX = size of buffer
;	ES:EDX = buffer containing saved state
;
@__0017h:
	push	es
	pop	ds
	sub	esp,32h
	mov	ebp,esp
	mov	esi,edx
	mov	[ebp+1Ch],ax
	mov	ax,cs:_seg_buf
	mov	word ptr [ebp+22h],ax
	mov	word ptr [ebp+14h],0
	mov	es,cs:_sel_ds
	mov	edi,cs:_lobufbase
	mov	ecx,cs:_mus_size
	rep	movs byte ptr es:[edi],[esi]
	call	int33h
	add	esp,32h
	jmp	@__ok


;=============================================================================
; Enable Mouse Driver
; In:	-
;
@__0020h:
	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+1Ch],ax
	call	int33h
	add	esp,32h
	mov	word ptr [esp+1Ch],-1
	jmp	@__ok



PopState
