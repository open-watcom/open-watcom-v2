	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Convert a string into upper case.
;
;On Entry:
;
;ESI	- String to convert.
;
;On Exit:
;
;ALL registers preserved.
;
UpperString	proc	near
	pushm	eax,esi,edi
	mov	edi,esi
l0:	lodsb
	cmp	al,61h		; 'a'
	jb	l1
	cmp	al,7Ah		; 'z'
	ja	l1
	and	al,5Fh		;convert to upper case.
l1:	stosb
	or	al,al
	jnz	l0
	popm	eax,esi,edi
	ret
UpperString	endp


	efile
	end

