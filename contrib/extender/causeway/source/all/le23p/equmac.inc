;*******************************************************************************
;Lazy variable access equates.
;*******************************************************************************
b	equ	byte ptr
w	equ	word ptr
d	equ	dword ptr
f	equ	fword ptr


;*******************************************************************************
;Declare & initialise variables needed for CALLS
;*******************************************************************************
curlocsiz	= 0
curstakedsiz	= 0
curstkbse	= 0
paramdef	= 0
localsdef	= 0


;*******************************************************************************
;Similar to PROC but it resets the variables needed for ESP local variable and
;stack parameter addressing.
;
;Usage:   PROCS TestProc
;
; Does:   TestProc PROC   and some initialisation.
;
;*******************************************************************************
procs	macro name
curproc	textequ <name>
curproc	proc
curlocsiz	= 0
curstakedsiz	= 0
curstkbse	= 0
paramdef	= 0
localsdef	= 0
	endm


;*******************************************************************************
;Similar to ENDP but doesn't need a name. It closes a PROCS routine.
;*******************************************************************************
endps	macro
curproc	endp
	endm


;*******************************************************************************
;Defines stacked calling parameters for a PROCS routine. All parameters are
;assumed to be DWORDs. Parameters should be listed in C calling convention
;order.
;
;Usage:   PARAMS name1, name2
;
; Does:   Defines name1 & name2 as dword ptr[esp+??]  where ?? is replaced with
;         a value that tracks stack usage.
;
;*******************************************************************************
params	macro p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16
paramdef	= 1
curstakedsiz	= curlocsiz+4
	irp	x,<p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16>
	ifnb	<x>
curstksiz&x	textequ %(curstakedsiz)
x	textequ <d[curstksiz&x+curstkbse+esp]>
curstakedsiz	= curstakedsiz+4
	endif	 
	endm
	endm


;*******************************************************************************
;Defines stack based local variables for a PROCS routine. All parameters are
;assumed to be DWORDs.
;
;Usage:   LOCALS name1, name2
;
; Does:   Defines name1 & name2 as dword ptr[esp+??]  where ?? is replaced with
;         a value that tracks stack usage. A DWORD is reserved on the stack for
;         each variable.
;
;*******************************************************************************
locals	macro l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,l14,l15,l16,l17,l18,l19,l20
localsdef	= 1
	if	paramdef
	echo "LOCALS must come before PARAMS"
	push
	endif
	irp	x,<l20,l19,l18,l17,l16,l15,l14,l13,l12,l11,l10,l9,l8,l7,l6,l5,l4,l3,l2,l1>
	ifnb	<x>
curlocpos&x	textequ %(curlocsiz)
x	textequ <d[curlocpos&x+curstkbse+esp]>
curlocsiz	= curlocsiz+4
	endif	 
	endm
	if	curlocsiz
	sub	esp,curlocsiz
	endif
	endm


;*******************************************************************************
;Defines a single stack based local variable of specified size for a PROCS
;routine.
;
;Usage:   MLOCAL NAME, SIZE, LENGTH
;
; Does:   Defines NAME as SIZE ptr[esp+??]  where ?? is replaced with a value
;         that tracks stack usage. LENGTH bytes are reserved for the variable.
;
; Note:   SIZE should be BYTE,WORD,DWORD,FWORD (or abreviations b,w,d,f) etc.
;         LENGTH should be the number of bytes that need to be reserved for
;         NAME. Note that it's BYTES that are reserved regardless of SIZE so if
;         SIZE is WORD then LENGTH needs to be the number of words*2 etc.
;
;*******************************************************************************
mlocal	macro nm,sz1,sz2
	if	localsdef
	echo "MLOCAL must come before LOCALS"
	push
	endif
curlocpos&nm	textequ %(curlocsiz)
nm	textequ <sz1[curlocpos&nm+curstkbse+esp]>
curlocsiz	= curlocsiz+(sz2)
	endm


;*******************************************************************************
;Replacement for RET. Adjusts the stack to take acount of PARAMS,LOCALS &
;MLOCAL variables.
;*******************************************************************************
rets	macro
	if	curstkbse
	echo "PUSHS and POPS not balanced"
	push
	endif
	if	curlocsiz
	lea	esp,[esp+curlocsiz]
	endif
	ret
	nop
	endm


;*******************************************************************************
;Replacement for PUSHFD that maintains the stack offset for PARAMS,LOCALS &
;MLOCAL.
;*******************************************************************************
pushfds	macro
	pushfd
curstkbse	=	curstkbse+4
	endm


;*******************************************************************************
;Replacement for PUSHAD that maintains the stack offset for PARAMS,LOCALS &
;MLOCAL.
;*******************************************************************************
pushads	macro
	pushad
curstkbse	=	curstkbse+(4*8)
	endm


;*******************************************************************************
;Replacement for PUSH that maintains the stack offset for PARAMS,LOCALS &
;MLOCAL and allows multiple parameters.
;*******************************************************************************
pushs	macro regs:vararg
	irp	x,<regs>
	push	x
curstkbse	=	curstkbse+4
	endm
	endm


;*******************************************************************************
;Dummy for PUSHS. It is sometimes useful to be able to adjust the stack offsets
;for PARAMS,LOCALS & MLOCAL without actually PUSH'ing anything.
;*******************************************************************************
dpushs	macro regs:vararg
	irp	x,<regs>
curstkbse	=	curstkbse+4
	endm
	endm


;*******************************************************************************
;Dummy for PUSHADS. It is sometimes useful to be able to adjust the stack offsets
;for PARAMS,LOCALS & MLOCAL without actually PUSH'ing anything.
;*******************************************************************************
dpushads	macro
curstkbse	=	curstkbse+(4*8)
	endm


;*******************************************************************************
;Replacement for POPFD that maintains the stack offset for PARAMS,LOCALS &
;MLOCAL.
;*******************************************************************************
popfds	macro
	popfd
curstkbse	=	curstkbse-4
	endm


;*******************************************************************************
;Replacement for POPAD that maintains the stack offsets for PARAMS,LOCALS &
;MLOCAL.
;*******************************************************************************
popads	macro
	popad
curstkbse	=	curstkbse-(4*8)
	endm


;*******************************************************************************
;A replacement for POP that maintains the stack offset for PARAMS,LOCALS &
;MLOCAL and allows multiple parameters. POP's in reverse order.
;*******************************************************************************
pops	macro r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16
	irp	x,<r16,r15,r14,r13,r12,r11,r10,r9,r8,r7,r6,r5,r4,r3,r2,r1> ;REPEAT FOR EACH PARM
	ifnb	<x>
curstkbse	=	curstkbse-4
	pop	x
	endif	 
	endm
	endm


;*******************************************************************************
;Dummy for POPS. It is sometimes useful to be able to adjust the stack offsets
;for PARAMS,LOCALS & MLOCAL without actually POP'ing anything.
;*******************************************************************************
dpops	macro regs:vararg
	irp	x,<regs>
curstkbse	=	curstkbse-4
	endm
	endm


;*******************************************************************************
;Dummy for POPADS. It is sometimes useful to be able to adjust the stack offsets
;for PARAMS,LOCALS & MLOCAL without actually POP'ing anything.
;*******************************************************************************
dpopads	macro
curstkbse	=	curstkbse-(4*8)
	endm


;*******************************************************************************
;Call a routine with stacked parameters and clean the stack afterwards.
;*******************************************************************************
calls	macro name,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16
callsize	= 0
	irp	x,<p16,p15,p14,p13,p12,p11,p10,p9,p8,p7,p6,p5,p4,p3,p2,p1>
	ifnb	<x>
callsize	= callsize+4
	pushs	x
	endif
	endm
	call	name
	irp	x,<p16,p15,p14,p13,p12,p11,p10,p9,p8,p7,p6,p5,p4,p3,p2,p1>
	ifnb	<x>
	dpops	x
	endif
	endm
	if	callsize
	lea	esp,[esp+callsize]
	endif
	endm


;*******************************************************************************
;Call a C routine with stacked parameters and clean the stack afterwards. Also
;preserves all registers except EAX.
;*******************************************************************************
callc	macro name,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16
	pushs	ebx,ecx,edx,esi,edi,ebp
callsize	= 0
	irp	x,<p16,p15,p14,p13,p12,p11,p10,p9,p8,p7,p6,p5,p4,p3,p2,p1>
	ifnb	<x>
callsize	= callsize+4
	pushs	x
	endif
	endm
	call	name
	irp	x,<p16,p15,p14,p13,p12,p11,p10,p9,p8,p7,p6,p5,p4,p3,p2,p1>
	ifnb	<x>
	dpops	x
	endif
	endm
	if	callsize
	lea	esp,[esp+callsize]
	endif
	pops	ebx,ecx,edx,esi,edi,ebp
	endm


;*******************************************************************************
;Replacement for REP MOVSB. Does the operation with DWORD's where possible.
;*******************************************************************************
rep_movsb	macro
	local __0, __1, __2
	push	eax
;
;dword align source
;
	mov	eax,esi
	and	eax,3
	sub	eax,4
	neg	eax
	and	eax,3
	jz	__2
	cmp	ecx,eax
	jc	__2
	pushs	ecx
	mov	ecx,eax
	rep	movsb
	pops	ecx
	sub	ecx,eax
;
;do main copy.
;
__2:	push	ecx
	shr	ecx,2
	jz	__1
__0:	mov	eax,[esi]
	mov	[edi],eax
	add	esi,4
	add	edi,4
	dec	ecx
	jnz	__0
__1:	pop	ecx
	and	ecx,3
	rep	movsb
	pop	eax
	endm


;*******************************************************************************
;Replacement for REP MOVSW. Does the operation with DWORD's where possible.
;*******************************************************************************
rep_movsw	macro
	local __0, __1
	push	eax
	push	ecx
	shr	ecx,1
	jz	__1
__0:	mov	eax,[esi]
	mov	[edi],eax
	add	esi,4
	add	edi,4
	dec	ecx
	jnz	__0
__1:	pop	ecx
	and	ecx,1
	rep	movsw
	pop	eax
	endm


;*******************************************************************************
;Replacement for REP MOVSD. Allows us to change method easily.
;*******************************************************************************
rep_movsd	macro
	local __0, __1
	or	ecx,ecx
	jz	__1
__0:	mov	eax,[esi]
	mov	[edi],eax
	add	esi,4
	add	edi,4
	dec	ecx
	jnz	__0
__1:	;
	endm


;*******************************************************************************
;Replacement for REP STOSB. Does the operation with DWORD's where possible.
;*******************************************************************************
rep_stosb	macro
	local __0, __1
	push	eax
	mov	ah,al
	push	ax
	shl	eax,16
	pop	ax
	push	ecx
	shr	ecx,2
	jz	__1
__0:	mov	[edi],eax
	add	edi,4
	dec	ecx
	jnz	__0
__1:	pop	ecx
	and	ecx,3
	rep	stosb
	pop	eax
	endm


;*******************************************************************************
;Replacement for REP STOSW. Does the operation with DWORD's where possible.
;*******************************************************************************
rep_stosw	macro
	local __0, __1
	push	eax
	push	ax
	shl	eax,16
	pop	ax
	push	ecx
	shr	ecx,1
	jz	__1
__0:	mov	[edi],eax
	add	edi,4
	dec	ecx
	jnz	__0
__1:	pop	ecx
	and	ecx,1
	rep	stosw
	pop	eax
	endm


;*******************************************************************************
;Replacement for REP STOSD. Allows us to change method easily.
;*******************************************************************************
rep_stosd	macro
	local __0, __1
	or	ecx,ecx
	jz	__1
__0:	mov	[edi],eax
	add	edi,4
	dec	ecx
	jnz	__0
__1:	;
	endm


;*******************************************************************************
;Convert character in AL to upper case.
;*******************************************************************************
UpperChar	macro
	local __0
	cmp	al,61h		; 'a'
	jb	__0
	cmp	al,7Ah		; 'z'
	ja	__0
	and	al,5Fh		;convert to upper case.
__0:	;
	endm


;*******************************************************************************
;Add a memory region to the auto-lock list.
;*******************************************************************************
autolock	macro p1,p2
	extrn __autolock:near
	extrn __autounlock:near
_AUTOLOCKB	segment dword public 'DATA'
_AUTOLOCKB	ends
_AUTOLOCK	segment dword public 'DATA'
	dd p1,p2
_AUTOLOCK	ends
_AUTOLOCKE	segment dword public 'DATA'
_AUTOLOCKE	ends
	endm


