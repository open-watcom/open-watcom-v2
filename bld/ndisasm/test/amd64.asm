.data

m8	db	0
m16	dw	0
m32	dd	0
m48	df	0
m64	dq	0

.code

	adc	al,2
	adc	al,-7
	adc	ax,3
	adc	eax,0ffffh
	adc	rax,-27

	adc	ah,1
	adc	bl,1
	adc	bh,1
	adc	bx,1
	adc	ebx,1
	adc	rbx,1
	adc	cl,1
	adc	ch,1
	adc	cx,1
	adc	ecx,1
	adc	rcx,1
	adc	dl,1
	adc	dh,1
	adc	dx,1
	adc	edx,1
	adc	rdx,1
	adc	spl,1
	adc	sp,1
	adc	esp,1
	adc	rsp,1
	adc	bpl,1
	adc	bp,1
	adc	ebp,1
	adc	rbp,1
	adc	sil,1
	adc	si,1
	adc	esi,1
	adc	rsi,1
	adc	dil,1
	adc	di,1
	adc	edi,1
	adc	rdi,1
	adc	r8b,1
	adc	r8w,1
	adc	r8d,1
	adc	r8,1
	adc	r9b,1
	adc	r9w,1
	adc	r9d,1
	adc	r9,1
	adc	r10b,1
	adc	r10w,1
	adc	r10d,1
	adc	r10,1
	adc	r11b,1
	adc	r11w,1
	adc	r11d,1
	adc	r11,1
	adc	r12b,1
	adc	r12w,1
	adc	r12d,1
	adc	r12,1
	adc	r13b,1
	adc	r13w,1
	adc	r13d,1
	adc	r13,1
	adc	r14b,1
	adc	r14w,1
	adc	r14d,1
	adc	r14,1
	adc	r15b,1
	adc	r15w,1
	adc	r15d,1
	adc	r15,1

	adc	byte ptr m8,1
	adc	word ptr m16,1
	adc	dword ptr m32,1
	adc	qword ptr m64,1

	adc	bl,ch
	adc	r8w,dx
	adc	r11,rsp

	adc	byte ptr m8,al
	adc	word ptr m16,r12w
	adc	dword ptr m32,ebp
	adc	qword ptr m64,r15

	adc	dh,byte ptr m8
	adc	cx,word ptr m16
	adc	r13d,dword ptr m32
	adc	rsp,qword ptr m64

	add	bl,1
	add	ebx,12345h
	add	r13,-3
	add	byte ptr m8,cl
	add	qword ptr m64,rdx
	add	bl,byte ptr m8
	add	rdx,rbp
	add	al,1
	add	rax,12345h

	and	ch,1
	and	edi,12345h
	and	r10,-3
	and	byte ptr m8,ah
	and	qword ptr m64,rdi
	and	cl,byte ptr m8
	and	r10,rcx
	and	al,1
	and	rax,12345h

	bsf	edx,ebp
	bsf	cx,sp
	bsf	rsi,qword ptr m64

	bsr	edi,ecx
	bsr	si,di
	bsr	r9,qword ptr m64

	bswap	ecx
	bswap	rdi

	bt	m16,di
	bt	rax,7

	btc	r8w,r9w
	btc	r10d,5

	btr	m32,edi
	btr	ebx,6

	bts	m64,r12
	bts	r11,4


	call	tgt1
	call	qword ptr m64
	call	rbx
	call	fword ptr m48
tgt1:

	cbw
	cwde
	cdqe

	cwd
	cdq
	cqo

	clc

	cld

;	clflush	m8

	cmc

	cmovo	edx,eax
	cmovno	r10,qword ptr m64
	cmovb	r8w,di
	cmovae	bx,word ptr m16
	cmove	rdx,rbp
	cmovne	r15,r13
	cmovbe	ebp,dword ptr m32
	cmova	r9w,sp
	cmovs	rcx,r14
	cmovns	dx,si
	cmovp	r9d,esi
	cmovpo	edi,r8d
	cmovl	sp,r8w
	cmovge	rbx,qword ptr m64
	cmovle	ecx,esp
	cmovg	r9d,dword ptr m32

	cmp	m8,dl
	cmp	r9d,esp
	cmp	dh,al
	cmp	rdx,r13
	cmp	al,-1
	cmp	eax,12345h
	cmp	dh,3
	cmp	rcx,12345h
	cmp	rsp,0

	cmpsb
	cmpsw
	cmpsd
	cmpsq

	cmpxchg	bl,ch
	cmpxchg	esp,r9d

	cmpxchg8b qword ptr m64

	cpuid

	dec	byte ptr m8
	dec	word ptr m16
	dec	rbp

	div	dh
	div	ecx

	enter	4,0
	enter	1234h,56h

	idiv	ah
	idiv	m64

	imul	dl
	imul	r10w
	imul	r13,qword ptr m64
	imul	edx,6
	imul	dx,word ptr m16,1234h
	imul	rdx,rax,12345h

	in	al,42h
	in	ax,80h
	in	eax,63h
	in	al,dx
	in	ax,dx
	in	eax,dx

	inc	byte ptr m8
	inc	edi

	ins	byte ptr [rdi],dx
	ins	word ptr [rdi],dx
	ins	dword ptr [rdi],dx
	ins	dword ptr [edi],dx

	int	21h
	int	3

	jo	tgt1
	jo	tgt2
	jno	tgt1
	jno	tgt2
	jb	tgt1
	jb	tgt2
	jae	tgt1
	jae	tgt2
	je	tgt1
	je	tgt2
	jne	tgt1
	jne	tgt2
	jbe	tgt1
	jbe	tgt2
	ja	tgt1
	ja	tgt2
	js	tgt1
	js	tgt2
	jns	tgt1
	jns	tgt2
	jp	tgt1
	jp	tgt2
	jpo	tgt1
	jpo	tgt2
	jl	tgt1
	jl	tgt2
	jge	tgt1
	jge	tgt2
	jle	tgt1
	jle	tgt2
	jg	tgt1
	jg	tgt2

tgt2:

	jecxz	tgt2
	jrcxz	tgt2

	jmp	tgt2
	jmp	tgt1
	jmp	word ptr m16
	jmp	r9w
	jmp	qword ptr m64
	jmp	rsi
	jmp	fword ptr m48

	lahf

	lfs	di,dword ptr m32
	lfs	edx,fword ptr m48
	lgs	ax,dword ptr m32
	lgs	r11d,fword ptr m48
	lss	sp,dword ptr m32
	lss	ebp,fword ptr m48

	lea	di,dword ptr m32
	lea	rdx,byte ptr m8

	leave

	lods	byte ptr [rsi]
	lods	word ptr [rsi]
	lods	dword ptr [rsi]
	lods	qword ptr [rsi]
	lods	word ptr [esi]

tgt3:
	loopnz	tgt3
	loopz	tgt3
	loop	tgt3
	loopnzd	tgt3
	loopzd	tgt3
	loopd	tgt3

	mov	byte ptr m8,bl
	mov	word ptr m16,r8w
	mov	dh,ah
	mov	rdi,rax
	mov	r12,ds
	mov	bx,es
	mov	fs,dx
	mov	gs,ebx
	mov	ss,rcx
	mov	al,byte ptr m8
	mov	ax,word ptr m16
	mov	eax,dword ptr m32
	mov	rax,qword ptr m64
	mov	byte ptr m8,al
	mov	word ptr m16,ax
	mov	dword ptr m32,eax
	mov	qword ptr m64,rax
	mov	dh,1
	mov	bp,2
	mov	r9d,3
	mov	r13,4
	mov	byte ptr m8,5
	mov	qword ptr m64,6
	mov	rsp,123456789h

	movs	byte ptr [rdi],byte ptr[rsi]
	movs	qword ptr [edi],qword ptr[esi]

	movsx	dx,al
	movsx	r9d,byte ptr m8
	movsx	ebx,dl
	movsx	ecx,r8w
	movsx	eax,ax
	movsx	r15,word ptr m16

	movsxd	r11,edx
	movsxd	rcx,dword ptr m32

	movzx	ecx,r9b
	movzx	r10,r10w

	mul	r9b
	mul	ebp
	mul	qword ptr m64

	neg	bh
	neg	r14

	nop

	not	byte ptr m8
	not	dword ptr m32

	or	byte ptr m8,r9b
	or	dword ptr m32,r9d
	or	r8b,byte ptr m8
	or	edx,edx
	or	al,1
	or	ax,1234h
	or	eax,12345h
	or	rax,12345h
	or	ah,5
	or	rdx,12345h
	or	r15,6

	out	16h,al
	out	60h,ax
	out	0a0h,eax
	out	dx,al
	out	dx,ax
	out	dx,eax

	outs	dx,byte ptr [esi]
	outs	dx,word ptr [esi]
	outs	dx,byte ptr [rsi]
	outs	dx,dword ptr [rsi]

	pop	fs
	pop	gs
	pop	dx
	pop	rbp
	pop	qword ptr m64
	pop	word ptr m16

	popf
	popfq

	push	fs
	push	gs
	push	dx
	push	qword ptr m64
	push	r8w
	push	rbp
	push	123h
	push	0
	pushw	0
	
	pushf
	pushfq

	rcl	ah,2
	rcl	dx,2
	rcl	cl,1
	rcl	r9w,1
	rcl	dword ptr m32,1
	rcl	bl,cl
	rcl	word ptr m16,cl
	rcl	r15,cl	

	rcr	al,5
	rcr	bp,5
	rcr	dh,1
	rcr	ebx,1
	rcr	byte ptr m8,cl
	rcr	qword ptr m64,cl

	ret
	ret	4

	retf
	retf	12

	rol	al,1
	rol	word ptr m16,3
	rol	rax,cl

	ror	r8b,1
	ror	byte ptr m8,4
	ror	edx,cl

	sahf

	sar	ch,1
	sar	ch,2
	sar	ch,cl

	sbb	al,2
	sbb	ax,123h
	sbb	eax,12345h
	sbb	byte ptr m8,7
	sbb	edx,12345h
	sbb	rdx,1
	sbb	byte ptr m8,dl
	sbb	qword ptr m64,rdi
	sbb	r9b,byte ptr m8
	sbb	r8d,eax

	scas	byte ptr [rdi]
	scas	dword ptr [edi]

	seto	ah
	setno	byte ptr m8
	setb	al
	setae	r9b
	sete	bl
	setne	sil
	setbe	dil
	seta	bpl
	sets	spl
	setns	bh
	setp	cl
	setpo	ch
	setl	dl
	setge	dh
	setle	r8b
	setg	r10b

	stos	byte ptr [edi]
	stos	qword ptr [rdi]

	shld	word ptr m16,dx,1
	shld	rdx,r12,cl

	shr	byte ptr m8,1
	shr	dx,2
	shr	rdx,cl

	shrd	dx,ax,3
	shrd	r8d,ebp,4
	shrd	r14,rax,5
	shrd	word ptr m16,r8w,cl
	shrd	dword ptr m32,ebp,cl
	shrd	qword ptr m64,rcx,cl

	stc

	std

	stos	byte ptr [rdi]
	stos	word ptr [edi]

	sub	byte ptr m8,spl
	sub	word ptr m16,dx
	sub	bpl,byte ptr m8
	sub	bx,r8w
	sub	edx,edx
	sub	al,3
	sub	eax,123h
	sub	ah,5
	sub	word ptr m16,123h
	sub	r13,6

	test	al,1
	test	r10,2
	test	byte ptr m8,3
	test	bp,456h
	test	dl,dh
	test	dword ptr m32,r15d

	xadd	byte ptr m8,dil
	xadd	rbx,r12

	xlatb

	xor	byte ptr m8,dh
	xor	qword ptr m64,rdx
	xor	spl,byte ptr m8
	xor	rbx,rcx
	xor	al,8
	xor	eax,12345h
	xor	spl,3
	xor	r8w,123h
	xor	dword ptr m32,2

	cli

	clts

	hlt

	invd

	invlpg	byte ptr m8

	iret
	iretd
	iretq

	lar	dx,word ptr m16
	lar	eax,esp
	lar	r10,rax

	lgdt	fword ptr m48

	lidt	fword ptr m48

	lldt	word ptr m16
	lldt	bp

	lmsw	r9w

	lsl	ebx,r9d

	ltr	dx

	mov	cr0,r9
	mov	cr8,rdx
	mov	rcx,cr4
	mov	r11,cr2

	mov	dr0,r15
	mov	rdx,dr7

	rdmsr

	rdpmc

	rdtsc

	rsm

	sgdt	fword ptr m48

	sidt	fword ptr m48

	sldt	dx
	sldt	ecx
	sldt	r13
	sldt	word ptr m16

	smsw	dx
	smsw	rcx
	smsw	ebx
	smsw	word ptr m16

	sti

	str	eax
	str	r12

	swapgs

	syscall

	sysret

	ud2

	verr	r9w
	verr	word ptr m16

	verw	bp

	wbinvd

	wrmsr

	end
