	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get X coord of a dialog entry.
;
;On Entry:
;
;ECX	- Entry number.
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- X coord.
;
;ALL other registers preserved.
;
DialogGetButtonX proc near
	pushm	ecx,edx
	mov	eax,size Dlg
	mul	ecx
	mov	ecx,eax
	add	ecx,size DlgMain
	add	ecx,esi
	mov	eax,Dlg_FXCoord[ecx]
	mul	FontWidth
	add	eax,Dlg_PXCoord[ecx]
	popm	ecx,edx
	ret
DialogGetButtonX endp


	efile
	end



