	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------------
;
;Sort a file list into alpha-numeric order.
;
;Calling:
;
;SortFileList(filelist);
;
;On Entry:
;
;filelist	- pointer to file list to sort.
;
;On Exit:
;
;ALL registers preserved.
;
;File names are not case converted so upper case names will appear at the top
;of the list.
;
_SortFileList	proc	syscall filelist:dword
	public _SortFileList
	push	esi
	mov	esi,filelist
	call	SortFileList
	pop	esi
	ret
_SortFileList	endp


	efile
	end


