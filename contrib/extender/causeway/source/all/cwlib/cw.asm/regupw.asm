	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------------
;
;Update a particular rectangle of the display.
;
;On Entry:
;
;ECX	- X coord.
;EDX	- Y coord.
;ESI	- width.
;EDI	- depth.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
WindowRegionUpdate:
	public WindowRegionUpdate
_WindowRegionUpdate_ proc syscall
	local @@Count:dword, @@Handle:dword
	pushad
	push	WindowRegionFlag
	push	WindowRegionX
	push	WindowRegionY
	push	WindowRegionW
	push	WindowRegionD
	;
	mov	WindowRegionFlag,-1
	mov	WindowRegionX,ecx
	mov	WindowRegionW,ecx
	mov	WindowRegionY,edx
	mov	WindowRegionD,edx
	add	WindowRegionW,esi
	add	WindowRegionD,edi
	;
	mov	@@Count,MaxWinWindows
	mov	@@Handle,offset WinWindowList
	;
@@0:	mov	ebx,@@Handle
	mov	ebx,[ebx]
	cmp	ebx,-1
	jz	@@1
	mov	ecx,Win_XCoord[ebx]
	mov	edx,Win_YCoord[ebx]
	mov	esi,Win_Wide[ebx]
	mov	edi,Win_Depth[ebx]
	call	WindowRegionClip
	jnz	@@1
	;
	mov	esi,ebx
	mov	eax,WinMess_Draw
	call	WinMessageWindow
	;
@@1:	add	@@Handle,4
	dec	@@Count
	jnz	@@0
	;
	pop	WindowRegionD
	pop	WindowRegionW
	pop	WindowRegionY
	pop	WindowRegionX
	pop	WindowRegionFlag
	popad
	ret
_WindowRegionUpdate_ endp


	efile
	end

