	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------------
;
;Convert normal names to lower case and directory names to upper case.
;
;Calling:
;
;ULFileList(filelist);
;
;On Entry:
;
;filelist	- pointer to file list.
;
;On Exit:
;
;ALL registers preserved.
;
;Most routines that need to display a file list want directory names in upper
;case and the remaining names in lower case. This routine formats a file list
;to that specification. Doing things this way means that if the file list is
;then sorted, directory names will appear at the top.
;
_ULFileList	proc	syscall filelist:dword
	public _ULFileList
	pushm	esi
	mov	esi,filelist
	call	ULFileList
	popm	esi
	ret
_ULFileList	endp


	efile
	end


