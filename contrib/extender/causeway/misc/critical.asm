; demo critical error handler by John Wildsmith
	.386
	.model small
	.stack 1024

	include cw.inc

b	equ	byte ptr
w	equ	word ptr
d	equ	dword ptr


;
;If you set this EQU to nonzero then a short handler is used that simply
;returns a fail code. The longer version prompts the user for what to do.
;
SHORTONE	equ	0


	.code


Start	proc	near
	mov	ax,DGROUP		;Make data addressable.
	mov	ds,ax
	;
	mov	bl,24h
	sys	GetVect		;Get current setting in case it's
	mov	d[OldInt24],edx	;needed for chaining.
	mov	w[OldInt24+2],cx
	mov	edx,offset CriticalHandler
	mov	cx,cs
	sys	SetVect		;Install new handler.
	;
	mov	edx,offset DummyName	;Try opening a file on A: to
	mov	ax,3d02h		;trigger a critical error.
	int	21h
	jnc	NoError
	;
	mov	edx,offset FailedMessage	;Print a message so we know it
	mov	ah,9		;failed.
	int	21h
	;
NoError:	mov	ax,4c00h
	int	21h
Start	endp


	if	SHORTONE
;-------------------------------------------------------------------------
;
;Critical error handler, returns FAIL code so just checking for carry should
;take care of things.
;
CriticalHandler proc far
	mov	ax,3
	iretd
CriticalHandler endp


	else


;-------------------------------------------------------------------------
;
;This version prompts the user to see what they want to do.
;
CriticalHandler proc far
	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi
	push	ebp
	push	ds
	push	es
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	;
@@0:	mov	edx,offset CriticalPrompt
	mov	ah,9		;Display the prompt.
	int	21h
	mov	ah,1
	int	21h		;Get key press.
	mov	edi,offset CriticalKeys
	mov	ecx,8
	cld
	repne	scasb		;Scan for valid response.
	jnz	@@0
	movzx	eax,b[edi+7]
	cmp	eax,2
	jz	@@Terminate		;Terminate the program.
	;
	pop	es		;Otherwise return the code to
	pop	ds		;DOS.
	pop	ebp
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	iretd
	;
@@Terminate:	mov	ax,4cffh
	int	21h
CriticalHandler endp
	endif


	.data
;
FailedMessage	db 13,10,"Critical error handled without problems if you see this.",13,10,"$"
DummyName	db "a:test.bin",0
OldInt24	df ?
CriticalPrompt	db 13,10,'Critical Error: Abort, Retry, Ignore, Fail? $'
CriticalKeys	db 'aArRiIfF'
CriticalCodes	db 2,2,1,1,0,0,3,3
;
	end	Start


