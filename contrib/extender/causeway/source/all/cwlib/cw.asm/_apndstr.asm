	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Append a string to another string.
;
;On Entry:
;
;source	- pointer to source string.
;dest	- pointer to destination string to append to.
;
_AppendString proc	syscall source:dword, dest:dword
	public _AppendString
	pushm	esi,edi
	mov	esi,source
	mov	edi,dest
	call	AppendString		;Call ASM version.
	popm	esi,edi
	ret
_AppendString endp

	efile

	end


