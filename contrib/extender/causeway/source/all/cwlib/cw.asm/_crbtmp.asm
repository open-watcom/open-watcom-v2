	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Create a new bitmap.
;
;Calling:
;
;CreateBitmap(bflags,wide,depth);
;
;On Entry:
;
;bflags	- bitmap flags entry.
;wide	- bitmap width.
;depth	- bitmap depth.
;
;On Exit:
;
;Carry set on error and EAX=0 else,
;
;EAX	- pointer to bitmap.
;
;ALL other registers preserved.
;
_CreateBitmap	proc syscall bflags:dword, wide:dword, depth:dword
	public _CreateBitmap
	pushm	ebx,ecx,edx
	mov	ebx,bflags
	mov	ecx,wide
	mov	edx,depth
	call	CreateBitmap
	popm	ebx,ecx,edx
	ret
_CreateBitmap	endp


	efile
	end


