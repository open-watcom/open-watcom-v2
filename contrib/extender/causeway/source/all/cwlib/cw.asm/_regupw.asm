	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------------
;
;Update a particular rectangle of the display.
;
;Calling:
;
;WindowRegionUpdate(xcoord,ycoord,wide,depth);
;
;On Entry:
;
;xcoord	- X coord.
;ycoord	- Y coord.
;wide	- width.
;depth	- depth.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_WindowRegionUpdate proc syscall xcoord:dword, ycoord:dword, wide:dword, depth:dword
	public _WindowRegionUpdate
	pushm	ecx,edx,esi,edi
	mov	ecx,xcoord
	mov	edx,ycoord
	mov	esi,wide
	mov	edi,depth
	call	WindowRegionUpdate
	popm	ecx,edx,esi,edi
	ret
_WindowRegionUpdate endp


	efile
	end


