	.model small

	.code

start:	push	cs
	pop	ds
	mov	dx,offset Message
	mov	ah,9
	int	21h
	mov	ax,4cffh
	int	21h

	.data

Message	db "this is a CauseWay dynamic link library",13,10,"$"

	end	start
 
