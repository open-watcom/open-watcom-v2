	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Display a message box with OK/CANCEL buttons.
;
;Calling:
;
;DlgMessageOKCANCEL(message);
;
;On Entry:
;
;message	- Message to display.
;
;On Exit:
;
;EAX	- status.
;	0 - Cancel.
;	!=0 - OK.
;
;All other registers preserved.
;
_DlgMessageOKCANCEL proc syscall message:dword
	push	esi
	mov	esi,message
	call	DlgMessageOKCANCEL
	pop	esi
	ret
_DlgMessageOKCANCEL endp


;-------------------------------------------------------------------------
;
;Display a message box with OK/CANCEL buttons.
;
;On Entry:
;
;ESI	- Message to display.
;
;On Exit:
;
;EAX	- status.
;	0 - Cancel.
;	!=0 - OK.
;
;All other registers preserved.
;
DlgMessageOKCANCEL proc near
	public DlgMessageOKCANCEL
	local @@window:dword, @@mainx:dword, @@mainy:dword, @@mainw:dword,
	@@maind:dword
	pushm	ebx,ecx,edx,esi,edi
;
;setup the string pointer.
;
	mov	edi,size DlgMain+offset dMessageOkCancel
	mov	Dlg_Text[edi],esi
;
;Setup main window dimensions and coords, then open it ready for action.
;
	mov	esi,offset dMessageOkCancel
	call	DialogGetWidth
	mov	@@mainw,eax
	call	DialogGetDepth
	mov	@@maind,eax
	mov	eax,VideoXResolution
	sub	eax,@@mainw
	shr	eax,1
	mov	@@mainx,eax
	mov	eax,VideoYResolution
	sub	eax,@@maind
	shr	eax,1
	mov	@@mainy,eax
	push	0
	push	@@maind
	push	@@mainw
	push	@@mainy
	push	@@mainx
	push	500
	push	0
	call	WinOpenWindow
	add	esp,4*7
	mov	@@window,eax
	;
	mov	esi,@@window
	or	eax,-1
	xor	ecx,ecx
	xor	ebx,ebx
	mov	edx,-1
	mov	edi,-1
	call	ButtonWindow
	;
	mov	edi,esi
	mov	esi,offset dMessageOkCancel
	or	ecx,-1
	call	DrawDialog
;
;Get current mouse pointer mode so we can restore it and make sure we're
;useing the arrow.
;
	xor	al,al
	call	MouseSetType
	mov	_OC_OldMouse,al
	;
@@waitloop:	call	_OC_ReadInput
	cmp	_OC_KeyCode,0
	jz	@@w0
	cmp	_OC_KeyCode,1
	jz	@@cancelexit
	cmp	_OC_KeyASCII,13
	jz	@@okexit
	;
@@w0:	test	_OC_mouseb,1
	jz	@@waitloop
	;
	mov	eax,_OC_mousewin
	cmp	eax,@@window
	jnz	@@waitloop
	mov	ecx,_OC_mousewinx
	mov	edx,_OC_mousewiny
	mov	esi,offset dMessageOkCancel
	call	DialogWhich
	cmp	eax,1
	jz	@@okexit
	cmp	eax,2
	jz	@@cancelexit
	jmp	@@waitloop
	;
@@okexit:	mov	esi,offset dMessageOkCancel
	mov	ecx,1
	mov	eax,1
	call	DialogSetState
	mov	edi,@@window
	call	DrawDialog
@@oe0:	call	_OC_ReadInput
	test	_OC_mouseb,1
	jnz	@@oe0
	mov	ecx,2
	call	Delay
	mov	esi,offset dMessageOkCancel
	mov	ecx,1
	mov	eax,0
	call	DialogSetState
	mov	edi,@@window
	call	DrawDialog
	mov	ecx,2
	call	Delay
	or	eax,-1
	jmp	@@exit
	;
@@cancelexit:	mov	esi,offset dMessageOkCancel
	mov	ecx,2
	mov	eax,1
	call	DialogSetState
	mov	edi,@@window
	call	DrawDialog
@@oe1:	call	_OC_ReadInput
	test	_OC_mouseb,1
	jnz	@@oe1
	mov	ecx,2
	call	Delay
	mov	esi,offset dMessageOkCancel
	mov	ecx,2
	mov	eax,0
	call	DialogSetState
	mov	edi,@@window
	call	DrawDialog
	mov	ecx,2
	call	Delay
	xor	eax,eax
	;
@@exit:	push	eax
	mov	eax,@@window
	call	WinCloseWindow
	mov	al,_OC_OldMouse
	call	MouseSetType
	pop	eax
	popm	ebx,ecx,edx,esi,edi
	ret
DlgMessageOKCANCEL endp


;-------------------------------------------------------------------------
;
;Read the keyboard and mouse.
;
_OC_ReadInput	proc	near
	call	GetKey		;read the keyboard state.
	mov	_OC_KeyASCII,al
	mov	_OC_KeyCode,ah
	shr	eax,16
	mov	_OC_KeyFlags,ax
	call	MouseGet		;read the mouse state.
	mov	_OC_mouseb,ebx
	mov	_OC_mousex,ecx
	mov	_OC_mousey,edx
	call	WinWhichWindow	;find out which (if any) window
	mov	_OC_mousewin,eax	;the mouse is currenty in.
	mov	_OC_mousewinx,ecx
	mov	_OC_mousewiny,edx
	ret
_OC_ReadInput	endp


	sdata
;
_OC_mousewin	dd ?
_OC_mousewinx	dd ?
_OC_mousewiny	dd ?
_OC_mouseb	dd ?
_OC_mousex	dd ?
_OC_mousey	dd ?
_OC_KeyASCII	db ?
_OC_KeyCode	db ?
_OC_KeyFlags	dw ?
;
dMessageOkCancel label byte
	DlgMain <30,6, 2,19>
	Dlg <2, 0,3, 0,3, 30,0, 1,6, (9*256)+1,0>	; message
	Dlg <0, 9,0, 1,10, 6,6, 1,6, (9*256)+1+8,_OC_Ok> ; OK button.
	Dlg <0, 15,7, 1,10, 6,6, 1,6, (9*256)+1+8,_OC_Cancel> ; CANCEL button.
	Dlg <-1>
;
_OC_Ok	db "OK",0
_OC_Cancel	db "CANCEL",0
;
_OC_OldMouse	db 0
;
	efile
	end


