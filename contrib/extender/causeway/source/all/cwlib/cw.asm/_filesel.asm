	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Display a file selector and get a list of file names from the user.
;
;Calling:
;
;dlgFileSelector(flags,titleptr,pathptr,maskptr,nameptr,filestart,maskstart,
; masknum,drivestart,results);
;
;On Entry:
;
;C style stack parameters as follows,
;
;flags	- Flags, bit significant if set.
;	0 - Allow multiple selections with SHIFT.
;	1 - Allow typed name.
;titleptr	- Title pointer, normal ASCII zero terminated string.
;pathptr	- Path pointer, normal ASCII zero terminated string.
;maskptr	- Masks pointer, list of normal ASCII zero terminated strings, -1
;	  to terminate list.
;nameptr	- Default name pointer, normal ASCII zero terminated string.
;filestart	- Start position of file list.
;maskstart	- Start position of mask list.
;masknum	- Mask number to use by default.
;drivestart	- Start position of drive list.
;results	- Pointer to 6 int's to put results in.
;
;On Exit:
;
;EAX	- Exit code.
;	0 - no problems.
;	1 - CANCEL'd.
;	2 - no files selected.
;	3 - ran out of memory.
;
;list of int's pointed to by "result" filled in as follows:
;
;0	- Exit code (EAX).
;1	- File list start position.
;2	- Mask list start position.
;3	- Mask number.
;4	- Drive list start position.
;5	- List of file names.
;
;ALL other registers preserved.
;
;Notes:
;
;Expects SYSTEM font to have been loaded.
;
_dlgFileSelector proc syscall flags:dword, titleptr:dword, pathptr:dword, \
	maskptr:dword, nameptr:dword, filestart:dword, maskstart:dword, \
	masknum:dword, drivestart:dword, results:dword
	public _dlgFileSelector
	pushad
	push	drivestart
	push	masknum
	push	maskstart
	push	filestart
	push	nameptr
	push	maskptr
	push	pathptr
	push	titleptr
	push	flags
	call	dlgFileSelector
	mov	flags,eax
	mov	eax,results
	mov	[eax+4],ebx
	mov	[eax+8],ecx
	mov	[eax+12],edx
	mov	[eax+16],esi
	mov	[eax+20],edi
	mov	ebx,flags
	mov	[eax],ebx
	popad
	mov	eax,flags
	ret
_dlgFileSelector endp


	efile
	end

