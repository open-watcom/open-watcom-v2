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
; DOS/32A LC-style 32-bit (compressed) application loader
;
;*****************************************************************************

;-----------------------------------------------------------------------------
; LC Header Format
;
;	0000	DD	"LC" \0\0
;	0004	DB	# of Objects
;	0005	DB	LC-flags
;			  bit0..3: LC implementation version
;			  bit4..7: reserved
;	0006	DB	EIP Object #
;	0007	DB	ESP Object #
;	0008	DD	EIP Offset
;	000C	DD	ESP Offset
;	0010	...	...Object #1 Header follows...
;
;-----------------------------------------------------------------------------
; Object Header Format
;
;	0000	DD	Virtual Size (Uncompressed Size)
;			  bit31: 0=encoded, 1=not encoded
;	0004	DD	Compressed Size
;	0008	DW	Object Flags
;	000A	DW	Extended Object Flags
;	000C	DW	Page Table Index
;	000E	DW	# of Page Table Entries
;	0010	...	...Object Data follows...
;
;-----------------------------------------------------------------------------
;
; Fixups Header Format
;
;	0000	DD	Uncompresed Size
;			  bit31: 0=encoded, 1=not encoded
;	0004	DD	Compressed Size
;	0008	DD	Fixup Record Table Offset
;	000C	...	...Fixup Page Table Data ...
; 000C+[0008]	...	...Fixup Record Table Data ...
;

LC_SPECVER	equ 04h

;=============================================================================
load_lc_header:
	mov	cl,0Ch			; load 'LC' header
	call	load_fs_block
	xor	eax,eax
	mov	al,fs:[0004h]		; get # Objects
	mov	_app_num_objects,eax
	mov	al,fs:[0006h]		; get EIP Object #
	mov	_app_eip_object,eax
	mov	al,fs:[0007h]		; get ESP Object #
	mov	_app_esp_object,eax
	mov	eax,fs:[0008h]		; get EIP
	mov	_app_eip,eax
	mov	eax,fs:[000Ch]		; get ESP
	mov	_app_esp,eax
	mov	al,fs:[0005h]
	and	al,0Fh
	cmp	al,LC_SPECVER		; check LC implementation version
	mov	ax,3006h		; "exec not supported"
	jnz	file_error
	mov	ax,4007h		; "not enough DOS Transfer Buffer"
	cmp	_lobufsize,2000h	; must be at least 8KB
	jb	file_error
	ret



;-----------------------------------------------------------------------------
load_lc_object:
	mov	ecx,10h
	xor	edx,edx
	mov	_err_code,3002h		; "error in app file"
	call	load_fs_block		; load object header
	mov	eax,fs:[0000h]		; get Virtual_Size[Object]
	btr	eax,31			; check if encoded and clear bit "31"
	setc	_ic_byte		; if not encoded, set _ic_byte to "1"
	push	eax
	call	alloc_block		; allocate EAX memory block to EDI
	mov	ecx,eax			; ECX = bytes to read
	mov	edx,edi			; EDX = address to read to
	call	fill_zero_pages		; fill allocated memory with zeroes
	mov	_err_code,3002h		; "error in app file"
	mov	ebx,fs:[0004h]		; EBX = compressed data size
	mov	ecx,ebx
	jecxz	@@done			; skip null-size Objects
	cmp	_ic_byte,0
	jnz	@@1
	mov	ax,0FF91h
	int	21h
	mov	ax,4003h		; "not enough extended memory"
	jc	file_error
	mov	_app_buf_fixrecstab,esi	; preserve memory block handle
	mov	edx,ebx
	call	decompress_data
	mov	ax,0FF92h
	mov	esi,_app_buf_fixrecstab
	int	21h
	jmp	@@done
@@1:	call	load_gs_block
@@done:	pop	ebp			; get Virtual_Size[Object]
	movzx	ebx,word ptr fs:[000Eh]	; get # Page Table Entries
	movzx	edx,word ptr fs:[0008h]	; get Flags[Object]
	movzx	esi,word ptr fs:[000Ch]	; get Page Table Index
	pop	ecx
	ret



preload_lc_fixups:
	mov	ecx,0Ch
	xor	edx,edx
	mov	_err_code,3002h		; "error in app file"
	call	load_fs_block		; load LC Fixup Header
	mov	ax,0FF91h		; alloc memory for Decoded fixups
	mov	ebx,fs:[0000h]
	btr	ebx,31			; check if encoded and clear bit "31"
	setc	_ic_byte		; if not encoded, set _ic_byte to "1"
	mov	ecx,ebx
	inc	ebx			; prevent error when EBX = 0
	int	21h
	mov	ax,4003h
	jc	file_error
	mov	edx,ebx
	mov	edi,ebx
	mov	_app_buf_fixrecstab,esi
	call	fill_zero_pages		; fill allocated memory with zeroes
	mov	ebx,fs:[0004h]
	mov	ecx,ebx
	cmp	_ic_byte,0
	jnz	@@1
	mov	ax,0FF91h		; alloc memory for Encoded fixups
	inc	ebx			; prevent error when EBX = 0
	int	21h
	mov	ax,4003h
	jc	file_error
	mov	edx,ebx
	call	decompress_data
	mov	ax,0FF92h
	int	21h
	jmp	@@done
@@1:	call	load_gs_block
@@done:	mov	_app_off_fixpagetab,edi
	add	edi,fs:[0008h]
	mov	_app_off_fixrectab,edi
	ret

unload_lc_fixups:
	mov	ax,0FF92h
	mov	esi,_app_buf_fixrecstab
	int	21h
	ret



;=============================================================================
; EBX = source address
; EDI = destination address
; ECX = size of source
;
decompress_data:
	call	load_gs_block

decompress:
	pushad
	mov	_xc_byte,0
	mov	_srcaddr,ebx
	mov	_codesize,ecx
	push	edi
	mov	ecx,_lobufsize
	mov	esi,_lobufbase
	mov	edi,_lobufzero
	call	fill_zero_pages
	pop	edi
	xor	bx,bx
	mov	edx,0FEEh
@@3:	shr	bx,1
	and	dx,0FFFh
	test	bh,01h
	je	@@6
@@4:	test	bl,01h
	je	@@8
	call	getbyte
	js	@@5
	mov	[esi+edx],al
	mov	gs:[edi],al
	inc	dx
	inc	edi
	jmp	@@3
@@5:	mov	_codesize,edi		; for INT 21h / AX=0FF8Dh function
	popad
	ret
@@6:	call	getbyte
	js	@@5
	or	ah,-1
	mov	bx,ax
	jmp	@@4
@@8:	call	getbyte
	js	@@5
	mov	cl,al
	call	getbyte
	js	@@5
	mov	ch,al
	shr	ch,4
	and	ax,0Fh
	add	al,02h
	mov	bp,ax
	test	ax,ax
	jl	@@3
@@9:	and	cx,0FFFh
	and	dx,0FFFh
	mov	al,[esi+ecx]
	mov	[esi+edx],al
	mov	gs:[edi],al
	inc	cx
	inc	dx
	inc	edi
	dec	bp
	jns	@@9
	jmp	@@3


;-----------------------------------------------------------------------------
getbyte:mov	eax,_srcaddr
	mov	al,gs:[eax]
	xor	al,_xc_byte
	inc	_srcaddr
	mov	_xc_byte,al
	dec	_codesize
	ret




;-----------------------------------------------------------------------------
_ic_byte	equ	_int_number			;db 0
_xc_byte	equ	_app_load			;db 0
_srcaddr	equ	_app_tmp_addr1			;dd 0
_codesize	equ	_app_tmp_addr2			;dd 0

