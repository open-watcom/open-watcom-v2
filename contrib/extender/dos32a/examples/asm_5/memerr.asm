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
; MEMERR.ASM
; This program shows how the DOS Extender keeps track on whether the Extended
; memory blocks are valid or they have been corrupted (overwritten).
; NOTE: this program will not work correctly under external DPMI.
;
;*****************************************************************************

	.386p
	.MODEL flat

DGROUP	group _DATA,STACK		; Tasm will complain about STACK here

.CODE
;=============================================================================
start:
	push	ds			; make ES=DS
	pop	es

	mov	ax,0FF91h		; allocate memory (use DOS/32 Advanced
	mov	ebx,1000h		;  specific mem allocation functions)
	int	21h

	mov	edi,ebx			; fill memory with 0FFh
	mov	ecx,1001h		; Note: 1000h + 1 bytes to force
	mov	eax,-1			;  overwriting of next block header
	rep	stosb

				; Note: the DOS Extender cannot detect that
				; memory blocks have been corrupted unless
				; you call an extended memory related DPMI
				; function (any 05xxh function will do)

	mov	ax,0FF92h		; try to deallocate memory
	int	21h			;  at this point DOS/32 Advanced will
					;  terminate the application with an
					;  error

	mov	ah,9			; this code should never be executed
	mov	edx,offset _str1
	int	21h
	mov	ax,4C00h
	int	21h


.DATA
;=============================================================================
_str1	db 0Dh,0Ah
	db 'You should never see this!!!',0Dh,0Ah,'$'


.STACK
;=============================================================================
	db	1000h dup(?)


end	start
