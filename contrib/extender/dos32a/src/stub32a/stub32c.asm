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

	.8086
	.MODEL tiny
	LOCALS

;=============================================================================
_ID32	segment para public use16 'CODE0'
_ID32_SIZE=16		; size exclude 'ID32' signature
;-----------------------------------------------------------------------------
db	'ID32'		; ID signature
db	00111111b	; KERNEL misc. bits:
			;   bit 0: 0=test DPMI/VCPI, 1=VCPI/DPMI	/1=def
			;   bit 1: 0=exception control off, 1=on	/1=def
			;   bit 2: 0=VCPI smart page alloc off, 1=on	/1=def
			;   bit 3: 0=VCPI+XMS alloc scheme off, 1=on	/1=def
			;   bit 4: 0=trap software INTs off, 1=on	/1=def
			;   bit 5: 0=extended blocks check off, 1=on	/1=def
			;   bit 6: reserved				/0=def
			;   bit 7: 0=ignore 4G extensions off, 1=on	/0=def
db	64		; Max. number of page tables under VCPI		/256MB
db	2		; Max. number of page tables mem_mapping	/2MB
db	16		; Max. number of real mode callbacks		/16
dw	256		; Max. number of Selectors under VCPI/XMS/raw	/256
db	8		; Real mode Stack nesting
db	8		; Protected mode Stack nesting
dw	20h		; Real mode Stack length, (in para)
dw	20h		; Protected mode Stack length, (in para)
dd	0FFFFFFFFh	; Max. extended memory to allocate in bytes
;-----------------------------------------------------------------------------
db	00111111b	; DOS/32A misc. bits:
			;   bit 0: 0=console output off, 1=on		/1=def
			;   bit 1: 0=sound generation off, 1=on		/1=def
			;   bit 2: 0=restore INT table off, 1=on	/1=def
			;   bit 3: 0=report modified INTs off, 1=on	/1=def
			;   bit 4: 0=load 16 in lowmem off, 1=on	/1=def
			;   bit 5: 0=force load 16 low off, 1=on	/1=def
			;   bit 6: 0=cls on exception off, 1=on		/0=def
			;   bit 7: 0=null-ptr protect off, 1=on		/0=def
if EXEC_TYPE eq 0
db	00001001b	; DOS/32A Pro second misc. bits
else
db	10001001b	; DOS/32A Beta second misc. bits
endif
			;   bit 0: 0=config by enironment off, 1=on	/1=def
			;   bit 1: 0=focus on this VM off, 1=on		/0=def
			;   bit 2: 0=align objects on PARA, 1=PAGE	/0=def
			;   bit 3: 0=show copyright off, 1=on		/1=def
			;   bit 4: 0=verbose mode off, 1=on		/0=def
			;   bit 5: reserved				/0=def
			;   bit 6: 0=lock configuration off, 1=on	/*
			;   bit 7: 0=Professional, 1=Beta		/*
dw	0200h		; DOS INT 21h buffer in low memory (in para)	/8 KB
dw	0000h		; Internal Version of DOS/32A (N/A for stub)
dw	0000h		; Reserved (v7.0+)
;-----------------------------------------------------------------------------
db	'STUB/32C'							,0
include	oemtitle.inf
db	'(C) 1996-98, 2002 by Narech Koumar.'				,0
db	'All Rights Reserved.'						,0
db	??date								,0
db	??time								,0

_ID32	ends





_TEXT16	segment dword public use16 'CODE1'
assume	cs:_TEXT16, ds:_TEXT16
;=============================================================================

start:	push	cs
	pop	ds
	mov	ax,es
	mov	bx,ss
	mov	_psp,ax
	sub	bx,ax
	mov	ax,sp
	shr	ax,4
	add	bx,ax
	inc	bx
	mov	ah,4Ah
	int	21h
	jc	err1

	push	ax ds				; install INT 21h
	xor	ax,ax
	mov	ds,ax
	cli
	mov	ax,ds:[21h*4+0]
	mov	cs:_ip21h,ax
	mov	ax,ds:[21h*4+2]
	mov	cs:_cs21h,ax
	mov	ax,offset int21h
	mov	ds:[21h*4+0],ax
	mov	ax,seg _TEXT16
	mov	ds:[21h*4+2],ax
	sti
	pop	ds ax


@@1:	mov	es,es:[002Ch]		; get environment
	call	make_cmd
	mov	bp,0090h
	call	exec
	call	find_path1
	call	find_path2

err0:	mov	ah,09h
	mov	dx,offset str1
	mov	byte ptr byte0,22h
	int	21h
err1:	mov	ax,4CFFh
	int	21h


;-----------------------------------------------------------------------------
make_cmd:
	push	ds es
	push	ss
	pop	ds
	xor	ax,ax
	xor	di,di
	mov	cx,7FFFh
@@1:	repne	scasb
	scasb
	jnz	@@1
	inc	di
	inc	di
	mov	si,di
	repne	scasb
	sub	di,si
	mov	cx,di
	mov	ax,di
	mov	di,0011h
	push	ss
	push	es
	pop	ds
	pop	es
	rep	movsb
	mov	byte ptr es:[di-1],20h
	mov	ds,cs:_psp
	mov	si,0081h
	mov	cl,[si-1]
	add	al,cl
	inc	cx
	mov	es:[0010h],al
	rep	movsb
	pop	es ds
	ret


;-----------------------------------------------------------------------------
find_path1:
	xor	ax,ax
	xor	di,di
@@1:	mov	cx,0007h
	mov	si,offset path1
	rep	cmpsb
	jz	@@2
	mov	cl,0FFh
	repne	scasb
	cmp	al,es:[di]
	jnz	@@1
	ret
@@2:	mov	al,20h				; skip leading spaces
	mov	cl,0FFh
	rep	scasb
	cmp	word ptr es:[di],'\:'
	jz	@@3
	ret
@@3:	dec	di
	mov	bp,0090h
@@loop:	mov	al,es:[di]
	test	al,al
	jz	@@4
	inc	di
	cmp	al,20h
	jz	@@4
	mov	[bp],al
	inc	bp
	jmp	@@loop
@@4:	mov	byte ptr [bp],'\'
	mov	word ptr [bp+1],'IB'
	mov	word ptr [bp+3],'WN'
	add	bp,5
	jmp	exec




;-----------------------------------------------------------------------------
find_path2:
	xor	ax,ax
	xor	di,di
@@1:	mov	cx,0005h
	mov	si,offset path2
	rep	cmpsb
	jz	@@2
	mov	cl,0FFh
	repne	scasb
	cmp	al,es:[di]
	jnz	@@1
	ret
@@2:	mov	bp,0090h
@@loop:	mov	al,es:[di]
	test	al,al
	jz	exec
	inc	di
	cmp	al,20h
	jz	@@loop
	cmp	al,';'
	jz	@@3
	mov	[bp],al
	inc	bp
	jmp	@@loop
@@3:	call	exec
	jmp	@@2


;-----------------------------------------------------------------------------
exec:	push	ds es di
	xor	si,si
	cmp	bp,0090h
	jz	@@0
	inc	si
	mov	byte ptr [bp],'\'
@@0:	lea	di,[bp+si]
	mov	si,offset name1
	push	ss
	pop	es
	mov	cx,11
	rep	movsb

	push	ss ss ss
	pop	ds es ax
	xor	bx,bx
	mov	word ptr ds:[0000h],bx
	mov	word ptr ds:[0002h],0010h
	mov	word ptr ds:[0004h],ax
	mov	word ptr ds:[0006h],005Ch
	mov	word ptr ds:[0008h],ax
	mov	word ptr ds:[000Ah],006Ch
	mov	word ptr ds:[000Ch],ax

	mov	dx,0090h
	mov	ax,4B00h
	int	21h
	jc	@@1

	mov	ah,4Dh
	int	21h
	mov	ah,4Ch
	int	21h

@@1:	pop	di es ds
	ret



;-----------------------------------------------------------------------------
	Align 4
int21h:	cmp	ah,4Ch
	jz	@@1
	cmp	ax,0FF87h
	jz	@@2
@@0:	jmp	dword ptr cs:_ip21h

@@1:	push	ax
	call	uninstall_int21h
	pop	ax
	jmp	@@0

@@2:	call	uninstall_int21h
	mov	ax,seg _ID32
	mov	ds,ax			; DS:SI = pointer to configuration
	xor	si,si
	mov	dx,'ID'			; DX:AX = "ID32" ID-string
	mov	ax,'32'
	iret

;-----------------------------------------------------------------------------
uninstall_int21h:
	cli
	xor	ax,ax
	mov	ds,ax
	mov	ax,cs:_ip21h
	mov	ds:[21h*4+0],ax
	mov	ax,cs:_cs21h
	mov	ds:[21h*4+2],ax
	ret



;=============================================================================
path1	db 'DOS32A='
path2	db 'PATH='
str1	db 'Cannot find file "'
name1	db 'DOS32A.EXE'
byte0	db 0,0Dh,0Ah,'$'

$theend	proc near
	db 5 dup(0)
$theend	endp


	evendata
_ip21h	dw ?
_cs21h	dw ?
_psp	dw ?


_TEXT16	ends


.STACK
;=============================================================================


end	start
