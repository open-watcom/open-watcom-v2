	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Work out which section of a dialog the specified co-ords relate to.
;
;On Entry:
;
;ECX	- X coord.
;EDX	- Y coord.
;ESI	- Dialog.
;
;On Exit:
;
;EAX	- Entry number, -1 for none.
;ECX	- Relative X coord.
;EDX	- Relative Y coord.
;
;ALL other registers preserved.
;
DialogWhich	proc	near
	pushm	ebx,esi,edi,ebp
	mov	ebx,ecx
	mov	edi,esi
	add	edi,size DlgMain
	xor	ecx,ecx
	;
@@0:	cmp	Dlg_State[edi],-1
	jz	@@9
	call	DialogGetButtonX
	cmp	ebx,eax
	jl	@@1
	mov	ebp,eax
	call	DialogGetButtonW
	add	eax,ebp
	cmp	ebx,eax
	jge	@@1
	call	DialogGetButtonY
	cmp	edx,eax
	jl	@@1
	mov	ebp,eax
	call	DialogGetButtonD
	add	eax,ebp
	cmp	edx,eax
	jge	@@1
	call	DialogGetButtonX
	sub	ebx,eax
	call	DialogGetButtonY
	sub	edx,eax
	mov	eax,ecx
	mov	ecx,ebx
	jmp	@@10
	;
@@1:	add	edi,size Dlg
	inc	ecx
	jmp	@@0
	;
@@9:	mov	eax,-1
@@10:	popm	ebx,esi,edi,ebp
	ret
DialogWhich	endp


	efile
	end

