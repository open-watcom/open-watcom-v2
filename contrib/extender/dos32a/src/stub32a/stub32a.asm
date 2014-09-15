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

	.286
	.MODEL tiny
	LOCALS

.CODE
;=============================================================================
db	'STUB/32A'							,0
db	'Copyright (C) 1996-2006 by Narech K.'				,0
db	??date								,0
db	??time								,0


start:	sti
	push	cs
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







;=============================================================================
path1	db 'DOS32A='
path2	db 'PATH='
str1	db 'Cannot find file "'
name1	db 'DOS32A.EXE'
byte0	db 0,0Dh,0Ah,'$'

$theend	proc near
	db 2 dup(0)
$theend	endp


_psp	dw ?



.STACK
;=============================================================================


end	start
