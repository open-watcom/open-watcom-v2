	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;See which (if any) window co-ords fall inside and return relative values
;
;Calling:
;
;WinWhichWindow(ints1,ints2);
;
;On Entry:
;
;ints1	- pointer to 3 int's with button, xcoord and ycoord info.
;ints2	- pointer to 3 int's to fill in, button becomes window.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_WinWhichWindow proc syscall ints1:dword, ints2:dword
	public _WinWhichWindow
	pushm	eax,ecx,edx,esi
	mov	esi,ints1
	mov	ecx,[esi+4]
	mov	edx,[esi+8]
	call	WinWhichWindow
	mov	esi,ints2
	mov	[esi],eax
	mov	[esi+4],ecx
	mov	[esi+8],edx
	popm	eax,ecx,edx,esi
	ret
_WinWhichWindow endp


	efile
	end


