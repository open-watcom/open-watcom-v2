	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Convert number into ASCII Hex version.
;
;On Entry:
;
;EAX	- Number to convert.
;ECX	- Digits to do.
;EDI	- Buffer to put string in.
;
;On Exit:
;
;EDI	- Next un-used byte in buffer.
;
;All other registers preserved.
;
Bin2Hex	proc	near
	pushm	eax,ebx,ecx,edx,edi
	mov	ebx,offset HexTable
	add	edi,ecx
	dec	edi
	mov	edx,eax
@@0:	mov	al,dl
	shr	edx,4
	and	al,15
	xlat
	mov	[edi],al
	dec	edi
	loop	@@0
	popm	eax,ebx,ecx,edx,edi
	add	edi,ecx
	ret
Bin2Hex	endp


	sdata

HexTable	db "0123456789ABCDEF"


	efile
	end
