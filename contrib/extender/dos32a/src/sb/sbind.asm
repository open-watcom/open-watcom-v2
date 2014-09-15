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

		.486p
		.MODEL	flat
		LOCALS
		NOJUMPS

include	stddef.inc

EXTRN	_filesize	:dword
EXTRN	_Main_Type	:dword
EXTRN	_Exec_Type	:dword
EXTRN	_tempname	:dword

PUBLIC	open_exec_, close_exec_
PUBLIC	check_if_unbound_exec_
PUBLIC	get_exec_start_
PUBLIC	get_exec_type_
PUBLIC	get_extender_type_
PUBLIC	_exec_handle
PUBLIC	unbind_exec_
PUBLIC	bind_exec_
PUBLIC	find_oem_info_

.CODE
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ





bind_exec_:
	pushad
	mov	_tmp_handle1,eax
	mov	_tmp_handle2,edx
	mov	_tmp_size1,ebx
	mov	_tmp_size2,ecx

	lea	eax,[ebx+ecx+0FFFh]
	call	allocate_memory
	jc	@@err1

	mov	ebx,_tmp_handle1		; load stub
	mov	ecx,_tmp_size1
	mov	edx,dword ptr _mem_ptr
	mov	ah,3Fh
	int	21h
	jc	@@err2

	movzx	eax,word ptr [edx+0018h]	; get reloc tab offset
	mov	esi,dword ptr _mem_ptr
	add	esi,eax
	add	esi,_tmp_size1
	mov	ebx,0040h
	mov	edi,dword ptr _mem_ptr
	add	edi,ebx
	add	edi,_tmp_size1
	mov	ecx,_tmp_size1
	inc	ecx
	std
	rep	movsb
	cld

	mov	edi,dword ptr _mem_ptr
	add	edi,eax
	sub	ebx,eax
	jc	@@err6				; invalid stub format
	mov	ecx,ebx
	xor	eax,eax
	rep	stosb
	mov	eax,_tmp_size1
	add	eax,ebx
	mov	_tmp_size1,eax

	mov	esi,dword ptr _mem_ptr
	mov	edx,eax
	and	eax,1FFh
	mov	[esi+02h],ax

	mov	eax,edx
	shr	eax,9
	inc	eax
	mov	[esi+04h],ax

	movzx	eax,word ptr [esi+06]
	lea	eax,[eax*4]
	add	eax,004Fh
	shr	eax,4
	mov	[esi+08h],ax

	mov	ax,0040h
	mov	[esi+18h],ax

	mov	eax,_tmp_size1
	mov	[esi+3Ch],eax

	mov	ebx,_tmp_handle2		; load exec
	mov	ecx,_tmp_size2
	mov	edx,dword ptr _mem_ptr
	add	edx,_tmp_size1
	mov	ah,3Fh
	int	21h
	jc	@@err3

	pushad
	mov	eax,_exec_start
	mov	ecx,eax
	mov	edx,eax
	shr	ecx,16
	mov	ebx,_tmp_handle2
	mov	ax,4200h
	int	21h				; move to next exec in file
	popad
	jc	@@err3

	mov	eax,_tmp_size1			; update Data Pages Offset
	mov	ebp,[edx+80h]
	add	eax,ebp

	cmp _Main_Type,0
	jz	@@0

	cmp	_Main_Type,1
	jz	@@1
	cmp	_Main_Type,2
	jz	@@1
	jmp	@@2

@@0:
	cmp	_Exec_Type,1
	jz	@@1
	cmp	_Exec_Type,2
	jz	@@1
	jmp	@@2

@@1:	mov	[edx+80h],eax

@@2:	mov	edx,_tempname			; create temp file
	mov	ecx,00h
	mov	ah,3Ch
	int	21h
	mov	_temp_handle,ax
	jc	@@err4

	mov	edx,dword ptr _mem_ptr
	mov	ecx,_tmp_size1
	add	ecx,_tmp_size2
	mov	bx,_temp_handle			; write to temp file
	mov	ah,40h
	int	21h
	jc	@@err5

	mov	bx,_temp_handle			; close temp file
	mov	ah,3Eh
	int	21h
	call	free_memory
	popad
	xor	eax,eax
	ret

@@err1:	call	free_memory
	popad
	mov	eax,-1
	ret
@@err2:	call	free_memory
	popad
	mov	eax,-2
	ret
@@err3:	call	free_memory
	popad
	mov	eax,-3
	ret
@@err4:	call	free_memory
	popad
	mov	eax,-4
	ret
@@err5:	call	free_memory
	popad
	mov	eax,-5
	ret
@@err6:	call	free_memory
	popad
	mov	eax,-6
	ret


unbind_exec_:
	pushad
	call	seek_to_start
	mov	eax,_filesize			; allocate memory
	add	eax,0FFFh
	call	allocate_memory
	jc	@@err1

	mov	bx,_exec_handle			; load exec
	mov	ecx,_filesize
	mov	edx,dword ptr _mem_ptr
	mov	ah,3Fh
	int	21h
	jc	@@err2

	mov	edx,_tempname			; create temp file
	mov	ecx,00h
	mov	ah,3Ch
	int	21h
	mov	_temp_handle,ax
	jc	@@err3

	mov	edx,dword ptr _mem_ptr
	mov	ecx,_filesize
	add	edx,_exec_start
	sub	ecx,_exec_start
	mov	eax,[edx+80h]			; update Data Pages Offset
	mov	ebp,_exec_start
	sub	ebp,_app_off_datapages
	sub	eax,ebp
	cmp	_Exec_Type,1
	jz	@@1
	cmp	_Exec_Type,2
	jz	@@1
	jmp	@@2
@@1:	mov	[edx+80h],eax

@@2:	mov	bx,_temp_handle			; write to temp file
	mov	ah,40h
	int	21h
	jc	@@err4

	mov	bx,_temp_handle			; close temp file
	mov	ah,3Eh
	int	21h
	call	free_memory
	popad
	xor	eax,eax
	ret

@@err1:	call	free_memory
	popad
	mov	eax,-1
	ret
@@err2:	call	free_memory
	popad
	mov	eax,-2
	ret
@@err3:	call	free_memory
	popad
	mov	eax,-3
	ret
@@err4:	call	free_memory
	popad
	mov	eax,-4
	ret




;-----------------------------------------------------------------------------
allocate_memory:
	pushad
	mov	ebx,eax
	mov	ax,0FF91h
	int	21h
	mov	_mem_ptr,ebx
	mov	_mem_handle,esi
	popad
	ret

free_memory:
	pushad
	mov	esi,_mem_handle
	mov	ax,0FF92h
	int	21h
	popad
	ret







open_exec_:
	pushad
	xor	ecx,ecx
	mov	edx,eax
	mov	ax,4300h
	int	21h
	test	ecx,01h
	jnz	@@err2
	mov	ax,3D02h
	int	21h
	jc	@@err1
	mov	_exec_handle,ax
	popad
	xor	eax,eax
	ret
@@err1:	popad
	mov	eax,-1
	ret
@@err2:	popad
	mov	eax,-2
	ret

close_exec_:
	pushad
	mov	bx,_exec_handle
	mov	ah,3Eh
	int	21h
	popad
	ret


check_if_unbound_exec_:
	pushad
	call	seek_to_start
	jc	@@err1
	mov	ax,3F00h
	mov	bx,_exec_handle
	mov	ecx,64			; ECX = 64 bytes to load
	mov	edx,_buffer		; DS:EDX = ptr
	int	21h
	jc	@@err2
	call	seek_to_start
	jc	@@err1
	cmp	word ptr [edx],'ZM'	; is exec 'MZ' file type
	jz	@@0
	cmp	word ptr [edx],'EL'	; is exec 'LE' file type
	jz	@@1
	cmp	word ptr [edx],'XL'	; is exec 'LX' file type
	jz	@@2
	cmp	word ptr [edx],'CL'	; is exec 'LC' file type
	jz	@@3
	cmp	word ptr [edx],'EP'	; is exec 'PE' file type
	jz	@@4
	cmp	dword ptr [edx],'1WMP'	; is exec 'PMW1' file type
	jz	@@5
	popad
	mov	eax,-3
	ret
@@err1:	popad
	mov	eax,-1
	ret
@@err2:	popad
	mov	eax,-2
	ret

@@0:	popad
	mov	eax,0
	ret
@@1:	popad
	mov	eax,1
	ret
@@2:	popad
	mov	eax,2
	ret
@@3:	popad
	mov	eax,3
	ret
@@4:	popad
	mov	eax,4
	ret
@@5:	popad
	mov	eax,5
	ret




get_exec_start_:
	pushad
	mov	_saved_stack,esp
	call	load_exec_header
	popad
	xor	eax,eax
	ret

@err1:	mov	esp,_saved_stack
	popad
	mov	eax,-1
	ret
@err2:	mov	esp,_saved_stack
	popad
	mov	eax,-2
	ret


load_exec_header:
	call	seek_to_start
	jc	@err1

	mov	ax,3F00h
	mov	bx,_exec_handle
	mov	ecx,64			; ECX = 64 bytes to load
	mov	edx,_buffer		; DS:EDX = ptr
	int	21h
	jc	@err1

	call	seek_to_start
	jc	@err1

	xor	ebp,ebp			; reset ptr in app
	cmp	word ptr [edx],'ZM'	; is exec 'MZ' file type
	jnz	search_for_le		; if not, search for known exec type

	mov	eax,[edx+18h]		; MZ reloc-tab must be at offset 0040h
	cmp	ax,40h
	jnz	search_for_mz

	mov	eax,[edx+3Ch]		; get start of 32-bit code
	test	ax,ax			; check if exec is bound
	jz	search_for_mz		; if not, search
	mov	_exec_start,eax
	mov	_app_off_datapages,0
	ret


search_for_mz:
	xor	esi,esi

@@0:	movzx	eax,wptr [edx+04h]	; get pages in file
	shl	eax,9			; *512
	movzx	ebx,wptr [edx+02h]	; get bytes on last page
	add	eax,ebx
	cmp	wptr [edx],'ZM'
	jz	@@1
	cmp	wptr [edx],'WB'
	jz	@@2
	jmp	@@3
@@1:	sub	eax,0200h
@@2:	mov	esi,ebp
	add	ebp,eax

	mov	ecx,ebp
	mov	edx,ebp
	shr	ecx,16
	mov	bx,_exec_handle
	mov	ax,4200h
	int	21h			; move to next exec in file
	jc	@err1
	mov	ax,3F00h
	mov	bx,_exec_handle
	mov	ecx,64
	mov	edx,_buffer
	int	21h
	jc	@err1
	test	eax,eax
	jz	@err2			; unknown exec format
	jmp	@@0

@@3:	cmp	wptr [edx],'EL'
	jz	@@4
	cmp	wptr [edx],'XL'
	jz	@@4
	cmp	wptr [edx],'CL'
	jz	@@4
	mov	ecx,ebp
	mov	edx,ebp
	shr	ecx,16
	mov	bx,_exec_handle
	mov	ax,4200h
	int	21h			; move to next exec in file
	jc	@err1
	call	search_for_le
@@4:	mov	_exec_start,ebp
	mov	_app_off_datapages,esi
	ret


search_for_le:
@@1:	mov	edx,_buffer		; DS:EDX = current ptr
	mov	ecx,1000h		; ECX = bytes to load
	mov	ax,3F00h
	mov	bx,_exec_handle
	int	21h
	jc	@err1
	test	eax,eax			; check if no bytes read
	jz	@err2			; if true, no app in file
	shr	ecx,1
@@2:	mov	ax,[edx+0]
	mov	bx,[edx+2]
	test	bx,bx
	jnz	@@4
	cmp	ax,'EL'			; 'LE' type
	jz	@@3
	cmp	ax,'XL'			; 'LX' type
	jz	@@3
@@4:	cmp	ax,'CL'			; 'LC' type (Linear Compressed)
	jz	@@3
	add	edx,2
	add	ebp,2			; increment pointer in file
	loop	@@2
	jmp	@@1
@@3:	mov	_app_off_datapages,0
	ret


get_exec_type_:
	pushad
	mov	edx,_exec_start
	mov	ecx,edx
	shr	ecx,16
	mov	bx,_exec_handle
	mov	ax,4200h
	int	21h
	jc	@@err1

	mov	ax,3F00h
	mov	bx,_exec_handle
	mov	ecx,64
	mov	edx,_buffer
	int	21h
	jc	@@err1
	test	eax,eax
	jz	@@err2			; unknown exec format

	cmp	wptr [edx],'EL'
	jz	@@1
	cmp	wptr [edx],'XL'
	jz	@@2
	cmp	wptr [edx],'CL'
	jz	@@3
	cmp	wptr [edx],'EP'
	jz	@@4
	cmp	dptr [edx],'1WMP'
	jz	@@5

@@err2:	popad
	mov	eax,-2
	ret
@@err1:	popad
	mov	eax,-1
	ret

@@0:	popad
	xor	eax,eax
	ret
@@1:	popad
	mov	eax,1
	ret
@@2:	popad
	mov	eax,2
	ret
@@3:	popad
	mov	eax,3
	ret
@@4:	popad
	mov	eax,4
	ret
@@5:	popad
	mov	eax,5
	ret



get_extender_type_:
	pushad
	call	seek_to_start
	jc	@@err1

	mov	ax,3F00h
	mov	bx,_exec_handle
	mov	ecx,0400h		; ECX = 64 bytes to load
	mov	edx,_buffer		; DS:EDX = ptr
	int	21h
	jc	@@err1

	mov	ebx,_buffer
	mov	esi,offs _str_dos32a
	lea	edi,[ebx+009Ah]
	call	check_string
	jnc	@@1
	mov	ebx,_buffer
	mov	esi,offs _str_stb32a
	lea	edi,[ebx+006Ah]
	call	check_string
	jnc	@@2
	mov	ebx,_buffer
	mov	esi,offs _str_stub32a
	lea	edi,[ebx+0040h]
	call	check_string
	jnc	@@3

	mov	ebx,_buffer
	mov	esi,offs _str_dos32a_new
	lea	edi,[ebx+009Ch]
	call	check_string
	jnc	@@1
	mov	ebx,_buffer
	mov	esi,offs _str_stub32c_new
	lea	edi,[ebx+006Ch]
	call	check_string
	jnc	@@2
	mov	ebx,_buffer
	mov	esi,offs _str_stub32a_new
	lea	edi,[ebx+0040h]
	call	check_string
	jnc	@@3

	mov	esi,offs _str_dos4g
	lea	edi,[ebx+025Ah]
	call	check_string
	jnc	@@4
	mov	esi,offs _str_dos4g
	lea	edi,[ebx+025Ch]
	call	check_string
	jnc	@@4

	mov	esi,offs _str_pmodew
	lea	edi,[ebx+0055h]
	call	check_string
	jnc	@@5

	popad
	xor	eax,eax
	ret
@@err1:	popad
	mov	eax,-1
	ret

@@1:	popad
	mov	eax,1			; DOS/32A
	ret
@@2:	popad
	mov	eax,2			; STUB/32A Configurable
	ret
@@3:	popad
	mov	eax,3			; STUB/32A Standard
	ret
@@4:	popad
	mov	eax,4			; DOS/4G
	ret
@@5:	popad
	mov	eax,5			; PMODE/W
	ret


check_string:
	pushad
@@0:	mov	al,[esi]
	test	al,al
	jz	@@ok
	cmp	al,[edi]
	jnz	@@err
	inc	esi
	inc	edi
	jmp	@@0
@@ok:	popad
	clc
	ret
@@err:	popad
	stc
	ret


seek_to_start:
	pushad
	xor	ecx,ecx
	xor	edx,edx
	jmp	common_seek
seek_from_start:
	pushad
	mov	ecx,edx
	shr	ecx,16
common_seek:
	mov	bx,_exec_handle
	mov	ax,4200h
	int	21h
	popad
	ret
seek_from_current:
	pushad
	mov	ecx,edx
	shr	ecx,16
	mov	bx,_exec_handle
	mov	ax,4201h
	int	21h
	popad
	ret
read_from_exec:
	pushad
	mov	ah,3Fh
	mov	bx,_exec_handle
	int	21h
	mov	[esp+1Ch],eax
	popad
	ret


find_oem_info_:
	pushad
	mov	edx,_exec_start
	call	seek_from_start
	jc	@@err

	mov	ecx,10h				; read LC header
	mov	edx,_buffer
	call	read_from_exec
	jc	@@err

	mov	al,[edx+05h]			; get LC flags
	test	al,80h				; check if OEM is appended
	jz	@@err

	xor	esi,esi				; current Object #
	movzx	ebp,byte ptr [edx+04h]		; get # of Objects

@@1:	mov	ecx,10h				; read Object header
	mov	edx,_buffer
	call	read_from_exec
	jc	@@err
	mov	edx,[edx+04h]			; get Object compressed size
	call	seek_from_current
	jc	@@err
	inc	esi
	cmp	esi,ebp
	jnz	@@1

	mov	ecx,0Ch				; read Fixups header
	mov	edx,_buffer
	call	read_from_exec
	jc	@@err
	mov	edx,[edx+04h]			; get Fixups compressed size
	call	seek_from_current
	jc	@@err

	xor	eax,eax
	mov	ecx,128
	mov	edi,_buffer
	rep	stosd

	mov	ecx,512
	mov	edx,_buffer
	call	read_from_exec
	jc	@@err
	test	eax,eax				; if read nothing, no OEM info
	jz	@@err

	popad
	mov	eax,_buffer
	ret

@@err:	popad
	xor	eax,eax
	ret



.DATA
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
_buffer			dd offset @buffer

_str_dos32a_new		db 'DOS/32A',0
_str_stub32a_new	db 'STUB/32A',0
_str_stub32c_new	db 'STUB/32C',0

_str_dos32a		db 'DOS/32 Advanced.',0
_str_stb32a		db 'DOS/32 Advanced!',0
_str_stub32a		db 'DOS/32 Advanced stub file.',0
_str_dos4g		db 'DOS/4G',0
_str_pmodew		db 'PMODE/W',0

_exec_start		dd 0
_app_off_datapages	dd 0



.DATA?
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
@buffer			db 10000h dup(?)
_tmp_handle1		dd ?
_tmp_handle2		dd ?
_tmp_size1		dd ?
_tmp_size2		dd ?

_mem_ptr		dd ?
_mem_handle		dd ?

_exec_handle		dw ?
_saved_stack		dd ?
_temp_handle		dw ?

end
