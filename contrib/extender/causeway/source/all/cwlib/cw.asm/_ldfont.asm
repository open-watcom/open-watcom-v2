	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Load font file into a slot.
;
;Calling:
;
;LoadFont(filename,slot);
;
;On Entry:
;
;filename	- file name.
;slot	- slot number.
;
;On Exit:
;
;EAX	- status.
;	0 - no error.
;	1 - file not found or I/O error.
;	2 - not enough memory.
;
;ALL other registers preserved.
;
_LoadFont	proc	syscall filename:dword, slot:dword
	public _LoadFont
	pushm	ecx,edx
	mov	ecx,slot
	mov	edx,filename
	call	LoadFont
	popm	ecx,edx
	ret
_LoadFont	endp


	efile
	end


