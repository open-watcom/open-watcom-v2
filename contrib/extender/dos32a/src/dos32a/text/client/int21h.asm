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
_int21:	cld
	test	cs:_sys_misc,0100h	; check for CTRL-C flag
	jnz	_ctrl_c

	push	ds es
	pushad

	cmp	ah,09h		; Print string:		DS:EDX
	jz	@__09h

	cmp	ah,1Ah		; Set DTA buffer:	DS:EDX
	jz	@__1Ah
	cmp	ah,1Bh		; Get DefDrive Info:	-> AL, DS:EBX, ECX EDX
	jz	@__1Bh
	cmp	ah,1Ch		; Get Drive Info:	AL, (-> ---//---)
	jz	@__1Ch
	cmp	ah,1Fh		; Get DefDrive PB:	-> AL, DS:EBX
	jz	@__1Fh

	cmp	ah,25h		; Set INT vector:	AL, DS:EDX
	jz	@__25h

	cmp	ah,2Fh		; Get DTA buffer:	-> ES:EBX
	jz	@__2Fh
	cmp	ah,31h		; Go TSR		AL
	jz	@__31h
	cmp	ah,32h		; Get Drive PB:		AL, (-> DS:EBX)
	jz	@__32h
	cmp	ah,34h		; Get InDOS flag:	-> ES:EBX
	jz	@__34h

	cmp	ah,35h		; Get INT vector:	AL, ES:EBX
	jz	@__35h

	cmp	ah,39h		; Create DIR:		DS:EDX
	jz	@__39h
	cmp	ah,3Ah		; Remove DIR:		DS:EDX
	jz	@__3Ah
	cmp	ah,3Bh		; Change DIR:		DS:EDX
	jz	@__3Bh

	cmp	ah,3Ch		; Create file:		CX, DS:EDX
	jz	@__3Ch
	cmp	ah,3Dh		; Open file:		AL, DS:EDX
	jz	@__3Dh
	cmp	ah,3Fh		; Read file:		BX, ECX, DS:EDX
	jz	@__3Fh
	cmp	ah,40h		; Write file:		BX, ECX, DS:EDX
	jz	@__40h
	cmp	ah,41h		; Delete file:		DS:EDX
	jz	@__41h
	cmp	ah,42h		; Move file ptr:	AL, BX, EDX, (->EAX)
	jz	@__42h
	cmp	ah,43h		; Change file attr:	CX, DS:EDX
	jz	@__43h

	cmp	ah,47h		; Get DIR:		DL, DS:ESI
	jz	@__47h

	cmp	ah,48h		; Alloc mem:		BX (->AX)
	jz	@__48h
	cmp	ah,49h		; Dealloc mem:		ES
	jz	@__49h
	cmp	ah,4Ah		; Realloc mem:		BX
	jz	@__4Ah

	cmp	ah,4Bh		; Execute Prog:		AL, DS:EDX, ES:EBX
	jz	@__4Bh
	cmp	ah,4Ch		; Terminate Prog:	AL
	jz	@__4Ch

	cmp	ah,4Eh		; Find file:		CX, DS:EDX
	jz	@__4Eh
	cmp	ah,4Fh		; Find next file:	-
	jz	@__4Fh
	cmp	ah,51h		; Get PSP segment:	(->BX)
	jz	@__51h
	cmp	ah,56h		; Rename file:		DS:EDX, ES:EDI
	jz	@__56h
	cmp	ah,5Ah		; Create temp file:	CX, DS:EDX
	jz	@__5Ah
	cmp	ah,5Bh		; Create new file:	CX, DS:EDX
	jz	@__5Bh
	cmp	ah,62h		; Get PSP selector:	(->BX)
	jz	@__62h

	cmp	ah,0FFh		; Rational DOS/4G call
	jz	@__FFh

	cmp	ah,71h		; Windows 95 long filename extensions
	jnz	@__go21
	cmp	al,39h		; Win95 Create DIR:	DS:EDX
	jz	@__39h
	cmp	al,3Ah		; Win95 Remove DIR:	DS:EDX
	jz	@__3Ah
	cmp	al,3Bh		; Win95 Change DIR:	DS:EDX
	jz	@__3Bh
	cmp	al,41h		; Win95 Delete file:	DS:EDX, CX, SI
	jz	@__41h
	cmp	al,43h		; Win95 Change attr:	DS:EDX, BL, CX
	jz	@__43h
	cmp	al,47h		; Win95 Get curr DIR:	DS:ESI, DL
	jz	@__47h
	cmp	al,56h		; Win95 Rename file:	DS:EDX, ES:EDI
	jz	@__56h
	cmp	al,60h		; Win95 True name:
	jz	@_7160
	cmp	al,6Ch		; Win95 C/Open file:
	jz	@_716C

@__go21:popad
	pop	es ds
	db	66h
	jmp	cs:_int21_ip


_ctrl_c:mov	ax,4CFFh	; exit on CTRL-C with code 255
	jmp	@__4Ch




;=============================================================================
; Print String
;  In:	DS:EDX = offset of string
;  Out:	-
;
@__09h:	push	ds
	pop	es
	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+1Ch],ax
	mov	esi,edx
	mov	edi,edx
	mov	al,'$'
	or	ecx,-1
	repne	scas byte ptr es:[edi]
	not	ecx
	mov	es,cs:_sel_ds
	mov	edi,cs:_lobufbase
	rep	movs byte ptr es:[edi],[esi]
	stos	byte ptr es:[edi]
	mov	ax,cs:_seg_buf
	mov	word ptr [ebp+24h],ax
	mov	word ptr [ebp+14h],00h
	call	int21h
	add	esp,32h
	jmp	@__ok



;=============================================================================
; Set DTA buffer
;  In:	DS:EDX = sel:offs
;  Out:	-
;
@__1Ah:	mov	es,cs:_sel_ds
	mov	es:_app_dta_sel,ds
	mov	es:_app_dta_off,edx
	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+1Ch],ax
	mov	ax,cs:_seg_dta
	mov	word ptr [ebp+24h],ax
	mov	word ptr [ebp+14h],0
	call	int21h
	add	esp,32h
	jmp	@__ok



;=============================================================================
; Get Default Drive Info
;  In:	-
;  Out:	AL = sectors per cluster
;	DS:EBX = addr of FAT
;	ECX = bytes per sector
;	EDX = number of clusters on drive
;
@__1Bh:
@__1Ch:	call	@__all
	mov	[esp+1Ch],al
	cmp	al,0FFh
	jz	@@done
	mov	[esp+14h],edx
	mov	[esp+18h],ecx
	shl	esi,4
	add	ebx,esi
	mov	[esp+10h],ebx
	mov	ax,cs:_sel_zero
	mov	[esp+22h],ax
@@done:	jmp	@__ok



;=============================================================================
; Get Drive Info
;  In:	DL = Drive number
;  Out:	AL = sectors per cluster
;	DS:EBX = addr of FAT
;	ECX = bytes per sector
;	EDX = number of clusters on drive
;
;@__1Ch:	jmp	@__1Bh



;=============================================================================
; Get DefDrive Parameter Block
;  In:	-
;  Out:	AL = status
;	DS:EBX = addr of DPB
;
@__1Fh:
@__32h:	call	@__all
	mov	[esp+1Ch],al
	cmp	al,0FFh
	jz	@@done
	shl	esi,4
	add	ebx,esi
	mov	[esp+10h],ebx
	mov	ax,cs:_sel_zero
	mov	[esp+22h],ax
@@done:	jmp	@__ok



;=============================================================================
; Set INT vector
;  In:	AL = INT vector, DS:EDX = sel:offs
;  Out:	-
;
@__25h:	mov	bl,al
	mov	cx,ds
	mov	ax,0205h
	int	31h
	jc	@__err
	jmp	@__ok



;=============================================================================
; Get DTA buffer
;  In:	-
;  Out:	ES:EBX = addr of DTA
;
@__2Fh:	mov	ax,cs:_app_dta_sel
	mov	[esp+20h],ax
	mov	eax,cs:_app_dta_off
	mov	[esp+10h],eax
	jmp	@__ok



;=============================================================================
; Go TSR
;  In:	AL = exit code
;  Out:	-
;
@__31h:	sub	esp,32h
	mov	ebp,esp

	mov	bx,cs:_membase
	add	bx,cs:_buf_size
	sub	bx,cs:_seg_es
	mov	[ebp+1Ch],ax
	mov	[ebp+14h],bx
	call	int21h

	add	esp,32h
	jmp	@__ok



;=============================================================================
; Get Drive Parameter Block
;  In:	DL = drive number
;  Out:	AL = status
;	DS:EBX = addr of DPB
;
;@__32h:	jmp	@__1Fh



;=============================================================================
; Get InDOS flag address
;  In:	-
;  Out:	ES:EBX = addr of InDOS flag
;
@__34h:	call	@__all
	shl	edi,4
	add	ebx,edi
	mov	[esp+10h],ebx
	mov	ax,cs:_sel_zero
	mov	[esp+20h],ax
	jmp	@__ok



;=============================================================================
; Get INT vector
;  In:	AL = INT vector
;  Out:	ES:EBX = sel:offs
;
@__35h:	mov	bl,al
	mov	ax,0204h
	int	31h
	mov	[esp+20h],cx
	mov	[esp+10h],edx
	jmp	@__ok



;=============================================================================
; Create Directory
;  In:	DS:EDX = directory name
;  Out:	-
;
@__39h:
@__3Ah:
@__3Bh:
@__41h:
@__5Ah:
@__5Bh:
	call	@__std
	jz	@__ok
	mov	[esp+1Ch],eax
	jmp	@__err



;=============================================================================
; Remove Directory
;  In:	DS:EDX = directory name
;  Out:	-
;
;@__3Ah:	jmp	@__39h



;=============================================================================
; Change Directory
;  In:	DS:EDX = directory name
;  Out:	-
;
;@__3Bh:	jmp	@__39h



;=============================================================================
; Create File
;  In:	DS:EDX = file name, ECX = attributes
;  Out:	EAX = file handle
;
@__3Ch:
@__3Dh:	call	@__std
	mov	[esp+1Ch],eax
	jz	@__ok
	jmp	@__err



;=============================================================================
; Open File
;  In:	DS:EDX = file name, AL = access mode
;  Out:	EAX = file handle
;
;@__3Dh:	jmp	@__3Ch



;=============================================================================
; Read from File
;  In:	DS:EDX = addr, ECX = size, EBX = handle
;  Out:	EAX = bytes read
;
@__3Fh:	push	ds
	pop	es
	mov	ds,cs:_sel_ds			; DS=_TEXT16
	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+10h],bx			; store handle in structure
	mov	edi,edx				; ES:EDI=destination
	mov	ebx,ecx				; EBX=bytes to read
	xor	edx,edx				; EDX=counter bytes read
	cmp	ecx,_lobufsize
	jbe	@@low
@@0:	mov	ax,_seg_buf
	mov	word ptr [ebp+24h],ax		; store DS in structure
	mov	word ptr [ebp+14h],0		; store DX in structure
	mov	byte ptr [ebp+1Dh],3Fh		; store AH in structure
	mov	eax,ebx
	cmp	eax,_lobufsize
	jbe	@@1
	mov	eax,_lobufsize
@@1:	mov	[ebp+18h],ax			; store CX in structure
	call	int21h				; DOS read from file
	movzx	eax,word ptr [ebp+1Ch]		; EAX=bytes read
	test	byte ptr [ebp+20h],1		; check for error
	jnz	@@err
	mov	ecx,eax				; ECX=EAX for string copy
	mov	esi,_lobufbase
	jcxz	@@done				; if ECX=0, 0 bytes read, done
	shr	cx,2
	rep	movs dword ptr es:[edi],[esi]	; copy buffer
	mov	cl,al
	and	cl,03h
	rep	movs byte ptr es:[edi],[esi]
	add	edx,eax				; adjust bytes read
	sub	ebx,eax				; adjust bytes to read
	ja	@@0
@@done:	add	esp,32h
	mov	[esp+1Ch],edx			; store bytes count
	jmp	@__ok
@@err:	add	esp,32h
	mov	[esp+1Ch],eax			; error reading, store errcode
	jmp	@__err

@@low:	mov	ax,_seg_buf
	mov	word ptr [ebp+24h],ax		; store DS in structure
	mov	word ptr [ebp+18h],cx		; store CX in structure
	mov	word ptr [ebp+14h],0		; store DX in structure
	mov	byte ptr [ebp+1Dh],3Fh		; store AH in structure
	call	int21h				; DOS read from file
	movzx	eax,word ptr [ebp+1Ch]		; EAX=bytes read
	test	byte ptr [ebp+20h],1		; check for error
	jnz	@@err
	mov	ecx,eax				; ECX=EAX for string copy
	mov	esi,_lobufbase
	jcxz	@@done				; if ECX=0, 0 bytes read, done
	shr	cx,2
	rep	movs dword ptr es:[edi],[esi]	; copy buffer
	mov	cl,al
	and	cl,03h
	rep	movs byte ptr es:[edi],[esi]
	add	edx,eax				; adjust bytes read
	jmp	@@done


;=============================================================================
; Write to File
;  In:	DS:EDX = addr, ECX = size, EBX = handle
;  Out:	EAX = bytes written
;
@__40h:	mov	es,cs:_sel_ds
	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+10h],bx			; store handle in structure
	mov	esi,edx				; DS:ESI=source
	mov	ebx,ecx				; EBX=bytes to write
	xor	edx,edx				; EDX=counter bytes written
	cmp	ecx,cs:_lobufsize
	jbe	@@low
@@0:	mov	ax,cs:_seg_buf
	mov	word ptr [ebp+24h],ax		; store DS in structure
	mov	word ptr [ebp+14h],0		; store DX in structure
	mov	byte ptr [ebp+1Dh],40h		; store AH in structure
	mov	eax,ebx
	cmp	eax,cs:_lobufsize
	jbe	@@1
	mov	eax,cs:_lobufsize
@@1:	mov	[ebp+18h],ax			; store CX in structure
	mov	ecx,eax
	mov	edi,cs:_lobufbase
	jcxz	@@done
	shr	cx,2
	rep	movs dword ptr es:[edi],[esi]
	mov	cl,al
	and	cl,03h
	rep	movs byte ptr es:[edi],[esi]
	mov	ecx,eax				; preserve EAX in ECX
	call	int21h				; DOS write to file
	movzx	eax,word ptr [ebp+1Ch]		; EAX=bytes written
	test	byte ptr [ebp+20h],1		; check for error
	jnz	@@err
	add	edx,eax				; adjust bytes written
	sub	ebx,ecx				; adjust bytes to write
	ja	@@0				; loop until done
@@done:	add	esp,32h
	mov	[esp+1Ch],edx			; store bytes count
	jmp	@__ok
@@err:	add	esp,32h
	mov	[esp+1Ch],eax			; error writing, store errcode
	jmp	@__err

@@low:	mov	ax,cs:_seg_buf
	mov	word ptr [ebp+24h],ax		; store DS in structure
	mov	word ptr [ebp+18h],cx		; store CX in structure
	mov	word ptr [ebp+14h],0		; store DX in structure
	mov	byte ptr [ebp+1Dh],40h		; store AH in structure
	jcxz	@@l1
	mov	edi,cs:_lobufbase
	mov	al,cl
	shr	cx,2
	rep	movs dword ptr es:[edi],[esi]
	mov	cl,al
	and	cl,03h
	rep	movs byte ptr es:[edi],[esi]
@@l1:	call	int21h				; DOS write to file
	movzx	eax,word ptr [ebp+1Ch]		; EAX=bytes written
	test	byte ptr [ebp+20h],1		; check for error
	jnz	@@err
	add	edx,eax				; adjust bytes written
	jmp	@@done


;=============================================================================
; Delete File
;  In:	DS:EDX = file name
;  Out:	-
;
;@__41h:	jmp	@__39h



;=============================================================================
; Move File Ptr
;  In:	ECX:EDX = ptr, AL = mode
;  Out:	EDX:EAX = ptr
;
@__42h:	call	@__all
	mov	[esp+1Ch],eax
	jnz	@__err
	mov	[esp+14h],edx
	jmp	@__ok



;=============================================================================
; Change File Attributes
;  In:	DS:EDX = file name
;  Out:	-
;
@__43h:	call	@__std
	jz	@@1
	mov	[esp+1Ch],eax
	jmp	@__err
@@1:	mov	[esp+18h],ecx
	jmp	@__ok



;=============================================================================
; Get Directory
;  In:	DL = drive
;	DS:ESI = buffer
;  Out:	-
;
@__47h:	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+1Ch],ax
	mov	[ebp+14h],dx
	mov	ax,cs:_seg_buf
	mov	[ebp+24h],ax
	mov	word ptr [ebp+04h],0
	call	int21h
	test	byte ptr [ebp+20h],1
	jnz	@@err
	push	ds
	pop	es
	mov	edi,esi
	mov	ds,cs:_sel_ds
	mov	esi,_lobufbase
@@1:	lods	byte ptr ds:[esi]
	stos	byte ptr es:[edi]
	test	al,al
	jnz	@@1
	add	esp,32h
	jmp	@__ok
@@err:	movzx	eax,word ptr [ebp+1Ch]
	add	esp,32h
	mov	[esp+1Ch],eax
	jmp	@__err



;=============================================================================
; Allocate DOS Memory
;  In:	EBX = bytes to allocate
;  Out:	EAX = selector
;
@__48h:	mov	ax,0100h
	int	31h
	jc	@@1
	movzx	edx,dx
	mov	[esp+1Ch],edx
	jmp	@__ok
@@1:	movzx	eax,ax
	movzx	ebx,bx
	mov	[esp+1Ch],eax
	mov	[esp+10h],ebx
	jmp	@__err



;=============================================================================
; Deallocate DOS Memory
;  In:	ES = selector
;  Out:	-
;
@__49h:	mov	ax,0101h
	mov	dx,es
	int	31h
	jnc	@@0
	movzx	eax,ax
	mov	[esp+1Ch],eax
	jmp	@__err
@@0:	mov	word ptr [esp+20h],0
	jmp	@__ok



;=============================================================================
; Resize DOS Memory
;  In:	ES = selector, EBX = new size
;  Out:	-
;
@__4Ah:	mov	ax,0102h
	mov	dx,es
	int	31h
	jnc	@__ok
	movzx	eax,ax
	movzx	ebx,bx
	mov	[esp+1Ch],eax
	mov	[esp+10h],ebx
	jmp	@__err



;=============================================================================
; Execute Program (sub-func AL=00h)
;  In:	AL = 00h
;	DS:EDX = path name
;	ES:EBX = parameter block
;  Out:	-
;
@__4Bh:	test	al,al			; only subfunction AL=00h supported
	jnz	@__err
	cmp	cs:_lobufsize,0400h	; buffer size must be at least 1Kb
	jb	@__err			; if less, return -1

	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+1Ch],ax		; put AX in structure

	push	es
	mov	es,cs:_sel_ds
	mov	edi,cs:_lobufbase
	mov	esi,edx
	add	edi,100h
@@1:	lods	byte ptr ds:[esi]	; copy Path\Name into buffer
	stos	byte ptr es:[edi]
	test	al,al
	jnz	@@1
	pop	es

	push	ds es
	mov	esi,es:[ebx+06h]
	mov	edi,cs:_lobufbase
	mov	ds,es:[ebx+0Ah]
	mov	es,cs:_sel_ds
	add	edi,180h
	movzx	ecx,byte ptr ds:[esi]
	inc	cx
	inc	cx
	rep	movs byte ptr es:[edi],[esi]	; copy command line
	pop	es ds

	push	ds es
	mov	edi,es:[ebx+00h]	; get environment
	mov	ax,es:[ebx+04h]
	test	ax,ax			; check env selector is 0
	jz	@@3			; if yes, jump
	mov	es,ax
	xor	ax,ax
	mov	esi,edi
	or	ecx,-1
@@2:	repne	scas byte ptr es:[edi]
	dec	ecx
	scas	byte ptr es:[edi]
	jnz	@@2
	not	ecx
	mov	ax,0100h		; allocate mem for environment
	mov	bx,cx
	shr	bx,4
	inc	bx
	int	31h
	jc	@@4
	push	es
	pop	ds
	mov	es,dx
	xor	edi,edi
	rep	movs byte ptr es:[edi],[esi]	; copy environment
@@3:	mov	ds,cs:_sel_ds
	mov	edi,_lobufbase
	mov	word ptr [edi+00h],ax
	mov	ax,_seg_buf
	mov	word ptr [edi+02h],180h
	mov	word ptr [edi+04h],ax
	mov	ax,_seg_es
	mov	word ptr [edi+06h],5Ch
	mov	word ptr [edi+08h],ax
	mov	word ptr [edi+0Ah],6Ch
	mov	word ptr [edi+0Ch],ax
@@4:	pop	es ds
	jc	@@err

	push	dx			; save env selector
	mov	ds,cs:_sel_ds
	mov	ax,_seg_buf
	mov	word ptr [ebp+22h],ax
	mov	word ptr [ebp+24h],ax
	mov	word ptr [ebp+10h],0000h
	mov	word ptr [ebp+14h],0100h

	cmp	_sys_type,3
	jz	@@5
	mov	eax,cr0
	mov	edi,eax
	and	al,0FBh
	mov	cr0,eax
@@5:	call	uninstall_client_ints
	call	int21h
	call	install_client_ints
	cmp	_sys_type,3
	jz	@@6
	mov	cr0,edi

@@6:	pop	dx				; restore env sel
	mov	ax,0101h
	int	31h

	movzx	eax,word ptr [ebp+1Ch]		; get return code
	test	byte ptr [ebp+20h],01h		; check carry flag
	lea	esp,[esp+32h]
	mov	[esp+1Ch],eax			; put return code in AX
	jnz	@__err				; if error, set carry on ret
	jmp	@__ok

@@err:	add	esp,32h
	mov	dword ptr [esp+1Ch],-1
	jmp	@__err



;=============================================================================
; Terminate Program
;  In:	AL = error code
;  Out:	-
;
@__4Ch:	cli				; disable interrupts
	cld
	mov	ds,cs:_sel_ds		; restore SEG registers
	mov	es,_sel_es
	lss	esp,fword ptr _sel_esp	; set default stack
	push	ax
	mov	ax,_sel_env
	mov	es:[002Ch],ax		; restore default environment

	cmp	_sys_type,3		; if under DPMI, do not clear DRx
	jz	@@1
	xor	eax,eax
	mov	dr7,eax

@@1:	push	es			; undefine Mouse ISR
	mov	ax,000Ch
	xor	edx,edx
	mov	cx,dx
	mov	es,dx
	int	33h
	pop	es

	mov	dx,_mus_backoff		; free mouse callback
	mov	cx,_mus_backseg
	mov	ax,cx
	or	ax,dx
	jz	@@2
	mov	ax,0304h
	int	31h

@@2:	mov	ecx,_app_num_objects	; deallocate selectors
	jcxz	@@4
@@3:	mov	ax,0001h
	mov	bx,_app_buf_allocsel[ecx*2]
	int	31h
	loop	@@3

@@4:	call	check_inttab
	call	restore_inttab
	call	uninstall_client_ints

@@5:	mov	ax,0001h			; free ZERO selector
	mov	bx,cs:_sel_zero
	int	31h

	xor	ax,ax
	mov	fs,ax
	mov	gs,ax
	pop	ax
	db	66h				; 32-bit jump
	jmp	cs:_int21_ip			; exit with errorcode in AL



;=============================================================================
; Find First File
;  In:	CX = file attributes
;	DS:EDX = file name
;  Out:	-
;
@__4Eh:	call	@__std
	jnz	@@err
	mov	ds,cs:_dta_sel
	mov	esi,cs:_dta_off
	mov	es,cs:_app_dta_sel
	mov	edi,cs:_app_dta_off
	mov	ecx,2Bh
	rep	movs byte ptr es:[edi],[esi]
	jmp	@__ok
@@err:	mov	[esp+1Ch],eax
	jmp	@__err



;=============================================================================
; Find Next File
;  In:	-
;  Out:	-
;
@__4Fh:	mov	ds,cs:_app_dta_sel
	mov	esi,cs:_app_dta_off
	mov	es,cs:_dta_sel
	mov	edi,cs:_dta_off
	mov	ecx,2Bh
	rep	movs byte ptr es:[edi],[esi]
	call	@__all
	jnz	@@err
	mov	ds,cs:_dta_sel
	mov	esi,cs:_dta_off
	mov	es,cs:_app_dta_sel
	mov	edi,cs:_app_dta_off
	mov	ecx,2Bh
	rep	movs byte ptr es:[edi],[esi]
	jmp	@__ok
@@err:	mov	[esp+1Ch],eax
	jmp	@__err



;=============================================================================
; Get PSP Segment
;  In:	-
;  Out:	EBX = PSP segment
;
@__51h:	movzx	eax,cs:_seg_es
	mov	[esp+10h],eax
	jmp	@__ok



;=============================================================================
; Rename File
;  In:	DS:EDX = old filename
;	ES:EDI = new filename
;  Out:	-
;
@__56h:	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+1Ch],ax
	or	ecx,-1
	xor	al,al
	repne	scas byte ptr es:[edi]
	not	ecx
	sub	edi,ecx
	mov	esi,edi
	push	ds
	push	es
	pop	ds
	mov	es,cs:_sel_ds
	mov	edi,cs:_lobufbase
	rep	movs byte ptr es:[edi],[esi]
	pop	ds
	mov	ecx,edi
	mov	ebx,cs:_lobufbase
	sub	ecx,ebx
	xchg	ecx,ebx
	push	ds
	pop	es
	mov	esi,edx
	xchg	esi,edi
	or	ecx,-1
	xor	al,al
	repne	scas byte ptr es:[edi]
	not	ecx
	sub	edi,ecx
	xchg	esi,edi
	mov	es,cs:_sel_ds
	rep	movs byte ptr es:[edi],[esi]
	mov	ax,cs:_seg_buf
	mov	[ebp+24h],ax
	mov	[ebp+22h],ax
	mov	word ptr [ebp+00h],0
	mov	[ebp+14h],bx
	call	int21h
	jmp	@__tst



;=============================================================================
; Create Temp File
;  In:	DS:EDX = file name
;  Out:	-
;
;@__5Ah:	jmp	@__39h



;=============================================================================
; Create New File
;  In:	CX = attributes
;	DS:EDX = file name
;  Out:	-
;
;@__5Bh:	jmp	@__39h



;=============================================================================
; Get PSP Selector
;  In:	-
;  Out:	BX = PSP selector
;
@__62h:	movzx	eax,cs:_sel_es
	mov	[esp+10h],eax
	jmp	@__ok



;=============================================================================
; Win95 Get Short (8.3) filename
;  In:	CL = 00/01/02h
;	CH = SUBST expansion flag
;	DS:ESI = ASCIIZ long filename or path
;	ES:EDI = 261/128/261-byte buffer for short name
; Out:	-
;
@_7160:	sub	esp,32h
	mov	ebp,esp
	push	es edi
	mov	[ebp+1Ch],ax
	mov	[ebp+18h],cx
	mov	es,cs:_sel_ds
	mov	edi,cs:_lobufbase
	add	edi,0200h
@@1:	lods	byte ptr ds:[esi]
	stos	byte ptr es:[edi]
	test	al,al
	jnz	@@1
	mov	ax,cs:_seg_buf
	mov	word ptr [ebp+24h],ax
	mov	word ptr [ebp+22h],ax
	mov	word ptr [ebp+04h],0200h
	mov	word ptr [ebp+00h],0000h
	call	int21h
	push	es
	pop	ds
	mov	esi,cs:_lobufbase
	pop	edi es
	test	byte ptr [ebp+20h],1
	jnz	@@err
@@2:	lods	byte ptr ds:[esi]
	stos	byte ptr es:[edi]
	test	al,al
	jnz	@@2
	add	esp,32h
	jmp	@__ok
@@err:	movzx	eax,word ptr [ebp+1Ch]
	add	esp,32h
	mov	[esp+1Ch],eax
	jmp	@__err



;=============================================================================
; Win95 Create or Open File
;  In:	BX = access mode
;	CX = attributes
;	DX = action
;	DI = alias hint
;	DS:ESI = ASCIIZ long filename
; Out:	AX = file handle
;	CX = action taken
;
@_716C:	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+00h],di
	mov	[ebp+10h],bx
	mov	[ebp+14h],dx
	mov	[ebp+18h],cx
	mov	[ebp+1Ch],ax
	mov	ax,cs:_seg_buf
	mov	word ptr [ebp+24h],ax
	mov	word ptr [ebp+04h],0
	mov	es,cs:_sel_ds
	mov	edi,cs:_lobufbase
@@0:	lods	byte ptr ds:[esi]
	stos	byte ptr es:[edi]
	test	al,al
	jnz	@@0
	call	int21h
	movzx	eax,word ptr [ebp+1Ch]
	movzx	ecx,word ptr [ebp+18h]
	test	byte ptr [ebp+20h],1
	lea	esp,[esp+32h]
	mov	[esp+1Ch],eax
	jnz	@__err
	mov	[esp+18h],ecx
	jmp	@__ok





;=============================================================================
; DOS/4G Identification call
;  In:	AX = 0FF00h, DX = 0078h
;  Out:	EAX = 0FFFF3447h '..4G'
;
@__FFh:	cmp	al,88h			; AX=0FF88h - DOS/32A functional call
	jz	@_FF88
	cmp	al,89h			; AX=0FF89h - DOS/32A get config
	jz	@_FF89
	cmp	al,8Ah			; AX=0FF8Ah - DOS/32A get info
	jz	@_FF8A
	cmp	al,8Dh			; AX=0FF8Dh - DOS/32A decompress data
	jz	@_FF8D
	cmp	al,8Eh			; AX=0FF8Eh - DOS/32A get Client ptrs
	jz	@_FF8E
	cmp	al,8Fh			; AX=0FF8Fh - DOS/32A resize DOS buf
	jz	@_FF8F

	cmp	al,80h			; AX=0FF80h - DOS/32A prints (magic)
	jz	@_FF80
	cmp	al,90h			; AX=0FF90h - DOS/32A get hi mem
	jz	@_FF90
	cmp	al,91h			; AX=0FF91h - DOS/32A alloc hi mem
	jz	@_FF91
	cmp	al,92h			; AX=0FF92h - DOS/32A free hi mem
	jz	@_FF92
	cmp	al,93h			; AX=0FF93h - DOS/32A resize hi mem
	jz	@_FF93

	cmp	al,94h			; AX=0FF94h - DOS/32A get lo mem
	jz	@_FF94
	cmp	al,95h			; AX=0FF95h - DOS/32A alloc lo mem
	jz	@_FF95
	cmp	al,96h			; AX=0FF96h - DOS/32A free lo mem
	jz	@_FF96
	cmp	al,97h			; AX=0FF97h - DOS/32A resize lo mem
	jz	@_FF97

	cmp	al,98h			; AX=0FF98h - DOS/32A map phys mem
	jz	@_FF98
	cmp	al,99h			; AX=0FF99h - DOS/32A free phys mem
	jz	@_FF99
	cmp	al,9Ah			; AX=0FF9Ah - DOS/32A alloc selector
	jz	@_FF9A

	cmp	dx,0078h		; DX=0078h - DOS/4G functional call
	jnz	@__go21
	mov	gs,cs:_sel_ds
	mov	dword ptr [esp+1Ch],4734FFFFh
	jmp	@__ok


;=============================================================================
; DOS/32A Identification call
;  In:	AX = 0FF88h
;  Out:	EAX = 'ID32'
;	EBX = DOS Extender version
;
@_FF88:	sub	esp,32h			; DOS/32A internal function
	mov	ebp,esp
	mov	[ebp+1Ch],ax
	call	int21h
	mov	eax,'ID32'
	movzx	ebx,cs:_version
	mov	ecx,[ebp+18h]
	mov	edx,[ebp+14h]
	mov	esi,[ebp+04h]
	mov	edi,[ebp+00h]
	mov	ebp,[esp+3Ah]
	add	esp,52h
	jmp	@__exi

;=============================================================================
; DOS/32A Get Client Configuration
;  In:	AX = 0FF89h
;  Out:	EAX = 'ID32'
;	EBX = DOS Extender version
;	ECX = size of low buffer
;	EDX = configuration bits
;	ESI = pointer to ID32 config header
;	FS = zero selector
;
@_FF89:	mov	eax,'ID32'
	movzx	esi,cs:_seg_id32
	shl	esi,4
	mov	fs,cs:_sel_zero
	movzx	ebx,cs:_version
	mov	ecx,cs:_lobufsize
	movzx	edx,word ptr cs:_misc_byte
	add	esp,20h
	jmp	@__exi

;=============================================================================
; DOS/32A Get Kernel Configuration
;  In:	AX = 0FF8Ah
;  Out:	EAX = 'ID32'
;	EBX = DOS Extender version
;	CL = CPU type
;	CH = System software
;	DL = Kernel configuration bits
;	ESI = pointer to Kernel config header
;	FS = zero selector
;
@_FF8A:	mov	eax,'ID32'
	movzx	esi,cs:_seg_kernel
	shl	esi,4
	add	esi,offs pm32_data
	mov	fs,cs:_sel_zero
	movzx	ebx,cs:_version
	mov	cl,cs:_cpu_type
	mov	ch,cs:_sys_type
	mov	dl,fs:[esi+00h]
	add	esp,20h
	jmp	@__exi

;=============================================================================
; DOS/32A Decompress data
;  In:	AX = 0FF8Dh
;	DS:EBX = source address
;	DS:EDI = destination address
;	ECX = source size
;  Out:	EAX = destination size
;
@_FF8D:	push	gs ds
	pop	gs
	mov	ds,cs:_sel_ds
	call	decompress
	mov	eax,_codesize
	sub	eax,edi
	pop	gs
	mov	[esp+1Ch],eax
	jmp	@__ok

;=============================================================================
; DOS/32A Return pointers to Client variables
;  In:	AX = 0FF8Eh
;  Out:	GS = Client data selector
;	EDX = pointer to "start" == module file name
;	ESI = pointer to loaded application sel/base table
;	EDI = pointer to Client variables structure
;
@_FF8E:	mov	gs,cs:_sel_ds
	mov	edx,offset start
	mov	esi,offset _app_buf_allocsel
	mov	edi,offset _misc_byte
	add	esp,20h
	jmp	@__exi

;=============================================================================
; DOS/32A Resize DOS transfer buffer
;  In:	AX = 0FF8Fh
;	EBX = new size of DOS transfer buffer in bytes
;  Out:	EBX = old size of DOS transfer buffer in bytes
;
@_FF8F:	mov	ds,cs:_sel_ds
	xchg	_lobufsize,ebx
	mov	[esp+10h],ebx
	jmp	@__ok


;=============================================================================
; DOS/32A Print String (Magic)
;  In:	AX = 0FF80h
;	EBX = value
;	DS:EDX = pointer to string
;  Out:	-
;
@_FF80:	mov	es,cs:_sel_ds
	mov	edi,cs:_lobufbase
	mov	esi,edx
	mov	dx,di
@@1:	lods	byte ptr ds:[esi]
	stosb
	test	al,al
	jnz	@@1
	push	es
	pop	ds
	mov	_int_ss,ss
	mov	_int_esp,esp
	lss	esp,fword ptr _sel_esp
	push	ecx
	push	ebx
	call	prints
	lss	esp,fword ptr _int_esp
	jmp	@__ok


;=============================================================================
; DOS/32A Get Free Extended Memory Information
;  In:	AX = 0FF90h
;  Out:	EAX = largest free memory block
;
@_FF90:	push	ss
	pop	es
	sub	esp,30h
	mov	edi,esp
	mov	ax,0500h
	int	31h
	mov	eax,[esp]
	add	esp,30h
	mov	[esp+1Ch],eax
	jmp	@__ok

;=============================================================================
; DOS/32A Allocate Extended Memory
;  In:	AX = 0FF91h
;	EBX = size of block
;  Out:	EBX = linear address of block
;	ESI = handle of block
;
@_FF91:	call	@_FF9x1
	mov	ax,0501h
	int	31h
	jmp	@_FF9x2

;=============================================================================
; DOS/32A Free Extended Memory
;  In:	AX = 0FF92h
;	ESI = handle of block
;  Out:	-
;
@_FF92:	call	@_FF9x1
	mov	ax,0502h
	int	31h
	jc	@__err
	jmp	@__ok

;=============================================================================
; DOS/32A Resize Extended Memory
;  In:	AX = 0FF93h
;	EBX = new size of block
;	ESI = handle of block
;  Out:	EBX = new linear address of block
;	ESI = new handle of block
;
@_FF93:	call	@_FF9x1
	mov	ax,0503h
	int	31h
	jmp	@_FF9x2

;=============================================================================
; DOS/32A Get Free DOS Memory Information
;  In:	AX = 0FF94h
;  Out:	EAX = largest free memory block
;
@_FF94:	mov	ah,48h
	mov	bx,-1
	call	@__all
	shl	ebx,4
	mov	[esp+1Ch],ebx
	jmp	@__ok

;=============================================================================
; DOS/32A Allocate DOS Memory
;  In:	AX = 0FF94h
;	EBX = size of block
;  Out:	EBX = linear address of block
;	ESI = handle of block
;
@_FF95:	add	ebx,0Fh
	shr	ebx,4
	test	ebx,0FFFF0000h
	jnz	@__err
	test	bx,bx
	jz	@__err
	mov	ah,48h
	call	@__all
	jnz	@__err
	mov	[esp+04h],eax
	shl	eax,4
	mov	[esp+10h],eax
	jmp	@__ok

;=============================================================================
; DOS/32A Free DOS Memory
;  In:	AX = 0FF96h
;	ESI = handle of block
;  Out:	-
;
@_FF96:	sub	esp,32h
	mov	ebp,esp
	mov	byte ptr [ebp+1Dh],49h
	mov	word ptr [ebp+22h],si
	call	int21h
	test	byte ptr [ebp+20h],1
	lea	esp,[esp+32h]
	jnz	@__err
	jmp	@__ok

;=============================================================================
; DOS/32A Resize DOS Memory
;  In:	AX = 0FF97h
;	EBX = new size of block
;	ESI = handle of block
;  Out:	EBX = new linear address of block
;	ESI = new handle of block
;
@_FF97:	add	ebx,0Fh
	shr	ebx,4
	test	ebx,0FFFF0000h
	jnz	@__err
	test	bx,bx
	jz	@__err
	sub	esp,32h
	mov	ebp,esp
	mov	byte ptr [ebp+1Dh],4Ah
	mov	word ptr [ebp+10h],bx
	mov	word ptr [ebp+22h],si
	call	int21h
	test	byte ptr [ebp+20h],1
	lea	esp,[esp+32h]
	jnz	@__err
	jmp	@__ok

;=============================================================================
; DOS/32A Map Physical Memory
;  In:	AX = 0FF98h
;	EBX = base of physical memory
;	ESI = size of region
;  Out:	EBX = linear address of memory
;
@_FF98:	call	@_FF9x1
	mov	ax,0800h
	int	31h
	jc	@__err
	jmp	@_FF9x3

;=============================================================================
; DOS/32A Unmap Physical Memory
;  In:	AX = 0FF99h
;	EBX = linear address of memory
;  Out:	-
;
@_FF99:	call	@_FF9x1
	mov	ax,0801h
	int	31h
	jc	@__err
	jmp	@__ok

;=============================================================================
; DOS/32A Allocate Selector
;  In:	AX = 0FF94h
;	EBX = selector base
;	ECX = selector limit
;	DX = selector access rights
;  Out:	AX = selector
;
@_FF9A:	mov	edi,ebx
	call	set_descriptor
	jc	@__err
	mov	[esp+1Ch],ax
	jmp	@__ok




;-----------------------------------------------------------------------------
@_FF9x1:mov	cx,bx			; convert BX:CX to EBX
	shr	ebx,16
	mov	di,si			; convert SI:DI to EDI
	shr	esi,16
	ret
@_FF9x2:jc	@__err
	shl	esi,16
	mov	si,di
	mov	[esp+04h],esi
@_FF9x3:shl	ebx,16
	mov	bx,cx
	mov	[esp+10h],ebx
	jmp	@__ok




;*****************************************************************************
@__cpy:	push	ds		; copy NULL-terminated string to buffer
	pop	es
	xor	ax,ax
	mov	esi,edx
	mov	edi,edx
	or	ecx,-1
	repne	scas byte ptr es:[edi]
	not	ecx
	mov	es,cs:_sel_ds
	mov	edi,cs:_lobufbase
	rep	movs byte ptr es:[edi],[esi]
	mov	ax,cs:_seg_buf
	mov	word ptr [ebp+24h],ax
	mov	word ptr [ebp+14h],0
	jmp	int21h
@__std:	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+04h],si			; these two pass-downs are
	mov	[ebp+10h],bx			; for Win95 longfilenames
	mov	[ebp+18h],cx
	mov	[ebp+1Ch],ax
	call	@__cpy
	movzx	eax,word ptr [ebp+1Ch]
	movzx	ecx,word ptr [ebp+18h]
	test	byte ptr [ebp+20h],1
	lea	esp,[esp+32h]			; restore stack
	ret
@__all:	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+10h],bx
	mov	[ebp+14h],dx
	mov	[ebp+18h],cx
	mov	[ebp+1Ch],ax
	call	int21h
	movzx	eax,word ptr [ebp+1Ch]
	movzx	ecx,word ptr [ebp+18h]
	movzx	edx,word ptr [ebp+14h]
	movzx	ebx,word ptr [ebp+10h]
	movzx	edi,word ptr [ebp+22h]		; ES
	movzx	esi,word ptr [ebp+24h]		; DS
	test	byte ptr [ebp+20h],1
	lea	esp,[esp+32h]			; restore stack
	ret
@__tst:	movzx	eax,word ptr [ebp+1Ch]		; get error number
	test	byte ptr [ebp+20h],1		; check if CF is set
	lea	esp,[esp+32h]			; restore stack
	mov	[esp+1Ch],eax			; put EAX (error) on stack
	jnz	@__err
@__ok:	popad					; return ok
@__exi:	pop	es ds
	and	byte ptr [esp+8],0FEh
	iretd
@__err:	popad					; return with error
	pop	es ds
	or	byte ptr [esp+8],01h
	iretd


PopState
