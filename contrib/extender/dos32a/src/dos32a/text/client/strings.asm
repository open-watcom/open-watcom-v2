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


.8086
;=============================================================================
; report_error:		displays error code(AX) and error message
; report_errorm:	retreives error code from memory
;
report_errorm:
	mov	ax,cs:_err_code
;
; note: this function may autimatically terminate the client
;
report_error:
	push	bx ds
	cmp	cs:_sel_ds,0
	jnz	@@1
	mov	ds,cs:_seg_ds
	jmp	@@2
@@1:	mov	ds,cs:_sel_ds
@@2:	xor	bx,bx
@@3:	cmp	ah,g_errtab[bx]		; check Global error #
	jz	@@4			; if found, jump
	cmp	bptr [bx],0FFh
	jz	@@done
	add	bx,4
	jmp	@@3

@@4:	cmp	g_errtab[bx+1],1	; check if warning
	jnz	@@5			; if not, jump
	test	_misc_byte,00000001b	; check if report warnings on
	jz	@@done			; if not, done
@@5:	push	ax cx dx
	push	bx
	mov	bl,g_errtab[bx+1]
	mov	bh,0
	mov	cx,bx
	add	bx,bx			; BX = error header #
	mov	dx,h_errtab[bx]		; DX = Header error string
	pop	bx

	push	di si			; SI, DI = event. parameters
	mov	si,wptr g_errtab[bx+2]	; SI = Local error offset
	mov	bl,al
	mov	bh,0
	add	bx,bx			; BX = Local error #
	mov	di,[bx+si]		; DX = Local error string
	test	di,di
	jz	@@6
	push	ax
	call	prints
	pop	ax
	mov	dx,di
	call	prints
	call	printcr
@@6:	pop	si di
	cmp	cl,1
	jnz	@@exit
	pop	dx cx ax
@@done:	pop	ds bx
	ret

@@exit:	mov	al,-1
	cmp	_sel_cs,0
	jnz	@@0
	jmp	exit86
@@0:	jmp	exit386




.8086
;=============================================================================
; display copyright message and version number
;
copyright:
	test	_misc_byte2,00001000b
	jz	@@done
	mov	ax,_version
	mov	bx,offs ver_msg
	mov	si,offs cpr_msg
	push	ax
	mov	al,ah
	aam
	add	al,30h
	mov	[bx+0],al
	pop	ax
	aam
	add	ax,3030h
	mov	[bx+2],ah
	mov	[bx+4],al
	mov	cx,offs cpr_end - offs cpr_msg
@@1:	lodsb
	push	cx
	xor	bx,bx
	mov	ah,0Eh
	int	10h
	pop	cx
	loop	@@1
@@done:	ret



.8086
;=============================================================================
; Console I/O Routines (a la printf...)
;=============================================================================
STRSIZE	= 0100h

@char	db	0,0
@crlf	db	cre

;=============================================================================
; Print character in AL
;
printc:
	push	dx
	mov	@char,al
	mov	dx,offs @char
	jmp	@prnt

;=============================================================================
; Print NewLine (CR,LF)
;
printcr:
	push	dx
	mov	dx,offs @crlf
@prnt:	call	prints
	pop	dx
	ret

;=============================================================================
; Print string
;	DX = offset
;	... = arguments pushed on stack
;
prints:
	push	ax bx cx dx si di bp ds es
	push	ss
	pop	es
	mov	bp,sp
	sub	sp,STRSIZE
	add	bp,2*10		; BP -> argument list
	mov	si,dx		; DS:SI -> string
	mov	di,sp		; ES:DI -> space for formatted string
	push	di
@@loop:	lodsb
	cmp	al,'%'
	jz	@@args
	cmp	al,'$'
	jnz	@@next
	mov	al,'?'
@@next:	stosb
	test	al,al
	jnz	@@loop
	pop	di
	call	writes
	add	sp,STRSIZE
	pop	es ds bp di si dx cx bx ax
	ret
@@args:	lodsb
	cmp	al,'%'
	jz	@@next
	cmp	al,'c'		; char
	jz	@@c
	cmp	al,'s'		; string
	jz	@@s
	cmp	al,'b'		; byte
	jz	@@b
	cmp	al,'w'		; word
	jz	@@w
	cmp	al,'l'		; long
	jz	@@l
	cmp	al,'d'		; decimal
	jz	@@d
	jmp	@@next		; unrecognized
@@c:	mov	al,[bp]
	add	bp,2
	jmp	@@next
@@s:	mov	bx,[bp]
	add	bp,2
@@s0:	mov	al,[bx]
	inc	bx
	test	al,al
	jz	@@loop
	cmp	al,'$'
	jnz	@@s1
	mov	al,'?'
@@s1:	stosb
	jmp	@@s0
@@b:	mov	ah,[bp]
	add	bp,2
	mov	cx,2
	call	@@hex
	jmp	@@loop
@@w:	mov	ax,[bp]
	add	bp,2
	mov	cx,4
	call	@@hex
	jmp	@@loop
@@l:	mov	ax,[bp+2]
	mov	cx,4
	call	@@hex
	mov	ax,[bp]
	add	bp,4
	mov	cx,4
	call	@@hex
	jmp	@@loop
@@d:	mov	ax,[bp]
	add	bp,2
	xor	cx,cx
	call	@@dec
	jmp	@@loop
@@hex:	rol	ax,4
	mov	bl,al
	and	bl,0Fh
	add	bl,30h
	cmp	bl,39h
	jbe	@@hex0
	add	bl,07h
@@hex0:	xchg	ax,bx
	stosb
	xchg	ax,bx
	loop	@@hex
	ret
@@dec:	mov	bx,10000
	call	@@dec0
	mov	bx,1000
	call	@@dec0
	mov	bx,100
	call	@@dec0
	mov	bx,10
	call	@@dec0
	jmp	@@dec2
@@dec0:	xor	dx,dx
	div	bx
	test	ax,ax
	jz	@@dec1
	inc	cx
@@dec1:	test	cx,cx
	jz	@@dec3
@@dec2:	add	al,30h
	stosb
@@dec3:	mov	ax,dx
	ret

;=============================================================================
writes:	xor	al,al
	mov	dx,di
	mov	cx,-1
	repne	scasb
	dec	di
	push	es
	pop	ds
	mov	ax,0924h
	mov	[di],al
	cmp	cs:_sel_cs,0
	jnz	@@0
	int	21h
	ret

.386p
;-----------------------------------------------------------------------------
@@0:	push	ebp
	sub	esp,32h
	mov	ebp,esp
	mov	[ebp+1Ch],ax
	mov	[ebp+14h],dx
	mov	ax,cs:_seg_ss
	mov	[ebp+24h],ax
	call	int21h
	add	esp,32h
	pop	ebp
	ret




.386p
;=============================================================================
file_error:
	mov	si,offs start
	jmp	report_error
file_errorm:
	mov	si,offs start
	jmp	report_errorm
dos_error:
	mov	si,ax
	mov	ax,8002h
	jmp	common_error
dpmi_error:
	mov	si,ax
	mov	ax,8003h
common_error:
	cli
	lss	esp,fptr cs:_sel_esp
	jmp	report_error


PopState
