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

		.386p
		.MODEL	flat
		LOCALS
		NOJUMPS

APP_MAXOBJECTS	= 64

EXTRN	_quiet		:dword
EXTRN	_nocount	:dword

PUBLIC	relocate_exec_
PUBLIC	_relocated_fixups
PUBLIC	_app_obj_seekattr

include	stddef.inc

.CODE
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

relocate_exec_:
	pushad
	mov	__saved_esp,esp
	mov	__filename_ptr,eax
	mov	_relocated_fixups,0
	call	clear_struct
	call	open_exec
	call	load_application
;	call	relocate_application
	call	free_resources
	call	close_exec
	xor	eax,eax
	jmp	common_exit




;=============================================================================
load_application:
	mov	__loader_esp,esp
	call	load_header		; load 'LE'/'LX' exec header
	mov	ecx,1			; start with Object #1
@@1:	call	show_progress
	mov	_app_cur_object,ecx
	call	load_object		; load object
	call	create_selector		; allocate selector for loaded object
	push	edx			; save Object Selector/Object Flags
	push	edi			; save Address of loaded Object
	push	esi			; save Page Table Index
	push	ebx			; save # Page Table Entries
	inc	ecx			; increment Current_Object#
	cmp	ecx,_app_num_objects
	jbe	@@1			; loop until all objects are loaded
	call	preload_fixups		; preload fixup tables and records
	mov	ebp,esp			; base pointer to last loaded Object
	mov	ebx,_app_num_objects	; number of Objects
	dec	ebx
	shl	ebx,4
	mov	_app_tmp_addr1,ebx
	mov	__current_object,0
@@4:	inc	__current_object
	call	relocate_object
	sub	ebx,10h
	jnc	@@4
	call	unload_fixups		; free allocated memory for fixups
	mov	esp,__loader_esp
	ret


;relocate_application:
;	mov	ebp,1
;	xor	eax,eax
;	xor	ebx,ebx
;	mov	_err_code,3002h
;@@1:	cmp	_app_obj_seekattr[ebp*4],0
;	jz	@@2
;	mov	edx,_app_obj_seekptr[ebp*4]
;	call	seek_from_start
;	mov	ah,40h
;	mov	bx,__exec_handle
;	mov	ecx,_app_obj_seeksize[ebp*4]
;	jecxz	@@2
;	mov	edx,_app_buf_allocbase[ebp*4]
;	int	21h
;	jc	file_errorm
;@@2:	inc	ebp			; increment Current_Object#
;	cmp	ebp,_app_num_objects
;	jbe	@@1			; loop until all objects are loaded
;	ret




;-----------------------------------------------------------------------------
load_header:
	sub	esp,0ACh
	mov	ecx,0A8h		; load 'LE' header
	mov	ebp,esp
	lea	edx,[esp+4]
	mov	_err_code,3002h		; "error in app file"

	call	load_fs_block
	mov	edx,__exec_start

	mov	ax,[ebp+0010h]		; get Module Flags
	and	ax,2000h		; check if not-loadable;/no-fixups
	mov	ax,3005h
	jnz	file_error
	mov	eax,[ebp+0044h]		; get # Objects
	mov	ecx,eax
	cmp	eax,APP_MAXOBJECTS
	mov	ax,4001h		; "too many objects"
	ja	file_error
	mov	_app_num_objects,ecx

	mov	eax,[ebp+0040h]		; get Object Table Offset
	add	eax,edx
	mov	_app_off_objects,eax
	mov	eax,[ebp+0048h]		; get Object PageTable Offset
	add	eax,edx
	mov	_app_off_objpagetab,eax
	mov	eax,[ebp+0068h]		; get Fixup PageTable Offset
	add	eax,edx
	mov	_app_off_fixpagetab,eax
	mov	eax,[ebp+006Ch]		; get Fixup Record Table Offset
	add	eax,edx
	mov	_app_off_fixrectab,eax
	mov	eax,[ebp+0080h]		; get Data Pages Offset
	add	_app_off_datapages,eax

	mov	eax,[ebp+0030h]		; get Fixup Records Size
	mov	_app_siz_fixrecstab,eax
	mov	eax,[ebp+002Ch]		; get Bytes on Last Page (LE-only)
	mov	_app_siz_lastpage,eax

	mov	eax,0FFFh
	cmp	_app_type,0
	jz	@@done
	mov	eax,1
	mov	ecx,[ebp+002Ch]		; get Page Offset Shift for LX-type
	shl	eax,cl			; max shift is 15 (8000h-1)
	dec	eax

@@done:	mov	_app_off_pageshift,eax
	add	esp,0ACh
	ret



;-----------------------------------------------------------------------------
load_object:
	push	ecx

	mov	_err_code,3002h		; "error in app file"
	mov	edx,_app_off_objects
	call	seek_from_start		; move to object header

	sub	esp,18h
	mov	ecx,18h
	mov	edx,esp
	mov	ebp,esp
	call	load_fs_block		; load object header
	add	_app_off_objects,eax

	mov	edx,_app_off_datapages	; get Data_Pages_Offset
	mov	eax,_app_cur_object
	mov	_app_obj_seekptr[eax*4],edx
	call	seek_from_start		; move to object data
	mov	eax,[ebp+0000h]		; get Virtual_Size[Object]
	mov	ebx,[ebp+0010h]		; get # Page Table Entries
	mov	ecx,[ebp+0008h]		; get Flags[Object]
	mov	esi,[ebp+000Ch]		; get Page Table Index
	add	esp,18h

	push	ecx			; save Object Flags
	call	alloc_block		; allocate EAX memory block to EDI
	mov	ecx,eax			; ECX = bytes to read
	mov	ebp,eax			; EBP = preserve Virtual Size
	mov	edx,edi			; EDX = addres to read to
	call	fill_zero_pages		; fill allocated memory with zeroes

	mov	eax,ebx
	test	eax,eax			; check if # Page Table Entries = 0
	jz	@@5			; if yes, skip loading
	shl	eax,12			; convert # Page Table Entries to bytes
	cmp	eax,ecx			; check if # bytes >= bytes to load
	jae	@@1			; if yes, jump
	mov	ecx,eax			; else adjust number of bytes to read

@@1:	mov	eax,[esp+4]		; get Object #
	cmp	eax,_app_num_objects
	jnz	@@3
	cmp	_app_type,0
	jnz	@@2
	lea	ecx,[ebx-1]		; load LE-style Last Object (BSS)
	shl	ecx,12
	add	ecx,_app_siz_lastpage
	jmp	@@3
@@2:	mov	ecx,ebx			; load LX-style Last Object (BSS)
	shl	ecx,12

@@3:	mov	_err_code,3002h		; "error in app file"
	mov	eax,_app_cur_object
	push	eax
	call	load_gs_block		; load object data
	pop	edx
	mov	_app_obj_seeksize[edx*4],eax
	mov	eax,ecx
	mov	edx,_app_off_pageshift
	test	eax,edx
	jz	@@4
	mov	ecx,edx
	not	edx
	and	eax,edx
	lea	eax,[eax+ecx+1]
@@4:	add	_app_off_datapages,eax

@@5:	pop	edx			; restore Object Flags
@@done:	pop	ecx
	ret




;=============================================================================
relocate_object:
	xor	eax,eax
	cmp	eax,[ebp+ebx+0]		; get # Page Table Entries[Object]
	jnz	@@0			; if zero, done
	ret
@@0:	cmp	_app_type,0
	jnz	relocate_lx_object

relocate_le_object:
	mov	ecx,[ebp+ebx+4]		; get Page Table Index
	mov	edx,_app_off_objpagetab	; get Object Page Table Offset in exec
	lea	edx,[ecx*4+edx-4]
	mov	_err_code,3002h		; "error in app file"
	call	seek_from_start		; *1) move file ptr
@@1:	push	eax			; EAX = counter
	mov	ecx,4
	sub	esp,4
	mov	edx,esp
	mov	_err_code,3002h		; "error in app file"
	call	load_fs_block		; load block
	xor	ecx,ecx			; get index into FixupPageTab
	mov	ch,[esp+0001h]
	mov	cl,[esp+0002h]
	add	esp,4
	jecxz	@@2
	mov	eax,_app_off_fixpagetab	; get Fixup Page Table Offset
	lea	eax,[ecx*4+eax-4]
	mov	esi,[eax+00h]		; get offset of 1st fixup table
	mov	ecx,[eax+04h]		; get offset of 2nd fixup table
	sub	ecx,esi			; calculate size of 1st tab
	jz	@@2			; if 1st == 2nd, no fixups
	add	esi,_app_off_fixrectab	; get Fixup Record Table Offset
	mov	edi,[esp]		; get current page number
	shl	edi,12
	add	edi,[ebp+ebx+8]		; address of page target to fix in mem
	add	ecx,esi
	call	apply_fixups		; patch target with fixup data
@@2:	pop	eax
	inc	eax
	cmp	eax,[ebp+ebx+0]
	jb	@@1
	ret

relocate_lx_object:
	mov	ecx,[ebp+ebx+4]		; get Page Table Index
	mov	edx,_app_off_fixpagetab	; get Fixup Page Table Offset
	lea	edx,[ecx*4+edx-4]
@@1:	push	eax edx			; EAX = counter
	mov	esi,[edx+00h]		; get offset of 1st fixup table
	mov	ecx,[edx+04h]		; get offset of 2nd fixup table
	sub	ecx,esi			; calculate size of 1st tab
	jz	@@2			; if 1st == 2nd, no fixups
	add	esi,_app_off_fixrectab	; get Fixup Record Table Offset
	mov	edi,[esp+4]		; get current page number
	shl	edi,12
	add	edi,[ebp+ebx+8]		; address of page target to fix in mem
	add	ecx,esi
	call	apply_fixups		; patch target with fixup data
@@2:	pop	edx eax
	add	edx,4
	inc	eax
	cmp	eax,[ebp+ebx+0]
	jb	@@1
	ret



;=============================================================================
	Align 4
apply_fixups:
@@0:	call	show_progress
	inc	_relocated_fixups
	push	ecx edi
	mov	_err_code,4005h		; "unrecognized fixup data"
	mov	cx,[esi+0]		; get SRC/FLAGS
	movsx	edx,word ptr [esi+2]	; get SRCOFF
	movzx	eax,word ptr [esi+4]	; get OBJNUM
	add	edi,edx			; calculate dest addr to be fixed
	test	cx,0F20h		; SrcLists/Imports not supported
	jnz	file_errorm		; jump if one of these
	test	cx,4000h		; test if 16bit object number
	jnz	@@1			; if yes, jump
	mov	ah,0
	dec	esi
@@1:	add	esi,6
	dec	eax			; Object Number - 1
	shl	eax,4
	mov	edx,_app_tmp_addr1
	sub	edx,eax
	jc	file_errorm
	mov	_app_tmp_addr2,edx
	mov	edx,[ebp+edx+8]		; EDX = Destination Object Address
	mov	al,cl
	and	al,0Fh
	cmp	al,02h			; check if 16bit Selector
	jz	@@3			; if yes, jump
	cmp	al,08h
	ja	file_errorm
	mov	eax,[esi]
	test	cx,1000h		; check for Alias flag
	jnz	@@2			; if not, jump
	movzx	eax,ax
	sub	esi,2
@@2:	add	esi,4
@@3:	push	esi
	mov	esi,ecx
	and	esi,0Fh
	lea	esi,[esi*4]
	mov	_err_code,4006h		; "16bit fixup overflow"
	call	fix_tab[esi]
	pop	esi
@@5:	pop	edi ecx
	cmp	esi,ecx
	jb	@@0
	ret


;
; EAX = Data
; EDX = Address of Object
; EDI = Address to Fixup
; EBP:EBX = Ptr to Current Object Table
;-----------------------------------------------------------------------------
fix_byte:
	call	mark_object
	mov	eax,_dummy_fill		;** dummy fill
	mov	[edi+0],al
	call	patch_it8
	ret
fix_16off:
	call	mark_object
	mov	eax,_dummy_fill		;** dummy fill
	mov	[edi+0],ax
	call	patch_it16
	ret
fix_32off:
	call	mark_object
	add	eax,edx
	mov	eax,_dummy_fill		;** dummy fill
	mov	[edi+0],eax
	call	patch_it32
	ret
fix_32selfrel:
	call	mark_object
	add	eax,edx
	lea	ecx,[edi+4]
	sub	eax,ecx
	test	word ptr [ebp+ebx+12],2000h
	jnz	@@1
	lea	ecx,[eax+8002h]
	shr	ecx,16
	jnz	file_errorm
	mov	eax,_dummy_fill		;** dummy fill
	mov	[edi+0],ax
	call	patch_it16
	ret
@@1:	mov	eax,_dummy_fill		;** dummy fill
	mov	[edi+0],eax
	call	patch_it32
	ret
fix_16sel:
	call	mark_object
	call	check_range
	mov	edx,_dummy_fill		;** dummy fill
	mov	[edi+0],dx
	call	patch_it16
	ret
fix_1616ptr:
	call	mark_object
	call	check_range
	mov	eax,_dummy_fill		;** dummy fill
	mov	edx,_dummy_fill		;** dummy fill
	mov	[edi+0],ax
	mov	[edi+2],dx
	call	patch_it32
	ret
fix_1632ptr:
	call	mark_object
	add	eax,edx
	mov	eax,_dummy_fill		;** dummy fill
	mov	[edi+0],eax
	call	patch_it32
	call	check_range
	mov	edx,_dummy_fill		;** dummy fill
	mov	[edi+4],dx
	add	edi,4
	call	patch_it16
	ret
fix_invalid:
	mov	ax,4005h			; "unrecognized fixup data"
	jmp	file_error

check_range:
	test	word ptr [ebp+ebx+12],1000h	; check if 16:16 alias requird
	jnz	@@1				; if yes, jump
	test	cl,10h
	jnz	@@1
@@0:	mov	ecx,_app_tmp_addr2
	mov	dx,[ebp+ecx+14]			; get selector
	ret
@@1:	test	cl,10h
	jz	@@0
	mov	ecx,_app_tmp_addr2
	mov	dx,[ebp+ecx+14]			; get selector
	test	eax,0FFFF0000h			; check 64K range
	jnz	file_errorm
	ret


	Align 4
fix_tab	label dword
	dd	fix_byte		; 00h
	dd	fix_invalid		; 01h
	dd	fix_16sel		; 02h
	dd	fix_1616ptr		; 03h
	dd	fix_invalid		; 04h
	dd	fix_16off		; 05h
	dd	fix_1632ptr		; 06h
	dd	fix_32off		; 07h
	dd	fix_32selfrel		; 08h








;-----------------------------------------------------------------------------
; In:	ECX = size
; Out:	EDI = address
;
fill_zero_pages:
	pushad
	mov	dl,cl
	shr	ecx,2
	xor	eax,eax
	rep	stosd
	mov	cl,dl
	and	cl,3
	rep	stosb
	popad
	ret


;-----------------------------------------------------------------------------
; In:	EAX = size
; Out:	EDI = address
;
alloc_block:
	xor	edi,edi
	test	eax,eax			; if size of Object is zero
	jz	@@null			; then report warning 9005
	push	eax ebx esi
	mov	_err_code,4003h		; "not enough DPMI mem"
	mov	ebx,eax
	mov	ax,0FF91h		; allocate DPMI memory
	int	21h
	mov	_app_tmp_addr1,esi
	mov	edi,ebx
	pop	esi ebx eax
	jc	file_errorm		; if failed, error
@@null:	ret








;-----------------------------------------------------------------------------
create_selector:
	push	ebx edx
	xor	eax,eax
	mov	[esp+2],ax		; store selector in high word of EDX
	pop	edx
	mov	_app_buf_allocbase[ecx*4],edi
	mov	ebx,_app_tmp_addr1
	mov	_app_buf_allochandle[ecx*4],ebx
	pop	ebx
	ret


preload_fixups:
	mov	ebx,_app_siz_fixrecstab	; allocate memory for fixups
	mov	ax,0FF91h
	int	21h
	mov	ax,4004h
	jc	file_error		; if not enough memory, error
	mov	_app_buf_fixrecstab,esi
	mov	_err_code,3002h		; "error in app file"
	mov	edx,_app_off_fixpagetab	; move file ptr to fixups
	call	seek_from_start
	mov	edx,ebx
	mov	ecx,_app_siz_fixrecstab
	call	load_gs_block
	mov	eax,_app_off_fixrectab
	mov	ebx,_app_off_fixpagetab
	sub	eax,ebx
	add	eax,edx
	mov	_app_off_fixpagetab,edx
	mov	_app_off_fixrectab,eax
	ret


unload_fixups:
	mov	esi,_app_buf_fixrecstab
	mov	ax,0FF92h
	int	21h
	ret



;=============================================================================
open_exec:
	sub	esp,40h
	mov	__exec_handle,-1
	mov	_err_code,3001h		; "could not open app exec"
	mov	ax,3D02h
	mov	edx,__filename_ptr
	int	21h
	jc	file_errorm
	mov	__exec_handle,ax
	mov	ecx,40h
	mov	edx,esp
	mov	_err_code,3002h		; "error in app exec"
	call	load_fs_block
	xor	eax,eax
	mov	_err_code,3004h		; "exec format not supported"
	cmp	word ptr [edx],'EL'
	jz	@@1
	cmp	word ptr [edx],'XL'
	jnz	file_errorm
@@1:	mov	__exec_start,eax
	mov	_app_off_datapages,0
	mov	edx,eax
	mov	_err_code,3002h		; "error in app exec"
	call	seek_from_start
	mov	ecx,04h
	mov	edx,esp
	call	load_fs_block
	mov	_err_code,3004h		; "exec format not supported"
	cmp	word ptr [edx],'EL'
	mov	_app_type,0
	jz	@@2
	cmp	word ptr [edx],'XL'
	mov	_app_type,1
	jnz	file_errorm
@@2:	add	esp,40h
	ret

close_exec:
	mov	bx,__exec_handle
	cmp	bx,-1
	jz	@@1
	mov	ah,3Eh
	int	21h
	mov	__exec_handle,-1
@@1:	ret


free_resources:
	mov	ecx,_app_num_objects
	jecxz	@@2
@@1:	mov	ax,0FF92h
	mov	esi,_app_buf_allochandle[ecx*4]
	int	21h
	loop	@@1
@@2:	ret



;=============================================================================
clear_struct:
	mov	ecx,offset @app_struct_end
	mov	edi,offset @app_struct_begin
	sub	ecx,edi
	xor	eax,eax
	rep	stosb
	ret




;=============================================================================
;dpmi_error:
;	movzx	eax,ax
;	bts	eax,31
;	jmp	common_exit
file_errorm:
	mov	ax,_err_code
file_error:
	movzx	eax,ax
common_exit:
	mov	esp,__saved_esp
	mov	[esp+1Ch],eax
	call	close_exec
	popad
	ret


;=============================================================================
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
	mov	bx,__exec_handle
	mov	ax,4200h
	int	21h
	jc	file_errorm
	popad
	ret
seek_from_start@pos:
	pushad
	xor	ecx,ecx
	xor	edx,edx
	mov	bx,__exec_handle
	mov	ax,4201h
	int	21h
	jc	file_errorm
	mov	wptr __oldfilepos[0],ax
	mov	wptr __oldfilepos[2],dx
	popad
	jmp	seek_from_start
load_fs_block:
load_gs_block:
	push	ebx
	mov	bx,__exec_handle
	mov	ah,3Fh
	int	21h
	pop	ebx
	jc	file_errorm
	ret

mark_object:
	push	eax
	mov	eax,__current_object
	inc	_app_obj_seekattr[eax*4]
	pop	eax
	ret

patch_it32:
	pushad
	mov	edx,edi
	mov	eax,__current_object
	sub	edx,_app_buf_allocbase[eax*4]
	add	edx,_app_obj_seekptr[eax*4]
	call	seek_from_start@pos
	mov	ah,40h
	mov	bx,__exec_handle
	mov	ecx,4
	mov	edx,offset _dummy_fill
	int	21h
	jc	file_errorm
	mov	edx,__oldfilepos
	call	seek_from_start
	popad
	ret

patch_it16:
	pushad
	mov	edx,edi
	mov	eax,__current_object
	sub	edx,_app_buf_allocbase[eax*4]
	add	edx,_app_obj_seekptr[eax*4]
	call	seek_from_start@pos
	mov	ah,40h
	mov	bx,__exec_handle
	mov	ecx,2
	mov	edx,offset _dummy_fill
	int	21h
	jc	file_errorm
	mov	edx,__oldfilepos
	call	seek_from_start
	popad
	ret

patch_it8:
	pushad
	mov	edx,edi
	mov	eax,__current_object
	sub	edx,_app_buf_allocbase[eax*4]
	add	edx,_app_obj_seekptr[eax*4]
	call	seek_from_start@pos
	mov	ah,40h
	mov	bx,__exec_handle
	mov	ecx,1
	mov	edx,offset _dummy_fill
	int	21h
	jc	file_errorm
	mov	edx,__oldfilepos
	call	seek_from_start
	popad
	ret


show_progress:
	pushad
	cmp	_quiet,TRUE
	jz	@@done
	cmp	_nocount,TRUE
	jz	@@done
	mov	al,_str0
	inc	al
	and	al,03h
	mov	_str0,al
	jnz	@@done

	mov	ah,09h			; else show string1
	mov	edx,offset _str1
	int	21h			; Note: nested calls (RM->PM->RM)

	mov	al,_str1+1		; ajust string
	mov	ah,"\"
	cmp	al,"-"
	jz	@@1
	mov	ah,"|"
	cmp	al,"\"
	jz	@@1
	mov	ah,"/"
	cmp	al,"|"
	jz	@@1
	mov	ah,"-"
	cmp	al,"/"
	jz	@@1

@@1:	mov	_str1+1,ah
@@done:	popad
	ret

.DATA
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

_err_code		dw 0
_dummy_fill		dd 0
_relocated_fixups	dd 0

_str0			db 0
_str1			db '[-]',08h,08h,08h,'$'


.DATA?
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
@app_struct_begin	label byte
_app_type		db ?
_app_load		db ?
_app_tmp_addr1		dd ?
_app_tmp_addr2		dd ?
_app_num_objects	dd ?
_app_off_objects	dd ?

_app_off_objpagetab	dd ?
_app_off_fixpagetab	dd ?
_app_off_fixrectab	dd ?
_app_off_datapages	dd ?
_app_off_pageshift	dd ?
_app_siz_fixrecstab	dd ?
_app_siz_lastpage	dd ?
_app_buf_fixrecstab	dd ?

_app_cur_object		dd ?

_app_buf_allocbase	dd APP_MAXOBJECTS dup(?)
_app_buf_allochandle	dd APP_MAXOBJECTS dup(?)

_app_obj_seekptr	dd APP_MAXOBJECTS dup(?)
_app_obj_seeksize	dd APP_MAXOBJECTS dup(?)
_app_obj_seekattr	dd APP_MAXOBJECTS dup(?)
@app_struct_end		label byte

;-----------------------------------------------------------------------------
__filename_ptr		dd ?
__structure_ptr		dd ?

__current_object	dd ?

__oldfilepos		dd ?
__exec_handle		dw ?
__exec_start		dd ?
__saved_esp		dd ?
__saved_ss		dw ?
__loader_esp		dd ?



end

