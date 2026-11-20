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


.CODE

;-----------------------------------------------------------------------------
_come_here:
	mov	eax,_cpu_ypos
	dec	eax
	mov	eax,_addrbuffer[eax*4]
	mov	_dr0,eax
	and	bptr _dr7[2],0F0h		; LEN=byte, R/W=exec
	or	bptr _dr7[0],003h
	call	restore_video
	call	restore_state
	mov	ebp,_ebp
	mov	ds,_ds
	btr	dptr [esp+8],8
	iretd

;-----------------------------------------------------------------------------
_step_over:
	mov	esi,_eip
	call	decode
	mov	eax,dptr cmdbuf[0]
	cmp	eax,'llac'			; check for CALL
	jz	@@doit
	cmp	eax,'pool'			; check for LOOP..
	jz	@@doit
	and	eax,00FFFFFFh
	mov	ebx,00746E69h			;' tni'
	cmp	eax,ebx				; check for INT
	jz	@@doit
	mov	ebx,00706572h			;' per'
	cmp	eax,ebx				; check for REP
	jz	@@doit
	jmp	_trace_into
@@doit:	mov	_dr0,esi
	and	bptr _dr7[2],0F0h		; LEN=byte, R/W=exec
	or	bptr _dr7[0],003h
	call	restore_video
	call	restore_state
	mov	ebp,_ebp
	mov	ds,_ds
	btr	dptr [esp+8],8
	iretd

;-----------------------------------------------------------------------------
_jump_over:
	mov	esi,_eip
	call	decode
	mov	eax,dptr cmdbuf[0]
	cmp	eax,'llac'			; check for CALL
	jz	@@doit
	cmp	eax,'pool'			; check for LOOP..
	jz	@@doit
	and	eax,00FFFFFFh
	mov	ebx,00746E69h			;' tni'
	cmp	eax,ebx				; check for INT
	jz	@@doit
	mov	ebx,00706572h			;' per'
	cmp	eax,ebx				; check for REP
	jz	@@doit
	mov	ebx,00706D6Ah			;' pmj'
	cmp	eax,ebx				; check for JMP
	jz	@@doit
	mov	ebx,006F6E6Ah			;' onj'
	cmp	eax,ebx				; check for JNO
	jz	@@doit
	mov	ebx,0065616Ah			;' eaj'
	cmp	eax,ebx				; check for JAE
	jz	@@doit
	mov	ebx,007A6E6Ah			;' znj'
	cmp	eax,ebx				; check for JNZ
	jz	@@doit
	mov	ebx,0065626Ah			;' ebj'
	cmp	eax,ebx				; check for JBE
	jz	@@doit
	mov	ebx,00736E6Ah			;' snj'
	cmp	eax,ebx				; check for JNS
	jz	@@doit
	mov	ebx,0065706Ah			;' epj'
	cmp	eax,ebx				; check for JPE
	jz	@@doit
	mov	ebx,006F706Ah			;' opj'
	cmp	eax,ebx				; check for JPO
	jz	@@doit
	mov	ebx,0065676Ah			;' egj'
	cmp	eax,ebx				; check for JGE
	jz	@@doit
	mov	ebx,00656C6Ah			;' elj'
	cmp	eax,ebx				; check for JLE
	jz	@@doit
	and	eax,0000FFFFh
	mov	ebx,00006F6Ah			;' oj'
	cmp	eax,ebx				; check for JO
	jz	@@doit
	mov	ebx,0000626Ah			;' bj'
	cmp	eax,ebx				; check for JB
	jz	@@doit
	mov	ebx,00007A6Ah			;' zj'
	cmp	eax,ebx				; check for JZ
	jz	@@doit
	mov	ebx,0000616Ah			;' aj'
	cmp	eax,ebx				; check for JA
	jz	@@doit
	mov	ebx,0000736Ah			;' sj'
	cmp	eax,ebx				; check for JS
	jz	@@doit
	mov	ebx,00006C6Ah			;' lj'
	cmp	eax,ebx				; check for JL
	jz	@@doit
	mov	ebx,0000676Ah			;' gj'
	cmp	eax,ebx				; check for JG
	jz	@@doit
	jmp	_trace_into
@@doit:	mov	_dr0,esi
	and	bptr _dr7[2],0F0h		; LEN=byte, R/W=exec
	or	bptr _dr7[0],003h
	call	restore_video
	call	restore_state
	mov	ebp,_ebp
	mov	ds,_ds
	btr	dptr [esp+8],8
	iretd

;-----------------------------------------------------------------------------
_trace_into:
	mov	esi,_eip
	call	decode
	mov	eax,dptr cmdbuf[0]
	and	eax,00FFFFFFh
	mov	ebx,00746E69h			;' tni'
	cmp	eax,ebx				; check for INT
	jz	_step_over
	and	bptr _dr7[2],0F0h		; LEN=byte, R/W=exec
	and	bptr _dr7[0],0FCh
	call	restore_state
	mov	ebp,_ebp
	mov	ds,_ds
	bts	dptr [esp+8],8
	iretd

;-----------------------------------------------------------------------------
_return_to:
	mov	ecx,1024			; try 1024 instructions
	mov	esi,_eip
@@l0:	mov	ebp,esi
	call	decode
	mov	eax,dptr cmdbuf[0]
	mov	ebx,'teri'
	cmp	eax,ebx				; check for IRET
	jz	@@l1
	and	eax,00FFFFFFh
	mov	ebx,00746572h			;' ter'
	cmp	eax,ebx				; check for RET
	jz	@@l1
	mov	ebx,00706D6Ah			;' pmj'
	cmp	eax,ebx				; check for JMP
	jz	@@l1
	loop	@@l0
	ret
@@l1:	cmp	ecx,512
	jnz	@@l2
	ret
@@l2:	mov	_dr0,ebp
	and	bptr _dr7[2],0F0h
	or	bptr _dr7[0],003h
	call	restore_video
	call	restore_state
	mov	ebp,_ebp
	mov	ds,_ds
	btr	dptr [esp+8],8
	iretd


;-----------------------------------------------------------------------------
_proceed:
	call	restore_video
	call	restore_state
	mov	ebp,_ebp
	mov	ds,_ds
	btr	dptr [esp+8],8
	iretd

;-----------------------------------------------------------------------------
_new_address:
	mov	eax,_cpu_ypos
	dec	eax
	mov	eax,_addrbuffer[eax*4]
	mov	_eip,eax
	jmp	repaint_text

;-----------------------------------------------------------------------------
_break_pnt:
	mov	eax,_cpu_ypos
	dec	eax
	mov	edx,_addrbuffer[eax*4]		; EDX = addr of breakpoint
	mov	ebx,1
	mov	ecx,0Ch
@@l0:	cmp	_dr0[ebx*4],edx
	jz	@@l1
	inc	ebx
	shl	cl,2
	jnz	@@l0
	mov	ebx,1
	mov	ecx,0Ch
	mov	al,bptr _dr7[0]
@@l00:	test	al,cl
	jz	@@l2
	inc	ebx
	shl	cl,2
	jnz	@@l00
	ret
@@l1:	mov	_dr0[ebx*4],0
	not	cl
	and	bptr _dr7[0],cl
	jmp	repaint_text
@@l2:	mov	_dr0[ebx*4],edx
	or	bptr _dr7[0],cl
	mov	ax,0FFF0h
	lea	ecx,[ebx*4]
	rol	ax,cl
	and	wptr _dr7[2],ax
	jmp	repaint_text


;-----------------------------------------------------------------------------
_swap_screen:
	call	restore_video
	call	get_key
	call	video_init
	call	show_text
	call	show_stack
	call	show_cursor
	jmp	screen_on

_redraw_now:
	call	restore_video
	call	video_init
	call	clearkeytab
	call	show_text
	call	show_regs
	call	show_data
	call	show_mode
	call	show_stack
	call	show_cursor
	jmp	switch_to_cpu



;-----------------------------------------------------------------------------
_show_info:
	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
	call	clear_cmdline
	mov	xpos,1
	mov	ypos,49
	mov	edx,offs hlp00
	call	prints
	cmp	_pagenum,0
	jz	show_page0
	cmp	_pagenum,1
	jz	show_page1
	cmp	_pagenum,2
	jz	show_page2
	cmp	_pagenum,3
	jz	show_page3
	cmp	_pagenum,4
	jz	show_page4
	cmp	_pagenum,5
	jz	show_page5
	cmp	_pagenum,8
	jz	show_page8

get_page_key:
	call	get_key
	cmp	al,1				; ESC
	jz	info_exit
	cmp	al,3Bh				; F1 - DPMI
	jz	show_page0
	cmp	al,3Ch				; F2 - GDT
	jz	show_page1
	cmp	al,3Dh				; F3 - IDT
	jz	show_page2
	cmp	al,3Eh				; F4 - ExtMemBlks
	jz	show_page3
	cmp	al,3Fh				; F5 - History
	jz	show_page4
	cmp	al,40h				; F6 - INT buffers
	jz	show_page5
	cmp	al,44h				; F10 - DOS/32A
	jz	show_page8
	jmp	get_page_key

info_exit:
	call	show_text_clr
	call	print_cmdline
	jmp	show_cursor


show_page0:
	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
	mov	_pagenum,0

	mov	xpos,3
	mov	ypos,2
	mov	edx,offs hlp01
	mov	color,30h
	call	prints

	mov	xpos,3			; show DPMI function AX=0400h
	add	ypos,2
	mov	edx,offs hlp10
	mov	color,30h
	call	prints
	mov	xpos,4
	inc	ypos
	mov	edx,offs hlp11
	mov	color,31h
	call	prints
	mov	ax,0400h
	push	ds es fs gs
	int	31h
	pop	gs fs es ds
	push	dx cx bx
	mov	color,3Fh
	call	printax
	mov	xpos,4
	inc	ypos
	mov	edx,offs hlp12
	mov	color,31h
	call	prints
	pop	ax
	mov	color,3Fh
	call	printax
	mov	xpos,4
	inc	ypos
	mov	edx,offs hlp13
	mov	color,31h
	call	prints
	pop	ax
	mov	color,3Fh
	call	printax
	mov	xpos,4
	inc	ypos
	mov	edx,offs hlp14
	mov	color,31h
	call	prints
	pop	ax
	mov	color,3Fh
	call	printax

	mov	xpos,3			; show DPMI function AX=0500h
	add	ypos,2
	mov	edx,offs hlp20
	mov	color,30h
	call	prints
	mov	xpos,4
	inc	ypos
	mov	edx,offs hlp21
	mov	color,31h
	call	prints
	sub	esp,48
	mov	edi,esp
	mov	ax,0500h
	int	31h
	mov	eax,[esp+00h]
	mov	ebx,[esp+18h]
	add	esp,48
	mov	color,3Fh
	call	printeax
	mov	xpos,4
	inc	ypos
	mov	edx,offs hlp22
	mov	color,31h
	call	prints
	mov	eax,ebx
	shl	eax,12
	mov	color,3Fh
	call	printeax

	mov	xpos,3			; show DPMI function AX=0100h
	add	ypos,2
	mov	edx,offs hlp25
	mov	color,30h
	call	prints
	mov	xpos,4
	inc	ypos
	mov	edx,offs hlp26
	mov	color,31h
	call	prints
	mov	ax,0100h
	mov	bx,-1
	int	31h
	movzx	eax,bx
	shl	eax,4
	mov	color,3Fh
	call	printeax

	mov	xpos,37			; show DPMI function AX=0A00h
	mov	ypos,4
	mov	edx,offs hlp30
	mov	color,30h
	call	prints
	mov	xpos,38
	inc	ypos
	mov	edx,offs hlp31
	mov	color,31h
	call	prints
	mov	ax,0A00h
	push	ds es fs gs
	mov	esi,offs hlpid
	int	31h
	pop	gs fs es ds
	push	dx cx bx
	mov	color,3Fh
	call	printax
	mov	xpos,38
	inc	ypos
	mov	edx,offs hlp32
	mov	color,31h
	call	prints
	pop	ax
	mov	color,3Fh
	call	printax
	mov	xpos,38
	inc	ypos
	mov	edx,offs hlp33
	mov	color,31h
	call	prints
	pop	ax
	mov	color,3Fh
	call	printax
	mov	xpos,38
	inc	ypos
	mov	edx,offs hlp34
	mov	color,31h
	call	prints
	pop	ax
	mov	color,3Fh
	call	printax

	mov	xpos,37			; show DPMI function AX=0E00h
	add	ypos,2
	mov	edx,offs hlp40
	mov	color,30h
	call	prints
	mov	xpos,38
	inc	ypos
	mov	edx,offs hlp41
	mov	color,31h
	call	prints
	mov	ax,0E00h
	push	ds es fs gs
	int	31h
	pop	gs fs es ds
	mov	color,3Fh
	call	printax

	mov	xpos,2				; show Selectors
	add	ypos,7
	mov	color,30h
	mov	edx,offs hlp50
	call	prints

	mov	color,31h
	mov	xpos,3
	inc	ypos
	mov	edx,offs hlp51
	call	prints
	mov	xpos,3
	inc	ypos
	mov	edx,offs hlp52
	call	prints
	mov	xpos,3
	inc	ypos
	mov	edx,offs hlp53
	call	prints
	mov	xpos,3
	inc	ypos
	mov	edx,offs hlp54
	call	prints
	mov	xpos,3
	inc	ypos
	mov	edx,offs hlp55
	call	prints
	mov	xpos,3
	inc	ypos
	mov	edx,offs hlp56
	call	prints

	mov	color,3Fh
	mov	xpos,6
	sub	ypos,5
	mov	eax,_cs
	call	printax
	mov	xpos,6
	inc	ypos
	mov	eax,_ds
	call	printax
	mov	xpos,6
	inc	ypos
	mov	eax,_es
	call	printax
	mov	xpos,6
	inc	ypos
	mov	eax,_ss
	call	printax
	mov	xpos,6
	inc	ypos
	mov	eax,_fs
	call	printax
	mov	xpos,6
	inc	ypos
	mov	eax,_gs
	call	printax

	mov	xpos,13
	sub	ypos,5
	mov	eax,_cs_base
	call	printeax
	mov	xpos,13
	inc	ypos
	mov	eax,_ds_base
	call	printeax
	mov	xpos,13
	inc	ypos
	mov	eax,_es_base
	call	printeax
	mov	xpos,13
	inc	ypos
	mov	eax,_ss_base
	call	printeax
	mov	xpos,13
	inc	ypos
	mov	eax,_fs_base
	call	printeax
	mov	xpos,13
	inc	ypos
	mov	eax,_gs_base
	call	printeax

	mov	xpos,24
	sub	ypos,5
	mov	eax,_cs_limit
	call	printeax
	mov	xpos,24
	inc	ypos
	mov	eax,_ds_limit
	call	printeax
	mov	xpos,24
	inc	ypos
	mov	eax,_es_limit
	call	printeax
	mov	xpos,24
	inc	ypos
	mov	eax,_ss_limit
	call	printeax
	mov	xpos,24
	inc	ypos
	mov	eax,_fs_limit
	call	printeax
	mov	xpos,24
	inc	ypos
	mov	eax,_gs_limit
	call	printeax

	mov	xpos,35
	sub	ypos,5
	movzx	eax,_cs_acc
	shr	eax,15
	lea	edx,_acc_typeg[eax*8]
	call	prints
	mov	xpos,35
	inc	ypos
	movzx	eax,_ds_acc
	shr	eax,15
	lea	edx,_acc_typeg[eax*8]
	call	prints
	mov	xpos,35
	inc	ypos
	movzx	eax,_es_acc
	shr	eax,15
	lea	edx,_acc_typeg[eax*8]
	call	prints
	mov	xpos,35
	inc	ypos
	movzx	eax,_ss_acc
	shr	eax,15
	lea	edx,_acc_typeg[eax*8]
	call	prints
	mov	xpos,35
	inc	ypos
	movzx	eax,_fs_acc
	shr	eax,15
	lea	edx,_acc_typeg[eax*8]
	call	prints
	mov	xpos,35
	inc	ypos
	movzx	eax,_gs_acc
	shr	eax,15
	lea	edx,_acc_typeg[eax*8]
	call	prints

	mov	xpos,42
	sub	ypos,5
	movzx	eax,_cs_acc
	shr	eax,14
	and	eax,1
	lea	edx,_acc_typeb[eax*8]
	call	prints
	mov	xpos,42
	inc	ypos
	movzx	eax,_ds_acc
	shr	eax,14
	and	eax,1
	lea	edx,_acc_typeb[eax*8]
	call	prints
	mov	xpos,42
	inc	ypos
	movzx	eax,_es_acc
	shr	eax,14
	and	eax,1
	lea	edx,_acc_typeb[eax*8]
	call	prints
	mov	xpos,42
	inc	ypos
	movzx	eax,_ss_acc
	shr	eax,14
	and	eax,1
	lea	edx,_acc_typeb[eax*8]
	call	prints
	mov	xpos,42
	inc	ypos
	movzx	eax,_fs_acc
	shr	eax,14
	and	eax,1
	lea	edx,_acc_typeb[eax*8]
	call	prints
	mov	xpos,42
	inc	ypos
	movzx	eax,_gs_acc
	shr	eax,14
	and	eax,1
	lea	edx,_acc_typeb[eax*8]
	call	prints

	mov	xpos,50
	sub	ypos,5
	movzx	eax,_cs_acc
	shr	eax,1
	and	eax,7
	add	eax,30h
	call	printc
	mov	xpos,50
	inc	ypos
	movzx	eax,_ds_acc
	shr	eax,1
	and	eax,7
	add	eax,30h
	call	printc
	mov	xpos,50
	inc	ypos
	movzx	eax,_es_acc
	shr	eax,1
	and	eax,7
	add	eax,30h
	call	printc
	mov	xpos,50
	inc	ypos
	movzx	eax,_ss_acc
	shr	eax,1
	and	eax,7
	add	eax,30h
	call	printc
	mov	xpos,50
	inc	ypos
	movzx	eax,_fs_acc
	shr	eax,1
	and	eax,7
	add	eax,30h
	call	printc
	mov	xpos,50
	inc	ypos
	movzx	eax,_gs_acc
	shr	eax,1
	and	eax,7
	add	eax,30h
	call	printc

	mov	xpos,56
	sub	ypos,5
	mov	ax,_cs_acc
	call	printax
	mov	xpos,56
	inc	ypos
	mov	ax,_ds_acc
	call	printax
	mov	xpos,56
	inc	ypos
	mov	ax,_es_acc
	call	printax
	mov	xpos,56
	inc	ypos
	mov	ax,_ss_acc
	call	printax
	mov	xpos,56
	inc	ypos
	mov	ax,_fs_acc
	call	printax
	mov	xpos,56
	inc	ypos
	mov	ax,_gs_acc
	call	printax

	mov	xpos,3
	add	ypos,4
	mov	color,31h
	mov	edx,offs hlp0A
	call	prints
	mov	color,3Fh
	mov	eax,__page_faults
	call	printeax
	mov	xpos,3
	inc	ypos
	mov	color,31h
	mov	edx,offs hlp0B
	call	prints
	mov	color,3Fh
	mov	eax,offs _system_stack
	sub	eax,esp
	call	printeax

	jmp	get_page_key


;-----------------------------------------------------------------------------
show_page1:					; show GDT
	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
	mov	_pagenum,1

	mov	xpos,2
	mov	ypos,30
	mov	edx,offs hlp62
	mov	color,30h
	call	prints
	mov	xpos,3
	mov	ypos,2
	mov	edx,offs hlp60
	mov	color,30h
	call	prints
	mov	xpos,4
	inc	ypos
	mov	edx,offs hlp61
	mov	color,31h
	call	prints
	mov	xpos,4
	add	ypos,2
	mov	edx,offs _gdt_string
	mov	color,30h
	call	prints
	mov	xpos,4
	inc	ypos
	mov	edx,offs _gdt_string2
	call	prints

	mov	al,00h
	call	fword ptr __api_off

	mov	color,3Fh
	mov	xpos,13
	mov	ypos,3
	mov	eax,esi
	call	printeax
	mov	xpos,33
	mov	eax,ecx
	call	printeax

@@0:	lea	edx,[esi+ecx-7]
	sub	edx,_gdt_ptr
	mov	ebp,22
	mov	ypos,6
@@1:	inc	ypos
	mov	xpos,6

	push	es			; check Avail
	mov	es,bx
	test	byte ptr es:[edx+6],10h
	pop	es
	mov	color,3Eh
	jnz	@@avl
	mov	color,3Fh
@@avl:	mov	eax,edx
	sub	eax,esi
	call	printax
	add	xpos,3
	push	edx
	push	es			; get Limit 0..15
	mov	es,bx
	mov	ax,es:[edx]
	add	edx,2
	pop	es
	call	printax
	add	xpos,3
	push	es			; get Base 23..0
	mov	es,bx
	mov	eax,es:[edx]
	add	edx,3
	pop	es
	push	eax
	shr	eax,16
	call	printal
	pop	eax
	call	printax
	add	xpos,3
	push	es			; get Acc
	mov	es,bx
	mov	ax,es:[edx]
	add	edx,2
	pop	es
	call	printax
	add	xpos,6
	push	es			; get Base 24..31
	mov	es,bx
	mov	al,es:[edx]
	add	edx,1
	pop	es
	call	printal
	pop	edx
	sub	edx,8
	cmp	edx,esi
	jae	@@w
	mov	xpos,1
	mov	al,20h
	push	ecx
	mov	ecx,50
@@x:	call	printc
	loop	@@x
	pop	ecx
@@w:	dec	ebp
	jnz	@@1

@@2:	call	get_key

	cmp	al,49h
	jz	@@down
	cmp	al,51h
	jz	@@up
	cmp	al,47h				; Home
	jz	@@home
	cmp	al,1				; ESC
	jz	info_exit
	cmp	al,3Bh				; F1 - DPMI
	jz	show_page0
	cmp	al,3Ch				; F2 - GDT
	jz	show_page1
	cmp	al,3Dh				; F3 - IDT
	jz	show_page2
	cmp	al,3Eh				; F4 - ExtMemBlks
	jz	show_page3
	cmp	al,3Fh				; F5 - History
	jz	show_page4
	cmp	al,40h				; F6 - INT buffers
	jz	show_page5
	cmp	al,44h				; F10 - DOS/32A
	jz	show_page8
	jmp	@@2

@@up:	mov	eax,_gdt_ptr
	add	eax,00B0h
	lea	edx,[esi+ecx-7]
	sub	edx,eax
	cmp	edx,esi
	jbe	@@2
	mov	_gdt_ptr,eax
	jmp	@@0
@@down:	mov	eax,_gdt_ptr
	sub	eax,00B0h
	jb	@@2
	mov	_gdt_ptr,eax
	jmp	@@0
@@home:	mov	_gdt_ptr,0
	jmp	@@0




;-----------------------------------------------------------------------------
show_page2:					; show IDT
	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
	mov	_pagenum,2

	mov	xpos,2
	mov	ypos,30
	mov	edx,offs hlp72
	mov	color,30h
	call	prints
	mov	xpos,3
	mov	ypos,2
	mov	edx,offs hlp70
	mov	color,30h
	call	prints
	mov	xpos,4
	inc	ypos
	mov	edx,offs hlp71
	mov	color,31h
	call	prints
	mov	xpos,4
	add	ypos,2
	mov	edx,offs _idt_string
	mov	color,30h
	call	prints
	mov	xpos,4
	inc	ypos
	mov	edx,offs _idt_string2
	call	prints

	mov	al,00h
	call	fword ptr __api_off
	mov	ecx,edx
	mov	esi,edi

	mov	color,3Fh
	mov	xpos,13
	mov	ypos,3
	mov	eax,esi
	call	printeax
	mov	xpos,33
	mov	eax,ecx
	call	printeax

@@0:	lea	edx,[esi]
	add	edx,_idt_ptr
	mov	ebp,22
	mov	ypos,6
@@1:	inc	ypos
	mov	xpos,6

	push	es			; check Selector
	mov	es,bx
	mov	ax,es:[edx+2]
	pop	es
	cmp	ax,__kernel_codesel
	mov	color,3Eh
	jnz	@@inst
	mov	color,3Fh

@@inst:	mov	eax,edx
	sub	eax,esi
	shr	eax,3
	call	printal
	add	xpos,6
	push	edx

	push	es			; get Offset 0..15
	mov	es,bx
	mov	ax,es:[edx]
	add	edx,2
	pop	es
	call	printax
	add	xpos,6

	push	es			; get Selector
	mov	es,bx
	mov	ax,es:[edx]
	add	edx,3
	pop	es
	call	printax
	add	xpos,5

	push	es			; get Type
	mov	es,bx
	mov	al,es:[edx]
	add	edx,1
	pop	es
	test	al,01h
	mov	ah,color
	jnz	@@m
	mov	color,3Ah
@@m:	call	printal
	mov	color,ah
	add	xpos,3

	push	es			; get Offset 16..31
	mov	es,bx
	mov	ax,es:[edx]
	add	edx,2
	pop	es
	call	printax
	pop	edx

	add	edx,8
	lea	eax,[esi+ecx+1]
	cmp	edx,eax
	jbe	@@w
	mov	xpos,1
	mov	al,20h
	push	ecx
	mov	ecx,50
@@x:	call	printc
	loop	@@x
	pop	ecx
@@w:	dec	ebp
	jnz	@@1

@@2:	call	get_key

	cmp	al,49h
	jz	@@up
	cmp	al,51h
	jz	@@down
	cmp	al,47h				; Home
	jz	@@home
	cmp	al,1				; ESC
	jz	info_exit
	cmp	al,3Bh				; F1 - DPMI
	jz	show_page0
	cmp	al,3Ch				; F2 - GDT
	jz	show_page1
	cmp	al,3Dh				; F3 - IDT
	jz	show_page2
	cmp	al,3Eh				; F4 - ExtMemBlks
	jz	show_page3
	cmp	al,3Fh				; F5 - History
	jz	show_page4
	cmp	al,40h				; F6 - INT buffers
	jz	show_page5
	cmp	al,44h				; F10 - DOS/32A
	jz	show_page8
	jmp	@@2

@@up:	mov	eax,_idt_ptr
	sub	eax,00B0h
	jb	@@2
	mov	_idt_ptr,eax
	jmp	@@0
@@down:	mov	eax,_idt_ptr
	add	eax,00B0h
	lea	edx,[ecx+1]
	cmp	eax,edx
	ja	@@2
	mov	_idt_ptr,eax
	jmp	@@0
@@home:	mov	_idt_ptr,0
	jmp	@@0



;-----------------------------------------------------------------------------
show_page3:					; show Ext Blocks
	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
	mov	_pagenum,3

	mov	xpos,2
	mov	ypos,30
	mov	edx,offs hlp82
	mov	color,30h
	call	prints
	mov	xpos,3
	mov	ypos,2
	mov	edx,offs hlp80
	mov	color,30h
	call	prints
	mov	xpos,4
	inc	ypos
	mov	edx,offs hlp81
	mov	color,31h
	call	prints
	mov	xpos,4
	add	ypos,2
	mov	edx,offs _emb_string
	mov	color,30h
	call	prints
	mov	xpos,4
	inc	ypos
	mov	edx,offs _emb_string2
	call	prints

	mov	al,03h
	call	fword ptr __api_off

	mov	xpos,13
	mov	ypos,3
	mov	color,3Fh
	mov	eax,edx
	call	printeax
	mov	xpos,32
	mov	eax,ecx
	call	printeax
	mov	xpos,50
	mov	eax,esi
	call	printeax
	mov	_emb_mem_ptr,edx
	mov	_emb_mem_top,esi
@@0:	pushad
	mov	ebx,22
	mov	ecx,40
	mov	esi,4
	mov	edi,7
	call	clearwindow
	popad
	mov	ebp,22
	xor	edi,edi
	mov	esi,_emb_mem_ptr
	mov	ypos,7
@@1:	call	@read_emb
	cmp	edi,_emb_num
	jb	@@3

	mov	xpos,5
	mov	color,3Fh
	push	eax
	mov	eax,edi
	call	printal
	pop	eax
	mov	xpos,12
	call	printeax
	mov	xpos,22
	mov	eax,ecx
	call	printeax
	mov	xpos,32
	mov	eax,edx
	test	al,al
	mov	color,3Fh
	mov	edx,offs _emb_str1
	jz	@@l1
	mov	color,3Eh
	mov	edx,offs _emb_str2
@@l1:	call	prints
	push	eax
	mov	al,'/'
	mov	color,30h
	call	printc
	pop	eax
	test	ah,ah
	mov	color,3Ah
	mov	edx,offs _emb_str3
	jz	@@l2
	mov	color,34h
	mov	edx,offs _emb_str4
@@l2:	call	prints
	inc	ypos
	dec	ebp
	jz	@@4
@@3:	inc	edi
	cmp	esi,_emb_mem_top
	jb	@@1

@@4:	call	get_key
	cmp	al,48h				; up
	jz	@@upk
	cmp	al,50h				; down
	jz	@@dnk
	cmp	al,49h
	jz	@@up
	cmp	al,51h
	jz	@@down
	cmp	al,47h				; Home
	jz	@@home
	cmp	al,1				; ESC
	jz	info_exit
	cmp	al,3Bh				; F1 - DPMI
	jz	show_page0
	cmp	al,3Ch				; F2 - GDT
	jz	show_page1
	cmp	al,3Dh				; F3 - IDT
	jz	show_page2
	cmp	al,3Eh				; F4 - ExtMemBlks
	jz	show_page3
	cmp	al,3Fh				; F5 - History
	jz	show_page4
	cmp	al,40h				; F6 - INT buffers
	jz	show_page5
	cmp	al,44h				; F10 - DOS/32A
	jz	show_page8
	jmp	@@4

@@upk:	mov	eax,_emb_num
	sub	eax,1
	jc	@@home
	mov	_emb_num,eax
	jmp	@@0
@@up:	mov	eax,_emb_num
	sub	eax,22
	jc	@@home
	mov	_emb_num,eax
	jmp	@@0
@@dnk:	mov	eax,_emb_num
	add	eax,1
	mov	ecx,eax
	mov	esi,_emb_mem_ptr
@@dk1:	cmp	esi,_emb_mem_top
	jz	@@dk2
	push	eax ecx
	call	@read_emb
	pop	ecx eax
	test	dh,dh
	jnz	@@dn2
	loop	@@dn1
	mov	_emb_num,eax
	jmp	@@0
@@dk2:	jmp	@@4
@@dk3:	dec	_emb_num
	jmp	@@0
@@down:	mov	eax,_emb_num
	add	eax,22
	mov	ecx,eax
	mov	esi,_emb_mem_ptr
@@dn1:	push	eax ecx
	call	@read_emb
	pop	ecx eax
	test	dh,dh
	jnz	@@dn2
	loop	@@dn1
	mov	_emb_num,eax
	jmp	@@0
@@dn2:	jmp	@@4
@@home:	mov	_emb_num,0
	jmp	@@0

@read_emb:
	xor	eax,eax			; EAX = base
	xor	ecx,ecx			; ECX = size
	xor	edx,edx			; DL = free:0/used:1,DH=ok:0/invalid:1
	push	es
	mov	es,bx
	mov	ecx,es:[esi+04h]
	pop	es
	lea	eax,[esi+10h]
	btr	ecx,31			; check if memory block is used
	setc	dl
	lea	esi,[esi+ecx+10h]	; load addres of next memory block
	cmp	esi,_emb_mem_ptr
	jb	@@err
	cmp	esi,_emb_mem_top	; check if at top of memory
	ja	@@err
	ret
@@err:	mov	dh,1
	ret


;-----------------------------------------------------------------------------
show_page4:					; show Interrupt History
	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
	mov	_pagenum,4

	mov	xpos,2
	mov	ypos,30
	mov	edx,offs hlp92
	mov	color,30h
	call	prints
	mov	xpos,3
	mov	ypos,2
	mov	edx,offs hlp90
	mov	color,30h
	call	prints
	mov	xpos,4
	inc	ypos
	mov	edx,offs hlp91
	mov	color,31h
	call	prints
	mov	eax,__history_bufnum
	mov	color,3Fh
	call	printax
	cmp	__history_bufon,0
	jz	get_page_key

@@0:	pushad
	mov	ebx,24
	mov	ecx,56
	mov	esi,3
	mov	edi,5
	call	clearwindow
	popad
	mov	xpos,46
	mov	ypos,3
	mov	color,31h
	mov	edx,offs _hist_str30
	cmp	__history_intnum,0
	jz	@@lax
	mov	edx,offs _hist_str31
	cmp	__history_intnum,10h
	jz	@@lax
	mov	edx,offs _hist_str32
	cmp	__history_intnum,21h
	jz	@@lax
	mov	edx,offs _hist_str33
	cmp	__history_intnum,31h
	jz	@@lax
	mov	edx,offs _hist_str34
	cmp	__history_intnum,33h
	jz	@@lax
	mov	edx,offs _hist_str30
@@lax:	call	prints

	mov	ypos,5
	mov	ecx,__history_bufnum
	test	ecx,ecx
	jz	@@done
	mov	esi,__history_bufbase
	mov	edx,__history_current
	mov	ebp,4
	mov	eax,ecx
	sub	eax,edx
	jbe	@@l2
	cmp	eax,4
	jae	@@2
	mov	ebp,eax
@@2:	mov	eax,__history_bufsize
	shr	eax,6
	sub	eax,edx
	jbe	@@l2
	cmp	eax,4
	jae	@@5
	mov	ebp,eax
@@5:	imul	edx,40h
	add	esi,edx

@@l1:	mov	al,[esi+02h]
	cmp	__history_intnum,0
	jz	@@all
	cmp	__history_intnum,10h
	jz	@@10h
	cmp	__history_intnum,21h
	jz	@@21h
	cmp	__history_intnum,31h
	jz	@@31h
	cmp	__history_intnum,33h
	jz	@@33h
	jmp	@@all
@@10h:	cmp	al,10h
	jnz	@@next
	jmp	@@all
@@21h:	cmp	al,21h
	jnz	@@next
	jmp	@@all
@@31h:	cmp	al,31h
	jnz	@@next
	jmp	@@all
@@33h:	cmp	al,33h
	jnz	@@next
	jmp	@@all
@@next:	add	esi,40h
	dec	ecx
	jnz	@@l1
	jmp	@@l2
@@all:	mov	xpos,4
	mov	edx,offs _hist_str01
	mov	color,31h
	call	prints
	mov	ax,[esi+00h]			; Number
	inc	ax
	mov	color,3Eh
	call	printax
	mov	xpos,18
	mov	edx,offs _hist_str02
	mov	color,31h
	call	prints
	mov	al,[esi+02h]			; INT nnh
	push	eax
	mov	color,3Eh
	call	printal
	mov	al,'h'
	call	printc
	mov	xpos,26
	mov	color,31h
	mov	al,'('
	call	printc
	pop	eax
	cmp	al,10h
	mov	color,39h
	mov	edx,offs _hist_str04
	jz	@@3
	cmp	al,21h
	mov	color,3Ah
	mov	edx,offs _hist_str05
	jz	@@3
	cmp	al,31h
	mov	color,3Bh
	mov	edx,offs _hist_str06
	jz	@@3
	cmp	al,33h
	mov	color,38h
	mov	edx,offs _hist_str07
	jz	@@3
	jmp	@@4
@@3:	call	prints
@@4:	mov	edx,offs _hist_str03
	mov	color,31h
	call	prints
	mov	color,3Eh
	mov	ax,[esi+0Ch]			; AX
	call	printax
	mov	color,31h
	mov	al,'h'
	call	printc
	mov	al,')'
	call	printc

	inc	ypos
	mov	xpos,4
	mov	color,31h
	mov	edx,offs _hist_str10
	call	prints
	inc	ypos
	mov	xpos,4
	mov	edx,offs _hist_str11
	call	prints
	inc	ypos
	mov	xpos,4
	mov	edx,offs _hist_str12
	call	prints
	inc	ypos
	mov	xpos,4
	mov	edx,offs _hist_str20
	call	prints
	sub	ypos,3

	mov	color,3Fh
	mov	xpos,8
	mov	eax,[esi+04h]			; EFL
	call	printeax
	mov	xpos,22
	mov	eax,[esi+08h]			; EIP
	call	printeax
	inc	ypos
	mov	xpos,8
	mov	eax,[esi+0Ch]			; EAX
	call	printeax
	mov	xpos,22
	mov	eax,[esi+10h]			; EBX
	call	printeax
	mov	xpos,36
	mov	eax,[esi+14h]			; ECX
	call	printeax
	mov	xpos,50
	mov	eax,[esi+18h]			; EDX
	call	printeax
	inc	ypos
	mov	xpos,8
	mov	eax,[esi+1Ch]			; ESI
	call	printeax
	mov	xpos,22
	mov	eax,[esi+20h]			; EDI
	call	printeax
	mov	xpos,36
	mov	eax,[esi+24h]			; EBP
	call	printeax
	mov	xpos,50
	mov	eax,[esi+28h]			; ESP
	call	printeax

	inc	ypos
	mov	xpos,7
	mov	ax,[esi+2Ch]			; CS
	call	printax
	mov	xpos,16
	mov	ax,[esi+30h]			; DS
	call	printax
	mov	xpos,25
	mov	ax,[esi+32h]			; ES
	call	printax
	mov	xpos,34
	mov	ax,[esi+34h]			; SS
	call	printax
	mov	xpos,43
	mov	ax,[esi+36h]			; FS
	call	printax
	mov	xpos,52
	mov	ax,[esi+38h]			; GS
	call	printax
	inc	ypos
	mov	xpos,3
	mov	al,'ฤ'
	mov	edx,56
	mov	color,31h
@@1:	call	printc
	dec	edx
	jnz	@@1

	inc	ypos
	add	esi,40h
	dec	ebp
	jz	@@l2
	dec	ecx
	jnz	@@l1

@@l2:	call	get_key
	cmp	al,48h				; up
	jz	@@up
	cmp	al,50h				; down
	jz	@@dn
	cmp	al,49h
	jz	@@pgup
	cmp	al,51h
	jz	@@pgdn
	cmp	al,47h				; Home
	jz	@@home
	cmp	al,4Fh				; End
	jz	@@end
	cmp	al,0Eh				; Backspace
	jz	@@clr
	cmp	al,2				; 1
	jz	@@s10h
	cmp	al,3				; 2
	jz	@@s21h
	cmp	al,4				; 3
	jz	@@s31h
	cmp	al,5				; 4
	jz	@@s33h
	cmp	al,11				; 0
	jz	@@sall
	cmp	al,1				; ESC
	jz	info_exit
	cmp	al,3Bh				; F1 - DPMI
	jz	show_page0
	cmp	al,3Ch				; F2 - GDT
	jz	show_page1
	cmp	al,3Dh				; F3 - IDT
	jz	show_page2
	cmp	al,3Eh				; F4 - ExtMemBlks
	jz	show_page3
	cmp	al,3Fh				; F5 - History
	jz	show_page4
	cmp	al,40h				; F6 - INT buffers
	jz	show_page5
	cmp	al,44h				; F10 - DOS/32A
	jz	show_page8
	jmp	@@l2

@@up:	cmp	__history_current,0
	jbe	@@l2
	dec	__history_current
	jmp	@@0
@@dn:	mov	eax,__history_current
	mov	edx,__history_bufsize
	inc	eax
	shr	edx,6
	cmp	eax,edx
	jae	@@l2
	cmp	eax,__history_bufnum
	jae	@@l2
	mov	__history_current,eax
	jmp	@@0
@@pgup:	mov	eax,__history_current
	sub	eax,4
	jbe	@@home
	mov	__history_current,eax
	jmp	@@0
@@pgdn:	mov	eax,__history_current
	mov	edx,__history_bufsize
	add	eax,4
	shr	edx,6
	cmp	eax,edx
	jae	@@l2
	cmp	eax,__history_bufnum
	jae	@@l2
	mov	__history_current,eax
	jmp	@@0
@@home:	mov	__history_current,0
	jmp	@@0
@@end:	mov	eax,__history_bufnum
	mov	edx,__history_bufsize
	shr	edx,6
	cmp	eax,edx
	jb	@@end1
	mov	eax,edx
@@end1:	test	eax,eax
	jz	@@end2
	dec	eax
@@end2:	mov	__history_current,eax
	jmp	@@0
@@s10h:	mov	__history_intnum,10h
	jmp	@@home
@@s21h:	mov	__history_intnum,21h
	jmp	@@home
@@s31h:	mov	__history_intnum,31h
	jmp	@@home
@@s33h:	mov	__history_intnum,33h
	jmp	@@home
@@sall:	mov	__history_intnum,00h
	jmp	@@home
@@clr:	xor	eax,eax
	mov	__history_intnum,al
	mov	__history_current,eax
	mov	__history_bufptr,eax
	mov	__history_bufnum,eax
	mov	ecx,__history_bufsize
	mov	edi,__history_bufbase
	rep	stosb
	mov	xpos,4
	mov	ypos,3
	mov	edx,offs hlp91
	mov	color,31h
	call	prints
	mov	eax,__history_bufnum
	mov	color,3Fh
	call	printax
	jmp	@@home
@@done:	jmp	get_page_key


;-----------------------------------------------------------------------------
show_page5:					; show INT Buffers
	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
	mov	_pagenum,5

	mov	xpos,3
	mov	ypos,2
	mov	edx,offs hlpB0
	mov	color,30h
	call	prints
	mov	xpos,4
	add	ypos,2
	mov	edx,offs hlpB1
	mov	color,30h
	call	prints
	mov	xpos,4
	inc	ypos
	mov	edx,offs hlpB2
	call	prints
	add	ypos,2

	mov	al,02h
	call	fword ptr __api_off

	mov	ecx,16
	mov	al,00h
	mov	color,3Fh
@@1:	mov	xpos,6
	call	printal
	inc	ypos
	inc	al
	loop	@@1
	sub	ypos,16

	push	gs
	mov	gs,bx

	mov	ecx,16
	mov	edx,40h+80h
@@2:	mov	xpos,12
	mov	ax,gs:[edx+edi+4]
	call	printax
	mov	al,':'
	call	printc
	mov	eax,gs:[edx+edi+0]
	call	printeax
	add	edx,8
	inc	ypos
	loop	@@2
	sub	ypos,16

	mov	ecx,16
	mov	edx,40h
@@3:	mov	xpos,28
	mov	ax,gs:[edx+edi+4]
	call	printax
	mov	al,':'
	call	printc
	mov	eax,gs:[edx+edi+0]
	call	printeax
	add	edx,8
	inc	ypos
	loop	@@3
	sub	ypos,16

	inc	ypos
	mov	xpos,28
	mov	eax,__irq1_buffer_sel
	call	printax
	mov	al,':'
	call	printc
	mov	eax,__irq1_buffer_off
	call	printeax
	dec	ypos

	mov	ecx,16
	mov	bx,gs:[esi+02h]
	mov	dx,gs:[esi+04h]
@@4:	mov	xpos,45
	mov	ax,dx
	and	al,01h
	mov	color,3Eh
	jnz	@@l1
	mov	color,3Fh
@@l1:	add	al,30h
	call	printc
	mov	al,'/'
	mov	color,30h
	call	printc
	mov	ax,bx
	and	al,01h
	mov	color,3Eh
	jnz	@@l2
	mov	color,3Fh
@@l2:	add	al,30h
	call	printc
	shr	bx,1
	shr	dx,1
	inc	ypos
	loop	@@4
	sub	ypos,16

	inc	ypos
	mov	xpos,47
	mov	eax,__irq1_buffer_sel
	shr	eax,17
	and	al,01h
	mov	color,3Eh
	jnz	@@l3
	mov	color,3Fh
@@l3:	add	al,30h
	call	printc

	pop	gs
	jmp	get_page_key


;-----------------------------------------------------------------------------
show_page8:					; show Extender/Debugger info
	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
	mov	_pagenum,8

	mov	xpos,3
	mov	ypos,2
	mov	edx,offs hlpA0
	mov	color,30h
	call	prints
	mov	xpos,3
	add	ypos,2
	mov	color,31h
	push	fs
	pushad
	mov	ax,0FF89h
	int	21h
	test	dx,8000h
	popad
	pop	fs
	jz	@@1

	mov	edx,offs hlpA1
	call	prints
	mov	xpos,3
	inc	ypos
	mov	edx,offs hlpA3
	call	prints
	mov	color,3Fh
	call	@@ver
	mov	xpos,3
	add	ypos,2
	mov	color,31h
	mov	edx,offs hlpA5
	call	prints
	jmp	@@done
@@1:	mov	edx,offs hlpA2
	call	prints
	mov	xpos,3
	inc	ypos
	mov	edx,offs hlpA4
	call	prints
	mov	color,3Fh
	call	@@ver
	mov	xpos,3
	add	ypos,2
	mov	color,31h
	mov	edx,offs hlpA6
	call	prints
@@done:	mov	xpos,3
	inc	ypos
	mov	edx,offs hlpA7
	call	prints
	mov	xpos,3
	inc	ypos
	mov	edx,offs hlpA8
	call	prints
	add	ypos,2
	mov	esi,offs library_string
@@l1:	mov	al,[esi]
	inc	esi
	test	al,al
	jz	@@ok
	call	printc
	cmp	al,0Ah
	jnz	@@l1
	mov	xpos,3
	jmp	@@l1
@@ok:	jmp	get_page_key

@@ver:	mov	ax,0EEFFh
	push	es
	int	31h
	pop	es
	mov	ax,dx
	mov	al,ah
	aam
	add	ax,3030h
	call	printc
	mov	al,'.'
	call	printc
	mov	ax,dx
	aam
	add	ax,3030h
	xchg	ah,al
	push	ax
	call	printc
	pop	ax
	xchg	ah,al
	jmp	printc





;-----------------------------------------------------------------------------
show_mode:
	mov	xpos,67
	mov	ypos,34
	cmp	_assmmode_flag,0
	mov	edx,offs msg086
	jnz	prints
	mov	edx,offs msg386
	jmp	prints

show_text_clr:
	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
show_text:
	mov	ypos,1
	mov	color,30h
	mov	ecx,30			; lines to disassemble
	mov	al,_assmmode_flag
	mov	_opsiz,al		; set default 32bit opcode size
	mov	_adrsiz,al		; set default 32bit address size
	mov	_prefix,3		; set default prefix to DS:
	mov	_cpu_xptr,0
	mov	eax,_code_addr		; start addr (sel=always CS)
	mov	esi,eax
	xor	ebx,ebx
@@l1:	mov	eax,esi
	call	decode
	mov	_addrbuffer[ebx*4],eax
	inc	ebx
	mov	edx,offs textbuf
	cmp	eax,_eip
	jnz	@@l2
	mov	bptr [edx+8],10h	; show EIP '>>' marker
	mov	_cpu_xptr,ebx
	push	ebx
	cmp	_showmode_flag,0
	mov	ebx,23
	jz	@@0
	mov	ebx,8
@@0:	cmp	_jump_taken,0
	jz	@@2
	jg	@@1
	mov	bptr [edx+ebx],18h
	jmp	@@2
@@1:	mov	bptr [edx+ebx],19h
@@2:	pop	ebx
@@l2:	mov	xpos,1
	call	prints
	call	@@bchk
	inc	ypos
	dec	ecx
	jnz	@@l1
	mov	_addrbuffer[ebx*4],esi
	mov	ebx,_cpu_ypos
	dec	ebx
	mov	esi,_addrbuffer[ebx*4]
	jmp	decode
@@bchk:	push	ebx ecx esi edi
	mov	bl,0C0h
	mov	ebp,3
@@l001:	test	bptr _dr7,bl
	jz	@@l002
	cmp	eax,_dr0[ebp*4]
	jnz	@@l002
	push	eax
	mov	esi,1
	movzx	edi,ypos
	mov	ecx,58
	mov	ax,7F00h
	call	_cursor
	mov	eax,ebp			; EBP = breakpoint number
	shl	eax,16
	add	eax,60306023h		; show #X (X=BrkPnt)
	add	esi,ecx			; adjust X-pos
	push	edx
	lea	edx,[edi*8+edi]
	add	edx,edi
	shl	edx,4
	lea	esi,[edx+esi*2]
	add	esi,screen_base
	mov	[esi],eax
	pop	edx
	pop	eax
@@l002:	shr	bl,2
	dec	ebp
	jnz	@@l001
	pop	edi esi ecx ebx
	ret


;-----------------------------------------------------------------------------
show_regs:
	call	show_gregs
	call	show_sregs
	call	show_xregs
	call	show_slide
	jmp	show_mode
show_gregs:
	mov	dl,5
	mov	ypos,32
	mov	xpos,dl
	mov	eax,_eax
	cmp	eax,_old_eax
	mov	color,30h
	jz	@@l1
	mov	color,3Fh
@@l1:	call	printeax
	inc	ypos
	mov	xpos,dl
	mov	eax,_ebx
	cmp	eax,_old_ebx
	mov	color,30h
	jz	@@l2
	mov	color,3Fh
@@l2:	call	printeax
	inc	ypos
	mov	xpos,dl
	mov	eax,_ecx
	cmp	eax,_old_ecx
	mov	color,30h
	jz	@@l3
	mov	color,3Fh
@@l3:	call	printeax
	inc	ypos
	mov	xpos,dl
	mov	eax,_edx
	cmp	eax,_old_edx
	mov	color,30h
	jz	@@l4
	mov	color,3Fh
@@l4:	call	printeax
	mov	dl,20
	mov	ypos,32
	mov	xpos,dl
	mov	eax,_esi
	cmp	eax,_old_esi
	mov	color,30h
	jz	@@l5
	mov	color,3Fh
@@l5:	call	printeax
	inc	ypos
	mov	xpos,dl
	mov	eax,_edi
	cmp	eax,_old_edi
	mov	color,30h
	jz	@@l6
	mov	color,3Fh
@@l6:	call	printeax
	inc	ypos
	mov	xpos,dl
	mov	eax,_ebp
	cmp	eax,_old_ebp
	mov	color,30h
	jz	@@l7
	mov	color,3Fh
@@l7:	call	printeax
	inc	ypos
	mov	xpos,dl
	mov	eax,_esp
	cmp	eax,_old_esp
	mov	color,30h
	jz	@@l8
	mov	color,3Fh
@@l8:	jmp	printeax
show_sregs:				; segment registers
	mov	dl,34
	mov	ypos,32
	mov	xpos,dl
	mov	eax,_cs
	cmp	ax,wptr _old_cs
	mov	color,30h
	jz	@@l1
	mov	color,3Fh
@@l1:	call	printax
	call	@@show
	inc	ypos
	mov	xpos,dl
	mov	eax,_ds
	cmp	ax,wptr _old_ds
	mov	color,30h
	jz	@@l2
	mov	color,3Fh
@@l2:	call	printax
	call	@@show
	inc	ypos
	mov	xpos,dl
	mov	eax,_es
	cmp	ax,wptr _old_es
	mov	color,30h
	jz	@@l3
	mov	color,3Fh
@@l3:	call	printax
	call	@@show
	inc	ypos
	mov	xpos,dl
	mov	eax,_ss
	cmp	ax,wptr _old_ss
	mov	color,30h
	jz	@@l4
	mov	color,3Fh
@@l4:	call	printax
	call	@@show
	inc	ypos
	mov	xpos,dl
	mov	eax,_fs
	cmp	ax,wptr _old_fs
	mov	color,30h
	jz	@@l5
	mov	color,3Fh
@@l5:	call	printax
	call	@@show
	inc	ypos
	mov	xpos,dl
	mov	eax,_gs
	cmp	ax,wptr _old_gs
	mov	color,30h
	jz	@@l6
	mov	color,3Fh
@@l6:	call	printax
@@show:	add	xpos,3
	push	edx
	test	ax,ax
	jz	@@s1
	mov	bx,ax
	mov	ax,0006h
	int	31h
	jc	@@s2
	shl	ecx,16
	mov	cx,dx
	mov	eax,ecx
	call	printeax
	pop	edx
	ret
@@s1:	mov	edx,offs selerr1
	call	prints
	pop	edx
	ret
@@s2:	mov	edx,offs selerr2
	call	prints
	pop	edx
	ret
show_xregs:
	mov	dl,56
	mov	ypos,32
	mov	xpos,dl
	mov	eax,_eip
	cmp	eax,_old_eip
	mov	color,30h
	jz	@@l1
	mov	color,3Fh
@@l1:	call	printeax
	inc	ypos
	mov	xpos,dl
	mov	eax,_cr0
	cmp	eax,_old_cr0
	mov	color,30h
	jz	@@l2
	mov	color,3Fh
@@l2:	call	printeax
	inc	ypos
	mov	xpos,dl
	mov	eax,_cr2
	cmp	eax,_old_cr2
	mov	color,30h
	jz	@@l3
	mov	color,3Fh
@@l3:	call	printeax
	inc	ypos
	mov	xpos,dl
	mov	eax,_cr3
	cmp	eax,_old_cr3
	mov	color,30h
	jz	@@l4
	mov	color,3Fh
@@l4:	call	printeax
	mov	edx,71
	mov	ypos,32
	mov	xpos,dl
	mov	eax,_efl
	cmp	eax,_old_efl
	mov	color,30h
	jz	@@l5
	mov	color,3Fh
@@l5:	jmp	printeax
show_slide:
	mov	xpos,1
	mov	ypos,37
	mov	esi,_efl
	mov	edi,_old_efl
	shl	esi,14
	shl	edi,14
	call	@@show			; VM-flag
	call	@@show			; RF-flag
	shl	esi,1
	shl	edi,1
	call	@@show			; NT-flag
	call	@@show			; IOPL-flag
	dec	xpos
	call	@@show
	call	@@show			; OF-flag
	call	@@show			; DF-flag
	call	@@show			; IF-flag
	call	@@show			; TF-flag
	call	@@show			; SF-flag
	inc	xpos
	call	@@show			; ZF-flag
	shl	esi,1
	shl	edi,1
	call	@@show			; AF-flag
	shl	esi,1
	shl	edi,1
	call	@@show			; PF-flag
	shl	esi,1
	shl	edi,1
	call	@@show			; CF-flag
	mov	xpos,53
	mov	ypos,37
	mov	esi,_cr0
	mov	edi,_old_cr0
	call	@@show			; PG-flag
	inc	xpos
	shl	esi,12
	shl	edi,12
	call	@@show			; AM-flag
	inc	xpos
	shl	esi,1
	shl	edi,1
	call	@@show			; WP-flag
	inc	xpos
	inc	xpos
	shl	esi,10
	shl	edi,10
	call	@@show			; NE-flag
	inc	xpos
	call	@@show			; ET-flag
	inc	xpos
	call	@@show			; TS-flag
	inc	xpos
	call	@@show			; EM-flag
	inc	xpos
	call	@@show			; MP-flag
	inc	xpos
@@show:	xor	ax,ax
	shl	esi,1
	adc	al,'0'
	shl	edi,1
	adc	ah,'0'
	cmp	al,ah
	mov	color,30h
	jz	@@s1
	mov	color,3Fh
@@s1:	call	printc
	inc	xpos
	ret

;-----------------------------------------------------------------------------
show_data:
	mov	bl,10
	mov	ypos,39
	mov	color,30h
	mov	esi,_data_addr
	mov	al,_data_override
	test	al,al			; 0 = es
	jnz	@@l1
	mov	edx,offs msg2es
	call	_show_data
	jmp	@@l0
@@l1:	dec	al			; 1 = cs
	jnz	@@l2
	mov	edx,offs msg2cs
	call	_show_data
	jmp	@@l0
@@l2:	dec	al			; 2 = ss
	jnz	@@l3
	mov	edx,offs msg2ss
	call	_show_data
	jmp	@@l0
@@l3:	dec	al			; 3 = ds
	jnz	@@l4
	mov	edx,offs msg2ds
	call	_show_data
	jmp	@@l0
@@l4:	dec	al			; 4 = fs
	jnz	@@l5
	mov	edx,offs msg2fs
	call	_show_data
	jmp	@@l0
@@l5:	dec	al			; 5 = gs
	jnz	@@l0
	mov	edx,offs msg2gs
	call	_show_data
@@l0:	cmp	_fpuregs_on,0
	jz	@@__0
	call	_show_fpuregs
	jmp	_show_addr

@@__0:	cmp	_fpuregs_set,0
	jz	@@__1
	mov	ebx,10
	mov	ecx,30
	mov	esi,49
	mov	edi,38
	call	clearwindow
	mov	ah,3Fh
	xor	ecx,ecx
	mov	edi,screen_base
	mov	al,	'ย'
	mov	edx,80*38+48
	call	drawchar
	mov	al,	'ฤ'
	mov	cl,30
	call	drawline
	mov	cl,30
	mov	edx,80*43+49
	call	drawline
	mov	edx,80*43+48
	mov	al,	'ร'
	call	drawchar
	mov	al,	'ถ'
	mov	edx,80*43+79
	call	drawchar
	mov	color,3Ah
	mov	xpos,51
	mov	ypos,38
	mov	edx,offs msg21
	call	prints
	mov	xpos,51
	mov	ypos,43
	mov	edx,offs msg22
	call	prints
	mov	color,30h
	mov	_fpuregs_set,0
@@__1:	mov	bl,4
	xor	ebp,ebp
	mov	edi,_esi
	mov	ypos,39
	call	_show_data_esi
	mov	bl,4
	xor	ebp,ebp
	mov	edi,_edi
	mov	ypos,44
	call	_show_data_edi
_show_addr:
	mov	xpos,60
	mov	ypos,48
	mov	color,3Fh
	clr	edx
	mov	ebx,3
	mov	eax,_mem_xpos
	sub	al,14
	div	ebx
	add	eax,_data_addr
	mov	ecx,_mem_ypos
	sub	cl,39
	lea	eax,[eax+ecx*8]
	call	printeax
	add	xpos,3
	mov	esi,eax
	call	get_data_byte
	jc	printal
	mov	edx,offs msg2er
	jmp	prints

_show_data:
@@l1:	mov	xpos,1
	call	prints
	mov	eax,esi
	call	printeax
	add	xpos,2
	mov	ecx,2708h
@@l2:	call	get_data_byte
	jc	@@l3
	mov	eax,offs msg2er
	xchg	eax,edx
	call	prints
	xchg	eax,edx
	mov	al,'?'
	jmp	@@l5
@@l3:	call	printal
	test	al,al
	jz	@@l4
	cmp	al,08h
	jz	@@l4
	cmp	al,09h
	jz	@@l4
	cmp	al,0Ah
	jz	@@l4
	cmp	al,0Dh
	jz	@@l4
	jmp	@@l5
@@l4:	mov	al,'.'
@@l5:	xchg	xpos,ch
	call	printc
	xchg	xpos,ch
	inc	xpos
	inc	esi
	dec	cl
	jnz	@@l2
	inc	ypos
	dec	bl
	jnz	@@l1
	ret
_show_data_esi:
	mov	bh,_data_override
	mov	_data_override,3
@@l1:	mov	xpos,50
	mov	eax,ebp
	test	ebp,ebp
	mov	edx,offs msg2sip
	jns	@@l01
	mov	edx,offs msg2sim
	neg	eax
@@l01:	call	prints
	call	printal
	mov	al,']'
	call	printc
	test	ebp,ebp
	jnz	@@l03
	mov	al,10h
	call	printc
	inc	xpos
	jmp	@@l04
@@l03:	add	xpos,2
@@l04:	mov	ecx,4B04h
@@l2:	lea	esi,[edi+ebp]
	call	get_data_byte
	jc	@@l3
	mov	edx,offs msg2er
	call	prints
	mov	al,'?'
	jmp	@@l5
@@l3:	call	printal
	test	al,al
	jz	@@l4
	cmp	al,08h
	jz	@@l4
	cmp	al,09h
	jz	@@l4
	cmp	al,0Ah
	jz	@@l4
	cmp	al,0Dh
	jz	@@l4
	jmp	@@l5
@@l4:	mov	al,'.'
@@l5:	xchg	xpos,ch
	call	printc
	xchg	xpos,ch
	inc	xpos
	inc	ebp
	dec	cl
	jnz	@@l2
	inc	ypos
	dec	bl
	jnz	@@l1
	mov	_data_override,bh
	ret
_show_data_edi:
	mov	bh,_data_override
	mov	_data_override,0
@@l1:	mov	xpos,50
	mov	eax,ebp
	test	ebp,ebp
	mov	edx,offs msg2dip
	jns	@@l01
	mov	edx,offs msg2dim
	neg	eax
@@l01:	call	prints
	call	printal
	mov	al,']'
	call	printc
	test	ebp,ebp
	jnz	@@l03
	mov	al,10h
	call	printc
	inc	xpos
	jmp	@@l04
@@l03:	add	xpos,2
@@l04:	mov	ecx,4B04h
@@l2:	lea	esi,[edi+ebp]
	call	get_data_byte
	jc	@@l3
	mov	edx,offs msg2er
	call	prints
	mov	al,'?'
	jmp	@@l5
@@l3:	call	printal
	test	al,al
	jz	@@l4
	cmp	al,08h
	jz	@@l4
	cmp	al,09h
	jz	@@l4
	cmp	al,0Ah
	jz	@@l4
	cmp	al,0Dh
	jz	@@l4
	jmp	@@l5
@@l4:	mov	al,'.'
@@l5:	xchg	xpos,ch
	call	printc
	xchg	xpos,ch
	inc	xpos
	inc	ebp
	dec	cl
	jnz	@@l2
	inc	ypos
	dec	bl
	jnz	@@l1
	mov	_data_override,bh
	ret

;-----------------------------------------------------------------------------
_show_fpuregs:
	cmp	_fpuregs_set,0
	jnz	@@1
	mov	ebx,10
	mov	ecx,30
	mov	esi,49
	mov	edi,38
	call	clearwindow
	mov	ah,3Fh
	xor	ecx,ecx
	mov	edi,screen_base
	mov	al,	'ย'
	mov	edx,80*38+48
	call	drawchar
	mov	al,	'ฤ'
	mov	cl,30
	call	drawline
	mov	edx,80*39+48
	mov	al,	'ณ'
	mov	cl,9
	call	drawdown
	mov	al,	'บ'
	mov	edx,80*43+79
	call	drawchar
	mov	color,3Ah
	mov	xpos,51
	mov	ypos,38
	mov	edx,offs msg10f
	call	prints
	mov	color,30h
	mov	xpos,49
	mov	ypos,39
	mov	edx,offs msg11f
	call	prints
	mov	xpos,49
	inc	ypos
	mov	edx,offs msg12f
	call	prints
	mov	xpos,49
	inc	ypos
	mov	edx,offs msg13f
	call	prints
	mov	xpos,49
	inc	ypos
	mov	edx,offs msg14f
	call	prints
	mov	xpos,49
	inc	ypos
	mov	edx,offs msg15f
	call	prints
	mov	xpos,49
	inc	ypos
	mov	edx,offs msg16f
	call	prints
	mov	xpos,49
	inc	ypos
	mov	edx,offs msg17f
	call	prints
	mov	xpos,49
	inc	ypos
	mov	edx,offs msg18f
	call	prints
	mov	xpos,49
	inc	ypos
	mov	edx,offs msg19f
	call	prints
	mov	_fpuregs_set,1

@@1:	mov	color,30h
	mov	xpos,55
	mov	ypos,39
	sub	esp,108
	mov	edi,28		; displacement
	mov	bl,8		; counter
	clts			; clear task switched bit
	fsave	[esp]
	movzx	eax,word ptr [esp+8]
	mov	_temp,eax
	mov	eax,[esp+4]
	shr	eax,11
	and	eax,07h
	lea	ecx,[eax*2]
	ror	word ptr _temp,cl

@@loop:	mov	eax,_temp
	ror	word ptr _temp,02h
	and	eax,0003h
	cmp	eax,0003h
	jz	@@__ftoa_empty
	fld	tbyte ptr ss:[esp+edi]
	fxam
	fstsw	ax
	mov	al,ah
	and	al,01000101b
	jz	@@__ftoa_err	; unsupported number
	cmp	al,00000001b
	jz	@@__ftoa_NaN	; NaN number
	cmp	al,00000101b
	jz	@@__ftoa_inf	; infinity
	cmp	al,01000000b
	jz	@@__ftoa_zero	; zero
	cmp	al,01000100b
	jz	@@__ftoa_den	; denormal
	cmp	al,01000001b
	jz	@@__ftoa_unu	; unused
	cmp	al,01000101b
	jz	@@__ftoa_unu	; unused
	mov	eax,57
	call	_float2dec
	add	eax,17
	mov	edx,eax		; save exponent in edx

	sub	esp,12
	fbstp	tbyte ptr [esp]
	mov	cx,[esp+8]
	mov	esi,[esp+4]
	mov	ebp,[esp+0]
	add	esp,12

	mov	bh,15
	and	ch,80h
	mov	al,'+'
	jz	@@2
	mov	al,'-'
@@2:	call	printc
	call	__ftoa_char
	call	printc
	mov	al,'.'
	call	printc
@@3:	call	__ftoa_char
	call	printc
	dec	bh
	jnz	@@3

	mov	al,' '
	call	printc
	mov	al,'E'
	call	printc
	test	edx,edx
	mov	al,'+'
	jns	@@4
	mov	al,'-'
	neg	edx
@@4:	call	printc

	mov	bh,3
	mov	eax,edx
	xor	ebp,ebp
@@5:	mov	ecx,10
	xor	edx,edx
	idiv	ecx
	shl	ebp,8
	or	ebp,edx
	dec	bh
	jnz	@@5

	mov	bh,3
@@6:	mov	eax,ebp
	add	al,30h
	shr	ebp,8
	call	printc
	dec	bh
	jnz	@@6

@@next:	add	edi,10
	mov	xpos,55
	inc	ypos
	dec	bl
	jnz	@@loop

	mov	xpos,52
	mov	ax,[esp+0]	; CW
	call	printax
	add	xpos,5
	mov	ax,[esp+4]	; SW
	call	printax
	add	xpos,6
	mov	ax,[esp+8]	; Tag
	call	printax

	frstor	[esp]
	add	esp,108
	ret


@@__ftoa_err:
	mov	edx,offs msg1Ef1
	jmp	@@__ftoa_
@@__ftoa_NaN:
	mov	edx,offs msg1Ef2
	jmp	@@__ftoa_
@@__ftoa_inf:
	and	ah,00000010h
	mov	al,'+'
	jz	@@__ftoa_inf_
	mov	al,'-'
@@__ftoa_inf_:
	mov	edx,offs msg1Ef3
	mov	[edx],al
	jmp	@@__ftoa_
@@__ftoa_zero:
	and	ah,00000010h
	mov	al,'+'
	jz	@@__ftoa_zero_
	mov	al,'-'
@@__ftoa_zero_:
	mov	edx,offs msg1Ef4
	mov	[edx],al
	jmp	@@__ftoa_
@@__ftoa_den:
	mov	edx,offs msg1Ef5
	jmp	@@__ftoa_
@@__ftoa_unu:
	mov	edx,offs msg1Ef6
	jmp	@@__ftoa_
@@__ftoa_empty:
	mov	edx,offs msg1Ef7
@@__ftoa_:
	call	prints
	jmp	@@next

__ftoa_char:
	mov	al,cl
	shl	ebp,1
	rcl	esi,1
	rcl	cl,1
	shl	ebp,1
	rcl	esi,1
	rcl	cl,1
	shl	ebp,1
	rcl	esi,1
	rcl	cl,1
	shl	ebp,1
	rcl	esi,1
	rcl	cl,1
	shr	al,4
	add	al,30h
	ret

_float2dec:
	sub	esp,3*4
	fstcw	wptr [esp]
	mov	wptr [esp+2],03BFh
	fldcw	wptr [esp+2]
	mov	dptr [esp+4],eax
	fld	st(0)
	fxtract
	fstp	st(0)
	fisubr	dptr [esp+4]
	fldl2t
	fdiv
	frndint
	fist	dptr [esp+8]
	call	_exp10
	fmul
	fldcw	wptr [esp]
	add	esp,2*4
	pop	eax
	neg	eax
	ret

_exp10:	fldl2t
	fmul
_exp2:	sub	esp,2*4
	fstcw	wptr [esp]
	mov	wptr [esp+2],03BFh
	fldcw	wptr [esp+2]
	fld	st(0)
	frndint
	fxch
	fsub	st,st(1)
	ftst
	fstsw	wptr [esp+4]
	and	bptr [esp+5],45h
	cmp	bptr [esp+5],01h
	ja	@@err
	je	@@neg
	f2xm1
	fld1
	fadd
	fxch
	fld1
	fscale
	fmulp	st(2),st
	fstp	st(0)
	jmp	@@done
@@neg:	fabs
	f2xm1
	fld1
	fadd
	fxch
	fld1
	fscale
	fdivrp	st(2),st
	fstp	st(0)
@@done:	fldcw	wptr [esp]
	add	esp,2*4
	ret
@@err:	fstp	st(0)
	fstp	st(0)
	fld1
	jmp	@@done


;-----------------------------------------------------------------------------
show_stack:
	mov	ypos,30
	mov	color,30h
	mov	cl,30
	mov	edi,_esp
	add	edi,_stack_addr
@@l1:	mov	eax,edi
	sub	eax,_esp
	mov	edx,offs msg3spp
	test	eax,eax
	jns	@@l2
	mov	edx,offs msg3spm
	neg	eax
@@l2:	mov	xpos,62
	call	prints
	call	printal
	mov	al,']'
	call	printc
@@l20:	cmp	edi,_esp
	mov	al,10h
	jz	@@l3
	cmp	edi,_ebp
	mov	al,'>'
	jz	@@l3
	mov	al,20h
@@l3:	call	printc
	lea	esi,[edi+3]
	call	get_stack_byte
	dec	esi
	shl	eax,8
	call	get_stack_byte
	dec	esi
	shl	eax,8
	call	get_stack_byte
	dec	esi
	shl	eax,8
	call	get_stack_byte
	cmp	_assmmode_flag,0
	jz	@@l4
	call	printax
	add	edi,2
	jmp	@@l5
@@l4:	call	printeax
	add	edi,4
@@l5:	dec	ypos
	dec	cl
	jnz	@@l1
	cmp	_current_window,3
	jz	_show_addr_stk
	ret
_show_addr_stk:
	mov	xpos,60
	mov	ypos,48
	mov	color,3Fh
	mov	eax,_stack_addr
	add	eax,_esp
	call	printeax
	add	xpos,3
	mov	esi,eax
	call	get_stack_byte
	jc	printal
	mov	edx,offs msg2er
	jmp	prints




;-----------------------------------------------------------------------------
show_cpu:
	mov	xpos,3
	mov	ypos,0
	mov	edx,offs msg00
	call	prints
_show_cpu:
	movzx	edx,__cputype
	lea	edx,cpu_table[edx*8]
	jmp	prints
show_fpu:
_show_fpu:
	movzx	edx,__cputype
	lea	edx,fpu_table[edx*8]
	jmp	prints


;-----------------------------------------------------------------------------
drawframe:
	mov	ah,3Fh		; color
	mov	edi,screen_base
	xor	edx,edx
	xor	ecx,ecx

	mov	al,	'ษ'
	call	drawchar
	mov	al,	'อ'
	mov	cl,4Eh
	call	drawline
	mov	al,	'ป'
	call	drawchar

	mov	al,	'บ'	; left border
	mov	edx,0050h
	mov	cl,30h
	call	drawdown
	mov	al,	'ศ'
	call	drawchar

	mov	al,	'บ'	; right border
	mov	edx,009Fh
	mov	cl,30h
	call	drawdown
	mov	al,	'ผ'
	call	drawchar

	mov	al,	'ว'
	mov	edx,80*31
	call	drawchar
	mov	al,	'ฤ'
	mov	cl,78
	call	drawline
	mov	al,	'ถ'
	call	drawchar

	mov	al,	'ว'
	mov	edx,80*38
	call	drawchar
	mov	al,	'ฤ'
	mov	cl,78
	call	drawline
	mov	al,	'ถ'
	call	drawchar

	mov	al,	'ย'
	mov	edx,80*38+48
	call	drawchar
	mov	edx,80*39+48
	mov	al,	'ณ'
	mov	cl,10
	call	drawdown

	mov	al,	'ร'
	mov	edx,80*43+48
	call	drawchar
	mov	al,	'ฤ'
	mov	cl,30
	call	drawline
	mov	al,	'ถ'
	call	drawchar

	mov	al,	'ั'
	mov	edx,61
	call	drawchar
	mov	al,	'ณ'
	mov	edx,80+61
	mov	cl,30
	call	drawdown
	mov	al,	'ม'
	call	drawchar

	mov	al,	'ร'
	mov	edx,80*48+48
	call	drawchar
	mov	al,	'ฤ'
	mov	cl,30
	call	drawline
	mov	al,	'ถ'
	call	drawchar

	call	print_cmdline
	mov	color,3Ah
	call	show_cpu
	call	show_fpu
	mov	color,3Ah
	mov	xpos,3
	mov	ypos,31
	mov	edx,offs msg10
	call	prints
	mov	color,30h
	mov	xpos,1
	mov	ypos,32
	mov	edx,offs msg11
	call	prints
	mov	xpos,1
	mov	ypos,33
	mov	edx,offs msg12
	call	prints
	mov	xpos,1
	mov	ypos,34
	mov	edx,offs msg13
	call	prints
	mov	xpos,1
	mov	ypos,35
	mov	edx,offs msg14
	call	prints
	mov	xpos,1
	mov	ypos,36
	mov	edx,offs msg15
	call	prints
	mov	xpos,1
	mov	ypos,37
	mov	edx,offs msg16
	call	prints
	mov	xpos,3
	mov	ypos,38
	mov	color,3Ah
	mov	edx,offs msg20
	call	prints
	mov	xpos,51
	mov	ypos,38
	mov	edx,offs msg21
	call	prints
	mov	xpos,51
	mov	ypos,43
	mov	edx,offs msg22
	call	prints
	mov	xpos,64
	mov	ypos,0
	mov	edx,offs msg30
	call	prints
	mov	xpos,51
	mov	ypos,48
	mov	edx,offs msg40
	jmp	prints

clear_cmdline:
	mov	xpos,1
	mov	ypos,49
	mov	color,70h
	mov	al,' '
	mov	ecx,78
@@loop:	call	printc
	loop	@@loop
	ret
print_cmdline:
	call	clear_cmdline
	mov	xpos,1
	mov	edx,offs msg50
	jmp	prints

print_errline:
	movzx	ebx,al
	mov	xpos,1
	mov	ypos,49
	mov	color,4Fh
	mov	al,' '
	mov	ecx,78
@@loop:	call	printc
	loop	@@loop
	mov	xpos,1

	cmp	bl,4Ch
	jz	@@term
	cmp	bl,0FFh
	jz	@@kbrk

	cmp	bl,16
	mov	edx,offs err0
	jb	@@l1
	mov	edx,offs err1
@@l1:	call	prints
	mov	edx,errortab[ebx*4]
	call	prints
	cmp	bl,8
	jb	@@l2
	cmp	bl,16
	ja	@@l2
	mov	edx,offs errec
	call	prints
	mov	eax,_errorcode
	call	printeax
@@l2:	ret

@@term:	mov	edx,offs errterm
	jmp	prints
@@kbrk:	mov	edx,offs errkbrk
	jmp	prints



;-----------------------------------------------------------------------------
_cursor:pushad
	lea	edx,[edi*8+edi]
	add	edx,edi
	shl	edx,4
	lea	esi,[edx+esi*2]
	add	esi,screen_base
@@loop:	xor	[esi],ax
	add	esi,2
	dec	cl
	jnz	@@loop
	popad
	ret
_cursorx:
	pushad
	lea	edx,[edi*8+edi]
	add	edx,edi
	shl	edx,4
	lea	esi,[edx+esi*2]
	add	esi,screen_base
@@loop:	mov	al,[esi+1]
	cmp	al,17h
	jz	@@l1
	cmp	al,1Fh
	jz	@@l2
	cmp	al,30h
	jz	@@l3
	cmp	al,3Fh
	jz	@@l4
	jmp	@@next
@@l1:	mov	al,3Fh
	jmp	@@next
@@l2:	mov	al,30h
	jmp	@@next
@@l3:	mov	al,1Fh
	jmp	@@next
@@l4:	mov	al,17h
@@next:	mov	[esi+1],al
	add	esi,2
	dec	cl
	jnz	@@loop
	popad
	ret
syncdisplay:
	push	eax edx
	mov	dx,03DAh
@@l1:	in	al,dx
	test	al,08h
	jnz	@@l1
@@l2:	in	al,dx
	test	al,08h
	jz	@@l2
	pop	edx eax
	ret
screen_on:
	mov	dx,03C4h
	mov	al,01
	out	dx,al
	inc	dx
	in	al,dx
	and	al,0DFh
	out	dx,al
	ret
screen_off:
	mov	dx,03C4h
	mov	al,01
	out	dx,al
	inc	dx
	in	al,dx
	or	al,20h
	out	dx,al
	ret
clearwindow:
	call	syncdisplay
	lea	edx,[edi*8+edi]
	add	edx,edi
	shl	edx,4
	lea	edi,[edx+esi*2]
	add	edi,screen_base
	lea	edx,[ecx*2]
	mov	ax,3F00h
@@loop:	push	ecx
	rep	stosw
	pop	ecx
	sub	esi,edx
	sub	edi,edx
	add	esi,80*2
	add	edi,80*2
	dec	ebx
	jnz	@@loop
	ret
drawchar:
	mov	[edi+edx*2],ax
	inc	edx
	ret
drawline:
	mov	[edi+edx*2],ax
	inc	edx
	loop	drawline
	ret
drawdown:
	mov	[edi+edx*2],ax
	add	edx,50h
	loop	drawdown
	ret
printc:	push	edx edi
	cmp	al,09h
	jz	@@09
	cmp	al,0Ah
	jz	@@0A
	cmp	al,0Dh
	jz	@@0D
	movzx	edi,ypos
	lea	edx,[edi*8+edi]
	add	edx,edi
	shl	edx,3
	movzx	edi,xpos
	add	edx,edi
	mov	edi,screen_base
	mov	ah,color
	mov	[edi+edx*2],ax
	inc	xpos
	cmp	xpos,50h
	jb	@@done
	mov	xpos,0
	jmp	@@0A
@@done:	pop	edi edx
	ret
@@0D:	mov	xpos,0
	pop	edi edx
	ret
@@0A:	inc	ypos
	cmp	ypos,32h
	jb	@@done
	mov	ypos,0
	pop	edi edx
	ret
@@09:	and	xpos,0F8h
	add	xpos,8
	cmp	xpos,80
	jb	@@done
	sub	xpos,80
	jmp	@@0A
prints:	push	eax edx
@@loop:	mov	al,[edx]
	inc	edx
	test	al,al
	jz	@@done
	cmp	al,08h
	jz	@@l1
	call	printc
	jmp	@@loop
@@l1:	mov	al,[edx]
	inc	edx
	test	al,al
	jz	@@done
	mov	color,al
	jmp	@@loop
@@done:	pop	edx eax
	ret
printsback:
	push	eax ebx
	clr	ebx
@@l1:	mov	al,[edx+ebx]
	inc	ebx
	test	al,al
	jnz	@@l1
	sub	xpos,bl
	jnc	@@l2
	mov	xpos,0
@@l2:	call	prints
	pop	ebx eax
	ret
printal:push	edi
	mov	edi,offs hexbuf
	call	makehex
	lea	edi,[edi+6]
	jmp	printhex
printax:push	edi
	mov	edi,offs hexbuf
	call	makehex
	lea	edi,[edi+4]
	jmp	printhex
printeax:
	push	edi
	mov	edi,offs hexbuf
	call	makehex
printhex:
	push	edx
	mov	edx,edi
	call	prints
	pop	edx edi
	ret
makehex:pushad
	mov	edx,7
@@loop:	mov	esi,eax
	and	esi,0Fh
	mov	bl,hextab[esi]
	mov	[edi+edx],bl
	shr	eax,4
	sub	edx,1
	jnc	@@loop
	popad
	ret
get_key:
	push	ebx ecx edx
	mov	bl,xpos
	mov	bh,ypos
	mov	cl,color
	mov	xpos,75
	mov	ypos,0
	mov	edx,offs msgrdy
	call	prints
	sti
	mov	_keyshift,0
@@loop:	cmp	_keyshift,0
	jnz	@@1
	cmp	_keyhit,0
	jz	@@loop
@@1:	mov	_keyhit,0
	cli
	mov	xpos,75
	mov	ypos,0
	mov	edx,offs msgbusy
	call	prints
	mov	xpos,bl
	mov	ypos,bh
	mov	color,cl
	movzx	eax,_keycode
	pop	edx ecx ebx
	ret

clearkeytab:
	pushad
	clr	eax
	mov	ecx,20h
	mov	edi,offs _keytab
	rep	stosd
	mov	_keyshift,0
	mov	_keychar,0
	mov	_keycode,0
	mov	_keyhit,0
	popad
	ret
beep:	push	eax ecx
	mov	al,0B6h		; set frequency
	out	43h,al
	mov	al,0		; fL
	out	42h,al
	mov	al,5		; fH
	out	42h,al
	in	al,61h		; beep on
	or	al,03h
	out	61h,al
	mov	ecx,3FFFh
@@loop:	in	al,42h
	in	al,42h
	mov	ah,al
@@0:	in	al,40h
	in	al,40h
	cmp	ah,al
	je	@@0
	loop	@@loop
	in	al,61h		; beep off
	and	al,not 03h
	out	61h,al
	pop	ecx eax
	ret


selector_init:
	mov	bx,wptr _es
	call	get_selector
	mov	_es_acc,bx
	mov	_es_base,esi
	mov	_es_limit,edi
	mov	bx,wptr _cs
	call	get_selector
	mov	_cs_acc,bx
	mov	_cs_base,esi
	mov	_cs_limit,edi
	test	cl,40h
	mov	_assmmode_flag,0
	jnz	@@l1
	mov	_assmmode_flag,1
@@l1:	mov	bx,wptr _ss
	call	get_selector
	mov	_ss_acc,bx
	mov	_ss_base,esi
	mov	_ss_limit,edi
	mov	bx,wptr _ds
	call	get_selector
	mov	_ds_acc,bx
	mov	_ds_base,esi
	mov	_ds_limit,edi
	mov	bx,wptr _fs
	call	get_selector
	mov	_fs_acc,bx
	mov	_fs_base,esi
	mov	_fs_limit,edi
	mov	bx,wptr _gs
	call	get_selector
	mov	_gs_acc,bx
	mov	_gs_base,esi
	mov	_gs_limit,edi
	ret
get_selector:
	mov	ax,000Bh
	mov	edi,offs es:selbuf
	int	31h
	jc	@@err
	mov	ah,bptr es:selbuf[7]
	mov	al,bptr es:selbuf[4]
	shl	eax,16
	mov	ax,wptr es:selbuf[2]
	mov	esi,eax			; ESI = base
	mov	al,bptr es:selbuf[6]
	mov	bl,bptr es:selbuf[5]	; BL = access rights
	mov	bh,al
	mov	cl,al			; CL = additional fields
	mov	ch,al
	and	eax,0Fh
	shl	eax,16
	mov	ax,wptr es:selbuf[0]
	shr	ch,7			; CH = granularity
	jz	@@l1
	shl	eax,12			; page granular
	add	eax,0FFFh		; one page is 4096 bytes
@@l1:	mov	edi,eax			; EDI = limit
	ret
@@err:	clr	eax
	mov	ebx,eax
	mov	ecx,eax
	mov	esi,eax
	mov	edi,eax
	ret



;-----------------------------------------------------------------------------
;	CPU - Window
;=============================================================================
_cpu_window:
	push	offs _cpu_window
	call	get_key

;push eax
;mov ah,_keycode
;mov _eax,eax
;call show_regs
;pop eax

	cmp	wptr _keychar,0278h	; alt-x
	jz	_exit
	cmp	al,01h			; ESC
	jz	_redraw_now
	cmp	al,15			; TAB
	jz	window_switch
	cmp	al,37h			; Print-Screen
	jz	dump_screen

	cmp	al,48h			; up
	jz	_cpu_up
	cmp	al,50h			; down
	jz	_cpu_down
	cmp	al,4Bh			; left
	jz	_cpu_left
	cmp	al,4Dh			; right
	jz	_cpu_right
	cmp	al,49h			; page-up
	jz	_cpu_pageup
	cmp	al,51h			; page-down
	jz	_cpu_pagedn
	cmp	al,47h			; home
	jz	_cpu_home
	cmp	al,1Ch			; ENTER
	jz	_cpu_newaddr
	cmp	al,4Eh			; '+'
	jz	_cpu_incsel
	cmp	al,4Ah			; '-'
	jz	_cpu_decsel
;	cmp	al,52h			; Insert
;	jz	_cpu_setbreakpoint
;	cmp	al,53h			; Delete
;	jz	_cpu_resetbreakpoint

	cmp	al,3Bh			; F1 - Info
	jz	_show_info
	cmp	al,3Ch			; F2 - Break point
	jz	_break_pnt
	cmp	al,3Dh			; F3 - Return
	jz	_return_to
	cmp	al,3Eh			; F4 - Here
	jz	_come_here

	cmp	al,3Fh			; F5 - Swap screen
	jz	_swap_screen
	cmp	al,40h			; F6 - New address
	jz	_new_address
	cmp	al,41h			; F7 - Trace
	jz	_trace_into
	cmp	al,42h			; F8 - Step over
	jz	_step_over

	cmp	al,43h			; F9 - Proceed
	jz	_proceed

	cmp	al,44h			; F10 - Jump over
	jz	_jump_over
	cmp	al,57h			; F11 - Search
	jz	_cpu_search
	cmp	al,58h			; F12 - Search Next
	jz	_cpu_searchnext


	cmp	al,02h			; 1 - 16bit/32bit text
	jz	_cpu_showassm
	cmp	al,03h			; 2 - show hex data
	jz	_cpu_showcode
	cmp	al,04h			; 3 - FPU Registers
	jz	_cpu_fpuregs
	cmp	al,33h			; ',' - FPU Registers
	jz	_cpu_fpuregs
	cmp	al,05h			; 4 - unrelocated EIP
	jz	_cpu_unreloc
	cmp	al,34h			; '.' - unrelocated EIP
	jz	_cpu_unreloc
	ret





;-----------------------------------------------------------------------------
_cpu_cursor:
	mov	esi,_cpu_xpos
	mov	edi,_cpu_ypos
	mov	ecx,60
	mov	ax,2F00h
	call	_cursor
	mov	ax,3FCDh
	mov	ecx,29
	mov	edx,32
	mov	edi,screen_base
	call	drawline
	mov	edx,offs commbuf
	cmp	bptr [edx],20h
	jnz	@@l1
	ret
@@l1:	mov	xpos,61
	mov	ypos,0
	mov	color,3Fh
	jmp	printsback
_cpu_up:call	show_cursor
	cmp	_cpu_ypos,1
	jbe	@@l1
	dec	_cpu_ypos
	mov	ebx,_cpu_ypos
	dec	ebx
	mov	esi,_addrbuffer[ebx*4]
	call	decode
	jmp	show_cursor
@@l1:	call	find_code_up
	mov	_code_addr,eax
	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
	call	show_text
	jmp	show_cursor
_cpu_down:
	call	show_cursor
	cmp	_cpu_ypos,30
	jae	@@l1
	inc	_cpu_ypos
	mov	ebx,_cpu_ypos
	dec	ebx
	mov	esi,_addrbuffer[ebx*4]
	call	decode
	jmp	show_cursor
@@l1:	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
	mov	eax,_addrbuffer[4]
	mov	_code_addr,eax
	call	show_text
	jmp	show_cursor
_cpu_left:
	call	show_cursor
	dec	_code_addr
	jmp	_cpu_common
_cpu_right:
	call	show_cursor
	inc	_code_addr
	jmp	_cpu_common
_cpu_pageup:
	mov	ecx,30
@@l1:	call	find_code_up
	mov	_addrbuffer[0],eax
	loop	@@l1
	mov	_code_addr,eax
	jmp	_cpu_common
_cpu_pagedn:
	mov	esi,_addrbuffer[29*4]
	call	decode
	mov	_code_addr,esi
	jmp	_cpu_common
_cpu_home:
	mov	eax,_eip
	mov	_code_addr,eax
	mov	_cpu_ypos,1
_cpu_common:
	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
	call	show_text
	jmp	show_cursor
find_code_up:
	mov	eax,_addrbuffer[0]
	sub	eax,16			; average opcode length
@@l1:	inc	eax
	mov	ebx,eax
	mov	esi,eax
@@l2:	mov	esi,ebx
	mov	edx,ebx
	call	decode
	mov	ebx,esi
	sub	esi,_addrbuffer[0]
	jb	@@l2
	ja	@@done
@@done:	mov	eax,edx
	ret
_cpu_showassm:
	xor	_showmode_flag,1
	xor	_commmode_flag,1
	jmp	repaint_text
_cpu_showcode:
	xor	_assmmode_flag,1
	jmp	repaint_text
_cpu_incsel:
	call	show_text_clr
	jmp	_mem_incsel
_cpu_decsel:
	call	show_text_clr
	jmp	_mem_decsel
_cpu_fpuregs:
	cmp	__fputype,0
	jz	@@1
	xor	_fpuregs_on,1
	jmp	show_data
@@1:	ret
_cpu_unreloc:
	xor	_show_unreloc,1
	jmp	repaint_text

_cpu_newaddr:
	call	clear_cmdline
	mov	xpos,1
	mov	edx,offs msg57
	call	prints
	call	_input
	cmp	al,1
	jz	@@end

	clr	ebx
	mov	cl,8
	mov	edx,offs _fname
	cmp	bptr [edx],'.'
	setz	ch
	jnz	@@1
	inc	edx
@@1:	cmp	dptr [edx],00584145h		;EAX
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00584245h		;EBX
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00584345h		;ECX
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00584445h		;EDX
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00495345h		;ESI
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00494445h		;EDI
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00504245h		;EBP
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00505345h		;ESP
	jz	@@reg
	clr	ebx
	cmp	dptr [edx],00504945h		;EIP
	jnz	@@loop
	mov	ebx,_eip
	jmp	@@doit
@@reg:	mov	eax,_reg_rtab[ebx*4]
	mov	ebx,[eax]
	jmp	@@doit

@@loop:	mov	al,[edx]
	inc	edx
	test	al,al
	jz	@@done
	sub	al,30h
	jc	@@err
	cmp	al,9
	jbe	@@m0
	sub	al,7
	cmp	al,0Fh
	ja	@@err
@@m0:	rol	ebx,4
	or	bl,al
	dec	cl
	jnz	@@loop
@@done:	cmp	cl,8
	jz	@@end
@@doit:	mov	eax,ebx
	test	ch,ch
	jz	@@unrl
	mov	dx,wptr _cs
	call	make_unreloc
@@unrl:	mov	_code_addr,eax
	call	show_cursor
	mov	_cpu_ypos,1
	call	show_text_clr
	call	show_cursor
@@end:	jmp	print_cmdline
@@err:	call	clear_cmdline
	mov	edx,offs msg58
	mov	xpos,1
	mov	ypos,49
	call	prints
	call	beep
	call	beep
	call	get_key
	jmp	print_cmdline

make_unreloc:
	clr	ebx
@@0:	cmp	_obj_selector[ebx*2],0
	jz	@@err
	cmp	dx,_obj_selector[ebx*2]
	jz	@@1
	inc	ebx
	jmp	@@0
@@1:	add	eax,_obj_address[ebx*4]
	clc
	ret
@@err:	stc
	ret


_cpu_searchnext:
	call	clear_cmdline
	mov	xpos,1
	mov	edx,offs msg61
	call	prints
	clr	ebx
	clr	ecx
@@zx:	mov	al,_fname[ebx]
	test	al,al
	jz	@@zz
	cmp	al,9
	jnz	@@zy
	mov	al,20h
@@zy:	call	printc
	inc	ebx
	jmp	@@zx
@@zz:	xor	eax,eax
	jmp	_cpu_search_common

_cpu_search:
	call	clear_cmdline
	mov	xpos,1
	mov	edx,offs msg60
	call	prints
	call	_inputl
_cpu_search_common:
	cmp	al,1
	jz	@@end
	cmp	bptr _fname[0],0
	jz	@@end
	mov	xpos,1
	mov	edx,offs msg61
	call	prints
	mov	eax,_cpu_ypos
	mov	esi,_addrbuffer[eax*4]
	mov	ebx,esi
	mov	ecx,-1
	sti
@@l00:	mov	eax,ebx
	mov	esi,ebx
	mov	xpos,23
	call	printeax
	call	decode
	mov	edi,offs _fname
	mov	edx,offs cmdbuf
@@loop:	mov	al,[edx]
	inc	edx
	test	al,al
	jz	@@done
	mov	ah,[edi]
	inc	edi
	test	ah,ah
	jz	@@done
	cmp	ah,'*'
	jz	@@loop
	cmp	al,ah
	jz	@@loop
@@llll:	cmp	_keytab[1],0
	jnz	@@err
	mov	ebx,esi
	loop	@@l00
	jmp	@@err
@@done:	cmp	edx,offs cmdbuf+1
	jz	@@llll
	cli
	mov	_keyhit,0
	mov	_code_addr,ebx
	call	show_cursor
	mov	_cpu_ypos,1
	call	_cpu_common
@@end:	jmp	print_cmdline
@@err:	cli
	mov	_keyhit,0
	call	clear_cmdline
	mov	edx,offs msg62
	mov	xpos,1
	mov	ypos,49
	call	prints
	call	beep
	call	beep
	call	get_key
	jmp	print_cmdline

_input:	clr	ebx
	mov	cl,32
@@loop:	mov	al,'_'
	call	printc
	dec	xpos
	call	get_key
	cmp	al,1
	jz	@@end
@@l0x:	mov	al,_keychar
	cmp	al,80h
	jae	@@loop
	cmp	al,0Dh
	jz	@@load
	cmp	al,10h
	jz	@@l1x
	cmp	al,20h
	jbe	@@loop
	cmp	al,'a'
	jb	@@next
	sub	al,20h
	jmp	@@next
@@l1x:	test	ebx,ebx
	jz	@@loop
	dec	ebx
	inc	cl
	mov	al,' '
	call	printc
	sub	xpos,2
	jmp	@@loop
@@next:	call	printc
	mov	_fname[ebx],al
	inc	ebx
	dec	cl
	jnz	@@loop
@@load:	mov	bptr _fname[ebx],0
	clr	eax
@@end:	ret

_inputl:clr	ebx
	clr	ecx
@@zx:	mov	al,_fname[ebx]
	test	al,al
	jz	@@zz
	cmp	al,9
	jnz	@@zy
	mov	al,20h
@@zy:	call	printc
	inc	ebx
	jmp	@@zx
@@zz:	mov	cl,32
	sub	cl,bl
	jnc	@@loop
	clr	ecx
@@loop:	mov	al,'_'
	call	printc
	dec	xpos
	call	get_key
	cmp	al,1
	jz	@@end
@@l0x:	mov	al,_keychar
	cmp	al,80h
	jae	@@loop
	cmp	al,0Dh
	jz	@@load
	cmp	al,10h
	jz	@@l1x
	cmp	al,0Fh
	jz	@@nxt
	cmp	al,20h
	jb	@@loop
	jmp	@@next
@@l1x:	test	ebx,ebx
	jz	@@loop
	dec	ebx
	inc	cl
	mov	al,' '
	call	printc
	sub	xpos,2
	jmp	@@loop
@@nxt:	mov	al,20h
@@next:	jecxz	@@load
	call	printc
	cmp	al,20h
	jnz	@@cn
	mov	al,9
@@cn:	mov	_fname[ebx],al
	inc	ebx
	dec	cl
	jnz	@@loop
@@load:	mov	bptr _fname[ebx],0
	clr	eax
@@end:	ret


dump_screen:
	mov	dptr _dumpname[4],'0000'
@@l1:	mov	ax,3DC0h
	mov	edx,offs _dumpname
	int	21h
	jc	@@l2
	mov	bx,ax
	mov	ax,3E00h
	int	21h
	inc	bptr _dumpname[7]
	cmp	bptr _dumpname[7],'9'
	jbe	@@l1
	mov	bptr _dumpname[7],'0'
	inc	bptr _dumpname[6]
	cmp	bptr _dumpname[6],'9'
	jbe	@@l1
	mov	bptr _dumpname[6],'0'
	cli
	ret

@@l2:	mov	ax,3C00h
	mov	ecx,0000h
	mov	edx,offs _dumpname
	int	21h
	jc	@@err
	push	eax
	mov	ebx,50
	mov	esi,0BE000h
	mov	edi,__buffer
@@l3:	mov	ecx,80
@@l4:	mov	al,[esi]
	test	al,al
	jz	@@l5
	cmp	al,0Ah
	jz	@@l5
	cmp	al,0Dh
	jz	@@l5
	cmp	al,10h
	jb	@@l51
	jmp	@@l6
@@l5:	mov	al,20h
	jmp	@@l6
@@l51:	mov	al,'.'
@@l6:	mov	[edi],al
	add	esi,2
	add	edi,1
	loop	@@l4
	mov	wptr [edi],0A0Dh
	add	edi,2
	dec	ebx
	jnz	@@l3
	pop	ebx
	push	ebx
	mov	ax,4000h
	mov	ecx,4100
	mov	edx,__buffer
	int	21h
	pop	ebx
	jc	@@err
	mov	ax,3E00h
	int	21h
@@err:	cli
	ret




;-----------------------------------------------------------------------------
;	MEMORY - Window
;=============================================================================
_mem_window:
	push	offs _mem_window
	call	_show_addr

	call	get_key
	cmp	wptr _keychar,0278h	; alt-x
	jz	_exit
	cmp	al,1			; ESC
	jz	switch_to_cpu
	cmp	al,15			; TAB
	jz	window_switch
	cmp	al,37h			; Print-Screen
	jz	dump_screen

	cmp	al,48h			; up
	jz	_mem_up
	cmp	al,50h			; down
	jz	_mem_down
	cmp	al,4Bh			; left
	jz	_mem_left
	cmp	al,4Dh			; right
	jz	_mem_right
	cmp	al,49h			; page-up
	jz	_mem_pageup
	cmp	al,51h			; page-down
	jz	_mem_pagedn
	cmp	al,4Eh			; '+'
	jz	_mem_incsel
	cmp	al,4Ah			; '-'
	jz	_mem_decsel
	cmp	al,1Ch			; ENTER
	jz	_mem_address
	cmp	al,47h			; home
	jz	_mem_home

	cmp	al,02h			; 1 - 16bit/32bit text
	jz	_cpu_showassm
	cmp	al,03h			; 2 - show hex data
	jz	_cpu_showcode
	cmp	al,04h			; 3 - FPU Registers
	jz	_mem_fpuregs
	cmp	al,33h			; ',' - FPU Registers
	jz	_mem_fpuregs
	cmp	al,05h			; 4 - unrelocated EIP
	jz	_cpu_unreloc
	cmp	al,34h			; '.'- unrelocated EIP
	jz	_cpu_unreloc
	ret






;-----------------------------------------------------------------------------
_mem_cursor:
	mov	esi,_mem_xpos
	mov	edi,_mem_ypos
	mov	ecx,2
	mov	ax,2F00h
	jmp	_cursor
_mem_up:call	show_cursor
	dec	_mem_ypos
	cmp	_mem_ypos,38
	ja	show_cursor
	inc	_mem_ypos
	sub	_data_addr,8
	call	show_data
	jmp	show_cursor
_mem_down:
	call	show_cursor
	inc	_mem_ypos
	cmp	_mem_ypos,49
	jb	show_cursor
	dec	_mem_ypos
	add	_data_addr,8
	call	show_data
	jmp	show_cursor
_mem_pageup:
	sub	_data_addr,80
	call	show_data
	jmp	show_cursor
_mem_pagedn:
	add	_data_addr,80
	call	show_data
	jmp	show_cursor
_mem_left:
	call	show_cursor
	sub	_mem_xpos,3
	cmp	_mem_xpos,12
	ja	show_cursor
	add	_mem_xpos,3*8
	call	show_cursor
	jmp	_mem_up
_mem_right:
	call	show_cursor
	add	_mem_xpos,3
	cmp	_mem_xpos,36
	jb	show_cursor
	sub	_mem_xpos,3*8
	call	show_cursor
	jmp	_mem_down
_mem_decsel:
	dec	_data_override
	jns	@@l1
	mov	_data_override,5
@@l1:	call	show_data
	jmp	show_cursor
_mem_incsel:
	inc	_data_override
	cmp	_data_override,6
	jb	@@l1
	mov	_data_override,0
@@l1:	call	show_data
	jmp	show_cursor
_mem_fpuregs:
	call	_cpu_fpuregs
	jmp	show_cursor
_mem_address:
	call	clear_cmdline
	mov	xpos,1
	mov	edx,offs msg57
	call	prints
	call	_input
	cmp	al,1
	jz	@@end

	clr	ebx
	mov	cl,8
	mov	edx,offs _fname
	cmp	bptr [edx],'.'
	setz	ch
	jnz	@@1
	inc	edx
@@1:	cmp	dptr [edx],00584145h		;EAX
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00584245h		;EBX
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00584345h		;ECX
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00584445h		;EDX
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00495345h		;ESI
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00494445h		;EDI
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00504245h		;EBP
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00505345h		;ESP
	jz	@@reg
	clr	ebx
	cmp	dptr [edx],00504945h		;EIP
	jnz	@@loop
	mov	ebx,_eip
	mov	_data_override,1
	jmp	@@doit
@@reg:	mov	eax,_reg_rtab[ebx*4]
	mov	ebx,[eax]
	jmp	@@doit

@@loop:	mov	al,[edx]
	inc	edx
	test	al,al
	jz	@@done
	sub	al,30h
	jc	@@err
	cmp	al,9
	jbe	@@m0
	sub	al,7
	cmp	al,0Fh
	ja	@@err
@@m0:	rol	ebx,4
	or	bl,al
	dec	cl
	jnz	@@loop
@@done:	cmp	cl,8
	jz	@@end
@@doit:	mov	eax,ebx
	test	ch,ch
	jz	@@unrl
	mov	bl,_data_override
	test	bl,bl			; 0 = es
	mov	dx,wptr _es
	jz	@@keen
	dec	bl
	mov	dx,wptr _cs
	jz	@@keen
	dec	bl
	mov	dx,wptr _ss
	jz	@@keen
	dec	bl
	mov	dx,wptr _ds
	jz	@@keen
	dec	bl
	mov	dx,wptr _fs
	jz	@@keen
	dec	bl
	mov	dx,wptr _gs
@@keen:	call	make_unreloc
@@unrl:	mov	_data_addr,eax
	call	show_cursor
	mov	_mem_ypos,39
	mov	_mem_xpos,14
	call	show_data
	call	show_cursor
@@end:	jmp	print_cmdline
@@err:	call	clear_cmdline
	mov	edx,offs msg58
	mov	xpos,1
	mov	ypos,49
	call	prints
	call	beep
	call	beep
	call	get_key
	jmp	print_cmdline

_mem_home:
	mov	_data_addr,0
	call	show_cursor
	mov	_mem_ypos,39
	mov	_mem_xpos,14
	call	show_data
	jmp	show_cursor






;-----------------------------------------------------------------------------
;	STACK - Window
;=============================================================================
_stk_window:
	push	offs _stk_window
	call	_show_addr_stk

	call	get_key
	cmp	wptr _keychar,0278h	; alt-x
	jz	_exit
	cmp	al,1			; ESC
	jz	switch_to_cpu
	cmp	al,15			; TAB
	jz	window_switch
	cmp	al,37h			; Print-Screen
	jz	dump_screen

	cmp	al,48h			; up
	jz	_stk_up
	cmp	al,50h			; down
	jz	_stk_down
	cmp	al,47h			; home
	jz	_stk_home

	cmp	al,02h			; 1 - 16bit/32bit text
	jz	_stk_showassm
	cmp	al,03h			; 2 - show hex data
	jz	_stk_showcode
	cmp	al,04h			; 3 - FPU Registers
	jz	_cpu_fpuregs
	cmp	al,033h			; ',' - FPU Registers
	jz	_cpu_fpuregs
	cmp	al,05h			; 4 - unrelocated EIP
	jz	_stk_unreloc
	cmp	al,034h			; '.' - unrelocated EIP
	jz	_stk_unreloc
	ret






;-----------------------------------------------------------------------------
_stk_cursor:
	mov	esi,_stk_xpos
	mov	edi,_stk_ypos
	mov	ecx,17
	mov	ax,2F00h
	jmp	_cursor
_stk_up:add	_stack_addr,4
	jmp	_stk_common
_stk_down:
	sub	_stack_addr,4
	jmp	_stk_common
_stk_home:
	mov	_stack_addr,0
_stk_common:
	mov	ebx,30
	mov	ecx,17
	mov	esi,62
	mov	edi,1
	call	clearwindow
	call	show_stack
	jmp	show_cursor
_stk_showassm:
	xor	_showmode_flag,1
	xor	_commmode_flag,1
	call	repaint_text
	jmp	show_cursor
_stk_showcode:
	xor	_assmmode_flag,1
	call	repaint_text
	jmp	show_cursor
_stk_unreloc:
	xor	_show_unreloc,1
	call	repaint_text
	jmp	show_cursor








;-----------------------------------------------------------------------------
;	REGISTERS - Window
;=============================================================================
_reg_window:
	push	offs _reg_window
	call	get_key
	cmp	wptr _keychar,0278h	; alt-x
	jz	_exit
	cmp	al,1			; ESC
	jz	switch_to_cpu
	cmp	al,15			; TAB
	jz	window_switch
;	cmp	al,37h			; Print-Screen
;	jz	dump_screen

	cmp	al,48h			; up
	jz	_reg_up
	cmp	al,50h			; down
	jz	_reg_down
	cmp	al,4Bh			; left
	jz	_reg_left
	cmp	al,4Dh			; right
	jz	_reg_right
	cmp	al,47h			; home
	jz	_reg_home
	cmp	al,4Fh			; end
	jz	_reg_end
	cmp	al,4Eh			; '+'
	jz	_reg_incr
	cmp	al,4Ah			; '-'
	jz	_reg_decr
	cmp	al,37h			; '*'
	jz	_reg_not
	cmp	al,35h			; '/'
	jz	_reg_res
	cmp	al,1Ch			; ENTER
	jz	_reg_input

	cmp	al,02h			; 1 - 16bit/32bit text
	jz	_cpu_showassm
	cmp	al,03h			; 2 - show hex data
	jz	_cpu_showcode
	cmp	al,04h			; 3 - FPU Registers
	jz	_cpu_fpuregs
	cmp	al,33h			; ',' - FPU Registers
	jz	_cpu_fpuregs
	cmp	al,05h			; 4 - unrelocated EIP
	jz	_cpu_unreloc
	cmp	al,34h			; '.' - unrelocated EIP
	jz	_cpu_unreloc
	ret






;-----------------------------------------------------------------------------
_reg_cursor:
	mov	esi,_reg_xpos
	mov	edi,_reg_ypos
	mov	ecx,_reg_ldat
	jmp	_cursorx
_reg_up:
	call	show_cursor
	mov	ebp,_reg_xptr
	dec	ebp
	jns	_reg_common
	jmp	show_cursor
_reg_down:
	call	show_cursor
	mov	ebp,_reg_xptr
	inc	ebp
	cmp	ebp,17
	jb	_reg_common
	jmp	show_cursor
_reg_left:
	call	show_cursor
	mov	ebp,_reg_xptr
	sub	ebp,4
	jns	_reg_common
	clr	ebp
	jmp	_reg_common
_reg_right:
	call	show_cursor
	mov	ebp,_reg_xptr
	add	ebp,4
	cmp	ebp,17
	jb	_reg_common
	jmp	show_cursor
_reg_home:
	call	show_cursor
	clr	ebp
	jmp	_reg_common
_reg_end:
	call	show_cursor
	mov	ebp,16
	jmp	_reg_common

_reg_incr:
	mov	ebp,_reg_xptr
	cmp	ebp,9
	jb	@@l1
	mov	eax,_reg_rtab[ebp*4]
	xor	_efl,eax
	call	show_cursor
	call	show_regs
	call	show_text_clr
	jmp	show_cursor
@@l1:	mov	eax,_reg_rtab[ebp*4]
	inc	dptr [eax]
	call	show_cursor
	call	show_regs
	call	show_text_clr
	call	show_data
	jmp	show_cursor
_reg_decr:
	mov	ebp,_reg_xptr
	cmp	ebp,9
	jb	@@l1
	mov	eax,_reg_rtab[ebp*4]
	xor	_efl,eax
	call	show_cursor
	call	show_regs
	call	show_text_clr
	jmp	show_cursor
@@l1:	mov	eax,_reg_rtab[ebp*4]
	dec	dptr [eax]
	call	show_cursor
	call	show_regs
	call	show_text_clr
	call	show_data
	jmp	show_cursor
_reg_res:
	mov	ebp,_reg_xptr
	cmp	ebp,9
	jb	@@l1
	mov	eax,_reg_rtab[ebp*4]
	not	eax
	and	_efl,eax
	call	show_cursor
	call	show_regs
	call	show_text_clr
	call	show_data
	jmp	show_cursor
@@l1:	mov	eax,_reg_rtab[ebp*4]
	mov	dptr [eax],0
	call	show_cursor
	call	show_regs
	call	show_text_clr
	call	show_data
	jmp	show_cursor
_reg_not:
	mov	ebp,_reg_xptr
	cmp	ebp,9
	jb	@@l1
	mov	eax,_reg_rtab[ebp*4]
	xor	_efl,eax
	call	show_cursor
	call	show_text_clr
	call	show_regs
	jmp	show_cursor
@@l1:	mov	eax,_reg_rtab[ebp*4]
	not	dptr [eax]
	call	show_cursor
	call	show_regs
	call	show_text_clr
	call	show_data
	jmp	show_cursor
_reg_common:
	mov	_reg_xptr,ebp
	movzx	eax,_reg_xtab[ebp]
	mov	_reg_xpos,eax
	movzx	eax,_reg_ytab[ebp]
	mov	_reg_ypos,eax
	movzx	eax,_reg_ltab[ebp]
	mov	_reg_ldat,eax
	jmp	show_cursor

_reg_input:
	mov	ebp,_reg_xptr
	cmp	ebp,9
	jb	@@strt
	ret
@@strt:	call	clear_cmdline
	mov	xpos,1
	mov	edx,offs msg55
	call	prints
	call	_input
	cmp	al,1
	jz	@@end
	clr	ebx
	mov	cl,8
	mov	edx,offs _fname

	cmp	dptr [edx],00584145h		;EAX
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00584245h		;EBX
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00584345h		;ECX
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00584445h		;EDX
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00495345h		;ESI
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00494445h		;EDI
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00504245h		;EBP
	jz	@@reg
	inc	ebx
	cmp	dptr [edx],00505345h		;ESP
	jz	@@reg
	clr	ebx
	cmp	dptr [edx],00504945h		;EIP
	jnz	@@loop
	mov	ebx,_eip
	jmp	@@doit
@@reg:	mov	eax,_reg_rtab[ebx*4]
	mov	ebx,[eax]
	jmp	@@doit

@@loop:	mov	al,[edx]
	inc	edx
	test	al,al
	jz	@@done
	sub	al,30h
	jc	@@err
	cmp	al,9
	jbe	@@m0
	sub	al,7
	cmp	al,0Fh
	ja	@@err
@@m0:	rol	ebx,4
	or	bl,al
	dec	cl
	jnz	@@loop
@@done:	cmp	cl,8
	jz	@@end
@@doit:	mov	eax,_reg_rtab[ebp*4]
	mov	dptr [eax],ebx
	push	ebp
	call	show_cursor
	pop	ebp
	cmp	ebp,8
	jnz	@@lll
	mov	eax,_eip
	mov	_code_addr,eax
	mov	_cpu_ypos,1
	call	show_text_clr
@@lll:	call	show_regs
	call	show_data
	call	show_cursor
@@end:	jmp	print_cmdline
@@err:	call	clear_cmdline
	mov	edx,offs msg56
	mov	xpos,1
	mov	ypos,49
	call	prints
	call	beep
	call	beep
	call	get_key
	jmp	print_cmdline



;-----------------------------------------------------------------------------
repaint_text:
	call	show_text_clr
	mov	ebx,30
	mov	ecx,17
	mov	esi,62
	mov	edi,1
	call	clearwindow
	call	show_stack
	call	show_mode
	cmp	_current_window,0
	jz	_cpu_cursor
	ret
window_switch:
	add	esp,4
	call	show_cursor
	test	_keyshift,01h
	jz	@@1
	dec	_current_window
	jmp	@@2
@@1:	inc	_current_window
@@2:	and	_current_window,3
_win_switch:
	call	show_cursor
	mov	ebp,offs _windowtab
	jmp	goto_window
switch_to_cpu:
	add	esp,4
	call	show_cursor
	mov	_current_window,0
	jmp	_win_switch
show_cursor:
	mov	ebp,offs _cursortab
goto_window:
	mov	ebx,_current_window
	and	ebx,3
	jmp	[ebp+ebx*4]




;=============================================================================
	Align 4
_int00:	cli				; Divide by 0
	push	es ss ds fs gs
	pushad
	call	save_state
	xor	al,al
	jmp	_exception
	Align 4
_int01:	cli				; hardware breakpoint, stepping
	push	es ss ds fs gs
	pushad
	call	save_state
	mov	al,1
	jmp	_exception
	Align 4
_int03:	cli				; software breakpnt int 3
	push	es ss ds fs gs
	pushad
	call	save_state
	mov	al,3
	jmp	_exception
	Align 4
_int04:	cli				; Into
	push	es ss ds fs gs
	pushad
	call	save_state
	mov	al,4
	jmp	_exception
	Align 4
_int05:	cli				; Bound
	push	es ss ds fs gs
	pushad
	call	save_state
	mov	al,5
	jmp	_exception
	Align 4
_int06:	cli				; Invalid OP
	push	es ss ds fs gs
	pushad
	call	save_state
	mov	al,6
	jmp	_exception

	Align 4
_int09:	push	eax
	in	al,60h
	cmp	al,45h
	jnz	@@0
	mov	ax,cs
	cmp	ax,[esp+12]
	jnz	_interrupted
@@0:	pop	eax
	push	_old_int9sel
	push	_old_int9off
	retf
_interrupted:
	pop	eax
	cli				; Keyboard trap
	push	es ss ds fs gs
	pushad
	call	save_state
	mov	al,0FFh
	jmp	_exception

	Align 4
_int0A:	cli				; Invalid TSS
	push	eax
	mov	al,0Bh
	out	20h,al
	in	al,20h
	test	al,al
	pop	eax
	jz	@@int
	push	_old_intAsel
	push	_old_intAoff
	retf
@@int:	push	es ss ds fs gs
	pushad
	call	save_state_err
	mov	al,10
	jmp	_exception
	Align 4
_int0B:	cli				; Segment not present
	push	eax
	mov	al,0Bh
	out	20h,al
	in	al,20h
	test	al,al
	pop	eax
	jz	@@int
	push	_old_intBsel
	push	_old_intBoff
	retf
@@int:	push	es ss ds fs gs
	pushad
	call	save_state
	mov	al,11
	jmp	_exception
	Align 4
_int0C:	cli				; Stack exception
	push	eax
	mov	al,0Bh
	out	20h,al
	in	al,20h
	test	al,al
	pop	eax
	jz	@@int
	push	_old_intCsel
	push	_old_intCoff
	retf
@@int:	push	es ss ds fs gs
	pushad
	call	save_state_err
	mov	al,12
	jmp	_exception
	Align 4
_int0D:	cli				; General Protection
	push	eax
	mov	al,0Bh
	out	20h,al
	in	al,20h
	test	al,al
	pop	eax
	jz	@@int
	push	_old_intDsel
	push	_old_intDoff
	retf
@@int:	push	es ss ds fs gs
	pushad
	call	save_state_err
	mov	al,13
	jmp	_exception
	Align 4
_int0E:	cli				; Page fault
	push	eax
	mov	al,0Bh
	out	20h,al
	in	al,20h
	test	al,al
	pop	eax
	jz	@@int
	push	_old_intEsel
	push	_old_intEoff
	retf
@@int:	push	eax
	mov	eax,[esp+8]
	cmp	eax,offs __safe_load
	jnz	@@exc
	mov	ax,cs
	cmp	ax,[esp+12]
	jnz	@@exc
	pop	eax
	push	ds
	mov	ds,cs:__datasel
	inc	__page_faults
	pop	ds
	add	esp,4		; remove ercode
	add	dptr [esp],4	; update EIP
	or	bptr [esp+8],1	; set carry flag
	iretd
@@exc:	pop	eax
	push	es ss ds fs gs
	pushad
	call	save_state_err
	mov	al,14
	jmp	_exception

;-----------------------------------------------------------------------------
	Align 4
_keyboard:				; IRQ 1
	push	eax ebx ds
	mov	ds,cs:__datasel
	clr	ebx
	in	al,60h
	mov	bl,al
	in	al,61h		; acknowledge receiving keycode
	mov	ah,al
	or	al,80h
	out	61h,al
	mov	al,ah
	out	61h,al
	mov	al,20h
	out	20h,al
	mov	al,bl
	cmp	al,0E0h
	jz	@@exit
	mov	_keycode,al
	btr	ebx,7
	setnc	al
	mov	_keytab[ebx],al
	test	al,al
	jz	@@exit
	mov	ah,_keytab[2Ah]		; SHIFT
	or	ah,_keytab[36h]		; SHIFT
	mov	bh,ah
	shl	bh,7
	add	bl,bh
	and	ebx,0FFh
	mov	al,kbxtbl[ebx]
	test	al,al
	jz	@@exit
	mov	_keychar,al
	mov	_keyhit,1
	mov	al,_keytab[1Dh]		; CONTROL
	shl	al,1
	or	al,_keytab[38h]		; ALT
	shl	al,1
	or	al,ah
	mov	_keyshift,al
	jmp	@@done
@@exit:	xor	al,al
	mov	_keychar,al
	mov	_keyhit,al
@@done:	pop	ds ebx eax
	iretd

;-----------------------------------------------------------------------------
	Align 4
_exception:			; EXCEPTION entry: activated by INTs 0..14
	push	eax			; EAX = interrupt number
	call	selector_init		; get selector info
	call	video_init		; initialize video (if necessary)
	call	show_text_clr
	cmp	_cpu_xptr,0
	jnz	@@l1
	mov	eax,_eip
	mov	_code_addr,eax
	mov	_cpu_ypos,1
	mov	ebx,30
	mov	ecx,60
	mov	esi,1
	mov	edi,1
	call	clearwindow
	call	show_text
	jmp	@@l2
@@l1:	mov	eax,_cpu_xptr
	mov	_cpu_ypos,eax
	dec	eax
	mov	esi,_addrbuffer[eax*4]
	call	decode
@@l2:	mov	ebx,30
	mov	ecx,17
	mov	esi,62
	mov	edi,1
	call	clearwindow
	call	show_stack
	call	show_cursor
	call	clearkeytab
	call	syncdisplay
	call	screen_on
	pop	eax
	cmp	al,1
	jz	@@l3
	call	print_errline
	call	beep
	call	get_key
@@l3:	call	print_cmdline
	jmp	_cpu_window

;-----------------------------------------------------------------------------
	Align 4
save_state_err:
	pop	ebp
	mov	ds,cs:__datasel		; load data selectors
	pop	_edi _esi _ebp _esp _ebx _edx _ecx _eax
	pop	_gs _fs _ds _ss _es	; copy registers into structure
	pop	_errorcode		; remove errorcode from stack
	mov	_old_stack,esp		; save program's stack pointer
	jmp	_save_common
	Align 4
save_state:
	pop	ebp			; pop caller
	mov	ds,cs:__datasel		; load data selectors
	pop	_edi _esi _ebp _esp _ebx _edx _ecx _eax
	pop	_gs _fs _ds _ss _es	; copy registers into structure
	mov	_old_stack,esp		; save program's stack pointer
_save_common:
	cld
	clr	eax
	mov	dr7,eax			; reset breakpoint events
	and	bptr _dr7[0],0FCh
	mov	es,cs:__datasel
	mov	fs,cs:__zerosel
	lea	eax,[esp+12]
	mov	_esp,eax
	mov	eax,[esp+0]		; get EIP
	mov	_eip,eax
	mov	eax,[esp+4]
	mov	_cs,eax			; get caller's CS
	mov	eax,[esp+8]
	mov	_efl,eax		; get flags
	mov	eax,cr0
	mov	_cr0,eax		; get control regs
	mov	eax,cr2
	mov	_cr2,eax
	mov	eax,cr3
	mov	_cr3,eax
	mov	ss,cs:__datasel		; load system's stack selector
	mov	esp,offs _system_stack	; load new stack pointer
	mov	al,02h
	call	fword ptr __api_off
	push	ds
	mov	ds,bx
	mov	ax,ds:[esi+2]
	shl	eax,16
	mov	ax,ds:[edi+40h+8+4]
	mov	edx,ds:[edi+40h+8+0]
	pop	ds
	mov	__irq1_buffer_sel,eax
	mov	__irq1_buffer_off,edx
	mov	ax,0204h		; AX = DPMI get prot. mode int vector
	mov	bl,09h
	xor	ecx,ecx
	int	31h			; get int 9 vector (keyboard handler)
	mov	_old_int9sel,ecx
	mov	_old_int9off,edx
	mov	ax,0205h		; AX = DPMI set prot. mode int vector
	mov	cx,cs
	mov	bl,09h
	mov	edx,offs cs:_keyboard
	int	31h			; install custom int 9
	jc	_abort
	cli
	in	al,21h
	mov	ah,al
	in	al,0A1h
	mov	_old_picmask,ax		; get and save pic mask
	mov	al,0FDh
	out	21h,al			; disable all IRQs except IRQ1 (kbrd)
	mov	al,0FFh
	out	0A1h,al
	mov	al,20h			; acknowledge interrupt rec.
	out	20h,al			; (in case hardware intr. call)
	out	0A0h,al			; write to both interrupt controllers
	jmp	ebp

	Align 4
restore_state:
	pop	ebp
	mov	ax,0205h
	mov	ecx,_old_int9sel
	mov	edx,_old_int9off
	mov	bl,09h
	int	31h			; restore orignal int 9 (keyboard)
	jc	_abort
	mov	esi,offs _registers
	mov	edi,offs _old_registers
	mov	ecx,19
	rep	movsd				; copy registers to _old_regs
	cli
	mov	ax,_old_picmask
	out	0A1h,al
	mov	al,ah
	out	21h,al
	mov	al,20h			; acknowledge interrupt rec.
	out	20h,al			; (in case hardware intr. call)
	out	0A0h,al			; write to both interrupt controllers
	mov	edx,_cs_base
	mov	eax,_dr0		; set breakpoints
	add	eax,edx
	mov	dr0,eax
	mov	eax,_dr1
	add	eax,edx
	mov	dr1,eax
	mov	eax,_dr2
	add	eax,edx
	mov	dr2,eax
	mov	eax,_dr3
	add	eax,edx
	mov	dr3,eax
	mov	eax,_dr7
	mov	dr7,eax
	mov	esp,_old_stack
	mov	ss,_ss
	mov	eax,_efl
	mov	[esp+8],eax
	mov	eax,_cs
	mov	[esp+4],eax
	mov	eax,_eip
	mov	[esp+0],eax
	mov	gs,_gs
	mov	fs,_fs
	mov	es,_es
	mov	edi,_edi
	mov	esi,_esi
	mov	ebx,_ebx
	mov	edx,_edx
	mov	ecx,_ecx
	mov	eax,_eax
	jmp	ebp

;-----------------------------------------------------------------------------
video_init:
	cmp	old_video_mode,0
	jz	@@l0
	call	show_regs
	call	show_data
	jmp	show_mode
@@l0:	mov	eax,0BE000h
	mov	screen_base,eax

	cmp	_vgastate_set,0
	jz	@@novg
	push	es			; save VGA state
	mov	ax,1C01h
	mov	es,__datasel
	mov	ebx,_vgastate_buf
	mov	ecx,0007h
	int	10h
	cli
	mov	ax,1C02h
	mov	ecx,0007h
	int	10h
	cli
	pop	es

@@novg:	cmp	_svgastate_set,0
	jz	@@nosv
	push	es			; save VESA state
	mov	ax,4F04h
	mov	es,__datasel
	mov	ebx,_svgastate_buf
	mov	ecx,000Fh
	mov	edx,0001h
	int	10h
	cli
	mov	ax,4F04h
	mov	ecx,000Fh
	mov	edx,0002h
	int	10h
	cli
	pop	es

@@nosv:	call	screen_off
	mov	ax,4F03h		; get VGA/VESA mode
	int	10h
	cli
	mov	old_video_mode,bx
	and	bh,7Fh			; clear high bit of BX
	jz	@@l1			; if only VGA, no need in bank
	mov	ax,4F05h
	mov	bx,0100h
	int	10h			; get VESA bank
	cli
	mov	old_video_bank,dl
	mov	ax,4F05h		; set proper vesa bank 0
	clr	ebx
	clr	edx
	int	10h
	cli

@@l1:	mov	ax,old_video_mode
	mov	bx,8000h
	and	ah,7Fh			; clear high bit of AX
	jnz	@@l2			; if VESA, certainly graphics mode
	mov	bx,0080h

	and	al,7Fh
	cmp	al,08h
	mov	esi,0B8000h		; Text modes seg
	mov	ecx,800h
	jbe	@@m1
@@l2:	mov	esi,0A0000h		; Graphics modes seg
	mov	ecx,4000h
@@m1:	or	old_video_mode,bx
	mov	edi,_system_block
	cld
	;shl	ecx,2
	rep	movsd
	clr	eax
	mov	ecx,300h
	mov	dx,03C7h	; read block of pals
	out	dx,al
	mov	dx,03C9h
	rep	insb		; block read dac registers

@@__1:	clr	ebx
	mov	ax,0300h
	int	10h			; get DOS cursor position
	cli
	mov	old_video_pos,dx
	mov	ax,0083h
	int	10h			; set DOS mode03 (don't clearscreen)
	cli
	clr	ebx
	mov	ax,1112h
	int	10h			; set DOS screen lines to 50
	cli
	clr	ebx
	mov	cx,2000h
	mov	ax,0100h
	int	10h			; turn off DOS cursor
	cli
	mov	al,0FDh
	out	21h,al			; disable all IRQs except IRQ1 (kbrd)
	mov	al,0FFh
	out	0A1h,al
	mov	dx,03D4h
	mov	ax,300Ch
	out	dx,ax
	mov	ax,000Dh
	out	dx,ax
	mov	eax,3F003F00h
	mov	ecx,000007D0h		; 80*50/4
	mov	edi,screen_base
	cld
	rep	stosd
	mov	_fpuregs_set,0
	call	drawframe
	call	show_regs
	call	show_data
	jmp	show_mode

restore_video:
	clr	eax
	xchg	ax,old_video_mode	; get and reset video mode
	push	eax
	mov	ebx,eax
	and	ah,7Fh
	jnz	@@l1			; it is VESA mode
	and	bl,7Fh			; if mode > 03h, don't clear screen
	cmp	bl,03h
	ja	@@0
	and	al,7Fh
@@0:	xor	ah,ah
	int	10h
	cli
	jmp	@@l2

@@l1:	mov	ax,4F02h
	int	10h
	cli
	mov	ax,4F05h		; set proper vesa bank
	clr	ebx
	clr	edx
	mov	dl,old_video_bank
	int	10h
	cli
	cmp	_svgastate_set,0
	jz	@@nosv
	push	es			; restore VESA state
	mov	ax,4F04h
	mov	es,__datasel
	mov	ebx,_svgastate_buf
	mov	ecx,000Fh
	mov	edx,0002h
	int	10h
	cli
	pop	es
	jmp	@@novg

@@nosv:
@@l2:	cmp	_vgastate_set,0
	jz	@@novg
	push	es			; restore VGA state
	mov	ax,1C02h
	mov	es,__datasel
	mov	ebx,_vgastate_buf
	mov	ecx,0007h
	int	10h
	cli
	pop	es

@@novg:	clr	ebx
	mov	ax,0200h
	mov	dx,old_video_pos
	int	10h
	cli
	pop	eax
	and	ah,7Fh
	jnz	@@l3
	and	al,7Fh			; mask bit 7 in al
	cmp	al,08h
	mov	edi,0B8000h
	mov	ecx,800h
	jb	@@m1
@@l3:	mov	edi,0A0000h
	mov	ecx,4000h
@@m1:	mov	esi,_system_block
	cld
	;shl	ecx,2
	rep	movsd
	clr	eax
	mov	ecx,300h
	mov	dx,03C8h
	out	dx,al
	inc	edx
	rep	outsb
	ret




.DATA
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
library_string label byte
db 10,'SD/32A -- Protected Mode Debugger Library'
db 10,'Copyright (C) Supernar Systems, Ltd. 1996-2002'
db 10,'All Rights Reserved.'
db 10,0


	Align	4
_registers	label	dword
_eax	label	dword
_ax	label	word
_al	db	0
_ah	db	0, 0,0
_ecx	label	dword
_cx	label	word
_cl	db	0
_ch	db	0, 0,0
_edx	label	dword
_dx	label	word
_dl	db	0
_dh	db	0, 0,0
_ebx	label	dword
_bx	label	word
_bl	db	0
_bh	db	0, 0,0
_esp	label	dword
_sp	dw	0, 0
_ebp	label	dword
_bp	dw	0, 0
_esi	label	dword
_si	dw	0, 0
_edi	label	dword
_di	dw	0, 0
_cs	dd	0
_ds	dd	0
_es	dd	0
_ss	dd	0
_fs	dd	0
_gs	dd	0
_eip	label	dword		; Non-standard reg. structure
_ip	dw	0, 0
_efl	label	dword
_fl	dw	0, 0
_cr0	dd	0
_cr2	dd	0
_cr3	dd	0

_old_registers	label	dword
_old_eax	dd -1
_old_ecx	dd -1
_old_edx	dd -1
_old_ebx	dd -1
_old_esp	dd -1
_old_ebp	dd -1
_old_esi	dd -1
_old_edi	dd -1
_old_cs		dd -1
_old_ds		dd -1
_old_es		dd -1
_old_ss		dd -1
_old_fs		dd -1
_old_gs		dd -1
_old_eip	dd -1		; Non-standard reg. structure
_old_efl	dd -1
_old_cr0	dd -1
_old_cr2	dd -1
_old_cr3	dd -1

_errorcode	dd 0		; error code pushed on stack on an exception
_es_acc		dw 0
_cs_acc		dw 0
_ss_acc		dw 0
_ds_acc		dw 0
_fs_acc		dw 0
_gs_acc		dw 0
_es_base	dd 0
_cs_base	dd 0
_ss_base	dd 0
_ds_base	dd 0
_fs_base	dd 0
_gs_base	dd 0
_es_limit	dd 0
_cs_limit	dd 0
_ss_limit	dd 0
_ds_limit	dd 0
_fs_limit	dd 0
_gs_limit	dd 0

_dr0		dd 0
_dr1		dd 0
_dr2		dd 0
_dr3		dd 0
_dr7		dd 0

_old_stack	dd 0
_old_int9sel	dd 0
_old_int9off	dd 0
_old_intAsel	dd 0
_old_intAoff	dd 0
_old_intBsel	dd 0
_old_intBoff	dd 0
_old_intCsel	dd 0
_old_intCoff	dd 0
_old_intDsel	dd 0
_old_intDoff	dd 0
_old_intEsel	dd 0
_old_intEoff	dd 0
_old_picmask	dw 0
old_video_mode	dw 0
old_video_bank	db 0
old_video_pos	dw 0
screen_base	dd 0
xpos		db 0
ypos		db 0
color		db 3Fh
_keycode	db 0		; scan code
_keychar	db 0		; last key pressed
_keyshift	db 0		; shift states for lask keypress
_keyhit		db 0		; 0=no key hit, 1=key hit available
_keytab		db 80h dup(0)	; non-sticky keyboard table
kbxtbl	db	0,27,'1234567890-=',16,15,'qwertyuiop[]',13,0	; 1-0q-p
	db	'asdfghjkl;:`',0,'\zxcvbnm,./',0,'*',0,32,0	; a-lz-m (+31)
	db	1,2,3,4,5,6,7,8,9,10,0,0,19,25,21,'-',23,'5'	; f1-f10 (+60)
	db	24,'+',20,26,22,17,18,0,0,0,11,12,27h dup(0)	; (+78)
	db	0,14,'!@#$%&/()=+~',16,15,'QWERTYUIOP{}',13,0	;
	db	'ASDFGHJKL:"~',0,'|ZXCVBNM<>?',0,'*',0,32,0	;
	db	1,2,3,4,5,6,7,8,9,10,0,0,19,25,21,'-',23,'5'	;
	db	24,'+',20,26,22,17,18,0,0,0,11,12,27h dup(0)	;

selbuf		db 8 dup(0)
__page_faults	dd 0
_temp		dd 0
_fname		db 64 dup(0)
_data_byte_tab	dd _get_data_byte_es, _get_data_byte_cs
		dd _get_data_byte_ss, _get_data_byte_ds
		dd _get_data_byte_fs, _get_data_byte_gs
		dd _get_byte_err, _get_byte_err

;-----------------------------------------------------------------------------
msg00	db 'CPU ',0
cpu_table label byte
	db '8086',0,0,0,0
	db '80186',0,0,0
	db '80286',0,0,0
	db '80386',0,0,0
	db '80486',0,0,0
	db '80586',0,0,0
	db '80686',0,0,0
fpu_table label byte
	db '/None',0,0,0
	db '/8087',0,0,0
	db '/287',0,0,0,0
	db '/387',0,0,0,0
	db '/487',0,0,0,0
	db '/587',0,0,0,0
	db '/687',0,0,0,0


msg10	db 'CPU Registers',0
msg11	db 'EAX 00000000   ESI 00000000   CS 0000 B=00000000   EIP 00000000   EFL 00000000',0
msg12	db 'EBX 00000000   EDI 00000000   DS 0000 B=00000000   CR0 00000000   ------------',0
msg13	db 'ECX 00000000   EBP 00000000   ES 0000 B=00000000   CR2 00000000               ',0
msg14	db 'EDX 00000000   ESP 00000000   SS 0000 B=00000000   CR3 00000000               ',0
msg15	db 'V R N IO O D I T S  Z A P C   FS 0000 B=00000000   PG AM WP  NE ET TS EM MP PE',0
msg16	db '0 0 0 00 0 0 0 0 0  0 0 0 0   GS 0000 B=00000000    0  0  0   0  0  0  0  0  0',0

msg10f	db 'FPU Registers',0
msg11f	db 'ST(0)',0
msg12f	db 'ST(1)',0
msg13f	db 'ST(2)',0
msg14f	db 'ST(3)',0
msg15f	db 'ST(4)',0
msg16f	db 'ST(5)',0
msg17f	db 'ST(6)',0
msg18f	db 'ST(7)',0
msg19f	db 'CW=0000  SW=0000  TAG=0000',0

msg1Ef1	db 'Unsupported             ',0
msg1Ef2	db 'NaN                     ',0
msg1Ef3	db '+Infinity               ',0
msg1Ef4	db '+0.0                    ',0
msg1Ef5	db 'Denormal                ',0
msg1Ef6	db 'Unused                  ',0
msg1Ef7	db 'Empty                   ',0

msg20	db 'Memory',0
msg21	db 'ds:[esi]',0
msg22	db 'es:[edi]',0
msg2es	db 'es:',0
msg2cs	db 'cs:',0
msg2ss	db 'ss:',0
msg2ds	db 'ds:',0
msg2fs	db 'fs:',0
msg2gs	db 'gs:',0
msg2er	db '--',0
msg2sip	db 'ds:[esi+',0
msg2sim	db 'ds:[esi-',0
msg2dip	db 'es:[edi+',0
msg2dim	db 'es:[edi-',0

msg30	db 'Stack',0
msg3spp	db '[esp+',0
msg3spm	db '[esp-',0
selerr1	db '--NULL--',0
selerr2	db 'INVALID!',0

msg40	db 'Address: ',8,3Fh,'00000000 ',8,3Ah,'=',8,3Fh,' 00h',0

msg086	db 8,31h,'SEGS: 16-bit',0
msg386	db 8,31h,'SEGS: 32-bit',0
msgrdy	db 8,7Fh,'READY',0
msgbusy	db 8,0FFh,'BUSY.',0

msg50	label byte
	db 8,74h,'F1',8,70h,'-Info '
	db 8,74h,'F2',8,70h,'-BreakPnt '
	db 8,74h,'F3',8,70h,'-Return '
	db 8,74h,'F4',8,70h,'-Here '
	db 8,74h,'F5',8,70h,'-Screen '
	db 8,74h,'F6',8,70h,'-New '
	db 8,74h,'F7',8,70h,'-Trace '
	db 8,74h,'F8',8,70h,'-Step '
	db 8,74h,'F9',8,70h,'-Run'
	db 0


msg55	db 8,74h,'Enter new value: ',8,70h,0
msg56	db 8,74h,'ERROR: Invalid value!',0
msg57	db 8,74h,'Enter new address: ',8,70h,0
msg58	db 8,74h,'ERROR: Invalid address!',0
msg60	db 8,74h,'Enter expression to search for: ',8,70h,0
msg61	db 8,74h,'Searching at address: 00000000 >',8,70h,,0
msg62	db 8,74h,'Expression not found!',0

errortab	dd err00, err01, err02, err03, err04, err05, err06, err07
		dd err08, err09, err0A, err0B, err0C, err0D, err0E, err0F

err0	db 8,4Eh,' * EXCEPTION:  ',0
err1	db 8,4Eh,' * ERROR:  ',0
errterm	db 8,4Eh,' * Program Terminated (INT 21h, AH=4Ch)',0
errkbrk	db 8,4Eh,' * Program Interrupted (Break-Key used)',0
errec	db 8,47h,'   ec=',0
err00	db 8,4Fh,'(INT 00h)  Integer division by zero fault',0
err01	db 8,4Fh,'(INT 01h)  Debugger trap',0
err02	db 8,4Fh,'(INT 02h)  NMI',0
err03	db 8,4Fh,'(INT 03h)  Software breakpoint',0
err04	db 8,4Fh,'(INT 04h)  Interrupt on overflow',0
err05	db 8,4Fh,'(INT 05h)  Array boundary violation fault',0
err06	db 8,4Fh,'(INT 06h)  Invalid opcode fault',0
err07	db 8,4Fh,'(INT 07h)  Coprocessor not available fault',0
err08	db 8,4Fh,'(INT 08h)  Double fault',0
err09	db 8,4Fh,'(INT 09h)  Coprocessor segment overrun fault',0
err0A	db 8,4Fh,'(INT 0Ah)  Invalid TSS fault',0
err0B	db 8,4Fh,'(INT 0Bh)  Segment not present fault',0
err0C	db 8,4Fh,'(INT 0Ch)  Stack exception fault',0
err0D	db 8,4Fh,'(INT 0Dh)  General protection fault',0
err0E	db 8,4Fh,'(INT 0Eh)  Page fault',0
err0F	db 8,4Fh,'(INT 0Fh)  Reserved',0

hlpid	db 'SUNSYS DOS/32A',0

hlp00	label byte
	db 8,74h,'ESC',8,70h,'-Exit    '
	db 8,74h,'F1',8,70h,'-DPMI '
	db 8,74h,'F2',8,70h,'-GDT '
	db 8,74h,'F3',8,70h,'-IDT '
	db 8,74h,'F4',8,70h,'-Blocks '
	db 8,74h,'F5',8,70h,'-History '
	db 8,74h,'F6',8,70h,'-Buffers '
	db 8,74h,'F10',8,70h,'-Extender'
	db 0
hlp0A	db 'Internal page faults=',0
hlp0B	db 'Internal stack pointer=',0
hlp0C	db 'PIT Counter 0=',0
hlp0D	db 'PIT frequency=',0
hlp01	db 'DPMI Reports:',0
hlp10	db 'Function AX=0400h',0
hlp11	db 'AX=',0
hlp12	db 'BX=',0
hlp13	db 'CX=',0
hlp14	db 'DX=',0

hlp20	db 'Function AX=0500h',0
hlp21	db 'Largest free block=',0
hlp22	db 'Total extended mem=',0
hlp25	db 'Function AX=0100h',0
hlp26	db 'Largest free DOS block=',0

hlp30	db 'Function AX=0A00h',0
hlp31	db 'AX=',0
hlp32	db 'BX=',0
hlp33	db 'CX=',0
hlp34	db 'DX=',0

hlp40	db 'Function AX=0E00h',0
hlp41	db 'AX=',0

hlp50	db 'CPU Selectors:',0
hlp51	db 'CS=0000 B=00000000 L=00000000 G=xxxx S=xxxxx T=0 Acc=0000',0
hlp52	db 'DS=0000 B=00000000 L=00000000 G=xxxx S=xxxxx T=0 Acc=0000',0
hlp53	db 'ES=0000 B=00000000 L=00000000 G=xxxx S=xxxxx T=0 Acc=0000',0
hlp54	db 'SS=0000 B=00000000 L=00000000 G=xxxx S=xxxxx T=0 Acc=0000',0
hlp55	db 'FS=0000 B=00000000 L=00000000 G=xxxx S=xxxxx T=0 Acc=0000',0
hlp56	db 'GS=0000 B=00000000 L=00000000 G=xxxx S=xxxxx T=0 Acc=0000',0

hlp60	db 'Global Descriptor Table (GDT)',0
hlp61	db 'GDT Base=00000000, GDT Limit=00000000',0
hlp62	db 'Use PAGE UP and PAGE DOWN keys to view the data in the GDT',0

hlp70	db 'Interrupt Descriptor Table (IDT)',0
hlp71	db 'IDT Base=00000000, IDT Limit=00000000',0
hlp72	db 'Use PAGE UP and PAGE DOWN keys to view the data in the IDT',0

hlp80	db 'Extended Memory Blocks (EMBs)',0
hlp81	db 'EMB Base=00000000, EMB Size=00000000, EMB Top=00000000',0
hlp82	db 'Use PAGE UP and PAGE DOWN keys to view the EMBs',0

hlp90	db 'System Interrupt History',0
hlp91	db 'Number of System Interrupts Issued=',0
hlp92	db 'Use PAGE UP and PAGE DOWN keys to view the History List',0

hlpA0	db 'DOS Extender and Debugger Information',0
hlpA1	db 'Running under DOS/32 Advanced DOS Extender',0
hlpA2	db 'Running under DOS/32 Advanced DOS Extender',0
hlpA3	db 'DOS/32 Advanced Version: ',0
hlpA4	db 'DOS/32 Advanced Version: ',0
hlpA5	db 'DOS/32A -- Protected Mode Run-Time',0
hlpA6	db 'DOS/32A -- Protected Mode Run-Time',0
hlpA7	db 'Copyright (C) Supernar Systems, Ltd. 1990-2002',0
hlpA8	db 'All Rights Reserved.',0

hlpB0	db 'Buffered Interrupt Tables',0
hlpB1	db 'EXC/IRQ  EXC Vector      IRQ Vector     EXC/IRQ',0
hlpB2	db 'Number    Sel:Offs        Sel:Offs      Status',0

_acc_typeg	db 'BYTE',0,0,0,0,'PAGE',0,0,0,0
_acc_typeb	db 'USE16',0,0,0,'USE32',0,0,0
_pagenum	db 0
_gdt_string	db 'Selector Limit  Base   Acc/Limit  Base ',0
_gdt_string2	db '         0..15  0..23   16..19   24..31',0
_idt_string	db 'Interrupt Offset  Selector  Type Offset',0
_idt_string2	db '          0..15                  16..31',0
_emb_string	db ' EMB      EMB       EMB       EMB   ',0
_emb_string2	db 'Number    Base      Size     Status ',0
_emb_str1	db 'Free',0
_emb_str2	db 'Used',0
_emb_str3	db 'Ok',0
_emb_str4	db 'Invalid',0
_hist_str01	db 'Num=',0
_hist_str02	db 'INT ',0
_hist_str03	db 'AX=',0
_hist_str04	db 'VideoBIOS API ',0
_hist_str05	db 'DOS API ',0
_hist_str06	db 'DPMI API ',0
_hist_str07	db 'Mouse API ',0
_hist_str10	db 'EFL=00000000  EIP=00000000',0
_hist_str11	db 'EAX=00000000  EBX=00000000  ECX=00000000  EDX=00000000',0
_hist_str12	db 'ESI=00000000  EDI=00000000  EBP=00000000  ESP=00000000',0
_hist_str20	db 'CS=0000  DS=0000  ES=0000  SS=0000  FS=0000  GS=0000',0
_hist_str30	db '(Show All)    ',0
_hist_str31	db '(Show INT 10h)',0
_hist_str32	db '(Show INT 21h)',0
_hist_str33	db '(Show INT 31h)',0
_hist_str34	db '(Show INT 33h)',0

		evendata
__api_off	dd 0
__api_sel	dw 0,0
_gdt_ptr	dd 0
_idt_ptr	dd 0
_emb_num	dd 0
_emb_mem_ptr	dd 0
_emb_mem_top	dd 0

__history_bufsize	dd 10000h	; history buffer size
__history_bufbase	dd 0		; history buffer base
__history_bufhand	dd 0		; history buffer memory handle
__history_bufptr	dd 0		; history buffer current pointer
__history_bufnum	dd 0		; history buffer current number
__history_current	dd 0
__history_bufon		db 0		; history buffer 0=off, 1=on
__history_intnum	db 0
__kernel_codesel	dw 0
__kernel_datasel	dw 0
__kernel_zerosel	dw 0
__kernel_codeseg	dw 0
__client_codesel	dw 0
__irq1_buffer_sel	dd 0
__irq1_buffer_off	dd 0

_dumpname	db 'dump0000.txt',0
;============================================================================
		evendata
_cpu_xpos	dd 0			; coordinates of cursor in windows
_cpu_ypos	dd 0
_cpu_xptr	dd 0
_mem_xpos	dd 0
_mem_ypos	dd 0
_stk_xpos	dd 0
_stk_ypos	dd 0
_reg_xpos	dd 0
_reg_ypos	dd 0
_reg_ldat	dd 0
_reg_xptr	dd 0
_reg_xtab	db 1, 1, 1, 1,  16,16,16,16, 52, 10,12,14,18, 21,23,25,27
_reg_ytab	db 32,33,34,35, 32,33,34,35, 32, 37,37,37,37, 37,37,37,37
_reg_ltab	db 12,12,12,12, 12,12,12,12, 12,  1, 1, 1, 1,  1, 1, 1, 1
_reg_rtab	dd _eax, _ebx, _ecx, _edx, _esi, _edi, _ebp, _esp, _eip
		dd 0800h, 0400h, 0200h, 0080h, 0040h, 0010h, 0004h, 0001h

_current_window	dd 0
_windowtab	dd _cpu_window, _reg_window, _mem_window, _stk_window
_cursortab	dd _cpu_cursor, _reg_cursor, _mem_cursor, _stk_cursor

_addrbuffer	dd 32 dup(0)
_fpuregs_on	db 0
_fpuregs_set	db 0
_assmmode_flag	db 0			; assembler mode: 8086/80386+
_showmode_flag	db 0			; show code equialents in CPU-window
_commmode_flag	db 0
_data_override	db 0			; override for Memory-window
_data_addr	dd 0			; address of memory in Memory-window
_code_addr	dd 0			; address for disassembler to start on
_stack_addr	dd 0
_system_block	dd 0
_vgastate_buf	dd 0
_svgastate_buf	dd 0
_unreloc_eip	dd 0
_show_unreloc	db 0
_vgastate_set	db 0
_svgastate_set	db 0

	Align	4
	dd 512 dup(-1)
_system_stack	label	dword


include	sdisassm.inc

