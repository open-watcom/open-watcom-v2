	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Make a piece of window look like a button.
;
;Calling:
;
;ButtonWindow(btype,xcoord,ycoord,wide,depth,window);
;
;On Entry:
;
;btype	- Type of button to draw.
;xcoord	- X coord to draw at.
;ycoord	- Y coord to draw at.
;wide	- width of button.
;depth	- depth of button.
;window	- Pointer to window structure.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_ButtonWindow	proc	syscall btype:dword, xcoord:dword, ycoord:dword, \
	wide:dword, depth:dword, window:dword
	public _ButtonWindow
	pushad
	mov	eax,btype
	mov	ecx,xcoord
	mov	ebx,ycoord
	mov	edx,wide
	mov	edi,depth
	mov	esi,window
	call	ButtonWindow
	popad
	ret
_ButtonWindow	endp


	efile
	end


