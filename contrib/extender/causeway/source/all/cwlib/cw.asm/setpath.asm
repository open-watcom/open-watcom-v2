	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set current drive and path.
;
;On Entry:
;
;EDX	- pointer to path string.
;
;On Exit:
;
;ALL registers preserved.
;
SetPath	proc	near
	pushm	eax,edx
	push	edx
	mov	dl,[edx]
	cmp	dl,61h		; 'a'
	jb	@@0
	cmp	dl,7Ah		; 'z'
	ja	@@0
	and	dl,5Fh		;convert to upper case.
@@0:	sub	dl,"A"
	mov	ah,0eh	;Set drive
	int	21h	;Get number of logical drives
	pop	edx
	mov	ah,3bh
	int	21h
	popm	eax,edx
	ret
SetPath	endp


	efile
	end

