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
		.MODEL flat
		LOCALS
		NOJUMPS

include	stddef.inc


.CODE

start32:
	push	ds es ss fs gs
	pushfd
	pushad
	mov	edx,offs __message0
	call	print_string
	call	check_d32a_dpmi
	call	get_d32a_selectors
	popad
	popfd

	pushfd
	pushad
	call	Debug_Init_		; initialize debugger
	call	fill_memory_with_ints

	mov	ebx,__history_bufsize	; setup history buffer
	mov	ax,0FF91h
	int	21h
	mov	__history_bufbase,ebx
	mov	__history_bufhand,esi
	mov	__history_bufnum,0
	mov	__history_bufptr,0

	call	check_command_line
	test	esi,esi
	jz	@err1
	call	open_exec
	call	load_exec_header
	call	update_environment
	mov	bx,ds
	mov	ax,000Ah
	int	31h
	mov	bx,ax
	mov	dx,wptr __buffer+0
	mov	cx,wptr __buffer+2
	mov	ax,0007h
	int	31h
	push	fs
	pop	gs			; GS = zero selector
	mov	fs,bx			; FS = buffer selector
	mov	ax,cs
	lar	dx,ax
	shr	dx,8			; set rights = data
	and	dx,00060h
	xor	dx,0C092h
	mov	_acc_rights,dx

	movzx	edx,wptr __exec_start+0
	movzx	ecx,wptr __exec_start+2
	mov	bx,__exec_handle
	mov	ax,4200h
	int	21h			; move to file start
	jc	@err4
	mov	edx,__buffer		; DS:EDX = current ptr
	mov	ecx,4			; ECX = bytes to load
	mov	bx,__exec_handle
	mov	ax,3F00h
	int	21h
	jc	@err4
	movzx	edx,wptr __exec_start+0
	movzx	ecx,wptr __exec_start+2
	mov	bx,__exec_handle
	mov	ax,4200h
	int	21h			; move to file start
	jc	@err4

	push	offs @@2
	mov	edx,__buffer
	mov	ax,[edx+0]		; get file signature
	mov	bx,[edx+2]
	test	bx,bx
	jnz	@lerr6
	cmp	ax,'EL'			; 'LE' type
	jz	load_le_app
	cmp	ax,'XL'			; 'LX' type
	jz	load_lx_app
	cmp	ax,'CL'			; 'LC' type
	jz	load_lc_app
	jmp	@lerr6

@@2:	call	remove_name_from_cmd
	call	close_exec
	clr	ebx
	mov	ax,0300h
	int	10h			; get DOS cursor position
	dec	dh
	dec	dh
	mov	ax,0200h
	int	10h
	push	edx
	mov	ecx,160
@@l1:	mov	ax,0E20h
	xor	ebx,ebx
	push	ecx
	int	10h
	pop	ecx
	loop	@@l1
	pop	edx
	mov	ax,0200h
	int	10h
	xor	eax,eax
	int	33h

	cli
	cld
	mov	ax,wptr _sel32_cs
	mov	wptr _old_cs,ax
	mov	ax,wptr _sel32_ss
	mov	wptr _old_ds,ax
	mov	wptr _old_ss,ax
	mov	eax,_app_esp
	mov	dptr _old_esp,eax

	mov	bx,_sel32_cs		; get application's CS base
	mov	ax,0006h
	int	31h
	shl	ecx,16
	mov	cx,dx
	add	ecx,_app_eip
	mov	dr0,ecx			; place a breakpoint
	mov	eax,00000303h
	mov	dr7,eax
	mov	__history_bufon,1	; turn on history buffer
	popad
	popfd
	pop	gs fs ss es ds			; restore ES, FS, GS

	mov	ds,word ptr cs:_sel32_ss	; load new DS
	lss	esp,fword ptr cs:_app_esp	; load new SS:ESP
	jmp	fword ptr cs:_app_eip




;=============================================================================
check_d32a_dpmi:
	push	es ds			; check for DOS/32 Advanced ADPMI
	mov	esi,offs __idstring
	mov	ax,0A00h
	int	31h
	pop	ds
	mov	__api_off,edi
	mov	__api_sel,es
	pop	es
	jc	@err0
	mov	ax,es
	cmp	ax,__api_sel
	jz	@err0
	test	bl,02h
	jz	@err2
	cmp	cl,04h
	jb	@err6
	ret

get_d32a_selectors:
	mov	al,06h			; get ADPMI selectors
	call	fword ptr __api_off
	mov	__kernel_codesel,bx
	mov	__kernel_datasel,cx
	mov	__kernel_zerosel,dx
	mov	__kernel_codeseg,si
	mov	__client_codesel,di
	ret

fill_memory_with_ints:
	push	es
	push	ds
	pop	es
	mov	ax,0FF90h		; get free memory information
	int	21h
	mov	__free_mem,eax
	mov	ebx,eax			; allocate memory
	mov	ax,0FF91h
	int	21h
	jc	@@done
	mov	__base_mem,ebx
	mov	eax,0CCCCCCCCh		; set memory to INT 3
	mov	edi,__base_mem
	mov	ecx,__free_mem
	mov	dl,cl
	shr	ecx,2
	rep	stosd
	mov	cl,dl
	and	cl,3
	rep	stosb
	mov	ax,0FF92h		; free memory
	int	21h
@@done:	pop	es
	ret



;=============================================================================
@err0:	mov	edx,offs __error0
	call	print_string
	jmp	_abort
@err1:	mov	edx,offs __error1
	call	print_string
	jmp	_abort
@err2:	mov	edx,offs __error2
	call	print_string
	jmp	_abort
@err3:	mov	edx,offs __error3
	call	print_string
	jmp	_abort
@err4:	mov	edx,offs __error4
	call	print_string
	jmp	_abort
@err5:	mov	edx,offs __error5
	call	print_string
	jmp	_abort
@err6:	mov	edx,offs __error6
	call	print_string
	jmp	_abort





;=============================================================================
check_command_line:
	mov	edi,80h
	movzx	ecx,byte ptr es:[edi]	; get length of command line
	jecxz	@@err			; if zero, error
	inc	edi			; offset to start of command line
	mov	al,20h
	repe	scasb			; look for non-space character
	jz	@@err			; if not found, error
	dec	edi
	inc	ecx
	mov	ebx,edi
@@1:	mov	al,es:[edi]
	cmp	al,09h			; look for TAB character
	jz	@@2
	cmp	al,0Dh			; look for CR character
	jz	@@2
	cmp	al,20h			; look for ' ' character
	jz	@@2
	inc	edi
	loop	@@1
@@2:	mov	ecx,edi
	mov	esi,ebx			; SI = pointer to file name
	mov	edi,ebx			; DI = pointer to file name
	sub	ecx,ebx
@@done:	ret
@@err:	xor	esi,esi
	ret
remove_name_from_cmd:
	call	check_command_line
	jz	@@done
	mov	al,20h
	rep	stosb
@@done:	ret


;-----------------------------------------------------------------------------
update_environment:
	push	ds es
	sub	esp,64
	mov	ebp,esp
	mov	ah,19h
	int	21h
	mov	dl,al
	add	al,'A'
	mov	byte ptr [ebp+0],al	; set drive
	mov	word ptr [ebp+1],'\:'
	inc	dl
	mov	ah,47h
	lea	esi,[ebp+3]
	int	21h			; read directory
	push	ds
	pop	es
	xor	al,al
	mov	edi,esi
	mov	ecx,64
	repne	scasb
	cmp	byte ptr [edi-2],'\'
	jnz	@@0
	dec	edi
@@0:	mov	byte ptr [edi-1],'\'
	mov	es,[esp+64]
	push	edi
	call	check_command_line
	pop	edi
	push	ds
	pop	es
	mov	ds,[esp+64]
@@1:	lodsb
	stosb
	test	al,al
	loopne	@@1
	xor	al,al
	stosb
	push	es
	pop	ds
	mov	ecx,-1
	mov	edi,ebp
	repne	scasb
	not	ecx
	mov	ebx,ecx
	mov	es,[esp+64]
	mov	ax,es:[002Ch]		; get environment selector
	test	ax,ax			; check if selector is NULL
	jz	@@err			; if yes, jump to error
	lar	cx,ax
	jnz	@@err
	mov	es,ax
	xor	al,al
	xor	edi,edi
	mov	ecx,7FFFh
@@2:	repne	scasb
	scasb
	jnz	@@2
	add	edi,2
	mov	ecx,-1
	mov	edx,edi
	repne	scasb
	not	ecx
	cmp	ebx,ecx
	ja	create_new_environment
	mov	ecx,ebx
	mov	edi,edx
	mov	esi,ebp
	rep	movsb
@@done:	add	esp,64
	pop	es ds
	ret
@@err:	add	esp,64
	pop	es ds
	stc
	ret

create_new_environment:
	push	ebx ebp
	mov	ecx,-1
	xor	edi,edi
@@1:	repne	scasb
	dec	ecx
	scasb
	jnz	@@1
	not	ecx
	add	ecx,2
	push	ecx
	add	ecx,ebx
	shr	ecx,4
	inc	ecx
	mov	ebx,ecx
	mov	ax,0100h
	int	31h
	jc	_abort
	pop	ecx
	push	es
	pop	ds
	mov	es,dx
	xor	esi,esi
	xor	edi,edi
	rep	movsb
	pop	esi ecx
	push	ss
	pop	ds
	rep	movsb
	add	esp,64
	pop	es ds
	mov	es:[002Ch],dx
	ret


;-----------------------------------------------------------------------------
open_exec:
	push	ds es
	push	ss es
	pop	ds es
	mov	edi,cs:_system_block
	rep	movsb
	mov	byte ptr es:[edi],0
	push	ss
	pop	ds
	mov	edx,_system_block
	mov	ax,3DC0h
	int	21h
	jc	@@1			; if error opening, try ".EXE"
	pop	es ds
	mov	__exec_handle,ax
	jc	@err3
	ret

@@1:	mov	ebx,_system_block
@@2:	cmp	byte ptr [ebx],2Eh
	stc
	jz	@@err
	inc	ebx
	cmp	ebx,edi
	jb	@@2
	mov	eax,4558452Eh
	cmp	eax,[edi-4]
	stc
	jz	@@err
	mov	eax,6578652Eh
	cmp	eax,[edi-4]
	stc
	jz	@@err
	mov	[edi],eax
	mov	byte ptr [edi+4],0
	mov	ax,3DC0h
	int	21h
@@err:	pop	es ds
	mov	__exec_handle,ax
	jc	@err3
	ret


load_exec_header:
	mov	ax,3F00h
	mov	bx,__exec_handle
	mov	ecx,64			; ECX = 64 bytes to load
	mov	edx,__buffer		; DS:EDX = ptr
	int	21h
	jc	@err4
	push	edx
	xor	ecx,ecx
	xor	edx,edx
	mov	bx,__exec_handle
	mov	ax,4200h
	int	21h			; move to file start
	pop	edx
	jc	@err4

	xor	ebp,ebp			; reset ptr in app
	cmp	word ptr [edx],'ZM'	; is exec 'MZ' file type
	jnz	search_for_le		; if not, search for known exec type

	mov	eax,[edx+18h]		; MZ reloc-tab must be at offset 0040h
	cmp	ax,40h
	jnz	search_for_mz

	mov	eax,[edx+3Ch]		; get start of 32-bit code
	test	ax,ax			; check if exec is bound
	jz	search_for_mz		; if not, search
	mov	__exec_start,eax
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
	mov	bx,__exec_handle
	mov	ax,4200h
	int	21h			; move to next exec in file
	jc	@err4
	mov	ax,3F00h
	mov	bx,__exec_handle
	mov	ecx,64
	mov	edx,__buffer
	int	21h
	jc	@err4
	test	eax,eax
	jz	@err5
	jmp	@@0

@@3:	cmp	wptr [edx],'EL'
	jz	@@4
	cmp	wptr [edx],'XL'
	jz	@@4
	mov	ecx,ebp
	mov	edx,ebp
	shr	ecx,16
	mov	bx,__exec_handle
	mov	ax,4200h
	int	21h			; move to next exec in file
	jc	@err4
	call	search_for_le
@@4:	mov	__exec_start,ebp
	mov	_app_off_datapages,esi
	ret


search_for_le:
@@1:	mov	edx,__buffer		; DS:EDX = current ptr
	mov	ecx,1000h		; ECX = bytes to load
	mov	ax,3F00h
	mov	bx,__exec_handle
	int	21h
	jc	@err4
	test	eax,eax			; check if no bytes read
	jz	@err5			; if true, no app in file
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

close_exec:
	mov	ah,0Dh			; flush disk buffers
	int	21h
	mov	bx,__exec_handle
	mov	ah,3Eh
	int	21h
	ret


print_string:
	pushad
	clr	eax
	clr	ecx
	mov	ebx,edx
@@1:	cmp	al,[ebx]
	jz	@@2
	inc	ebx
	inc	ecx
	jmp	@@1
@@2:	mov	eax,4000h
	mov	ebx,0002h
	int	21h
	popad
	ret









;=============================================================================
	Align 4
Debug_:
@debug@	db	0C3h		; RET
	xchg	eax,[esp]
	mov	ds:_temp,eax
	xchg	eax,[esp]
	lea	esp,[esp+4]	; remove caller from stack, don't change flags
	pushfd			; simulate an INT (push eflags)
	cli
	push	cs
	push	_temp		; push original caller
	jmp	_int03		; execute INT 3 handler






;=============================================================================
	Align 4
Debug_Init_:				; needed to initialize debugger's INTs
	pushfd
	pushad
	push	cs ds es ss fs gs
	cli
	cld

	mov	ax,001Fh		; disable mouse driver
	int	33h
	cli
	cld

	push	ds
	pop	es

	mov	ds:__datasel,ds		; set up selectors
	mov	ds:__zerosel,fs		; DOS/32A defines DS=ES=CS=0
	mov	ds:__savedstack,esp
	add	ds:__savedstack,56

	mov	ax,0400h
	int	31h
	mov	__cputype,cl
	mov	ax,0E00h
	int	31h
	shr	al,4
	mov	__fputype,al

	mov	edi,offs _registers
	lea	esi,[esp+52]
	mov	ecx,14
@@l1:	mov	eax,[esi]		; copy general & segment regs into buf
	sub	esi,4
	mov	[edi],eax
	add	edi,4
	loop	@@l1
	mov	eax,cr0
	mov	_cr0,eax
	mov	eax,cr2
	mov	_cr2,eax
	mov	eax,cr3
	mov	_cr3,eax
	mov	eax,[esp+60]
	mov	_eip,eax
	mov	eax,[esp+56]
	mov	_efl,eax
	mov	esi,offs _registers	; copy new regs into old regs
	mov	edi,offs _old_registers
	mov	ecx,19
	rep	movsd
	call	clearkeytab
	xor	ecx,ecx
	xor	edx,edx

;	mov	al,08h
;	mov	ecx,seg crt16
;	mov	edx,offs 0
;	call	fword ptr __api_off

	mov	ax,0204h		; get INT vectors
	mov	bl,10h
	int	31h
	mov	_old_int10sel,ecx
	mov	_old_int10off,edx
	mov	bl,21h
	int	31h
	mov	_old_int21sel,ecx
	mov	_old_int21off,edx
	mov	bl,31h
	int	31h
	mov	_old_int31sel,ecx
	mov	_old_int31off,edx
	mov	bl,33h
	int	31h
	mov	_old_int33sel,ecx
	mov	_old_int33off,edx

	mov	ax,0205h
	mov	bl,21h
	mov	cx,cs
	mov	edx,offs _int21		; INT 21h
	int	31h
	mov	ax,0205h
	mov	bl,31h
	mov	cx,cs
	mov	edx,offs _int31		; INT 31h
	int	31h
	mov	ax,0205h
	mov	bl,10h
	mov	cx,cs
	mov	edx,offs _int10		; INT 10h
	int	31h
	mov	ax,0205h
	mov	bl,33h
	mov	cx,cs
	mov	edx,offs _int33		; INT 33h
	int	31h


	mov	ax,0204h		; get IRQ vectors
	mov	bl,09h
	int	31h
	mov	_old_int9sel,ecx
	mov	_old_int9off,edx
	mov	bl,0Ah
	int	31h
	mov	_old_intAsel,ecx
	mov	_old_intAoff,edx
	mov	ax,0204h
	mov	bl,0Bh
	int	31h
	mov	_old_intBsel,ecx
	mov	_old_intBoff,edx
	mov	ax,0204h
	mov	bl,0Ch
	int	31h
	mov	_old_intCsel,ecx
	mov	_old_intCoff,edx
	mov	ax,0204h
	mov	bl,0Dh
	int	31h
	mov	_old_intDsel,ecx
	mov	_old_intDoff,edx
	mov	ax,0204h
	mov	bl,0Eh
	int	31h
	mov	_old_intEsel,ecx
	mov	_old_intEoff,edx

;	mov	ax,0205h
;	mov	cx,cs
;	mov	bl,9
;	mov	edx,offs _int09
;	int	31h

	mov	ax,0203h		; install INT handlers
	mov	cx,cs
	mov	bl,0
	mov	edx,offs _int00		; divide by zero
	int	31h
	mov	bl,1
	mov	edx,offs _int01		; INT 01h (trap)
	int	31h
	mov	bl,3
	mov	edx,offs _int03		; INT 03h (breakpnt)
	int	31h
	mov	bl,4
	mov	edx,offs _int04		; INTO
	int	31h
	mov	bl,5
	mov	edx,offs _int05		; bound
	int	31h
	mov	bl,6
	mov	edx,offs _int06		; invalid op
	int	31h
	mov	bl,0Ah
	mov	edx,offs _int0A
	int	31h
	mov	bl,0Bh
	mov	edx,offs _int0B
	int	31h
	mov	bl,0Ch
	mov	edx,offs _int0C
	int	31h
	mov	bl,0Dh
	mov	edx,offs _int0D
	int	31h
	mov	bl,0Eh
	mov	edx,offs _int0E
	int	31h

@@dpmi:	clr	eax			; initialize DEBUG REGISTERS
	mov	dr0,eax		; clear DR0-DR7: linear addresses
	mov	dr1,eax
	mov	dr2,eax
	mov	dr3,eax
	mov	dr6,eax		; clear DR6: bits T,S,D and breakpoint matches
	mov	dr7,eax		; setup DR7: disable all breakpoints
	mov	_dr0,eax
	mov	_dr1,eax
	mov	_dr2,eax
	mov	_dr3,eax
	mov	_dr7,eax
	xor	wptr _dr7,0300h
	mov	old_video_mode,ax
	mov	_data_addr,eax
	mov	_stack_addr,eax
	mov	_current_window,eax
	mov	eax,_eip		; setup variables
	mov	_code_addr,eax
	mov	_data_override,3
	mov	_cpu_xpos,1
	mov	_cpu_ypos,1
	mov	_cpu_xptr,0
	mov	_mem_xpos,14
	mov	_mem_ypos,39
	mov	_stk_xpos,62
	mov	_stk_ypos,30
	mov	_reg_xpos,1
	mov	_reg_ypos,32
	mov	_reg_ldat,12
	mov	_reg_xptr,0
	mov	__buffer,offset __screen2
	mov	_system_block,offset __screen
	mov	_vgastate_buf,offset __vgastatebuf
	mov	_svgastate_buf,offset __svgastatebuf
	mov	_vgastate_set,0			; set no VGA save/restore
	mov	_svgastate_set,0		; set no SVGA save/restore

	mov	ax,1C00h
	mov	cx,0007h
	int	10h
	cmp	al,1Ch
	jnz	@@verr
	cmp	ebx,4096/64
	ja	@@verr
	mov	_vgastate_set,1

	mov	ax,4F04h
	mov	cx,000Fh
	mov	dx,0000h
	int	10h
	cmp	ax,004Fh
	jnz	@@verr
	cmp	ebx,4096/64
	ja	@@verr
	mov	_svgastate_set,1

@@verr:	mov	byte ptr ds:@debug@,90h	; replace RET with NOP

	pop	gs fs ss es ds		; return to caller
	add	esp,4
	sti
	popad
	popfd
	ret




;-----------------------------------------------------------------------------
_exit:	cli
	cld
	mov	al,36h			; reprogram PIT to DOS freq
	out	43h,al
	mov	al,00h
	out	40h,al
	out	40h,al
	clr	eax
	mov	dr6,eax
	mov	dr7,eax			; reset hardware breakpoints
	mov	ds,cs:__datasel
	mov	es,cs:__datasel
	mov	ss,cs:__datasel
	mov	esp,cs:__savedstack
	and	old_video_mode,7F7Fh
	cmp	old_video_mode,0003h
	jnz	@@l1
	call	restore_video		; raw exit to DOS
	call	restore_state
	xor	eax,eax
	int	33h
	mov	ax,4C00h
	int	21h
@@l1:	mov	ax,0003h
	int	10h
	call	restore_state
	xor	eax,eax
	int	33h
	mov	ax,4C00h
	int	21h
	jmp	$

;-----------------------------------------------------------------------------
_abort:	cli
	cld
	mov	ax,4CFFh
	int	21h
	jmp	$



;-----------------------------------------------------------------------------
	Align 4
_int21:	cld
	push	ds es
	pushad

	cmp	cs:__history_bufon,0
	jz	@@l1
	mov	dx,cs
	mov	ax,[esp+2Ch]
	cmp	ax,dx
	jz	@@l1
	cmp	ax,cs:__kernel_codesel
	jz	@@l1
	cmp	ax,cs:__client_codesel
	jz	@@l1

	mov	ds,cs:__datasel
	mov	edx,__history_bufptr
	imul	edx,40h
	cmp	edx,__history_bufsize
	jb	@@1
	xor	edx,edx
	mov	__history_bufptr,edx
@@1:	add	edx,__history_bufbase
	mov	eax,__history_bufnum
	inc	__history_bufnum
	inc	__history_bufptr
	mov	[edx+00h],ax		; 00h - NUM
	mov	ax,0021h
	mov	[edx+02h],ax		; 02h - INT
	mov	eax,[esp+20h+8+8]
	mov	[edx+04h],eax		; 04h - EFL
	mov	eax,[esp+20h+8+0]
	sub	eax,2
	mov	[edx+08h],eax		; 08h - EIP
	mov	eax,[esp+1Ch]
	mov	[edx+0Ch],eax		; 0Ch - EAX
	mov	eax,[esp+10h]
	mov	[edx+10h],eax		; 10h - EBX
	mov	eax,[esp+18h]
	mov	[edx+14h],eax		; 14h - ECX
	mov	eax,[esp+14h]
	mov	[edx+18h],eax		; 18h - EDX
	mov	eax,[esp+04h]
	mov	[edx+1Ch],eax		; 1Ch - ESI
	mov	eax,[esp+00h]
	mov	[edx+20h],eax		; 20h - EDI
	mov	eax,[esp+08h]
	mov	[edx+24h],eax		; 24h - EBP
	mov	eax,esp
	add	eax,20h+14h
	mov	[edx+28h],eax		; 28h - ESP
	mov	ax,[esp+20h+8+4]
	mov	[edx+2Ch],ax		; 2Ch - CS
	mov	ax,[esp+24h]
	mov	[edx+30h],ax		; 30h - DS
	mov	[edx+32h],es		; 32h - ES
	mov	[edx+34h],ss		; 34h - SS
	mov	[edx+36h],fs		; 36h - FS
	mov	[edx+38h],gs		; 38h - GS

	mov	ds,[esp+24h]
@@l1:	popad
	pushad
	cmp	ah,4Ch
	jz	@__4Ch
	popad
	pop	es ds
	jmp	fword ptr cs:_old_int21off


@__4Ch:	mov	ax,cs
	cmp	ax,[esp+2Ch]
	jnz	_terminated
	mov	ax,0205h
	mov	bl,21h
	mov	ecx,cs:_old_int21sel
	mov	edx,cs:_old_int21off
	int	31h
	mov	bl,31h
	mov	ecx,cs:_old_int31sel
	mov	edx,cs:_old_int31off
	int	31h
	mov	bl,10h
	mov	ecx,cs:_old_int10sel
	mov	edx,cs:_old_int10off
	int	31h
	mov	bl,33h
	mov	ecx,cs:_old_int33sel
	mov	edx,cs:_old_int33off
	int	31h
	popad
	pop	es ds
	jmp	fword ptr cs:_old_int21off

_terminated:
	mov	ds,cs:__datasel
	mov	__history_bufon,0
	mov	eax,[esp+20h+8+0]
	sub	eax,2
	mov	[esp+20h+8+0],eax
	popad
	pop	es ds

	push	es ss ds fs gs
	pushad
	call	save_state
	mov	al,4Ch
	jmp	_exception


;-----------------------------------------------------------------------------
	Align 4
_int31:	cld
	push	ds es
	pushad

	cmp	cs:__history_bufon,0
	jz	@@l1
	mov	dx,cs
	mov	ax,[esp+2Ch]
	cmp	ax,dx
	jz	@@l1
	cmp	ax,cs:__kernel_codesel
	jz	@@l1
	cmp	ax,cs:__client_codesel
	jz	@@l1

	mov	ds,cs:__datasel
	mov	edx,__history_bufptr
	imul	edx,40h
	cmp	edx,__history_bufsize
	jb	@@1
	xor	edx,edx
	mov	__history_bufptr,edx
@@1:	add	edx,__history_bufbase
	mov	eax,__history_bufnum
	inc	__history_bufnum
	inc	__history_bufptr
	mov	[edx+00h],ax		; 00h - NUM
	mov	ax,0031h
	mov	[edx+02h],ax		; 02h - INT
	mov	eax,[esp+20h+8+8]
	mov	[edx+04h],eax		; 04h - EFL
	mov	eax,[esp+20h+8+0]
	sub	eax,2
	mov	[edx+08h],eax		; 08h - EIP
	mov	eax,[esp+1Ch]
	mov	[edx+0Ch],eax		; 0Ch - EAX
	mov	eax,[esp+10h]
	mov	[edx+10h],eax		; 10h - EBX
	mov	eax,[esp+18h]
	mov	[edx+14h],eax		; 14h - ECX
	mov	eax,[esp+14h]
	mov	[edx+18h],eax		; 18h - EDX
	mov	eax,[esp+04h]
	mov	[edx+1Ch],eax		; 1Ch - ESI
	mov	eax,[esp+00h]
	mov	[edx+20h],eax		; 20h - EDI
	mov	eax,[esp+08h]
	mov	[edx+24h],eax		; 24h - EBP
	mov	eax,esp
	add	eax,20h+14h
	mov	[edx+28h],eax		; 28h - ESP
	mov	ax,[esp+20h+8+4]
	mov	[edx+2Ch],ax		; 2Ch - CS
	mov	ax,[esp+24h]
	mov	[edx+30h],ax		; 30h - DS
	mov	[edx+32h],es		; 32h - ES
	mov	[edx+34h],ss		; 34h - SS
	mov	[edx+36h],fs		; 36h - FS
	mov	[edx+38h],gs		; 38h - GS

	mov	ds,[esp+24h]
@@l1:	popad
	pushad

	popad
	pop	es ds
	jmp	fword ptr cs:_old_int31off



;-----------------------------------------------------------------------------
	Align 4
_int10:	cld
	push	ds es
	pushad

	cmp	cs:__history_bufon,0
	jz	@@l1
	mov	dx,cs
	mov	ax,[esp+2Ch]
	cmp	ax,dx
	jz	@@l1
	cmp	ax,cs:__kernel_codesel
	jz	@@l1
	cmp	ax,cs:__client_codesel
	jz	@@l1

	mov	ds,cs:__datasel
	mov	edx,__history_bufptr
	imul	edx,40h
	cmp	edx,__history_bufsize
	jb	@@1
	xor	edx,edx
	mov	__history_bufptr,edx
@@1:	add	edx,__history_bufbase
	mov	eax,__history_bufnum
	inc	__history_bufnum
	inc	__history_bufptr
	mov	[edx+00h],ax		; 00h - NUM
	mov	ax,0010h
	mov	[edx+02h],ax		; 02h - INT
	mov	eax,[esp+20h+8+8]
	mov	[edx+04h],eax		; 04h - EFL
	mov	eax,[esp+20h+8+0]
	sub	eax,2
	mov	[edx+08h],eax		; 08h - EIP
	mov	eax,[esp+1Ch]
	mov	[edx+0Ch],eax		; 0Ch - EAX
	mov	eax,[esp+10h]
	mov	[edx+10h],eax		; 10h - EBX
	mov	eax,[esp+18h]
	mov	[edx+14h],eax		; 14h - ECX
	mov	eax,[esp+14h]
	mov	[edx+18h],eax		; 18h - EDX
	mov	eax,[esp+04h]
	mov	[edx+1Ch],eax		; 1Ch - ESI
	mov	eax,[esp+00h]
	mov	[edx+20h],eax		; 20h - EDI
	mov	eax,[esp+08h]
	mov	[edx+24h],eax		; 24h - EBP
	mov	eax,esp
	add	eax,20h+14h
	mov	[edx+28h],eax		; 28h - ESP
	mov	ax,[esp+20h+8+4]
	mov	[edx+2Ch],ax		; 2Ch - CS
	mov	ax,[esp+24h]
	mov	[edx+30h],ax		; 30h - DS
	mov	[edx+32h],es		; 32h - ES
	mov	[edx+34h],ss		; 34h - SS
	mov	[edx+36h],fs		; 36h - FS
	mov	[edx+38h],gs		; 38h - GS

	mov	ds,[esp+24h]
@@l1:	popad
	pushad

	popad
	pop	es ds
	jmp	fword ptr cs:_old_int10off





;-----------------------------------------------------------------------------
	Align 4
_int33:	cld
	push	ds es
	pushad

	cmp	cs:__history_bufon,0
	jz	@@l1
	mov	dx,cs
	mov	ax,[esp+2Ch]
	cmp	ax,dx
	jz	@@l1
	cmp	ax,cs:__kernel_codesel
	jz	@@l1
	cmp	ax,cs:__client_codesel
	jz	@@l1

	mov	ds,cs:__datasel
	mov	edx,__history_bufptr
	imul	edx,40h
	cmp	edx,__history_bufsize
	jb	@@1
	xor	edx,edx
	mov	__history_bufptr,edx
@@1:	add	edx,__history_bufbase
	mov	eax,__history_bufnum
	inc	__history_bufnum
	inc	__history_bufptr
	mov	[edx+00h],ax		; 00h - NUM
	mov	ax,0033h
	mov	[edx+02h],ax		; 02h - INT
	mov	eax,[esp+20h+8+8]
	mov	[edx+04h],eax		; 04h - EFL
	mov	eax,[esp+20h+8+0]
	sub	eax,2
	mov	[edx+08h],eax		; 08h - EIP
	mov	eax,[esp+1Ch]
	mov	[edx+0Ch],eax		; 0Ch - EAX
	mov	eax,[esp+10h]
	mov	[edx+10h],eax		; 10h - EBX
	mov	eax,[esp+18h]
	mov	[edx+14h],eax		; 14h - ECX
	mov	eax,[esp+14h]
	mov	[edx+18h],eax		; 18h - EDX
	mov	eax,[esp+04h]
	mov	[edx+1Ch],eax		; 1Ch - ESI
	mov	eax,[esp+00h]
	mov	[edx+20h],eax		; 20h - EDI
	mov	eax,[esp+08h]
	mov	[edx+24h],eax		; 24h - EBP
	mov	eax,esp
	add	eax,20h+14h
	mov	[edx+28h],eax		; 28h - ESP
	mov	ax,[esp+20h+8+4]
	mov	[edx+2Ch],ax		; 2Ch - CS
	mov	ax,[esp+24h]
	mov	[edx+30h],ax		; 30h - DS
	mov	[edx+32h],es		; 32h - ES
	mov	[edx+34h],ss		; 34h - SS
	mov	[edx+36h],fs		; 36h - FS
	mov	[edx+38h],gs		; 38h - GS

	mov	ds,[esp+24h]
@@l1:	popad
	pushad

	popad
	pop	es ds
	jmp	fword ptr cs:_old_int33off



_critical_error_handler:
	push	es ss ds fs gs
	pushad
	call	save_state
	mov	eax,cs:_eax
	push	ds es ecx esi edi
	mov	ds,cs:__datasel
	mov	es,cs:__datasel
	mov	esi,offs _old_registers
	mov	edi,offs _registers
	mov	ecx,19
	rep	movsd
	pop	edi esi ecx es ds
	jmp	_exception

	Align 4
__datasel	dw 0
__zerosel	dw 0
__savedstack	dd 0





.DATA
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
__cputype	db 0
__fputype	db 0
__buffer	dd 0
__exec_handle	dw 0
__exec_start	dd 0
__free_mem	dd 0
__base_mem	dd 0
		evendata
_old_int10off	dd 0
_old_int10sel	dd 0
_old_int21off	dd 0
_old_int21sel	dd 0
_old_int31off	dd 0
_old_int31sel	dd 0
_old_int33off	dd 0
_old_int33sel	dd 0

__idstring	db 'SUNSYS DOS/32A',0
__message0	db 'SD/32A -- Protected Mode Debugger  '
	 db 'Version 7.2',cr
	 db 'Copyright (C) Supernar Systems, Ltd. 1996-2002',cre
__error0 db 'SD/32A fatal: DOS/32 Advanced ADPMI is required!',cr
	 db cr
db 'Note that this program can not be run under Windows or OS/2 systems',cr
db 'as they have an internal DPMI server which runs the protected mode.',cr
db 'If you are running SUNSYS Debugger under one of these systems, or under',cr
db 'any other, external (relative to DOS/32A) DPMI host, please exit to the',cr
db 'real-mode DOS prompt and restart the program from there.',cre

__error1 db 'SD/32A fatal: syntax is SD <execname.xxx>',cre
__error2 db 'SD/32A fatal: internal exception handling must be enabled',cre
__error3 db 'SD/32A fatal: could not open application file exec',cre
__error4 db 'SD/32A fatal: error in application file exec',cre
__error5 db 'SD/32A fatal: application file exec format not supported',cre
__error6 db 'SD/32A fatal: 80486 processor or better required',cre



.DATA?
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

__screen	db 0C000h dup(?)	; allocate memory for user screen
__screen2	db 10000h dup(?)
__vgastatebuf	db 4096 dup(?)
__svgastatebuf	db 4096 dup(?)


.STACK
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
		db 4096 dup(?)

include	sloader.asm
include	sdebug.asm

end	start32
