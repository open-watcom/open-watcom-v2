	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Draw a dialog entry.
;
;On Entry:
;
;ECX	- Button number, -1 for all.
;ESI	- Dialog.
;EDI	- Window.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
DrawDialog	proc	near
	pushad
	mov	ebx,esi
	add	ebx,size DlgMain	;Point to start of entries.
	cmp	ecx,-1		;Doing all of them?
	jz	@@0
	mov	eax,size Dlg		;move to specific entry.
	mul	ecx
	add	ebx,eax
	;
@@0:	mov	_DD_Dlg,esi
	mov	_DD_Win,edi
	mov	_DD_But,ecx
	mov	_DD_This,ebx
	cmp	_DD_But,-1
	jnz	@@1
	mov	_DD_But,0
	pushm	ebx,ecx,esi
	call	DialogGetWidth
	mov	edx,eax
	call	DialogGetDepth
	mov	edi,eax
	xor	ebx,ebx
	mov	ecx,ebx
	or	eax,-1
	mov	esi,_DD_Win
	call	ButtonWindow
	popm	ebx,ecx,esi
	;
@@1:	test	Dlg_Flags[ebx],128	;Button it?
	jnz	@@7
	pushm	ebx,ecx,esi
	mov	ecx,_DD_But		;Get button dimensions and
	call	DialogGetButtonD	;draw the button.
	mov	edi,eax
	call	DialogGetButtonW
	mov	edx,eax
	call	DialogGetButtonY
	mov	ebx,eax
	call	DialogGetButtonX
	push	eax
	call	DialogGetState
	pop	ecx
	mov	esi,_DD_Win
	call	ButtonWindow
	popm	ebx,ecx,esi
;
;Check if any text needs printing.
;
@@7:	cmp	Dlg_Text[ebx],0	;Will be != 0 if text is needed.
	jz	@@2
	pushm	ebx,ecx,esi
	pushm	esi,edi
	mov	esi,Dlg_Text[ebx]	;Get pointer to text.
	mov	edi,SystemFont
	call	StringLenPixels	;Get pixel length of the string.
	popm	esi,edi
	mov	edx,eax
	mov	ecx,_DD_But
	call	DialogGetButtonY	;Get button position.
	add	eax,2
	mov	ebp,eax
	call	DialogGetButtonX
	add	eax,2
	mov	edi,eax
	test	Dlg_Flags[ebx],1+2	;Check horizontal justification.
	jz	@@3		;Left justified.
	test	Dlg_Flags[ebx],1
	jz	@@4
	call	DialogGetButtonW	;Needs to be centered so get
	sub	eax,4		;button width and use it to
	shr	eax,1		;work out where we really want
	shr	edx,1		;to print.
	sub	eax,edx
	add	edi,eax
	jmp	@@3
@@4:	call	DialogGetButtonW	;Needs to be right justified.
	sub	eax,4
	sub	eax,edx
	add	edi,eax
;
@@3:	mov	edx,FontDepth
	test	Dlg_Flags[ebx],4+8	;Check vertical justification.
	jz	@@5
	test	Dlg_Flags[ebx],4	;Top justified.
	jz	@@6
	call	DialogGetButtonD	;Needs to be centered.
	sub	eax,4
	shr	eax,1
	shr	edx,1
	sub	eax,edx
	add	ebp,eax
	jmp	@@5
;
@@6:	call	DialogGetButtonD	;Needs to be bottom justified.
	sub	eax,4
	sub	eax,edx
	add	ebp,eax
;
@@5:	push	ebp		;Now finaly we print the string.
	push	edi
	push	Dlg_Text[ebx]
	push	SystemFont
	mov	eax,Dlg_Flags[ebx]
	shr	eax,8
	call	VideoSys2Hard
	push	eax
	push	2
	push	_DD_Win
	call	WinPrintWindow
	add	esp,4*7
	popm	ebx,ecx,esi
;
@@2:	cmp	ecx,-1		;Doing multiple buttons?
	jnz	@@9
	add	ebx,size Dlg
	inc	_DD_But
	cmp	Dlg_State[ebx],-1	;Reached end of the button list?
	jnz	@@1
	;
@@9:	popad
	ret
DrawDialog	endp


	sdata
;
_DD_Dlg	dd ?
_DD_Win	dd ?
_DD_But	dd ?
_DD_This	dd ?


	efile
	end

