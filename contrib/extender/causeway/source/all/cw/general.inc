;
;
;Some things to bear in mind when in protected mode.
;
;
;Loading a segment register with an invalid selector causes an exception. This includes
;pop'ing it off the stack. eg, ES points to a memory block used at some previous point in
;the program, you then release the memory, perhaps to load more sprites etc. ES now contains
;an invalid segment selector value, the next time its pop'd an exception will occur. The best
;solution to this problem seems to be to start with ALL segment regs pointing to something like
;your data seg which you know isn't going to become invalid, then procede on the assumption that
;any segment register loads require the old value to be preserved, just as you would normaly do
;when modifying DS.
;
;
;386SHELL.EXE assumes any program files it loads use a single 32 bit segment (USE32). A command
;line switch maybe added to 386PREP to allow for 16 bit segments.
;
;
;In a Use32 segment, the default oprand size is 32 bits. This includes LOOP,REP etc (ECX is used).
;String instructions, eg, lodsb/stosb use ESI/EDI. Using a DB 66h before the instuction will
;revert to the 16 bit usage. This overide sets the operand size for ALL registers involved in an
;instruction, eg, db 66h, rep movsb would use CX,SI,DI.
;
;
;"Only" 16384 different segment selectors in the LDT. Keep allocation of small chunks of memory
;to a minimum. Memory allocation requests may be refused by 386shell due to insuficient selector
;table space. No use is made of the GDT to preserve protection model.
;


b	equ	byte ptr
w	equ	word ptr
d	equ	dword ptr
f	equ	fword ptr

Pushm	MACRO	R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16
	IRP	X,<R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16> 	;REPEAT FOR EACH PARM
	IFNB	<X>			;IF THIS PARM NOT BLANK
	PUSH	X			;SAVE THE REGISTER
	ENDIF				;END IFNB
	ENDM				;END IRP
	ENDM

Popm	macro	R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16
	IRP	X,<R16,R15,R14,R13,R12,R11,R10,R9,R8,R7,R6,R5,R4,R3,R2,R1> ;REPEAT FOR EACH PARM
	IFNB	<X>				;IF THIS PARM NOT BLANK
	POP	X				;POP THE REGISTER
	ENDIF					;END IFNB
	ENDM					;END IRP
	ENDM

Rep_movs	macro
	push	ecx
	shr	ecx,1
	push	ecx
	shr	ecx,1
	rep	movsd
	pop	ecx
	and	ecx,1
	rep	movsw
	pop	ecx
	and	ecx,1
	rep	movsb
	endm

Rep_stos	macro
	push	ecx
	shr	ecx,1
	push	ecx
	shr	ecx,1
	rep	stosd
	pop	ecx
	and	ecx,1
	rep	stosw
	pop	ecx
	and	ecx,1
	rep	stosb
	endm


GetMemory	macro p1,p2,p3
	mov	ecx,p1
	mov	dx,cx
	shr	ecx,16
	mov	ah,48h
	sys	GetMem
	jc	p3
	mov	p2,bx
	mov	esi,MemoryListPos
	mov	[esi],bx
	add	MemoryListPos,2
	endm

LoseMemory	macro
	local __reloop1
	local __reloop2
	pushm	ds,ds,ds
	popm	es,fs,gs
	lea	esi,MemoryList
__reloop1:	mov	bx,[esi]
	cmp	bx,-1
	jz	__reloop2
	push	esi
	sys	RelMem
	pop	esi
	add	esi,2
	jmp	__reloop1
__reloop2:	nop
	endm


DDS	struc		;Structure to get at Low/high words.
lw	dw ?
hw	dw ?
DDS	ends


DDF	struc		;Structure to get at offset/segment in F word.
of	dd ?
sg	dw ?
DDF	ends


;
;Some new instructions to take note of:-
;
;BT	test bit. eg BT Flags,1  bit is reflected in carry flag.
;
;BTC	bit test & complement.
;
;BTR	bit test & reset.
;
;BTS	bit test & set.
;
;CWDE	word to dword in EAX.
;
;CDQ	dword to qword.
;
;ENTER	make stack frame. ENTER bytes,?. subs bytes from sp.
;
;IRETD	32-bit iret.
;
;JECXZ	jump ECX zero.
;
;LAR	load access rights. LAR EAX,DS look in the book for bit details.
;
;LEAVE	cleanup the stack.
;
;LGDT	load global descriptor table reg.
;
;LIDT	load interupt descriptor table register.
;
;LLDT	load local descriptor table. (effectively segment selector value for GDT)
;
;LMSW	load machine status word.
;
;SMSW	store machine status word.
;
;LSL	load segment limit. LSL EAX,Segment (adjusts for G bit)
;
;LTR	load task register.
;
;MOVSX	mov with sign extend. eg, MOVSX EAX,AX (same as CWDE AX)
;
;MOVZX	mov with zero extend.
;
;PUSHAD	push 8 general regs 32 bit.
;
;POPAD	pop 8 general regs 32 bit.
;
;SETcc	set byte on condition.
;
;SGDT	store global descriptor table.
;
;SIDT	store interupt descriptor table.
;
;SHLD	double precision shift left. SHLD reg1,reg2,count. :- shl reg2,count rcl reg1,count etc
;
;SHRD	as above only not.
;
;SLDT	store local descriptor table register.
;
;STR	store task register.
;
;VERR	verify segment for reading.
;
;VERW	verify segment for writing.
;


