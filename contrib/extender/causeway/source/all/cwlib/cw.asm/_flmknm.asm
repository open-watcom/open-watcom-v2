	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------------
;
;Make a full drive:path\file name from a file list path string and specified
;file name.
;
;Calling:
;
;FileListMakeName(filelist,entry,buffer);
;
;On Entry:
;
;filelist	- pointer to file list.
;entry	- entry number to make name from.
;buffer	- pointer to destination buffer.
;
;On Exit:
;
;ALL registers preserved.
;
;Name in a file list are just the 8.3 portion. A more useful name can be made
;by combineing the path string from the start of the list buffer with the file
;name which this routine does.
;
_FileListMakeName proc syscall filelist:dword, entry:dword, buffer:dword
	public _FileListMakeName
	pushm	ecx,esi,edi
	mov	ecx,entry
	mov	esi,filelist
	mov	edi,buffer
	call	FileListMakeName
	popm	ecx,esi,edi
	ret
_FileListMakeName endp


	efile
	end


