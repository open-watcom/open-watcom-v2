; RING0_32.ASM
; Demonstration of how to switch to ring 0 with CauseWay for those who
;  may find it useful, 32-bit non-flat version.
; Remember that no application can run under ring 0 under DPMI, even with
;  another DOS extender that might normally run applications at ring 0
;  for whatever strange reason it thinks it appropriate.
;
; Type: WL32 ring0_32 to create RING0_32.EXE
;
.386P				; 32-bit code segment
;.MODEL FLAT
.MODEL SMALL
.STACK 400h

; CauseWay specific equates
GDTData			EQU	0e0h+3
KernalPL3_2_PL0	EQU 48H+3	; PL3 to PL0 call gate.
KernalZero		EQU	70h+3	; Kernal 0-4G data reference.

.DATA
HiText	DB	'Hello from 32-bit Ring 3 (if still in Ring 0 this DOS call would crash).',13,10
NoZero	DB	'Cannot go to Ring 0.',13,10

.CODE
;	DB	64*1024	DUP (?)		; just to show use with nonzero high word 32-bit offsets
start:
	mov	ax,0ff00h	; Info
	int	31h
	jc	neverdid
	test	edi,8	; see if running under DPMI
	jne	neverdid	; yes, can't switch to ring 0

; setup for ring 3 to ring 0 switch
	mov	bx,cs
	mov	ax,0ff06h	; AliasSel
	int	31h
	jc	neverdid
	mov	bx,ax
	mov	cl,10011011b	; access rights
	mov	ch,01000000b	; extended access rights bits, 32-bit default, byte granular
	mov	ax,9		; Set Descriptor Access Rights
	int	31h
	jc	neverdid
	push	bx		; save aliased PL0 CS

	mov	bx,GDTData
	mov	ax,0ff08h	; GetSelDet32
	int	31h
	jc	neverdid

	cli				; no interrupt processing allowed under ring 0 or setup

; edx holds linear base of GDT
	add	edx,KernalPL3_2_PL0	; add in ring 3 to 0 call gate address
	and	edx,NOT 7	; zap offset bytes
	mov	esi,edx		; save GDT offset to call gate
	mov	ax,KernalZero
	mov	es,ax
	mov	edi,es:[edx]	; save original call gate CS:EIP
	mov	bx,es:[edx+6]	; save high word of EIP

	mov	ecx,OFFSET In0
	mov	es:[edx],cx
	shr	ecx,16
	mov	WORD PTR es:[edx+6],cx
	pop	WORD PTR es:[edx+2]	; set aliased PL0 CS

	mov	edx,esp		; save SS:ESP in [E]CX:EDX
	mov	cx,ss
	movzx	ecx,cx
	mov	bp,cs		; save non-aliased PL3 CS

; do the ring 0 to ring 3 switch
	db	9ah			; absolute 32-bit call to clear pre-fetch and load CS
	dd	OFFSET In0	; In0 is a dummy place-holder here
	dw	KernalPL3_2_PL0

; do ring 0-only stuff to show that it works;
;  these instructions would all fail under ring 3.
;  Just do benign reads so things remain stable.
; CauseWay emulates MOV EAX,CR0; MOV CR0,EAX; MOV EAX,CR3; MOV CR3,EAX
;  instructions in the normal ring 3 GPF-handler, so don't use those.
In0:
	mov	eax,DR0		; just for show
	mov	eax,DR6
	mov	eax,CR2

; do the ring 0 to ring 3 switch
	mov	es:[esi],edi	; restore original CS:EIP in call gate
	mov	es:[esi+6],bx
	push	ecx		; SS
	push	edx		; ESP
	pushfd
	pop	eax			; EFlags in eax
	and	ax,1000111111111111b	; clear NT and IOPL flag bits
	or	ax,0011000000000000b	; force IOPL to 3
	push	eax
	popfd
	push	eax		; flags
	xor	eax,eax
	mov	ax,bp
	push	eax		; CS, PL3
	mov	eax,OFFSET In3
	push	eax		; EIP
	iretd			; return control at ring 3 to In3

In3:
	sti				; re-enable interrupts

	mov	ax,_DATA
	mov	ds,ax
	mov	edx,OFFSET HiText
	mov	cx,SIZEOF HiText
	mov	bx,1
	mov	ah,40h
	int	21h

	mov	ax,4c00h
	int	21h

neverdid:
	mov	ax,_DATA
	mov	ds,ax
	mov	edx,OFFSET NoZero
	mov	cx,SIZEOF NoZero
	mov	bx,1
	mov	ah,40h
	int	21h
	mov	ax,4c01h
	int	21h

END	start
