	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Change the border colour. Provided mainly for simplistic debugging.
;
;On Entry:
;
;AL	- colour to set.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
Bord	proc	near
	pushm	ax,dx
	mov	ah,al
	mov	dx,3dah
	in	al,dx
	mov	dl,0c0h
	mov	al,11h
	out	dx,al
	mov	al,ah
	out	dx,al
	mov	al,20h
	out	dx,al
	popm	ax,dx
	ret
Bord	endp


	efile
	end

