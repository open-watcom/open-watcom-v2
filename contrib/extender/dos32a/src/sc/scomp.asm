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

		.486p
		.MODEL	flat
		LOCALS
		NOJUMPS

include	stddef.inc

APP_MAXOBJECTS	= 64
LC_SPECVER	= 04h

EXTRN	_tempname2	:dword
EXTRN	_Exec_Type	:dword
EXTRN	_quiet		:dword
EXTRN	_smart		:dword
EXTRN	_strip		:dword
EXTRN	_encode		:dword
EXTRN	_nocount	:dword
EXTRN	CompressData_	:near

PUBLIC	_exec_handle
PUBLIC	_temp_handle
PUBLIC	_app_num_objects
PUBLIC	_app_enc_status
PUBLIC	_app_obj_iscode
PUBLIC	_app_obj_is32bit

PUBLIC	open_exec_, close_exec_
PUBLIC	create_temp_, close_temp_
PUBLIC	write_oem_to_temp_
PUBLIC	get_exec_type_

PUBLIC	_obj_old_size, _obj_new_size

PUBLIC	create_lc_header_
PUBLIC	create_lc_object_
PUBLIC	create_lc_fixups_


.CODE
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ


;-----------------------------------------------------------------------------
; LC Header Format
;
;	0000	DD	"LC" \0\0
;	0004	DB	# of Objects
;	0005	DB	reserved, 0
;	0006	DB	EIP Object #
;	0007	DB	ESP Object #
;	0008	DD	EIP Offset
;	000C	DD	ESP Offset
;

create_lc_header_:
	pushad
	call	seek_to_start			; move to exec start
	jc	@@err1
	mov	ecx,0A8h
	mov	edx,_buffer
	call	read_from_exec			; load LE/LX header
	jc	@@err2

	mov	esi,_buffer			; source LE/LX header
	lea	edi,[esi+0100h]			; destination LC header

	mov	eax,'CL'
	mov	[edi+0000h],eax			; LC signature
	mov	eax,[esi+0044h]
	mov	[edi+0004h],al			; # of Objects
	mov	al,LC_SPECVER
	mov	[edi+0005h],al			; LC implement.
	mov	eax,[esi+0018h]
	mov	[edi+0006h],al			; EIP Object #
	mov	eax,[esi+0020h]
	mov	[edi+0007h],al			; ESP Object #
	mov	eax,[esi+001Ch]
	mov	[edi+0008h],eax			; EIP Offset
	mov	eax,[esi+0024h]
	mov	[edi+000Ch],eax			; ESP Offset

	mov	eax,[esi+0010h]			; check Module Flags
	and	eax,2000h
	jnz	@@err4
	mov	eax,[esi+0044h]			; check # of Objects
	cmp	eax,APP_MAXOBJECTS
	ja	@@err5
	mov	_app_num_objects,eax
	mov	eax,[esi+0040h]
	mov	_app_off_objects,eax
	mov	eax,[esi+0048h]
	mov	_app_off_objpagetab,eax
	mov	eax,[esi+0068h]
	mov	_app_off_fixpagetab,eax
	mov	eax,[esi+006Ch]
	mov	_app_off_fixrectab,eax
	mov	eax,[esi+0080h]
	mov	_app_off_datapages,eax
	mov	eax,[esi+0030h]
	mov	_app_siz_fixrectab,eax
	mov	eax,[esi+002Ch]
	mov	_app_siz_lastpage,eax

	mov	eax,0FFFh
	cmp	_Exec_Type,1
	jz	@@1
	mov	eax,1
	mov	ecx,[esi+002Ch]		; get Page Offset Shift for LX-type
	shl	eax,cl			; max shift is 15 (8000h-1)
	dec	eax
@@1:	mov	_app_off_pageshift,eax

	mov	ecx,16
	mov	edx,edi
	call	write_to_temp			; write LC header to temp file
	jc	@@err3
	popad
	xor	eax,eax
	ret

@@err1:	popad					; error seeking in exec
	mov	eax,-1
	ret
@@err2:	popad					; error loading from exec
	mov	eax,-2
	ret
@@err3:	popad					; error writing to temp
	mov	eax,-3
	ret
@@err4:	popad					; error in application exec
	mov	eax,-4
	ret
@@err5:	popad					; too many objects in exec
	mov	eax,-5
	ret




;-----------------------------------------------------------------------------
; Object Header Format
;
;	0000	DD	Virtual Size (Uncompressed Size)
;			  bit 31: 0=encoded, 1=not encoded
;	0004	DD	Compressed Size
;	0008	DW	Object Flags
;	000A	DW	Extended Object Flags
;	000C	DW	Page Table Index
;	000E	DW	# of Page Table Entries
;	0010	...	...Object Data...
;

create_lc_object_:
	pushad
	mov	_tmp_cur_object,eax
	mov	_app_enc_status,1
	mov	_app_obj_iscode,0
	mov	_app_obj_is32bit,0

	call	print_reading

	mov	edx,_app_off_objects		; move to current Object head
	call	seek_to_edx
	jc	@@err1

	mov	ecx,18h				; load Object header
	mov	edx,_buffer
	call	read_from_exec
	jc	@@err2
	add	_app_off_objects,ecx

	mov	edx,_app_off_datapages
	call	seek_to_edx
	jc	@@err1

	mov	esi,_buffer
	mov	eax,[esi+0000h]		; Virtual Size
	mov	ebx,[esi+0010h]		; # of Page Table Entries
	mov	_app_obj_virtsize,eax

	call	alloc_exec_buffer
	jc	@@err3
	call	clear_exec_buffer

	mov	ecx,eax
	mov	eax,ebx
	xor	ebp,ebp
	test	eax,eax			; check if # Page Table Entries = 0
	jz	@@5			; if yes, skip loading
	shl	eax,12			; convert # Page Table Entries to bytes
	cmp	eax,ecx			; check if # bytes >= bytes to load
	jae	@@1			; if yes, jump
	mov	ecx,eax			; else adjust number of bytes to read

@@1:	mov	eax,_tmp_cur_object	; get Object #
	cmp	eax,_app_num_objects
	jnz	@@3
	cmp	_Exec_Type,1
	jnz	@@2
	lea	ecx,[ebx-1]		; load LE-style Last Object (BSS)
	shl	ecx,12
	add	ecx,_app_siz_lastpage
	jmp	@@3
@@2:	mov	ecx,ebx			; load LX-style Last Object (BSS)
	shl	ecx,12
	cmp	ecx,_app_obj_virtsize
	jb	@@3
	mov	ecx,_app_obj_virtsize
@@3:	mov	ebp,ecx			; EBP = bytes to load (phys. size)
	mov	edx,_exec_buffer
	call	read_from_exec		; load object data
	jc	@@err2

	mov	eax,ecx
	mov	edx,_app_off_pageshift
	test	eax,edx
	jz	@@4
	mov	ecx,edx
	not	edx
	and	eax,edx
	lea	eax,[eax+ecx+1]
@@4:	add	_app_off_datapages,eax

@@5:	mov	eax,ebp				; EAX = size
	mov	_obj_old_size,eax
	shl	eax,1				; double that
	add	eax,10h				; + Object header size
	call	alloc_temp_buffer
	jc	@@err3
	call	clear_temp_buffer

	mov	eax,ebp
	test	eax,eax				; if null-size Object
	jz	@@6				; then skip compressing it
	call	find_last_byte
	mov	ebp,eax

	call	print_nothing

	mov	eax,_exec_buffer		; EAX = source
	mov	edx,_temp_buffer		; EDX = destination
	add	edx,10h				; skip header size
	mov	ebx,ebp				; EBX = size
	call	compress_data			; returns: EAX = new size

@@6:	call	print_writing

	mov	ebp,eax				; EBP = compressed size
	mov	_obj_new_size,eax
	mov	esi,_buffer
	mov	edi,_temp_buffer

	mov	eax,[esi+0000h]			; Virtual Size
	cmp	_app_enc_status,0
	jz	@@l1
	or	eax,80000000h
@@l1:	mov	[edi+0000h],eax
	mov	eax,ebp				; Compressed Size
	mov	[edi+0004h],eax
	mov	eax,[esi+0008h]			; Object Flags
	mov	[edi+0008h],ax
	mov	eax,0				; Extended Flags
	mov	[edi+000Ah],ax
	mov	eax,[esi+000Ch]			; Page Table Index
	test	eax,0FFFF0000h
	jnz	@@err5
	mov	[edi+000Ch],ax
	mov	eax,[esi+0010h]			; # of Page Table Entries
	test	eax,0FFFF0000h
	jnz	@@err5
	mov	[edi+000Eh],ax

	lea	ecx,[ebp+10h]
	mov	edx,_temp_buffer
	call	write_to_temp
	jc	@@err4

	mov	eax,[esi+0008h]			; Object Flags
	test	eax,0004h
	setnz	_app_obj_iscode			; 1=code, 0=data
	test	eax,2000h
	setnz	_app_obj_is32bit		; 1=32bit, 0=16bit

	call	free_temp_buffer
	call	free_exec_buffer

	call	print_nothing

	popad
	xor	eax,eax
	ret
@@err1:	popad					; error seeking in exec
	mov	eax,-1
	ret
@@err2:	popad					; error reading from exec
	mov	eax,-2
	ret
@@err3:	popad					; error allocating memory
	mov	eax,-3
	ret
@@err4:	popad					; error writing to temp
	mov	eax,-4
	ret
@@err5:	popad					; object header overflow
	mov	eax,-5
	ret






;-----------------------------------------------------------------------------
;
; Fixups Header Format
;
;	0000	DD	Uncompresed Size
;			  bit 31: 0=encoded, 1=not encoded
;	0004	DD	Compressed Size
;	0008	DD	Fixup Record Table Offset
;	000C	...	...Fixup Page Table Data ...
;	000C+[0008]	...Fixup Record Table Data ...

create_lc_fixups_:
	pushad
	mov	_app_enc_status,1

	call	print_reading

	mov	eax,_app_siz_fixrectab
	call	alloc_exec_buffer
	jc	@@err3
	call	clear_exec_buffer

	mov	edx,_app_off_fixpagetab
	call	seek_to_edx
	jc	@@err1
	mov	ecx,_app_siz_fixrectab
	mov	edx,_exec_buffer
	call	read_from_exec
	jc	@@err2

	mov	eax,_app_siz_fixrectab
	mov	_obj_old_size,eax
	shl	eax,1
	add	eax,0Ch
	call	alloc_temp_buffer
	jc	@@err3
	call	clear_temp_buffer

	mov	eax,_app_siz_fixrectab
	call	find_last_byte
	mov	ebp,eax

	call	print_nothing

	mov	eax,_exec_buffer		; EAX = source
	mov	edx,_temp_buffer		; EDX = destination
	add	edx,0Ch				; skip header size
	mov	ebx,ebp				; EBX = size
	call	compress_data
	mov	ebp,eax
	mov	_obj_new_size,eax

	call	print_writing

	mov	edi,_temp_buffer

	mov	eax,_app_siz_fixrectab
	cmp	_app_enc_status,0
	jz	@@l1
	or	eax,80000000h
@@l1:	mov	[edi+0000h],eax
	mov	[edi+0004h],ebp
	mov	eax,_app_off_fixrectab
	mov	ebx,_app_off_fixpagetab
	sub	eax,ebx
	mov	[edi+0008h],eax

	lea	ecx,[ebp+0Ch]
	mov	edx,_temp_buffer
	call	write_to_temp
	jc	@@err4

	call	free_temp_buffer
	call	free_exec_buffer

	call	print_nothing

	popad
	xor	eax,eax
	ret

@@err1:	popad					; error seeking in exec
	mov	eax,-1
	ret
@@err2:	popad					; error reading from exec
	mov	eax,-2
	ret
@@err3:	popad					; error allocating memory
	mov	eax,-3
	ret
@@err4:	popad					; error writing to temp
	mov	eax,-4
	ret




;-----------------------------------------------------------------------------
compress_data:
	mov	_app_enc_status,0
	cmp	_encode,TRUE			; if NOT encode, copy data
	jnz	@@2
	cmp	_smart,TRUE			; if NOT smart, compress data
	jnz	CompressData_
	test	ebx,ebx
	jnz	@@0
	xor	eax,eax
	mov	_app_enc_status,1
	ret

@@0:	push	eax edx ebx
	call	CompressData_
	pop	ebx edx
	cmp	eax,ebx
	ja	@@1
	add	esp,4
	ret

@@1:	pop	eax
@@2:	pushad
	mov	esi,eax
	mov	edi,edx
	mov	ecx,ebx
	shr	ecx,2
	rep	movsd
	mov	ecx,ebx
	and	ecx,03h
	rep	movsb
	popad
	mov	eax,ebx
	mov	_app_enc_status,1
	ret


comment ~
;-----------------------------------------------------------------------------
; Find the most common byte in the _exec_buffer (very slow)
; In:	EAX = size of buffer
; Out:	EAX = byte
;
find_common_byte:
	pushad
	mov	ebp,eax
	sub	esp,0100h*4
	xor	eax,eax
	mov	edi,esp
	mov	ecx,0100h
	rep	stosd
	xor	eax,eax
	mov	ebx,0100h
	Align 4
@@1:	mov	ecx,ebp
	mov	edi,_exec_buffer
	Align 4
@@2:	scasb
	jnz	@@3
	inc	dword ptr [esp+eax*4]
	Align 4
@@3:	dec	ecx
	jnz	@@2
	inc	eax
	dec	ebx
	jnz	@@1
	xor	eax,eax
	xor	ebx,ebx
	xor	edx,edx
@@4:	cmp	eax,[esp+ebx*4]
	jae	@@5
	mov	eax,[esp+ebx*4]
	mov	edx,ebx
@@5:	inc	ebx
	cmp	ebx,0100h
	jb	@@4
	add	esp,0100h*4
	mov	[esp+1Ch],edx
	popad
	ret
~


;-----------------------------------------------------------------------------
; Find the last *non-zero* byte in the _exec_buffer
; In:	EAX = real size of the buffer
; Out:	EAX = virtual size of the buffer
;
find_last_byte:
	pushad
	cmp	_strip,TRUE
	jnz	@@done

	mov	ecx,eax
	jecxz	@@done
	xor	eax,eax
	mov	edi,_exec_buffer
	lea	edi,[edi+ecx-1]
	std
	repe	scasb
	cld
	cmp	al,[edi+1]
	jz	@@1
	inc	ecx
@@1:	mov	[esp+1Ch],ecx
@@done:	popad
	ret



;-----------------------------------------------------------------------------
open_exec_:
	pushad
	mov	edx,eax
	mov	ax,3D02h
	int	21h
	jc	@@0
	mov	_exec_handle,ax
@@0:	popad
	mov	eax,0
	sbb	eax,eax
	ret

close_exec_:
	pushad
	mov	bx,_exec_handle
	cmp	bx,-1
	jz	@@0
	mov	ah,3Eh
	int	21h
	mov	_exec_handle,-1
@@0:	popad
	ret

create_temp_:
	pushad
	mov	edx,_tempname2			; create temp2 file
	mov	ecx,00h
	mov	ah,3Ch
	int	21h
	jc	@@0
	mov	_temp_handle,ax
@@0:	popad
	mov	eax,0
	sbb	eax,eax
	ret

close_temp_:
	pushad
	mov	bx,_temp_handle
	cmp	bx,-1
	jz	@@0
	mov	ah,3Eh
	int	21h
	mov	_temp_handle,-1
@@0:	popad
	ret


;-----------------------------------------------------------------------------
get_exec_type_:
	pushad
	call	seek_to_start
	mov	ax,3F00h
	mov	bx,_exec_handle
	mov	ecx,4
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


;-----------------------------------------------------------------------------
write_oem_to_temp_:
	pushad
	mov	ecx,ebx
	mov	ebx,eax
	mov	ah,40h
	mov	bx,_temp_handle
	int	21h
	jc	@@err

	xor	ecx,ecx
	xor	edx,edx
	mov	ax,4200h
	mov	bx,_temp_handle
	int	21h
	jc	@@err

	mov	ah,3Fh
	mov	bx,_temp_handle
	mov	ecx,10h
	mov	edx,_buffer
	int	21h
	jc	@@err

	xor	ecx,ecx
	xor	edx,edx
	mov	ax,4200h
	mov	bx,_temp_handle
	int	21h
	jc	@@err

	mov	edx,_buffer
	mov	al,[edx+05h]
	or	al,80h
	mov	[edx+05h],al

	mov	ah,40h
	mov	bx,_temp_handle
	mov	ecx,10h
	mov	edx,_buffer
	int	21h
	jc	@@err

	popad
	xor	eax,eax
	ret

@@err:	popad
	mov	eax,-1
	ret

;-----------------------------------------------------------------------------
seek_to_start:
	pushad
	xor	ecx,ecx
	xor	edx,edx
	mov	bx,_exec_handle
	mov	ax,4200h
	int	21h
	popad
	ret
seek_to_edx:
	pushad
	mov	ecx,edx
	shr	ecx,16
	mov	bx,_exec_handle
	mov	ax,4200h
	int	21h
	popad
	ret
read_from_exec:
	pushad
	mov	ah,3Fh
	mov	bx,_exec_handle
	int	21h
	popad
	ret
write_to_temp:
	pushad
	mov	ah,40h
	mov	bx,_temp_handle
	int	21h
	popad
	ret
alloc_exec_buffer:
	pushad
	lea	ebx,[eax+10h]
	mov	ax,0FF91h
	int	21h
	mov	_exec_buffer,ebx
	mov	_exec_bufhand,esi
	popad
	ret
free_exec_buffer:
	pushad
	mov	esi,_exec_bufhand
	mov	ax,0FF92h
	int	21h
	popad
	ret
alloc_temp_buffer:
	pushad
	lea	ebx,[eax+10h]
	mov	ax,0FF91h
	int	21h
	mov	_temp_buffer,ebx
	mov	_temp_bufhand,esi
	popad
	ret
free_temp_buffer:
	pushad
	mov	esi,_temp_bufhand
	mov	ax,0FF92h
	int	21h
	popad
	ret
clear_exec_buffer:
	pushad
	mov	ecx,eax
	mov	edx,eax
	shr	ecx,2
	and	edx,3
	xor	eax,eax
	mov	edi,_exec_buffer
	rep	stosd
	mov	ecx,edx
	rep	stosb
	popad
	ret
clear_temp_buffer:
	pushad
	mov	ecx,eax
	mov	edx,eax
	shr	ecx,2
	and	edx,3
	xor	eax,eax
	mov	edi,_temp_buffer
	rep	stosd
	mov	ecx,edx
	rep	stosb
	popad
	ret


print_reading:
	pushad
	cmp	_quiet,0
	jnz	@@0
	cmp	_nocount,TRUE
	jz	@@0
	mov	ax,0FF80h
	mov	edx,offset _str_reading
	int	21h
@@0:	popad
	ret
print_writing:
	pushad
	cmp	_quiet,0
	jnz	@@0
	cmp	_nocount,TRUE
	jz	@@0
	mov	ax,0FF80h
	mov	edx,offset _str_writing
	int	21h
@@0:	popad
	ret
print_nothing:
	pushad
	cmp	_quiet,0
	jnz	@@0
	cmp	_nocount,TRUE
	jz	@@0
	mov	ax,0FF80h
	mov	edx,offset _str_nothing
	int	21h
@@0:	popad
	ret



.DATA
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
_buffer			dd offset @buffer
_exec_handle		dw -1
_temp_handle		dw -1

_str_reading		db 'Reading...',8,8,8,8,8,8,8,8,8,8,0
_str_writing		db 'Writing...',8,8,8,8,8,8,8,8,8,8,0
_str_nothing		db '          ',8,8,8,8,8,8,8,8,8,8,0



.DATA?
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
@buffer		db 1000h dup(?)

_exec_type		dd ?
_exec_name		dd ?

_exec_buffer		dd ?
_exec_bufhand		dd ?
_temp_buffer		dd ?
_temp_bufhand		dd ?

__saved_esp		dd ?
_tmp_cur_object		dd ?
_tmp_cnt_bytes		dd ?
_obj_old_size		dd ?
_obj_new_size		dd ?

;-----------------------------------------------------------------------------
_app_num_objects	dd ?
_app_off_objects	dd ?
_app_off_objpagetab	dd ?
_app_off_fixpagetab	dd ?
_app_off_fixrectab	dd ?
_app_off_datapages	dd ?
_app_siz_fixrectab	dd ?
_app_siz_lastpage	dd ?
_app_off_pageshift	dd ?
_app_obj_virtsize	dd ?

_app_enc_status		db ?			; 0=encoded, 1=not encoded
_app_obj_iscode		db ?
_app_obj_is32bit	db ?

end
