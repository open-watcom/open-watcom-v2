	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get main width of a dialog.
;
;On Entry:
;
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Width.
;
;ALL other registers preserved.
;
DialogGetWidth	proc near
	push	edx
	mov	eax,DlgM_FWide[esi]
	mul	FontWidth
	add	eax,DlgM_PWide[esi]
	pop	edx
	ret
DialogGetWidth	endp


	efile
	end

