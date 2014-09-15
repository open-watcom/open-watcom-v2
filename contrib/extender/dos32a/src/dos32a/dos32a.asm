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

_ID32	segment para public use16 'CODE0'
_ID32	ends
_KERNEL	segment para public use16 'CODE1'
_KERNEL	ends
_TEXT16	segment para public use16 'CODE2'
_TEXT16	ends
_STACK	segment para stack use16 'STACK'
_STACK	ends

extrn	pm32_info	:far
extrn	pm32_init	:far
extrn	pm32_data	:byte

include	TEXT\include.asm


		.386p
		LOCALS
		NOJUMPS
;=============================================================================
_ID32	segment para public use16 'CODE0'
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
include	TEXT\oemtitle.asm
_ID32	ends









;=============================================================================
_TEXT16	segment para public use16 'CODE2'
	assume  cs:_TEXT16, ds:_TEXT16
	org 0

@text16_beg	label byte

include	TEXT\CLIENT\config.asm
include	TEXT\CLIENT\strings.asm
include	TEXT\CLIENT\misc.asm
include	TEXT\CLIENT\debug.asm
include	TEXT\CLIENT\int10h.asm
include	TEXT\CLIENT\int21h.asm
include	TEXT\CLIENT\int33h.asm
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
	jnc	@@1
@@err1:	neg	si
	mov	cl,6
	shr	si,cl			; convert para to KB
	mov	ax,1001h
	jmp	report_error
@@err2:	mov	ax,1002h
	jmp	report_error

@@1:	sub	ax,_seg_es		; (SS+STK+INT+DOS)-ES	*NOTE*: no BUF
	mov	bx,ax			; resize DOS memory
	mov	ah,4Ah
	int	21h
	jc	@@err2
	call	far ptr pm32_info	; prepare and check for errors
	jnc	@@2			; if error had occured, AX=error code
	jmp	report_error		; exit with error message
;
; No errors had occured, initialize kernel
;
	.386p
@@2:	mov	wptr _buf_size,bx
	mov	wptr _cpu_type,cx

	call	remove_kernel

	mov	ax,_membase		; do second check for available memory
	mov	si,es:[0002h]		; (not really needed)
	add	ax,bx			; AX=base of free mem after PMbuf
	sub	si,ax
	jc	@@err1
	add	bx,_membase		; resize DOS memory
	sub	bx,_seg_es		; (SS+STK+INT+DOS+BUF)-ES
	mov	ah,4Ah
	int	21h
	jc	@@err2
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
	push	di ecx
	call	init_system		; setup PM interrupts/buffers
	call	save_inttab		; now we can safely save inttab
	call	verbose_showsys
	pop	ecx di
	sti

	call	check_system		; check for, and issue any warnings
	mov	fs,_sel_ss		; FS: = selector of buffer (and stack)
	mov	gs,_sel_zero		; GS: = zero selector
	call	open_exec		; open executable file
	call	load_exec_header	; load exec header into memory
	call	check_command_line	; get command line info
	cmp	dx,0040h		; check if reloc-tab starts at 0040h
	jnz	@@6			; if not, check command line for fname
	mov	edx,_exec_start		; get start of exec
	test	dx,dx			; check if application is bound
	jnz	load_bound_app		; if bound, load BOUND Application
@@6:	test	si,si			; check if command line is not empty
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
	jnz	@@1
	cmp	ax,'EL'			; 'LE' type
	jz	load_le_app
	cmp	ax,'XL'			; 'LX' type
	jz	load_lx_app
	cmp	ax,'CL'			; 'LC' type (Linear Compressed)
	jz	load_lc_app
	cmp	ax,'EP'			; 'PE' type
	jz	load_pe_app
@@1:	call	close_exec
	mov	ax,3004h		; "app exec format not supported"
	jmp	file_error


;=============================================================================
; Jump to loaded 32-bit code
;
enter_32bit_code:
	test	cs:_misc_byte2,00010000b
	jz	@@0

	sti
	mov	al,'>'			; "press any key to continue..."
	call	printc
	xor	ax,ax			; wait for a keypress
	int	16h
	call	printcr

@@0:	cli				; disable interrupts
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
@@0:	int	31h
	mov	wptr _exc_tab[ebx*8+4],cx
	mov	dptr _exc_tab[ebx*8+0],edx
	inc	bl
	cmp	bl,15
	jb	@@0

	call	install_client_ints	; install client PM interrupts
	jc	dpmi_error

	call	win_focus_vm		; switch to full-screen under Windows

	cmp	_sys_type,3
	jz	@@done
	cmp	_process_id,0		; do not reset PIT if we've been
	jnz	@@done			;  spawned to avoid timing problems

	call	restore_pit

@@done:	ret


;=============================================================================
check_system:
	test	di,di			; if no prev. DOS/32A in system
	jz	@@1			; then jump
	cmp	di,_version		; check DOS/32A versions
	jz	@@1			; if the same then jump
	mov	ax,9006h		; "incompatible version of DOS/32A"
	call	report_error
@@1:	cmp	_sys_type,3		; if running under an external DPMI
	jz	@@2			; then jump
	test	ecx,ecx			; if extended memory has been alloced
	jnz	@@2			; then jump
	mov	ax,9001h		; "no ext mem has been allocated"
	call	report_error
@@2:	mov	ax,0400h		; get DPMI info
	int	31h
	cmp	dh,08h			; warn if PICs have been remapped
	jnz	@@3
	cmp	dl,70h
	jz	@@4
@@3:	movzx	si,dh
	movzx	di,dl
	mov	ax,9002h		; "PICs have been relocated"
	call	report_error
@@4:	ret


;=============================================================================
remove_kernel:
	cmp	_sys_type,3
	jnz	@@done

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
@@done:	ret





;=============================================================================
; DATA

include	TEXT\CLIENT\data.asm


;=============================================================================
; BETA test code

If EXEC_TYPE eq 2
include	TEXT\testbeta.asm
Endif

	Align 16
@text16_end	label byte
_TEXT16	ends



;=============================================================================
; STACK
_STACK	segment para stack use16 'STACK'
	db	STACKSIZE*16 dup(?)
_STACK	ends
end start
