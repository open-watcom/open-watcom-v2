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

		.486p
		.MODEL flat
		LOCALS
		NOJUMPS


PUBLIC	VideoInit_, VideoReset_
PUBLIC	DrawBackground_
PUBLIC	GetKey_
PUBLIC	Printc_, Prints_, SetPos_, SetColor_, SetBackColor_, SetBlink_
PUBLIC	OpenWindow_, DrawWindow_, CloseWindow_, ShowCursor_, PrintHelp_
PUBLIC	get_himem_size_, get_lomem_size_, get_cpu_type_
PUBLIC	get_sys_type_, get_dosmem_used_, get_total_mem_, get_dpmi_bufsize_
PUBLIC	ReadHeader_, WriteHeader_, CloseFile_, GetFileType_, GetExtenderType_

PUBLIC	_keychar, _keycode

EXTRN	_mem_dos32a	: dword
EXTRN	_mem_kernel	: dword

include	stddef.inc

.CODE


GetFileType_:
	pushad
	mov	esi,offs @file_buffer
	mov	ax,4200h
	mov	bx,_file_handle
	xor	ecx,ecx
	movzx	edx,wptr [esi+3Ch]
	int	21h
	jc	@@t_no

	mov	ah,3Fh
	mov	bx,_file_handle
	mov	ecx,0004h
	mov	edx,offs @file_buffer+80h
	int	21h
	jc	@@t_no

	cmp	wptr [edx],'EL'
	jz	@@t_le
	cmp	wptr [edx],'XL'
	jz	@@t_lx
	cmp	wptr [edx],'CL'
	jz	@@t_lc
	cmp	wptr [edx],'EP'
	jz	@@t_pe

@@t_no:	popad				; not bound
	xor	eax,eax
	ret
@@t_le:	popad				; LE - style
	mov	eax,1
	ret
@@t_lx:	popad				; LX - style
	mov	eax,2
	ret
@@t_lc:	popad				; LC - style
	mov	eax,3
	ret
@@t_pe:	popad				; PE - style
	mov	eax,4
	ret


;=============================================================================
GetExtenderType_:
	pushad
	mov	esi,_buffer
	mov	ax,4200h
	mov	bx,_file_handle
	xor	ecx,ecx
	xor	edx,edx
	int	21h
	jc	@@0

	mov	ax,3F00h
	mov	bx,_file_handle
	mov	ecx,0100h
	mov	edx,_buffer
	int	21h
	jc	@@0

	mov	ebx,_buffer
	mov	esi,offs _str_dos32a
	lea	edi,[ebx+007Ch]
	call	check_string
	jnc	@@1
	mov	ebx,_buffer
	mov	esi,offs _str_dos32a
	lea	edi,[ebx+009Ch]
	call	check_string
	jnc	@@1

	mov	ebx,_buffer
	mov	esi,offs _str_stub32c
	lea	edi,[ebx+004Ch]
	call	check_string
	jnc	@@2
	mov	ebx,_buffer
	mov	esi,offs _str_stub32c
	lea	edi,[ebx+006Ch]
	call	check_string
	jnc	@@2


@@0:	popad
	xor	eax,eax			; N/A
	ret
@@1:	popad
	mov	eax,1			; DOS/32A
	ret
@@2:	popad
	mov	eax,2			; STUB/32C
	ret


check_string:
	pushad
@@0:	mov	al,[esi]
	test	al,al
	jz	@@ok
	cmp	al,[edi]
	jnz	@@err
	inc	esi
	inc	edi
	jmp	@@0
@@ok:	popad
	clc
	ret
@@err:	popad
	stc
	ret


;=============================================================================
ReadHeader_:
	pushad

	mov	ebp,edx
	mov	edx,eax
	mov	ax,3D02h
	int	21h
	jc	@@err1
	mov	_file_handle,ax

	mov	ah,3Fh
	mov	bx,_file_handle
	mov	ecx,0040h
	mov	edx,offs @file_buffer
	int	21h
	jc	@@err2

	mov	esi,offs @file_buffer
	cmp	wptr [esi+00h],'ZM'
	jnz	@@err3
	mov	ax,4200h
	mov	bx,_file_handle
	xor	ecx,ecx
	movzx	edx,wptr [esi+08h]
	shl	edx,4
	int	21h
	jc	@@err2

	mov	ah,3Fh
	mov	bx,_file_handle
	mov	ecx,0080h
	mov	edx,ebp
	int	21h
	jc	@@err2

	popad
	xor	eax,eax
	ret
@@err1:	popad
	mov	eax,1		; error opening
	ret
@@err2:	popad
	mov	eax,2		; error reading
	ret
@@err3:	popad
	mov	eax,3		; invalid header type (not MZ exec)
	ret



;=============================================================================
WriteHeader_:
	pushad
	push	eax
	mov	esi,offs @file_buffer
	mov	ax,4200h
	mov	bx,_file_handle
	xor	ecx,ecx
	movzx	edx,wptr [esi+08h]
	shl	edx,4
	int	21h
	pop	edx
	jc	@@err1
	mov	ah,40h
	mov	bx,_file_handle
	mov	ecx,0080h
	int	21h
	jc	@@err1
	popad
	xor	eax,eax
	ret
@@err1:	popad
	mov	eax,1		; error writing
	ret

CloseFile_:
	pushad
	mov	bx,_file_handle
	mov	ah,3Eh
	int	21h
	popad
	ret





;=============================================================================
	Align 4
get_dosmem_used_:
	pushad
	mov	ebp,eax			; preserve EAX
	mov	ax,0FF89h
	int	21h
	cmp	eax,'ID32'
	jz	@@l1
	popad
	xor	eax,eax
	ret

@@l1:	pushad
	mov	ax,0FF8Ah
	int	21h
	cmp	eax,'ID32'
	jz	@@l2
	popad
	popad
	xor	eax,eax
	ret

@@l2:	cmp	ch,03h			; if SYS = DPMI
	popad
	jnz	@@l3
	sub	ebp,_mem_kernel		; kernel has been removed

@@l3:	mov	eax,ebp			; DOS/32A exec size
	add	eax,0080h		; + DTA and Mouse_Buf
	add	eax,0400h		; + RM IDT
	add	eax,ecx			; + DOS Transfer Buffer

	mov	ebp,eax			; D32A API get ptr to client
	mov	ax,0FF8Eh
	int	21h
	movzx	eax,word ptr gs:[edi+08h] ; get size of DPMI required buffer
	shl	eax,4
	add	eax,ebp

	test	word ptr gs:[edi+06h],0001h ; check if STUB/32C is resident
	jz	@@done
	add	eax,2048		; count in size of STUB/32C

@@done:	mov	[esp+1Ch],eax
	popad
	ret



;=============================================================================
get_himem_size_:
	mov	ax,0FF90h
	int	21h
	ret


;=============================================================================
get_lomem_size_:
	mov	ax,0FF94h
	int	21h
	ret


;=============================================================================
get_total_mem_:
	pushad
	xor	eax,eax
	mov	al,30h
	out	70h,al
	in	al,71h
	mov	ah,al
	mov	al,31h
	out	70h,al
	in	al,71h
	xchg	al,ah
	mov	[esp+1Ch],eax
	popad
	ret


;=============================================================================
get_dpmi_bufsize_:
	pushad
	mov	ax,0FF8Eh
	int	21h
	movzx	eax,word ptr gs:[edi+08h] ; get size of DPMI required buffer
	shl	eax,4
	mov	[esp+1Ch],eax
	popad
	ret



;=============================================================================
; Get CPU type:	80386, 80486, PENTIUM, ...
;
	Align 4
get_cpu_type_:
	pushad
	mov	ax,0400h		; DPMI get CPU
	int	31h
	movzx	eax,cl
	mov	[esp+1Ch],eax
	cmp	cl,3
	ja	@@1
	popad
	ret

@@1:	pushfd
	cli
	pushfd
	pop     eax
	mov     edx,eax
	xor	eax,00200000h
	push    eax
	popfd
	pushfd
	pop     eax
	xor     eax,edx
	jne	@@l4
	jmp	@@x
@@l4:	xor	eax,eax
	db	0Fh, 0A2h
	mov	dword ptr [esp+1Ch+4],5	; CPU is 586
	cmp	eax,1			; check CPUID level
	jb	@@x			; if level 1 not supported, exit
	mov	eax,1
	db	0Fh, 0A2h
	mov	al,ah
	and     eax,0Fh
	mov	[esp+1Ch+4],eax
@@x:	sti
	popfd
	popad
	ret


;=============================================================================
; Get SYSTEM type:	raw, XMS, VCPI, DPMI
;
	Align 4
get_sys_type_:
	pushad
	mov	ax,1600h
	int	2Fh
	test	al,al
	jz	@@0
	cmp	al,80h
	jz	@@0
	mov	ch,4
	jmp	@@1

@@0:	mov	ax,0FF8Ah
	push	fs
	int	21h
	pop	fs
	cmp	eax,'ID32'
	jz	@@1
	mov	ch,5

@@1:	movzx	eax,ch
	mov	[esp+1Ch],eax
	popad
	ret




;=============================================================================
VideoInit_:
	pushad
	mov	esi,_screen_addr
	mov	edi,_buffer_addr
	mov	ecx,0800h
	rep	movsd
	call	sync_display
	mov	ax,0300h
	xor	ebx,ebx
	int	10h			; get DOS cursor position
	mov	_old_video_pos,dx
	mov	ax,0200h
	xor	edx,edx
	int	10h			; set cursor to 0,0 (hide under bkgnd)
	call	set_video_mode_init
	mov	_xpos,0
	mov	_ypos,0
	mov	_color,0Fh
	popad
	ret

VideoReset_:
	pushad
	call	sync_display
	call	set_video_mode_exit
	mov	ax,0200h
	mov	dx,_old_video_pos
	xor	ebx,ebx
	int	10h
	mov	esi,_buffer_addr
	mov	edi,_screen_addr
	mov	ecx,0800h
	rep	movsd
	popad
	ret

set_video_mode_init:
	pushad
	mov	ah,0Fh
	int	10h
	cmp	al,03h
	jnz	@@0
	cmp	ah,80
	jnz	@@0
	cmp	bh,00h
	jnz	@@0
	jmp	@@1
@@0:	mov	ax,0003h
	int	10h
@@1:	mov	ax,1003h		; toggle blink/background
	mov	bx,0001h
	int	10h
;	xor	eax,eax
;	mov	edi,_screen_addr
;	mov	ecx,0800h
;	rep	stosd
	popad
	ret
set_video_mode_exit:
	pushad
	mov	ah,0Fh
	int	10h
	cmp	al,03h
	jnz	@@0
	cmp	ah,80
	jnz	@@0
	cmp	bh,00h
	jnz	@@0
	jmp	@@1
@@0:	mov	ax,0003h
	int	10h
@@1:	mov	ax,1003h		; toggle blink/background
	mov	bx,0000h
	int	10h
;	xor	eax,eax
;	mov	edi,_screen_addr
;	mov	ecx,0800h
;	rep	stosd
	popad
	ret

DrawBackground_:
	pushad
	call	sync_display
	mov	edi,_screen_addr
	mov	ecx,80
	mov	ax,1120h
	rep	stosw
	mov	ecx,80*23
	mov	ax,1FB1h
	rep	stosw
	mov	ecx,80
	mov	ax,1120h
	rep	stosw
	popad
	ret



;-----------------------------------------------------------------------------
GetKey_:
	pushad
	xor	eax,eax
	int	16h
	mov	[esp+1Ch],eax
	mov	_keychar,al
	mov	_keycode,ah
	popad
	ret


Printc_:
	pushad
	cmp	al,0Ah
	jz	@@1
	cmp	al,0Dh
	jz	@@2
	mov	edi,_screen_addr
	movzx	ebx,_ypos
	lea	edx,[ebx*4]
	add	edx,ebx
	shl	edx,5
	add	edi,edx
	movzx	ebx,_xpos
	lea	ebx,[ebx*2]
	add	edi,ebx
	mov	[edi],al
	inc	edi
	mov	al,_color
	mov	[edi],al
	popad
	ret
@@1:	mov	_xpos,0			; handle 0Ah
	popad
	ret
@@2:	inc	_ypos			; handle 0Dh
	cmp	_ypos,25
	jb	@@3
	mov	_ypos,0
@@3:	popad
	ret



Prints_:
	pushad
	mov	esi,eax
@@loop:	mov	al,[esi]
	inc	esi
	test	al,al
	jz	@@done
	call	Printc_
	inc	_xpos
	cmp	_xpos,80
	jae	@@1
	jmp	@@loop
@@1:	mov	_xpos,0
	inc	_ypos
	cmp	_ypos,25
	jae	@@2
	jmp	@@loop
@@2:	mov	_ypos,0
	jmp	@@loop
@@done:	popad
	ret

SetPos_:
	pushad
	mov	_xpos,dl
	mov	_ypos,al
	popad
	ret

SetColor_:
	pushad
	and	al,0Fh
	mov	ah,_color
	and	ah,0F0h
	or	al,ah
	mov	_color,al
	popad
	ret

SetBackColor_:
	pushad
	and	al,0Fh
	mov	ah,_color
	and	ah,0Fh
	shl	al,4
	or	al,ah
	mov	_color,al
	popad
	ret


SetBlink_:
	pushad
	and	al,01h
	jnz	@@1
	and	_color,7Fh
	jmp	@@done
@@1:	or	_color,80h
@@done:	popad
	ret

ShowCursor_:
	pushad
	movzx	ebp,_xpos
	push	ebp
	movzx	ebp,_ypos
	push	ebp
	mov	_xpos,dl
	mov	_ypos,al
	mov	ecx,ebx
	mov	edi,_screen_addr
	movzx	ebx,_ypos
	lea	edx,[ebx*4]
	add	edx,ebx
	shl	edx,5
	add	edi,edx
	movzx	ebx,_xpos
	lea	ebx,[ebx*2]
	add	edi,ebx
	inc	edi
@@1:	mov	al,[edi]

	mov	ah,30h
	cmp	al,1Fh
	jz	@@2
	mov	ah,1Fh
	cmp	al,30h
	jz	@@2

	mov	ah,37h
	cmp	al,17h
	jz	@@2
	mov	ah,17h
	cmp	al,37h
	jz	@@2

	mov	ah,3Ch OR 80h
	cmp	al,1Ch OR 80h
	jz	@@2
	mov	ah,1Ch OR 80h
	cmp	al,3Ch OR 80h
	jz	@@2

	mov	ah,3Eh
	cmp	al,1Eh
	jz	@@2
	mov	ah,1Eh
	cmp	al,3Eh
	jz	@@2

	mov	ah,3Bh
	cmp	al,1Bh
	jz	@@2
	mov	ah,1Bh
	cmp	al,3Bh
	jz	@@2

@@2:	mov	[edi],ah
	add	edi,2
	loop	@@1
	pop	eax
	mov	_ypos,al
	pop	eax
	mov	_xpos,al
	popad
	ret


PrintHelp_:
	pushad
	mov	esi,eax

@@0:	lodsb
	test	al,al
	jz	@@done
	cmp	al,'['
	jz	@@l1on
	cmp	al,']'
	jz	@@l1of
	cmp	al,'<'
	jz	@@l2on
	cmp	al,'>'
	jz	@@l2of
	cmp	al,0Dh
	jz	@@0
	cmp	al,0Ah
	jnz	@@1
	inc	_ypos
	mov	_xpos,3
	jmp	@@0
@@1:	call	Printc_
	inc	_xpos
	jmp	@@0

@@done:	popad
	ret

@@l1on:	mov	al,_color
	mov	_color2,al
	and	al,0F0h
	or	al,004h
	mov	_color,al
	jmp	@@0
@@l1of:	mov	al,_color2
	mov	_color,al
	jmp	@@0
@@l2on:	mov	al,_color
	mov	_color3,al
	and	al,0F0h
	or	al,00Eh
	mov	_color,al
	jmp	@@0
@@l2of:	mov	al,_color3
	mov	_color,al
	jmp	@@0


sync_display:
	push	eax edx
	mov	dx,03DAh
@@1:	in	al,dx
	test	al,08h
	jnz	@@1
@@2:	in	al,dx
	test	al,08h
	jz	@@2
	pop	edx eax
	ret



save_window:
	pushad
	mov	esi,_screen_addr
	mov	ebx,_ywinpos
	lea	edx,[ebx*4]
	add	edx,ebx
	shl	edx,5
	add	esi,edx
	mov	ebx,_xwinpos
	lea	ebx,[ebx*2]
	add	esi,ebx
	mov	edi,_window_addr
	mov	ecx,_xwinlen
	mov	edx,_ywinlen
@@1:	push	esi
@@2:	mov	ax,[esi]
	mov	[edi],ax
	add	esi,2
	add	edi,2
	dec	ecx
	jnz	@@2
	pop	esi
	add	esi,80*2
	mov	ecx,_xwinlen
	dec	edx
	jnz	@@1
	mov	eax,_xwinpos
	mov	[edi+0*4],eax
	mov	eax,_ywinpos
	mov	[edi+1*4],eax
	mov	eax,_xwinlen
	mov	[edi+2*4],eax
	mov	eax,_ywinlen
	mov	[edi+3*4],eax
	add	edi,16
	mov	_window_addr,edi
	popad
	ret


restore_window:
	call	sync_display
	pushad
	mov	esi,_window_addr
	sub	esi,16
	mov	eax,[esi+0*4]
	mov	_xwinpos,eax
	mov	ebx,[esi+1*4]
	mov	_ywinpos,ebx
	mov	ecx,[esi+2*4]
	mov	_xwinlen,ecx
	mov	edx,[esi+3*4]
	mov	_ywinlen,edx
	imul	ecx,edx
	add	ecx,ecx
	sub	esi,ecx
	mov	_window_addr,esi
	mov	edi,_screen_addr
	mov	ebx,_ywinpos
	lea	edx,[ebx*4]
	add	edx,ebx
	shl	edx,5
	add	edi,edx
	mov	ebx,_xwinpos
	lea	ebx,[ebx*2]
	add	edi,ebx
	mov	ecx,_xwinlen
	mov	edx,_ywinlen
@@1:	push	edi
@@2:	mov	ax,[esi]
	mov	[edi],ax
	add	esi,2
	add	edi,2
	dec	ecx
	jnz	@@2
	pop	edi
	add	edi,80*2
	mov	ecx,_xwinlen
	dec	edx
	jnz	@@1
	popad
	ret


draw_window:
	pushad
	mov	edi,_screen_addr
	mov	ebx,_ywinpos
	lea	edx,[ebx*4]
	add	edx,ebx
	shl	edx,5
	add	edi,edx
	mov	ebx,_xwinpos
	lea	ebx,[ebx*2]
	add	edi,ebx
	mov	ecx,_xwinlen
	mov	edx,_ywinlen
@@1:	push	edi
@@2:	mov	[edi],ax
	add	edi,2
	dec	ecx
	jnz	@@2
	pop	edi
	add	edi,80*2
	mov	ecx,_xwinlen
	dec	edx
	jnz	@@1

	mov	eax,_xwinpos
	mov	edx,_ywinpos
	mov	_xpos,al
	mov	_ypos,dl
	mov	al,	'ษ'
	call	Printc_
	inc	_xpos
	mov	ecx,_xwinlen
	sub	ecx,2
	mov	al,	'อ'
@@3:	call	Printc_
	inc	_xpos
	loop	@@3
	mov	al,	'ป'
	call	Printc_

	mov	eax,_xwinpos
	mov	edx,_ywinpos
	inc	edx
	mov	_xpos,al
	mov	_ypos,dl
	mov	ecx,_ywinlen
	sub	ecx,2
	mov	al,	'บ'
@@4:	call	Printc_
	inc	_ypos
	loop	@@4
	mov	al,	'ศ'
	call	Printc_

	mov	eax,_xwinpos
	mov	edx,_ywinpos
	inc	edx
	dec	eax
	add	eax,_xwinlen
	mov	_xpos,al
	mov	_ypos,dl
	mov	ecx,_ywinlen
	sub	ecx,2
	mov	al,	'บ'
@@5:	call	Printc_
	inc	_ypos
	loop	@@5
	mov	al,	'ผ'
	call	Printc_

	mov	eax,_xwinpos
	mov	edx,_ywinpos
	inc	eax
	dec	edx
	add	edx,_ywinlen
	mov	_xpos,al
	mov	_ypos,dl
	mov	ecx,_xwinlen
	sub	ecx,2
	mov	al,	'อ'
@@6:	call	Printc_
	inc	_xpos
	loop	@@6

	mov	edi,_screen_addr
	mov	ebx,_ywinpos
	inc	ebx
	lea	edx,[ebx*4]
	add	edx,ebx
	shl	edx,5
	add	edi,edx
	mov	ebx,_xwinpos
	add	ebx,_xwinlen
	lea	ebx,[ebx*2]
	add	edi,ebx
	mov	ecx,_ywinlen
	inc	edi
	mov	al,07h
@@7:	mov	[edi+0],al
	mov	[edi+2],al
	add	edi,80*2
	loop	@@7

	mov	edi,_screen_addr
	mov	ebx,_ywinpos
	add	ebx,_ywinlen
	lea	edx,[ebx*4]
	add	edx,ebx
	shl	edx,5
	add	edi,edx
	mov	ebx,_xwinpos
	inc	ebx
	inc	ebx
	lea	ebx,[ebx*2]
	add	edi,ebx
	mov	ecx,_xwinlen
	inc	edi
	mov	al,07h
@@8:	mov	[edi],al
	add	edi,2
	loop	@@8

	popad
	ret


OpenWindow_:
	pushad
	inc	ebx
	inc	ecx
	inc	ecx
	mov	_xwinpos,edx
	mov	_ywinpos,eax
	mov	_xwinlen,ecx
	mov	_ywinlen,ebx
	call	save_window
	mov	al,20h
	mov	ah,_color
	dec	_xwinlen
	dec	_xwinlen
	dec	_ywinlen
	call	draw_window
	popad
	ret

DrawWindow_:
	pushad
	inc	ebx
	inc	ecx
	inc	ecx
	mov	_xwinpos,edx
	mov	_ywinpos,eax
	mov	_xwinlen,ecx
	mov	_ywinlen,ebx
	mov	al,20h
	mov	ah,_color
	dec	_xwinlen
	dec	_xwinlen
	dec	_ywinlen
	call	draw_window
	popad
	ret

CloseWindow_:
	pushad
	cmp	_window_addr,offs @window_buffer
	jbe	@@done
	call	restore_window
@@done:	popad
	ret




.DATA
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
_str_dos32a		db 'DOS/32A',0
_str_stub32c		db 'STUB/32C',0

_screen_addr	dd 000B8000h
_buffer_addr	dd offs @video_buffer
_window_addr	dd offs @window_buffer
_buffer		dd offs @file_buffer
_old_video_pos	dw 0
_xpos		db 0
_ypos		db 0
_color		db 0
_color2		db 0
_color3		db 0
_xwinpos	dd 0
_ywinpos	dd 0
_xwinlen	dd 0
_ywinlen	dd 0
_keychar	db 0
_keycode	db 0
_file_handle	dw 0


.DATA?
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
@video_buffer	db 2000h dup(?)
@window_buffer	db 10000h dup(?)
@file_buffer	db 400h dup(?)
end
