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

.CODE

decode:	push	eax ebx ecx edx edi ebp

	mov	eax,20202020h		; clear text buffers
	mov	ecx,20
	mov	edi,offs cmdbuf
	rep	stosd
	mov	ecx,20
	mov	edi,offs textbuf
	rep	stosd
	mov	ecx,20
	mov	edi,offs commbuf
	rep	stosd
	clr	eax
	mov	cmdbuf+80,al
	mov	textbuf+80,al
	mov	commbuf+80,al
	mov	cmdadr,offs cmdbuf
	mov	textadr,offs textbuf
	mov	commadr,offs commbuf

	mov	_jump_taken,al		; initialize variables
	mov	_prefix_esp,al

	mov	eax,esi			; print eff. address
	mov	edi,textadr
	cmp	_show_unreloc,0
	jz	@@1
	call	show_unreloc_addr
	jc	@@1
	jmp	@@4
@@1:	cmp	_adrsiz,0
	jz	@@2
	mov	dptr [edi],':sc '
	add	textadr,4
	call	text_makehexax
	jmp	@@3
@@2:	call	text_makehexeax
@@3:	inc	textadr			; add one space

@@4:	call	put_textal		; get first byte (op-code)
	movzx	ebx,al
	shr	ebx,4			; ebx = 0F0h masked opcode (group)
	mov	edi,cmdadr
	call	_grouptab[ebx*4]	; goto choosen group
	mov	bptr [edi],0
	call	merge_text
	pop	ebp edi edx ecx ebx eax
_empty:	ret

decode2:push	eax ebx ecx edx
	call	put_textal		; get first byte (op-code)
	movzx	ebx,al
	shr	ebx,4			; ebx = 0F0h masked opcode (group)
	call	_grouptab[ebx*4]	; goto choosen group
	pop	edx ecx ebx eax
	ret

show_unreloc_addr:
	clr	ebx
	mov	dx,wptr _cs
@@0:	cmp	_obj_selector[ebx*2],0
	jz	@@err
	cmp	dx,_obj_selector[ebx*2]
	jz	@@1
	inc	ebx
	jmp	@@0
@@1:	sub	eax,_obj_address[ebx*4]
	call	text_makehexeax
	lea	eax,[ebx+1]
	and	eax,3Fh
	cmp	eax,09h
	lea	eax,[eax+30h]
	jbe	@@2
	lea	eax,[eax+07h]
@@2:	mov	ah,':'
	mov	[edi],ax
	inc	textadr			; add one space
	clc
	ret
@@err:	stc
	ret


;=============================================================================
makehexal:
	push	ecx esi edi
	mov	edi,offs hexbuf
	call	makehex
	lea	esi,[edi+6]
	mov	ecx,2
	pop	edi
	rep	movsb
	pop	esi ecx
	ret
makehexax:
	push	ecx esi edi
	mov	edi,offs hexbuf
	call	makehex
	lea	esi,[edi+4]
	mov	ecx,4
	pop	edi
	rep	movsb
	pop	esi ecx
	ret
makehexeax:
	push	ecx esi edi
	mov	edi,offs hexbuf
	call	makehex
	mov	esi,edi
	mov	ecx,8
	pop	edi
	rep	movsb
	pop	esi ecx
	ret
makehexword:
	test	ax,0FF00h
	jz	makehexal
	jmp	makehexax
makehexdword:
	test	eax,0FFFFFF00h
	jz	makehexal
	jmp	makehexeax
text_makehexal:
	push	edi
	mov	edi,textadr
	call	makehexal
	mov	textadr,edi
	pop	edi
	ret
text_makehexax:
	push	edi
	mov	edi,textadr
	call	makehexax
	mov	textadr,edi
	pop	edi
	ret
text_makehexeax:
	push	edi
	mov	edi,textadr
	call	makehexeax
	mov	textadr,edi
	pop	edi
	ret
put_textal:
	call	load_byte
	cmp	_showmode_flag,0
	jnz	@@done
	call	text_makehexal
@@done:	inc	esi
	ret
put_textax:
	inc	esi
	call	load_byte
	dec	esi
	mov	ah,al
	call	load_byte
	cmp	_showmode_flag,0
	jnz	@@done
	xchg	ah,al
	call	text_makehexax
	xchg	ah,al
@@done:	add	esi,2
	ret
put_texteax:
	add	esi,3
	call	load_byte
	dec	esi
	shl	eax,8
	call	load_byte
	dec	esi
	shl	eax,8
	call	load_byte
	dec	esi
	shl	eax,8
	call	load_byte
	cmp	_showmode_flag,0
	jnz	@@done
	bswap	eax
	call	text_makehexeax
	bswap	eax
@@done:	add	esi,4
	ret
@bswap:	push	edx
	ror	eax,16
	mov	dh,al
	mov	dl,ah
	rol	edx,16
	ror	eax,16
	mov	dh,al
	mov	dl,ah
	rol	edx,16
	mov	eax,edx
	pop	edx
	ret

put_commaddr:
	push	eax ebx edi
	mov	edi,commadr
	movzx	ebx,_prefix
	cmp	bl,3			; if prefix is not DS
	jnz	@@0			; then do not check for SS
	cmp	_prefix_esp,0
	jz	@@0
	mov	bl,02h			; SS:[ xxx ]
@@0:	lea	edx,_segtab86[ebx*4]
	call	put_string
	call	put_colon
	cmp	_adrsiz,0
	jz	@@l1
	movzx	eax,ax
	call	makehexax
	jmp	@@l2
@@l1:	call	makehexeax
@@l2:	mov	_refaddr,eax
	mov	dptr [edi],'  = '
	add	edi,3
	mov	commadr,edi
	mov	_refshow,1
	pop	edi ebx eax
	ret
put_memx_err:
	call	put_string
	mov	commadr,edi
	mov	wptr [edi],0068h
	pop	edi esi eax
	ret
put_memb:
	cmp	_refshow,0
	jz	@@done
	push	eax esi edi
	mov	esi,_refaddr
	mov	edi,commadr
	mov	edx,offs err_addr_b
	call	get_code_byte
	jnc	put_memx_err
	call	makehexal
	mov	commadr,edi
	mov	wptr [edi],0068h
	pop	edi esi eax
@@done:	ret
put_memw:
	cmp	_refshow,0
	jz	@@done
	push	eax esi edi
	mov	esi,_refaddr
	mov	edi,commadr
	mov	edx,offs err_addr_w
	inc	esi
	call	get_code_byte
	jnc	put_memx_err
	dec	esi
	mov	ah,al
	call	get_code_byte
	jnc	put_memx_err
	call	makehexax
	mov	commadr,edi
	mov	wptr [edi],0068h
	pop	edi esi eax
@@done:	ret
put_memd:
	cmp	_refshow,0
	jz	@@done
	push	eax esi edi
	mov	esi,_refaddr
	mov	edi,commadr
	mov	edx,offs err_addr_d
	add	esi,3
	call	get_code_byte
	jnc	put_memx_err
	dec	esi
	shl	eax,8
	call	get_code_byte
	jnc	put_memx_err
	dec	esi
	shl	eax,8
	call	get_code_byte
	jnc	put_memx_err
	dec	esi
	shl	eax,8
	call	get_code_byte
	jnc	put_memx_err
	call	makehexeax
	mov	commadr,edi
	mov	wptr [edi],0068h
	pop	edi esi eax
@@done:	ret
put_memfw:
	cmp	_refshow,0
	jz	@@done
	push	eax esi edi
	mov	esi,_refaddr
	mov	edi,commadr
	mov	edx,offs err_addr_f2
	add	esi,3
	call	get_code_byte
	jnc	put_memx_err
	dec	esi
	shl	eax,8
	call	get_code_byte
	jnc	put_memx_err
	dec	esi
	shl	eax,8
	call	get_code_byte
	jnc	put_memx_err
	dec	esi
	shl	eax,8
	call	get_code_byte
	jnc	put_memx_err
	ror	eax,16
	call	makehexax
	call	put_colon
	ror	eax,16
	call	makehexax
	mov	commadr,edi
	mov	wptr [edi],0068h
	pop	edi esi eax
@@done:	ret
put_memfd:
	cmp	_refshow,0
	jz	@@done
	push	eax esi edi
	mov	esi,_refaddr
	mov	edi,commadr
	mov	edx,offs err_addr_f
	add	esi,5
	call	get_code_byte
	jnc	put_memx_err
	dec	esi
	mov	ah,al
	call	get_code_byte
	jnc	put_memx_err
	dec	esi
	mov	ecx,eax
	call	get_code_byte
	jnc	put_memx_err
	dec	esi
	shl	eax,8
	call	get_code_byte
	jnc	put_memx_err
	dec	esi
	shl	eax,8
	call	get_code_byte
	jnc	put_memx_err
	dec	esi
	shl	eax,8
	call	get_code_byte
	jnc	put_memx_err
	xchg	ecx,eax
	call	makehexax
	call	put_colon
	mov	eax,ecx
	call	makehexeax
	mov	commadr,edi
	mov	wptr [edi],0068h
	pop	edi esi eax
@@done:	ret
put_adrmemb:
	call	put_commaddr
	jmp	put_memb
put_adrmemw:
	call	put_commaddr
	jmp	put_memw
put_adrmemd:
	call	put_commaddr
	jmp	put_memd

	Align 4
__safe_load:
	mov	al,fs:[esi]
	nop
	ret
	Align 4
load_byte:
	push	esi
	cmp	_cs_limit,-1
	jz	@@l1
	cmp	esi,_cs_limit
	ja	@@l2
@@l1:	add	esi,_cs_base
	xor	al,al
	call	__safe_load
	pop	esi
	ret
@@l2:	xor	al,al
	pop	esi
	ret
	Align 4
get_code_byte:
	push	esi
	movzx	ebx,_prefix
	cmp	_prefix_esp,0
	jz	@@0
	mov	bl,02h
@@0:	jmp	_data_byte_tab[ebx*4]
	Align 4
get_data_byte:
	push	esi
	movzx	eax,_data_override
	jmp	_data_byte_tab[eax*4]
	Align 4
_get_byte_err:
	clr	al
	pop	esi
	ret
	Align 4
_get_byte_safe:
	xor	al,al
	call	__safe_load
	jc	_get_byte_err
	pop	esi
	stc
	ret
	Align 4
_get_data_byte_es:
	cmp	wptr _es,0
	jz	_get_byte_err
	cmp	_es_limit,-1
	jz	@@l1
	cmp	esi,_es_limit
	ja	_get_byte_err
@@l1:	add	esi,_es_base
	jmp	_get_byte_safe
	Align 4
_get_data_byte_cs:
	cmp	wptr _cs,0
	jz	_get_byte_err
	cmp	_cs_limit,-1
	jz	@@l1
	cmp	esi,_cs_limit
	ja	_get_byte_err
@@l1:	add	esi,_cs_base
	jmp	_get_byte_safe
get_stack_byte:
	push	esi
	Align 4
_get_data_byte_ss:
	cmp	wptr _ss,0
	jz	_get_byte_err
	cmp	_ss_limit,-1
	je	@@l1
	cmp	esi,_ss_limit
	ja	_get_byte_err
@@l1:	add	esi,_ss_base
	jmp	_get_byte_safe
	Align 4
_get_data_byte_ds:
	cmp	wptr _ds,0
	jz	_get_byte_err
	cmp	_ds_limit,-1
	je	@@l1
	cmp	esi,_ds_limit
	ja	_get_byte_err
@@l1:	add	esi,_ds_base
	jmp	_get_byte_safe
	Align 4
_get_data_byte_fs:
	cmp	wptr _fs,0
	jz	_get_byte_err
	cmp	_fs_limit,-1
	je	@@l1
	cmp	esi,_fs_limit
	ja	_get_byte_err
@@l1:	add	esi,_fs_base
	jmp	_get_byte_safe
	Align 4
_get_data_byte_gs:
	cmp	wptr _gs,0
	jz	_get_byte_err
	cmp	_gs_limit,-1
	je	@@l1
	cmp	esi,_gs_limit
	ja	_get_byte_err
@@l1:	add	esi,_gs_base
	jmp	_get_byte_safe

put_comma:
	mov	bptr [edi],','
	inc	edi
	ret
put_colon:
	mov	bptr [edi],':'
	inc	edi
	ret
put_open:
	mov	bptr [edi],'['
	inc	edi
	ret
put_close:
	mov	bptr [edi],']'
	inc	edi
	ret
put_plus:
	mov	bptr [edi],'+'
	inc	edi
	ret
put_minus:
	mov	bptr [edi],'-'
	inc	edi
	ret
put_charh:
	mov	bptr [edi],'h'
	inc	edi
	ret
put_tab:mov	bptr [edi],09h
	inc	edi
	ret
put_one:mov	bptr [edi],'1'
	inc	edi
	ret
put_cl:	mov	wptr [edi],'lc'
	inc	edi
	inc	edi
	ret
put_prefix:
	cmp	_prefix,03h
	jnz	@@l0
	ret
@@l0:	movzx	ebx,_prefix
	lea	edx,_segtab86[ebx*4]
	call	put_string
	jmp	put_colon
make_indexah:
	cmp	ah,0
	jnz	@@l0
	mov	ah,'1'
	jmp	@@done
@@l0:	cmp	ah,1
	jnz	@@l1
	mov	ah,'2'
	jmp	@@done
@@l1:	cmp	ah,2
	jnz	@@l2
	mov	ah,'4'
	jmp	@@done
@@l2:	mov	ah,'8'
@@done:	mov	al,'*'
	mov	[edi],ax
	add	edi,2
	ret
put_string:
	push	eax
@@loop:	mov	al,[edx]
	test	al,al
	jz	@@done
	mov	[edi],al
	inc	edx
	inc	edi
	jmp	@@loop
@@done:	pop	eax
	ret

merge_text:
	pushad
	clr	ebx
	cmp	_showmode_flag,0
	mov	bl,24
	jz	@@l1
	mov	bl,9
@@l1:	mov	esi,offs cmdbuf
	mov	edi,offs textbuf
	cmp	bptr [edi+ebx],20h
	jz	@@loop
	mov	bptr [edi+ebx-1],0AFh
@@loop:	mov	al,[esi]
	inc	esi
	test	al,al
	jz	@@done
	cmp	al,09h
	jz	@@tab
	mov	[edi+ebx],al
	inc	ebx
	cmp	ebx,60
	jb	@@loop
	mov	wptr [edi+ebx-1],00AFh
	popad
	ret
@@tab:	mov	ecx,ebx
	not	ecx
	and	ecx,07h
@@ltab:	mov	bptr [edi+ebx],20h
	inc	ebx
	jecxz	@@loop
	loop	@@ltab
	jmp	@@loop
@@done:	mov	[edi+ebx],al

	cmp	_commmode_flag,0
	jz	@@xxxx
	mov	esi,offs commbuf
	cmp	byte ptr [esi],20h
	jz	@@xxxx
	mov	byte ptr [edi+ebx],20h

	push	edi
	mov	edi,esi
	mov	ecx,80
	xor	al,al
	repne	scasb
	lea	esi,[edi-3]
	pop	edi
	mov	ecx,59
	sub	ecx,ebx
	jbe	@@xxxx
	mov	ebx,59
@@1:	mov	al,[esi]
	dec	esi
	test	al,al
	jz	@@2
	cmp	al,20h
	jz	@@1
	mov	[edi+ebx],al
	dec	ebx
	loop	@@1
@@2:	cmp	esi,offs commbuf-1
	jbe	@@xxxx
	mov	byte ptr [edi+ebx],0AEh

@@xxxx:	mov	edi,offs textbuf
	mov	byte ptr [edi+60],0
	popad
	ret



;=============================================================================
_group0:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type0tab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type0tab[ebx*8+4]	; goto command offset inside group
_group1:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type1tab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type1tab[ebx*8+4]	; goto command offset inside group
_group2:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type2tab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type2tab[ebx*8+4]	; goto command offset inside group
_group3:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type3tab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type3tab[ebx*8+4]	; goto command offset inside group
_group4:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type4tab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type4tab[ebx*8+4]	; goto command offset inside group
_group5:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type5tab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type5tab[ebx*8+4]	; goto command offset inside group
_group6:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type6tab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type6tab[ebx*8+4]	; goto command offset inside group
_group7:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type7tab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type7tab[ebx*8+4]	; goto command offset inside group
_group8:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type8tab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type8tab[ebx*8+4]	; goto command offset inside group
_group9:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type9tab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type9tab[ebx*8+4]	; goto command offset inside group
_groupA:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_typeAtab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_typeAtab[ebx*8+4]	; goto command offset inside group
_groupB:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_typeBtab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_typeBtab[ebx*8+4]	; goto command offset inside group
_groupC:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_typeCtab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_typeCtab[ebx*8+4]	; goto command offset inside group
_groupD:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_typeDtab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_typeDtab[ebx*8+4]	; goto command offset inside group
_groupE:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_typeEtab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_typeEtab[ebx*8+4]	; goto command offset inside group
_groupF:mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_typeFtab[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_typeFtab[ebx*8+4]	; goto command offset inside group

;-----------------------------------------------------------------------------
_type_0F:				; group op-codes with 0Fh header
	call	put_textal
	mov	bl,al
	shr	ebx,4
	and	ebx,0Fh
	jmp	_group0Ftab[ebx*4]
_group0F0:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type0tab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type0tab0F[ebx*8+4]	; goto command offset inside group
_group0F1:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type1tab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type1tab0F[ebx*8+4]	; goto command offset inside group
_group0F2:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type2tab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type2tab0F[ebx*8+4]	; goto command offset inside group
_group0F3:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type3tab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type3tab0F[ebx*8+4]	; goto command offset inside group
_group0F4:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type4tab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type4tab0F[ebx*8+4]	; goto command offset inside group
_group0F5:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type5tab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type5tab0F[ebx*8+4]	; goto command offset inside group
_group0F6:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type6tab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type6tab0F[ebx*8+4]	; goto command offset inside group
_group0F7:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type7tab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type7tab0F[ebx*8+4]	; goto command offset inside group
_group0F8:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type8tab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type8tab0F[ebx*8+4]	; goto command offset inside group
_group0F9:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_type9tab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_type9tab0F[ebx*8+4]	; goto command offset inside group
_group0FA:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_typeAtab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_typeAtab0F[ebx*8+4]	; goto command offset inside group
_group0FB:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_typeBtab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_typeBtab0F[ebx*8+4]	; goto command offset inside group
_group0FC:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_typeCtab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_typeCtab0F[ebx*8+4]	; goto command offset inside group
_group0FD:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_typeDtab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_typeDtab0F[ebx*8+4]	; goto command offset inside group
_group0FE:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_typeEtab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_typeEtab0F[ebx*8+4]	; goto command offset inside group
_group0FF:
	mov	bl,al
	and	ebx,0Fh			; ebx = 0Fh masked opcode
	mov	edx,_typeFtab0F[ebx*8]	; get command keyword (text)
	call	put_string
	jmp	_typeFtab0F[ebx*8+4]	; goto command offset inside group

;-----------------------------------------------------------------------------
_type_grp1c80:
	mov	ebp,offs _grp1tab
	call	decode_by_grp
	jmp	_type_EbIb
_type_grp1c81:
	mov	ebp,offs _grp1tab
	call	decode_by_grp
	jmp	_type_EvIv
_type_grp1c83:
	mov	ebp,offs _grp1tab
	call	decode_by_grp
	jmp	_type_EvIb
_type_grp2cC0:
	mov	ebp,offs _grp2tab
	call	decode_by_grp
	cmp	al,06h
	jz	_invalid
	jmp	_type_EbIb
_type_grp2cC1:
	mov	ebp,offs _grp2tab
	call	decode_by_grp
	cmp	al,06h
	jz	_invalid
	jmp	_type_EvIb
_type_grp2cD0:
	mov	ebp,offs _grp2tab
	call	decode_by_grp
	cmp	al,06h
	jz	_invalid
	call	_type_Eb
	call	put_comma
	jmp	put_one
_type_grp2cD1:
	mov	ebp,offs _grp2tab
	call	decode_by_grp
	cmp	al,06h
	jz	_invalid
	call	_type_Ev
	call	put_comma
	jmp	put_one
_type_grp2cD2:
	mov	ebp,offs _grp2tab
	call	decode_by_grp
	cmp	al,06h
	jz	_invalid
	call	_type_Eb
	call	put_comma
	jmp	put_cl
_type_grp2cD3:
	mov	ebp,offs _grp2tab
	call	decode_by_grp
	cmp	al,06h
	jz	_invalid
	call	_type_Ev
	call	put_comma
	jmp	put_cl
_type_grp3cF6:
	mov	ebp,offs _grp3tab
	call	decode_by_grp
	test	al,al
	jz	_type_EbIb
	cmp	al,01h
	jz	_invalid
	jmp	_type_Eb
_type_grp3cF7:
	mov	ebp,offs _grp3tab
	call	decode_by_grp
	test	al,al
	jz	_type_EvIv
	cmp	al,01h
	jz	_invalid
	jmp	_type_Ev
_type_grp4cFE:
	mov	ebp,offs _grp4tab
	call	decode_by_grp
	cmp	al,02h
	jae	_invalid
	jmp	_type_Eb
_type_grp5cFF:
	mov	ebp,offs _grp5tab
	call	decode_by_grp
	cmp	al,07h
	jz	_invalid
	cmp	al,03h
	jz	_type_farEv
	cmp	al,05h
	jz	_type_farjumpEv
	cmp	al,04h
	jz	_type_jumpEv
	jmp	_type_Ev
_type_grp6c:
	mov	ebp,offs _grp6tab
	call	decode_by_grp
	cmp	al,06h
	jae	_invalid
	jmp	_type_Ew
_type_grp7c:
	mov	ebp,offs _grp7tab
	call	decode_by_grp
	cmp	al,05h
	jz	_invalid
	jmp	_type_Ew
_type_grp8c:
	mov	ebp,offs _grp8tab
	call	decode_by_grp
	cmp	al,04h
	jb	_invalid
	jmp	_type_EvIb



;=============================================================================
_type_esc0:
	call	put_textal
	call	decode_mode
	movzx	ebx,ch
	cmp	ah,3
	jz	@@mod3
	mov	edx,_esc0tab0[ebx*4]
	call	put_string
	call	put_tab
	call	decode_by_mod
	jmp	put_memd
@@mod3:	mov	edx,_esc0tab1[ebx*4]
	call	put_string
	call	put_tab
	mov	edx,offs _fpureg
	call	put_string
	call	put_comma
	movzx	ebx,cl
	lea	edx,_fputab87[ebx*8]
	jmp	put_string

_type_esc1:
	call	put_textal
	call	decode_mode
	movzx	ebx,ch
	cmp	ah,3
	jz	@@mod3
	mov	edx,_esc1tab0[ebx*4]
	test	edx,edx
	jz	_escape
	call	put_string
	call	put_tab
	cmp	ch,5
	jz	@@l0
	cmp	ch,7
	jz	@@l0
	call	decode_by_mod
	jmp	put_memd
@@l0:	call	decode_by_mod
	jmp	put_memw
@@mod3:	movzx	eax,cl
	shl	eax,3
	add	ebx,eax
	mov	edx,_esc1tab1[ebx*4]
	test	edx,edx
	jz	_escape
	call	put_string
	cmp	ch,1
	jbe	@@l1
	ret
@@l1:	call	put_tab
	movzx	ebx,cl
	lea	edx,_fputab87[ebx*8]
	jmp	put_string

_type_esc2:
	call	put_textal
	call	decode_mode
	movzx	ebx,ch
	cmp	ah,3
	jz	@@mod3
	mov	edx,_esc2tab0[ebx*4]
	call	put_string
	call	put_tab
	call	decode_by_mod
	jmp	put_memd
@@mod3:	mov	edx,_esc2tab1[ebx*4]
	test	edx,edx
	jz	_escape
	jmp	put_string

_type_esc3:
	call	put_textal
	call	decode_mode
	movzx	ebx,ch
	cmp	ah,3
	jz	@@mod3
	mov	edx,_esc3tab0[ebx*4]
	test	edx,edx
	jz	_escape
	call	put_string
	call	put_tab
	cmp	ch,3
	ja	@@l1
	call	decode_by_mod
	jmp	put_memd
@@l1:	mov	edx,offs _text_temp
	call	put_string
	call	decode_by_mod
	jmp	put_memd
@@mod3:	cmp	ch,4
	jnz	_escape
	movzx	ebx,cl
	mov	edx,_esc3tab1[ebx*4]
	test	edx,edx
	jz	_escape
	jmp	put_string

_type_esc4:
	call	put_textal
	call	decode_mode
	movzx	ebx,ch
	cmp	ah,3
	jz	@@mod3
	mov	edx,_esc4tab0[ebx*4]
	call	put_string
	call	put_tab
	mov	edx,offs _text_long
	call	put_string
	call	decode_by_mod
	jmp	put_memd
@@mod3:	mov	edx,_esc4tab1[ebx*4]
	call	put_string
	call	put_tab
	movzx	ebx,cl
	lea	edx,_fputab87[ebx*8]
	call	put_string
	call	put_comma
	mov	edx,offs _fpureg
	jmp	put_string

_type_esc5:
	call	put_textal
	call	decode_mode
	movzx	ebx,ch
	cmp	ah,3
	jz	@@mod3
	mov	edx,_esc5tab0[ebx*4]
	test	edx,edx
	jz	_escape
	call	put_string
	call	put_tab
	cmp	ch,4
	jz	@@l1
	cmp	ch,6
	jz	@@l1
	cmp	ch,7
	jz	@@l2
	mov	edx,offs _text_long
	call	put_string
@@l1:	call	decode_by_mod
	jmp	put_memd
@@l2:	call	decode_by_mod
	jmp	put_memw
@@mod3:	mov	edx,_esc5tab1[ebx*4]
	test	edx,edx
	jz	_escape
	call	put_string
	call	put_tab
	movzx	ebx,cl
	lea	edx,_fputab87[ebx*8]
	jmp	put_string

_type_esc6:
	call	put_textal
	call	decode_mode
	movzx	ebx,ch
	cmp	ah,3
	jz	@@mod3
	mov	edx,_esc6tab0[ebx*4]
	call	put_string
	call	put_tab
	mov	edx,offs _text_short
	call	put_string
	call	decode_by_mod
	jmp	put_memw
@@mod3:	mov	edx,_esc6tab1[ebx*4]
	test	edx,edx
	jz	_escape
	call	put_string
	call	put_tab
	movzx	ebx,cl
	lea	edx,_fputab87[ebx*8]
	call	put_string
	call	put_comma
	mov	edx,offs _fpureg
	jmp	put_string

_type_esc7:
	call	put_textal
	call	decode_mode
	movzx	ebx,ch
	cmp	ah,3
	jz	@@mod3
	mov	edx,_esc7tab0[ebx*4]
	test	edx,edx
	jz	_escape
	call	put_string
	call	put_tab
	cmp	ch,3
	ja	@@l1
	mov	edx,offs _text_short
	call	put_string
	call	decode_by_mod
	jmp	put_memw
@@l1:	cmp	ch,4
	jz	@@l2
	cmp	ch,6
	jz	@@l2
	mov	edx,offs _text_long
	call	put_string
	call	decode_by_mod
	jmp	put_memd
@@l2:	mov	edx,offs _text_temp
	call	put_string
	call	decode_by_mod
	jmp	put_memd
@@mod3:	mov	edx,_esc7tab1[ebx*4]
	test	edx,edx
	jz	_escape
	call	put_string
	call	put_tab
	mov	edx,offs _regtab16
	jmp	put_string




;=============================================================================
decode_mode:
	mov	ah,al
	mov	cx,ax
	and	cl,7			; cl = mask r/m field
	shr	ch,3
	and	ch,7			; ch = mask reg field
	shr	ah,6			; ah = mask mod field
	ret

decode_by_mod:
	clr	edx
	mov	_refshow,dl
	mov	_refaddr,edx
	mov	_prefix_esp,dl
	mov	__temp_addr,edx
	cmp	_adrsiz,0
	jnz	decode_by_mod86
	cmp	ah,03h
	jz	@@l3			; mod=11
	call	put_prefix
	call	put_open
@@l0:	cmp	ah,02h
	jz	@@l2			; mod=10
	cmp	ah,01h
	jz	@@l1			; mod=01
;---------------------------------------------------------------------------
	cmp	cl,05h			; [ addr ],reg
	jnz	@@l01
	call	put_texteax
	call	makehexeax
	call	put_commaddr
	jmp	put_close
@@l01:	cmp	cl,04h		; is s-i-b present?
	jnz	@@l02		; no, jump
	push	ecx		; yes, do [ reg * index ]
	call	put_textal
	call	decode_mode
	cmp	cl,4
	setz	_prefix_esp		; reference to SS:[ xxx ]
	cmp	cl,05h			; [ d32 + reg * index ]
	jnz	@@l01e
	push	eax
	call	put_texteax		; get d32
	add	__temp_addr,eax
	test	eax,eax			; don't print if zero
	jz	@@l01w
	call	makehexeax
	pop	eax
	jmp	@@l01d
@@l01w:	pop	eax
	jmp	@@l01v
@@l01e:	movzx	ebx,cl			; get base register
	lea	edx,_regtab32[ebx*4]
	call	put_string
	mov	edx,_registers[ebx*4]
	add	__temp_addr,edx
	cmp	ch,04h			; when index=100b, no index is present
	jz	@@l01a
@@l01d:	call	put_plus
@@l01v:	movzx	ebx,ch			; get index register
	lea	edx,_regtab32[ebx*4]
	call	put_string
	mov	edx,_registers[ebx*4]
	mov	cl,ah
	shl	edx,cl
	add	__temp_addr,edx
	test	ah,ah
	jz	@@l01a
	call	make_indexah
@@l01a:	mov	eax,__temp_addr
	call	put_commaddr
	call	put_close
	pop	ecx
	ret
@@l02:	movzx	ebx,cl
	lea	edx,_regtab32[ebx*4]
	call	put_string
	mov	eax,_registers[ebx*4]
	call	put_commaddr
	jmp	put_close
;---------------------------------------------------------------------------
@@l1:	cmp	cl,04h		; if s-i-b, do   [reg+reg*index+disp8]
	jnz	@@l10		; no sib, jump
	push	ecx
	call	put_textal
	call	decode_mode
	movzx	ebx,cl
	cmp	cl,4
	jz	@@__1
	cmp	cl,5
@@__1:	setz	_prefix_esp		; reference to SS:[ xxx ]
	lea	edx,_regtab32[ebx*4]	; get base register
	call	put_string
	mov	edx,_registers[ebx*4]
	add	__temp_addr,edx
	cmp	ch,04h			; when index=100b, no index is present
	jz	@@l11a
	call	put_plus
	movzx	ebx,ch
	lea	edx,_regtab32[ebx*4]	; get index register
	call	put_string
	mov	edx,_registers[ebx*4]
	mov	cl,ah
	shl	edx,cl
	add	__temp_addr,edx
	test	ah,ah
	jz	@@l11a
	call	make_indexah
@@l11a:	call	put_textal
	movsx	edx,al
	add	__temp_addr,edx
	test	al,al
	jz	@@l11d
	js	@@l11b
	call	put_plus
	jmp	@@l11c
@@l11b:	call	put_minus
	neg	al
@@l11c:	call	makehexal
@@l11d:	mov	eax,__temp_addr
	call	put_commaddr
	call	put_close
	pop	ecx
	ret
@@l10:	movzx	ebx,cl
	cmp	cl,5
	setz	_prefix_esp		; reference to SS:[ xxx ]
	lea	edx,_regtab32[ebx*4]
	call	put_string
	mov	edx,_registers[ebx*4]
	add	__temp_addr,edx
	call	put_textal
	movsx	edx,al
	add	__temp_addr,edx
	test	al,al
	jz	@@l10d
	js	@@l10b
	call	put_plus
	jmp	@@l10c
@@l10b:	call	put_minus
	neg	al
@@l10c:	call	makehexal
@@l10d:	mov	eax,__temp_addr
	call	put_commaddr
	jmp	put_close
;---------------------------------------------------------------------------
@@l2:	cmp	cl,04h			; s-i-b   [reg+reg*index+disp32]
	jnz	@@l20
	push	ecx
	call	put_textal
	call	decode_mode
	movzx	ebx,cl
	cmp	cl,4
	jz	@@__2
	cmp	cl,5
@@__2:	setz	_prefix_esp		; reference to SS:[ xxx ]
	lea	edx,_regtab32[ebx*4]	; get base
	call	put_string
	mov	edx,_registers[ebx*4]
	add	__temp_addr,edx
	cmp	ch,04h			; when index=100b, no index is present
	jz	@@l21a
	call	put_plus
	movzx	ebx,ch
	lea	edx,_regtab32[ebx*4]	; get index
	call	put_string
	mov	edx,_registers[ebx*4]
	mov	cl,ah
	shl	edx,cl
	add	__temp_addr,edx
	test	ah,ah
	jz	@@l21a
	call	make_indexah
@@l21a:	call	put_texteax
	add	__temp_addr,eax
	test	eax,eax
	jz	@@l21d
	js	@@l21b
	call	put_plus
	jmp	@@l21c
@@l21b:	call	put_minus
	neg	eax
@@l21c:	call	makehexdword
@@l21d:	mov	eax,__temp_addr
	call	put_commaddr
	call	put_close
	pop	ecx
	ret
@@l20:	movzx	ebx,cl
	cmp	cl,5
	setz	_prefix_esp		; reference to SS:[ xxx ]
	lea	edx,_regtab32[ebx*4]
	call	put_string
	mov	edx,_registers[ebx*4]
	add	__temp_addr,edx
	call	put_texteax
	add	__temp_addr,eax
	test	eax,eax
	jz	@@l20d
	js	@@l20b
	call	put_plus
	jmp	@@l20c
@@l20b:	call	put_minus
	neg	eax
@@l20c:	call	makehexdword
@@l20d:	mov	eax,__temp_addr
	call	put_commaddr
	jmp	put_close
;---------------------------------------------------------------------------
@@l3:	movzx	ebx,cl			; mod=11	register only
	lea	edx,[ebp+ebx*4]
	jmp	put_string


decode_by_mod86:
	cmp	ah,03h
	jz	@@l3			; mod=11
	call	put_prefix
	call	put_open
	cmp	ah,02h
	jz	@@l2			; mod=10
	cmp	ah,01h
	jz	@@l1			; mod=01
;---------------------------------------------------------------------------
	cmp	cl,06h			; [ addr ],reg
	jnz	@@l01
	call	put_textax
	call	makehexax
	call	put_commaddr
	jmp	put_close
@@l01:	movzx	ebx,cl
	lea	edx,_regtab86[ebx*8]
	call	put_string
	call	@@d86
	mov	eax,__temp_addr
	call	put_commaddr
	jmp	put_close
;---------------------------------------------------------------------------
@@l1:	movzx	ebx,cl
	lea	edx,_regtab86[ebx*8]
	call	put_string
	call	@@d86
	call	put_textal
	movsx	dx,al
	add	wptr __temp_addr,dx
	test	al,al
	js	@@l11
	call	put_plus
	jmp	@@l12
@@l11:	call	put_minus
	neg	al
@@l12:	call	makehexal
	mov	eax,__temp_addr
	call	put_commaddr
	jmp	put_close
;---------------------------------------------------------------------------
@@l2:	movzx	ebx,cl
	lea	edx,_regtab86[ebx*8]
	call	put_string
	call	@@d86
	call	put_textax
	add	wptr __temp_addr,ax
	test	ax,ax
	js	@@l21
	call	put_plus
	jmp	@@l22
@@l21:	call	put_minus
	neg	ax
@@l22:	call	makehexax
	mov	eax,__temp_addr
	call	put_commaddr
	jmp	put_close
;---------------------------------------------------------------------------
@@l3:	movzx	ebx,cl			; mod=11	// register
	lea	edx,[ebp+ebx*4]
	jmp	put_string
@@d86:	movzx	ebx,cl
	and	ebx,07h
	jmp	_r86tab[ebx*4]
_r86c0:	mov	dx,_bx
	add	dx,_si
	add	wptr __temp_addr,dx
	ret
_r86c1:	mov	dx,_bx
	add	dx,_di
	add	wptr __temp_addr,dx
	ret
_r86c2:	mov	dx,_bp
	add	dx,_si
	add	wptr __temp_addr,dx
	ret
_r86c3:	mov	dx,_bp
	add	dx,_di
	add	wptr __temp_addr,dx
	ret
_r86c4:	mov	dx,_si
	add	wptr __temp_addr,dx
	ret
_r86c5:	mov	dx,_di
	add	wptr __temp_addr,dx
	ret
_r86c6:	mov	dx,_bp
	add	wptr __temp_addr,dx
	ret
_r86c7:	mov	dx,_bx
	add	wptr __temp_addr,dx
	ret

;---------------------------------------------------------------------------
decode_by_reg:
	movzx	ebx,ch
	lea	edx,[ebp+ebx*4]
	jmp	put_string

;---------------------------------------------------------------------------
decode_by_grp:
	call	load_byte
	mov	ah,al
	shr	al,3
	and	al,7			; ch = mask reg field
	movzx	ebx,al
	mov	edx,[ebp+ebx*4]
	jmp	put_string

;---------------------------------------------------------------------------
check_jump:
	pushfd
	push	eax edx
	and	eax,0Fh
	mov	edx,cs:_jumptbl[eax*4]
	pushfd
	and	dptr [esp],0FFFFF700h
	mov	eax,_efl
	and	eax,000008FFh
	or	[esp],eax
	popfd
	jmp	edx
_jump_end:
	pop	edx eax
	popfd
	ret

_jumptbl	dd _jump_jo, _jump_jno, _jump_jb, _jump_jnb
		dd _jump_jz, _jump_jnz, _jump_jbe, _jump_jnbe
		dd _jump_js, _jump_jns, _jump_jp, _jump_jnp
		dd _jump_jl, _jump_jnl, _jump_jle, _jump_jnle

_jump_jo:
	jo	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jno:
	jno	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jb:
	jb	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jnb:
	jnb	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jz:
	jz	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jnz:
	jnz	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jbe:
	jbe	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jnbe:				; (ja)
	jnbe	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_js:
	js	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jns:
	jns	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jp:
	jp	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jnp:
	jnp	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jl:
	jl	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jnl:				; (jge)
	jnl	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jle:
	jle	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end
_jump_jnle:				; (jg)
	jnle	@@0
	mov	_jump_taken,0
	jmp	_jump_end
@@0:	mov	_jump_taken,1
	jmp	_jump_end




;*****************************************************************************
;=============================================================================
_type_EbGb:					; addr8 , reg8
	call	put_tab
	call	put_textal
	call	decode_mode
	mov	ebp,offs _regtab8
	call	decode_by_mod
	call	put_memb
	call	put_comma
	jmp	decode_by_reg
_type_GbEb:					; reg8 , addr8
	call	put_tab
	call	put_textal
	call	decode_mode
	mov	ebp,offs _regtab8
	call	decode_by_reg
	call	put_comma
	call	decode_by_mod
	jmp	put_memb
_type_EvGv:					; addr16/32 , reg16/32
	call	put_tab
	call	put_textal
	call	decode_mode
	cmp	_opsiz,0
	jz	@@l1
	mov	ebp,offs _regtab16
	call	decode_by_mod
	call	put_memw
	call	put_comma
	jmp	decode_by_reg
@@l1:	mov	ebp,offs _regtab32
	call	decode_by_mod
	call	put_memd
	call	put_comma
	jmp	decode_by_reg
_type_GvEv:					; reg16/32 , addr16/32
	call	put_tab
	call	put_textal
	call	decode_mode
	cmp	_opsiz,0
	jz	@@l1
	mov	ebp,offs _regtab16
	call	decode_by_reg
	call	put_comma
	call	decode_by_mod
	jmp	put_memw
@@l1:	mov	ebp,offs _regtab32
	call	decode_by_reg
	call	put_comma
	call	decode_by_mod
	jmp	put_memd
_type_GvEb:					; reg16/32 , addr8
	call	put_tab
	call	put_textal
	call	decode_mode
	cmp	_opsiz,0
	mov	ebp,offs _regtab16
	jnz	@@l1
	mov	ebp,offs _regtab32
@@l1:	call	decode_by_reg
	call	put_comma
	mov	ebp,offs _regtab8
	call	decode_by_mod
	jmp	put_memb
_type_GvEw:					; reg16/32 , addr16
	call	put_tab
	call	put_textal
	call	decode_mode
	cmp	_opsiz,0
	mov	ebp,offs _regtab16
	jnz	@@l1
	mov	ebp,offs _regtab32
@@l1:	call	decode_by_reg
	call	put_comma
	mov	ebp,offs _regtab16
	call	decode_by_mod
	jmp	put_memw
_type_EwSw:					; reg , segment
	call	put_tab
	call	put_textal
	call	decode_mode
	mov	ebp,offs _regtab16
	call	decode_by_mod
	call	put_memw
	call	put_comma
	mov	ebp,offs _segtab86
	jmp	decode_by_reg
_type_SwEw:					; segment , reg
	call	put_tab
	call	put_textal
	call	decode_mode
	mov	ebp,offs _segtab86
	call	decode_by_reg
	call	put_comma
	mov	ebp,offs _regtab16
	call	decode_by_mod
	jmp	put_memw


;----------------------------------
_type_ALIb:					; AL , imm8
	call	put_tab
	mov	edx,offs _regtab8
	call	put_string
	call	put_comma
	call	put_textal
	call	makehexal
	jmp	put_charh
_type_eAXIb:					; eAX , imm8	***IN
	call	put_tab
	cmp	_opsiz,0
	mov	edx,offs _regtab16
	jnz	@@l1
	mov	edx,offs _regtab32
@@l1:	call	put_string
	call	put_comma
	call	put_textal
	call	makehexal
	jmp	put_charh
_type_eAXIv:					; eAX , imm16/32
	call	put_tab
	cmp	_opsiz,0
	jz	@@l1
	mov	edx,offs _regtab16
	call	put_string
	call	put_comma
	call	put_textax
	call	makehexax
	jmp	put_charh
@@l1:	mov	edx,offs _regtab32
	call	put_string
	call	put_comma
	call	put_texteax
	call	makehexdword
	jmp	put_charh
_type_IbAL:					; imm8 , AL	***OUT
	call	put_tab
	call	put_textal
	call	makehexal
	call	put_charh
	call	put_comma
	mov	edx,offs _regtab8
	jmp	put_string
_type_IbeAX:					; imm8 , eAX	***OUT
	call	put_tab
	call	put_textal
	call	makehexal
	call	put_charh
	call	put_comma
	cmp	_opsiz,0
	mov	edx,offs _regtab16
	jnz	@@l1
	mov	edx,offs _regtab32
@@l1:	jmp	put_string



;----------------------------------
_type_EbIb:					; addr8 , imm8
	call	put_tab
	call	put_textal
	call	decode_mode
	mov	ebp,offs _regtab8
	call	decode_by_mod
	call	put_memb
	call	put_comma
	call	put_textal
	call	makehexal
	jmp	put_charh
_type_EvIb:					; addr16/32 , imm8
	call	put_tab
	call	put_textal
	call	decode_mode
	cmp	_opsiz,0
	jz	@@l1
	mov	ebp,offs _regtab16
	call	decode_by_mod
	call	put_memw
	call	put_comma
	call	put_textal
	movsx	eax,al
	call	makehexax
	jmp	put_charh
@@l1:	mov	ebp,offs _regtab32
	call	decode_by_mod
	call	put_memd
	call	put_comma
	call	put_textal
	movsx	eax,al
	call	makehexdword
	jmp	put_charh
_type_EvIv:					; addr16/32 , imm16/32
	call	put_tab
	call	put_textal
	call	decode_mode
	cmp	_opsiz,0
	jz	@@l1
	mov	ebp,offs _regtab16
	call	decode_by_mod
	call	put_memw
	call	put_comma
	call	put_textax
	call	makehexax
	jmp	put_charh
@@l1:	mov	ebp,offs _regtab32
	call	decode_by_mod
	call	put_memd
	call	put_comma
	call	put_texteax
	call	makehexdword
	jmp	put_charh



;----------------------------------
_type_ALOb:					; AL , offs16/32
	call	put_tab
	mov	edx,offs _regtab8
	call	put_string
	call	put_comma
	call	put_prefix
	call	put_open
	cmp	_adrsiz,0
	jz	@@l1
	call	put_textax
	call	makehexax
	call	put_adrmemb
	jmp	put_close
@@l1:	call	put_texteax
	call	makehexeax
	call	put_adrmemb
	jmp	put_close
_type_ObAL:					; offs16/32 , AL
	call	put_tab
	call	put_prefix
	call	put_open
	cmp	_adrsiz,0
	jz	@@l1
	call	put_textax
	call	makehexax
	call	put_adrmemb
	jmp	@@l2
@@l1:	call	put_texteax
	call	makehexeax
	call	put_adrmemb
@@l2:	call	put_close
	call	put_comma
	mov	edx,offs _regtab8
	jmp	put_string

_type_eAXOv:
	call	put_tab
	cmp	_opsiz,0
	mov	edx,offs _regtab16
	jnz	@@l0
	mov	edx,offs _regtab32
@@l0:	call	put_string
	call	put_comma
	call	put_prefix
	call	put_open
	cmp	_adrsiz,0
	jz	@@l1
	call	put_textax
	call	makehexax
	call	put_close
	cmp	_opsiz,0
	jnz	put_adrmemw
	jmp	put_adrmemd
@@l1:	call	put_texteax
	call	makehexeax
	call	put_close
	cmp	_opsiz,0
	jnz	put_adrmemw
	jmp	put_adrmemd
_type_OveAX:
	call	put_tab
	call	put_prefix
	call	put_open
	cmp	_adrsiz,0
	jz	@@l1
	call	put_textax
	call	makehexax
	jmp	@@l2
@@l1:	call	put_texteax
	call	makehexeax
@@l2:	call	put_close
	call	put_comma
	cmp	_opsiz,0
	jz	@@l0
	call	put_adrmemw
	mov	edx,offs _regtab16
	jmp	put_string
@@l0:	call	put_adrmemd
	mov	edx,offs _regtab32
	jmp	put_string




;----------------------------------
_type_Eb:					; ex:INC bptr [addr]
	call	put_tab
	call	put_textal
	call	decode_mode
	mov	ebp,offs _regtab8
	mov	edx,offs _text_bptr
	cmp	ah,03h
	jz	@@l2
	call	put_string
@@l2:	call	decode_by_mod
	jmp	put_memb
_type_Ew:					; ex:NEG wptr [addr]
	call	put_tab
	call	put_textal
	call	decode_mode
	mov	ebp,offs _regtab16
	mov	edx,offs _text_wptr
	cmp	ah,03h
	jz	@@l2
	call	put_string
@@l2:	call	decode_by_mod
	jmp	put_memw
_type_Ev:					; push 0000h / push 12345678h
	call	put_tab
type_Ev:call	put_textal
	call	decode_mode
	cmp	_opsiz,0
	mov	ebp,offs _regtab16
	mov	edx,offs _text_wptr
	jnz	@@l1
	mov	ebp,offs _regtab32
	mov	edx,offs _text_dptr
@@l1:	cmp	ah,03h
	jz	@@l2
	call	put_string
@@l2:	call	decode_by_mod
	cmp	_opsiz,0
	jnz	put_memw
	jmp	put_memd
_type_Ib:
	call	put_tab				; INT 00h
type_Ib:call	put_textal
	call	makehexal
	jmp	put_charh
_type_Iw:
	call	put_tab				; RET 0000h
type_Iw:call	put_textax
	call	makehexax
	jmp	put_charh
_type_Iv:
	call	put_tab
type_Iv:cmp	_opsiz,0
	jz	@@l1
	call	put_textax
	call	makehexax
	jmp	put_charh
@@l1:	call	put_texteax
	call	makehexdword
	jmp	put_charh
_type_IwIb:
	call	put_tab
	call	put_textax
	call	makehexax
	call	put_charh
	call	put_comma
	call	put_textal
	call	makehexal
	jmp	put_charh




;----------------------------------
_type_EwRw:
	call	put_tab
	call	put_textal
	call	decode_mode
	mov	ebp,offs _regtab16
	call	decode_by_mod
	call	put_memw
	call	put_comma
	jmp	decode_by_reg



;----------------------------------
_type_GvEvIb:
	call	_type_GvEv
	call	put_comma
	jmp	type_Ib
_type_GvEvIv:
	call	_type_GvEv
	call	put_comma
	jmp	type_Iv
_type_EvGvIb:
	call	_type_EvGv
	call	put_comma
	jmp	type_Ib
_type_EvGvIv:
	call	_type_EvGv
	call	put_comma
	jmp	type_Iv
_type_EvGvCL:
	call	_type_EvGv
	call	put_comma
	mov	edx,offs _regtab8+4
	jmp	put_string
_type_GvMp:
	call	put_tab
	call	put_textal
	call	decode_mode
	cmp	_opsiz,0
	jz	@@l1

	mov	ebp,offs _regtab16
	call	decode_by_reg
	call	put_comma
	call	decode_by_mod
	jmp	put_memfw

@@l1:	mov	ebp,offs _regtab32
	call	decode_by_reg
	call	put_comma
	call	decode_by_mod
	jmp	put_memfd





;----------------------------------
_type_movIb:
	call	put_tab
	and	eax,07h
	lea	edx,_regtab8[eax*4]
	call	put_string
	call	put_comma
	call	put_textal
	call	makehexal
	jmp	put_charh
_type_movIv:
	call	put_tab
	and	eax,07h
	cmp	_opsiz,0
	jz	@@l1
	lea	edx,_regtab16[eax*4]
	call	put_string
	call	put_comma
	call	put_textax
	call	makehexax
	jmp	put_charh
@@l1:	lea	edx,_regtab32[eax*4]
	call	put_string
	call	put_comma
	call	put_texteax
	call	makehexdword
	jmp	put_charh
_type_eAXr:					; eAX,reg16/32
	call	put_tab
	and	eax,7
	cmp	_opsiz,0
	mov	ebx,offs _regtab16
	jnz	@@l1
	mov	ebx,offs _regtab32
@@l1:	lea	edx,[ebx+eax*4]
	call	put_string
	call	put_comma
	mov	edx,ebx
	jmp	put_string



;----------------------------------
_type_Jb:
	call	put_tab
	call	put_textal
	movsx	eax,al
	test	eax,eax
	jns	@@0
	neg	_jump_taken
@@0:	add	eax,esi
	cmp	_adrsiz,0
	jnz	makehexax
	jmp	makehexeax
_type_Jv:					; relative addr calculation
	call	put_tab
	cmp	_adrsiz,0
	jz	@@l1
	call	put_textax
	movsx	eax,ax
	test	eax,eax
	jns	@@0
	neg	_jump_taken
@@0:	add	eax,esi
	jmp	makehexax
@@l1:	call	put_texteax
	test	eax,eax
	jns	@@1
	neg	_jump_taken
@@1:	add	eax,esi
	jmp	makehexeax

_type_loopne:
	mov	eax,5			; check for JNZ
	call	check_jump
	jmp	_type_loop
_type_loope:
	mov	eax,4			; check for JZ
	call	check_jump
	jmp	_type_loop
_type_loop:
	cmp	_opsiz,0		; check for CX or ECX == 0
	jnz	@@1
	mov	eax,dptr _ecx
	jmp	@@2
@@1:	movzx	eax,wptr _ecx
@@2:	dec	eax
	setnz	al
	or	_jump_taken,al
	jmp	_type_Jb

_type_jumpJb:
	mov	_jump_taken,1
	jmp	_type_Jb
_type_jumpJv:
	mov	_jump_taken,1
	jmp	_type_Jv
_type_jumpAp:
	mov	_jump_taken,1
	jmp	_type_Ap
_type_jumpEv:
	mov	_jump_taken,1
	jmp	_type_Ev



;----------------------------------
_type_regv:
	call	put_tab
	and	eax,07h
	cmp	_opsiz,0
	lea	edx,_regtab16[eax*4]
	jnz	@@l1
	lea	edx,_regtab32[eax*4]
@@l1:	jmp	put_string
_type_setb:
	and	eax,0Fh
	lea	edx,_settab386[eax*8]
	call	put_string
	jmp	_type_Eb
_type_jumpb:
	and	eax,0Fh
	lea	edx,_jumptab86[eax*4]
	call	check_jump
	call	put_string
	jmp	_type_Jb
_type_jumpv:
	and	eax,0Fh
	lea	edx,_jumptab86[eax*4]
	call	check_jump
	call	put_string
	jmp	_type_Jv
_type_Ap:				; very far jump (imm.seg:imm.offset)
	call	put_tab
	cmp	_opsiz,0
	jz	@@l1
	call	put_texteax		; offset16
	mov	ebx,eax
	shr	eax,16
	call	makehexax
	call	put_colon
	mov	eax,ebx
	jmp	makehexax
@@l1:	call	put_texteax		; offset32
	xchg	eax,ebx
	call	put_textax
	call	makehexax
	call	put_colon
	xchg	eax,ebx
	jmp	makehexeax
_type_farjumpEv:
	mov	_jump_taken,1
_type_farEv:
	call	put_tab
	call	put_textal
	call	decode_mode
	cmp	_opsiz,0
	mov	ebp,offs _regtab16
	mov	edx,offs _text_far
	jnz	@@l1
	mov	ebp,offs _regtab32
	mov	edx,offs _text_far
@@l1:	cmp	ah,03h
	jz	@@l2
	call	put_string
@@l2:	call	decode_by_mod
	cmp	_opsiz,0
	jnz	put_memfw
	jmp	put_memfd
_type_pushIb:
	call	put_tab
	call	put_textal
	and	eax,00FFh
	cmp	_opsiz,0
	jz	@@l1
	call	makehexax
	jmp	put_charh
@@l1:	call	makehexeax
	jmp	put_charh
_type_pushs:					; segment register
	call	put_tab
	shr	al,3
	and	eax,07h
	lea	edx,_segtab86[eax*4]
	jmp	put_string
_type_pushd:					; pusha(d)/popa(d)
	cmp	_opsiz,0
	jnz	@@done
	mov	bptr [edi],'d'
	inc	edi
@@done:	ret
_type_es:
	mov	_prefix,0
	jmp	_type_seg
_type_cs:
	mov	_prefix,1
	jmp	_type_seg
_type_ss:
	mov	_prefix,2
	jmp	_type_seg
_type_ds:
	mov	_prefix,3
	jmp	_type_seg
_type_fs:
	mov	_prefix,4
	jmp	_type_seg
_type_gs:
	mov	_prefix,5
_type_seg:
	call	decode2
	mov	_prefix,3
	ret
_type_opsiz:
	xor	_opsiz,1
	call	decode2
	xor	_opsiz,1
	ret
_type_adrsiz:
	xor	_adrsiz,1
	call	decode2
	xor	_adrsiz,1
	ret
_type_rep:
	call	put_tab
	jmp	decode2
_type_cdq:
	cmp	_opsiz,0
	jz	@@l1
	cmp	al,98h
	mov	edx,offs _text_cbw
	jz	@@done
	cmp	al,99h
	mov	edx,offs _text_cwd
	jz	@@done
	mov	edx,offs _text_none
	jmp	@@done
@@l1:	cmp	al,98h
	mov	edx,offs _text_cwde
	jz	@@done
	cmp	al,99h
	mov	edx,offs _text_cdq
	jz	@@done
	mov	edx,offs _text_none
@@done:	jmp	put_string
_type_ALDX:
	call	put_tab
	mov	edx,offs _text_aldx
	jmp	put_string
_type_DXAL:
	call	put_tab
	mov	edx,offs _text_dxal
	jmp	put_string
_type_eAXDX:
	call	put_tab
	cmp	_opsiz,0
	mov	edx,offs _text_axdx
	jnz	@@l1
	mov	edx,offs _text_eaxdx
@@l1:	jmp	put_string
_type_DXeAX:
	call	put_tab
	cmp	_opsiz,0
	mov	edx,offs _text_dxax
	jnz	@@l1
	mov	edx,offs _text_dxeax
@@l1:	jmp	put_string
_type_jcxz:
	cmp	_adrsiz,0
	jz	@@l1
	mov	edx,offs _text_jcxz
	cmp	wptr _ecx,0
	setz	_jump_taken
	jmp	@@l2
@@l1:	mov	edx,offs _text_jecxz
	cmp	dptr _ecx,0
	setz	_jump_taken
@@l2:	call	put_string
	jmp	_type_Jb
_type_strb:
	mov	bptr [edi],'b'
	inc	edi
	jmp	_type_str
_type_strd:
	cmp	_opsiz,0
	mov	al,'w'
	jnz	@@l1
	mov	al,'d'
@@l1:	mov	[edi],al
	inc	edi
_type_str:
	cmp	_prefix,03h
	jz	_type_print
	mov	bptr [edi],' '
	inc	edi
	jmp	put_prefix
_type_print:
	ret




;----------------------------------
_type_Ea:
	call	put_tab
	call	put_textal
	call	decode_mode
	call	decode_by_mod
	jmp	put_memd
_type_RdCd:
	call	put_tab
	call	put_textal
	call	decode_mode
	movzx	ebx,ch
	lea	edx,_crtab386[ebx*4]
	call	put_string
	call	put_comma
	movzx	ebx,cl
	lea	edx,_regtab32[ebx*4]
	jmp	put_string
_type_RdDd:
	call	put_tab
	call	put_textal
	call	decode_mode
	movzx	ebx,ch
	lea	edx,_drtab386[ebx*4]
	call	put_string
	call	put_comma
	movzx	ebx,cl
	lea	edx,_regtab32[ebx*4]
	jmp	put_string
_type_RdTd:
	call	put_tab
	call	put_textal
	call	decode_mode
	movzx	ebx,ch
	lea	edx,_trtab386[ebx*4]
	call	put_string
	call	put_comma
	movzx	ebx,cl
	lea	edx,_regtab32[ebx*4]
	jmp	put_string
_type_CdRd:
	call	put_tab
	call	put_textal
	call	decode_mode
	movzx	ebx,cl
	lea	edx,_regtab32[ebx*4]
	call	put_string
	call	put_comma
	movzx	ebx,ch
	lea	edx,_crtab386[ebx*4]
	jmp	put_string
_type_DdRd:
	call	put_tab
	call	put_textal
	call	decode_mode
	movzx	ebx,cl
	lea	edx,_regtab32[ebx*4]
	call	put_string
	call	put_comma
	movzx	ebx,ch
	lea	edx,_drtab386[ebx*4]
	jmp	put_string
_type_TdRd:
	call	put_tab
	call	put_textal
	call	decode_mode
	movzx	ebx,cl
	lea	edx,_regtab32[ebx*4]
	call	put_string
	call	put_comma
	movzx	ebx,ch
	lea	edx,_trtab386[ebx*4]
	jmp	put_string


_escape:
	mov	edx,offs _text_esc
	call	put_string
	jmp	_type_byte
_invalid:
	push	esi
	dec	esi
	call	load_byte
	pop	esi
	mov	edx,offs _text_byte
	call	put_string
	jmp	_type_byte
_type_esc:					; FPU escape code
	call	put_textal
_type_byte:					; invalid op-code: print  DB
	call	put_tab
	call	makehexal
	jmp	put_charh
_type_aamd:
	call	put_string
	call	put_textal
	cmp	al,0Ah
	jz	@@1
	call	put_tab
	call	makehexal
	call	put_charh
@@1:	ret


.DATA
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

_opsiz		db 0
_adrsiz		db 0
_prefix		db 0
		db 0
_refaddr	dd 0	; address of memory reference
_refshow	db 0	; addr present
_jump_taken	db 0
_prefix_esp	db 0
		db 0
__temp_addr	dd 0	; temporary addr
cmdadr		dd 0
textadr		dd 0
commadr		dd 0
cmdbuf		db 80 dup (20h),0	; command INTEL OPCODE -> ASCII STRING
textbuf		db 80 dup (20h),0	; formatted string: ADDR+DB+ASCII
commbuf		db 80 dup (20h),0	; memory reference in ASCII
hexbuf		db '        ',0
hextab		db '0123456789ABCDEF',0

;	*** FPU	COMMANDS ***
;-----------------------------------------------------------------------------
_text_fabs	db 'fabs'	,0
_text_fadd	db 'fadd'	,0
_text_faddp	db 'faddp'	,0
_text_fbld	db 'fbld'	,0
_text_fbstp	db 'fbstp'	,0
_text_fchs	db 'fchs'	,0
_text_fnclex	db 'fnclex'	,0
_text_fcom	db 'fcom'	,0
_text_fcomp	db 'fcomp'	,0
_text_fcompp	db 'fcompp'	,0
_text_fcos	db 'fcos'	,0
_text_fdecstp	db 'fdecstp'	,0
_text_fdiv	db 'fdiv'	,0
_text_fdivp	db 'fdivp'	,0
_text_fdivr	db 'fdivr'	,0
_text_fdivrp	db 'fdivrp'	,0
_text_ffree	db 'ffree'	,0
_text_fiadd	db 'fiadd'	,0
_text_ficom	db 'ficom'	,0
_text_ficomp	db 'ficomp'	,0
_text_fidiv	db 'fidiv'	,0
_text_fidivr	db 'fidivr'	,0
_text_fild	db 'fild'	,0
_text_fimul	db 'fimul'	,0
_text_fincstp	db 'fincstp'	,0
_text_fninit	db 'fninit'	,0
_text_fist	db 'fist'	,0
_text_fistp	db 'fistp'	,0
_text_fisub	db 'fisub'	,0
_text_fisubr	db 'fisubr'	,0
_text_fld	db 'fld'	,0
_text_fld1	db 'fld1'	,0
_text_fldl2e	db 'fldl2e'	,0
_text_fldl2t	db 'fldl2t'	,0
_text_fldlg2	db 'fldlg2'	,0
_text_fldln2	db 'fldln2'	,0
_text_fldpi	db 'fldpi'	,0
_text_fldz	db 'fldz'	,0
_text_fldcw	db 'fldcw'	,0
_text_fldenv	db 'fldenv'	,0
_text_fmul	db 'fmul'	,0
_text_fmulp	db 'fmulp'	,0
_text_fnop	db 'fnop'	,0
_text_fpatan	db 'fpatan'	,0
_text_fprem	db 'fprem'	,0
_text_fprem1	db 'fprem1'	,0
_text_fptan	db 'fptan'	,0
_text_frndint	db 'frndint'	,0
_text_frstor	db 'frstor'	,0
_text_fnsave	db 'fnsave'	,0
_text_fscale	db 'fscale'	,0
_text_fsetpm	db 'fsetpm'	,0
_text_fsin	db 'fsin'	,0
_text_fsincos	db 'fsincos'	,0
_text_fsqrt	db 'fsqrt'	,0
_text_fst	db 'fst'	,0
_text_fstp	db 'fstp'	,0
_text_fnstcw	db 'fnstcw'	,0
_text_fnstenv	db 'fnstenv'	,0
_text_fnstsw	db 'fnstsw'	,0
_text_fsub	db 'fsub'	,0
_text_fsubp	db 'fsubp'	,0
_text_fsubr	db 'fsubr'	,0
_text_fsubrp	db 'fsubrp'	,0
_text_ftst	db 'ftst'	,0
_text_fucom	db 'fucom'	,0
_text_fucomp	db 'fucomp'	,0
_text_fucompp	db 'fucompp'	,0
_text_fwait	db 'fwait'	,0
_text_fxam	db 'fxam'	,0
_text_fxch	db 'fxch'	,0
_text_fxtract	db 'fxtract'	,0
_text_fyl2x	db 'fyl2x'	,0
_text_fyl2xp1	db 'fyl2xp1'	,0
_text_f2xm1	db 'f2xm1'	,0
_text_fpremi	db 'fpremi'	,0
_text_feni	db 'feni'	,0
_text_fdisi	db 'fdisi'	,0


;	*** CPU	COMMANDS ***
;-----------------------------------------------------------------------------
_text_aaa	db 'aaa'	,0
_text_aad	db 'aad'	,0
_text_aam	db 'aam'	,0
_text_aas	db 'aas'	,0
_text_adc	db 'adc'	,0
_text_add	db 'add'	,0
_text_and	db 'and'	,0
_text_arpl	db 'arpl'	,0
_text_bound	db 'bound'	,0
_text_bsf	db 'bsf'	,0
_text_bsr	db 'bsr'	,0
_text_bswap	db 'bswap'	,0
_text_bt	db 'bt'		,0
_text_btc	db 'btc'	,0
_text_btr	db 'btr'	,0
_text_bts	db 'bts'	,0
_text_call	db 'call'	,0
_text_cbw	db 'cbw'	,0
_text_cdq	db 'cdq'	,0
_text_clc	db 'clc'	,0
_text_cld	db 'cld'	,0
_text_cli	db 'cli'	,0
_text_clts	db 'clts'	,0
_text_cmc	db 'cmc'	,0
_text_cmp	db 'cmp'	,0
_text_cmps	db 'cmps'	,0
_text_cmpx	db 'cmpxchg'	,0
_text_cmpxchg8b	db 'cmpxchg8b'	,0
_text_cpuid	db 'cpuid'	,0
_text_cwd	db 'cwd'	,0
_text_cwde	db 'cwde'	,0
_text_daa	db 'daa'	,0
_text_das	db 'das'	,0
_text_dec	db 'dec'	,0
_text_div	db 'div'	,0
_text_emms	db 'emms'	,0
_text_enter	db 'enter'	,0
_text_hlt	db 'hlt'	,0
_text_idiv	db 'idiv'	,0
_text_imul	db 'imul'	,0
_text_in	db 'in'		,0
_text_inc	db 'inc'	,0
_text_ins	db 'ins'	,0
_text_int	db 'int'	,0
_text_int1	db 'int 1'	,0
_text_int3	db 'int 3'	,0
_text_into	db 'into'	,0
_text_invd	db 'invd'	,0
_text_invlpg	db 'invlpg'	,0
_text_iret	db 'iret'	,0
_text_jmp	db 'jmp'	,0
_text_jcxz	db 'jcxz'	,0
_text_jecxz	db 'jecxz'	,0
_text_lahf	db 'lahf'	,0
_text_lar	db 'lar'	,0
_text_lea	db 'lea'	,0
_text_leave	db 'leave'	,0
_text_lgdt	db 'lgdt'	,0
_text_lidt	db 'lidt'	,0
_text_lldt	db 'lldt'	,0
_text_lmsw	db 'lmsw'	,0
_text_lock	db 'lock'	,0
_text_lods	db 'lods'	,0
_text_loop	db 'loop'	,0
_text_loope	db 'loope'	,0
_text_loopne	db 'loopne'	,0
_text_les	db 'les'	,0
_text_lds	db 'lds'	,0
_text_lss	db 'lss'	,0
_text_lfs	db 'lfs'	,0
_text_lgs	db 'lgs'	,0
_text_lsl	db 'lsl'	,0
_text_ltr	db 'ltr'	,0
_text_mov	db 'mov'	,0
_text_movs	db 'movs'	,0
_text_movsx	db 'movsx'	,0
_text_movzx	db 'movzx'	,0
_text_mul	db 'mul'	,0
_text_neg	db 'neg'	,0
_text_nop	db 'nop'	,0
_text_not	db 'not'	,0
_text_or	db 'or'		,0
_text_out	db 'out'	,0
_text_outs	db 'outs'	,0
_text_pop	db 'pop'	,0
_text_popa	db 'popa'	,0
_text_popf	db 'popf'	,0
_text_push	db 'push'	,0
_text_pusha	db 'pusha'	,0
_text_pushf	db 'pushf'	,0
_text_rcl	db 'rcl'	,0
_text_rcr	db 'rcr'	,0
_text_rdmsr	db 'rdmsr'	,0
_text_rdtsc	db 'rdtsc'	,0
_text_rep	db 'rep'	,0
_text_repne	db 'repne'	,0
_text_ret	db 'ret'	,0
_text_retf	db 'retf'	,0
_text_rol	db 'rol'	,0
_text_ror	db 'ror'	,0
_text_sahf	db 'sahf'	,0
_text_sar	db 'sar'	,0
_text_sbb	db 'sbb'	,0
_text_scas	db 'scas'	,0
_text_setalc	db 'setalc'	,0
_text_sgdt	db 'sgdt'	,0
_text_shl	db 'shl'	,0
_text_shld	db 'shld'	,0
_text_shr	db 'shr'	,0
_text_shrd	db 'shrd'	,0
_text_sidt	db 'sidt'	,0
_text_sldt	db 'sldt'	,0
_text_smsw	db 'smsw'	,0
_text_stc	db 'stc'	,0
_text_std	db 'std'	,0
_text_sti	db 'sti'	,0
_text_stos	db 'stos'	,0
_text_str	db 'str'	,0
_text_sub	db 'sub'	,0
_text_test	db 'test'	,0
_text_verr	db 'verr'	,0
_text_verw	db 'verw'	,0
_text_wait	db 'wait'	,0
_text_wbinvd	db 'wbinvd'	,0
_text_wrmsr	db 'wrmsr'	,0
_text_xadd	db 'xadd'	,0
_text_xchg	db 'xchg'	,0
_text_xlat	db 'xlatb'	,0
_text_xor	db 'xor'	,0
_text_esc	db 'escape'	,0
_text_aldx	db 'al,dx'	,0
_text_dxal	db 'dx,al'	,0
_text_axdx	db 'ax,dx'	,0
_text_dxax	db 'dx,ax'	,0
_text_eaxdx	db 'eax,dx'	,0
_text_dxeax	db 'dx,eax'	,0
_text_byte	db 'db'		,0
_text_short	db 'word ptr '	,0
_text_far	db 'far ptr '	,0
_text_bptr	db 'byte ptr '	,0
_text_wptr	db 'word ptr '	,0
_text_dptr	db 'dword ptr '	,0
_text_long	db 'qword ptr '	,0
_text_temp	db 'tbyte ptr '	,0
err_addr_f2	label	byte
		db '----:----',0
err_addr_f	label	byte
		db '----:'
err_addr_d	label	byte
		db '----'
err_addr_w	label	byte
		db '--'
err_addr_b	label	byte
		db '--'
_text_none	db 0

;-----------------------------------------------------------------------------
_fpureg		db 'st',0
_fputab87	db 'st(0)',0,0,0,    'st(1)',0,0,0
		db 'st(2)',0,0,0,    'st(3)',0,0,0
		db 'st(4)',0,0,0,    'st(5)',0,0,0
		db 'st(6)',0,0,0,    'st(7)',0,0,0
_regtab8	db 'al',0,0,	'cl',0,0,	'dl',0,0,	'bl',0,0
		db 'ah',0,0,	'ch',0,0,	'dh',0,0,	'bh',0,0
_regtab16	db 'ax',0,0,	'cx',0,0,	'dx',0,0,	'bx',0,0
		db 'sp',0,0,	'bp',0,0,	'si',0,0,	'di',0,0
_regtab32	db 'eax',0,	'ecx',0,	'edx',0,	'ebx',0
		db 'esp',0,	'ebp',0,	'esi',0,	'edi',0
_regtab86	db 'bx+si',0,0,0,    'bx+di',0,0,0
		db 'bp+si',0,0,0,    'bp+di',0,0,0
		db 'si',0,0,0,0,0,0, 'di',0,0,0,0,0,0
		db 'bp',0,0,0,0,0,0, 'bx',0,0,0,0,0,0
_segtab86	db 'es',0,0,	'cs',0,0,	'ss',0,0,	'ds',0,0
		db 'fs',0,0,	'gs',0,0,	'??',0,0,	'??',0,0
_crtab386	db 'cr0',0,	'cr1',0,	'cr2',0,	'cr3',0
_drtab386	db 'dr0',0,	'dr1',0,	'dr2',0,	'dr3',0
		db 'dr4',0,	'dr5',0,	'dr6',0,	'dr7',0
_trtab386	db 'tr0',0,	'tr1',0,	'tr2',0,	'tr3',0
		db 'tr4',0,	'tr5',0,	'tr6',0,	'tr7',0
_jumptab86	db 'jo'	,0,0,	'jno',	0,	'jb' ,0,0,	'jae',	0
		db 'jz'	,0,0,	'jnz',	0,	'jbe',0,	'ja',	0,0
		db 'js'	,0,0,	'jns',	0,	'jpe',0,	'jpo',	0
		db 'jl'	,0,0,	'jge',	0,	'jle',0,	'jg',	0,0
_settab386	db 'seto'	,4 dup(0),	'setno'		,3 dup(0)
		db 'setb'	,4 dup(0),	'setae'		,3 dup(0)
		db 'setz'	,4 dup(0),	'setnz'		,3 dup(0)
		db 'setbe'	,3 dup(0),	'seta'		,4 dup(0)
		db 'sets'	,4 dup(0),	'setns'		,3 dup(0)
		db 'setpe'	,3 dup(0),	'setpo'		,3 dup(0)
		db 'setl'	,4 dup(0),	'setge'		,3 dup(0)
		db 'setle'	,3 dup(0),	'setg'		,4 dup(0)


;=============================================================================
		Align 4
_esc0tab0	dd _text_fadd,	_text_fmul
		dd _text_fcom,	_text_fcomp
		dd _text_fsub,	_text_fsubr
		dd _text_fdiv,  _text_fdivr
_esc0tab1	dd _text_fadd,	_text_fmul
		dd _text_fcom,	_text_fcomp
		dd _text_fsub,	_text_fsubr
		dd _text_fdiv,	_text_fdivr

_esc1tab0	dd _text_fld,	0
		dd _text_fst,	_text_fstp
		dd _text_fldenv,_text_fldcw
		dd _text_fnstenv,_text_fnstcw
_esc1tab1	dd _text_fld,	_text_fxch
		dd _text_fnop,	0
		dd _text_fchs,	_text_fld1
		dd _text_f2xm1,	_text_fprem
		dd _text_fld,	_text_fxch
		dd 0,		0
		dd _text_fabs,	_text_fldl2t
		dd _text_fyl2x,	_text_fyl2xp1
		dd _text_fld,	_text_fxch
		dd 0,		0
		dd 0,		_text_fldl2e
		dd _text_fptan,	_text_fsqrt
		dd _text_fld,	_text_fxch
		dd 0,		0
		dd 0,		_text_fldpi
		dd _text_fpatan,_text_fsincos
		dd _text_fld,	_text_fxch
		dd 0,		0
		dd _text_ftst,	_text_fldlg2
		dd _text_fxtract,_text_frndint
		dd _text_fld,	_text_fxch
		dd 0,		0
		dd _text_fxam,	_text_fldln2
		dd _text_fpremi,_text_fscale
		dd _text_fld,	_text_fxch
		dd 0,		0
		dd 0,		_text_fldz
		dd _text_fdecstp,_text_fsin
		dd _text_fld,	_text_fxch
		dd 0,		0
		dd 0,		0
		dd _text_fincstp,_text_fcos

_esc2tab0	dd _text_fiadd,	_text_fimul
		dd _text_ficom,	_text_ficomp
		dd _text_fisub,	_text_fisubr
		dd _text_fidiv,  _text_fidivr
_esc2tab1	dd 0,		_text_fucompp
		dd 0,		0
		dd 0,		_text_fucompp
		dd 0,		0

_esc3tab0	dd _text_fild,	0
		dd _text_fist,	_text_fistp
		dd _text_fld,	_text_fld
		dd _text_fstp,  _text_fstp
_esc3tab1	dd _text_feni,	_text_fdisi
		dd _text_fnclex,_text_fninit
		dd _text_fsetpm,0
		dd 0, 		0

_esc4tab0	dd _text_fadd,	_text_fmul
		dd _text_fcom,	_text_fcomp
		dd _text_fsub,	_text_fsubr
		dd _text_fdiv,  _text_fdivr
_esc4tab1	dd _text_fadd,	_text_fmul
		dd _text_fcom,	_text_fcomp
		dd _text_fsub,	_text_fsubr
		dd _text_fdiv,	_text_fdivr

_esc5tab0	dd _text_fld,	0
		dd _text_fst,	_text_fstp
		dd _text_frstor,0
		dd _text_fnsave, _text_fnstsw
_esc5tab1	dd _text_ffree,	0
		dd _text_fst,	_text_fstp
		dd _text_fucom,	_text_fucomp
		dd 0,		0

_esc6tab0	dd _text_fiadd,	_text_fimul
		dd _text_ficom,	_text_ficomp
		dd _text_fisub,	_text_fisubr
		dd _text_fidiv,  _text_fidivr
_esc6tab1	dd _text_faddp,	_text_fmulp
		dd 0,		_text_fcompp
		dd _text_fsubrp,_text_fsubp
		dd _text_fdivrp,_text_fdivp

_esc7tab0	dd _text_fild,	0
		dd _text_fist,	_text_fistp
		dd _text_fbld,	_text_fild
		dd _text_fbstp, _text_fistp
_esc7tab1	dd _text_fnstsw,0
		dd 0,		0
		dd _text_fnstsw,0
		dd 0,		0


		Align 4
_grouptab	dd _group0,    _group1,    _group2,   _group3
		dd _group4,    _group5,    _group6,   _group7
		dd _group8,    _group9,    _groupA,   _groupB
		dd _groupC,    _groupD,    _groupE,   _groupF
_group0Ftab	dd _group0F0,  _group0F1,  _group0F2, _group0F3
		dd _group0F4,  _group0F5,  _group0F6, _group0F7
		dd _group0F8,  _group0F9,  _group0FA, _group0FB
		dd _group0FC,  _group0FD,  _group0FE, _group0FF
_type0tab	dd _text_add,  _type_EbGb,	_text_add,  _type_EvGv
		dd _text_add,  _type_GbEb,	_text_add,  _type_GvEv
		dd _text_add,  _type_ALIb,	_text_add,  _type_eAXIv
		dd _text_push, _type_pushs,	_text_pop,  _type_pushs
		dd _text_or,   _type_EbGb,	_text_or,   _type_EvGv
		dd _text_or,   _type_GbEb,	_text_or,   _type_GvEv
		dd _text_or,   _type_ALIb,	_text_or,   _type_eAXIv
		dd _text_push, _type_pushs,	_text_none, _type_0F
_type1tab	dd _text_adc,  _type_EbGb,	_text_adc,  _type_EvGv
		dd _text_adc,  _type_GbEb,	_text_adc,  _type_GvEv
		dd _text_adc,  _type_ALIb,	_text_adc,  _type_eAXIv
		dd _text_push, _type_pushs,	_text_pop,  _type_pushs
		dd _text_sbb,  _type_EbGb,	_text_sbb,  _type_EvGv
		dd _text_sbb,  _type_GbEb,	_text_sbb,  _type_GvEv
		dd _text_sbb,  _type_ALIb,	_text_sbb,  _type_eAXIv
		dd _text_push, _type_pushs,	_text_pop,  _type_pushs
_type2tab	dd _text_and,  _type_EbGb,	_text_and,  _type_EvGv
		dd _text_and,  _type_GbEb,	_text_and,  _type_GvEv
		dd _text_and,  _type_ALIb,	_text_and,  _type_eAXIv
		dd _text_none, _type_es,	_text_daa,  _type_print
		dd _text_sub,  _type_EbGb,	_text_sub,  _type_EvGv
		dd _text_sub,  _type_GbEb,	_text_sub,  _type_GvEv
		dd _text_sub,  _type_ALIb,	_text_sub,  _type_eAXIv
		dd _text_none, _type_cs,	_text_das,  _type_print
_type3tab	dd _text_xor,  _type_EbGb,	_text_xor,  _type_EvGv
		dd _text_xor,  _type_GbEb,	_text_xor,  _type_GvEv
		dd _text_xor,  _type_ALIb,	_text_xor,  _type_eAXIv
		dd _text_none, _type_ss,	_text_aaa,  _type_print
		dd _text_cmp,  _type_EbGb,	_text_cmp,  _type_EvGv
		dd _text_cmp,  _type_GbEb,	_text_cmp,  _type_GvEv
		dd _text_cmp,  _type_ALIb,	_text_cmp,  _type_eAXIv
		dd _text_none, _type_ds,	_text_aas,  _type_print
_type4tab	dd _text_inc,  _type_regv,	_text_inc,  _type_regv
		dd _text_inc,  _type_regv,	_text_inc,  _type_regv
		dd _text_inc,  _type_regv,	_text_inc,  _type_regv
		dd _text_inc,  _type_regv,	_text_inc,  _type_regv
		dd _text_dec,  _type_regv,	_text_dec,  _type_regv
		dd _text_dec,  _type_regv,	_text_dec,  _type_regv
		dd _text_dec,  _type_regv,	_text_dec,  _type_regv
		dd _text_dec,  _type_regv,	_text_dec,  _type_regv
_type5tab	dd _text_push, _type_regv,	_text_push, _type_regv
		dd _text_push, _type_regv,	_text_push, _type_regv
		dd _text_push, _type_regv,	_text_push, _type_regv
		dd _text_push, _type_regv,	_text_push, _type_regv
		dd _text_pop,  _type_regv,	_text_pop,  _type_regv
		dd _text_pop,  _type_regv,	_text_pop,  _type_regv
		dd _text_pop,  _type_regv,	_text_pop,  _type_regv
		dd _text_pop,  _type_regv,	_text_pop,  _type_regv
_type6tab	dd _text_pusha,_type_pushd,	_text_popa, _type_pushd
		dd _text_bound,_type_GvEv,	_text_arpl, _type_EwRw
		dd _text_none, _type_fs,	_text_none, _type_gs
		dd _text_none, _type_opsiz,	_text_none, _type_adrsiz
		dd _text_push, _type_Iv,	_text_imul, _type_GvEvIv
		dd _text_push, _type_pushIb,	_text_imul, _type_GvEvIb
		dd _text_ins,  _type_strb,	_text_ins,  _type_strd
		dd _text_outs, _type_strb,	_text_outs, _type_strd
_type7tab	dd _text_none, _type_jumpb,	_text_none, _type_jumpb
		dd _text_none, _type_jumpb,	_text_none, _type_jumpb
		dd _text_none, _type_jumpb,	_text_none, _type_jumpb
		dd _text_none, _type_jumpb,	_text_none, _type_jumpb
		dd _text_none, _type_jumpb,	_text_none, _type_jumpb
		dd _text_none, _type_jumpb,	_text_none, _type_jumpb
		dd _text_none, _type_jumpb,	_text_none, _type_jumpb
		dd _text_none, _type_jumpb,	_text_none, _type_jumpb
_type8tab	dd _text_none, _type_grp1c80,	_text_none, _type_grp1c81
		dd _text_byte, _type_byte,	_text_none, _type_grp1c83
		dd _text_test, _type_EbGb,	_text_test, _type_EvGv
		dd _text_xchg, _type_EbGb,	_text_xchg, _type_EvGv
		dd _text_mov,  _type_EbGb,	_text_mov,  _type_EvGv
		dd _text_mov,  _type_GbEb,	_text_mov,  _type_GvEv
		dd _text_mov,  _type_EwSw,	_text_lea,  _type_GvEv
		dd _text_mov,  _type_SwEw,	_text_pop,  _type_Ev
_type9tab	dd _text_nop,  _type_print,	_text_xchg, _type_eAXr
		dd _text_xchg, _type_eAXr,	_text_xchg, _type_eAXr
		dd _text_xchg, _type_eAXr,	_text_xchg, _type_eAXr
		dd _text_xchg, _type_eAXr,	_text_xchg, _type_eAXr
		dd _text_none, _type_cdq,	_text_none, _type_cdq
		dd _text_call, _type_Ap,	_text_wait, _type_print
		dd _text_pushf,_type_pushd,	_text_popf, _type_pushd
		dd _text_sahf, _type_print,	_text_lahf, _type_print
_typeAtab	dd _text_mov,  _type_ALOb,	_text_mov,  _type_eAXOv
		dd _text_mov,  _type_ObAL,	_text_mov,  _type_OveAX
		dd _text_movs, _type_strb,	_text_movs, _type_strd
		dd _text_cmps, _type_strb,	_text_cmps, _type_strd
		dd _text_test, _type_ALIb,	_text_test, _type_eAXIv
		dd _text_stos, _type_strb,	_text_stos, _type_strd
		dd _text_lods, _type_strb,	_text_lods, _type_strd
		dd _text_scas, _type_strb,	_text_scas, _type_strd
_typeBtab	dd _text_mov,  _type_movIb,	_text_mov,  _type_movIb
		dd _text_mov,  _type_movIb,	_text_mov,  _type_movIb
		dd _text_mov,  _type_movIb,	_text_mov,  _type_movIb
		dd _text_mov,  _type_movIb,	_text_mov,  _type_movIb
		dd _text_mov,  _type_movIv,	_text_mov,  _type_movIv
		dd _text_mov,  _type_movIv,	_text_mov,  _type_movIv
		dd _text_mov,  _type_movIv,	_text_mov,  _type_movIv
		dd _text_mov,  _type_movIv,	_text_mov,  _type_movIv
_typeCtab	dd _text_none, _type_grp2cC0,	_text_none, _type_grp2cC1
		dd _text_ret,  _type_Iw,	_text_ret,  _type_print
		dd _text_les,  _type_GvMp,	_text_lds,  _type_GvMp
		dd _text_mov,  _type_EbIb,	_text_mov,  _type_EvIv
		dd _text_enter,_type_IwIb,	_text_leave,_type_print
		dd _text_retf, _type_Iw,	_text_retf, _type_print
		dd _text_int3, _type_print,	_text_int,  _type_Ib
		dd _text_into, _type_print,	_text_iret, _type_pushd
_typeDtab	dd _text_none, _type_grp2cD0,	_text_none, _type_grp2cD1
		dd _text_none, _type_grp2cD2,	_text_none, _type_grp2cD3
		dd _text_aam,  _type_aamd,	_text_aad,  _type_aamd
		dd _text_setalc, _type_print,	_text_xlat, _type_print
		dd _text_none, _type_esc0,	_text_none, _type_esc1
		dd _text_none, _type_esc2,	_text_none, _type_esc3
		dd _text_none, _type_esc4,	_text_none, _type_esc5
		dd _text_none, _type_esc6,	_text_none, _type_esc7
_typeEtab	dd _text_loopne, _type_loopne,	_text_loope, _type_loope
		dd _text_loop, _type_loop,	_text_none, _type_jcxz
		dd _text_in,   _type_ALIb,	_text_in,   _type_eAXIb
		dd _text_out,  _type_IbAL,	_text_out,  _type_IbeAX
		dd _text_call, _type_Jv,	_text_jmp,  _type_jumpJv
		dd _text_jmp,  _type_jumpAp,	_text_jmp,  _type_jumpJb
		dd _text_in,   _type_ALDX,	_text_in,   _type_eAXDX
		dd _text_out,  _type_DXAL,	_text_out,  _type_DXeAX
_typeFtab	dd _text_lock, _type_print,	_text_byte, _type_byte
		dd _text_repne, _type_rep,	_text_rep,  _type_rep
		dd _text_hlt,  _type_print,	_text_cmc,  _type_print
		dd _text_none, _type_grp3cF6,	_text_none, _type_grp3cF7
		dd _text_clc,  _type_print,	_text_stc,  _type_print
		dd _text_cli,  _type_print,	_text_sti,  _type_print
		dd _text_cld,  _type_print,	_text_std,  _type_print
		dd _text_none, _type_grp4cFE,	_text_none, _type_grp5cFF

;/////////////////////////////////////////////////////////////////////////////
_type0tab0F	dd _text_none, _type_grp6c,	_text_none, _type_grp7c
		dd _text_lar,  _type_GvEw,	_text_lsl,  _type_GvEw
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_clts, _type_print,	_text_byte, _type_byte
		dd _text_invd, _type_print,	_text_wbinvd, _type_print
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
_type1tab0F	dd _text_invlpg, _type_Ea,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
_type2tab0F	dd _text_mov,  _type_CdRd,	_text_mov,  _type_DdRd
		dd _text_mov,  _type_RdCd,	_text_mov,  _type_RdDd
		dd _text_mov,  _type_TdRd,	_text_byte, _type_byte
		dd _text_mov,  _type_RdTd,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
_type3tab0F	dd _text_wrmsr,_type_print,	_text_rdtsc, _type_print
		dd _text_rdmsr,_type_print,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
_type4tab0F	dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
_type5tab0F	dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
_type6tab0F	dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
_type7tab0F	dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_emms, _type_print
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
_type8tab0F	dd _text_none, _type_jumpv,	_text_none, _type_jumpv
		dd _text_none, _type_jumpv,	_text_none, _type_jumpv
		dd _text_none, _type_jumpv,	_text_none, _type_jumpv
		dd _text_none, _type_jumpv,	_text_none, _type_jumpv
		dd _text_none, _type_jumpv,	_text_none, _type_jumpv
		dd _text_none, _type_jumpv,	_text_none, _type_jumpv
		dd _text_none, _type_jumpv,	_text_none, _type_jumpv
		dd _text_none, _type_jumpv,	_text_none, _type_jumpv
_type9tab0F	dd _text_none, _type_setb,	_text_none, _type_setb
		dd _text_none, _type_setb,	_text_none, _type_setb
		dd _text_none, _type_setb,	_text_none, _type_setb
		dd _text_none, _type_setb,	_text_none, _type_setb
		dd _text_none, _type_setb,	_text_none, _type_setb
		dd _text_none, _type_setb,	_text_none, _type_setb
		dd _text_none, _type_setb,	_text_none, _type_setb
		dd _text_none, _type_setb,	_text_none, _type_setb
_typeAtab0F	dd _text_push, _type_pushs,	_text_pop,  _type_pushs
		dd _text_cpuid,_type_print,	_text_bt,   _type_EvGv
		dd _text_shld, _type_EvGvIb,	_text_shld, _type_EvGvCL
		dd _text_cmpx, _type_EbGb,	_text_cmpx, _type_EvGv
		dd _text_push, _type_pushs,	_text_pop,  _type_pushs
		dd _text_byte, _type_byte,	_text_bts,  _type_EvGv
		dd _text_shrd, _type_EvGvIb,	_text_shrd, _type_EvGvCL
		dd _text_byte, _type_byte,	_text_imul, _type_GvEv
_typeBtab0F	dd _text_cmpx, _type_EbGb,	_text_cmpx, _type_EvGv
		dd _text_lss,  _type_GvMp,	_text_btr,  _type_EvGv
		dd _text_lfs,  _type_GvMp,	_text_lgs,  _type_GvMp
		dd _text_movzx,_type_GvEb,	_text_movzx,_type_GvEw
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_none, _type_grp8c,	_text_btc,  _type_EvGv
		dd _text_bsf,  _type_GvEv,	_text_bsr,  _type_GvEv
		dd _text_movsx,_type_GvEb,	_text_movsx,_type_GvEw
_typeCtab0F	dd _text_xadd, _type_EbGb,	_text_xadd, _type_EvGv
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_cmpxchg8b, _type_Ea
		dd _text_bswap,_type_regv,	_text_bswap,_type_regv
		dd _text_bswap,_type_regv,	_text_bswap,_type_regv
		dd _text_bswap,_type_regv,	_text_bswap,_type_regv
		dd _text_bswap,_type_regv,	_text_bswap,_type_regv
_typeDtab0F	dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
_typeEtab0F	dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
_typeFtab0F	dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
		dd _text_byte, _type_byte,	_text_byte, _type_byte
_grp1tab	dd _text_add,	_text_or,	_text_adc,	_text_sbb
		dd _text_and,	_text_sub,	_text_xor,	_text_cmp
_grp2tab	dd _text_rol,	_text_ror,	_text_rcl,	_text_rcr
		dd _text_shl,	_text_shr,	_text_none,	_text_sar
_grp3tab	dd _text_test,	_text_none,	_text_not,	_text_neg
		dd _text_mul,	_text_imul,	_text_div,	_text_idiv
_grp4tab	dd _text_inc,	_text_dec,	_text_none,	_text_none
		dd _text_none,	_text_none,	_text_none,	_text_none
_grp5tab	dd _text_inc,	_text_dec,	_text_call,	_text_call
		dd _text_jmp,	_text_jmp,	_text_push,	_text_none
_grp6tab	dd _text_sldt,	_text_str,	_text_lldt,	_text_ltr
		dd _text_verr,	_text_verw,	_text_none,	_text_none
_grp7tab	dd _text_sgdt,	_text_sidt,	_text_lgdt,	_text_lidt
		dd _text_smsw,	_text_none,	_text_lmsw,	_text_invlpg
_grp8tab	dd _text_none,	_text_none,	_text_none,	_text_none
		dd _text_bt,	_text_bts,	_text_btr,	_text_btc
_r86tab		dd _r86c0, _r86c1, _r86c2, _r86c3
		dd _r86c4, _r86c5, _r86c6, _r86c7



