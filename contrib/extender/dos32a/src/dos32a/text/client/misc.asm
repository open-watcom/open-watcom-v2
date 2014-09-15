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

PushState


.8086
;=============================================================================
; Real mode exit routine, using 8086 instructions
;
exit86:	cli
	cld
	mov	ds,cs:_seg_ds		; restore SEG regs
	mov	es,_seg_es
	mov	ss,_seg_ss
	mov	sp,STACKSIZE*16		; set stack to default size
	mov	ax,_seg_env
	mov	es:[002Ch],ax
	mov	ax,4CFFh		; exit with error code -1
	int	21h			; NOTE: DOS 1.0 will hang

.386p
;=============================================================================
; Protected mode exit routine, using 80386 instructions
;
exit386:cli
	cld
	mov	ds,cs:_sel_ds		; restore SEG regs
	mov	es,_sel_es
	lss	esp,fptr _sel_esp	; set stack to default size
	xor	dx,dx
	mov	fs,dx
	mov	gs,dx
	mov	ah,4Ch			; exit with error code in AL
	int	21h

;=============================================================================
save_inttab:				; save real mode interrupts
	push	cx si di ds es
	xor	si,si
	mov	di,STACKSIZE*16
	mov	ds,cs:_sel_zero
	mov	es,cs:_sel_ss
	mov	cx,0200h
	cld
	rep	movsw
	in	al,21h			; save PIC mask
	mov	ah,al
	in	al,0A1h
	pop	es ds di si cx
	mov	_pic_mask,ax
	ret

;=============================================================================
restore_inttab:				; restore real mode interrupts
	test	cs:_misc_byte,00000100b
	jz	@@done
	cmp	cs:_sys_type,3
	jz	@@1
	xor	eax,eax			; reset Null-Pointer protection
	mov	dr7,eax
@@1:	pushf
	cli
	push	si di ds es
	xor	di,di
	mov	si,STACKSIZE*16
	mov	es,cs:_sel_zero
	mov	ds,cs:_sel_ss
	mov	cx,0200h
	cld
	rep	movsw
	mov	ax,cs:_pic_mask		; restore PIC mask
	out	0A1h,al
	mov	al,ah
	out	21h,al
	pop	es ds di si
	popf
@@done:	ret

;=============================================================================
check_inttab:
	test	cs:_misc_byte,00001000b
	jz	@@done
	pushad
	push	ds es
	xor	bx,bx
	mov	ds,cs:_sel_ss
	mov	es,cs:_sel_zero
	mov	esi,STACKSIZE*16
	xor	edi,edi
	cld
@@1:	cmps	dptr ds:[esi],[edi]
	jnz	@@2
@@3:	inc	bx
	cmp	bx,256
	jb	@@1
	pop	es ds
	popad
@@done:	ret
@@2:	mov	ax,9003h
	push	bx si
	mov	si,bx
	call	report_error
	pop	si bx
	jmp	@@3

;=============================================================================
restore_pit:
	push	ax
	mov	al,36h			; reprogram PIT to DOS freq
	out	43h,al
	mov	al,00h
	out	40h,al
	out	40h,al
	pop	ax
	ret

;=============================================================================
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
	dec	ax			; set limit
	mov	ecx,ebp
	mov	dx,cx
	shr	ecx,16
	int	31h
	jc	@@err
	dec	ax			; set base
	mov	ecx,edi
	mov	dx,cx
	shr	ecx,16
	int	31h
	jc	@@err
	mov	ax,bx
@@err:	pop	ebp edx ecx ebx
	ret

;=============================================================================
_int23:	push	ds			; handle CTRL-C INT 23h
	mov	ds,cs:_sel_ds
	or	_sys_misc,0100h
	pop	ds
	iretd

;=============================================================================
int10h:	push	bx			; simulate INT 10h (VGA API)
	mov	bx,10h
	jmp	intxxh

int33h:	push	bx			; simulate INT 33h (Mouse API)
	mov	bx,33h
	jmp	intxxh

int21h:	push	bx			; simulate INT 21h (DOS API)
	mov	bx,21h

intxxh:	push	cx edi es
	xor	eax,eax
	mov	[ebp+20h],ax		; clear Flags
	mov	[ebp+2Eh],eax		; clear SS:SP
	xor	cx,cx
	push	ss
	pop	es
	mov	edi,ebp
	mov	ax,0300h
	int	31h
	pop	es edi cx bx
	jc	dpmi_error
	ret


;=============================================================================
setup_dta_buffer:
	sub	esp,32h
	mov	ebp,esp
	mov	ax,_seg_ss
	add	ax,0010h
	mov	_seg_dta,ax		; DTA at offset STK_TOP + 0100h
	mov	wptr [ebp+24h],ax
	add	ax,0008h
	mov	_seg_mus,ax		; MOUSE at offset STK_TOP + 0180h
	mov	ax,ss
	mov	_dta_sel,ax
	mov	_app_dta_sel,ax
	mov	eax,0100h
	mov	_dta_off,eax
	mov	_app_dta_off,eax
	mov	bptr [ebp+1Dh],1Ah
	mov	wptr [ebp+14h],0
	call	int21h			; set up new DTA buffer
	add	esp,32h
	ret

;=============================================================================
initialize_mouse:
	push	ds
	mov	ds,_sel_zero
	cmp	dptr ds:[4*33h],0	; check if mouse INT 33h is installed
	pop	ds
	jz	@@err			; if not, report warning
	mov	ax,0021h		; software reset mouse handler
	int	33h
	cmp	ax,0021h		; check if function supported
	jnz	@@1			; if yes, go on
	xor	ax,ax			; try hardware reset
	int	33h
	inc	ax			; if failed, report warning
	jnz	@@err
@@1:	mov	ax,0015h		; get mouse buffer size
	int	33h
	movzx	eax,bx
	cmp	eax,_lobufsize
	jae	@@err
	mov	_mus_size,eax
	push	ds es
	push	cs
	pop	ds
	push	ss
	pop	es
	mov	esi,offs _mus_int_pm
	mov	edi,STACKSIZE*16-40h
	mov	ax,0303h
	int	31h
	pop	es ds
	jc	dpmi_error
	mov	_mus_backoff,dx
	mov	_mus_backseg,cx
	ret
@@err:	mov	wptr _int33,0CF66h
	mov	ax,9004h
	jmp	report_error		; "mouse init failed"

;=============================================================================
install_client_ints:
	mov	ax,0205h
	mov	cx,cs

	mov	bl,10h
	mov	edx,offs _int10
	int	31h
	jc	@@err

	mov	bl,21h
	mov	dx,offs _int21
	int	31h
	jc	@@err

	mov	bl,23h
	mov	dx,offs _int23
	int	31h
	jc	@@err

	mov	bl,33h
	mov	dx,offs _int33
	int	31h
	jc	@@err


	mov	ax,0203h

	mov	bl,00h
	mov	dx,offs eh00
	int	31h
	jc	@@err

	mov	bl,01h
	mov	dx,offs eh01
	int	31h
	jc	@@err

	mov	bl,02h
	mov	dx,offs eh02
	int	31h
	jc	@@err

	mov	bl,03h
	mov	dx,offs eh03
	int	31h
	jc	@@err

	mov	bl,04h
	mov	dx,offs eh04
	int	31h
	jc	@@err

	mov	bl,05h
	mov	dx,offs eh05
	int	31h
	jc	@@err

	mov	bl,06h
	mov	dx,offs eh06
	int	31h
	jc	@@err

	mov	bl,07h
	mov	dx,offs eh07
	int	31h
	jc	@@err

	mov	bl,08h
	mov	dx,offs eh08
	int	31h
	jc	@@err

	mov	bl,09h
	mov	dx,offs eh09
	int	31h
	jc	@@err

	mov	bl,0Ah
	mov	dx,offs eh0A
	int	31h
	jc	@@err

	mov	bl,0Bh
	mov	dx,offs eh0B
	int	31h
	jc	@@err

	mov	bl,0Ch
	mov	dx,offs eh0C
	int	31h
	jc	@@err

	mov	bl,0Dh
	mov	dx,offs eh0D
	int	31h
	jc	@@err

	mov	bl,0Eh
	mov	dx,offs eh0E
	int	31h

	clc
@@err:	ret


;=============================================================================
uninstall_client_ints:
	mov	ax,0205h

	mov	bl,10h
	mov	cx, wptr cs:_int10_cs
	mov	edx,dptr cs:_int10_ip
	int	31h

	mov	bl,21h
	mov	cx, wptr cs:_int21_cs
	mov	edx,dptr cs:_int21_ip
	int	31h

	mov	bl,23h
	mov	cx, wptr cs:_int23_cs
	mov	edx,dptr cs:_int23_ip
	int	31h

	mov	bl,33h
	mov	cx, wptr cs:_int33_cs
	mov	edx,dptr cs:_int33_ip
	int	31h

	mov	ax,0203h			; restore default PM exception handlers
	xor	ebx,ebx
@@0:	mov	cx,wptr cs:_exc_tab[ebx*8+4]
	mov	edx,dptr cs:_exc_tab[ebx*8+0]
	int	31h
	inc	bl
	cmp	bl,15
	jb	@@0

	clc
@@done:	ret

;=============================================================================
install_nullptr_protect:
	cmp	_sys_type,3
	jz	@@done
	test	_misc_byte,10000000b
	jz	@@done

	xor	eax,eax				; install null-pointer protection
	mov	dr6,eax
	mov	dr0,eax
	add	al,04h
	mov	dr1,eax
	add	al,04h
	mov	dr2,eax
	add	al,04h
	mov	dr3,eax
	mov	eax,0DDDD03FFh
	mov	dr7,eax
@@done:	ret

;=============================================================================
setup_selectors:
	xor	edi,edi			; base = 0
	or	ecx,-1			; limit = 4GB
	mov	ax,cs			; get CS selector
	lar	dx,ax
	mov	dl,0C0h
	xchg	dh,dl
	and	dl,60h
	or	dl,92h
	mov	_acc_rights,dx		; set std selector access rights
	call	set_descriptor		; allocate descriptor
	jc	@@err
	mov	_sel_zero,ax
	mov	ax,0008h		; resize DS limit to 4GB
	mov	bx,ds			; this might be needed to access
	mov	cx,0FFFFh		; DOS buffer which is it self 64KB
	mov	dx,cx
	int	31h
	jc	@@err
	ret
@@err:	mov	ax,4CFFh
	int	21h





;=============================================================================
check_command_line:
	mov	di,80h
	movzx	cx,es:[di]		; get length of command line
	jcxz	@@err			; if zero, error
	inc	di			; offset to start of command line
	mov	al,20h
	repe	scasb			; look for non-space character
	jz	@@err			; if not found, error
	dec	di
	inc	cx
	mov	bx,di
@@1:	mov	al,es:[di]
	cmp	al,09h			; look for TAB character
	jz	@@2
	cmp	al,0Dh			; look for CR character
	jz	@@2
	cmp	al,20h			; look for ' ' character
	jz	@@2
	inc	di
	loop	@@1
@@2:	mov	cx,di
	mov	si,bx			; SI = pointer to file name
	mov	di,bx			; DI = pointer to file name
	sub	cx,bx			; CX = file name length
@@done:	ret
@@err:	xor	si,si
	ret

;=============================================================================
remove_name_from_cmd:
	call	check_command_line
	jz	@@done
	mov	al,20h
	rep	stosb
@@done:	ret

;=============================================================================
check_if_fullname:
	push	ds es
	call	check_command_line
	jz	@@done
	mov	al,'\'
	mov	bx,cx
	repne	scasb
	jcxz	@@done
	push	ds es
	pop	ds es
	mov	cx,bx
	mov	di,offs start+40h
	rep	movsb
	xor	al,al
	stosb
@@full:	pop	es ds
	stc
	ret
@@done:	pop	es ds
	clc
	ret

;=============================================================================
update_environment:
	push	ds es
	call	check_if_fullname		; check if full path provided
	jc	@@skip				; if yes, skip copying
	mov	ah,19h
	int	21h				; get drive letter
	mov	dl,al
	add	al,'A'
	mov	bptr [start+40h],al		; set drive
	mov	wptr [start+41h],'\:'
	inc	dx
	mov	ah,47h
	mov	esi,offs start+43h
	int	21h				; get current directory
	push	ds
	pop	es
	xor	al,al
	mov	di,si
	mov	cx,64
	repne	scasb				; get length of dir string
	cmp	bptr [di-2],'\'
	jnz	@@0
	dec	di
@@0:	mov	bptr [di-1],'\'
	mov	si,offs start
	mov	cx,64
@@1:	lodsb
	stosb
	test	al,al
	loopne	@@1
@@skip:	push	ds
	pop	es
	xor	al,al
	mov	cx,-1
	mov	di,offs start+40h
	repne	scasb
	not	cx
	mov	bx,cx
	pop	es
	push	es
	mov	ax,es:[002Ch]		; get environment selector
	test	ax,ax			; check if selector is NULL
	jz	@@err			; if yes, jump to error
	lar	cx,ax
	jnz	@@err
	mov	es,ax
	xor	al,al
	xor	di,di
	mov	cx,-1
@@2:	repne	scasb
	scasb
	jnz	@@2
	inc	di
	inc	di
	mov	cx,-1
	mov	dx,di
	repne	scasb
	not	cx
	cmp	bx,cx
	ja	create_new_environment
	mov	cx,bx
	mov	di,dx
	mov	si,offs start+40h
	rep	movsb
	pop	es ds
	ret
@@err:	mov	ax,2000h
	jmp	report_error

create_new_environment:
	push	bx
	mov	cx,-1
	xor	di,di
@@1:	repne	scasb			; get size of environment
	dec	cx
	scasb
	jnz	@@1
	not	cx
	inc	cx
	inc	cx
	push	cx
	add	bx,cx
	shr	bx,4
	inc	bx
	mov	ax,0100h
	int	31h
	pop	cx
	jc	@@err
	push	es
	pop	ds
	mov	es,dx
	xor	si,si
	xor	di,di
	rep	movsb
	pop	cx
	mov	si,offs start+40h
	push	cs
	pop	ds
	rep	movsb
	pop	es ds
	mov	es:[002Ch],dx
	ret
@@err:	mov	ax,2000h
	jmp	report_error


;=============================================================================
open_exec:
	push	ds es
	mov	ax,es:[002Ch]		; get environment selector
	test	ax,ax			; check if selector is NULL
	jz	@@err			; if yes, jump to error
	lar	cx,ax
	jnz	@@err
	mov	es,ax
	xor	ax,ax
	xor	di,di
	mov	cx,0FFFFh
@@1:	repne	scasb			; look for end of environment
	scasb
	jcxz	@@err			; reached end of environment
	jnz	@@1
	inc	di
	inc	di
	push	ds es
	pop	ds es
	push	di
	mov	si,di
	mov	di,offs start
@@2:	lodsb
	stosb
	test	al,al
	jnz	@@2
	pop	di
	movzx	edx,di			; DS:EDX = pointer to exec name
	mov	ax,3DC0h		; open file
	int	21h
	pop	es ds
	mov	_exec_handle,ax		; store file handle
	mov	ax,2001h
	jc	file_error		; if error, jump
	ret
@@err:	mov	ax,2000h
	jmp	report_error




;=============================================================================
load_exec_header:
	xor	edx,edx			; DS:EDX = ptr
	mov	ecx,64			; ECX = 64 bytes to load
	mov	_err_code,2002h		; "error in exec file"
	call	load_fs_block
	cmp	wptr fs:[0000h],'ZM'	; exec must be 'MZ' file type
	jnz	file_errorm
	mov	eax,fs:[003Ch]		; get start of 32-bit code
	mov	edx,fs:[0018h]		; get start of MZ reloc-table
	mov	_exec_start,eax
	ret




;=============================================================================
open_extrn_exec:
	push	ds es
	push	ds es			; swap DS and ES
	pop	ds es
	mov	di,offs start		; copy application exec name
	rep	movsb
	push	es
	pop	ds
	mov	bptr [di],0
	mov	edx,offs start
	mov	ax,3DC0h
	int	21h
	jc	@@1			; if error opening, try ".EXE"
	pop	es ds
	mov	_exec_handle,ax
	ret

@@1:	mov	bx,offs start
@@2:	cmp	bptr [bx],2Eh
	stc
	jz	@@err
	inc	bx
	cmp	bx,di
	jb	@@2
	mov	eax,4558452Eh
	cmp	eax,[di-4]
	stc
	jz	@@err
	mov	eax,6578652Eh
	cmp	eax,[di-4]
	stc
	jz	@@err
	mov	[di],eax
	mov	bptr [di+4],0
	mov	ax,3DC0h
	int	21h
@@err:	pop	es ds
	mov	_exec_handle,ax
	mov	ax,3001h
	jc	file_error
	ret


;=============================================================================
load_extrn_exec_header:
	xor	edx,edx			; DS:EDX = ptr
	mov	ecx,64			; ECX = 64 bytes to load
	mov	_err_code,3002h		; "error in app file"
	call	load_fs_block
	call	seek_from_start		; move to file start

	xor	ebp,ebp
	mov	_exec_start,ebp		; default start of exec offset and
	mov	_app_off_datapages,ebp	;  LE/LX data offset in file
	cmp	wptr fs:[0000h],'ZM'	; is exec 'MZ' file type
	jnz	search_for_le		; if not, search for known exec type
	mov	eax,fs:[0018h]		; MZ reloc-tab must be at offset 0040h
	cmp	ax,40h
	jnz	search_for_mz
	mov	eax,fs:[003Ch]		; if yes, get start of 32-bit code
	test	ax,ax			; check if it is bound
	jz	search_for_mz		; if not, search
	mov	_exec_start,eax
	ret


search_for_mz:
	xor	esi,esi
@@0:	movzx	eax,wptr fs:[0004h]	; get pages in file
	shl	eax,9			; *512
	movzx	ebx,wptr fs:[0002h]	; get bytes on last page
	add	eax,ebx
	mov	bx,fs:[0000h]
	cmp	bx,'ZM'
	jz	@@1
	cmp	bx,'WB'
	jz	@@2
	jmp	@@3
@@1:	sub	eax,0200h
@@2:	mov	esi,ebp
	add	ebp,eax
	mov	edx,ebp
	call	seek_from_start
	mov	ecx,64
	xor	edx,edx
	call	load_fs_block
	test	eax,eax
	jnz	@@0
	mov	ax,3003h
	jmp	file_error		; if zero, no app in file
@@3:	mov	bx,fs:[0000h]
	cmp	bx,'EL'
	jz	@@4
	cmp	bx,'XL'
	jz	@@4
	cmp	bx,'CL'
	jz	@@4
	cmp	bx,'EP'
	jz	@@4
	mov	edx,ebp
	call	seek_from_start
	call	search_for_le
@@4:	cmp	eax,esi
	jz	@@5
	mov	edx,esi
	add	eax,10h
	add	edx,10h
	and	al,0F0h
	and	dl,0F0h
	cmp	eax,edx
	jz	@@5
	mov	_exec_start,ebp
	mov	_app_off_datapages,esi
@@5:	ret

search_for_le:
@@1:	mov	edx,_lobufzero		; DS:EDX = current ptr
	mov	ecx,_lobufsize		; ECX = bytes to load
	call	load_gs_block
	test	ax,ax			; check if no bytes read
	mov	ax,3003h
	jz	file_error		; if true, no app in file
	shr	cx,1
@@2:	mov	ax,gs:[edx+0]
	mov	bx,gs:[edx+2]
	test	bx,bx
	jnz	@@4
	cmp	ax,'EL'			; 'LE' type
	jz	@@3
	cmp	ax,'XL'			; 'LX' type
	jz	@@3
	cmp	ax,'CL'			; 'LC' type
	jz	@@3
	cmp	ax,'PE'			; 'PE' type
	jz	@@3
@@4:	add	edx,2
	add	ebp,2			; increment pointer in file
	loop	@@2
	jmp	@@1
@@3:	ret





;=============================================================================
close_exec:
	mov	bx,cs:_exec_handle
	mov	ah,3Eh
	int	21h
	ret


;=============================================================================
seek_from_start:
	push	bx ecx edx eax
	mov	ecx,edx
	shr	ecx,16
	mov	bx,cs:_exec_handle
	mov	ax,4200h
	int	21h
	pop	eax edx ecx bx
	jc	file_errorm
	ret


;=============================================================================
load_fs_block:
	push	bx ds fs
	pop	ds
	jmp	@load_block
load_gs_block:
	push	bx ds gs
	pop	ds
@load_block:
	mov	bx,cs:_exec_handle
	mov	ah,3Fh
	int	21h
	pop	ds bx
	jc	file_errorm
	ret


;=============================================================================
verbose_getmem:
	pop	bp
	mov	ax,0FF90h
	int	21h
	shr	eax,10
	test	eax,0FFFF0000h
	mov	dx,offs v_msg04
	jz	@@1
	mov	dx,offs v_msg05
	shr	eax,10
@@1:	push	dx
	push	ax
	mov	ah,48h
	mov	bx,-1
	int	21h
	shr	bx,6
	push	bx
	jmp	bp

verbose_showsys:
	test	_misc_byte2,00010000b
	jz	@@done
	call	verbose_getmem
	movzx	eax,_sys_type
	lea	eax,v_msg02[eax*4+eax]
	push	ax
	movzx	ax,_cpu_type
	imul	ax,100
	add	ax,86
	push	ax
	mov	dx,offs v_msg01
	call	prints
	add	sp,10
@@done:	ret

verbose_showloadhdr:
	test	_misc_byte2,00010000b
	jz	@@done
	movzx	eax,_app_type
	lea	eax,v_msg03[eax*2+eax]
	push	ax
	push	offs start
	mov	dx,offs v_msg10
	call	prints
	add	sp,4
@@done:	ret

verbose_showloadobj:
	test	_misc_byte2,00010000b
	jz	@@done
	pushad
	mov	ax,dx
	shr	edx,16
	shl	ebx,12
	push	dx
	push	ax
	push	ebp
	push	ebx
	push	edi
	push	cx
	mov	dx,offs v_msg11
	call	prints
	add	sp,18
	popad
@@done:	ret

verbose_showstartup:
	test	_misc_byte2,00010000b
	jz	@@done
	push	wptr _seg_env
	push	wptr es:[002Ch]
	push	wptr _sel_es
	call	verbose_getmem
	mov	eax,_app_eip
	sub	eax,_unreloc_eip
	push	eax
	push	wptr _app_eip_object
	push	offs excmsgB
	push	dptr _app_esp
	push	wptr _sel32_ss
	push	dptr _app_eip
	push	wptr _sel32_cs
	mov	dx,offs v_msg12
	call	prints
	add	sp,32
@@done:	ret


win_focus_vm:				; Windows 9x - set focus on specified VM
	test	_misc_byte2,00000010b
	jz	@@done
	xor	bx,bx
	mov	ax,168Bh
	int	2Fh
@@done:	ret


PopState
