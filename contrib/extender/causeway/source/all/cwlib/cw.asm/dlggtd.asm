	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get main depth of a dialog.
;
;On Entry:
;
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Depth.
;
;ALL other registers preserved.
;
DialogGetDepth	proc near
	push	edx
	mov	eax,DlgM_FDepth[esi]
	mul	FontDepth
	add	eax,DlgM_PDepth[esi]
	pop	edx
	ret
DialogGetDepth	endp


	efile
	end


