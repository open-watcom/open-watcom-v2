	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------------
;
;Generate list of files that match a given file mask. Wild cards permitted.
;
;Calling:
;
;MakeFileList(filemask,attribs);
;
;On Entry:
;
;filemask	- File name mask (can include a path).
;attribs	- search attributes.
;
;On Exit:
;
;EAX	- buffer with file names etc, EAX=0 if not enough memory.
;
;Each entry in the buffer will have the following format.
;
;13 bytes zero terminated ASCII file name (may be less)
; 1 byte file attribute (directory,vulume,hidden,system etc.)
;
; If you specify directories in the attribute search mask the names . & .. will
; be removed from the list to avoid making special checks for them. Anything
; that might have found these useful will have to have knowledge of its
; current path anyway.
;
;
;Attributes are as follows:-
;
;  5  4  3  2  1  0
;  |  |  |  |  |  |
;  |  |  |  |  |  \_ Read only. (1)
;  |  |  |  |  \____ Hidden.    (2)
;  |  |  |  \_______ System.    (4)
;  |  |  \__________ Label.     (8)
;  |  \_____________ Directory. (16)
;  \________________ Archive.   (32)
;
; Bits 6-15 are reserved and may be used in the search mask, but bits 8-15 won't
; appear in the attributes for listed files.
;
; The first dword in the buffer is the number of entries.
; Next 256 bytes is the path to append the names to.
;
_MakeFileList	proc	syscall filemask:dword, attribs:dword
	public _MakeFileList
	pushm	ecx,edx
	mov	edx,filemask
	mov	ecx,attribs
	call	MakeFileList
	popm	ecx,edx
	ret
_MakeFileList	endp


	efile
	end


