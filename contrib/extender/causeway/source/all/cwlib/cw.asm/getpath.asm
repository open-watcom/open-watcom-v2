	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Get current drive and path.
;
;On Entry:
;
;EDX	- pointer to buffer.
;
;On Exit:
;
;ALL registers preserved.
;
GetPath	proc	near
	pushm	eax,edx,esi,edx
	mov	ah,19h	;get current disc
	int	21h
	mov	dl,al
	add	al,'A'	;make it a character
	pop	esi
	mov	[esi],al
	inc	esi
	mov	b[esi],":"
	inc	esi
	mov	b[esi],"\"
	inc	esi
	mov	b[esi],0
	mov	ah,47h	;get current directory
	xor	dl,dl	;default drive
	int	21h	;get text
	popm	eax,edx,esi
	ret
GetPath	endp


	efile
	end


