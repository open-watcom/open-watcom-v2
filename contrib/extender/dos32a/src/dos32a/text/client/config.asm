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


.8086
;=============================================================================
; Get default configuration from _ID32 segment (buried in the program)
; and configure the both KERNEL and CLIENT
;
get_default_config:
	push	ds es
	mov	ax,_ID32
	mov	dx,_KERNEL
	mov	_seg_id32,ax
	mov	_seg_kernel,dx

	push	ax dx			; STUB/32C configuration code
	mov	ax,0FF87h
	int	21h
	cmp	dx,'ID'
	jnz	@@1
	cmp	ax,'32'
	jnz	@@1
	mov	es,cs:_seg_id32
	xor	di,di
	mov	cx,12
	rep	movsw
	mov	ds,cs:_seg_ds
	or	_sys_misc,0001h		; indicate STUB/32C configuration
@@1:	pop	dx ax

	mov	ds,ax
	mov	es,dx
	xor	si,si			; DS:SI=_ID32:0000
	mov	di,offs pm32_data	; ES:DI=_KERNEL:PM32_DATA
	lodsw				; check for 'ID32' signature
	cmp	ax,'DI'			; if not present, skip confuguration
	jnz	@@err
	lodsw
	cmp	ax,'23'
	jnz	@@err
	mov	cx,_ID32_SIZE
	rep	movsb			; copy default config to KERNEL
	mov	es,cs:_seg_ds
If EXEC_TYPE eq 0
	and	word ptr ds:[si],7FFFh
Else
	or	word ptr ds:[si],8800h
Endif
	lodsw				; get DOS/32A config byte
	mov	word ptr es:_misc_byte,ax
	lodsw				; get DOS buffer size
	mov	word ptr es:_lowmembuf,ax
	lodsw				; get version
	mov	word ptr es:_version,ax
	clc
	jmp	@@done
@@err:	stc
@@done:	pop	es ds
	ret


	evendata
@area2_db label byte
@area2_dw label word
@area2_dd label dword

;=============================================================================
; Get environment configuration from the environment segment (at PSP:002C)
; and configure both KERNEL and DOS/32A
;
get_environ_config:
	push	ds es
	jc	@@done
	test	_misc_byte2,00000001b
	jz	@@done
	mov	es,_seg_env
	xor	di,di			; ES:DI=envirment
	mov	cx,-1			; environment size, unlimited
	xor	ax,ax
@@0:	push	cx
	mov	cx,7			; CX   =length of 'DOS32A=' string
	mov	si,offs dos_str		; DS:SI=offset of 'DOS32A=' string
	repe	cmpsb
	pop	cx
	jz	@@1			; quit loop if found
	repne	scasb
	cmp	al,es:[di]		; check for end of environment
	jnz	@@0			; loop until found or end of environ.
	jmp	@@done			; no string found, exit
@@1:	call	skip_env_spaces		; skip any leading spaces
	cmp	byte ptr es:[di],0	; if at the end of the line, exit
	jz	@@done			; (actually, just in case)
	call	get_env_word		; get word and configure
	call	skip_env_nonspaces	; skip anything else until space or 0
	cmp	byte ptr es:[di],0
	jnz	@@1
@@done:	pop	es ds
	ret

get_env_word:
	xor	bx,bx			; BX=pointer to next argument in tab
@@0:	mov	si,dfn_tab[bx]		; DS:SI=string offset
	cmp	si,-1			; check if at end of arg. list
	jz	@@1			; if yes, terminate search
	mov	cx,dfn_tab[bx+2]	; CX=get string length
	push	di
	repe	cmpsb			; compare strings
	pop	di
	jz	@@2			; if equ, process argument
	add	bx,6			; loop until done
	jmp	@@0
@@1:	ret
@@2:	add	di,dfn_tab[bx+2]	; adjust env pointer by string length
	jmp	cs:dfn_tab[bx-2+6]	; goto appropriate argument handler

skip_env_spaces:
@@1:	mov	al,es:[di]
	test	al,al
	jz	@@3
	cmp	al,'/'
	jz	@@2
	cmp	al,20h
	jnz	@@3
	inc	di
	jmp	@@1
@@2:	inc	di
@@3:	ret

skip_env_nonspaces:
@@1:	mov	al,es:[di]
	test	al,al
	jz	@@3
	cmp	al,'/'
	jz	@@2
	cmp	al,20h
	jz	@@3
	inc	di
	jmp	@@1
@@2:	inc	di
@@3:	ret



;=============================================================================
; Get switch in AX: 0, 1, ON, OFF	(CF=1 if not found)
;
get_env_swc:
	cmp	byte ptr es:[di],':'	; skip ':' if present
	jne	@@1
	inc	di
@@1:	xor	ax,ax			; if '0', return(0)
	cmp	byte ptr es:[di],'0'	; check for '0'=OFF
	jz	@@x1
	inc	ax			; if '1', return(1)
	cmp	byte ptr es:[di],'1'	; check for '1'=ON
	jz	@@x1
	cmp	word ptr es:[di],'NO'	; check for 'ON'
	jz	@@x2
	dec	ax
	cmp	word ptr es:[di],'FO'	; check for 'OF'(F)
	jnz	@@x0
	cmp	byte ptr es:[di+2],'F'	; check for (OF)'F'
	jz	@@x3
@@x0:	stc
	ret
@@x3:	inc	di
@@x2:	inc	di
@@x1:	inc	di
	test	al,al
	ret



;=============================================================================
; Get number in AX: 0<=N(dec)<=65535	(CF=1 if not found)
;
get_env_num:
	cmp	byte ptr es:[di],':'	; skip ':' if present
	jne	@@1
	inc	di
@@1:	xor	ax,ax
	xor	bx,bx
	mov	cx,10
	mov	al,es:[di]
	sub	al,'0'
	jb	@@exit
	cmp	al,9
	ja	@@exit
	xchg	bx,ax
	mul	cx
	xchg	bx,ax
	add	bx,ax
@@2:	inc	di
	xor	ax,ax
	mov	al,es:[di]
	sub	al,'0'
	jb	@@done
	cmp	al,9
	ja	@@done
	xchg	bx,ax
	mul	cx
	xchg	bx,ax
	add	bx,ax
	jmp	@@2
@@done:	mov	ax,bx
	clc
	ret
@@exit:	stc
	ret





;=============================================================================
; /QUIET
;
cfg_env_quiet:
If EXEC_TYPE eq 0
	and	word ptr _misc_byte,1111011111111100b
Else
	and	word ptr _misc_byte,1111111111111100b
Endif
	ret



;=============================================================================
; /PRINT:ON|OFF
;
cfg_env_print:
	call	get_env_swc
	jc	@@0
	jz	@@1
	or	word ptr _misc_byte,0000100000000001b
	ret
@@1:	and	word ptr _misc_byte,1111011111111110b
@@0:	ret



;=============================================================================
; SOUND:ON|OFF
;
cfg_env_sound:
	call	get_env_swc
	jc	@@0
	jz	@@1
	or	_misc_byte,00000010b
	ret
@@1:	and	_misc_byte,11111101b
@@0:	ret



;=============================================================================
; /EXTMEM:nnnn (KB)
;
cfg_env_extmem:
	call	get_env_num
	jc	@@0
	push	ds
	mov	ds,_seg_kernel
	assume	ds:_KERNEL
	push	ax
	mov	bx,1024				; make AX (Kb)-> DX:AX (bytes)
	mul	bx				; DX:AX= bytes
	mov	word ptr ds:[pm32_data+12],ax	; set ext mem requirements
	mov	word ptr ds:[pm32_data+14],dx
	pop	ax
	add	ax,00FFFh
	and	ax,0F000h
	xor	dx,dx
	mov	bx,1000h			; BX=Page size in KB
	div	bx				; MEM KB / Page Size KB
	test	al,al
	jnz	@@1				; must alloc at least one
	inc	al
@@1:	mov	byte ptr ds:[pm32_data+1],al	; set max allowed pagetables
	assume	ds:_TEXT16
	pop	ds
@@0:	ret



;=============================================================================
; /DOSBUF:nnnn (KB)
;
cfg_env_dosbuf:
	call	get_env_num
	cmp	ax,1				; 1KB < ax < 64KB
	jb	@@0
	cmp	ax,64
	ja	@@0
	jnz	@@2
	mov	ax,0FFFh
	jmp	@@1
@@2:	mov	cl,6
	shl	ax,cl				; convert KB to para
@@1:	mov	_lowmembuf,ax
@@0:	ret



;=============================================================================
; /DPMITST:ON|OFF
;
cfg_env_test:
	call	get_env_swc
	jc	@@0
	push	ds
	mov	ds,_seg_kernel
	assume	ds:_KERNEL
	jz	@@1
	or	byte ptr ds:pm32_data[0],00000001b
	pop	ds
	ret
@@1:	and	byte ptr ds:pm32_data[0],11111110b
	assume	ds:_TEXT16
	pop	ds
@@0:	ret



;=============================================================================
; /RESTORE:ON|OFF
;
cfg_env_restore:
	call	get_env_swc
	jc	@@0
	jz	@@1
	or	_misc_byte,00000100b
	ret
@@1:	and	_misc_byte,11111011b
@@0:	ret



;=============================================================================
; /NULLP[:ON|OFF]
;
cfg_env_nullp:
	call	get_env_swc
	jc	@@0
	jz	@@1
@@0:	or	_misc_byte,10000000b
	ret
@@1:	and	_misc_byte,01111111b
	ret



;=============================================================================
; /VERBOSE[:ON:OFF]
;
cfg_env_verbose:
	call	get_env_swc
	jc	@@0
	jz	@@1
@@0:	or	_misc_byte2,00010000b
	ret
@@1:	and	_misc_byte2,11101111b
	ret



;=============================================================================
; /NOWARN:nnnn
;
cfg_env_nowarn:
	call	get_env_num
	sub	ax,9000
	jb	@@0
	cmp	al,6
	ja	@@0
	add	ax,ax
	mov	bx,ax
	mov	word ptr errtab_90xx[bx],0
	cmp	byte ptr es:[di],','
	jnz	@@0
	inc	di
	jmp	cfg_env_nowarn
@@0:	ret



;=============================================================================
; /NOC
;
cfg_env_noc:
	and	byte ptr _misc_byte2,11110111b
	ret

PopState
