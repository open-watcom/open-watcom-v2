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

APP_MAXOBJECTS	= 64
if SVR ne 0
include sloadlc.asm
endif

.CODE

load_le_app:
	mov	_app_type,0
	jmp	load_application

load_lx_app:
	mov	_app_type,1
	jmp	load_application

if SVR eq 0
load_lc_app:
	mov	_app_type,2
	jmp	@lerr8
endif


load_application:
	mov	__saved_esp,esp
	call	load_header		; load 'LE' exec header
	mov	ecx,1			; start with Object #1
@@1:	call	load_object		; load object
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
@@2:	call	relocate_object
	sub	ebx,10h
	jnc	@@2
	call	unload_fixups		; free allocated memory for fixups
	mov	esp,__saved_esp
	ret





;=============================================================================
load_header:
	mov	ecx,0C4h		; load 'LE' header
	mov	edx,0
	call	load_fs_block
	mov	ax,fs:[0010h]		; get Module Flags
	and	ax,2000h		; check if not-loadable/no-fixups
	jnz	@lerr7

	mov	eax,fs:[0044h]		; get # Objects
	cmp	eax,APP_MAXOBJECTS
	mov	_app_num_objects,eax
	ja	@lerr1			; "too many objects"

	mov	edx,__exec_start
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
	mov	eax,fs:[002Ch]		; get bytes on last page
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
	mov	edx,_app_off_objects
	call	seek_from_start		; move to object header
	mov	ecx,18h
	xor	edx,edx
	call	load_fs_block		; load object header
	add	_app_off_objects,ecx

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
@@3:	call	load_gs_block		; load object data
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
	cmp	_app_type,0
	jnz	relocate_lx_object

relocate_le_object:
	xor	eax,eax
	cmp	eax,[ebp+ebx+0]		; get # Page Table Entries[Object]
	jz	@@done			; if zero, done
	mov	ecx,[ebp+ebx+4]		; get Page Table Index
	mov	edx,_app_off_objpagetab	; get Object Page Table Offset in exec
	lea	edx,[ecx*4+edx-4]
	call	seek_from_start		; move file ptr

@@1:	push	eax			; EAX = counter
	mov	ecx,4
	xor	edx,edx
	call	load_fs_block		; load block

	xor	ecx,ecx			; get index into FixupPageTab
	mov	ch,fs:[0001h]
	mov	cl,fs:[0002h]
	jecxz	@@3
	mov	eax,_app_off_fixpagetab	; get Fixup Page Table Offset
	lea	eax,[ecx*4+eax-4]
	mov	esi,[eax+00h]		; get offset of 1st fixup table
	mov	ecx,[eax+04h]		; get offset of 2nd fixup table
	sub	ecx,esi			; calculate size of 1st tab
	jz	@@3			; if 1st == 2nd, no fixups
	add	esi,_app_off_fixrectab	; get Fixup Record Table Offset
	mov	edi,[esp]		; get current page number
	shl	edi,12
	add	edi,[ebp+ebx+8]		; address of page target to fix in mem
	add	ecx,esi
	call	apply_le_fixups		; patch target with fixup data
@@3:	pop	eax
	inc	eax
	cmp	eax,[ebp+ebx+0]
	jb	@@1
@@done:	ret


relocate_lx_object:
	xor	eax,eax
	cmp	eax,[ebp+ebx+0]		; get # Page Table Entries[Object]
	jz	@@done			; if zero, done
	mov	ecx,[ebp+ebx+4]		; get Page Table Index
	mov	edx,_app_off_fixpagetab	; get Fixup Page Table Offset
	lea	edx,[ecx*4+edx-4]
@@1:	push	eax edx			; EAX = counter
	mov	esi,[edx+00h]		; get offset of 1st fixup table
	mov	ecx,[edx+04h]		; get offset of 2nd fixup table
	sub	ecx,esi			; calculate size of 1st tab
	jz	@@3			; if 1st == 2nd, no fixups
	add	esi,_app_off_fixrectab	; get Fixup Record Table Offset
	mov	edi,[esp+4]		; get current page number
	shl	edi,12
	add	edi,[ebp+ebx+8]		; address of page target to fix in mem
	add	ecx,esi
	call	apply_le_fixups		; patch target with fixup data

@@3:	pop	edx eax
	add	edx,4
	inc	eax
	cmp	eax,[ebp+ebx+0]
	jb	@@1
@@done:	ret


;=============================================================================
apply_le_fixups:
@@0:	push	ecx edi
	mov	cx,[esi+0]		; get SRC/FLAGS
	movsx	edx,word ptr [esi+2]	; get SRCOFF
	movzx	eax,word ptr [esi+4]	; get OBJNUM

	add	edi,edx			; calculate dest addr to be fixed
	test	cx,0F20h		; SrcLists/Imports not supported
	jnz	@lerr4			; jump if one of these
	test	cx,4000h		; test if 16bit object number
	jnz	@@1			; if yes, jump
	mov	ah,0
	dec	esi
@@1:	add	esi,6
	dec	eax			; Object Number - 1
	shl	eax,4
	mov	edx,_app_tmp_addr1
	sub	edx,eax
	jc	@lerr4
	mov	_app_tmp_addr2,edx
	mov	edx,[ebp+edx+8]		; EDX = Destination Object Address
	mov	al,cl
	and	al,0Fh
	cmp	al,02h			; check if 16bit Selector
	jz	@@3			; if yes, jump
	cmp	al,08h
	ja	@lerr4
	mov	eax,[esi]
	test	cx,1000h		; check for Alias flag
	jnz	@@2			; if not, jump
	movzx	eax,ax
	sub	esi,2
@@2:	add	esi,4
@@3:	push	esi
	mov	esi,ecx
	and	esi,0Fh
	call	cs:fix_tab[esi*4]
	pop	esi
	pop	edi ecx
	cmp	esi,ecx
	jb	@@0
	ret
;
; EAX = Data
; EDX = Address of Object
; EDI = Address to Fixup
;
; EBP:EBX = Ptr to Current Object Table
;

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
	jnz	@lerr5
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
	jnz	@lerr5
	ret


	even
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
fix_invalid:
	jmp	@lerr4







;-----------------------------------------------------------------------------
; In:	EAX = size
; Out:	EDI = address
;
fill_zero_pages:
	push	es eax ecx edx edi
	push	gs
	pop	es
	mov	dl,cl
	shr	ecx,2
	clr	eax
	rep	stosd
	mov	cl,dl
	and	cl,3
	rep	stosb
	pop	edi edx ecx eax es
	ret



;-----------------------------------------------------------------------------
; In:	EAX = size
; Out:	EDI = address
;
alloc_block:
	push	dx
	test	eax,eax
	jz	@@done
	mov	dl,00110000b		; get misc byte
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
;
; load 16bit/32bit -> low, then high, then error
;
@@00:	call	alloc_dos_mem		; try to allocate DOS memory block
	jnc	@@done			; if allocated, jump
	call	alloc_dpmi_mem		; try to allocate DPMI memory block
	jnc	@@done			; if allocated, jump
	jmp	@lerr3			; if failed, error
;
; load 16bit -> low, then high, then error
; load 32bit -> high only, then error
;
@@01:	test	cx,2000h		; check if 32bit Object
	jnz	@@01_1			; if yes, jump
	call	alloc_dos_mem
	jnc	@@done			; if allocated, jump
@@01_1:	call	alloc_dpmi_mem
	jnc	@@done			; if allocated, jump
	jmp	@lerr3			; if failed, error
;
; load 16bit/32bit low, then error
;
@@10:	call	alloc_dos_mem
	jnc	@@done
	jmp	@lerr3			; if failed, error
;
; load 16bit/32bit high, then error
;
@@11:	call	alloc_dpmi_mem
	jnc	@@done
	jmp	@lerr3			; if failed, error




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
	btr	word ptr [ebp+20h],0	; check for errors
	lea	esp,[esp+32h]
@@done:	pop	ebp eax
	ret

;-----------------------------------------------------------------------------
alloc_dpmi_mem:
	push	esi ebx ecx edx eax
	mov	cx,ax			; convert EAX bytes to BX:CX
	mov	ebx,eax
	shr	ebx,16
	mov	ax,0501h		; allocate DPMI memory
	int	31h
	jc	@@done
	xor	edx,edx
	test	cx,0FFFh		; check if returned addr aligned
	jz	@@1			; on PAGE boundary, if yes, jump
	shl	ebx,16			; convert BX:CX addr to EAX
	mov	bx,cx
	mov	eax,ebx
	add	ebx,0FFFh
	and	bx,0F000h		; align linear addr on PAGE boundary
	sub	ebx,eax			; calculate difference
	mov	edx,ebx
	add	ebx,[esp]
	mov	cx,bx
	shr	ebx,16
	mov	ax,0503h		; resize DPMI memory block
	int	31h
	jc	@@done
@@1:	shl	ebx,16			; get new addr (should be the same)
	mov	bx,cx
	lea	edi,[ebx+edx]		; adjust linear address
	test	di,0FFFh
	stc
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
	test	ax,0004h		; check if object is executable
	jz	@@1			; if not, jump
	or	dx,0008h		; set selector to Code
@@1:	test	ax,2000h		; check if object is 32bit
	jz	@@2			; if not, jump
	xor	edi,edi			; set base to Zero
	or	ecx,-1			; set limit to 4Gb
	jmp	@@3
@@2:	and	dx,0BFFFh		; set selector to 16bit
@@3:	call	set_descriptor		; allocate selector
	jc	@lerr2
	pop	edi esi
	mov	[esp+2],ax		; store selector in high word of EDX
	mov	edx,_obj_sel_ptr
	mov	_obj_selector[edx*2],ax
	mov	_obj_address[edx*4],edi
	inc	_obj_sel_ptr
	pop	edx ecx ebx

	cmp	ecx,_app_eip_object	; is Current_Object# == EIP_Object#
	jnz	@@l1			; if not, jump
	mov	_sel32_cs,ax
	mov	_unreloc_eip,edi
	test	dx,2000h
	jz	@@l1
	add	_app_eip,edi

@@l1:	cmp	ecx,_app_esp_object	; is Current_Object# == ESP_Object#
	jnz	@@l2			; if not, jump
	mov	_sel32_ss,ax
	add	_app_esp,edi		; adjust ESP
	mov	_unreloc_esp,edi
@@l2:	ret


preload_fixups:
	mov	ebx,_app_siz_fixrecstab	; allocate memory for fixups
	mov	ax,0FF91h
	int	21h
	jc	@lerr3			; if not enough memory, error
	mov	_app_buf_fixrecstab,esi

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
@@done:	ret


unload_fixups:
	push	es
	mov	ecx,_app_siz_fixrecstab
	mov	edi,_app_off_fixpagetab
	mov	eax,0CCCCCCCCh
	mov	dl,cl
	shr	ecx,2
	push	ds
	pop	es
	rep	stosd
	mov	cl,dl
	and	cl,3
	rep	stosb
	mov	esi,_app_buf_fixrecstab
	mov	ax,0FF92h
	int	21h
	pop	es
	ret


;=============================================================================
int21h:	push	ebx			; simulate INT 21h (DOS API)
	mov	bx,21h
	push	ecx edi es
	xor	ecx,ecx
	xor	eax,eax
	mov	[ebp+20h],ax		; clear Flags
	mov	[ebp+2Eh],eax		; clear SS:SP
	push	ss
	pop	es
	mov	edi,ebp
	mov	ax,0300h
	int	31h
	pop	es edi ecx
	pop	ebx
	ret
set_descriptor:				; EDI=base, ECX=limit, DX=access
	push	ebx ecx edx ebp
	mov	ebp,ecx
	xor	ax,ax			; allocate descriptor
	mov	cx,1
	int	31h
	jc	@@err
	mov	bx,ax
	mov	ax,0009h		; set access rights
	mov	cx,dx
	int	31h
	jc	@@err
	mov	ax,0007h		; set base
	mov	ecx,edi
	mov	dx,cx
	shr	ecx,16
	int	31h
	jc	@@err
	mov	ax,0008h		; set limit
	mov	ecx,ebp
	mov	dx,cx
	shr	ecx,16
	int	31h
	jc	@@err
	mov	ax,bx
	clc
@@err:	pop	ebp edx ecx ebx
	ret
seek_from_start:
	push	ebx ecx edx eax
	mov	ecx,edx
	shr	ecx,16
	mov	bx,__exec_handle
	mov	ax,4200h
	int	21h
	pop	eax edx ecx ebx
	jc	@err4
	ret
load_fs_block:
	push	ebx ds
	mov	bx,__exec_handle
	push	fs
	pop	ds
	mov	ah,3Fh
	int	21h
	pop	ds ebx
	jc	@err4
	ret
load_gs_block:
	push	ebx
	mov	bx,__exec_handle
	mov	ah,3Fh
	int	21h
	pop	ebx
	jc	@err4
	ret



@lerr1:	mov	edx,offs __lerror1
	call	print_string
	jmp	_abort
@lerr2:	mov	edx,offs __lerror2
	call	print_string
	jmp	_abort
@lerr3:	mov	edx,offs __lerror3
	call	print_string
	jmp	_abort
@lerr4:	mov	edx,offs __lerror4
	call	print_string
	jmp	_abort
@lerr5:	mov	edx,offs __lerror5
	call	print_string
	jmp	_abort
@lerr6:	mov	edx,offs __lerror6
	call	print_string
	jmp	_abort
@lerr7:	mov	edx,offs __lerror7
	call	print_string
	jmp	_abort
@lerr8:	mov	edx,offs __lerror8
	call	print_string
	jmp	_abort





.DATA
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
__lerror1	db 'SD/32A loader: too many objects in application file exec',cre
__lerror2	db 'SD/32A loader: could not allocate a selector for loaded object',cre
__lerror3	db 'SD/32A loader: not enough memory to load application file exec',cre
__lerror4	db 'SD/32A loader: unrecognized fixup data',cre
__lerror5	db 'SD/32A loader: invalid 16bit fixup data',cre
__lerror6	db 'SD/32A loader: executable format not supported',cre
__lerror7	db 'SD/32A loader: unsupported type of executable format',cre
__lerror8	db 'SD/32A loader: cannot debug LC-style executables',cre

_app_type		db 0
_app_type2		db 0
_app_tmp_addr1		dd 0
_app_tmp_addr2		dd 0
_app_num_objects	dd 0
_app_off_objects	dd 0

_app_off_objpagetab	dd 0
_app_off_fixpagetab	dd 0
_app_off_fixrectab	dd 0
_app_off_datapages	dd 0
_app_off_pageshift	dd 0
_app_siz_fixrecstab	dd 0
_app_siz_lastpage	dd 0
_app_buf_fixrecstab	dd 0

_app_eip_object		dd 0
_app_esp_object		dd 0

_app_eip		dd 0
_sel32_cs		dw 0,0
_app_esp		dd 0
_sel32_ss		dw 0,0

_unreloc_esp		dd 0
_obj_selector		dw APP_MAXOBJECTS dup(0)
_obj_address		dd APP_MAXOBJECTS dup(0)
_obj_sel_ptr		dd 0
_acc_rights		dw 0
__saved_esp		dd 0

