include d16info.inc

	.model	small

	.stack

	.data

ver_buf	db	'$$$$$$$$'
msg_ver	db	'DOS/16M version: $'
msg_eol db	10, 13, '$'

	.code
start:
	mov	ah, 09h
	mov	dx, dgroup
	mov	es, dx
	mov	ds, dx
	mov	dx, offset msg_ver
	int	21h			; print message header

	mov	dx, seg d16i
	mov	ds, dx
	lds	si, d16i.versionp	; load version string pointer
	mov	di, offset ver_buf
copy:
	lodsb				; copy version string
	test	al, al			; to convert from null-terminated
	jz	donecopy		; to $-terminated
	stosb
	jmp	copy
donecopy:
	mov	dx, dgroup		; reload DS again
	mov	ds, dx
	mov	dx, offset ver_buf
	int	21h			; print DOS/16M version string

	mov	dx, offset msg_eol
	int	21h			; print CR/LF

exit:
	mov	ah, 4ch
	int	21h

rtdata	segment	at 20h
d16i	d16info	<>
rtdata	ends

	end	start
