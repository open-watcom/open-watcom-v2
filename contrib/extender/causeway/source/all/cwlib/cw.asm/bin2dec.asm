	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Convert number into decimal ASCII.
;
;On Entry:
;
;EAX	- Number to convert.
;EDI	- Buffer to put it in.
;
;On Exit:
;
;EDI	- Next available byte in buffer.
;
Bin2Dec	proc	near
	pushm	eax,ebx,ecx,edx
	xor	bl,bl		;Clear output flag.
	mov	ecx,1000000000	;Set initial divisor.
l0:	xor	edx,edx
	div	ecx
	cmp	ecx,1		;Last digit?
	jz	l1
	or	al,al		;a zero?
	jnz	l1
	or	bl,bl		;done any others yet?
	jz	l2
l1:	mov	[edi],al		;Store character.
	add	b[edi],'0'		;make it ASCII.
	inc	edi
	inc	bl
l2:	push	edx		;store remainder.
	mov	eax,ecx
	xor	edx,edx
	mov	ecx,10		;work out new divisor.
	div	ecx
	mov	ecx,eax
	pop	eax		;get remaindor back.
	or	ecx,ecx
	jnz	l0
	popm	eax,ebx,ecx,edx
	ret
Bin2Dec	endp


	efile
	end


