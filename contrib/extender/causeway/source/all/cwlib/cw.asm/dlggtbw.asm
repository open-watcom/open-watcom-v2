	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get width of a dialog entry.
;
;On Entry:
;
;ECX	- Entry number.
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Width.
;
;ALL other registers preserved.
;
DialogGetButtonW proc near
	pushm	ecx,edx
	mov	eax,size Dlg
	mul	ecx
	mov	ecx,eax
	add	ecx,size DlgMain
	add	ecx,esi
	mov	eax,Dlg_FWide[ecx]
	mul	FontWidth
	add	eax,Dlg_PWide[ecx]
	popm	ecx,edx
	ret
DialogGetButtonW endp


	efile
	end


