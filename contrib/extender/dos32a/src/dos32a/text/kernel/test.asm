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
; Low-level debugging support for beta releases
;=============================================================================

hexal:	push	dx
	mov	dx,offs hexbuf+6
	jmp	mkhex
hexax:	push	dx
	mov	dx,offs hexbuf+4
	jmp	mkhex
hexeax:	push	dx
	mov	dx,offs hexbuf
mkhex:	push	eax cx si di ds dx
	smsw	si
	test	si,1
	jz	@@a
	mov	ds,cs:seldata
	jmp	@@b
@@a:	mov	ds,cs:kernel_code
@@b:	mov	di,7
@@0:	mov	si,ax
	and	si,000Fh
	mov	cl,cs:hextab[si]
	mov	ds:hexbuf[di],cl
	shr	eax,4
	dec	di
	jns	@@0
	pop	si
	mov	cx,offset hexbuf+10
	sub	cx,si
@@c:	lodsb
	mov	dl,al
	mov	ah,2
	int	21h
	loop	@@c
	pop	ds di si cx eax dx
	ret
hextab	db '0123456789ABCDEF'
hexbuf	db '        ',13,10
wait:	push ax
	xor ax,ax
	int 16h
	pop ax
	ret
@_tone:	push	ax cx dx		; AX=frequency, CX=time
	mov	dx,0540h
	mov	cx,9FFFh
	mov	al,0B6h			; set frequency
	out	43h,al
	mov	al,dl			; fLow
	out	42h,al
	mov	al,dh			; fHigh
	out	42h,al
	in	al,61h			; beep on
	or	al,03h
	out	61h,al
@@loop:	mov	ax,0001h
	cwd
	div	ax
	div	ax
	div	ax
	loop	@@loop
	in	al,61h			; beep off
	and	al,not 03h
	out	61h,al
	pop	dx cx ax
@@done:	ret


show_debug_string:
	pusha
	mov	ah,9
	mov	dx,offset _debug_string
	int	21h
	popa
	ret
_debug_string	label byte
db	'Kernel Module, built-in DPMI host 0.9A',0Dh,0Ah
db	'Debug Version, build ',??date,' ',??time,0Dh,0Ah
db	0Dh,0Ah,'$'
