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

; PushState


.8086
;=============================================================================
; Get default configuration from _ID32 segment (buried in the program)
; and configure the both KERNEL and CLIENT
;
get_default_config:
	push	ds
	push	es
	mov	ax,_ID32
	mov	dx,_KERNEL
	mov	_seg_id32,ax
	mov	_seg_kernel,dx

	push	ax	; STUB/32C configuration code
	push	dx
	mov	ax,0FF87h
	int	21h
	cmp	dx,'ID'
	jnz	@get_default_config_1
	cmp	ax,'32'
	jnz	@get_default_config_1
	mov	es,cs:_seg_id32
	xor	di,di
	mov	cx,12
	rep	movsw
	mov	ds,cs:_seg_ds
	or	_sys_misc,0001h		; indicate STUB/32C configuration
@get_default_config_1:	pop	dx
	pop	ax

	mov	ds,ax
	mov	es,dx
	xor	si,si			; DS:SI=_ID32:0000
	mov	di,offs pm32_data	; ES:DI=_KERNEL:PM32_DATA
	lodsw				; check for 'ID32' signature
	cmp	ax,'DI'			; if not present, skip confuguration
	jnz	@get_default_config_err
	lodsw
	cmp	ax,'23'
	jnz	@get_default_config_err
	mov	cx,_ID32_SIZE
	rep	movsb			; copy default config to KERNEL
	mov	es,cs:_seg_ds
If EXEC_TYPE eq 0
	and	wptr ds:[si],7FFFh
Else
	or	wptr ds:[si],8800h
Endif
	lodsw				; get DOS/32A config byte
	mov	wptr es:_misc_byte,ax
	lodsw				; get DOS buffer size
	mov	wptr es:_lowmembuf,ax
	lodsw				; get version
	mov	wptr es:_version,ax
	clc
	jmp	@get_default_config_done
@get_default_config_err:	stc
@get_default_config_done:	pop	es
	pop	ds
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
	push	ds
	push	es
	jc	@get_environ_config_done
	test	_misc_byte2,00000001b
	jz	@get_environ_config_done
	mov	es,_seg_env
	xor	di,di			; ES:DI=envirment
	mov	cx,-1			; environment size, unlimited
	xor	ax,ax
@get_environ_config_0:	push	cx
	mov	cx,7			; CX   =length of 'DOS32A=' string
	mov	si,offs dos_str		; DS:SI=offset of 'DOS32A=' string
	repe	cmpsb
	pop	cx
	jz	@get_environ_config_1			; quit loop if found
	repne	scasb
	cmp	al,es:[di]		; check for end of environment
	jnz	@get_environ_config_0			; loop until found or end of environ.
	jmp	@get_environ_config_done			; no string found, exit
@get_environ_config_1:	call	skip_env_spaces		; skip any leading spaces
	cmp	bptr es:[di],0		; if at the end of the line, exit
	jz	@get_environ_config_done			; (actually, just in case)
	call	get_env_word		; get word and configure
	call	skip_env_nonspaces	; skip anything else until space or 0
	cmp	bptr es:[di],0
	jnz	@get_environ_config_1
@get_environ_config_done:	pop	es
	pop	ds
	ret

get_env_word:
	xor	bx,bx			; BX=pointer to next argument in tab
@get_env_word_0:	mov	si,dfn_tab[bx]		; DS:SI=string offset
	cmp	si,-1			; check if at end of arg. list
	jz	@get_env_word_1			; if yes, terminate search
	mov	cx,dfn_tab[bx+2]	; CX=get string length
	push	di
	repe	cmpsb			; compare strings
	pop	di
	jz	@get_env_word_2			; if equ, process argument
	add	bx,6			; loop until done
	jmp	@get_env_word_0
@get_env_word_1:	ret
@get_env_word_2:	add	di,dfn_tab[bx+2]	; adjust env pointer by string length
	jmp	cs:dfn_tab[bx-2+6]	; goto appropriate argument handler

skip_env_spaces:
@skip_env_spaces_1:	mov	al,es:[di]
	test	al,al
	jz	@skip_env_spaces_3
	cmp	al,'/'
	jz	@skip_env_spaces_2
	cmp	al,20h
	jnz	@skip_env_spaces_3
	inc	di
	jmp	@skip_env_spaces_1
@skip_env_spaces_2:	inc	di
@skip_env_spaces_3:	ret

skip_env_nonspaces:
@skip_env_nonspaces_1:	mov	al,es:[di]
	test	al,al
	jz	@skip_env_nonspaces_3
	cmp	al,'/'
	jz	@skip_env_nonspaces_2
	cmp	al,20h
	jz	@skip_env_nonspaces_3
	inc	di
	jmp	@skip_env_nonspaces_1
@skip_env_nonspaces_2:	inc	di
@skip_env_nonspaces_3:	ret



;=============================================================================
; Get switch in AX: 0, 1, ON, OFF	(CF=1 if not found)
;
get_env_swc:
	cmp	bptr es:[di],':'	; skip ':' if present
	jne	@get_env_swc_1
	inc	di
@get_env_swc_1:	xor	ax,ax			; if '0', return(0)
	cmp	bptr es:[di],'0'	; check for '0'=OFF
	jz	@get_env_swc_x1
	inc	ax			; if '1', return(1)
	cmp	bptr es:[di],'1'	; check for '1'=ON
	jz	@get_env_swc_x1
	cmp	wptr es:[di],'NO'	; check for 'ON'
	jz	@get_env_swc_x2
	dec	ax
	cmp	wptr es:[di],'FO'	; check for 'OF'(F)
	jnz	@get_env_swc_x0
	cmp	bptr es:[di+2],'F'	; check for (OF)'F'
	jz	@get_env_swc_x3
@get_env_swc_x0:	stc
	ret
@get_env_swc_x3:	inc	di
@get_env_swc_x2:	inc	di
@get_env_swc_x1:	inc	di
	test	al,al
	ret



;=============================================================================
; Get number in AX: 0<=N(dec)<=65535	(CF=1 if not found)
;
get_env_num:
	cmp	bptr es:[di],':'	; skip ':' if present
	jne	@get_env_num_1
	inc	di
@get_env_num_1:	xor	ax,ax
	xor	bx,bx
	mov	cx,10
	mov	al,es:[di]
	sub	al,'0'
	jb	@get_env_num_exit
	cmp	al,9
	ja	@get_env_num_exit
	xchg	bx,ax
	mul	cx
	xchg	bx,ax
	add	bx,ax
@get_env_num_2:	inc	di
	xor	ax,ax
	mov	al,es:[di]
	sub	al,'0'
	jb	@get_env_num_done
	cmp	al,9
	ja	@get_env_num_done
	xchg	bx,ax
	mul	cx
	xchg	bx,ax
	add	bx,ax
	jmp	@get_env_num_2
@get_env_num_done:	mov	ax,bx
	clc
	ret
@get_env_num_exit:	stc
	ret





;=============================================================================
; /QUIET
;
cfg_env_quiet:
If EXEC_TYPE eq 0
	and	wptr _misc_byte,1111011111111100b
Else
	and	wptr _misc_byte,1111111111111100b
Endif
	ret



;=============================================================================
; /PRINT:ON|OFF
;
cfg_env_print:
	call	get_env_swc
	jc	@cfg_env_print_0
	jz	@cfg_env_print_1
	or	wptr _misc_byte,0000100000000001b
	ret
@cfg_env_print_1:	and	wptr _misc_byte,1111011111111110b
@cfg_env_print_0:	ret



;=============================================================================
; SOUND:ON|OFF
;
cfg_env_sound:
	call	get_env_swc
	jc	@cfg_env_sound_0
	jz	@cfg_env_sound_1
	or	_misc_byte,00000010b
	ret
@cfg_env_sound_1:	and	_misc_byte,11111101b
@cfg_env_sound_0:	ret



;=============================================================================
; /EXTMEM:nnnn (KB)
;
cfg_env_extmem:
	call	get_env_num
	jc	@cfg_env_extmem_0
	push	ds
	mov	ds,_seg_kernel
	assume	ds:_KERNEL
	push	ax
	mov	bx,1024				; make AX (Kb)-> DX:AX (bytes)
	mul	bx				; DX:AX= bytes
	mov	wptr ds:[pm32_data+12],ax	; set ext mem requirements
	mov	wptr ds:[pm32_data+14],dx
	pop	ax
	add	ax,00FFFh
	and	ax,0F000h
	xor	dx,dx
	mov	bx,1000h			; BX=Page size in KB
	div	bx				; MEM KB / Page Size KB
	test	al,al
	jnz	@cfg_env_extmem_1				; must alloc at least one
	inc	al
@cfg_env_extmem_1:	mov	bptr ds:[pm32_data+1],al	; set max allowed pagetables
	assume	ds:_TEXT16
	pop	ds
@cfg_env_extmem_0:	ret



;=============================================================================
; /DOSBUF:nnnn (KB)
;
cfg_env_dosbuf:
	call	get_env_num
	cmp	ax,1				; 1KB < ax < 64KB
	jb	@cfg_env_dosbuf_0
	cmp	ax,64
	ja	@cfg_env_dosbuf_0
	jnz	@cfg_env_dosbuf_2
	mov	ax,0FFFh
	jmp	@cfg_env_dosbuf_1
@cfg_env_dosbuf_2:	mov	cl,6
	shl	ax,cl				; convert KB to para
@cfg_env_dosbuf_1:	mov	_lowmembuf,ax
@cfg_env_dosbuf_0:	ret



;=============================================================================
; /DPMITST:ON|OFF
;
cfg_env_test:
	call	get_env_swc
;	jc	@cfg_env_test_0
;	push	ds
;	mov	ds,_seg_kernel
;	assume	ds:_KERNEL
;	jz	@cfg_env_test_1
;	or	bptr ds:pm32_data[0],00000001b
;	pop	ds
;	ret
;@cfg_env_test_1:	and	bptr ds:pm32_data[0],11111110b
;	assume	ds:_TEXT16
;	pop	ds
@cfg_env_test_0:	ret



;=============================================================================
; /RESTORE:ON|OFF
;
cfg_env_restore:
	call	get_env_swc
	jc	@cfg_env_restore_0
	jz	@cfg_env_restore_1
	or	_misc_byte,00000100b
	ret
@cfg_env_restore_1:	and	_misc_byte,11111011b
@cfg_env_restore_0:	ret



;=============================================================================
; /NULLP[:ON|OFF]
;
cfg_env_nullp:
	call	get_env_swc
	jc	@cfg_env_nullp_0
	jz	@cfg_env_nullp_1
@cfg_env_nullp_0:	or	_misc_byte,10000000b
	ret
@cfg_env_nullp_1:	and	_misc_byte,01111111b
	ret



;=============================================================================
; /VERBOSE[:ON:OFF]
;
cfg_env_verbose:
	call	get_env_swc
	jc	@cfg_env_verbose_0
	jz	@cfg_env_verbose_1
@cfg_env_verbose_0:	or	_misc_byte2,00010000b
	ret
@cfg_env_verbose_1:	and	_misc_byte2,11101111b
	ret



;=============================================================================
; /NOWARN:nnnn
;
cfg_env_nowarn:
	call	get_env_num
	sub	ax,9000
	jb	@cfg_env_nowarn_0
	cmp	al,6
	ja	@cfg_env_nowarn_0
	add	ax,ax
	mov	bx,ax
	mov	wptr errtab_90xx[bx],0
	cmp	bptr es:[di],','
	jnz	@cfg_env_nowarn_0
	inc	di
	jmp	cfg_env_nowarn
@cfg_env_nowarn_0:	ret



;=============================================================================
; /NOC
;
cfg_env_noc:
	and	bptr _misc_byte2,11110111b
	ret

	.386p
