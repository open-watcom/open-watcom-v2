	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Get key press info.
;
;On Entry:
;
;none.
;
;On Exit:
;
;AL	- ASCII character code.
;AH	- Hardware key code.
;EAX high word	- Shift keys status word from 417h.
;
GetKey	proc	near
	push	ebx
	xor	ebx,ebx
	mov	ah,1
	int	16h		;keypress waiting?
	jz	l0
	mov	ah,0
	int	16h		;get key codes.
	movzx	ebx,ax
l0:	push	es
	mov	es,ZeroSelector
	movzx	eax,es:w[417h]	;get shift status.
	pop	es
	shl	eax,16
	or	eax,ebx
	pop	ebx
	ret
GetKey	endp


	efile
	end

