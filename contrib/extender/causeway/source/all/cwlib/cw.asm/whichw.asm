	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;See which (if any) window co-ords fall inside and return relative values
;
;On Entry:
;
;ECX	- Xcoord
;EDX	- Ycoord
;
;On Exit:
;
;EAX	- Window structure pointer (handle), -1 for none.
;ECX	- relative X coord.
;EDX	- relative Y coord.
;
;ALL other registers preserved.
;
WinWhichWindow:
	public WinWhichWindow
_WinWhichWindow_ proc syscall
	local	@@window:dword, @@X:dword, @@Y:dword
	pushad
	;
	mov	@@X,ecx
	mov	@@Y,edx
	mov	esi,offset WinWindowList
	mov	ecx,MaxWinWindows
@@0:	cmp	d[esi],-1
	jz	@@8
	;
	push	esi
	mov	esi,[esi]
	mov	edx,esi
	lea	esi,Win_Splits[esi]
@@1:	cmp	Split_Wide[esi],0
	jz	@@7
	mov	eax,Split_XCoord[esi]
	cmp	eax,@@X
	jg	@@6
	add	eax,Split_Wide[esi]
	cmp	eax,@@X
	jl	@@6
	mov	eax,Split_YCoord[esi]
	cmp	eax,@@Y
	jg	@@6
	add	eax,Split_Depth[esi]
	cmp	eax,@@Y
	jl	@@6
	mov	eax,@@X
	sub	eax,Win_XCoord[edx]
	mov	@@X,eax
	mov	eax,@@Y
	sub	eax,Win_YCoord[edx]
	mov	@@Y,eax
	mov	@@window,edx
	pop	esi
	jmp	@@9
	;
@@6:	add	esi,size Split
	jmp	@@1
	;
@@7:	pop	esi
@@8:	add	esi,4
	loop	@@0
	mov	@@window,-1
@@9:	popad
	mov	eax,@@window
	mov	ecx,@@X
	mov	edx,@@Y
	ret
_WinWhichWindow_ endp


	efile
	end

