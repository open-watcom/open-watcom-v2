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

; superviser mode: can debug LC apps
if SVR eq 1

.CODE

load_lc_app:
	mov	_app_type,2
	mov	__saved_esp,esp
	call	load_lc_header		; load 'LC' exec header
	mov	ecx,1			; start with Object #1
@@1:	call	load_lc_object		; load compressed Object
	call	create_selector		; allocate selector for loaded object
	push	edx			; save Object Selector/Object Flags
	push	edi			; save Address of loaded Object
	push	esi			; save Page Table Index
	push	ebx			; save # Page Table Entries
	inc	ecx			; increment Current_Object#
	cmp	ecx,_app_num_objects
	jbe	@@1			; loop until all objects are loaded
	call	preload_lc_fixups	; preload fixup tables and records
	mov	ebp,esp			; base pointer to last loaded Object
	mov	ebx,_app_num_objects	; number of Objects
	dec	ebx
	shl	ebx,4
	mov	_app_tmp_addr1,ebx
@@2:	call	relocate_lx_object
	sub	ebx,10h
	jnc	@@2
	call	unload_lc_fixups	; free allocated memory for fixups
	mov	esp,__saved_esp
	ret




;=============================================================================
load_lc_header:
	mov	ecx,10h			; load 'LC' header
	xor	edx,edx
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
	ret



load_lc_object:
	push	ecx
	mov	ecx,10h
	xor	edx,edx
	call	load_fs_block		; load object header
	mov	eax,fs:[0000h]		; get Virtual_Size[Object]
	btr	eax,31
	setc	_ic_byte
	call	alloc_block		; allocate EAX memory block to EDI
	mov	ecx,eax			; ECX = bytes to read
	mov	edx,edi			; EDX = addres to read to
	call	fill_zero_pages		; fill allocated memory with zeroes
	mov	ax,0FF91h
	mov	ebx,fs:[0004h]		; EBX = compressed data size
	mov	ecx,ebx
	int	21h
	mov	_app_buf_fixrecstab,esi	; preserve memory block handle
	mov	edx,ebx
	call	load_gs_block
	call	decompress_data
	mov	ax,0FF92h
	mov	esi,_app_buf_fixrecstab
	int	21h
	mov	ebp,fs:[0000h]		; get Virtual_Size[Object]
	movzx	ebx,word ptr fs:[000Eh]	; get # Page Table Entries
	movzx	edx,word ptr fs:[0008h]	; get Flags[Object]
	movzx	esi,word ptr fs:[000Ch]	; get Page Table Index
	pop	ecx
	ret



preload_lc_fixups:
	mov	ecx,0Ch
	xor	edx,edx
	call	load_fs_block		; load LC Fixup Header
	mov	ax,0FF91h		; alloc memory for Decoded fixups
	mov	ebx,fs:[0000h]
	btr	ebx,31
	setc	_ic_byte
	mov	ecx,ebx
	inc	ebx
	int	21h
	mov	edx,ebx
	mov	edi,ebx
	mov	_app_buf_fixrecstab,esi
	call	fill_zero_pages		; fill allocated memory with zeroes
	mov	ax,0FF91h		; alloc memory for Encoded fixups
	mov	ebx,fs:[0004h]
	mov	ecx,ebx
	inc	ebx
	int	21h
	mov	edx,ebx
	call	load_gs_block
	call	decompress_data
	mov	_app_off_fixpagetab,edi
	add	edi,fs:[0008h]
	mov	_app_off_fixrectab,edi
	mov	ax,0FF92h
	int	21h
	ret



unload_lc_fixups:
	mov	ax,0FF92h
	mov	esi,_app_buf_fixrecstab
	int	21h
	ret



decompress_data:
	cmp	_ic_byte,0
	jz	decompress
	pushad
	push	ds es ds ds
	pop	es ds
	mov	esi,ebx
	mov	edx,ecx
	shr	ecx,2
	rep	movs dword ptr es:[edi],[esi]
	mov	cl,dl
	and	cl,3
	rep	movs byte ptr es:[edi],[esi]
	pop	es ds
	popad
	ret

decompress:
	push	eax
	mov	ax,0FF8Dh
	int	21h
	pop	eax
	ret




.DATA?
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

_ic_byte	db 0

endif
