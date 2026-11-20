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

;=============================================================================
; Get Protected Mode Info
;=========================
; In:	Nothing
; Out:	AX = return code:
;	 0000h = DOS version below 4.00
;	 0001h = no 80386+ detected
;	 0002h = system already in protected mode and no VCPI or DPMI found
;	 0003h = DPMI - host is not 32bit
;	 0004h = VCPI - incompatible PIC mappings
;	CF = set on error, if no error:
;	 BX = number of paragraphs needed for protected mode data (may be 0)
;	 CL = processor type
;	 CH = protected mode type:
;		00h = raw
;		01h = XMS
;		02h = VCPI
;		03h = DPMI
;	DX = size of removable Kernel body (in bytes)
;	DI = base of removable Kernel body
;=============================================================================
	Align 4
@area1_db label byte
@area1_dw label word
@area1_dd label dword

pm32_info:
	cld
	push	ds	; preserve registers
	push	es
	push	cs	; DS = CS (_KERNEL)
	push	cs
	pop	ds
	pop	es

	call	cpu_detect		; get processor type
	cmp	al,3			; check if processor is 80386+
	mov	ax,1			; error in case no 80386+
	jae	@pm32_info_0
	jmp	@pm32_info_fail

@pm32_info_0:	call	fpu_detect		; detect/initialize FPU

	mov	ah,30h			; get DOS version
	int	21h
If DOS_LEGACY_125_20 ne 0
	test	al,al			; DOS 1.x without AH=30h returns AL=0
	jnz	@pm32_info_ver_ok
	mov	al,1
@pm32_info_ver_ok:
	mov	dos_ver_major,al
	mov	dos_ver_minor,ah
	cmp	al,3			; XMS/multiplex INT 2Fh requires DOS 3.0+ and non-null IVT[2Fh]
	jb	@pm32_info_1
	push	es
	xor	ax,ax
	mov	es,ax
	mov	eax,es:[2Fh*4]
	pop	es
	test	eax,eax
	jz	@pm32_info_1
Else
	cmp	al,4			; if DOS version is below 4.0
	mov	ax,0			; return with error code 0
	jb	@pm32_info_fail
Endif

	mov	ax,4300h		; check for XMS
	int	2Fh
	cmp	al,80h
	jnz	@pm32_info_1			; jump if XMS not found

	push	es
	mov	ax,4310h
	int	2Fh
	mov	xms_call[0],bx		; store XMS driver address
	mov	xms_call[2],es
	mov	ah,30h
	int	21h			; HIMEM.SYS bug fix
	mov	ah,88h
	xor	bx,bx
	call	dptr xms_call		; get XMS v3 free mem in KB
	test	bl,bl			; if no XMS v3, use v2
	jz	@pm32_info_xms1
	mov	ah,08h
	call	dptr xms_call		; get XMS v2 free mem in KB
	movzx	eax,ax
@pm32_info_xms1:	mov	xms_data,eax
	pop	es

	pushf
	pop	ax			; AX = flags
	and	ah,0CFh			; reset IOPL to 0
	push	ax			;  (can only be done at CPL 0)
	popf				; reload flags with new IOPL
	pushf
	pop	ax			; AX = flags
	test	ah,30h			; is IOPL still 0?
	jz	@pm32_info_4			; if yes, omit VCPI/DPMI tests

@pm32_info_1:	call	@pm32_info_detect_VCPI		; check for VCPI first
	call	@pm32_info_detect_DPMI		; check for DPMI second

	smsw	ax			; AX = machine status word
	and	al,1			; is system in protected mode?
	mov	ax,2			; error code in case in protected mode
	jnz	@pm32_info_fail			; if in protected mode, fail

@pm32_info_4:	cmp	xms_data,0		; check if XMS_mem is present
	setnz	ch			; if yes, pmode type is XMS
	mov	bx,80h			; BX = memory requirement (IDT)

@pm32_info_vcpi:	movzx	ax,pm32_rmstacks	; size of real mode stack area
	imul	ax,pm32_rmstacklen
	add	bx,ax

	movzx	ax,pm32_pmstacks	; size of protected mode stack area
	imul	ax,pm32_pmstacklen
	add	bx,ax

	movzx	ax,pm32_callbacks	; size of callbacks
	imul	ax,25
	add	ax,0Fh
	shr	ax,4
	add	bx,ax

	mov	ax,pm32_selectors	; size of GDT
	add	ax,SYSSELECTORS+1
	shr	ax,1
	add	bx,ax

@pm32_info_done:	xor	ax,ax			; success code, also clear carry flag
	mov	cl,cputype
	mov	pmodetype,ch		; store pmode type

	mov	dx,offs @kernel_end - offs @kernel_beg
	mov	di,offs @kernel_beg

@pm32_info_exit:	pop	es	; restore other registers
	pop	ds
	retf				; return

@pm32_info_fail:	stc				; carry set, failed
	jmp	@pm32_info_exit




;=============================================================================
@pm32_info_detect_DPMI:				; detect a DPMI host
	pop	bp

If DOS_LEGACY_125_20 ne 0
	cmp	dos_ver_major,3		; DPMI multiplex INT 2Fh requires DOS 3.0+
	jb	@pm32_info_d0
	push	es
	xor	ax,ax
	mov	es,ax
	mov	eax,es:[2Fh*4]
	pop	es
	test	eax,eax
	jz	@pm32_info_d0
Endif

	mov	ax,1687h		; check for DPMI
	int	2Fh
	test	ax,ax			; DPMI present?
	jnz	@pm32_info_d0			; if no, exit routine

	mov	ax,1			; error code in case no processor 386+
	cmp	cl,3			; is processor 386+? (redundant)
	jb	@pm32_info_fail			; if no, fail

	mov	al,3			; error code in case DPMI not 32bit
	test	bl,1			; is DPMI 32bit?
	jz	@pm32_info_fail			; if no, fail

	mov	wptr dpmiepmode[0],di	; store DPMI initial mode switch addx
	mov	wptr dpmiepmode[2],es
	mov	bx,si			; BX = number of paragraphs needed
	mov	ch,3			; pmode type is 3 (DPMI)
	jmp	@pm32_info_done			; go to done ok
@pm32_info_d0:	jmp	bp			; return to calling routine


;=============================================================================
@pm32_info_detect_VCPI:				; detect a VCPI server
	pop	bp

	xor	ax,ax			; get INT 67h vector
	mov	es,ax
	mov	ax,es:[67h*4]
	or	ax,es:[67h*4+2]		; is vector NULL
	jz	@pm32_info_d0			; if yes, no VCPI

	mov	ax,0DE00h		; call VCPI installation check
	int	67h
	test	ah,ah			; AH returned as 0?
	jnz	@pm32_info_d0			; if no, no VCPI

	mov	ax,0DE0Ah		; get PIC mappings
	int	67h
	mov	picmaster,bl
	mov	picslave,cl
	mov	ax,0004h		; error code 4 in case of exit
	cmp	bl,cl			; BL=CL, only one PIC available
	je	@pm32_info_fail
	cmp	bl,30h			; PICs mapped on system vectors?
	je	@pm32_info_fail
	cmp	cl,30h
	je	@pm32_info_fail
	cmp	cl,08h			; slave PIC shouldn't be mapped onto INT 08h
	je	@pm32_info_fail
	test	bl,bl			; should not be mapped on INT 00h
	je	@pm32_info_fail
	test	cl,cl
	je	@pm32_info_fail

	mov	edx,xms_data		; EDX=get free XMS memory
	mov	ecx,edx			; ECX=free XMS memory in KB
	jecxz	@pm32_info_v1			; skip if none

	test	pm32_mode,00001000b	; check if VCPI+XMS alloc scheme
	jz	@pm32_info_vA			; if not, jump (save some DOS memory)
	call	xms_allocmem		; alloc XMS memory in EDX
	mov	di,dx			; DI=handle
	dec	ax			; if AX=0001, no error occured
	jz	@pm32_info_v1
@pm32_info_vA:	xor	ecx,ecx			; if error, XMS memory in ECX = 0

@pm32_info_v1:	mov	ax,0DE03h
	int	67h			; EDX=free VCPI pages

	push	es
	push	ecx
	push	edx
	push	di
	test	pm32_mode,00000100b	; check if VCPI smart pagetable alloc
	jz	@pm32_info_v1a			; no, use standard detection

	mov	ah,48h			; allocate 4K block of memory
	mov	bx,0100h		;  for VCPI pagetable
	int	21h
	jc	@pm32_info_v1a			; INT 21h failed, use standard alloc

	mov	es,ax			; ES=returned segment
	xor	di,di			; DI=zero pointer
	sub	sp,8*3			; DS:SI=pointer to structure
	mov	si,sp
	push	ds
	push	ss
	pop	ds
	mov	ax,0DE01h		; get PM interface
	int	67h
	pop	ds
	add	sp,8*3			; discard structure on stack
	mov	ah,49h			; discard pagetable and free DOS mem
	int	21h
	mov	eax,1000h		; pagetable limit 4K
	sub	ax,di			; minus used part
	shr	ax,2			; convert to 4K pages
	jmp	@pm32_info_v1b

@pm32_info_v1a:	xor	eax,eax
@pm32_info_v1b:	pop	di
	pop	edx
	pop	ecx
	pop	es

	mov	esi,ecx			; ECX=XMS free mem,EDX=VCPI free pages
	shr	esi,2			; ESI=XMS_freemem/4 (to match 4Kpages)
	lea	esi,[edx+esi+3FFh]	; ESI=XMS_4Kmem+VCPI_4Kpages+4M_align
	sub	esi,eax			; minus free 0th pagetable space
	jnc	@pm32_info_v1c			; just in case...
	adc	esi,eax
@pm32_info_v1c:	shr	esi,10			; ESI=ESI/1024
	jecxz	@pm32_info_v2

	mov	dx,di
	mov	ah,0Ah			; free what was allocated
	call	dptr xms_call
@pm32_info_v2:	movzx	ax,pm32_maxpages
	cmp	ax,si
	jbe	@pm32_info_v3
	mov	ax,si

@pm32_info_v3:	test	ax,ax			; if no pages, check for DOS/32A
	jnz	@pm32_info_v5
	pushad
	mov	bp,sp
	mov	ax,0FF88h		; check if running under DOS/32A
	int	21h
	cmp	eax,'ID32'		; if present, use the already alloc'ed pagetables
	jnz	@pm32_info_v4
	mov	[bp+1Ch],si		; store pagetables in AX
@pm32_info_v4:	popad

@pm32_info_v5:	cmp	al,64			; limit the number of pagetables to
	jbe	@pm32_info_v6			; maximum 64, = 256MB
	mov	al,64
@pm32_info_v6:	mov	pagetables,al		; BX = VCPI page tables needed
	add	al,pm32_maxfpages	; + physical memory mappable pages
	shl	ax,8			; 100h paragraphs per page table
	add	ax,100h+100h+0FFh+7+80h	; +page_dir+0th_page+align_buf+TSS+IDT
	mov	bx,ax
	mov	ch,2			; pmode type is 2 (VCPI)
	jmp	@pm32_info_vcpi			; go to figure other memory needed
@pm32_info_v0:	jmp	bp			; return to calling routine





;=============================================================================
cpu_detect:				; detect: 286, 386, 486, 586 etc
	cli
	mov	cl,2			; CL: cputype=2 (80286)
	pushf
	pop	ax
	or	ax,0F000h
	push	ax
	popf
	pushf
	pop	ax
	and	ax,0F000h
	jne	@cpu_detect_1
	jmp	@cpu_detect_x1
@cpu_detect_1:	inc	cl			; CPU = 80386
	pushfd
	pop	eax
	mov	edx,eax
	xor	eax,00040000h
	push	eax
	popfd
	pushfd
	pop	eax
	xor	eax,edx
	jne	@cpu_detect_2
	jmp	@cpu_detect_x1
@cpu_detect_2:	inc	cl			; CPU = 80486
	push	edx
	popfd
	pushfd
	pushfd
	pop	eax
	mov	edx,eax
	xor	eax,00200000h
	push	eax
	popfd
	pushfd
	pop	eax
	xor	eax,edx
	jne	@cpu_detect_3
	jmp	@cpu_detect_x2
@cpu_detect_3:	xor	eax,eax
	db	0Fh, 0A2h		; CPUID
	mov	cpuidlvl,eax		; set CPUID level
	mov	eax,1
	db	0Fh, 0A2h		; CPUID
	and	ah,0Fh
	mov	cl,ah
@cpu_detect_x2:	popfd
	xor	eax,eax
	xor	edx,edx
@cpu_detect_x1:	mov	al,cl
	mov	cputype,al		; store processor type
	sti
	ret

fpu_detect:				; detect 8087, 287, 387, 487 etc
	pushd	0
	mov	bp,sp
	fninit
	fnstcw	wptr [bp+2]
	mov	ax,wptr [bp+2]
	cmp	ah,03h
	jnz	@fpu_detect_done			; done: no FPU present
	mov	wptr [bp],1
	and	wptr [bp+2],0FF7Fh
	wait
	fldcw	wptr [bp+2]
	fdisi
	fstcw	wptr [bp+2]
	wait
	test	wptr [bp+2],0080h
	jnz	@fpu_detect_done			; done: 8087 is present
	mov	wptr [bp],2
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
	jz	@fpu_detect_done			; done: 80287 is present
	mov	wptr [bp],3
	mov	al,cputype
	cmp	al,4
	jb	@fpu_detect_done			; done: 80387 is present
	mov	bptr [bp],al		; done: 80487+ (build-in)
@fpu_detect_done:	pop	eax
	and	eax,7
	jz	@fpu_detect_exit
	mov	cx,8
@fpu_detect_loop:	fldz				; set ST(0) to ST(7) to +ZERO
	loop	@fpu_detect_loop
	finit				; reinitialize FPU
@fpu_detect_exit:	mov	fputype,al
	ret

	Align 4
@area1_end label byte

