	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Wait for a while.
;
;On Entry:
;
;ECX	- time to wait for.
;
Delay	proc	near
	pushm	eax,ebx,ecx,edx,esi
	mov	esi,ecx
	mov	ah,0
	int	1ah
	shl	ecx,16
	mov	cx,dx
	mov	ebx,ecx
l0:	mov	ah,0
	int	1ah
	shl	ecx,16
	mov	cx,dx
	sub	ecx,ebx
	cmp	ecx,esi
	jc	l0
	popm	eax,ebx,ecx,edx,esi
	ret
Delay	endp


	efile
	end


