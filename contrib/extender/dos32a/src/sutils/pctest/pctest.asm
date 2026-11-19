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
; LOCALS
; NOJUMPS

TIMER	= 10000		; timer setup value
MEMORY	= 300*1024	; 150KB + 150KB = 300KB to overcome 256KB cache limit
VMEMORY	= 64*1024	; 64KB video memory
PATTERN	= 01010101h

EXT_UNKNOWN	= -1
EXT_DOS32A	= 0
EXT_PMODEW	= 1
EXT_PMODEWNEW	= 2
EXT_DOS4G	= 3
EXT_DOS4GW	= 4
EXT_CAUSEWAY	= 5
EXT_WINDOWS	= 6
EXT_EOS		= 7

PUBLIC	setvideomode_, gotoxy_
PUBLIC	get_cpu_type_, get_cpu_id_
PUBLIC	get_fpu_type_, get_fpu_info_
PUBLIC	get_sys_type_, get_extender_type_, get_dpmi_flags_
PUBLIC	get_himem_size_, get_lomem_size_, get_total_size_
PUBLIC	get_pmodew_ver_
PUBLIC	get_cpl_, get_iopl_

PUBLIC	test_cpu_
PUBLIC	test_fpu_
PUBLIC	test_int_
PUBLIC	test_irq_
PUBLIC	test_risc_
PUBLIC	test_priv_
PUBLIC	test_low_movsw_, test_low_movsd_
PUBLIC	test_high_movsw_, test_high_movsd_
PUBLIC	test_vid_movsw_, test_vid_movsd_
PUBLIC	test_low_stosw_, test_low_stosd_
PUBLIC	test_high_stosw_, test_high_stosd_
PUBLIC	test_vid_stosw_, test_vid_stosd_

;extrn	Debug_		: near
include	stddef.inc

.CODE
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

;=============================================================================
	Align 4
get_cpl_:
	push	ds
	push	es
	push	fs
	push	gs
	pushad
	mov	ax,cs
	lar	ax,ax
	shr	ax,13
	and	eax,03h
	mov	[esp+1Ch],eax
	popad
	pop	gs
	pop	fs
	pop	es
	pop	ds
	ret


;=============================================================================
	Align 4
get_iopl_:
	push	ds
	push	es
	push	fs
	push	gs
	pushad
	pushfd
	pop	eax
	shr	eax,10
	and	eax,03h
	mov	[esp+1Ch],eax
	popad
	pop	gs
	pop	fs
	pop	es
	pop	ds
	ret


;=============================================================================
	Align 4
get_dpmi_flags_:
	push	ds
	push	es
	push	fs
	push	gs
	pushad
	mov	ax,0400h
	int	31h
	movzx	eax,bx
	mov	[esp+1Ch],eax
	popad
	pop	gs
	pop	fs
	pop	es
	pop	ds
	ret


;=============================================================================
	Align 4
get_total_size_:
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
	shl	eax,10
	mov	[esp+1Ch],eax
	popad
	ret


;=============================================================================
	Align 4
get_himem_size_:
	push	ds
	push	es
	pushad
	sub	esp,48
	mov	edi,esp
	push	ss
	pop	es
	mov	ax,0500h
	int	31h
	mov	eax,[esp]
	add	esp,48
	mov	[esp+1Ch],eax
	popad
	pop	es
	pop	ds
	ret



;=============================================================================
	Align 4
get_lomem_size_:
	pushad
	mov	ax,0100h
	mov	bx,-1
	int	31h
	movzx	eax,bx
	shl	eax,4
	mov	[esp+1Ch],eax
	popad
	ret



;=============================================================================
	Align 4
get_extender_type_:
	push	ds
	push	es
	pushad

	mov	ax,1600h	; check for Enhanced Windows
	int	2Fh
	test	al,al
	jz	@get_extender_type_1
	cmp	al,80h
	jz	@get_extender_type_1
	jmp	@get_extender_type_windows

@get_extender_type_1:	push	es		; save ES: selector
	push	ds		; check for DOS/4G
	push	cs
	pop	ds
	mov	esi,offs @s1
	mov	ax,0A00h
	int	31h
	pop	ds
	pop	edx		; restore ES: selector in DX
	jc	@get_extender_type_unknown	; if carry set, unknown DOS Extender

	mov	ax,es		; check if the same ES selector was returned
	cmp	ax,dx
	mov	es,dx
	jz	@get_extender_type_pmodew	; if yes, it is PMODE/W or EOS Extenders

	push	ds		; check for DOS/32 Advanced
	push	cs
	pop	ds
	mov	esi,offs @s2
	mov	ax,0A00h
	int	31h
	pop	ds
	jnc	@get_extender_type_dos32a

	mov	ax,cs		; check for CAUSEWAY
	lar	ax,ax
	shr	ax,13
	and	ax,3
	cmp	ax,3
	jz	@get_extender_type_causeway	; if DPL = 3 it is CauseWay Extender


	xor	ebx,ebx
	xor	ecx,ecx
	xor	esi,esi
	xor	edi,edi
	mov	ax,0306h	; get Raw Mode Switch Addr
	int	31h
	jc	@get_extender_type_dos4gw	; DOS/4GW does not support this call
	mov	ax,bx
	or	ax,cx
	jz	@get_extender_type_dos4gw
	mov	eax,esi
	or	eax,edi
	jz	@get_extender_type_dos4gw
	lar	ax,si		; check returned selector
	jnz	@get_extender_type_dos4gw

	jmp	@get_extender_type_dos4g

@get_extender_type_done:	mov	[esp+1Ch],eax
	popad
	pop	es
	pop	ds
	ret

@get_extender_type_unknown:
	mov	eax,EXT_UNKNOWN
	jmp	@get_extender_type_done
@get_extender_type_dos32a:
	mov	eax,EXT_DOS32A
	jmp	@get_extender_type_done
@get_extender_type_pmodew:
	mov	ax,cs		; check for EOS first
	lar	ax,ax
	shr	ax,13
	and	ax,3
	cmp	ax,3
	jz	@get_extender_type_eos		; if DPL = 3 it is EOS
	mov	ax,0EEFFh
	int	31h
	jnc	@get_extender_type_pmodew_new
	mov	eax,EXT_PMODEW
	jmp	@get_extender_type_done
@get_extender_type_pmodew_new:
	mov	eax,EXT_PMODEWNEW
	jmp	@get_extender_type_done
@get_extender_type_dos4g:
	mov	eax,EXT_DOS4G
	jmp	@get_extender_type_done
@get_extender_type_dos4gw:
	mov	eax,EXT_DOS4GW
	jmp	@get_extender_type_done
@get_extender_type_causeway:
	mov	eax,EXT_CAUSEWAY
	jmp	@get_extender_type_done
@get_extender_type_windows:
	mov	eax,EXT_WINDOWS
	jmp	@get_extender_type_done
@get_extender_type_eos:	mov	eax,EXT_EOS
	jmp	@get_extender_type_done

@s1	db 'RATIONAL DOS/4G',0
@s2	db 'SUNSYS DOS/32A',0

get_pmodew_ver_:
	pushad
	mov	ax,0EEFFh
	int	31h
	movzx	eax,dx
	jnc	@get_pmodew_ver_1
	mov	eax,-1
@get_pmodew_ver_1:	mov	[esp+1Ch],eax
	popad
	ret


;=============================================================================
	Align 4
get_fpu_info_:
	pushad
	call	get_extender_type_
	cmp	eax,EXT_CAUSEWAY; if running under CauseWay
	jz	@get_fpu_info_exit		; do not check for FPU
	cmp	eax,EXT_EOS	; if running under EOS
	jz	@get_fpu_info_exit		; do not check for FPU
	cmp	eax,EXT_UNKNOWN
	jnz	@get_fpu_info_1
	mov	eax,0E00h
	int	31h
	jc	@get_fpu_info_exit
	cmp	ax,0E00h
	jz	@get_fpu_info_exit

@get_fpu_info_1:	mov	eax,0E00h	; try DPMI check FPU installation
	int	31h
	jc	@get_fpu_info_l0		; if not supported, jump
	cmp	ax,0E00h
	jz	@get_fpu_info_l0		; if simply cleared CF, jump
	mov	edx,eax
	and	edx,03h		; HOST: bit0=MPr, bit1=EMr
	shr	eax,2
	and	eax,03h		; CLIENT: bit0=MPr, bit1=EMr
	or	eax,edx
	jmp	@get_fpu_info_done

@get_fpu_info_l0:	mov	eax,cr0		; try native instruction
	mov	edx,eax
	shr	eax,1
	and	eax,03h
	and	edx,10h
	shr	edx,4
	or	eax,edx

@get_fpu_info_done:	mov	[esp+1Ch],eax
	popad
	ret

@get_fpu_info_exit:	mov	eax,1		;  don't check for FPU type
	jmp	@get_fpu_info_done

;=============================================================================
; Get CPU Vendor specific ID string
;
	Align 4
get_cpu_id_:
	pushfd
	pushad
	cli
	cld
	mov	cpuid_name[0],0		; reset string
	pushfd
	pop	eax
	mov	ecx,eax
	xor	eax,00200000h
	push	eax
	popfd
	pushfd
	pop     eax
	xor     eax,ecx
	jne	@get_cpu_id_1
	jmp	@get_cpu_id_done
.586p
@get_cpu_id_1:	xor	eax,eax			; set up for CPUID instruction
	cpuid
	mov	dword ptr cpuid_name[0],ebx
	mov     dword ptr cpuid_name[4],edx
	mov     dword ptr cpuid_name[8],ecx
.486p
@get_cpu_id_done:	sti
	popad
	popfd
	mov	eax,offset cpuid_name
	ret



;=============================================================================
; Get CPU type:	80386, 80486, PENTIUM, ...
;
	Align 4
get_cpu_type_:
	pushad
	push	ds
	push	es
	push	fs
	push	gs
	mov	ax,0400h		; DPMI get CPU
	int	31h
	pop	gs
	pop	fs
	pop	es
	pop	ds
	movzx	eax,cl
	mov	[esp+1Ch],eax
	cmp	cl,3
	ja	@get_cpu_type_1
	popad
	ret
@get_cpu_type_1:	pushfd
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
	jne	@get_cpu_type_l4
	jmp	@get_cpu_type_x
@get_cpu_type_l4:	xor	eax,eax
.586p
	cpuid
	mov	dword ptr [esp+1Ch+4],5	; CPU is 586
	cmp	eax,1			; check CPUID level
	jb	@get_cpu_type_x			; if level 1 not supported, exit
	mov	eax,1
	cpuid
.486p
	and     eax,0F00H
	shr	ax,8
	movzx	eax,al
	mov	[esp+1Ch+4],eax
@get_cpu_type_x:	sti
	popfd
	popad
	ret



;=============================================================================
; Get FPU type:	8087, 80287, 80387, PENTIUM, ...
;
	Align 4
get_fpu_type_:
	pushad
	call	get_cpu_type_
	mov	edx,eax
	xor	eax,eax
	push	eax
	fninit
	fnstcw	word ptr [esp+2]
	mov	ax,word ptr [esp+2]
	cmp	ah,03h
	jnz	@get_fpu_type_done				; no FPU present
	mov	word ptr [esp],1
	and	word ptr [esp+2],0FF7Fh
	wait
	fldcw	word ptr [esp+2]
	fdisi
	fstcw	word ptr [esp+2]
	wait
	test	word ptr [esp+2],0080h
	jnz	@get_fpu_type_done				; 8087 is present
	mov	word ptr [esp],2
	fninit
	wait
	fld1
	wait
	fldz
	wait
	fdivp	st(1),st
	wait
	fld	st(0)
	wait
	fchs
	wait
	fcompp
	wait
	fnstsw	ax
	wait
	fnclex
	wait
	sahf
	jz	@get_fpu_type_done
	mov	word ptr [esp],3		; 387
	cmp	edx,4
	jb	@get_fpu_type_done
	mov	eax,edx
	mov	byte ptr [esp],al		; 487+ (build-in)
@get_fpu_type_done:	pop	eax
	and	eax,7
	jz	@get_fpu_type_exit
	fldz
	fldz
	fldz
	fldz
	fldz
	fldz
	fldz
	fldz
	finit
@get_fpu_type_exit:	mov	[esp+1Ch],eax
	popad
	ret



;=============================================================================
; Get SYSTEM type:	raw, XMS, VCPI, DPMI
;
	Align 4
get_sys_type_:
	pushad
	call	get_extender_type_
	test	eax,eax
	jnz	@get_sys_type_1
	push	ds
	push	es
	mov	esi,offs @s2
	push	cs
	pop	ds
	mov	ax,0A00h
	int	31h
	jc	@get_sys_type_err
	movzx	eax,bh
	pop	es
	pop	ds
	jmp	@get_sys_type_done

@get_sys_type_1:	cmp	eax,EXT_PMODEWNEW
	jnz	@get_sys_type_err
	mov	ax,0EEFFh
	int	31h
	jc	@get_sys_type_err
	movzx	eax,ch
	jmp	@get_sys_type_done

@get_sys_type_err:	mov	eax,-1
@get_sys_type_done:	mov	[esp+1Ch],eax
	popad
	ret





;=============================================================================
; Measure CPU speed by issuing 1000 slow 32-bit DIV instructions
;
; Returns:	EAX = number of PIT ticks * 2
;
	Align 4
test_cpu_:
	call	get_cpu_type_
	cmp	eax,5
	jae	test_cpu586_
	pushfd
	pushad
	cli
	cld
	call	disable_irqs
	xor	eax,eax
	xor	edx,edx
	mov	ebx,1

	Align	4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rept	1000
	div	ebx
	endm

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax
	mov	[esp+1Ch],eax
	call	enable_irqs
	sti
	popad
	popfd
	ret

.586p
test_cpu586_:
	pushfd
	pushad
	cli
	mov	ax,ds
	mov	cpu586_cs,ax


	mov	ax,0204h
	mov	bl,08h
	int	31h
	push	ecx
	push	edx

	mov	ax,0205h
	mov	bl,08h
	mov	cx,cs
	mov	edx,offs __586int
	int	31h

	mov	al,36h		; reset PIT frequency
	out	43h,al
	mov	al,00h
	out	40h,al
	mov	al,00h
	out	40h,al
	mov	__586ack,10
	mov	ecx,7FFFFFFh
	sti
	rdtsc
	mov	esi,eax
	mov	edi,edx

@test_cpu586_1:	cmp	bptr __586ack,0
	loopnz	@test_cpu586_1

	rdtsc
	cli
	sub	eax,esi
	sbb	edx,edi

	mov	ebp,esp
	push	edx
	push	eax
	fild	qword ptr [ebp-8]
	fld	qword ptr __586dat2
	fld	qword ptr __586dat1
	fld1
	fdivrp	st(1)
	fmulp	st(1)
	fdivp	st(1)
	fistp	qword ptr [ebp-8]
	pop	eax
	pop	edx

	pop	edx
	pop	ecx
	pushad
	mov	ax,0205h
	mov	bl,08h
	int	31h
	popad
	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret

__586int:
	push	ds
	push	eax
	mov	ds,cs:cpu586_cs
	dec	bptr __586ack
	mov	al,20h
	out	20h,al
	pop	eax
	pop	ds
	iretd
.486p



;=============================================================================
; Measure CPU speed by issuing 1000 32-bit RISC instructions (ADD)
;
; Returns:	EAX = number of ticks * 2
;
	Align 4
test_risc_:
	pushfd
	pushad
	cli
	cld
	call	disable_irqs
	xor	eax,eax
	mov	edx,1

	Align	4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rept	1000
	add	eax,edx
	endm

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax
	mov	[esp+1Ch],eax
	call	enable_irqs
	sti
	popad
	popfd
	ret


;=============================================================================
; Measure Privelege instructions speed
;
; Returns:	EAX = number of ticks * 2
;
	Align 4
test_priv_:
	pushfd
	pushad
	cli
	cld
	call	disable_irqs
	xor	eax,eax

	Align	4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rept	1000
	cli
	endm

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax
	mov	[esp+1Ch],eax
	call	enable_irqs
	sti
	popad
	popfd
	ret





;=============================================================================
; Measure FPU speed by issuing 1000 simple operations (FADD)
;
; Returns:	EAX = number of ticks * 2
;
	Align 4
test_fpu_:
	pushfd
	pushad
	call	get_fpu_type_
	test	eax,eax
	jz	@test_fpu_err			; if no FPU, exit
	cli
	cld
	call	disable_irqs
	xor	eax,eax
	finit				; initialize FPU
	fldpi				; load +0 into ST(0)
	fld1				; load +1 into ST(0)

	Align	4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rept	1000
	fadd	st(0),st(1)
	endm

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax
	mov	[esp+1Ch],eax
	finit				; reinitialize FPU
	call	enable_irqs
@test_fpu_done:	sti
	popad
	popfd
	ret
@test_fpu_err:	mov	eax,-1
	mov	[esp+1Ch],eax
	jmp	@test_fpu_done









;=============================================================================
; Measure Mode switch rate by issuing an INT instruction from Protected Mode
;  to Real Mode and then back to Protected Mode
;
; Returns:	low word(EAX)  = number of ticks * 2 for  Protected to Real
;		high word(EAX) = number of ticks * 2 for  Real to Protected
;
	Align 4
test_int_:
	pushfd
	pushad
	cli
	cld
	call	disable_irqs

	mov	bl,80h		; get real mode INT 80h
	mov	ax,0200h
	int	31h
	push	ecx
	push	edx

	mov	ax,int_mem_seg
	test	ax,ax
	jnz	@test_int_x1

	mov	bx,4		; allocate real mode memory (64 bytes)
	mov	ax,0100h
	int	31h
	mov	int_mem_seg,ax
	mov	int_mem_sel,dx
;	push	dx		; preserve selector on stack

	push	es
	mov	es,dx		; copy real mode handler to DOS memory
	mov	edi,0			; ES:EDI = destination
	mov	esi,offs int80_RM	; DS:ESI = source
	mov	ecx,64			; ECX = size
	rep	movsb
	pop	es

@test_int_x1:	mov	bl,80h
	mov	cx,ax		; CX = segment returned by INT 31h AX=0100h
	mov	dx,0		; DX = offset is zero
	mov	ax,0201h
	int	31h

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	int	80h		; OUT: EBX = switch Protected/Real

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al

	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax

	neg	ax		; EAX = switch Real/Protected
	neg	bx		; EBX = switch Protected/Real
	shl	eax,16
	and	ebx,0FFFFh
	or	eax,ebx		; EAX = [ high=RMPM | low=PMRM ]
	mov	ebp,eax

;	pop	dx		; free real mode DOS memory
;	mov	ax,0101h
;	int	31h

	pop	edx	; restore original Real Mode INT 80h
	pop	ecx
	mov	bl,80h
	mov	ax,0201h
	int	31h
	call	enable_irqs

	mov	[esp+1Ch],ebp
	sti
	popad
	popfd
	ret

;--------------------------------
	Align 4
int80_RM:			; Real Mode Interrupt Handler (INT 80h)
	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	mov	bx,ax		;**NOTE: mov ebx,eax
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	iret




;=============================================================================
; Measure Mode switch rate by trapping IRQ 0 (INT 08h - timer) in Real Mode
;  and then waiting for hardware interrupt
;
; Returns:	low word(EAX)  = number of ticks * 2 for  Protected to Real
;		high word(EAX) = number of ticks * 2 for  Real to Protected
;
	Align 4
test_irq_:
	pushfd
	pushad
	cli
	cld

	mov	ax,irq_mem_seg
	test	ax,ax
	jnz	@test_irq_x1

	mov	bx,4		; allocate real mode memory (64 bytes)
	mov	ax,0100h
	int	31h
	mov	irq_mem_seg,ax
	mov	irq_mem_sel,dx

	push	es
	mov	es,dx		; copy real mode handler to DOS memory
	xor	edi,edi			; ES:EDI = destination
	mov	esi,offs int08_RM	; DS:ESI = source
	mov	ecx,64			; ECX = size
	rep	movsb
	sub	edi,4
	mov	eax,-1
	stosd
	pop	es

@test_irq_x1:	mov	bl,08h		; get real mode IRQ 0 (INT 08h)
	mov	ax,0200h
	int	31h
	push	ecx	;*
	push	edx

	mov	bl,08h
	mov	ax,0204h
	int	31h
	push	ecx	;**
	push	edx

	mov	bl,08h		; install Real Mode IRQ 0 (INT 08h)
	mov	cx,irq_mem_seg
	mov	dx,0
	mov	ax,0201h
	int	31h

	push	ds
	mov	ds,irq_mem_sel
	mov	eax,-1
	mov	edx,-1
	mov	edi,60
	mov	ds:[edi],eax
;	mov	ecx,3FFFFFh

	mov	al,36h		; reset PIT frequency
	out	43h,al
	mov	al,00h
	out	40h,al
	mov	al,80h
	out	40h,al

	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al		; the PIT is started and running
	sti			; enable interrupts

; note: IRQ PM->RM (down) switch speed measurement is unreliable because we
; do not know the time elapsed between "sti" above and occurance of a hardware
; interrupt (and we cannot trigger a hardware interrupt on our own)

	Align	4
@test_irq_loop:	mov	bx,ds:[edi]	; BX = (PM to RM) + HLT switch time
	cmp	bx,dx
	jz	@test_irq_loop
	pop	ds
	cli			; disable interrupts

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	mov	al,36h
	out	43h,al
	mov	al,00h
	out	40h,al
	out	40h,al
	pop	eax

	neg	ax		; EAX = switch Real/Protected
	neg	bx		; EBX = switch Protected/Real
	shl	eax,16
	mov	ax,bx		; EAX = [ high=RMPM | low=PMRM ]
	mov	ebp,eax

	pop	edx	; restore original Prot. Mode IRQ 0
	pop	ecx
	mov	bl,08h
	mov	ax,0205h
	int	31h

	pop	edx	; restore original Real Mode IRQ 0
	pop	ecx
	mov	bl,08h
	mov	ax,0201h
	int	31h

;	mov	dx,irq_mem_sel	; free real mode DOS memory
;	mov	ax,0101h
;	int	31h

	mov	[esp+1Ch],ebp	; EAX = [ high=RM2PM | low=PM2RM ]
	sti
	popad
	popfd
	ret


;--------------------------------
	Align 4
int08_RM:			; Real Mode Interrupt Handler (INT 08h)
	push	eax
	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	db 2Eh, 0A3h, 3Ch, 00h	; real mode:	mov cs:[60],ax
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	mov	al,20h
	out	20h,al		; acknowledge interrupt receiving
	pop	eax
	iret

	Align	4
int08_PM:
	push	eax
	mov	al,20h
	out	20h,al
	pop	eax
	iretd








;=============================================================================
	Align 4
test_low_movsw_:
	pushfd
	pushad
	call	alloc_low
	mov	ebx,eax			; ebx = selector
	mov	eax,-1
	jc	@test_low_movsw_done

	cli
	cld
	push	ds
	push	es
	mov	ecx,MEMORY/2		; ECX = size of memory block
	xor	esi,esi			; ESI = source base
	lea	edi,[esi+ecx]		; EDI = destination base
	shr	ecx,1			; adjust ECX for operand size
	mov	ds,bx
	mov	es,bx

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rep	movsw

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax

	pop	es
	pop	ds
	call	dealloc_low

@test_low_movsw_done:	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret


;=============================================================================
	Align 4
test_low_movsd_:
	pushfd
	pushad
	call	alloc_low
	mov	ebx,eax			; ebx = selector
	mov	eax,-1
	jc	@test_low_movsd_done

	cli
	cld
	push	ds
	push	es
	mov	ecx,MEMORY/2		; ECX = size of memory block
	xor	esi,esi			; ESI = source base
	lea	edi,[esi+ecx]		; EDI = destination base
	shr	ecx,2			; adjust ECX for operand size
	mov	ds,bx
	mov	es,bx

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rep	movsd

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax

	pop	es
	pop	ds
	call	dealloc_low

@test_low_movsd_done:	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret



;=============================================================================
	Align 4
test_low_stosw_:
	pushfd
	pushad
	call	alloc_low
	mov	ebx,eax			; ebx = selector
	mov	eax,-1
	jc	@test_low_stosw_done

	cli
	cld
	push	es
	mov	ecx,MEMORY		; ECX = size of memory block
	xor	edi,edi			; EDI = destination base
	shr	ecx,1			; adjust ECX for operand size
	mov	es,bx
	mov	eax,PATTERN

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rep	stosw

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax

	pop	es
	call	dealloc_low

@test_low_stosw_done:	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret


;=============================================================================
	Align 4
test_low_stosd_:
	pushfd
	pushad
	call	alloc_low
	mov	ebx,eax			; ebx = selector
	mov	eax,-1
	jc	@test_low_stosd_done

	cli
	cld
	push	es
	mov	ecx,MEMORY		; ECX = size of memory block
	xor	edi,edi			; EDI = destination base
	shr	ecx,2			; adjust ECX for operand size
	mov	es,bx
	mov	eax,PATTERN

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rep	stosd

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax

	pop	es
	call	dealloc_low

@test_low_stosd_done:	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret















;=============================================================================
	Align 4
test_high_movsw_:
	pushfd
	pushad
	call	alloc_hi
	mov	ebx,eax			; ebx = selector
	mov	eax,-1
	jc	@test_high_movsw_done

	cli
	cld
	push	ds
	push	es
	mov	ecx,MEMORY/2		; ECX = size of memory block
	xor	esi,esi			; ESI = source base
	lea	edi,[esi+ecx]		; EDI = destination base
	shr	ecx,1			; adjust ECX for operand size

	mov	ds,bx
	mov	es,bx

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rep	movsw

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax

	pop	es
	pop	ds
	call	dealloc_hi

@test_high_movsw_done:	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret


;=============================================================================
	Align 4
test_high_movsd_:
	pushfd
	pushad
	call	alloc_hi
	mov	ebx,eax			; ebx = selector
	mov	eax,-1
	jc	@test_high_movsd_done

	cli
	cld
	push	ds
	push	es
	mov	ecx,MEMORY/2		; ECX = size of memory block
	xor	esi,esi			; ESI = source base
	lea	edi,[esi+ecx]		; EDI = destination base
	shr	ecx,2			; adjust ECX for operand size
	mov	ds,bx
	mov	es,bx

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rep	movsd

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax

	pop	es
	pop	ds
	call	dealloc_hi

@test_high_movsd_done:	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret


;=============================================================================
	Align 4
test_high_stosw_:
	pushfd
	pushad
	call	alloc_hi
	mov	ebx,eax			; ebx = selector
	mov	eax,-1
	jc	@test_high_stosw_done

	cli
	cld
	push	es
	mov	ecx,MEMORY		; ECX = size of memory block
	xor	edi,edi			; EDI = destination base
	shr	ecx,1			; adjust ECX for operand size
	mov	es,bx
	mov	eax,PATTERN

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rep	stosw

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax

	pop	es
	call	dealloc_hi

@test_high_stosw_done:	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret


;=============================================================================
	Align 4
test_high_stosd_:
	pushfd
	pushad
	call	alloc_hi
	mov	ebx,eax			; ebx = selector
	mov	eax,-1
	jc	@test_high_stosd_done

	cli
	cld
	push	es
	mov	ecx,MEMORY		; ECX = size of memory block
	xor	edi,edi			; EDI = destination base
	shr	ecx,2			; adjust ECX for operand size
	mov	es,bx
	mov	eax,PATTERN

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rep	stosd

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax

	pop	es
	call	dealloc_hi

@test_high_stosd_done:	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret









;=============================================================================
	Align 4
test_vid_movsw_:
	pushfd
	pushad
	call	alloc_hi
	mov	ebx,eax			; ebx = selector
	mov	eax,-1
	jc	@test_vid_movsw_done
	call	alloc_vid

	cli
	cld
	push	ds
	push	es
	mov	ecx,VMEMORY		; ECX = size of memory block
	xor	esi,esi			; ESI = source base
	xor	edi,edi			; EDI = destination base
	shr	ecx,1			; adjust ECX for operand size
	mov	ds,bx
	mov	es,ax

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rep	movsw

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax

	pop	es
	pop	ds
	call	dealloc_vid
	call	dealloc_hi

@test_vid_movsw_done:	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret


;=============================================================================
	Align 4
test_vid_movsd_:
	pushfd
	pushad
	call	alloc_hi
	mov	ebx,eax			; ebx = selector
	mov	eax,-1
	jc	@test_vid_movsd_done
	call	alloc_vid

	cli
	cld
	push	ds
	push	es
	mov	ecx,VMEMORY		; ECX = size of memory block
	xor	esi,esi			; ESI = source base
	xor	edi,edi			; EDI = destination base
	shr	ecx,2			; adjust ECX for operand size
	mov	ds,bx
	mov	es,ax

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rep	movsd

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax

	pop	es
	pop	ds
	call	dealloc_vid
	call	dealloc_hi

@test_vid_movsd_done:	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret


;=============================================================================
	Align 4
test_vid_stosw_:
	pushfd
	pushad
	call	alloc_vid

	cli
	cld
	push	es
	mov	ecx,VMEMORY		; ECX = size of memory block
	xor	edi,edi			; EDI = destination base
	shr	ecx,1			; adjust ECX for operand size
	mov	es,ax
	mov	eax,PATTERN

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rep	stosw

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax

	pop	es
	call	dealloc_vid

@test_vid_stosw_done:	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret


;=============================================================================
	Align 4
test_vid_stosd_:
	pushfd
	pushad
	call	alloc_vid

	cli
	cld
	push	es
	mov	ecx,VMEMORY		; ECX = size of memory block
	xor	edi,edi			; EDI = destination base
	shr	ecx,2			; adjust ECX for operand size
	mov	es,ax
	mov	eax,PATTERN

	Align 4
	mov	al,0B6h
	out	43h,al
	in	al,61h
	or	al,01h
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al

	rep	stosd

	mov	al,80h
	out	43h,al
	in	al,42h
	mov	ah,al
	in	al,42h
	xchg	ah,al
	push	eax
	mov	al,0B0h
	out	43h,al
	in	al,61h
	and	al,0FDh
	out	61h,al
	xor	al,al
	out	42h,al
	out	42h,al
	pop	eax
	neg	ax
	movzx	eax,ax

	pop	es
	call	dealloc_vid

@test_vid_stosd_done:	mov	[esp+1Ch],eax
	sti
	popad
	popfd
	ret









;=============================================================================
	Align 4
disable_irqs:
	push	eax
	in	al,21h
	mov	ah,al
	in	al,0A1h
	mov	pic_mask,ax		; get and save pic mask
	mov	al,0FFh
	out	21h,al			; disable all IRQs except IRQ1 (kbrd)
	mov	al,0FFh
	out	0A1h,al
	pop	eax
	cli
	ret
	Align 4
enable_irqs:
	push	eax
	mov	ax,pic_mask
	out	0A1h,al
	mov	al,ah
	out	21h,al
	pop	eax
	sti
	ret

	Align 4
alloc_low:
	pushad
	mov	ax,0100h
	mov	bx,MEMORY/16
	int	31h
	mov	eax,-1
	mov	mem_dx,0
	jc	@alloc_low_done
	mov	mem_dx,dx

	mov	bx,dx
	mov	ax,0006h		; get selector base
	int	31h

	push	ecx
	push	edx
	mov	bx,ds
	mov	ax,000Ah		; allocate alias selector
	int	31h
	mov	ebx,eax

	pop	edx
	pop	ecx
	mov	ax,0007h		; set selector base
	int	31h

	mov	ecx,MEMORY
	mov	edx,ecx
	shr	ecx,16
	mov	ax,0008h		; set selector limit
	int	31h

	xor	eax,eax
	mov	ax,bx
	mov	mem_sel,ax
@alloc_low_done:	mov	[esp+1Ch],eax		; return selector
	popad
	ret

	Align 4
alloc_hi:
	pushad
	mov	ecx,MEMORY
	mov	ebx,ecx
	shr	ebx,16
	mov	ax,0501h		; allocate memory
	int	31h
	mov	eax,-1
	mov	mem_si,0
	mov	mem_di,0
	jc	@alloc_hi_done
	mov	mem_si,si
	mov	mem_di,di
	mov	edx,ecx
	mov	ecx,ebx

	push	ecx
	push	edx
	mov	bx,ds
	mov	ax,000Ah		; allocate alias selector
	int	31h
	mov	ebx,eax
	mov	mem_sel,ax
	pop	edx
	pop	ecx

	push	ecx
	push	edx
	mov	ax,0007h		; set selector base
	int	31h

	mov	ecx,MEMORY
	mov	edx,ecx
	shr	ecx,16
	mov	ax,0008h		; set selector limit
	int	31h

	mov	esi,ecx
	mov	edi,edx
	pop	ecx
	pop	ebx
	mov	ax,0600h		; lock pages
	int	31h

	xor	eax,eax
	mov	ax,mem_sel
@alloc_hi_done:	mov	[esp+1Ch],eax		; return selector
	popad
	ret

	Align 4
alloc_vid:
	pushad
	mov	bx,ds
	mov	ax,000Ah		; allocate alias selector
	int	31h
	mov	ebx,eax

	mov	ecx,0A0000h
	mov	edx,ecx
	shr	ecx,16
	mov	ax,0007h		; set selector base
	int	31h

	mov	ecx,VMEMORY
	mov	edx,ecx
	shr	ecx,16
	mov	ax,0008h		; set selector limit
	int	31h

	xor	eax,eax
	mov	ax,bx
	mov	mem_vid,ax
	mov	[esp+1Ch],eax		; return selector
	popad
	ret



	Align 4
dealloc_low:
	pushad
	mov	dx,mem_dx
	test	dx,dx
	jz	@dealloc_low_done
	mov	ax,0101h
	int	31h
	mov	bx,mem_sel
	mov	ax,0001h
	int	31h
@dealloc_low_done:	popad
	ret

	Align 4
dealloc_hi:
	pushad
	mov	si,mem_si
	mov	di,mem_di
	mov	ax,si
	or	ax,di
	jz	@dealloc_hi_done
	mov	ax,0502h
	int	31h
	mov	bx,mem_sel
	mov	ax,0001h
	int	31h
@dealloc_hi_done:	popad
	ret

	Align 4
dealloc_vid:
	pushad
	mov	bx,mem_vid
	mov	ax,0001h
	int	31h
@dealloc_vid_done:	popad
	ret






;=============================================================================
	Align 4
setvideomode_:
	pushad
	xor	ah,ah
	int	10h
	popad
	ret

	Align 4
gotoxy_:
	pushad
	mov	dh,al
	xchg	dh,dl
	mov	ax,0200h
	xor	bx,bx
	int	10h
	popad
	ret





.DATA
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
__586ack	db 0,0
__586dat1	dq 18.2064971
__586dat2	dq 10.0000000
cpu586_cs	dw 0
mem_si		dw 0
mem_di		dw 0
mem_dx		dw 0
mem_sel		dw 0
mem_seg		dw 0
mem_vid		dw 0
pic_mask	dw 0
cpuid_name	db 20 dup(0)

int_mem_sel	dw 0
int_mem_seg	dw 0
irq_mem_sel	dw 0
irq_mem_seg	dw 0


end
