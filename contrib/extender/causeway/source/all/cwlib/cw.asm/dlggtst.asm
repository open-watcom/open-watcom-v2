	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Get state field of a dialog entry.
;
;On Entry:
;
;ECX	- Entry number.
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- State.
;
;ALL other registers preserved.
;
DialogGetState	proc	near
	pushm	ecx,edx
	mov	eax,size Dlg
	mul	ecx
	mov	ecx,eax
	add	ecx,size DlgMain
	add	ecx,esi
	mov	eax,Dlg_State[ecx]
	popm	ecx,edx
	ret
DialogGetState	endp


	efile
	end


