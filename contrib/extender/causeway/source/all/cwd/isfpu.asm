.386
.model flat
.stack 400h

.data
FoundText	DB	'FPU detected.',13,10
MissingText	DB	'No FPU detected.',13,10

.code

start:
	mov	ax,ss
	mov	ds,ax
	finit
	push	eax
	fstcw	[esp]
	pop	eax
	cmp	ah,3
	jne	nofpu

isfpu:
	mov	edx,OFFSET FoundText
	mov	ecx,SIZEOF FoundText
	jmp	output

nofpu:
	mov	edx,OFFSET MissingText
	mov	ecx,SIZEOF MissingText

output:
	mov	bx,1
	mov	ah,40h
	int	21h
	mov	ax,4c00h
	int	21h
end	start
