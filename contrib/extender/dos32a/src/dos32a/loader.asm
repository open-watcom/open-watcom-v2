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
; DOS/32A 32-bit application loader (LE/LX style file formats)
;
;*****************************************************************************

PushState

APP_MAXOBJECTS	= 64


.386p
;=============================================================================
load_le_app:
	mov	_app_type,0
	jmp	load_application

load_lx_app:
	mov	_app_type,1
	jmp	load_application

load_lc_app:
	mov	_app_type,2


;=============================================================================
load_application:
	call	load_header		; load 'LE'/'LX' exec header
	call	verbose_showloadhdr
	mov	ecx,1			; start with Object #1
@@1:	call	load_object		; load object
	call	create_selector		; allocate selector for loaded object
	call	verbose_showloadobj
	push	edx			; save Object Selector/Object Flags
	push	edi			; save Address of loaded Object
	push	esi			; save Page Table Index
	push	ebx			; save # Page Table Entries
	inc	cx			; increment Current_Object#
	cmp	cx,word ptr _app_num_objects
	jbe	@@1			; loop until all objects are loaded
	call	preload_fixups		; preload fixup tables and records
	mov	ebp,esp			; base pointer to last loaded Object
	mov	ebx,_app_num_objects	; number of Objects
	dec	bx
	shl	bx,4
	mov	_app_tmp_addr1,ebx
@@4:	call	relocate_object
	sub	bx,10h
	jnc	@@4
	call	unload_fixups		; free allocated memory for fixups
	call	close_exec		; close file
	mov	esp,_sel_esp
	call	verbose_showstartup
	jmp	enter_32bit_code



;-----------------------------------------------------------------------------
load_header:
	mov	ecx,0A8h		; load 'LE' header
	mov	edx,04h
	mov	_err_code,3002h		; "error in app file"
	cmp	_app_type,2
	jz	load_lc_header

	call	load_fs_block
	mov	edx,_exec_start

	mov	ax,fs:[0010h]		; get Module Flags
	and	ax,2000h		; check if not-loadable;/no-fixups
	mov	ax,3005h
	jnz	file_error
	mov	ax,fs:[0044h]		; get # Objects
	mov	cx,ax
	cmp	ax,APP_MAXOBJECTS
	mov	ax,4001h		; "too many objects"
	ja	file_error
	mov	_app_num_objects,ecx

	mov	eax,fs:[0040h]		; get Object Table Offset
	add	eax,edx
	mov	_app_off_objects,eax
	mov	eax,fs:[0048h]		; get Object PageTable Offset
	add	eax,edx
	mov	_app_off_objpagetab,eax
	mov	eax,fs:[0068h]		; get Fixup PageTable Offset
	add	eax,edx
	mov	_app_off_fixpagetab,eax
	mov	eax,fs:[006Ch]		; get Fixup Record Table Offset
	add	eax,edx
	mov	_app_off_fixrectab,eax
	mov	eax,fs:[0080h]		; get Data Pages Offset
	add	_app_off_datapages,eax

	mov	eax,fs:[0018h]		; get EIP Object #
	mov	_app_eip_object,eax
	mov	eax,fs:[0020h]		; get ESP Object #
	mov	_app_esp_object,eax
	mov	eax,fs:[001Ch]		; get EIP
	mov	_app_eip,eax
	mov	eax,fs:[0024h]		; get ESP
	mov	_app_esp,eax
	mov	eax,fs:[0030h]		; get Fixup Records Size
	mov	_app_siz_fixrecstab,eax
	mov	eax,fs:[002Ch]		; get Bytes on Last Page (LE-only)
	mov	_app_siz_lastpage,eax

	mov	eax,0FFFh
	cmp	_app_type,0
	jz	@@done
	mov	ax,1
	mov	cx,fs:[002Ch]		; get Page Offset Shift for LX-type
	shl	ax,cl			; max shift is 15 (8000h-1)
	dec	ax

@@done:	mov	_app_off_pageshift,eax
	ret



;-----------------------------------------------------------------------------
load_object:
	push	ecx
	cmp	_app_type,2
	jz	load_lc_object

	mov	_err_code,3002h		; "error in app file"
	mov	edx,_app_off_objects
	call	seek_from_start		; move to object header
	mov	ecx,18h
	xor	edx,edx
	call	load_fs_block		; load object header
	add	_app_off_objects,eax

	mov	edx,_app_off_datapages	; get Data_Pages_Offset
	call	seek_from_start		; move to object data
	mov	eax,fs:[0000h]		; get Virtual_Size[Object]
	mov	ebx,fs:[0010h]		; get # Page Table Entries
	mov	ecx,fs:[0008h]		; get Flags[Object]
	mov	esi,fs:[000Ch]		; get Page Table Index

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

@@1:	mov	ax,[esp+4]		; get Object #
	cmp	ax,word ptr _app_num_objects
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
	call	load_gs_block		; load object data
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
	xor	edx,edx
	mov	_err_code,3002h		; "error in app file"
	call	load_fs_block		; load block
	xor	ecx,ecx			; get index into FixupPageTab
	mov	ch,fs:[0001h]
	mov	cl,fs:[0002h]
	jcxz	@@2
	mov	eax,_app_off_fixpagetab	; get Fixup Page Table Offset
	lea	eax,[ecx*4+eax-4]
	mov	esi,gs:[eax+00h]	; get offset of 1st fixup table
	mov	ecx,gs:[eax+04h]	; get offset of 2nd fixup table
	sub	ecx,esi			; calculate size of 1st tab
	jz	@@2			; if 1st == 2nd, no fixups
	add	esi,_app_off_fixrectab	; get Fixup Record Table Offset
;	sub	esi,_seg_ds_base
	mov	edi,[esp]		; get current page number
	shl	edi,12
	add	edi,[ebp+ebx+8]		; address of page target to fix in mem
	add	ecx,esi
	call	apply_fixups		; patch target with fixup data
@@2:	pop	eax
	inc	ax
	cmp	ax,word ptr [ebp+ebx+0]
	jb	@@1
	ret

relocate_lx_object:
	mov	ecx,[ebp+ebx+4]		; get Page Table Index
	mov	edx,_app_off_fixpagetab	; get Fixup Page Table Offset
	lea	edx,[ecx*4+edx-4]
@@1:	push	eax edx			; EAX = counter
	mov	esi,gs:[edx+00h]	; get offset of 1st fixup table
	mov	ecx,gs:[edx+04h]	; get offset of 2nd fixup table
	sub	ecx,esi			; calculate size of 1st tab
	jz	@@2			; if 1st == 2nd, no fixups
	add	esi,_app_off_fixrectab	; get Fixup Record Table Offset
;	sub	esi,_seg_ds_base
	mov	edi,[esp+4]		; get current page number
	shl	edi,12
	add	edi,[ebp+ebx+8]		; address of page target to fix in mem
	add	ecx,esi
	call	apply_fixups		; patch target with fixup data
@@2:	pop	edx eax
	add	edx,4
	inc	ax
	cmp	ax,word ptr [ebp+ebx+0]
	jb	@@1
	ret



;=============================================================================
apply_fixups:
@@0:	push	ecx edi
	mov	_err_code,4005h		; "unrecognized fixup data"
	mov	cx,gs:[esi+0]		; get SRC/FLAGS
	movsx	edx,word ptr gs:[esi+2]	; get SRCOFF
	movzx	eax,word ptr gs:[esi+4]	; get OBJNUM
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
	mov	eax,gs:[esi]
	test	cx,1000h		; check for Alias flag
	jnz	@@2			; if not, jump
	movzx	eax,ax
	sub	esi,2
@@2:	add	esi,4
@@3:	cmp	cl,07h
	jnz	@@4
	add	eax,edx
	mov	gs:[edi+0],eax
@@5:	pop	edi ecx
	cmp	esi,ecx
	jb	@@0
	ret
@@4:	push	si
	mov	si,cx
	and	si,0Fh
	add	si,si
	mov	_err_code,4006h		; "16bit fixup overflow"
	call	fix_tab[si]
	pop	si
	jmp	@@5

;
; EAX = Data
; EDX = Address of Object
; EDI = Address to Fixup
; EBP:EBX = Ptr to Current Object Table
;-----------------------------------------------------------------------------
fix_byte:
	mov	gs:[edi+0],al
	ret
fix_16off:
	mov	gs:[edi+0],ax
	ret
fix_32off:
	add	eax,edx
	mov	gs:[edi+0],eax
	ret
fix_32selfrel:
	add	eax,edx
	lea	ecx,[edi+4]
	sub	eax,ecx
	test	word ptr [ebp+ebx+12],2000h
	jnz	@@1
	lea	ecx,[eax+8002h]
	shr	ecx,16
	jnz	file_errorm
	mov	gs:[edi+0],ax
	ret
@@1:	mov	gs:[edi+0],eax
	ret
fix_16sel:
	call	check_range
	mov	gs:[edi+0],dx
	ret
fix_1616ptr:
	call	check_range
	mov	gs:[edi+0],ax
	mov	gs:[edi+2],dx
	ret
fix_1632ptr:
	add	eax,edx
	mov	gs:[edi+0],eax
	call	check_range
	mov	gs:[edi+4],dx
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


	evendata
fix_tab	label word
	dw	fix_byte		; 00h
	dw	fix_invalid		; 01h
	dw	fix_16sel		; 02h
	dw	fix_1616ptr		; 03h
	dw	fix_invalid		; 04h
	dw	fix_16off		; 05h
	dw	fix_1632ptr		; 06h
	dw	fix_32off		; 07h
	dw	fix_32selfrel		; 08h








;-----------------------------------------------------------------------------
; In:	ECX = size
; Out:	EDI = address
;
fill_zero_pages:
	push	es dx eax ecx edi
	push	gs
	pop	es
	mov	dl,cl
	shr	ecx,2
	xor	eax,eax
	rep	stos dword ptr es:[edi]
	mov	cl,dl
	and	cl,3
	rep	stos byte ptr es:[edi]
	pop	edi ecx eax dx es
	ret


;-----------------------------------------------------------------------------
; In:	EAX = size
; Out:	EDI = address
;
alloc_block:
	push	dx
	test	eax,eax			; if size of Object is zero
	jz	@@null			; then report a warning 9005
	mov	dl,_misc_byte		; get misc byte
	shr	dx,4			; get memory alloc bits in bit1,0
	and	dx,3			; mask them
	jz	@@00			; if 00b alloc scheme, jump
	dec	dx
	jz	@@01
	dec	dx
	jz	@@10
	dec	dx
	jz	@@11
@@done:	pop	dx
	ret
@@null:	push	ax si
	mov	si,[esp+0Ch]
	mov	ax,9005h
	call	report_error
	pop	si ax dx
	xor	edi,edi
	ret

;
; load 16bit/32bit -> low, then high, then error
;---------------------------------------
@@00:	call	alloc_dos_mem		; try to allocate DOS memory block
	jnc	@@done			; if allocated, jump
	mov	_err_code,4003h		; "not enough DPMI mem"
	call	alloc_dpmi_mem		; try to allocate DPMI memory block
	jnc	@@done			; if allocated, jump
	jmp	file_errorm		; if failed, error
;
; load 16bit -> low, then high, then error
; load 32bit -> high only, then error
;---------------------------------------
@@01:	test	cx,2000h		; check if 32bit Object
	jnz	@@01_1			; if yes, jump
	mov	_err_code,4002h		; "not enough DOS mem"
	call	alloc_dos_mem
	jnc	@@done			; if allocated, jump
@@01_1:	mov	_err_code,4003h		; "not enough DPMI mem"
	call	alloc_dpmi_mem
	jnc	@@done			; if allocated, jump
	jmp	file_errorm		; if failed, error
;
; load 16bit/32bit low, then error
;---------------------------------------
@@10:	mov	_err_code,4002h		; "not enough DOS mem"
	call	alloc_dos_mem
	jnc	@@done
	jmp	file_errorm		; if failed, error
;
; load 16bit/32bit high, then error
;---------------------------------------
@@11:	mov	_err_code,4003h		; "not enough DPMI mem"
	call	alloc_dpmi_mem
	jnc	@@done
	jmp	file_errorm		; if failed, error




;-----------------------------------------------------------------------------
alloc_dos_mem:
	push	eax ebp			; EAX = size to allocate
	add	eax,0Fh			; align size on para
	shr	eax,4
	test	eax,0FFFF0000h		; check high word of EAX
	stc
	jnz	@@done
	sub	esp,32h
	mov	ebp,esp
	mov	byte ptr [ebp+1Dh],48h	; DOS func: AH=48h
	mov	word ptr [ebp+10h],ax	; DOS BX=size
	call	int21h
	movzx	edi,word ptr [ebp+1Ch]	; get returned value in EAX
	shl	edi,4			; NOTE: addres is relative to 0
	bt	word ptr [ebp+20h],0	; check for errors
	lea	esp,[esp+32h]
@@done:	pop	ebp eax
	ret

;-----------------------------------------------------------------------------
alloc_dpmi_mem:
	push	esi ebx ecx edx eax
	mov	ebx,eax
	mov	ax,0FF91h		; allocate DPMI memory
	int	21h
	jc	@@done
	mov	eax,ebx
	xor	edx,edx
	test	_misc_byte2,00000100b	; check if para or page alignment
	jnz	@@l1
	test	al,0Fh
	jz	@@1
	jmp	@@l2
@@l1:	test	ax,0FFFh		; check if returned addr aligned
	jz	@@1			; on PAGE boundary, if yes, jump
@@l2:	test	_misc_byte2,00000100b
	jnz	@@l3
	add	ebx,0Fh
	and	bl,0F0h
	jmp	@@l4
@@l3:	add	ebx,0FFFh
	and	bx,0F000h		; align linear addr on PAGE boundary
@@l4:	sub	ebx,eax			; calculate difference
	mov	edx,ebx
	add	ebx,[esp]
	mov	ax,0FF93h		; resize DPMI memory block
	int	21h
	jc	@@done
@@1:	lea	edi,[ebx+edx]		; adjust linear address
	test	_misc_byte2,00000100b
	jnz	@@l5
	test	di,000Fh
	jmp	@@l6
@@l5:	test	di,0FFFh
@@l6:	stc
	jnz	@@done
	clc
@@done:	pop	eax edx ecx ebx esi
	ret



;-----------------------------------------------------------------------------
create_selector:
	push	ebx ecx edx esi edi
	mov	ax,dx
	mov	ecx,ebp			; ECX = Virtual Size[Object]
	mov	dx,_acc_rights		; default: PAGE, USE32, DATA
	test	al,0004h		; check if object is executable
	jz	@@1			; if not, jump
	or	dl,0008h		; set selector to Code
@@1:	test	ax,2000h		; check if object is 32bit
	jz	@@2			; if not, jump
	xor	edi,edi			; set base to Zero
	or	ecx,-1			; set limit to 4Gb

	test	al,0004h		; check if code or data Object
	mov	ax,_sel32_cs		; AX = 32bit code selector
	jnz	@@0			; if code, then jump
	mov	ax,_sel32_ss		; AX = 32bit data selector
@@0:	test	ax,ax			; check if already allocated
	jnz	@@4			; if yes, use this selector
	jmp	@@3

@@2:	and	dx,0BFFFh		; set selector to 16bit
@@3:	call	set_descriptor		; allocate selector
	jc	dpmi_error
@@4:	pop	edi esi
	mov	[esp+2],ax		; store selector in high word of EDX
	pop	edx ecx ebx
	mov	_app_buf_allocsel[ecx*2],ax
	mov	_app_buf_allocbase[ecx*4],edi

	cmp	cx,word ptr _app_eip_object; is Current_Object# == EIP_Object#
	jnz	@@l1			; if not, jump
	mov	_sel32_cs,ax
	mov	_unreloc_eip,edi
	test	dx,2000h		; check if Object is 32bit
	jz	@@l1			; if not, leave _APP_EIP as is
	add	_app_eip,edi

@@l1:	cmp	cx,word ptr _app_esp_object; is Current_Object# == ESP_Object#
	jnz	@@l2			; if not, jump
	mov	_sel32_ss,ax
	mov	_unreloc_esp,edi
	add	_app_esp,edi		; adjust ESP
@@l2:	ret


preload_fixups:
	cmp	_app_type,2
	jz	preload_lc_fixups

	mov	ebx,_app_siz_fixrecstab	; allocate memory for fixups
	mov	_app_load,0		; default load fixups low
	mov	ax,0FF95h
	int	21h
	jnc	@@1
	mov	_app_load,1		; try load fixups hi
	mov	al,91h
	int	21h
	mov	ax,4004h
	jc	file_error		; if not enough memory, error
@@1:	mov	_app_buf_fixrecstab,esi

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
	cmp	_app_type,2
	jz	unload_lc_fixups

	mov	esi,_app_buf_fixrecstab
	mov	ax,0FF96h
	cmp	_app_load,0
	jz	@@1
	mov	al,92h
@@1:	int	21h
	ret


PopState

