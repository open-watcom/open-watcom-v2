	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get depth of a dialog entry.
;
;On Entry:
;
;ECX	- Entry number.
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Depth.
;
;ALL other registers preserved.
;
DialogGetButtonD proc near
	pushm	ecx,edx
	mov	eax,size Dlg
	mul	ecx
	mov	ecx,eax
	add	ecx,size DlgMain
	add	ecx,esi
	mov	eax,Dlg_FDepth[ecx]
	mul	FontDepth
	add	eax,Dlg_PDepth[ecx]
	popm	ecx,edx
	ret
DialogGetButtonD endp


	efile
	end



