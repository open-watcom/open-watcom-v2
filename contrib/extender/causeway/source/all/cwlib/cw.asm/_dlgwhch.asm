	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Work out which section of a dialog the specified co-ords relate to.
;
;Calling:
;
;DialogWhich(xcoord,ycoord,dialog,result);
;
;On Entry:
;
;xcoord	- X coord.
;ycoord	- Y coord.
;dialog	- pointer to dialog.
;result	- 3 int buffer for result.
;
;On Exit:
;
;EAX	- Entry number, -1 for none.
;result+0	- EAX
;result+4	- relative x co-ord.
;result+8	- relative y co-ord.
;
;ALL other registers preserved.
;
_DialogWhich	proc	syscall xcoord:dword, ycoord:dword, dialog:dword, result:dword
	public _DialogWhich
	pushm	ecx,edx,esi
	mov	ecx,xcoord
	mov	edx,ycoord
	mov	esi,dialog
	call	_DialogWhich
	mov	esi,result
	mov	[esi],eax
	mov	[esi+4],ecx
	mov	[esi+8],edx
	popm	ecx,edx,esi
	ret
_DialogWhich	endp


	efile
	end


