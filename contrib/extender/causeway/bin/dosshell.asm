;
;Example of shell to DOS.
;
	.model small
	.386

STACK	Segment para stack use16 'STACK'
	 DB	400h DUP (?)
ENDS

	include cw.inc
b	equ	byte ptr
w	equ	word ptr

	.data
aname	db 'c:\command.com',0	; change 'c' to drive containing COMMAND.COM
ptab	dw 0
	dw ctab
	dw dgroup
	dw 0
	dw dgroup
	dw 0
	dw dgroup
ctab	db 0,13

sometext	db 'hello world',13,10,'$'

	.code
start:	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax

	mov	dx,offset sometext
	mov	ah,9
	int	21h

	mov	dx,offset aname
	mov	bx,offset ptab
	mov	ax,4b00h
	int	21h

	mov	ax,4c00h
	int	21h

	end	start
