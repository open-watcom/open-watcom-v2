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
_int10:	cld
	push	ds es
	pushad

	cmp	ah,1Bh
	jz	@v_1Bh
	cmp	ah,1Ch
	jz	@v_1Ch
	cmp	ax,4F00h
	jz	@v_4F00h
	cmp	ax,4F01h
	jz	@v_4F01h
	cmp	ax,4F04h
	jz	@v_4F04h
	cmp	ax,4F09h
	jz	@v_4F09h
	cmp	ax,4F0Ah
	jz	@v_4F0Ah

	popad
	pop	es ds
	db	66h
	jmp	cs:_int10_ip



;=============================================================================
; VGA: Read Functionality Information
; In:	BX = implementation type
;	ES:EDI = 64 byte buffer
; Out:	AL = 1Bh if function supported by VGA BIOS
;	BX = implementation type
;	ES:EDI = 64 byte buffer
;
@v_1Bh:	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+1Ch],ax		; store AX
	mov	[ebp+10h],bx		; store BX
	mov	ds,cs:_sel_ds
	mov	ax,_seg_buf
	mov	word ptr [ebp+22h],ax	; store ES
	mov	word ptr [ebp+00h],0	; store DI
	call	int10h
	mov	ecx,16
	mov	esi,_lobufbase
	rep	movs dword ptr es:[edi],[esi]
	movzx	eax,word ptr [ebp+1Ch]	; get AX
	movzx	ebx,word ptr [ebp+10h]	; get BX
	add	esp,32h
	mov	[esp+1Ch],eax		; return EAX
	mov	[esp+10h],ebx		; return EBX
	jmp	@__ok


;=============================================================================
; VGA: Save/Restore VGA Video State
; In:	AL = subfunction:
;		00h - get state buffer size
;		Return: EBX = number of 64-byte blocks needed
;		01h - save video states
;		02h - restore video states
;	ECX = states to save/restore
;	ES:EBX = pointer to save/restore buffer
; Out:	AL = 1Ch if function supported by VGA BIOS
;
@v_1Ch:	test	al,al
	jz	@v_1Ch_00
	cmp	al,01h
	jz	@v_1Ch_01
	cmp	al,02h
	jz	@v_1Ch_02
	jmp	@v_err

@v_1Ch_00:
	pushfd
	db	66h
	call	cs:_int10_ip
	movzx	eax,ax
	movzx	ebx,bx
	mov	[esp+1Ch],eax
	mov	[esp+10h],ebx
	jmp	@__ok

@v_1Ch_01:
	sub	esp,32h
	mov	ebp,esp
	call	@v_std
	call	int10h
	mov	esi,_lobufbase
	mov	edi,ebx
	mov	ax,1C00h
	xor	bx,bx
	int	10h
	mov	ecx,ebx
	shl	ecx,4
	rep	movs dword ptr es:[edi],[esi]
	jmp	@v_ok

@v_1Ch_02:
	sub	esp,32h
	mov	ebp,esp
	call	@v_std
	mov	esi,ebx
	mov	edi,_lobufbase
	mov	ax,1C00h
	xor	bx,bx
	int	10h
	mov	ecx,ebx
	shl	ecx,4
	push	ds es
	pop	ds es
	rep	movs dword ptr es:[edi],[esi]
	call	int10h
	jmp	@v_ok


;=============================================================================
; VESA: Get SuperVGA Information
; In:	ES:EDI = 512 byte buffer
; Out:	AL = 4Fh if function supported by VESA BIOS
;	AH = 00h if function was successful
;	ES:EDI = 512 byte buffer
;
;=============================================================================
; VESA: Get SuperVGA Mode Information
; In:	CX = VideoMode
;	ES:EDI = 256 byte buffer
; Out:	AL = 4Fh if function supported by VESA BIOS
;	AH = 00h if function was successful
;	ES:EDI = 256 byte buffer
;
@v_4F00h:
@v_4F01h:
	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+1Ch],ax		; store AX
	mov	[ebp+18h],cx		; store CX
	mov	dx,ax
	mov	ds,cs:_sel_ds
	mov	ax,_seg_buf
	mov	word ptr [ebp+22h],ax	; store ES
	mov	word ptr [ebp+00h],0	; store DI
	call	int10h
	mov	esi,_lobufbase
	test	dl,dl
	mov	ecx,64
	jnz	@@1

	lea	ebx,[esi+06h]		; offset of OEM Str
	call	@@mod
	lea	ebx,[esi+0Eh]		; offset of VideoMode Ptr
	call	@@mod
	lea	ebx,[esi+16h]		; offset of OEM Version Ptr
	call	@@mod
	lea	ebx,[esi+1Ah]		; offset of OEM Name Ptr
	call	@@mod
	lea	ebx,[esi+1Eh]		; offset of OEM Revision Ptr
	call	@@mod
	mov	ecx,128
@@1:	rep	movs dword ptr es:[edi],[esi]
	jmp	@v_ok

@@mod:	movzx	edx,word ptr [ebx+00h]	; get low word (offset)
	movzx	eax,word ptr [ebx+02h]	; get high word (segment)
	shl	eax,4			; convert real mode seg:off
	add	eax,edx			; to linear ptr relative to zero
	mov	dx,[ebx+02h]
	cmp	dx,_seg_buf
	jnz	@@ok
	sub	eax,_lobufzero		; since the structure will be moved
	add	eax,edi			; from buffer, we must adjust ptr
@@ok:	mov	[ebx+00h],eax
	ret


;=============================================================================
; VESA: Save/Restore SuperVGA Video State
; In:	DL = subfunction:
;		00h - get state buffer size
;		Return: BX = number of 64-byte blocks needed
;		01h - save video states
;		02h - restore video states
;	ECX = states to save/restore
;	ES:EBX = pointer to save/restore buffer
; Out:	AL = 4Fh if function supported by VESA BIOS
;	AH = 00h if function was successful
;
@v_4F04h:
	test	dl,dl
	jz	@v_1Ch_00
	cmp	dl,01h
	jz	@v_4F04h_01
	cmp	dl,02h
	jz	@v_4F04h_02
	jmp	@v_err

@v_4F04h_01:
	sub	esp,32h
	mov	ebp,esp
	call	@v_std
	call	int10h
	mov	esi,_lobufbase
	mov	edi,ebx
	mov	ax,4F04h
	xor	dl,dl
	xor	bx,bx
	int	10h
	mov	ecx,ebx
	shl	ecx,4
	rep	movs dword ptr es:[edi],[esi]
	jmp	@v_ok

@v_4F04h_02:
	sub	esp,32h
	mov	ebp,esp
	call	@v_std
	mov	esi,ebx
	mov	edi,_lobufbase
	mov	ax,4F04h
	xor	dl,dl
	xor	bx,bx
	int	10h
	mov	ecx,ebx
	shl	ecx,4
	push	ds es
	pop	ds es
	rep	movs dword ptr es:[edi],[esi]
	call	int10h
	jmp	@v_ok


;=============================================================================
; VESA: Load/Unload Palette Data
; In:	BL = subfunction:
;		00h - set palette data
;		01h - get palette data
;		02h - set 2nd palette data
;		03h - get 2nd palette data
;		80h - set palette data during VR
;	ECX = number of palette registers to update
;	EDX = first palette register to update
;	ES:EDI = pointer to buffer
; Out:	AL = 4Fh if function supported by VESA BIOS
;	AH = 00h if function was successful
;
@v_4F09h:
	cmp	bl,03h
	jbe	@@0
	cmp	bl,80h
	jnz	@v_err
@@0:	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+1Ch],ax
	mov	[ebp+18h],cx
	mov	[ebp+14h],dx
	mov	[ebp+10h],bx
	mov	ds,cs:_sel_ds
	mov	ax,_seg_buf
	mov	word ptr [ebp+22h],ax
	mov	word ptr [ebp+00h],0
	test	bl,bl
	jz	@@1
	dec	bl
	jz	@@2
	dec	bl
	jz	@@1
	dec	bl
	jz	@@2
@@1:	mov	esi,edi
	mov	edi,_lobufbase
	push	ds es
	pop	ds es
	rep	movs dword ptr es:[edi],[esi]
	call	int10h
	jmp	@v_ok
@@2:	call	int10h
	mov	esi,_lobufbase
	rep	movs dword ptr es:[edi],[esi]
	jmp	@v_ok


;=============================================================================
; VESA: Get Protected Mode Interface
; In:	BL = 00h - return protected mode table
; Out:	AL = 4Fh if function supported by VESA BIOS
;	AH = 00h if function was successful
;	ES = protected mode selector of table
;	EDI = offset of table
;	ECX = length of table
;
@v_4F0Ah:
	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+1Ch],ax
	mov	[ebp+10h],bx
	call	int10h
	movzx	eax,word ptr [ebp+1Ch]	; get AX
	movzx	ecx,word ptr [ebp+18h]	; get CX
	movzx	edx,word ptr [ebp+22h]	; get ES segment
	movzx	edi,word ptr [ebp+00h]	; get DI pointer
	cmp	ax,004Fh		; check that there was no error
	jnz	@v_ok
	shl	edx,4
	add	edi,edx			; convert seg:off to linear addr
	add	esp,32h
	mov	[esp+1Ch],eax		; set EAX
	mov	[esp+18h],ecx		; set ECX
	mov	[esp+00h],edi		; set EDI
	mov	ax,cs:_sel_zero
	mov	[esp+20h],ax		; set ES
	jmp	@__ok




;-----------------------------------------------------------------------------
@v_std:	mov	[ebp+1Ch],ax		; store AX
	mov	[ebp+18h],cx		; store CX
	mov	[ebp+14h],dx		; store DX
	mov	ds,cs:_sel_ds
	mov	ax,_seg_buf
	mov	word ptr [ebp+22h],ax	; store ES
	mov	word ptr [ebp+10h],0	; store BX
	ret
@v_ok:	movzx	eax,word ptr [ebp+1Ch]
	add	esp,32h
	mov	[esp+1Ch],eax
	jmp	@__ok
@v_err:	mov	dword ptr [esp+1Ch],-1
	jmp	@__ok


PopState
