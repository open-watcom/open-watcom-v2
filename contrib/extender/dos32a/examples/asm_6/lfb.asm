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

;*****************************************************************************
;
; LFB.ASM
; This example will show how to use physical memory mapping under DOS/32A
; and how to access VBE Linear Framebuffer.
;
;*****************************************************************************

	.386p
	.MODEL flat

DGROUP	group _DATA,STACK		; Tasm will complain about STACK here

.CODE
;=============================================================================
start:
	push	ds			; make ES = DS
	pop	es


	mov	eax,4F01h		; VBE function - get videomode info
	mov	ecx,0111h		; videomode 0111h, 640*480*16bpp
	mov	edi,offset _mib_buffer	; offset of Mode Info Block buffer
	int	10h
	cmp	eax,004Fh		; check if VBE call is supported
	jnz	error1			; die if not

	mov	al,byte ptr _mib_buffer[00h]	; check if LFB is supported
	test	al,80h
	jz	error2

	mov	eax,dword ptr _mib_buffer[28h]	; get physical addr of LFB
	mov	_lfb_physbase,eax

	mov	eax,0FF98h		; map LFB into linear memory space
	mov	ebx,_lfb_physbase	;  using DOS/32A specific functions
	mov	esi,_lfb_physsize	; you can substitute this with a call
	int	21h			;  to DPMI function 0800h, of course
	jc	error3			; exit if could not map phys. memory
	mov	_lfb_linebase,ebx

	mov	eax,4F02h		; set videomode 0111h, 640*480*16bpp
	mov	ebx,4111h		; note 0111h OR 4000h (LFB mode)
	int	10h
	cmp	eax,004Fh		; if the call was unsuccessful,
	jnz	error4			;  return to DOS


	xor	ebx,ebx

@@0:	mov	eax,ebx			; display something on the screen
	mov	edi,_lfb_linebase
	mov	ecx,_lfb_linesize
@@1:	stosw
	inc	eax
	dec	ecx
	jnz	@@1
	inc	ebx
	mov	eax,0100h		; check if a key has been pressed
	int	16h
	jz	@@0

	xor	eax,eax			; remove char from keyboard buffer
	int	16h


	mov	eax,0003h		; set the famous mode 03h
	int	10h

	mov	eax,0FF99h		; free mapped linear memory
	mov	ebx,_lfb_linebase
	int	21h

	mov	eax,0FF80h		; show information about LFB
	mov	ebx,_lfb_physbase
	mov	edx,offset _exitmsg1
	int	21h
	mov	ebx,_lfb_linebase
	mov	edx,offset _exitmsg2
	int	21h
	mov	ebx,_lfb_physsize
	mov	edx,offset _exitmsg3
	int	21h

	mov	eax,4C00h		; happily exit to DOS
	int	21h



;-----------------------------------------------------------------------------
error1:	mov	edx,offset _errmsg1
	jmp	exit

error2:	mov	edx,offset _errmsg2
	jmp	exit

error3:	mov	edx,offset _errmsg3
	jmp	exit

error4:	mov	edx,offset _errmsg4

exit:	mov	eax,0FF80h		; display error message and exit
	int	21h
	mov	eax,4CFFh
	int	21h




.DATA
;=============================================================================
_lfb_physbase	dd 0			; physical address of LFB
_lfb_physsize	dd 640*480*2		; size of LFB for mode 0111h
_lfb_linebase	dd 0			; linear address of LFB
_lfb_linesize	dd 640*480		; size of LFB / 2 (16bit per colour)

_errmsg1	db 'Error: VBE functions not supported!',13,10,0
_errmsg2	db 'Error: VBE Linear Framebuffer not supported!',13,10,0
_errmsg3	db 'Error: cannot map LFB into linear memory!',13,10,0
_errmsg4	db 'Error: cannot set videomode 0111h (w/ LFB on)!',13,10,0

_exitmsg1	db 'Linear Framebuffer physical base: %lh',13,10,0
_exitmsg2	db 'Linear Framebuffer linear base:   %lh',13,10,0
_exitmsg3	db 'Linear Framebuffer size:          %lh',13,10,0



.DATA?
;=============================================================================
_mib_buffer	db 512 dup(?)		; Mode Info Block buffer



.STACK
;=============================================================================
	db	1000h dup(?)		; no comment here...



end	start
