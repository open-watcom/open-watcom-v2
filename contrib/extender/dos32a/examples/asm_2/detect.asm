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
; DETECT.ASM
; This program will show how to check if DOS/32 Advanced DOS Extender and
; DOS/32 Advanced built-in DPMI are present, ie if your program is running
; under DOS/32 Advanced.
; Note that if an external DPMI host is present, DOS/32 Advanced DOS Extender
; will be installed on top of it, extending DOS API (INT 21h), Mouse API
; (INT 33h) and VGA/VESA API (INT 10h), but the built-in DPMI will not be
; installed, and the present DPMI host will be responsible for handling of
; the exceptions and for memory allocation.
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

	call	dos32a_Detect		; detect DOS Extender, use method #1
	test	eax,eax
	mov	edx,offset _msg1
	jz	@@01
	mov	edx,offset _msg2
@@01:	call	prints

	call	dos32a_GetClientConfig	; detect DOS Extender, use method #2
	test	eax,eax
	mov	edx,offset _msg3
	jz	@@02
	mov	edx,offset _msg4
@@02:	call	prints

	call	dos32a_GetKernelConfig	; detect DOS Extender, use method #3
	test	eax,eax
	mov	edx,offset _msg5
	jz	@@03
	mov	edx,offset _msg6
@@03:	call	prints

	call	dpmi32a_Detect		; detect built-in DPMI
	test	eax,eax
	mov	edx,offset _msg7
	jz	@@04
	mov	edx,offset _msg8
@@04:	call	prints

	call	api32_GetAccessGDT	; show IDT information

	mov	ax,4C00h		; return to DOS with error code 00h
	int	21h


;-----------------------------------------------------------------------------
; This subroutine will use INT 21h, AX=0FF88h from protected mode to detect
; DOS/32 Advanced. Since this function in not used by the standard real mode
; DOS, the DOS Extender will install and use it for identification purposes.
; Note that this function will be available in both real and protected modes
; when DOS/32 Advanced built-in DPMI is installed, and in protected mode only
; when an external DPMI host is present.
;
dos32a_Detect:
	mov	ax,0FF88h		; AX = DOS/32A API function 0FF88h
	int	21h			; detect DOS Extender
	cmp	eax,'ID32'		; returns: EAX = 'ID32' if DOS/32A
	mov	eax,1			; DOS Extender is present
	jnz	@@11
	mov	eax,0
@@11:	ret


;-----------------------------------------------------------------------------
; This subroutine will use INT 21h, AX=0FF89h function to detect the DOS
; Extender and to get information about its configuration. This function,
; unlike the previous, will be supported by the DOS Extender in protected
; mode only.
;
dos32a_GetClientConfig:
	mov	ax,0FF89h		; AX = DOS/32A API function 0FF89h
	int	21h			; get DOS Extender configuration
	cmp	eax,'ID32'		; returns: EAX = 'ID32' if DOS/32A
	mov	eax,1			; DOS Extender is present
	jnz	@@21
	mov	eax,0
@@21:	ret


;-----------------------------------------------------------------------------
; This subroutine will use INT 21h, AX=0FF8Ah function to detect the DOS
; Extender and to get information about its configuration. This function
; is supported by the DOS Extender in the protected mode only.
;
dos32a_GetKernelConfig:
	mov	ax,0FF8Ah		; AX = DOS/32A API function 0FF8Ah
	int	21h			; get DPMI Kernel configuration
	cmp	eax,'ID32'		; returns: EAX = 'ID32' if DOS/32A
	mov	eax,1			; DOS Extender is present
	jnz	@@31
	mov	eax,0
@@31:	ret



;-----------------------------------------------------------------------------
; This subroutine will use DPMI API function 0A00h to detect whether the
; DOS/32 Advanced built-in DPMI is installed.
;
dpmi32a_Detect:
	push	ds
	push	es
	mov	ax,0A00h		; AX = DPMI function 0A00h
	mov	esi,offset _idstr	; ESI = ptr to ID-string
	int	31h			; call DPMI
	mov	eax,1
	jc	@@41			; if CF is clear, the ID-string
	mov	eax,0			; was recognized by the DPMI
	mov	_api_cs,es
	mov	_api_ip,edi
@@41:	pop	es
	pop	ds
	ret


;-----------------------------------------------------------------------------
; This subroutine will use ADPMI extensions to show information about IDT
;
api32_GetAccessGDT:
	mov	eax,_api_ip		; check if ADPMI API extensions are
	or	ax,_api_cs		; present
	jz	@@51			; if not, jump

	mov	al,0			; AL = extended API function 00h
	call	fword ptr _api_ip	; call extended ADPMI

	mov	ebx,edi			; EBX = IDT base (argument #1)
	mov	ecx,edx			; ECX = IDT limit (argument #2)
	mov	edx,offset _msg9	; EDX = pointer to string
	call	prints			; Magic!
@@51:	ret


;-----------------------------------------------------------------------------
; This subroutine will use the "undocumented" DOS API function AX=Magic.
;
; The Magic function writes a formatted, null-terminated string to the STDOUT
; using DOS API function 09h. EBX and ECX are used to pass arguments to the
; function and DS:EDX is used as a pointer to the string. The accepted
; argument modifiers are: "%b", "%c", "%d", "%l" and "%w". You will
; have to figure out yourself what they do. The maximum *formatted* string
; length is 256 chars. The arguments are optional, of course. Also when
; passing two words as arguments, place both in EBX (high, low) leaving
; ECX unused. Feel free to experiment, and enjoy Magic!
;
prints:	mov	ax,0FF80h		; DOS API Magic function
	int	21h			; call DOS API
	ret


.DATA
;=============================================================================
_idstr	db 'SUNSYS DOS/32A',0
_msg1	db ' + DOS/32 Advanced DOS Extender is present',0Dh,0Ah,0
_msg2	db ' - DOS/32 Advanced DOS Extender is *NOT* present',0Dh,0Ah,0
_msg3	db ' + DOS/32 Advanced DOS Extender function 0FF89h is supported',0Dh,0Ah,0
_msg4	db ' - DOS/32 Advanced DOS Extender function 0FF89h is *NOT* supported',0Dh,0Ah,0
_msg5	db ' + DOS/32 Advanced DOS Extender function 0FF8Ah is supported',0Dh,0Ah,0
_msg6	db ' - DOS/32 Advanced DOS Extender function 0FF8Ah is *NOT* supported',0Dh,0Ah,0
_msg7	db ' + DOS/32 Advanced built-in DPMI is installed',0Dh,0Ah,0
_msg8	db ' - DOS/32 Advanced built-in DPMI is *NOT* installed',0Dh,0Ah,0
_msg9	db ' + IDT Base=%lh, IDT Limit=%wh',0Dh,0Ah,0
_api_ip	dd 0
_api_cs	dw 0

.STACK
;=============================================================================
	db	1000h dup(?)


end	start
