	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Append a string to another string.
;
;On Entry:
;
;ESI	- source string.
;EDI	- destination string to append to.
;
AppendString	proc	near
	pushm	eax,ecx,esi,edi
	or	ecx,-1
	xor	al,al
	repne	scasb		;Find end of destination string.
	dec	edi
	push	esi
	xchg	esi,edi
	or	ecx,-1
	repne	scasb
	xchg	esi,edi
	neg	ecx
	inc	ecx
	pop	esi
	rep	movsb
	popm	eax,ecx,esi,edi
	ret
AppendString	endp


	efile
	end

