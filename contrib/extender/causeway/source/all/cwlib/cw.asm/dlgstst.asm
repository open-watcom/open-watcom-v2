	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Set state field of a dialog entry.
;
;On Entry:
;
;EAX	- State to set.
;ECX	- Entry number.
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
DialogSetState	proc	near
	pushm	ecx,edx,eax
	mov	eax,size Dlg
	mul	ecx
	mov	ecx,eax
	add	ecx,size DlgMain
	add	ecx,esi
	pop	Dlg_State[ecx]
	popm	ecx,edx
	ret
DialogSetState	endp


	efile
	end

