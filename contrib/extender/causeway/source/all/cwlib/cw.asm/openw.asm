	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Initialise a window structure and add it to the list of windows in service.
;
;On Entry:
;
;C style stack parameters as follows:
;
;flags	- undefined at the moment, set to zero.
;priority	- priority to assign.
;xcoord	- x co-ord for window.
;ycoord	- y co-ord for window.
;wide	- width of window in pixels.
;depth	- depth of window in pixels.
;handler	- address of handler (0 for none).
;
;On Exit:
;
;EAX	- pointer to window structure (handle).
;
;ALL other registers presereved.
;
WinOpenWindow:
_WinOpenWindow	proc	syscall flags:dword, priority:dword, xcoord:dword, \
	ycoord:dword, wide:dword, depth:dword, handler:dword
	local @@Window:dword
	pushad
	mov	ecx,size Win
	call	Malloc
	mov	@@Window,esi
	jc	@@9
	mov	eax,XCoord
	mov	Win_XCoord[esi],eax
	mov	eax,YCoord
	mov	Win_YCoord[esi],eax
	mov	eax,Wide
	mov	Win_Wide[esi],eax
	mov	eax,Depth
	mov	Win_Depth[esi],eax
	mov	eax,Priority
	mov	Win_Priority[esi],eax
	mov	eax,Handler
	mov	Win_Handler[esi],eax
	lea	edi,Win_Splits[esi]
	mov	Split_Wide[edi],0
	;
	mov	edi,offset WinWindowList
	mov	ecx,MaxWinWindows
@@0:	cmp	d[edi],-1
	jz	@@1
	add	edi,4
	loop	@@0
	;
	mov	esi,@@Window
	call	Free
	mov	@@Window,0
	jmp	@@9
	;
@@1:	mov	[edi],esi
	call	CalculateSplits
	;
	mov	eax,WinMess_Draw
	call	WinMessageWindow
	;
@@9:	popad
	mov	eax,@@Window
	ret
_WinOpenWindow	endp


	efile
	end

