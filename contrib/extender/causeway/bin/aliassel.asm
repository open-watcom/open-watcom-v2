;
;A demo of using AliasSel to access CODE seg variables.
;

	.386			;This order of .386 and .model
	.model small		;ensures use32 segments.
	.stack 1024		;should be enough for most.

	include cw.inc		;define CauseWay API

	.code

;-------------------------------------------------------------------------------
Start	proc	near
	mov	ax,DGROUP		;Make our data addressable.
	mov	ds,ax
;
;Get old INT 21h handler address.
;
	mov	bl,21h
	sys	GetVect
;
;We need an alias of _TEXT to be able to store the old handler address in a
;variable that will be accessible to the new interrupt handler so that it can
;chain to the old handler for functions it is not interested in.
;
	mov	bx,cs		;Selector to Alias.
	sys	AliasSel		;Allocate an Alias selector.
	jc	alias_error
	push	ds
	mov	ds,ax
	assume ds:_TEXT
	mov	dword ptr [OldInt21h],edx	;Store the old handler address
	mov	word ptr [OldInt21h+4],cx	;so we can chain to it.
	assume ds:DGROUP
	pop	ds
;
;We can release the Alias selector now we're done with it but it could be
;stored in a variable for future access to _TEXT.
;
	mov	bx,ax
	sys	RelSel
	jc	alias_error
;
;Now install the new INT 21h handler.
;
	mov	bl,21h
	mov	cx,cs
	mov	edx,offset INT21hHandler
	sys	SetVect
;
;Make the new handler work for a living.
;
	mov	ah,255
	mov	edx,offset HelloWorld
	int	21h
;
;Restore the old INT 21h handler.
;
	mov	edx,cs:dword ptr [OldInt21h]
	mov	cx,cs:word ptr [OldInt21h+4]
	mov	bl,21h
	sys	SetVect
;
;Time to exit.
;
	jmp	exit
;
;There wasn't a free selector available for the AliasSel function (very
;unlikely) so print an error message.
;
alias_error:	mov	edx,offset AliasErrorText
	mov	ah,9
	int	21h
;
;Back to the real world.
;
exit:	mov	ax,4c00h
	int	21h
Start	endp

;-------------------------------------------------------------------------------
;
;The replacement INT 21h handler. Doesn't do anything useful in itself but helps
;to demonstrate use of AliasSel.
;
INT21hHandler	proc	far
	cmp	ah,255		;Our dummy function?
	jnz	to_old_int21h
;
;This is only a test of chaining so change to the normal function number and
;do an INT 21h again. Next time through here the handler will chain to the old
;handler and then come back to out IRET.
;
	mov	ah,9
	int	21h
	iretd
;
;Chain to the old INT 21h handler via OldInt21h
;
to_old_int21h:	jmp	cs:fword ptr [OldInt21h]
;
;Storage for the old INT 21h handler address.
;
OldInt21h	df 0
INT21hHandler	endp

	.data
;-------------------------------------------------------------------------------
HelloWorld	db "Hello world",13,10,"$"
AliasErrorText	db "Not enough selectors to allocate Alias selector.",13,10,"$"

	end	Start
