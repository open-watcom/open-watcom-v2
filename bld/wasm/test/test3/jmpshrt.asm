	.MODEL MEDIUM,C

	.CODE
	.386p

	db 100h dup(0)

	public	proc1
proc1:
	mov	ax, 000Bh
	jmp	SHORT proc_both

	public	proc2
proc2:
	mov	ax, 000Ch

proc_both proc USES ES EDI EBX, gsel: word, gptr: dword
	les	di, gptr
	movzx	edi, di
	movzx	ebx, gsel
	int	31h
	jc	SHORT error1
	mov	ax, 1
	jmp	short exit
error1:	xor	ax, ax
exit:	ret
proc_both endp


        END
