	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Work out the pixel length of a string.
;
;Calling:
;
;StringLenPixels(string,font);
;
;On Entry:
;
;string	- String to scan.
;font	- Font to use.
;
;On Exit:
;
;EAX	- Length in pixels.
;
;ALL other registers preserved.
;
_StringLenPixels proc syscall string:dword, font:dword
	public _StringLenPixels
	pushm	esi,edi
	mov	esi,string
	mov	edi,font
	call	StringLenPixels
	popm	esi,edi
	ret
_StringLenPixels endp


	efile
	end


