
	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Close a window and update the area underneath it if possible.
;
;On Entry:
;
;EAX	- Pointer to window structure (handle).
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
WinCloseWindow:
	public WinCloseWindow
_WinCloseWindow_ proc syscall
	local @@XCoord:dword, @@YCoord:dword, @@Wide:dword, @@Depth:dword
	pushad
	mov	esi,eax
;
;Let the windows handler know its being closed incase it wants to do something.
;
	mov	eax,WinMess_Close
	call	WinMessageWindow
;
;Find the window in the main list.
;
@@2:	mov	edi,offset WinWindowList
	mov	ecx,MaxWinWindows
@@0:	cmp	[edi],esi
	jz	@@1
	add	edi,4
	loop	@@0
	jmp	@@9
;
;Delete list entry and retrieve coords.
;
@@1:	mov	d[edi],-1		;Remove it from the list.
	mov	eax,Win_XCoord[esi]
	mov	@@XCoord,eax
	mov	eax,Win_YCoord[esi]
	mov	@@YCoord,eax
	mov	eax,Win_Wide[esi]
	mov	@@Wide,eax
	mov	eax,Win_Depth[esi]
	mov	@@Depth,eax
	call	Free		;Release window's memory.
;
;Now we try and re-build what was under this window.
;
	call	CalculateSplits	;calculate splits again.
	mov	ecx,@@XCoord
	mov	edx,@@YCoord
	mov	esi,@@Wide
	mov	edi,@@Depth
	call	WindowRegionUpdate	;Update this area of the display.
;
@@9:	popad
	ret
_WinCloseWindow_ endp


	efile
	end

