;
;A demo of using multiple 16/32-bit segments and near model
;This example will NOT work with FLAT model 
;

	.386			;This order of .386 and .model
	.model small		;ensures use32 segments.
	.stack 1024		;should be enough for most.

	include cw.inc		;define CauseWay API

;
;If you set both ProgramLocking and DynamicLocking to be nonzero, the effect
;will be to make sure only physical memory is used to run the program and any
;of its MALLOC allocations. Use these options for time critical applications.
;

;
;For programs that need their memory locking set ProgramLocking to be nonzero.
;That will make sure all of the program's memory is locked during the startup
;code. If the lock fails then it indicates not enough physical memory present.
;
ProgramLocking	equ	1


;
;For programs that need / want all of their dynamicly allocated memory to be
;locked to prevent paging delays, set DynamicLocking to be nonzero. If a
;memory block allocated cannot be locked it will be treated as if there were
;not enough logical memory available.
;
DynamicLocking	equ	1

	.code

;-------------------------------------------------------------------------------
start	proc	near
	mov	ax,DGROUP		;Make our data addressable
	mov	ds,ax		;ready for error messages.
	;
	if	ProgramLocking
;
;This code assumes CS (_TEXT) is the first segment and _STACK (SS) is the last.
;It works out the start and end addresses of the program and attempts to lock
;that region of memory.
;This approach does NOT work with FLAT model (use ESP instead).
;
	mov	bx,cs
	sys	GetSelDet32		;Get linear address of the start
	mov	esi,edx		;of the program.
	mov	bx,ss
	sys	GetSelDet32		;Get linear address of the stack.
	add	ecx,edx
	inc	ecx		;address+limit+1=end address.
	sub	ecx,esi		;calculate program length.
	sys	LockMem32
	jc	memory_error		;not enough physical memory.
	endif
;
;To be able to access any address as DGROUP we need to modify the DGROUP (DS)
;selector to give it a limit of 4G-1 (limit=length-1). This removes segment
;limit protection for stray memory access but in most cases these will be
;picked up by the paging protection mechanism due to physical memory not being
;mapped in.
;
	mov	bx,ds
	sys	GetSelDet32		;Get DGROUP details.
	jc	selector_error
	mov	ecx,-1		;Need limit of -1 for DS relative
	sys	SetSelDet32		;addressing of everything.
	jc	selector_error
;
;To complete the DGROUP relative addressing scheme we need to adjust ESP and
;load SS with DGROUP so that ESP/EBP/SS memory referances will be DGROUP
;relative as well.
;
	mov	esi,edx		;EDX is already DGROUP address.
	mov	bx,ss
	sys	GetSelDet32		;Get linear address of the stack.
	jc	selector_error
	sub	edx,esi		;calculate distance between
	mov	ax,ds		;SS and DGROUP.
	mov	ss,ax		;Do SS=DS and adjust ESP to make
	add	esp,edx		;the stack DS relative.
;
;Calculate DGROUP relative address of the PSP.
;
	mov	bx,es		;ES is the PSP selector.
	sys	GetSelDet32
	jc	selector_error
	mov	esi,edx
	call	Linear2DGROUP	;Conver linear to DGROUP
	mov	PSP_Pointer,esi
;
;Calculate DGROUP relative address of the environment variables.
;
	mov	bx,es:[2ch]
	sys	GetSelDet32
	jc	selector_error
	mov	esi,edx
	call	Linear2DGROUP
	mov	ENV_Pointer,esi
;
;Setup the other segment registers to finish things off.
;
	mov	ax,ds
	mov	es,ax
	mov	fs,ax
	mov	gs,ax

;
;From this point on (except hardware interrupt handlers) DS=ES=FS=GS=SS=DGROUP
;and memory allocated via MALLOC can be addressed without thinking about segment
;registers. Insert your code here.
;

	mov	esi,0b8000h		;Linear address of text screen.
	call	Linear2DGROUP
	mov	edi,esi		;simple demo of using DGROUP
	mov	ecx,(80*25)/2	;relative addressing to get
	mov	eax,24422442h	;at any memory address.
	rep	stosd

	jmp	exit

;
;Not enough memory for this program.
;
memory_error:	mov	edx,offset memory_message
	jmp	error_print

;
;A most unlikely error. The selectors that can cause the program to come here
;are supplied by the extender so there shouldn't be any problems.
;
selector_error: mov	edx,offset selector_message

;
;Print the message pointed to by EDX.
;
error_print:	mov	ah,9
	int	21h

;
;Go back to CauseWay, which will wind up back in DOS.
;
exit:	mov	ax,4c00h
	int	21h
start	endp


;-------------------------------------------------------------------------------
;
;Allocate some DGROUP relative memory.
;
;On Entry:
;
;ECX	- Bytes needed.
;
;On Exit:
;
;Carry set on error else,
;
;ESI	- DGROUP relative address of meemory.
;
malloc	proc	near
	push	eax
	push	ecx
	if	DynamicLocking
	add	ecx,4		;make space for length storage.
	endif
	sys	GetMemLinear32	;try to allocate the memory.
	jc	malloc_2
	if	DynamicLocking
	sys	LockMem32		;try to lock the memory.
	jnc	malloc_1
	sys	RelMemLinear32	;better release the memory first.
	stc
	jmp	malloc_2
	endif
malloc_1:	call	Linear2DGROUP	;make it DGROUP relative.
	if	DynamicLocking
	mov	[esi],ecx		;store the block length ready
	add	esi,4		;for FREE to unlock it again.
	endif
	clc			;make sure we return success.
malloc_2:	pop	ecx
	pop	eax
	ret
malloc	endp


;-------------------------------------------------------------------------------
;
;Release some memory allocated via MALLOC.
;
;On Entry:
;
;ESI	- DGROUP relative address of block to release.
;
free	proc	near
	push	eax
	push	esi
	if	DynamicLocking
	push	ecx
	sub	esi,4		;move back to the block size
	mov	ecx,[esi]		;and retrieve it.
	endif
	call	DGROUP2Linear	;convert address back to linear.
	if	DynamicLocking
	sys	UnLockMem32		;unlock the memory again.
	pop	ecx
	endif
	sys	RelMemLinear32	;release the memory.
	pop	esi
	pop	eax
	ret
free	endp


;-------------------------------------------------------------------------------
;
;Convert linear address to DGROUP relative address.
;
;On Entry:
;
;ESI	- Linear address.
;
;On Exit:
;
;ESI	- DGROUP relative address.
;
Linear2DGROUP	proc	near
	push	eax
	push	ebx
	push	ecx
	push	edx
	mov	bx,ds
	sys	GetSelDet32
	sub	esi,edx
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	ret
Linear2DGROUP	endp


;-------------------------------------------------------------------------------
;
;Convert DGROUP relative address to a linear address.
;
;On Entry:
;
;ESI	- DGROUP relative address.
;
;On Exit:
;
;ESI	- Linear address.
;
DGROUP2Linear	proc	near
	push	eax
	push	ebx
	push	ecx
	push	edx
	mov	bx,ds
	sys	GetSelDet32
	add	esi,edx
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	ret
DGROUP2Linear	endp

	.data

PSP_Pointer	dd ?
ENV_Pointer	dd ?

memory_message	db "Not enough memory.",13,10,"$"
selector_message db "Selector error, system must be corrupt.",13,10,"$"

	end start
