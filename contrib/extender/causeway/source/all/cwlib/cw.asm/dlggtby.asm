	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get Y coord of a dialog entry.
;
;On Entry:
;
;ECX	- Entry number.
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Y coord.
;
;ALL other registers preserved,
;
DialogGetButtonY proc near
	pushm	ecx,edx
	mov	eax,size Dlg
	mul	ecx
	mov	ecx,eax
	add	ecx,size DLGMain
	add	ecx,esi
	mov	eax,Dlg_FYCoord[ecx]
	mul	FontDepth
	add	eax,Dlg_PYCoord[ecx]
	popm	ecx,edx
	ret
DialogGetButtonY endp


	efile
	end

