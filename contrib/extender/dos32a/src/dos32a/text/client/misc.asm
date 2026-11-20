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
	push	cx
	push	si
	push	di
	push	ds
	push	es
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
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	cx
	mov	_pic_mask,ax
	ret

;=============================================================================
restore_inttab:				; restore real mode interrupts
	test	cs:_misc_byte,00000100b
	jz	@restore_inttab_done
	cmp	cs:_sys_type,3
	jz	@restore_inttab_1
	xor	eax,eax			; reset Null-Pointer protection
	mov	dr7,eax
@restore_inttab_1:	pushf
	cli
	push	si
	push	di
	push	ds
	push	es
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
	pop	es
	pop	ds
	pop	di
	pop	si
	popf
@restore_inttab_done:	ret

;=============================================================================
check_inttab:
	test	cs:_misc_byte,00001000b
	jz	@check_inttab_done
	pushad
	push	ds
	push	es
	xor	bx,bx
	mov	ds,cs:_sel_ss
	mov	es,cs:_sel_zero
	mov	esi,STACKSIZE*16
	xor	edi,edi
	cld
@check_inttab_1:	cmps	dptr ds:[esi],[edi]
	jnz	@check_inttab_2
@check_inttab_3:	inc	bx
	cmp	bx,256
	jb	@check_inttab_1
	pop	es
	pop	ds
	popad
@check_inttab_done:	ret
@check_inttab_2:	mov	ax,9003h
	push	bx
	push	si
	mov	si,bx
	call	report_error
	pop	si
	pop	bx
	jmp	@check_inttab_3

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
	push	ebx
	push	ecx
	push	edx
	push	ebp
	mov	ebp,ecx
	xor	ax,ax			; allocate descriptor
	mov	cx,1
	int	31h
	jc	@set_descriptor_err
	mov	bx,ax
	mov	ax,0009h		; set access rights
	mov	cx,dx
	int	31h
	jc	@set_descriptor_err
	dec	ax			; set limit
	mov	ecx,ebp
	mov	dx,cx
	shr	ecx,16
	int	31h
	jc	@set_descriptor_err
	dec	ax			; set base
	mov	ecx,edi
	mov	dx,cx
	shr	ecx,16
	int	31h
	jc	@set_descriptor_err
	mov	ax,bx
@set_descriptor_err:	pop	ebp
	pop	edx
	pop	ecx
	pop	ebx
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

intxxh:	push	cx
	push	edi
	push	es
	xor	eax,eax
	mov	[ebp+20h],ax		; clear Flags
	mov	[ebp+2Eh],eax		; clear SS:SP
	xor	cx,cx
	push	ss
	pop	es
	mov	edi,ebp
	mov	ax,0300h
	int	31h
	pop	es
	pop	edi
	pop	cx
	pop	bx
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
	jz	@initialize_mouse_err			; if not, report warning
	mov	ax,0021h		; software reset mouse handler
	int	33h
	cmp	ax,0021h		; check if function supported
	jnz	@initialize_mouse_1			; if yes, go on
	xor	ax,ax			; try hardware reset
	int	33h
	inc	ax			; if failed, report warning
	jnz	@initialize_mouse_err
@initialize_mouse_1:	mov	ax,0015h		; get mouse buffer size
	int	33h
	movzx	eax,bx
	cmp	eax,_lobufsize
	jae	@initialize_mouse_err
	mov	_mus_size,eax
	push	ds
	push	es
	push	cs
	pop	ds
	push	ss
	pop	es
	mov	esi,offs _mus_int_pm
	mov	edi,STACKSIZE*16-40h
	mov	ax,0303h
	int	31h
	pop	es
	pop	ds
	jc	dpmi_error
	mov	_mus_backoff,dx
	mov	_mus_backseg,cx
	ret
@initialize_mouse_err:	mov	wptr _int33,0CF66h
If DOS_LEGACY_125_20 ne 0
	ret				; cleanly disable PM mouse wrapper when INT 33h absent
Else
	mov	ax,9004h
	jmp	report_error		; "mouse init failed"
Endif

;=============================================================================
install_client_ints:
	mov	ax,0205h
	mov	cx,cs

	mov	bl,10h
	mov	edx,offs _int10
	int	31h
	jc	@install_client_ints_err

	mov	bl,21h
	mov	dx,offs _int21
	int	31h
	jc	@install_client_ints_err

	mov	bl,23h
	mov	dx,offs _int23
	int	31h
	jc	@install_client_ints_err

	mov	bl,33h
	mov	dx,offs _int33
	int	31h
	jc	@install_client_ints_err


	mov	ax,0203h

	mov	bl,00h
	mov	dx,offs eh00
	int	31h
	jc	@install_client_ints_err

	mov	bl,01h
	mov	dx,offs eh01
	int	31h
	jc	@install_client_ints_err

	mov	bl,02h
	mov	dx,offs eh02
	int	31h
	jc	@install_client_ints_err

	mov	bl,03h
	mov	dx,offs eh03
	int	31h
	jc	@install_client_ints_err

	mov	bl,04h
	mov	dx,offs eh04
	int	31h
	jc	@install_client_ints_err

	mov	bl,05h
	mov	dx,offs eh05
	int	31h
	jc	@install_client_ints_err

	mov	bl,06h
	mov	dx,offs eh06
	int	31h
	jc	@install_client_ints_err

	mov	bl,07h
	mov	dx,offs eh07
	int	31h
	jc	@install_client_ints_err

	mov	bl,08h
	mov	dx,offs eh08
	int	31h
	jc	@install_client_ints_err

	mov	bl,09h
	mov	dx,offs eh09
	int	31h
	jc	@install_client_ints_err

	mov	bl,0Ah
	mov	dx,offs eh0A
	int	31h
	jc	@install_client_ints_err

	mov	bl,0Bh
	mov	dx,offs eh0B
	int	31h
	jc	@install_client_ints_err

	mov	bl,0Ch
	mov	dx,offs eh0C
	int	31h
	jc	@install_client_ints_err

	mov	bl,0Dh
	mov	dx,offs eh0D
	int	31h
	jc	@install_client_ints_err

	mov	bl,0Eh
	mov	dx,offs eh0E
	int	31h

	clc
@install_client_ints_err:	ret


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
@uninstall_client_ints_0:	mov	cx,wptr cs:_exc_tab[ebx*8+4]
	mov	edx,dptr cs:_exc_tab[ebx*8+0]
	int	31h
	inc	bl
	cmp	bl,15
	jb	@uninstall_client_ints_0

	clc
@uninstall_client_ints_done:	ret

;=============================================================================
install_nullptr_protect:
	cmp	_sys_type,3
	jz	@install_nullptr_protect_done
	test	_misc_byte,10000000b
	jz	@install_nullptr_protect_done

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
@install_nullptr_protect_done:	ret

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
	jc	@setup_selectors_err
	mov	_sel_zero,ax
	mov	ax,0008h		; resize DS limit to 4GB
	mov	bx,ds			; this might be needed to access
	mov	cx,0FFFFh		; DOS buffer which is it self 64KB
	mov	dx,cx
	int	31h
	jc	@setup_selectors_err
	ret
@setup_selectors_err:	mov	ax,4CFFh
	int	21h





;=============================================================================
check_command_line:
	mov	di,80h
	movzx	cx,es:[di]		; get length of command line
	jcxz	@check_command_line_err			; if zero, error
	inc	di			; offset to start of command line
	mov	al,20h
	repe	scasb			; look for non-space character
	jz	@check_command_line_err			; if not found, error
	dec	di
	inc	cx
	mov	bx,di
@check_command_line_1:	mov	al,es:[di]
	cmp	al,09h			; look for TAB character
	jz	@check_command_line_2
	cmp	al,0Dh			; look for CR character
	jz	@check_command_line_2
	cmp	al,20h			; look for ' ' character
	jz	@check_command_line_2
	inc	di
	loop	@check_command_line_1
@check_command_line_2:	mov	cx,di
	mov	si,bx			; SI = pointer to file name
	mov	di,bx			; DI = pointer to file name
	sub	cx,bx			; CX = file name length
@check_command_line_done:	ret
@check_command_line_err:	xor	si,si
	ret

;=============================================================================
remove_name_from_cmd:
	call	check_command_line
	jz	@remove_name_from_cmd_done
	mov	al,20h
	rep	stosb
@remove_name_from_cmd_done:	ret

;=============================================================================
check_if_fullname:
	push	ds
	push	es
	call	check_command_line
	jz	@check_if_fullname_done
	mov	al,'\'
	mov	bx,cx
	repne	scasb
	jcxz	@check_if_fullname_done
	push	ds
	push	es
	pop	ds
	pop	es
	mov	cx,bx
	mov	di,offs start+40h
	rep	movsb
	xor	al,al
	stosb
@check_if_fullname_full:	pop	es
	pop	ds
	stc
	ret
@check_if_fullname_done:	pop	es
	pop	ds
	clc
	ret

;=============================================================================
; dos_open_rdonly: Open DS:EDX read-only with automatic pre-DOS-3.0 mode lowering
;
dos_open_rdonly:
If DOS_LEGACY_125_20 ne 0
	cmp	cs:_dos_ver_major,3
	jb	@dos_open_compat
	mov	ax,3DC0h
	int	21h
	jnc	@dos_open_done
	cmp	ax,1			; retry compatibility mode if SHARE absent (AX=01h)
	jnz	@dos_open_stc
@dos_open_compat:
	mov	ax,3D00h
	int	21h
	ret
@dos_open_stc:
	stc
@dos_open_done:
	ret
Else
	mov	ax,3DC0h
	int	21h
	ret
Endif

;=============================================================================
update_environment:
	push	ds
	push	es
If DOS_LEGACY_125_20 ne 0
	cmp	_using_synth_env,0	; MS-DOS 1.25 / synthesized environment block
	jz	@update_env_check_v2
	push	ds
	pop	es
	mov	si,offs start
	mov	di,offs _synth_env+4
	mov	cx,60
@update_env_synth_cpy:
	lodsb
	stosb
	test	al,al
	loopne	@update_env_synth_cpy
	mov	bptr es:[offs _synth_env+64],0
	pop	es
	pop	ds
	ret
@update_env_check_v2:
Endif
	call	check_if_fullname		; check if full path provided
	jc	@update_environment_skip	; if yes, skip copying
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
	jnz	@update_environment_0
	dec	di
@update_environment_0:	mov	bptr [di-1],'\'
	mov	si,offs start
	mov	cx,64
@update_environment_1:	lodsb
	stosb
	test	al,al
	loopne	@update_environment_1
@update_environment_skip:	push	ds
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
	jz	@update_environment_err	; if yes, jump to error
	lar	cx,ax
	jnz	@update_environment_err
	mov	es,ax
	xor	al,al
	xor	di,di
	mov	cx,-1
If DOS_LEGACY_125_20 ne 0
	cmp	_dos_ver_major,3	; DOS 2.0 has no argv[0] tail yet; append via new block
	jb	create_new_environment
Endif
@update_environment_2:	repne	scasb
	scasb
	jnz	@update_environment_2
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
	pop	es
	pop	ds
	ret
@update_environment_err:	mov	ax,2000h
	jmp	report_error

create_new_environment:
	push	bx
	mov	cx,-1
	xor	di,di
	cmp	bptr es:[0],0		; handle empty ("\0\0") environment cleanly
	jz	@create_new_env_empty
@create_new_environment_1:	repne	scasb			; get size of environment
	dec	cx
	scasb
	jnz	@create_new_environment_1
	not	cx
	inc	cx
	inc	cx
	jmp	@create_new_env_alloc
@create_new_env_empty:
	mov	cx,4			; "\0\0" + word 0001h
@create_new_env_alloc:
	push	cx
	add	bx,cx
	shr	bx,4
	inc	bx
	mov	ax,0100h
	int	31h
	pop	cx
	jc	@create_new_environment_err
	push	es
	pop	ds
	mov	es,dx
	xor	si,si
	xor	di,di
	rep	movsb
	mov	wptr es:[di-2],0001h	; ensure DOS 3.0+ string-count word (0001h)
	pop	cx
	mov	si,offs start+40h
	push	cs
	pop	ds
	rep	movsb
	pop	es
	pop	ds
	mov	es:[002Ch],dx
	ret
@create_new_environment_err:	mov	ax,2000h
	jmp	report_error


;=============================================================================
open_exec:
	push	ds
	push	es
	mov	ax,es:[002Ch]		; get environment selector
	test	ax,ax			; check if selector is NULL
	jz	@open_exec_err		; if yes, jump to error
	lar	cx,ax
	jnz	@open_exec_err
	mov	es,ax
	xor	ax,ax
	xor	di,di
	mov	cx,0FFFFh
@open_exec_1:	repne	scasb			; look for end of environment
	scasb
	jcxz	@open_exec_err		; reached end of environment
	jnz	@open_exec_1
	inc	di
	inc	di
	push	ds
	push	es
	pop	ds
	pop	es
	push	di
	mov	si,di
	mov	di,offs start
@open_exec_2:	lodsb
	stosb
	test	al,al
	jnz	@open_exec_2
	pop	di
	movzx	edx,di			; DS:EDX = pointer to exec name
	call	dos_open_rdonly		; open file
	pop	es
	pop	ds
	mov	_exec_handle,ax		; store file handle
	mov	ax,2001h
	jc	file_error		; if error, jump
	ret
@open_exec_err:	mov	ax,2000h
	jmp	report_error

If DOS_LEGACY_125_20 ne 0
;=============================================================================
; try_open_self_legacy: Non-fatal check for bound payload on DOS < 3.0
; Out: CF=0 if self opened and bound MZ+LE found (_exec_start != 0), CF=1 otherwise
;
try_open_self_legacy:
	push	ds
	push	es
	push	ds
	pop	es
	mov	si,offs _default_self
	mov	di,offs start
	mov	cx,11
	rep	movsb
	mov	edx,offs start
	call	dos_open_rdonly
	jc	@try_self_not_bound
	mov	_exec_handle,ax
	xor	edx,edx
	mov	ecx,64
	push	bx
	push	ds
	push	fs
	pop	ds
	mov	bx,cs:_exec_handle
	mov	ah,3Fh
	int	21h
	pop	ds
	pop	bx
	jc	@try_self_close
	cmp	ax,64
	jb	@try_self_close
	cmp	wptr fs:[0000h],'ZM'
	jnz	@try_self_close
	cmp	wptr fs:[0018h],0040h
	jnz	@try_self_close
	mov	eax,fs:[003Ch]
	test	eax,eax
	jz	@try_self_close
	mov	_exec_start,eax
	pop	es
	pop	ds
	clc
	ret
@try_self_close:
	call	close_exec
@try_self_not_bound:
If DOS32A_BOUND_STUB ne 0
	; Bound stub on DOS < 3.0: scan COMMAND.COM memory for the launched .EXE
	call	find_bound_self_in_ram
	jnc	@try_self_ok
Endif
	pop	es
	pop	ds
	stc
	ret
@try_self_ok:
	pop	es
	pop	ds
	clc
	ret

If DOS32A_BOUND_STUB ne 0
;-----------------------------------------------------------------------------
; find_bound_self_in_ram: Locate bound .EXE path on MS-DOS 1.25 & 2.00
;   - DOS 1.25: scans COMMAND.COM FCBs below PSP ([00700h..PSP<<4]) for [drv,8,'EXE']
;   - DOS 2.00: scans COMMAND.COM EXEC path buffers ([00700h..9FFF0h]) for "*.EXE\0"
;
find_bound_self_in_ram:
	cmp	_dos_ver_major,2
	jae	@find_bound_dos2
	mov	edi,09FFF0h
	movzx	ebp,wptr es:[0002h]
	shl	ebp,4
	cmp	edi,ebp
	jbe	@find_bound_dos1_low
	call	@scan_fcb_range
	jnc	@find_bound_hit
	test	al,al
	jnz	@find_bound_fail
@find_bound_dos1_low:
	movzx	edi,_seg_es
	shl	edi,4
	sub	edi,12
	mov	ebp,00500h
	call	@scan_fcb_range
	jnc	@find_bound_hit
	jmp	@find_bound_fail

@scan_fcb_range:
@find_fcb_loop:
	cmp	edi,ebp
	jb	@scan_fcb_fail
	cmp	wptr gs:[edi+9],'XE'
	jnz	@find_fcb_next
	cmp	bptr gs:[edi+11],'E'
	jnz	@find_fcb_next
	mov	al,gs:[edi]
	cmp	al,4
	ja	@find_fcb_next
	mov	ah,gs:[edi+1]
	cmp	ah,21h
	jb	@find_fcb_next
	cmp	ah,5Ah
	ja	@find_fcb_next
	cmp	bptr gs:[edi+12],20h
	jnz	@find_fcb_not_dirent
	cmp	dptr gs:[edi+16],0
	jz	@find_fcb_next
@find_fcb_not_dirent:
	; Build [D:]NAME.EXE\0 in [offs start]
	mov	bx,offs start
	test	al,al
	jz	@find_fcb_nodrv
	add	al,'A'-1
	mov	[bx],al
	mov	bptr [bx+1],':'
	add	bx,2
@find_fcb_nodrv:
	mov	cx,8
	mov	esi,edi
	inc	esi
@find_fcb_chr:
	mov	al,gs:[esi]
	inc	esi
	cmp	al,' '
	jz	@find_fcb_ext
	cmp	al,21h
	jb	@find_fcb_next
	mov	[bx],al
	inc	bx
	loop	@find_fcb_chr
@find_fcb_ext:
	mov	dptr [bx],4558452Eh	; '.EXE'
	mov	bptr [bx+4],0
	call	@verify_bound_candidate
	jnc	@scan_fcb_ok
	test	al,al
	jnz	@scan_fcb_fail
@find_fcb_next:
	dec	edi
	jmp	@find_fcb_loop
@scan_fcb_fail:
	stc
	ret
@scan_fcb_ok:
	clc
	ret

@find_bound_dos2:
	; Scan transient COMMAND.COM EXEC buffer at top of RAM first, then resident COMMAND.COM below PSP
	mov	edi,09FFF0h
	movzx	ebp,wptr es:[0002h]
	shl	ebp,4
	cmp	edi,ebp
	jbe	@find_bound_dos2_low
	call	@scan_asciiz_exe_range
	jnc	@find_bound_hit
	test	al,al
	jnz	@find_bound_fail
@find_bound_dos2_low:
	movzx	edi,_seg_es
	shl	edi,4
	sub	edi,5
	mov	ebp,00700h
	call	@scan_asciiz_exe_range
	jnc	@find_bound_hit
@find_bound_fail:
	stc
	ret
@find_bound_hit:
	; Update argv[0] in environment with discovered bound .EXE name
	push	ds
	pop	es
	cmp	bptr [start+1],':'
	jnz	@find_bound_norm_done
	cmp	bptr [start+2],'\'
	jz	@find_bound_norm_done
	mov	si,offs start+2
	mov	di,offs start
	mov	cx,62
@find_bound_strip_drv:
	lodsb
	stosb
	test	al,al
	loopne	@find_bound_strip_drv
@find_bound_norm_done:
	mov	si,offs start
	mov	di,offs start+40h
	mov	cx,64
@find_bound_cp40:
	lodsb
	stosb
	test	al,al
	loopne	@find_bound_cp40
	mov	es,_sel_es
	call	update_environment
	clc
	ret

@scan_asciiz_exe_range:
@scan_exe_loop:
	cmp	edi,ebp
	jb	@scan_exe_fail
	cmp	bptr gs:[edi+4],0
	jnz	@scan_exe_prev
	cmp	bptr gs:[edi],'.'
	jnz	@scan_exe_prev
	mov	eax,gs:[edi+1]
	and	eax,00DFDFDFh		; uppercase 3 chars
	cmp	eax,00455845h		; 'EXE'
	jnz	@scan_exe_prev
	; Walk backwards to beginning of path token
	mov	esi,edi
@scan_exe_bk:
	cmp	esi,ebp
	jbe	@scan_exe_copy
	mov	al,gs:[esi-1]
	cmp	al,21h
	jb	@scan_exe_copy
	cmp	al,7Eh
	ja	@scan_exe_copy
	cmp	al,'"'
	jz	@scan_exe_copy
	dec	esi
	mov	ecx,edi
	sub	ecx,esi
	cmp	ecx,60
	jb	@scan_exe_bk
@scan_exe_copy:
	cmp	esi,edi
	jz	@scan_exe_prev
	mov	bx,offs start
@scan_exe_cp:
	mov	al,gs:[esi]
	mov	[bx],al
	inc	esi
	inc	bx
	test	al,al
	jnz	@scan_exe_cp
	call	@verify_bound_candidate
	jnc	@scan_exe_ok
	test	al,al
	jnz	@scan_exe_fail
@scan_exe_prev:
	dec	edi
	jmp	@scan_exe_loop
@scan_exe_fail:
	stc
	ret
@scan_exe_ok:
	clc
	ret

@verify_bound_candidate:
	push	edi
	push	ebp
	mov	edx,offs start
	call	dos_open_rdonly
	jc	@verify_cand_fail
	mov	_exec_handle,ax
	xor	edx,edx
	mov	ecx,64
	push	bx
	push	ds
	push	fs
	pop	ds
	mov	bx,cs:_exec_handle
	mov	ah,3Fh
	int	21h
	pop	ds
	pop	bx
	jc	@verify_cand_close
	cmp	ax,64
	jb	@verify_cand_close
	cmp	wptr fs:[0000h],'ZM'
	jnz	@verify_cand_close
	cmp	wptr fs:[0018h],0040h
	jnz	@verify_cand_close
	movzx	edx,wptr fs:[0008h]
	shl	edx,4
	mov	ebp,fs:[003Ch]
	push	bx
	push	cx
	mov	bx,_exec_handle
	mov	ecx,edx
	shr	ecx,16
	mov	ax,4200h
	int	21h
	pop	cx
	pop	bx
	jc	@verify_cand_close
	xor	edx,edx
	mov	ecx,4
	push	bx
	push	ds
	push	fs
	pop	ds
	mov	bx,cs:_exec_handle
	mov	ah,3Fh
	int	21h
	pop	ds
	pop	bx
	jc	@verify_cand_close
	cmp	ax,4
	jb	@verify_cand_close
	cmp	dptr fs:[0000h],32334449h
	jnz	@verify_cand_close
	test	ebp,ebp
	jz	@verify_cand_unbound_id32
	mov	_exec_start,ebp
	pop	ebp
	pop	edi
	xor	al,al
	clc
	ret
@verify_cand_unbound_id32:
	call	close_exec
	pop	ebp
	pop	edi
	mov	al,1
	stc
	ret
@verify_cand_close:
	call	close_exec
@verify_cand_fail:
	pop	ebp
	pop	edi
	xor	al,al
	stc
	ret
Endif
Endif




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
	push	ds
	push	es
	push	ds	; swap DS and ES
	push	es
	pop	ds
	pop	es
	mov	di,offs start		; copy application exec name
	rep	movsb
	push	es
	pop	ds
	mov	bptr [di],0
	mov	edx,offs start
	call	dos_open_rdonly
	jc	@open_extrn_exec_1			; if error opening, try ".EXE"
	pop	es
	pop	ds
	mov	_exec_handle,ax
	ret

@open_extrn_exec_1:	mov	bx,offs start
@open_extrn_exec_2:	cmp	bptr [bx],2Eh
	stc
	jz	@open_extrn_exec_err
	inc	bx
	cmp	bx,di
	jb	@open_extrn_exec_2
	mov	eax,4558452Eh
	cmp	eax,[di-4]
	stc
	jz	@open_extrn_exec_err
	mov	eax,6578652Eh
	cmp	eax,[di-4]
	stc
	jz	@open_extrn_exec_err
	mov	[di],eax
	mov	bptr [di+4],0
	call	dos_open_rdonly
@open_extrn_exec_err:	pop	es
	pop	ds
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
@search_for_mz_0:	movzx	eax,wptr fs:[0004h]	; get pages in file
	shl	eax,9			; *512
	movzx	ebx,wptr fs:[0002h]	; get bytes on last page
	add	eax,ebx
	mov	bx,fs:[0000h]
	cmp	bx,'ZM'
	jz	@search_for_mz_1
	cmp	bx,'WB'
	jz	@search_for_mz_2
	jmp	@search_for_mz_3
@search_for_mz_1:	sub	eax,0200h
@search_for_mz_2:	mov	esi,ebp
	add	ebp,eax
	mov	edx,ebp
	call	seek_from_start
	mov	ecx,64
	xor	edx,edx
	call	load_fs_block
	test	eax,eax
	jnz	@search_for_mz_0
	mov	ax,3003h
	jmp	file_error		; if zero, no app in file
@search_for_mz_3:	mov	bx,fs:[0000h]
	cmp	bx,'EL'
	jz	@search_for_mz_4
	cmp	bx,'XL'
	jz	@search_for_mz_4
	cmp	bx,'CL'
	jz	@search_for_mz_4
	cmp	bx,'EP'
	jz	@search_for_mz_4
	mov	edx,ebp
	call	seek_from_start
	call	search_for_le
@search_for_mz_4:	cmp	eax,esi
	jz	@search_for_mz_5
	mov	edx,esi
	add	eax,10h
	add	edx,10h
	and	al,0F0h
	and	dl,0F0h
	cmp	eax,edx
	jz	@search_for_mz_5
	mov	_exec_start,ebp
	mov	_app_off_datapages,esi
@search_for_mz_5:	ret

search_for_le:
@search_for_le_1:	mov	edx,_lobufzero		; DS:EDX = current ptr
	mov	ecx,_lobufsize		; ECX = bytes to load
	call	load_gs_block
	test	ax,ax			; check if no bytes read
	mov	ax,3003h
	jz	file_error		; if true, no app in file
	shr	cx,1
@search_for_le_2:	mov	ax,gs:[edx+0]
	mov	bx,gs:[edx+2]
	test	bx,bx
	jnz	@search_for_le_4
	cmp	ax,'EL'			; 'LE' type
	jz	@search_for_le_3
	cmp	ax,'XL'			; 'LX' type
	jz	@search_for_le_3
	cmp	ax,'CL'			; 'LC' type
	jz	@search_for_le_3
	cmp	ax,'PE'			; 'PE' type
	jz	@search_for_le_3
@search_for_le_4:	add	edx,2
	add	ebp,2			; increment pointer in file
	loop	@search_for_le_2
	jmp	@search_for_le_1
@search_for_le_3:	ret





;=============================================================================
close_exec:
	xor	bx,bx
	xchg	bx,cs:_exec_handle
	test	bx,bx
	jz	@close_exec_done
	mov	ah,3Eh
	int	21h
@close_exec_done:
	ret


;=============================================================================
seek_from_start:
	push	bx
	push	ecx
	push	edx
	push	eax
	mov	ecx,edx
	shr	ecx,16
	mov	bx,cs:_exec_handle
	mov	ax,4200h
	int	21h
	pop	eax
	pop	edx
	pop	ecx
	pop	bx
	jc	file_errorm
	ret


;=============================================================================
load_fs_block:
	push	bx
	push	ds
	push	fs
	pop	ds
	jmp	@load_block
load_gs_block:
	push	bx
	push	ds
	push	gs
	pop	ds
@load_block:
	mov	bx,cs:_exec_handle
	mov	ah,3Fh
	int	21h
	pop	ds
	pop	bx
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
	jz	@verbose_getmem_1
	mov	dx,offs v_msg05
	shr	eax,10
@verbose_getmem_1:	push	dx
	push	ax
	mov	ah,48h
	mov	bx,-1
	int	21h
	shr	bx,6
	push	bx
	jmp	bp

verbose_showsys:
	test	_misc_byte2,00010000b
	jz	@verbose_showsys_done
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
@verbose_showsys_done:	ret

verbose_showloadhdr:
	test	_misc_byte2,00010000b
	jz	@verbose_showloadhdr_done
	movzx	eax,_app_type
	lea	eax,v_msg03[eax*2+eax]
	push	ax
	push	offs start
	mov	dx,offs v_msg10
	call	prints
	add	sp,4
@verbose_showloadhdr_done:	ret

verbose_showloadobj:
	test	_misc_byte2,00010000b
	jz	@verbose_showloadobj_done
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
@verbose_showloadobj_done:	ret

verbose_showstartup:
	test	_misc_byte2,00010000b
	jz	@verbose_showstartup_done
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
@verbose_showstartup_done:	ret


win_focus_vm:				; Windows 9x - set focus on specified VM
	test	_misc_byte2,00000010b
	jz	@win_focus_vm_done
If DOS_LEGACY_125_20 ne 0
	cmp	_dos_ver_major,4
	jb	@win_focus_vm_done
Endif
	xor	bx,bx
	mov	ax,168Bh
	int	2Fh
@win_focus_vm_done:	ret


	.386p
