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

BUILDING_KERNEL = 0
BUILDING_CLIENT = 1

;*****************************************************************************
; DOS/32 Advanced DOS Extender master Client file, implements program entry
; point and includes the necessary client files from .\TEXT\CLIENT\.
;
;*****************************************************************************

STACKSIZE	=	0080h	; size of stack (in para)
INTTABSIZE	=	0040h	; size of copy of interrupt table (in para)

_ID32	segment para public 'CODE0'
_ID32	ends
_KERNEL	segment para public 'CODE1'
_KERNEL	ends
_TEXT16	segment para public 'CODE2'
_TEXT16	ends
_STACK	segment para stack 'STACK'
_STACK	ends

extrn	pm32_info	:far
extrn	pm32_init	:far
extrn	pm32_data	:byte
extrn	pm32_psp_seg	:word
extrn	dos_ver_major	:byte

include	text/include.asm


.8086
; LOCALS
; NOJUMPS
;=============================================================================
_ID32	segment para public 'CODE0'
_ID32_SIZE=16		; size excluding 'ID32' signature
;-----------------------------------------------------------------------------
db	'ID32'		; ID signature
db	00111111b	; KERNEL misc. bits:
			;   bit 0: **deprecated**
			;   bit 1: **deprecated**
			;   bit 2: 0=VCPI smart page alloc off, 1=on	/1=def
			;   bit 3: 0=VCPI+XMS alloc scheme off, 1=on	/1=def
			;   bit 4: **deprecated**
			;   bit 5: **deprecated**
			;   bit 6: reserved				/0=def
			;   bit 7: 0=ignore 4G extensions off, 1=on	/0=def
db	64		; Max. number of page tables under VCPI		/256MB
db	2		; Max. number of page tables mem_mapping	/4MB
db	16		; Max. number of real mode callbacks		/16
dw	256		; Max. number of Selectors under VCPI/XMS/raw	/256
db	8		; Real mode Stack nesting
db	8		; Protected mode Stack nesting
dw	20h		; Real mode Stack length, (in para)
dw	20h		; Protected mode Stack length, (in para)
dd	0FFFFFFFFh	; Max. extended memory to allocate in bytes
;-----------------------------------------------------------------------------
db	00111111b	; DOS/32A misc. bits:
			;   bit 0: 0=console output off, 1=on		/1=def
			;   bit 1: 0=sound generation off, 1=on		/1=def
			;   bit 2: 0=restore INT table off, 1=on	/1=def
			;   bit 3: 0=report modified INTs off, 1=on	/1=def
			;   bit 4: 0=load 16 in lowmem off, 1=on	/1=def
			;   bit 5: 0=force load 16 low off, 1=on	/1=def
			;   bit 6: 0=cls on exception off, 1=on		/0=def
			;   bit 7: 0=null-ptr protect off, 1=on		/0=def
If EXEC_TYPE eq 0
db	00001001b	; DOS/32A Pro second misc. bits
Else
db	10001001b	; DOS/32A Beta second misc. bits
Endif
			;   bit 0: 0=config by enironment off, 1=on	/1=def
			;   bit 1: 0=focus on this VM off, 1=on		/0=def
			;   bit 2: 0=align objects on PARA, 1=PAGE	/0=def
			;   bit 3: 0=show copyright off, 1=on		/1=def
			;   bit 4: 0=verbose mode off, 1=on		/0=def
			;   bit 5: reserved				/0=def
			;   bit 6: 0=lock configuration off, 1=on	/*
			;   bit 7: 0=Professional, 1=Beta		/*
dw	0200h		; DOS INT 21h buffer in low memory (in para)	/8 KB
dw	090Ch		; Internal Version of DOS/32A: db low,high
dw	0000h		; Reserved (v7.0+)
;-----------------------------------------------------------------------------
include	text/oemtitle.asm
	org	007Eh
_mz_reloc_pad	dw 0		; scratch word for 512-byte MZ header relocation padding
_ID32	ends









;=============================================================================
_TEXT16	segment para public 'CODE2'
	assume  cs:_TEXT16, ds:_TEXT16
	.386p
	.387
	org 0

@text16_beg	label byte

; Forward symbol sizes for two-pass WASM compatibility
externdef _dos_ver_major:byte
externdef _using_synth_env:byte
externdef _default_self:byte
externdef _synth_env:byte
externdef dfn_tab:word
externdef g_errtab:byte
externdef h_errtab:word
externdef errtab_00xx:word
externdef errtab_10xx:word
externdef errtab_20xx:word
externdef errtab_30xx:word
externdef errtab_40xx:word
externdef errtab_60xx:word
externdef errtab_80xx:word
externdef errtab_90xx:word
externdef i_msg:word
externdef _misc_byte:byte
externdef _misc_byte2:byte
externdef _version:word
externdef _cpu_type:byte
externdef _sys_type:byte
externdef _sys_misc:word
externdef _buf_size:word
externdef _pic_mask:word
externdef _err_code:word
externdef _acc_rights:word
externdef _mus_size:dword
externdef _mus_backoff:word
externdef _mus_backseg:word
externdef _mus_off:dword
externdef _mus_sel:dword
externdef _mus_data:byte
externdef _seg_ds:word
externdef _seg_es:word
externdef _seg_ss:word
externdef _seg_env:word
externdef _seg_buf:word
externdef _seg_dta:word
externdef _seg_mus:word
externdef _seg_id32:word
externdef _seg_kernel:word
externdef _sel_cs:word
externdef _sel_ds:word
externdef _sel_es:word
externdef _sel_esp:dword
externdef _sel_ss:word
externdef _sel_zero:word
externdef _sel_env:word
externdef _process_id:word
externdef _sel32_cs:word
externdef _sel32_ss:word
externdef _membase:word
externdef _lowmembuf:word
externdef _lobufzero:dword
externdef _lobufbase:dword
externdef _lobufsize:dword
externdef _seg_ds_base:dword
externdef _dta_sel:word
externdef _dta_off:dword
externdef _app_dta_sel:word
externdef _app_dta_off:dword
externdef _int10_ip:dword
externdef _int10_cs:dword
externdef _int21_ip:dword
externdef _int21_cs:dword
externdef _int23_ip:dword
externdef _int23_cs:dword
externdef _int33_ip:dword
externdef _int33_cs:dword
externdef _exc_tab:dword
externdef n_msg:byte
externdef cpr_msg:byte
externdef ver_msg:byte
externdef cpr_end:byte
externdef errmsg1:byte
externdef errmsg2:byte
externdef errmsg3:byte
externdef dos_str:byte
externdef df1_str:byte
externdef df2_str:byte
externdef df3_str:byte
externdef df4_str:byte
externdef df5_str:byte
externdef df6_str:byte
externdef df8_str:byte
externdef df9_str:byte
externdef dfA_str:byte
externdef dfB_str:byte
externdef dfC_str:byte
externdef @area1_db:byte
externdef @area1_dw:word
externdef @area1_dd:dword
externdef d_err0:byte
externdef d_err1:byte
externdef d_err2:byte
externdef d_err3:byte
externdef d_err4:byte
externdef d_err5:byte
externdef d_err6:byte
externdef d_err7:byte
externdef m_err1:byte
externdef m_err2:byte
externdef e_err0:byte
externdef e_err1:byte
externdef e_err2:byte
externdef a_err1:byte
externdef a_err2:byte
externdef a_err3:byte
externdef a_err4:byte
externdef l_err1:byte
externdef l_err2:byte
externdef l_err3:byte
externdef l_err4:byte
externdef l_err5:byte
externdef l_err6:byte
externdef l_err7:byte
externdef x_err1:byte
externdef x_err2:byte
externdef x_err3:byte
externdef w_msg1:byte
externdef w_msg2:byte
externdef w_msg3:byte
externdef w_msg4:byte
externdef w_msg5:byte
externdef w_msg6:byte
externdef r_msg80:byte
externdef r_msg81:byte
externdef r_msg82:byte
externdef r_msg83:byte
externdef r_msg84:byte
externdef v_msg01:byte
externdef v_msg02:byte
externdef v_msg03:byte
externdef v_msg04:byte
externdef v_msg05:byte
externdef v_msg10:byte
externdef v_msg11:byte
externdef v_msg12:byte
externdef excmsgE:byte
externdef excmsgI:byte
externdef excmsg1:byte
externdef excmsg2:byte
externdef excmsg4:byte
externdef excmsgA:byte
externdef excmsgB:byte
externdef excmsgC:byte
externdef nulmsg0:byte
externdef dbgmsg1:byte
externdef dbgmsg2:byte
externdef dbgmsg4:byte
externdef dbgmsg5:byte
externdef dbgmsg6:byte
externdef dbgmsg7:byte
externdef dbgmsgA:byte
externdef dbgmsgB:byte
externdef selmsg1:byte
externdef selmsg2:byte
externdef selmsg3:byte
externdef selmsg4:byte
externdef selmsg5:byte
externdef selmsg6:byte
externdef selmsg0:byte
externdef selmsg9:byte
externdef selmsg8:byte
externdef selmsgX:byte
externdef selmsgY:byte
externdef selmsgZ:byte
externdef selmsgW:byte
externdef i_msg00:byte
externdef i_msg01:byte
externdef i_msg02:byte
externdef i_msg03:byte
externdef i_msg04:byte
externdef i_msg05:byte
externdef i_msg06:byte
externdef i_msg07:byte
externdef i_msg08:byte
externdef i_msg09:byte
externdef i_msg0A:byte
externdef i_msg0B:byte
externdef i_msg0C:byte
externdef i_msg0D:byte
externdef i_msg0E:byte
APP_MAXOBJECTS = 64
_exec_handle		equ @area1_dw+00h		;dw 0
_exec_start		equ @area1_dd+04h		;dd 0
_app_tmp_addr1		equ @area1_dd+08h		;dd 0
_app_tmp_addr2		equ @area1_dd+0Ch		;dd 0
_app_num_objects	equ @area1_dd+10h		;dd 0
_app_off_objects	equ @area1_dd+14h		;dd 0
_app_off_objpagetab	equ @area1_dd+18h		;dd 0
_app_off_fixpagetab	equ @area1_dd+1Ch		;dd 0
_app_off_fixrectab	equ @area1_dd+20h		;dd 0
_app_off_datapages	equ @area1_dd+24h		;dd 0
_app_off_pageshift	equ @area1_dd+28h		;dd 0
_app_eip_object		equ @area1_dd+2Ch		;dd 0
_app_esp_object		equ @area1_dd+30h		;dd 0
_app_eip		equ @area1_dd+34h		;dd 0
_app_esp		equ @area1_dd+38h		;dd 0
_unreloc_eip		equ @area1_dd+3Ch		;dd 0
_unreloc_esp		equ @area1_dd+40h		;dd 0
_app_siz_fixrecstab	equ @area1_dd+44h		;dd 0
_app_buf_fixrecstab	equ @area1_dd+48h		;dd 0
_app_siz_lastpage	equ @area1_dd+4Ch		;dd 0
_int_number		equ @area1_db+50h		;db 0,0
_int_temp		equ @area1_dw+52h		;dw 0
_int_errcode		equ @area1_dd+54h		;dd 0
_int_esp		equ @area1_dd+58h		;dd 0
_int_ss			equ @area1_dd+5Ch		;dw 0
_int_erravail		equ @area1_db+5Eh		;db 0
_app_type		equ @area1_db+60h		;db 0
_app_load		equ @area1_db+61h		;db 0
_mus_esp		equ @area1_dd+64h		;dd 0
_mus_ss			equ @area1_dw+68h		;dw 0
ms_reg_cr0		equ @area1_dd+ 80h		;dd 0
ms_reg_cr2		equ @area1_dd+ 84h		;dd 0
ms_reg_cr3		equ @area1_dd+ 88h		;dd 0
ms_reg_dr6		equ @area1_dd+ 8Ch		;dd 0
ms_eflags		equ @area1_dd+ 90h		;dd 0
ms_ec			equ @area1_dd+ 94h		;dd 0
ms_frame_esp		equ @area1_dd+0A0h		;dd 0	; exception handler's stack frame: ESP
ms_frame_ss		equ @area1_dw+0A4h		;dw 0	; exception handler's stack frame: SS
ms_reg_ds		equ @area1_dw+0A8h		;dw 0
ms_int			equ @area1_dw+0ACh		;dw 0	; INT #
_app_buf_allocsel	equ @area2_dw+00h		;dw 0
_app_buf_allocbase	equ @area2_dd+APP_MAXOBJECTS*2	;dd APP_MAXOBJECTS*2 dup(00h)

include	text/client/config.asm
include	text/client/strings.asm
include	text/client/misc.asm
include	text/client/debug.asm
include	text/client/int10h.asm
include	text/client/int21h.asm
include	text/client/int33h.asm
include	loader.asm
include	loadlc.asm
include	loadpe.asm


	.8086
	Align 4

start:	push	cs			; DS = CS
	pop	ds
	mov	_seg_ds,ds		; save SEG regs
	mov	_seg_es,es
	mov	_seg_ss,ss
	mov	ax,es:[002Ch]
	mov	_seg_env,ax
	sti
	cld

If DOS_LEGACY_125_20 ne 0
	mov	ah,30h			; detect DOS major version and cache startup PSP
	int	21h
	test	al,al
	jnz	@start_ver_ok
	mov	al,1
@start_ver_ok:
	mov	_dos_ver_major,al
	push	es
	mov	dx,_KERNEL
	mov	es,dx
	mov	dx,_seg_es
	mov	es:pm32_psp_seg,dx
	pop	es
	cmp	al,2			; DOS < 2.0 has no environment block; synthesize valid block
	jb	@start_synth_env
	cmp	wptr _seg_env,0
	jnz	@start_env_ready
@start_synth_env:
	mov	ax,offs _synth_env
	mov	cl,4
	shr	ax,cl
	add	ax,_seg_ds
	mov	_seg_env,ax
	mov	es:[002Ch],ax
	mov	_using_synth_env,1
@start_env_ready:
Endif

	call	get_default_config	; configure using defaults
	call	get_environ_config	; configure using environment
	call	copyright		; show copyright message
	mov	ax,ss			; get end of program
	mov	si,es:[0002h]		; get free DOS memory (in para)
	add	ax,STACKSIZE+INTTABSIZE	; stacksize+inttable (in para)
	mov	_seg_buf,ax		; set base of INT 21h buffer segment
	add	ax,_lowmembuf		; +lowbuf (in para)
	mov	_membase,ax		; set base of memory for PMbuf
	sub	si,ax			; check if enough mem
	jnc	@start_1
@start_err1:	neg	si
	mov	cl,6
	shr	si,cl			; convert para to KB
	mov	ax,1001h
	jmp	report_error
@start_err2:	mov	ax,1002h
	jmp	report_error

@start_1:	sub	ax,_seg_es		; (SS+STK+INT+DOS)-ES	*NOTE*: no BUF
	mov	bx,ax			; resize DOS memory
If DOS_LEGACY_125_20 ne 0
	cmp	_dos_ver_major,2	; DOS 1.x has no MCB allocator
	jb	@start_skip_4a_1
Endif
	mov	ah,4Ah
	int	21h
	jc	@start_err2
@start_skip_4a_1:
	call	far ptr pm32_info	; prepare and check for errors
	jnc	@start_2			; if error had occured, AX=error code
	jmp	report_error		; exit with error message
;
; No errors had occured, initialize kernel
;
	.386p
@start_2:	mov	wptr _buf_size,bx
	mov	wptr _cpu_type,cx

	call	remove_kernel

	mov	ax,_membase		; do second check for available memory
	mov	si,es:[0002h]		; (not really needed)
	add	ax,bx			; AX=base of free mem after PMbuf
	sub	si,ax
	jc	@start_err1
	add	bx,_membase		; resize DOS memory
	sub	bx,_seg_es		; (SS+STK+INT+DOS+BUF)-ES
If DOS_LEGACY_125_20 ne 0
	cmp	_dos_ver_major,2
	jb	@start_skip_4a_2
Endif
	mov	ah,4Ah
	int	21h
	jc	@start_err2
@start_skip_4a_2:
	movzx	eax,_seg_buf
	mov	edx,eax
	sub	ax,_seg_ds		; adjust for CS(sel):0000(offs)
	shl	edx,4
	shl	eax,4
	mov	_lobufbase,eax		; set INT 21h buf base relative to CS:
	mov	_lobufzero,edx		; set INT 21h buf base relative to 0
	movzx	eax,_seg_ds
	shl	eax,4
	mov	_seg_ds_base,eax	; set 32bit base of CS and DS segment
	movzx	eax,_lowmembuf
	shl	eax,4
	mov	_lobufsize,eax		; set INT 21h buffer size in bytes
	mov	es,_membase
	mov	bx,_version
	mov	dx,offs critical_handler
	call	far ptr pm32_init	; enter Protected Mode
	jc	report_error

	cli
	mov	_sel_cs,cs		; save PM selectors
	mov	_sel_ds,ds
	mov	_sel_es,es
	mov	_sel_ss,ss
	mov	_sel_esp,esp
	mov	ax,es:[002Ch]
	mov	_sel_env,ax
	mov	_process_id,si
	push	di
	push	ecx
	call	init_system		; setup PM interrupts/buffers
	call	save_inttab		; now we can safely save inttab
	call	verbose_showsys
	pop	ecx
	pop	di
	sti

	call	check_system		; check for, and issue any warnings
	mov	fs,_sel_ss		; FS: = selector of buffer (and stack)
	mov	gs,_sel_zero		; GS: = zero selector
If DOS_LEGACY_125_20 ne 0
	cmp	_dos_ver_major,3
	jae	@start_dos3_exec
	call	try_open_self_legacy
	jnc	@start_legacy_bound
	call	check_command_line
	test	si,si
	jnz	load_extrn_app
	mov	ax,8001h
	jmp	report_error
@start_legacy_bound:
	mov	edx,_exec_start
	jmp	load_bound_app
@start_dos3_exec:
Endif
	call	open_exec		; open executable file
	call	load_exec_header	; load exec header into memory
	call	check_command_line	; get command line info
	cmp	dx,0040h		; check if reloc-tab starts at 0040h
	jnz	@start_6			; if not, check command line for fname
	mov	edx,_exec_start		; get start of exec
	test	dx,dx			; check if application is bound
	jnz	load_bound_app		; if bound, load BOUND Application
@start_6:	test	si,si			; check if command line is not empty
	jnz	load_extrn_app		; if cmd != 0, load EXTRN Application
	call	close_exec		; display help message
	mov	ax,8001h
	jmp	report_error

load_extrn_app:
	call	close_exec
	call	open_extrn_exec
	call	load_extrn_exec_header
	call	update_environment
	call	remove_name_from_cmd
	mov	edx,_exec_start

load_bound_app:
	mov	_err_code,3002h		; "error in app file"
	call	seek_from_start		; move file ptr to 32bit application
	mov	ecx,4			; load 4 bytes (app signature)
	xor	edx,edx			; offset is zero
	call	load_fs_block		; load
	mov	ax,fs:[0000h]		; get file signature
	mov	bx,fs:[0002h]
	test	bx,bx
	jnz	@load_bound_app_1
	cmp	ax,'EL'			; 'LE' type
	jz	load_le_app
	cmp	ax,'XL'			; 'LX' type
	jz	load_lx_app
	cmp	ax,'CL'			; 'LC' type (Linear Compressed)
	jz	load_lc_app
	cmp	ax,'EP'			; 'PE' type
	jz	load_pe_app
@load_bound_app_1:	call	close_exec
	mov	ax,3004h		; "app exec format not supported"
	jmp	file_error


;=============================================================================
; Jump to loaded 32-bit code
;
enter_32bit_code:
	test	cs:_misc_byte2,00010000b
	jz	@enter_32bit_code_0

	sti
	mov	al,'>'			; "press any key to continue..."
	call	printc
	xor	ax,ax			; wait for a keypress
	int	16h
	call	printcr

@enter_32bit_code_0:	cli				; disable interrupts
	cld
	call	install_nullptr_protect	; install Null-Ptr Protection
	mov	ss,_sel32_ss		; SS = app 32bit data sel
	mov	esp,_app_esp		; ESP = application stack

	mov	es,_sel_es		; ES = environment sel
	mov	fs,_sel_zero		; FS = 32bit zero sel
	mov	ds,_sel32_ss		; DS = app 32bit data sel
	xor	eax,eax			; clear registers
	xor	ebx,ebx
	xor	ecx,ecx
	xor	edx,edx
	xor	esi,esi
	xor	edi,edi
	xor	ebp,ebp
	mov	gs,ax

	pushfd				; push flags
	push	dptr cs:_sel32_cs	; push 32bit destination selector
	push	dptr cs:_app_eip	; push 32bit destination offset
	or	bptr ss:[esp+9],2	; enable interrupts
	sti
	iretd





;=============================================================================
init_system:
	xor	eax,eax			; reset temp variables to zero
	mov	_app_esp,eax
	mov	_app_num_objects,eax
	mov	_app_off_datapages,eax
	call	setup_selectors		; setup system selectors
	call	setup_dta_buffer	; allocate buffer for DTA and Mouse
	call	initialize_mouse	; initialize mouse

	mov	ax,0204h		; get default PM interrupt handlers
	mov	bl,10h
	int	31h
	mov	wptr _int10_cs,cx
	mov	dptr _int10_ip,edx

	mov	bl,21h
	int	31h
	mov	wptr _int21_cs,cx
	mov	dptr _int21_ip,edx

	mov	bl,23h
	int	31h
	mov	wptr _int23_cs,cx
	mov	dptr _int23_ip,edx

	mov	bl,33h
	int	31h
	mov	wptr _int33_cs,cx
	mov	dptr _int33_ip,edx

	mov	ax,0202h		; get default PM exception handlers
	xor	ebx,ebx
@init_system_0:	int	31h
	mov	wptr _exc_tab[ebx*8+4],cx
	mov	dptr _exc_tab[ebx*8+0],edx
	inc	bl
	cmp	bl,15
	jb	@init_system_0

	call	install_client_ints	; install client PM interrupts
	jc	dpmi_error

	call	win_focus_vm		; switch to full-screen under Windows

	cmp	_sys_type,3
	jz	@init_system_done
	cmp	_process_id,0		; do not reset PIT if we've been
	jnz	@init_system_done			;  spawned to avoid timing problems

	call	restore_pit

@init_system_done:	ret


;=============================================================================
check_system:
	test	di,di			; if no prev. DOS/32A in system
	jz	@check_system_1			; then jump
	cmp	di,_version		; check DOS/32A versions
	jz	@check_system_1			; if the same then jump
	mov	ax,9006h		; "incompatible version of DOS/32A"
	call	report_error
@check_system_1:	cmp	_sys_type,3		; if running under an external DPMI
	jz	@check_system_2			; then jump
	test	ecx,ecx			; if extended memory has been alloced
	jnz	@check_system_2			; then jump
	mov	ax,9001h		; "no ext mem has been allocated"
	call	report_error
@check_system_2:	mov	ax,0400h		; get DPMI info
	int	31h
	cmp	dh,08h			; warn if PICs have been remapped
	jnz	@check_system_3
	cmp	dl,70h
	jz	@check_system_4
@check_system_3:	movzx	si,dh
	movzx	di,dl
	mov	ax,9002h		; "PICs have been relocated"
	call	report_error
@check_system_4:	ret


;=============================================================================
remove_kernel:
	cmp	_sys_type,3
	jnz	@remove_kernel_done

	cli
	pop	bp
	push	es
	mov	es,_seg_kernel
	mov	si,offs @text16_beg
	mov	cx,(offs @text16_end - @text16_beg) / 2
	rep	movsw
	pop	es
	mov	ax,dx
	shr	ax,4
	mov	dx,ss
	sub	dx,ax
	mov	ss,dx
	mov	dx,ds
	sub	dx,ax
	mov	ds,dx
	sub	_seg_ds,ax
	sub	_seg_ss,ax
	sub	_seg_buf,ax
	sub	_membase,ax
	push	dx
	push	bp
	sti
	retf
@remove_kernel_done:	ret





;=============================================================================
; DATA

include	text/client/data.asm


;=============================================================================
; BETA test code

If EXEC_TYPE eq 2
include	text/testbeta.asm
Endif

	Align 16
@text16_end	label byte
_TEXT16	ends



;=============================================================================
; STACK
_STACK	segment para stack 'STACK'
	db	STACKSIZE*16 dup(?)
_STACK	ends
end start
