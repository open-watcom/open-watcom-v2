	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------------
;
;Make a full drive:path\file name from a file list path string and specified
;file name.
;
;On Entry:
;
;ECX	- entry number to make name from.
;ESI	- pointer to file list.
;EDI	- pointer to destination buffer.
;
;On Exit:
;
;ALL registers preserved.
;
;Name in a file list are just the 8.3 portion. A more useful name can be made
;by combineing the path string from the start of the list buffer with the file
;name which this routine does.
;
FileListMakeName proc near
	pushm	eax,edx,esi,edi
	add	esi,4
	call	CopyString		;Copy path string into place.
	pushm	ecx,edi
	or	ecx,-1
	xor	al,al
	repnz	scasb		;find end of string.
	dec	edi
	cmp	b[edi-1],"\"
	jz	@@0
	mov	b[edi],"\"
	mov	b[edi+1],0
@@0:	popm	ecx,edi
	mov	eax,14
	mul	ecx
	add	esi,eax
	add	esi,256
	call	AppendString		;Append file name.
	popm	eax,edx,esi,edi
	ret
FileListMakeName endp


	efile
	end

