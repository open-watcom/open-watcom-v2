	include ..\cwlib.inc
	scode


;-------------------------------------------------------------------------
;
;Display a file selector and get a list of file names from the user.
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
;
;On Exit:
;
;EAX	- Exit code.
;	0 - no problems.
;	1 - CANCEL'd.
;	2 - no files selected.
;	3 - ran out of memory.
;
;EBX	- File list start position.
;ECX	- Mask list start position.
;EDX	- Mask number.
;ESI	- Drive list start position.
;EDI	- List of file names.
;
;ALL other registers preserved.
;
;Notes:
;
;Expects SYSTEM font to have been loaded.
;
dlgFileSelector:
	public dlgFileSelector
_dlgFileSelector_ proc syscall flags:dword, titleptr:dword, pathptr:dword, \
	maskptr:dword, nameptr:dword, filestart:dword, maskstart:dword, \
	masknum:dword, drivestart:dword
	pushad
	;
	mov	eax,filestart
	mov	_FS_FirstFile,eax
	mov	_FS_CenterFile,0
	;
	mov	eax,titleptr
	mov	esi,((size Dlg)*dFILE_TITLE)+(size DlgMain)+offset dFileSelector
	mov	Dlg_Text[esi],eax
	mov	esi,((size Dlg)*dFILE_PATH)+(size DlgMain)+offset dFileSelector
	mov	Dlg_Text[esi],offset _FS_PrintPath
	;
	mov	_FS_PrintPath,0	;clear path for initial print.
	mov	_FS_FileList,0	;clear file list pointer.
	mov	_FS_FirstTyped,0	;reset typed name cursor to start.
	mov	_FS_TypedFocus,0	;remove typed name focus.
;
;Setup the initial path string.
;
	mov	esi,pathptr		;supplied path.
	mov	edi,offset _FS_currentpath ;our path storage.
	call	CopyString
	mov	esi,offset _FS_currentpath ;make sure it's upper case.
	call	UpperString
	mov	edx,offset _FS_currentpath ;force specified path.
	call	SetPath
;
;Setup mask stuff.
;
	mov	_FS_TotalMasks,0	;clear total masks.
	mov	esi,maskptr
@@1:	lodsb
	or	al,al		;end of a mask yet?
	jnz	@@1
	inc	_FS_TotalMasks	;update number of masks.
	cmp	b[esi],-1		;end of the list yet?
	jnz	@@1
	mov	eax,maskstart
	cmp	eax,_FS_TotalMasks
	jc	@@7
	jz	@@7
	xor	eax,eax
@@7:	mov	_FS_FirstMask,eax	;set list entry.
	mov	eax,masknum
	cmp	eax,_FS_TotalMasks
	jc	@@8
	jz	@@8
	xor	eax,eax
@@8:	mov	_FS_CurrentMask,eax	;set current mask.
;
;Copy typed name.
;
	mov	edi,offset _FS_typedname	;our name storage.
	mov	ecx,16/4
	xor	eax,eax
	rep	stosd
	mov	esi,nameptr		;supplied name.
	mov	edi,offset _FS_typedname	;our name storage.
	call	CopyString
	call	LenString		;get name length so we can set
	mov	_FS_TypedTotal,eax	;the limit.
	mov	b[_FS_typedname+13],128	;default to selected.
	cmp	b[_FS_typedname],0
	jz	@@NotFocus
	or	_FS_TypedFocus,-1
;
;Force current extension.
;
	mov	esi,offset _FS_TypedName
	or	edi,-1
@@itc0:	mov	al,[esi]
	inc	esi
	cmp	al,"."
	jnz	@@itc1
	mov	edi,esi
@@itc1:	cmp	al,"\"
	jnz	@@itc2
	or	edi,-1
@@itc2:	or	al,al
	jnz	@@itc0
	cmp	edi,-1		;any extension?
	jnz	@@itc6
	mov	edi,esi
	mov	b[edi-1],"."
@@itc6:	mov	ecx,_FS_CurrentMask	;current top of list.
	mov	esi,maskptr		;the mask list.
	or	ecx,ecx
	jz	@@itc4
@@itc3:	mov	al,[esi]
	inc	esi
	or	al,al		;end of a mask yet?
	jnz	@@itc3
	dec	ecx		;got to the entry we want yet?
	jnz	@@itc3
@@itc4:	lodsb
	cmp	al,"."
	jnz	@@itc4
	cmp	b[esi],"*"		;this an ALL job?
	jz	@@itc5
	call	CopyString
@@itc5:	;
@@NotFocus:	;
	mov	edx,offset _FS_drivelist	;our drive list buffer.
	call	GetDrives		;get a list of valid drives.
	mov	_FS_TotalDrives,ecx	;store total number of drives.
	mov	esi,offset _FS_drivelist
@@sd0:	add	b[esi],"A"		;convert drive numbers to ASCII.
	add	esi,2
	loop	@@sd0
	call	GetDrive		;get current drive.
	add	al,"A"		;make it ASCII.
	xor	ecx,ecx
	mov	esi,offset _FS_drivelist	;our list of drives.
@@5:	cmp	al,[esi]
	jz	@@6		;found current drive yet?
	add	esi,2
	inc	ecx		;next drive.
	jmp	@@5
@@6:	mov	_FS_CurrentDrive,ecx	;store current drive highlight position.
	mov	eax,drivestart
	cmp	eax,_FS_TotalDrives
	jc	@@9
	jz	@@9
	xor	eax,eax
@@9:	mov	_FS_FirstDrive,eax
;
;Setup main window dimensions and coords, then open it ready for action.
;
	mov	esi,offset dFileSelector
	call	DialogGetWidth	;width of main dialog.
	mov	_FS_mainw,eax
	mov	ecx,eax
	shr	ecx,1
	call	DialogGetDepth	;depth of main dialog.
	mov	_FS_maind,eax
	mov	edx,eax
	shr	edx,1
	mov	eax,VideoXResolution	;use video x resolution to
	shr	eax,1		;center the selector.
	sub	eax,ecx
	mov	_FS_mainx,eax
	mov	eax,VideoYResolution	;use video y resolution to
	shr	eax,1		;center the selector.
	sub	eax,edx
	mov	_FS_mainy,eax
;
;Open the window.
;
	push	0
	push	_FS_maind
	push	_FS_mainw
	push	_FS_mainy
	push	_FS_mainx
	push	200
	push	0
	call	WinOpenWindow
	add	esp,4*7
	mov	_FS_window,eax
;
;Do initial draw to make sure everything starts up ok.
;
	mov	edi,eax
	or	ecx,-1
	mov	esi,offset dFileSelector
	call	DrawDialog
	;
	mov	_FS_drawstate,-1	;force everything to be updated.
;
;Loop point when a new path/mask/file list is needed.
;
restartloop:	mov	edx,offset _FS_currentpath ;force current list path.
	call	SetPath
;
;Extract current extension mask from the list.
;
	mov	ecx,_FS_CurrentMask	;current top of list.
	mov	esi,maskptr		;the mask list.
	jecxz	@@3
@@2:	lodsb
	or	al,al		;end of a mask yet?
	jnz	@@2
	dec	ecx		;got to the entry we want yet?
	jnz	@@2
@@3:	mov	edi,offset _FS_filemask	;our storage for the mask.
	call	CopyString
;
;Build a file list based on current file mask and path.
;
	cmp	_FS_FileList,0	;old list in memory?
	jz	@@4
	mov	esi,_FS_FileList
	call	Free		;release the old list.
	mov	_FS_FileList,0
@@4:	mov	edx,offset _FS_filemask
	mov	ecx,16
	call	MakeFileList		;build a new file list.
	or	eax,eax
	jz	memexit
	mov	_FS_FileList,eax
	mov	esi,eax
	call	ULFileList		;convert DIR's to upper etc.
	call	SortFileList		;sort the names.
;
;See if we need to try and center on a previous sub-directory.
;
	cmp	_FS_CenterFile,0	;any name to center on?
	jz	@@11
;
;Try and find the name in the current file list.
;
	mov	ebx,_FS_FileList
	mov	ecx,[ebx]		;get list length.
	add	ebx,4+256		;skip count and path.
	xor	edx,edx		;reset entry counter.
@@12:	mov	edi,ebx		;point to current string.
	mov	esi,offset _FS_CenterFile	;point to string to center on.
@@13:	lodsb
	call	UpperChar
	mov	ah,al
	mov	al,[edi]
	inc	edi
	call	UpperChar
	or	ah,ah		;end of string?
	jz	@@15
	cmp	al,ah
	jz	@@13
@@14:	inc	edx		;update entry counter.
	add	ebx,14		;point to next file.
	loop	@@12
	jmp	@@11		;couldn't find it so use normal method.
@@15:	or	al,al		;end of file list name as well?
	jnz	@@14
;
;Seem to have found what we were looking for so try and center the file list
;around it.
;
	sub	edx,5		;enough entries to center?
	jns	@@16
	xor	edx,edx
@@16:	mov	eax,_FS_FileList
	mov	eax,[eax]		;get number of files.
	mov	ebx,edx
	add	ebx,10
	cmp	ebx,eax		;too close to the bottom of the list?
	jle	@@17
	mov	edx,eax
	sub	edx,10
	jns	@@17
	xor	edx,edx
@@17:	mov	_FS_FirstFile,edx
@@11:	mov	_FS_CenterFile,0
;
;Make sure FirstFile isn't beyond the end of the list.
;
	mov	esi,_FS_FileList
	mov	eax,_FS_FirstFile
	cmp	eax,[esi]
	jc	@@10
	jz	@@10
	xor	eax,eax
@@10:	mov	_FS_FirstFile,eax
;
;Loop point when a re-draw or new file list is needed.
;
drawloop:
;
;Work out where title needs to go and print it.
;
@@ds_1:	test	_FS_drawstate,1	;need to update the title?
	jz	@@ds_2
	mov	ecx,dFILE_TITLE	;use title entry details, prints
	mov	esi,offset dFileSelector	;the text itself.
	mov	edi,_FS_window
	call	DrawDialog		;update title button, text will
	and	_FS_drawstate, not 1	;be printed as well.
;
;Set and display the current path.
;
@@ds_2:	test	_FS_drawstate,2	;need to update path display?
	jz	@@ds_4
	mov	ecx,dFILE_PATH	;use path entry details.
	mov	esi,offset dFileSelector
	call	DialogGetButtonW	;get path button width.
	mov	edx,eax
	xor	ecx,ecx
	mov	esi,offset _FS_currentpath
	mov	edi,offset _FS_printpath
@@ds_2_0:	lodsb			;end of path string yet?
	or	al,al
	jz	@@ds_2_1
	movzx	eax,al
	shl	eax,2
	add	eax,SystemFont
	mov	eax,[eax]		;get pointer to character.
	add	ecx,BM_Wide[eax]	;include this in width calculation.
	cmp	ecx,edx
	jge	@@ds_2_1		;reached our limit so stop here.
	dec	esi
	movsb			;copy this character.
	jmp	@@ds_2_0
@@ds_2_1:	xor	al,al
	stosb			;terminate the string.
	mov	ecx,dFILE_PATH
	mov	esi,offset dFileSelector
	mov	edi,_FS_window
	call	DrawDialog		;re-draw path button, path string
	and	_FS_drawstate,not 2	;will be printed as well.
;
;Display the current file list.
;
@@ds_4:	test	_FS_drawstate,4	;need to re-print file list?
	jz	@@ds_8
	mov	ecx,dFILE_FILES	;use file list button details.
	mov	esi,offset dFileSelector
	mov	edi,_FS_window
	call	DrawDialog		;clear box ready for new print.
	;
	call	DialogGetButtonW	;get width for selected file
	mov	edi,eax		;bar filling.
	sub	edi,4
	call	DialogGetButtonX	;need X co-ord so we know where
	add	eax,2		;to start printing.
	push	eax
	call	DialogGetButtonY	;need Y co-ord so we know where
	add	eax,2		;to start printing.
	mov	ebx,eax
	pop	ecx
	;
	mov	esi,_FS_FileList	;get pointer to current list.
	mov	eax,[esi]		;get number of files in list
	mov	_FS_TotalFiles,eax	;and stow it away for easy access.
	add	esi,256+4		;move to start of name list.
	mov	eax,14
	mul	_FS_FirstFile	;move to current start of list.
	add	esi,eax
	xor	edx,edx		;clear count so far.
@@ds_4_0:	cmp	edx,10
	jz	@@ds_4_1		;reached limit so stop printing.
	mov	eax,edx
	add	eax,_FS_FirstFile	;check if we reached the end of
	cmp	eax,_FS_TotalFiles	;the file list.
	jge	@@ds_4_1
	test	b[esi+13],128	;this a selected file?
	jz	@@ds_4_2
	;
	push	FontDepth
	push	edi
	push	ebx
	push	ecx
	mov	al,251		;convert "system" pen number into
	call	VideoSys2Hard	;real hardware value.
	push	eax
	push	0
	push	_FS_window
	call	WinFillRectangle	;fill area behind this name so
	add	esp,4*7		;we know it's been selected.
	;
@@ds_4_2:	push	ebx
	push	ecx
	push	esi
	push	SystemFont
	mov	al,253		;convert "system" pen number into
	call	VideoSys2Hard	;real hardware value.
	push	eax
	push	2		;OR it on.
	push	_FS_window
	call	WinPrintWindow	;print current file name.
	add	esp,4*7
	add	esi,14
	add	ebx,FontDepth
	inc	edx
	jmp	@@ds_4_0
	;
@@ds_4_1:	and	_FS_drawstate,not 4
;
;Display the file mask list.
;
@@ds_8:	test	_FS_drawstate,8	;mask list need updateing?
	jz	@@ds_16
	mov	ecx,dFILE_TYPES	;use type button details.
	mov	esi,offset dFileSelector
	mov	edi,_FS_window
	call	DrawDialog		;clear type box ready for printing.
	;
	call	DialogGetButtonW	;need width for current selection
	mov	edi,eax
	sub	edi,4
	call	DialogGetButtonX	;need x co-ord so we know where
	add	eax,2		;to start printing.
	push	eax
	call	DialogGetButtonY	;need y co-ord so we know where
	add	eax,2		;to start printing.
	mov	ebx,eax
	pop	ecx
	;
	xor	edx,edx
	mov	esi,maskptr		;point to list of masks.
@@ds_8_0:	cmp	edx,_FS_FirstMask	;found first mask yet?
	jz	@@ds_8_2
@@ds_8_1:	lodsb
	or	al,al		;end of a masl yet?
	jnz	@@ds_8_1
	inc	edx		;update mask number.
	jmp	@@ds_8_0
	;
@@ds_8_2:	xor	edx,edx
@@ds_8_3:	cmp	edx,10		;done max print's?
	jz	@@ds_8_5
	mov	eax,edx
	add	eax,_FS_FirstMask	;check if we reached the end of
	cmp	eax,_FS_TotalMasks	;the mask list.
	jge	@@ds_8_5
	push	esi
	push	edi
	mov	edi,offset _FS_filemask	;our storage for one mask.
	add	esi,2		;skip the *.
	cmp	b[esi],"*"		;check for *
	jnz	@@ds_8_4
	mov	esi,offset _FS_ALL	;replace list mask with "ALL"
@@ds_8_4:	movsb
	cmp	b[esi-1],0
	jnz	@@ds_8_4
	pop	edi
	;
	mov	eax,edx
	add	eax,_FS_FirstMask	;check if we're about to print
	cmp	eax,_FS_CurrentMask	;the currently selected mask.
	jnz	@@ds_8_6
	;
	push	FontDepth		;clear area behind this mask
	push	edi		;so we know its selected.
	push	ebx
	push	ecx
	mov	al,251		;convert "system" pen number into
	call	VideoSys2Hard	;real hardware value.
	push	eax
	push	0
	push	_FS_window
	call	WinFillRectangle
	add	esp,4*7
	;
@@ds_8_6:	push	ebx
	push	ecx		;print current mask.
	push	offset _FS_filemask
	push	SystemFont
	mov	al,253		;convert "system" pen number into
	call	VideoSys2Hard	;real hardware value.
	push	eax
	push	2+16		;OR it on and use fixed spaceing.
	push	_FS_window
	call	WinPrintWindow
	add	esp,4*7
	;
	add	ebx,FontDepth	;fixed spaceing for mask list.
	inc	edx
	pop	esi
@@ds_8_7:	lodsb			;find the end of the mask we just
	or	al,al		;printed.
	jnz	@@ds_8_7
	jmp	@@ds_8_3
	;
@@ds_8_5:	and	_FS_drawstate,not 8
;
;Display the drive list.
;
@@ds_16:	test	_FS_drawstate,16	;need to update the drive list
	jz	@@ds_32		;display?
	mov	ecx,dFILE_DRIVES
	mov	esi,offset dFileSelector
	mov	edi,_FS_window
	call	DrawDialog		;clear drive list box ready for
	;			;printing.
	call	DialogGetButtonW	;need width for current selection
	mov	edi,eax		;highlighting.
	sub	edi,4
	call	DialogGetButtonX	;need x co-ord so we know where
	add	eax,2		;to start printing.
	push	eax
	call	DialogGetButtonY	;need y co-ord so we know where
	add	eax,2		;to start printing.
	mov	ebx,eax
	pop	ecx
	;
	mov	esi,_FS_FirstDrive	;get list start position.
	shl	esi,1		;2 bytes per entry.
	add	esi,offset _FS_drivelist	;offset into the drive list.
	xor	edx,edx
@@ds_16_0:	cmp	edx,10		;done maximum number yet?
	jz	@@ds_16_5
	mov	eax,edx
	add	eax,_FS_FirstDrive	;check if we've reached the end
	cmp	eax,_FS_TotalDrives	;of the drive list.
	jg	@@ds_16_5
	cmp	eax,_FS_CurrentDrive	;this the currently selected drive?
	jnz	@@ds_16_1
	;
	push	FontDepth
	push	edi		;clear area behind current drive
	push	ebx		;so we know it's selected.
	push	ecx
	mov	al,251		;convert "system" pen number into
	call	VideoSys2Hard	;real hardware value.
	push	eax
	push	0
	push	_FS_window
	call	WinFillRectangle
	add	esp,4*7
	;
@@ds_16_1:	push	ebx
	push	ecx
	push	esi		;print current drive.
	push	SystemFont
	mov	al,253		;convert "system" pen number into
	call	VideoSys2Hard	;real hardware value.
	push	eax
	push	2+16		;OR it on and use fixed spaceing.
	push	_FS_window
	call	WinPrintWindow
	add	esp,4*7
	;
	add	esi,2
	add	ebx,FontDepth	;fixed spaceing for drive list.
	inc	edx
	jmp	@@ds_16_0
	;
@@ds_16_5:	and	_FS_drawstate,not 16
;
;Display the file list UP button.
;
@@ds_32:	test	_FS_drawstate,32	;need to update file list up
	jz	@@ds_64		;button?
	mov	ecx,dFILE_FILEUP
	mov	esi,offset dFileSelector
	mov	edi,_FS_window
	call	DrawDialog		;re-draw it then.
	and	_FS_drawstate,not 32
;
;Display the file list DOWN button.
;
@@ds_64:	test	_FS_drawstate,64	;need to update file list down
	jz	@@ds_128		;button?
	mov	ecx,dFILE_FILEDOWN
	mov	esi,offset dFileSelector
	mov	edi,_FS_window
	call	DrawDialog		;re-draw it then.
	and	_FS_drawstate,not 64
;
;Display the type list UP button.
;
@@ds_128:	test	_FS_drawstate,128	;need to update the type list up
	jz	@@ds_256		;button?
	mov	ecx,dFILE_TYPEUP
	mov	esi,offset dFileSelector
	mov	edi,_FS_window
	call	DrawDialog		;re-draw it then.
	and	_FS_drawstate,not 128
;
;Display the type list down button.
;
@@ds_256:	test	_FS_drawstate,256	;need to update the type list down
	jz	@@ds_512		;button?
	mov	ecx,dFILE_TYPEDOWN
	mov	esi,offset dFileSelector
	mov	edi,_FS_window
	call	DrawDialog		;re-draw it then.
	and	_FS_drawstate,not 256
;
;Display the drive list up button.
;
@@ds_512:	test	_FS_drawstate,512	;need to update the drive list up
	jz	@@ds_1024		;button?
	mov	ecx,dFILE_DRIVEUP
	mov	esi,offset dFileSelector
	mov	edi,_FS_window
	call	DrawDialog		;re-draw it then.
	and	_FS_drawstate,not 512
;
;Display the drive list down button.
;
@@ds_1024:	test	_FS_drawstate,1024	;need to update the drive list down
	jz	@@ds_2048		;button?
	mov	ecx,dFILE_DRIVEDOWN
	mov	esi,offset dFileSelector
	mov	edi,_FS_window
	call	DrawDialog		;re-draw it then.
	and	_FS_drawstate,not 1024
;
;Display the OK button.
;
@@ds_2048:	test	_FS_drawstate,2048	;need to update the OK button?
	jz	@@ds_4096
	mov	ecx,dFILE_OK
	mov	esi,offset dFileSelector
	mov	edi,_FS_window
	call	DrawDialog		;re-draw it then.
	and	_FS_drawstate,not 2048
;
;Display the cancel button.
;
@@ds_4096:	test	_FS_drawstate,4096	;need to update the cancel button?
	jz	@@ds_8192
	mov	ecx,dFILE_CANCEL
	mov	esi,offset dFileSelector
	mov	edi,_FS_window
	call	DrawDialog		;re-draw it then.
	and	_FS_drawstate,not 4096
;
;Display current typed name entry.
;
@@ds_8192:	test	_FS_drawstate,8192	;need to update the typed name
	jz	@@ds_16384		;button?
	mov	ecx,dFILE_TYPED
	mov	esi,offset dFileSelector
	mov	edi,_FS_window	;clear typed name button ready
	call	DrawDialog		;for printing.
	call	DialogGetButtonX	;need the x co-ord so we know
	add	eax,2		;where to start printing.
	push	eax
	call	DialogGetButtonY	;need the y co-ord so we know
	add	eax,2		;where to start printing.
	mov	ebx,eax
	pop	ecx
	;
	cmp	_FS_TypedFocus,-1	;is typed name current focus?
	jnz	@@ds_8192_0
	;
	push	FontDepth		;need to put a cursor of sorts
	mov	eax,FontWidth	;by the current position in the
	shr	eax,3		;typed name.
	push	eax
	push	ebx
	mov	eax,_FS_FirstTyped	;current cursor position.
	mul	FontWidth		;fixed spaceing for typed name.
	add	eax,ecx
	dec	eax
	push	eax
	mov	al,1		;convert "system" pen number into
	call	VideoSys2Hard	;real hardware value.
	push	eax
	push	0
	push	_FS_window
	call	WinFillRectangle	;do a fill so we know where we are.
	add	esp,4*7
	;
@@ds_8192_0:	push	ebx
	push	ecx
	push	offset _FS_typedname	;text to print.
	push	SystemFont
	mov	al,253		;convert "system" pen number into
	call	VideoSys2Hard	;real hardware pen number.
	push	eax
	push	2+16		;OR it on and use fixed spaceing.
	push	_FS_window
	call	WinPrintWindow	;print the text.
	add	esp,4*7
	and	_FS_drawstate,not 8192
;
@@ds_16384:	jmp	waitloop2
;
;This is the start of the wait code. First we check if any of the buttons
;should be reset though.
;
waitloop:	mov	esi,offset dFileSelector	;get state of the file list up
	mov	ecx,dFILE_FILEUP	;button.
	call	DialogGetState
	or	eax,eax		;in or out?
	jz	@@wl0
	xor	eax,eax		;make sure file list up button
	call	DialogSetState	;gets reset to out position.
	or	_FS_drawstate,32
	jmp	drawloop
	;
@@wl0:	mov	ecx,dFILE_FILEDOWN	;get state of the file list down
	call	DialogGetState	;button.
	or	eax,eax		;in or out?
	jz	@@wl1
	xor	eax,eax		;make sure file list down button
	call	DialogSetState	;gets reset to out position.
	or	_FS_drawstate,64
	jmp	drawloop
	;
@@wl1:	mov	ecx,dFILE_TYPEUP	;get state of the type list up
	call	DialogGetState	;button.
	or	eax,eax		;in or out?
	jz	@@wl2
	xor	eax,eax		;make sure type list up button
	call	DialogSetState	;gets reset to out position.
	or	_FS_drawstate,128
	jmp	drawloop
	;
@@wl2:	mov	ecx,dFILE_TYPEDOWN	;get state of type list down
	call	DialogGetState	;button.
	or	eax,eax		;in or out?
	jz	@@wl3
	xor	eax,eax		;make sure type list down button
	call	DialogSetState	;gets reset to out position.
	or	_FS_drawstate,256
	jmp	drawloop
	;
@@wl3:	mov	ecx,dFILE_DRIVEUP	;get state of drive list up
	call	DialogGetState	;button.
	or	eax,eax		;in or out?
	jz	@@wl4
	xor	eax,eax		;make sure drive list up button
	call	DialogSetState	;gets reset to out position.
	or	_FS_drawstate,512
	jmp	drawloop
	;
@@wl4:	mov	ecx,dFILE_DRIVEDOWN	;get state of drive list down
	call	DialogGetState	;button.
	or	eax,eax		;in or out?
	jz	@@wl5
	xor	eax,eax		;make sure drive list down button
	call	DialogSetState	;gets reset to out position.
	or	_FS_drawstate,1024
	jmp	drawloop
@@wl5:
;
;Wait for some user action here.
;
waitloop2:	call	_FS_Readinput	;read keyboard and mouse states.
	cmp	_FS_KeyCode,0	;anything at the keyboard?
	jz	@@wl2_0
	cmp	_FS_KeyCode,1	;check for escape.
	jz	cancelexit		;pretend they pressed CANCEL.
	cmp	_FS_KeyASCII,13	;check for ENTER.
	jz	okexit		;pretend they pressed OK.
	cmp	_FS_KeyCode,71	;check for HOME
	jz	HomeKeyed
	cmp	_FS_KeyCode,79	;check for END
	jz	EndKeyed
	cmp	_FS_KeyCode,73	;check for PgUp
	jz	PgUpKeyed
	cmp	_FS_KeyCode,81	;check for PgDn
	jz	PgDnKeyed
	cmp	_FS_TypedFocus,0	;should we be passing this to
	jnz	typedinput		;the typed name handler?
	;
@@wl2_0:	cmp	_FS_mouseb,0		;anything interesting happening
	jz	waitloop		;with the mouse?
;
;Make sure pointer is in the right windows.
;
	mov	eax,_FS_window	;must make sure the mouse pointer
	cmp	eax,_FS_mousewin	;is inside our window before we
	jnz	@@wl2_1		;try doing anything with the values.
	mov	esi,offset dFileSelector
	mov	ecx,_FS_mousewinx	;Mouse is in our window so now
	mov	edx,_FS_mousewiny	;we need to know which, if any,
	call	DialogWhich		;dialog button is selected.
	cmp	eax,-1
	jz	@@wl2_1		;nothing useful selected.
;
;Check for cancel button.
;
	cmp	eax,dFILE_CANCEL
	jz	cancelexit
;
;Check for OK button.
;
	cmp	eax,dFILE_OK
	jz	okexit
;
;Check for PATH selection.
;
	cmp	eax,dFILE_PATH
	jz	pathchange
;
;Check for file or directory selection.
;
	cmp	eax,dFILE_FILES
	jz	fileordir
;
;Check for type selection.
;
	cmp	eax,dFILE_TYPES
	jz	typechange
;
;Check for drive selection.
;
	cmp	eax,dFILE_DRIVES
	jz	drivechange
;
;Check for typed name becomeing the focus.
;
	cmp	eax,dFILE_TYPED
	jz	changetyped
;
;Check for file list UP button.
;
	cmp	eax,dFILE_FILEUP
	jz	fileup
;
;Check for file list DOWN button.
;
	cmp	eax,dFILE_FILEDOWN
	jz	filedown
;
;Check for type list UP button.
;
	cmp	eax,dFILE_TYPEUP
	jz	typeup
;
;Check for type list DOWN button.
;
	cmp	eax,dFILE_TYPEDOWN
	jz	typedown
;
;Check for drive list UP button.
;
	cmp	eax,dFILE_DRIVEUP
	jz	driveup
;
;Check for drive list DOWN button.
;
	cmp	eax,dFILE_DRIVEDOWN
	jz	drivedown
;
;Selection was invalid for some reason so go back to waiting.
;
@@wl2_1:	jmp	waitloop
;
;File or directory selected so deal with it.
;
fileordir:	mov	esi,offset dFileSelector	;need file list y co-ord so we
	mov	ecx,dFILE_FILES	;can try and work out which file
	call	DialogGetButtonY	;is being selected.
	sub	eax,_FS_mousewiny	;get button relative y position.
	neg	eax
	xor	edx,edx		;use current font depth to work
	div	FontDepth		;out which entry it is.
	add	eax,_FS_FirstFile	;make sure start point is taken account of.
	cmp	eax,_FS_TotalFiles	;beyond the end of the list?
	jnc	@@fod0
	mov	ebx,14		;length of each entry in the list.
	mul	ebx
	add	eax,256+4		;move past count and path.
	mov	edi,eax
	add	edi,_FS_FileList	;offset into file list.
;
;check for selecting a directory.
;
	test	b[edi+13],16		;dir attribute set?
	jz	@@fod1
	push	edi
@@fod2:	call	_FS_ReadInput	;wait for user to let go of the
	cmp	_FS_mouseb,0		;mouse button.
	jnz	@@fod2
	pop	edi
	mov	esi,offset _FS_currentpath ;need to find the end of the
@@fod3:	lodsb			;current path string.
	or	al,al
	jnz	@@fod3
	dec	esi		;check for adding to just a drive
	cmp	b[esi-2],":"		;spec.
	jnz	@@fod4
	dec	esi		;bodge it so \ falls in the right place.
@@fod4:	mov	b[esi],"\"
	mov	b[esi+1],0
	mov	esi,edi		;now add selected dir to end of
	mov	edi,offset _FS_currentpath ;current path.
	call	AppendString
	test	flags,2
	jnz	@@fod99
	mov	b[_FS_typedname],0
	mov	b[_FS_typedname+13],0
	mov	_FS_TypedTotal,0
	mov	_FS_FirstTyped,0
	mov	_FS_TypedFocus,0
@@fod99:	mov	_FS_drawstate,2+4+8192
	mov	_FS_FirstFile,0
	jmp	restartloop
;
;must be selecting a file so see what we want to do with it.
;
@@fod1:	test	_FS_mouseb,8		;Double click?
	jnz	okexit
	;
	test	flags,1		;check if multiple selections
	jz	@@fod10		;are allowed.
	movzx	eax,_FS_KeyFlags	;if shift key is down we don't
	and	eax,SK_SHIFTL+SK_SHIFTR	;want to clear current selections.
	jz	@@fod5
;
;Shift key is down so add this file.
;
	mov	b[_FS_typedname],0	;can't have a typed name while
	mov	b[_FS_typedname+13],0	;multiple selections are in
	mov	_FS_FirstTyped,0	;action.
	mov	_FS_TypedFocus,0
	mov	_FS_TypedTotal,0
	xor	b[edi+13],128	;mark selected file.
	mov	_FS_drawstate,4+8192
@@fod6:	call	_FS_ReadInput	;now wait for user to relase
	cmp	_FS_mouseb,0		;the mouse button.
	jnz	@@fod6
	jmp	drawloop
;
;See if CTRL or ALT is being held down.
;
@@fod5:	movzx	eax,_FS_KeyFlags	;if shift key is down we don't
	and	eax,SK_CTRL+SK_ALT	;want to clear current selections.
	jz	@@fod10
	mov	b[_FS_typedname],0	;can't have a typed name while
	mov	b[_FS_typedname+13],0	;multiple selections are in
	mov	_FS_FirstTyped,0	;action.
	mov	_FS_TypedFocus,0
	mov	_FS_TypedTotal,0
	;
	mov	esi,_FS_FileList
	add	esi,4+256
@@fod12:	test	b[edi+13],128
	jnz	@@fod13
	test	b[edi+13],16
	jnz	@@fod12_0
	or	b[edi+13],128	;set selected file.
@@fod12_0:	sub	edi,14
	cmp	edi,esi
	jnc	@@fod12
@@fod13:	;
	mov	_FS_drawstate,4+8192
@@fod11:	call	_FS_ReadInput	;now wait for user to relase
	cmp	_FS_mouseb,0		;the mouse button.
	jnz	@@fod11
	jmp	drawloop
;
;No shift key so clear out previous settings.
;
@@fod10:	mov	esi,_FS_FileList	;point to file list.
	mov	ecx,[esi]		;get number of entries.
	add	esi,4+256		;skip count and path.
@@fod7:	and	b[esi+13],63		;clear attributes.
	add	esi,14
	loop	@@fod7
	or	b[edi+13],128	;mark current selection.
	mov	esi,edi
	mov	edi,offset _FS_typedname	;make sure typed name gets a
	call	CopyString		;copy of this.
	mov	al,[esi+13]
	mov	b[_FS_typedname+13],al	;copy attributes.
	mov	esi,offset _FS_typedname
	call	LenString
	mov	_FS_TypedTotal,eax	;need to know how long it is.
	mov	_FS_FirstTyped,0
	mov	_FS_TypedFocus,0
	mov	_FS_drawstate,4+8192
@@fod9:	call	_FS_ReadInput	;wait for user to release mouse
	cmp	_FS_mouseb,0		;button.
	jnz	@@fod9
	jmp	drawloop
;
;Not valid for this section so ignore it.
;
@@fod0:	call	_FS_ReadInput	;wait for user to release mouse
	cmp	_FS_mouseb,0		;button.
	jnz	@@fod0
	jmp	waitloop
;
;TYPED name is selected so make it the focus.
;
changetyped:	call	_FS_ReadInput	;wait for user to let go of mouse
	cmp	_FS_mouseb,0		;button.
	jnz	changetyped
	test	flags,2		;typed name allowed?
	jz	waitloop
	mov	esi,_FS_FileList	;point to file list.
	mov	ecx,[esi]		;get number of entries.
	add	esi,4+256		;skip count and path.
	jecxz	@@ct0_0
@@ct0:	and	b[esi+13],63		;make sure all selections are
	add	esi,14		;cleared while we're useing a
	loop	@@ct0		;typed name.
@@ct0_0:	;
	mov	ecx,dFILE_FILES	;need typed name X co-ord so
	mov	esi,offset dFileSelector	;we can work out where the user
	call	DialogGetButtonX	;is selecting.
	sub	eax,_FS_mousewinx
	neg	eax
	xor	edx,edx		;use font width to work out the
	div	FontWidth		;character offset.
	cmp	eax,_FS_TypedTotal	;beyond length of typed name?
	jc	@@ct1
	mov	eax,_FS_TypedTotal	;force to end of string.
@@ct1:	mov	_FS_FirstTyped,eax	;store new position.
	mov	_FS_drawstate,8192	;make sure we update cursor.
	cmp	_FS_TypedFocus,-1	;already the focus?
	jz	@@ct2
	or	_FS_drawstate,4	;make sure the file list gets
@@ct2:	or	_FS_TypedFocus,-1	;a re-draw.
	jmp	drawloop
;
;Stuff being typed so add it to the typed name.
;
typedinput:
;
;See if its a normal alpha-numeric.
;
	cmp	_FS_KeyASCII,32+1	;check for above space.
	jc	@@ti0
	cmp	_FS_KeyASCII,122+1	;how about little z?
	jnc	@@ti0
	cmp	_FS_TypedTotal,12	;too many characters typed?
	jnc	@@ti0
	mov	esi,offset _FS_typedname	;point to typed name buffer and
	add	esi,_FS_TypedTotal	;use current total to point to
	mov	edi,esi		;the end ready to copy everything
	dec	esi		;up a byte.
	mov	ecx,_FS_TypedTotal
	sub	ecx,_FS_FirstTyped
	std
	rep	movsb		;move em all up.
	cld
	mov	al,_FS_KeyASCII	;get the typed character again
	stosb			;and store it in the buffer.
	inc	_FS_TypedTotal	;update length of name and current
	inc	_FS_FirstTyped	;cursor position.
	mov	_FS_drawstate,8192	;make sure it gets a re-draw.
	jmp	drawloop
;
;See if its DEL.
;
@@ti0:	cmp	_FS_KeyASCII,0	;check for an ASCII code.
	jnz	@@ti1
	cmp	_FS_KeyCode,83	;check the hardware key code.
	jnz	@@ti1
	mov	eax,_FS_FirstTyped	;check we're not already at the
	cmp	eax,_FS_TypedTotal	;start of the buffer.
	jz	@@ti1
	mov	esi,offset _FS_typedname	;point to name buffer and use
	add	esi,_FS_FirstTyped	;current position to work out
	mov	edi,esi		;where we need to move everything
	inc	esi		;down from.
	mov	ecx,_FS_TypedTotal
	sub	ecx,_FS_FirstTyped
	rep	movsb		;copy it all down.
	dec	_FS_TypedTotal	;reduce length variable.
	mov	_FS_drawstate,8192	;make sure it gets a re-draw.
	jmp	drawloop
;
;See if its BACKSPACE.
;
@@ti1:	cmp	_FS_KeyASCII,8	;check for the right ASCII code.
	jnz	@@ti2
	cmp	_FS_FirstTyped,0	;anything at the current position
	jz	@@ti2		;to delete?
	mov	esi,offset _FS_typedname	;point to the buffer and use the
	add	esi,_FS_FirstTyped	;current cursor position to work
	mov	edi,esi		;out where we need to move
	dec	edi		;everything down from.
	mov	ecx,_FS_TypedTotal
	sub	ecx,_FS_FirstTyped
	inc	ecx
	rep	movsb		;move it all down.
	dec	_FS_TypedTotal	;reduce total character count and
	dec	_FS_FirstTyped	;cursor position.
	mov	_FS_drawstate,8192	;make sure it gets a re-draw.
	jmp	drawloop
;
;See if its cursor left.
;
@@ti2:	cmp	_FS_KeyCode,75	;cursor left key?
	jnz	@@ti3
	cmp	_FS_FirstTyped,0	;already at left edge?
	jz	waitloop
	dec	_FS_FirstTyped	;update cursor position and make
	mov	_FS_drawstate,8192	;sure it gets a re-draw.
	jmp	drawloop
;
;See if its cursor right.
;
@@ti3:	cmp	_FS_KeyCode,77	;cursor right key?
	jnz	@@ti4
	mov	eax,_FS_FirstTyped	;check we're not already at the
	cmp	eax,_FS_TypedTotal	;right edge.
	jnc	waitloop
	inc	_FS_FirstTyped	;update cursor position and make
	mov	_FS_drawstate,8192	;sure it gets a re-draw.
	jmp	drawloop
;
;Nothing we recognise.
;
@@ti4:	jmp	waitloop		;not a recognised key press.
;
;PATH is selected so move back a dir.
;
pathchange:	call	_FS_ReadInput 	;wait for the user to relase
	cmp	_FS_mouseb,0		;the mouse button.
	jnz	pathchange
	mov	esi,offset _FS_currentpath ;need to find the last section
@@pc0:	lodsb			;of the current path string so
	or	al,al		;we can chop it off. Find end
	jnz	@@pc0		;of the string first.
@@pc1:	dec	esi		;now work backwards in string and
	cmp	b[esi],"\"		;check for dir seperator.
	jnz	@@pc1
	cmp	b[esi-1],":"		;check for drive spec.
	jnz	@@pc2
	inc	esi
@@pc2:	push	esi
	cmp	b[esi-2],":"
	jz	@@pc3
	inc	esi
@@pc3:	mov	edi,offset _FS_CenterFile	;we're loseing into centering
	call	CopyString		;string.
	pop	esi
	mov	b[esi],0		;lose last piece of path string.
	test	flags,2
	jnz	@@pc4
	mov	b[_FS_typedname],0	;make sure typed name gets cleared
	mov	b[_FS_typedname+13],0	;out and reset its variables.
	mov	_FS_TypedTotal,0
	mov	_FS_FirstTyped,0
@@pc4:	mov	_FS_drawstate,2+4+8192	;make sure it gets a re-draw.
	mov	_FS_FirstFile,0
	jmp	restartloop
;
;TYPE is selected so set new type.
;
typechange:	call	_FS_ReadInput	;wait for the user to release
	cmp	_FS_mouseb,0		;the mouse button.
	jnz	typechange
	mov	ecx,dFILE_TYPES	;need type list y co-ord so we
	mov	esi,offset dFileSelector	;can try and work out which entry
	call	DialogGetButtonY	;is being selected. Start off with
	sub	eax,_FS_mousewiny	;a pixel offset for now.
	neg	eax
	xor	edx,edx		;use font depth to work out the
	div	FontDepth		;entry number and make sure the
	add	eax,_FS_FirstMask	;selection isn't past the end of
	cmp	eax,_FS_TotalMasks	;the list.
	jnc	waitloop
	cmp	eax,_FS_CurrentMask	;same as we're already useing?
	jz	waitloop
	mov	_FS_CurrentMask,eax	;set new mask number.
	mov	_FS_drawstate,2+4+8+8192	;make sure it gets a re-draw.
	;
	;Change typed names extension.
	;
	test	flags,2
	jz	@@tc9
	mov	esi,offset _FS_TypedName
	or	edi,-1
@@tc0:	mov	al,[esi]
	inc	esi
	cmp	al,"."
	jnz	@@tc1
	mov	edi,esi
@@tc1:	cmp	al,"\"
	jnz	@@tc2
	or	edi,-1
@@tc2:	or	al,al
	jnz	@@tc0
	cmp	edi,-1		;any extension?
	jnz	@@tc6
	mov	edi,esi
	mov	b[edi-1],"."
@@tc6:	mov	ecx,_FS_CurrentMask	;current top of list.
	mov	esi,maskptr		;the mask list.
	jecxz	@@tc4
@@tc3:	lodsb
	or	al,al		;end of a mask yet?
	jnz	@@tc3
	dec	ecx		;got to the entry we want yet?
	jnz	@@tc3
@@tc4:	lodsb
	cmp	al,"."
	jnz	@@tc4
	cmp	b[esi],"*"		;this an ALL job?
	jz	@@tc5
	call	CopyString
@@tc5:	jmp	restartloop
	;
@@tc9:	mov	b[_FS_typedname],0	;make sure the typed name is
	mov	b[_FS_typedname+13],0	;cleared and doesn't have the
	mov	_FS_FirstTyped,0	;focus anymore.
	mov	_FS_TypedFocus,0
	mov	_FS_TypedTotal,0
	jmp	restartloop
;
;DRIVE is selected so set new drive.
;
drivechange:	call	_FS_ReadInput	;wait for the user to let go
	cmp	_FS_mouseb,0		;of the mouse button.
	jnz	drivechange
	mov	ecx,dFILE_DRIVES	;need drive list y co-ord so
	mov	esi,offset dFileSelector	;we can work out the pixel
	call	DialogGetButtonY	;offset into the list selected.
	sub	eax,_FS_mousewiny
	neg	eax
	xor	edx,edx		;use font depth to work out entry
	div	FontDepth		;number being selected and make
	add	eax,_FS_FirstDrive	;sure it's not beyonf the end of
	cmp	eax,_FS_TotalDrives	;the list.
	jnc	waitloop
	cmp	eax,_FS_CurrentDrive	;same as we're already useing?
	jz	waitloop
	mov	_FS_CurrentDrive,eax	;set new drive entry number
	mov	al,b[_FS_drivelist+eax*2]
	sub	al,"A"
	call	SetDrive		;let DOS know what we want.
	mov	edx,offset _FS_currentpath ;get path for selected drive.
	call	GetPath
	mov	_FS_drawstate,2+4+16+8192 ;make sure it gets a re-draw.
	test	flags,2
	jnz	@@drvc0
	mov	b[_FS_typedname],0
	mov	b[_FS_typedname+13],0	;make sure typed file name is
	mov	_FS_FirstTyped,0	;cleared and loses focus if it
	mov	_FS_TypedFocus,0	;had it.
	mov	_FS_TypedTotal,0
@@drvc0:	mov	_FS_FirstFile,0
	jmp	restartloop
;
;Home key was pressed.
;
HomeKeyed:	cmp	_FS_FirstFile,0	;check if we're already at the
	jz	waitloop		;top of the list.
	mov	_FS_FirstFile,0	;move us to the top.
	mov	_FS_drawstate,4	;and make sure it gets a re-draw.
	jmp	drawloop
;
;End key was pressed.
;
EndKeyed:	mov	eax,_FS_FirstFile	;see if we reached the end of the
	add	eax,10		;file list yet.
	cmp	eax,_FS_TotalFiles
	jnc	waitloop
	mov	eax,_FS_TotalFiles
	sub	eax,10
	jns	@@ek0
	xor	eax,eax
@@ek0:	mov	_FS_FirstFile,eax
	mov	_FS_drawstate,4	;make sure list gets a re-draw.
	jmp	drawloop
;
;PgUp key was pressed.
;
PgUpKeyed:	cmp	_FS_FirstFile,0	;check if we're already at the
	jz	waitloop		;top of the list.
	mov	eax,_FS_FirstFile
	sub	eax,10
	js	HomeKeyed
	mov	_FS_FirstFile,eax
	mov	_FS_drawstate,4	;make sure list gets a re-draw.
	jmp	drawloop
;
;PgDn key was pressed.
;
PgDnKeyed:	mov	eax,_FS_FirstFile	;see if we reached the end of the
	add	eax,10		;file list yet.
	cmp	eax,_FS_TotalFiles
	jnc	waitloop
	add	eax,10
	cmp	eax,_FS_TotalFiles
	jnc	EndKeyed
	sub	eax,10
	mov	_FS_FirstFile,eax
	mov	_FS_drawstate,4	;make sure list gets a re-draw.
	jmp	drawloop
;
;File list UP button pressed so deal with it.
;
fileup:	cmp	_FS_FirstFile,0	;check if we're already at the
	jz	waitloop		;top of the list.
	dec	_FS_FirstFile	;update list start entry number
	mov	_FS_drawstate,4	;and make sure it gets a re-draw.
	mov	ecx,dFILE_FILEUP
	mov	esi,offset dFileSelector	;get buttons current state so
	call	DialogGetState	;we know if it should be re-drawn.
	or	eax,eax
	jnz	@@fu1
	inc	eax		;set button to in position and
	call	DialogSetState	;force a re-draw.
	or	_FS_drawstate,32
@@fu1:	mov	ecx,1		;wait for a bit so the list doesn't
	call	Delay		;go past to quickly.
	jmp	drawloop
;
;File list DOWN button pressed so deal with it.
;
filedown:	mov	eax,_FS_FirstFile	;see if we reached the end of the
	add	eax,10		;file list yet.
	cmp	eax,_FS_TotalFiles
	jnc	waitloop
	inc	_FS_FirstFile	;update start file number and
	mov	_FS_drawstate,4	;make sure list gets a re-draw.
	inc	eax
	mov	ecx,dFILE_FILEDOWN
	mov	esi,offset dFileSelector	;get button state so we know if
	call	DialogGetState	;it needs pushing in.
	or	eax,eax
	jnz	@@fd1
	inc	eax		;put button in the in position
	call	DialogSetState	;and force a re-draw.
	or	_FS_drawstate,64
@@fd1:	mov	ecx,1		;wait for a bit so the list doesn't
	call	Delay		;go past to quickly.
	jmp	drawloop
;
;Type list UP button pressed so deal with it.
;
typeup:	cmp	_FS_FirstMask,0	;check we're not already at
	jz	waitloop		;the top of the list.
	dec	_FS_FirstMask	;update start position.
	mov	_FS_drawstate,8	;make sure type list is updated.
	mov	ecx,dFILE_TYPEUP
	mov	esi,offset dFileSelector	;check if button is already in
	call	DialogGetState	;the in position.
	or	eax,eax
	jnz	@@tu1
	inc	eax		;put button in the in position
	call	DialogSetState	;and force a re-draw.
	or	_FS_drawstate,128
@@tu1:	mov	ecx,1		;wait for a bit so the list doesn't
	call	Delay		;go past to quickly.
	jmp	drawloop
;
;Type list DOWN button pressed so deal with it.
;
typedown:	mov	eax,_FS_FirstMask	;make sure we're not already at
	add	eax,10		;the end of the list.
	cmp	eax,_FS_TotalMasks
	jnc	waitloop
	inc	_FS_FirstMask	;update start position and make
	mov	_FS_drawstate,8	;sure list gets a re-draw.
	inc	eax
	mov	ecx,dFILE_TYPEDOWN
	mov	esi,offset dFileSelector	;now see if button is already in
	call	DialogGetState	;the in position.
	or	eax,eax
	jnz	@@td1
	inc	eax		;put button in the in position and
	call	DialogSetState	;force a re-draw.
	mov	_FS_drawstate,8+256
@@td1:	mov	ecx,2		;wait for a bit so the list doesn't
	call	Delay		;go past to quickly.
	jmp	drawloop
;
;Drive list UP button pressed so deal with it.
;
driveup:	cmp	_FS_FirstDrive,0	;check we're not already at the
	jz	waitloop		;top of the list.
	dec	_FS_FirstDrive	;update start position and make
	mov	_FS_drawstate,16	;sure list gets a re-draw.
	mov	ecx,dFILE_DRIVEUP
	mov	esi,offset dFileSelector	;now see if the button is already
	call	DialogGetState	;in the in position.
	or	eax,eax
	jnz	@@du1
	inc	eax		;put button in the in position and
	call	DialogSetState	;force a re-draw.
	mov	_FS_drawstate,16+512
@@du1:	mov	ecx,2		;wait for a bit so the list doesn't
	call	Delay		;go past to quickly.
	jmp	drawloop
;
;Drive list DOWN button pressed so deal with it.
;
drivedown:	mov	eax,_FS_FirstDrive	;check if we've already reached
	add	eax,10		;the end of the list.
	cmp	eax,_FS_TotalDrives
	jnc	waitloop
	inc	_FS_FirstDrive	;update start position and make
	mov	_FS_drawstate,16	;sure the list gets a re-draw.
	mov	ecx,dFILE_DRIVEDOWN
	mov	esi,offset dFileSelector	;now see if the button is already
	call	DialogGetState	;in the in position.
	or	eax,eax
	jnz	@@dd1
	inc	eax		;put button in the in position and
	call	DialogSetState	;force a re-draw.
	mov	_FS_drawstate,8+1024
@@dd1:	mov	ecx,2		;wait for a bit so the list doesn't
	call	Delay		;go past to quickly.
	jmp	drawloop
;
;
; OK button selected so return selected files to caller.
;
;
okexit:
;
;Build a selected file list from either the typed name or the current list.
;
	cmp	_FS_TypedFocus,0	;is the typed name the current
	jz	@@oe0		;focus?
	mov	esi,_FS_FileList	;Any file names in the list?
	cmp	d[esi],0
	jnz	@@oe1
	mov	ecx,4+256+14		;make sure at least one names
	call	ReMalloc		;worth of space in the list
	jc	memexit		;buffer.
	mov	_FS_FileList,esi
	inc	d[esi]		;update number of entries.
@@oe1:	;
	add	esi,4+256		;skip past count and path.
	mov	edi,esi
	mov	esi,offset _FS_typedname	;copy typed name into the file
	call	CopyString		;list buffer.
	mov	al,[esi+13]
	mov	[edi+13],al		;copy attributes and make sure
	or	b[edi+13],128	;selected bit is set.
@@oe0:	;
	mov	esi,_FS_FileList	;point to file list.
	mov	ecx,[esi]		;get number of entries.
	add	esi,4+256		;skip count and path.
	xor	edx,edx		;reset counter.
@@oe2:	test	b[esi+13],128	;this a selected entry?
	jz	@@oe3
	inc	edx		;update counter.
@@oe3:	add	esi,14		;next file name.
	dec	ecx
	jnz	@@oe2
	;
	or	edx,edx		;if no file names in the list
	jz	nofileexit		;then pretend CANCEL pressed.
	;
	push	edx
	mov	esi,_FS_FileList	;point to file list again.
	add	esi,4+256		;skip count and path.
@@oe4:	or	edx,edx
	jz	@@oe5
@@oe6:	test	b[esi+13],128	;this a selected file?
	jnz	@@oe7
	add	esi,14		;next file.
	jmp	@@oe6
@@oe7:	and	b[esi+13],not 128	;clear selected bit so this entry
	mov	edi,_FS_FileList	;can be found in search.
	add	edi,4+256
@@oe8:	test	b[edi+13],128	;find an entry that's not selected.
	jz	@@oe9
	add	edi,14
	jmp	@@oe8
@@oe9:	call	CopyString		;copy current name into new position.
	mov	al,[esi+13]
	or	al,128		;copy attrivutes and make sure
	mov	[edi+13],al		;selection bit is set.
	add	esi,14
	dec	edx
	jmp	@@oe4
@@oe5:	;
	pop	ebx		;get number of selected entries
	mov	eax,14		;and work out how big the buffer
	mul	ebx		;needs to be now.
	add	eax,4+256		;allow for count and path.
	mov	ecx,eax
	mov	esi,_FS_FileList	;current address.
	call	ReMalloc		;try and re-size it, should be
	jc	memexit		;shrinking so it's not likely
	mov	_FS_FileList,esi	;to fail.
	mov	[esi],ebx		;store new count value.
;
;Show the button down
;
	mov	ecx,dFILE_OK		;set OK button to the in position
	mov	esi,offset dFileSelector	;so they know we reacted to the
	mov	eax,1		;selection.
	call	DialogSetState
	mov	edi,_FS_window	;make sure the button gets a
	call	DrawDialog		;re-draw.
;
@@oe10:	call	_FS_ReadInput	;now wait for them to let go of
	cmp	_FS_mouseb,0		;the mouse button.
	jnz	@@oe10
;
;show the button up.
;
	mov	ecx,dFILE_OK		;set OK button to the out position
	mov	esi,offset dFileSelector	;to complete the effect of something
	xor	eax,eax		;happening.
	call	DialogSetState
	mov	edi,_FS_window	;make sure the button get a
	call	DrawDialog		;re-draw.
;
@@oe11:	mov	ecx,2		;now wait a while so the button
	call	Delay		;shows up as being out before
	mov	eax,_FS_window	;it all disapears.
	call	WinCloseWindow	;close the window.
	;
	popad
	xor	eax,eax
	mov	ebx,_FS_FirstFile	;file list start position.
	mov	ecx,_FS_FirstMask	;mask list start position.
	mov	edx,_FS_CurrentMask	;current mask number.
	mov	esi,_FS_FirstDrive	;drive list start position.
	mov	edi,_FS_FileList	;return the file list address.
	clc
	ret
;
;
; CANCEL selected so don't pass a file name back.
;
;
cancelexit:	mov	ecx,dFILE_CANCEL	;set CANCEL button to the in
	mov	esi,offset dFileSelector	;position so they know we
	mov	eax,1		;reacted to the action.
	call	DialogSetState
	mov	edi,_FS_window	;make sure the button get a
	call	DrawDialog		;re-draw.
	;
@@ce0:	call	_FS_ReadInput
	cmp	_FS_mouseb,0
	jnz	@@ce0
	;
	mov	ecx,dFILE_CANCEL	;set button to the out position
	mov	esi,offset dFileSelector	;to complete the effect of 
	xor	eax,eax		;something happening.
	call	DialogSetState
	mov	edi,_FS_window	;make sure the button get a
	call	DrawDialog		;re-draw.
	;
@@ce1:	mov	ecx,2		;brief delay to make sure they
	call	Delay		;see the button out again.
	mov	_FS_ErrorCode,1
RealExit:	mov	eax,_FS_window
	call	WinCloseWindow	;close the window.
	cmp	_FS_FileList,0
	jz	@@ce2
	mov	esi,_FS_FileList	;release the file list.
	call	Free
@@ce2:	popad
	mov	eax,_FS_ErrorCode	;return a null pointer.
	stc
	ret
;
;Not enough memory for file list.
;
memexit:	mov	_FS_ErrorCode,3
	jmp	RealExit
;
;No file selected.
;
nofileexit:	jmp	cancelexit
	mov	_FS_ErrorCode,2
	jmp	RealExit
_dlgFileSelector_ endp


;-------------------------------------------------------------------------
;
;Read the keyboard and mouse.
;
_FS_ReadInput	proc	near
	call	GetKey		;read the keyboard state.
	mov	_FS_KeyASCII,al
	mov	_FS_KeyCode,ah
	shr	eax,16
	mov	_FS_KeyFlags,ax
	call	MouseGet		;read the mouse state.
	mov	_FS_mouseb,ebx
	mov	_FS_mousex,ecx
	mov	_FS_mousey,edx
	call	WinWhichWindow	;find out which (if any) window
	mov	_FS_mousewin,eax	;the mouse is currenty in.
	mov	_FS_mousewinx,ecx
	mov	_FS_mousewiny,edx
	ret
_FS_ReadInput	endp


	sdata
;
_FS_mousewin	dd ?
_FS_mousewinx	dd ?
_FS_mousewiny	dd ?
_FS_mouseb	dd ?
_FS_mousex	dd ?
_FS_mousey	dd ?
_FS_KeyASCII	db ?
_FS_KeyCode	db ?
_FS_KeyFlags	dw ?
_FS_window	dd ?
_FS_mainw	dd ?
_FS_maind	dd ?
_FS_mainx	dd ?
_FS_mainy	dd ?
_FS_i	dd ?
_FS_j	dd ?
_FS_k	dd ?
_FS_m	dd ?
_FS_p	dd ?
_FS_bmptr	dd ?
_FS_charptr	dd ?
_FS_charptr2	dd ?
_FS_intptr	dd ?
_FS_currentpath db 256 dup (?)
_FS_printpath	db 128 dup (?)
_FS_filemask	db 6 dup (?)
_FS_drivelist	db 64 dup (?)
_FS_typedname	db 16 dup (?)
_FS_TotalMasks	dd ?
_FS_CurrentMask dd ?
_FS_FirstMask	dd ?
_FS_TotalFiles	dd ?
_FS_FirstFile	dd ?
_FS_TotalDrives dd ?
_FS_CurrentDrive dd ?
_FS_FirstDrive	dd ?
_FS_FileList	dd ?
_FS_drawstate	dd ?
_FS_action	dd ?
_FS_FirstTyped	dd ?
_FS_TypedFocus	dd ?
_FS_TypedTotal	dd ?
_FS_ALL	db "ALL",0
_FS_CenterFile	db 14 dup (?)
_FS_ErrorCode	dd ?
;
dFILE_TITLE	equ	0
dFILE_PATH	equ	1
dFILE_FILES	equ	2
dFILE_TYPED	equ	3
dFILE_FILEUP	equ	4
dFILE_FILEDOWN	equ	5
dFILE_TYPES	equ	6
dFILE_TYPEUP	equ	7
dFILE_TYPEDOWN	equ	8
dFILE_DRIVES	equ	9
dFILE_DRIVEUP	equ	10
dFILE_DRIVEDOWN equ	11
dFILE_OK	equ	12
dFILE_CANCEL	equ	13
;
dFileSelector	label byte
	DlgMain <22,23, 13,25>
	Dlg <0, 0,3, 0,3, 22,17, 1,4, (9*256)+1,0>	; title.
	Dlg <2, 0,3, 1,8, 22,17, 1,4, (253*256),0>	; path.
	Dlg <2, 0,3, 2,13, 12,4, 10,4, 0,0>	; file list.
	Dlg <2, 0,3, 12,18, 12,4, 1,4, 0,0>	; typed name.
	Dlg <0, 12,8, 2,13, 2,0, 5,2, (9*256)+1,_FS_Up> ; file list UP
	Dlg <0, 12,8, 7,15, 2,0, 5,2, (9*256)+8+1,_FS_Down> ; file list DOWN.
	Dlg <2, 14,9, 2,13, 3,4, 10,4, 0,0>	; type list.
	Dlg <0, 17,14, 2,13, 2,0, 5,2, (9*256)+1,_FS_Up> ; type list UP
	Dlg <0, 17,14, 7,15, 2,0, 5,2, (9*256)+8+1,_FS_Down> ; type list DOWN.
	Dlg <2, 19,15, 2,13, 1,4, 10,4, 0,0>	; drive list.
	Dlg <0, 20,20, 2,13, 2,0, 5,2, (9*256)+1,_FS_Up> ; drive list UP
	Dlg <0, 20,20, 7,15, 2,0, 5,2, (9*256)+8+1,_FS_Down> ; drive list DOWN.
	Dlg <0, 12,8, 12,18, 5,6, 1,4, (9*256)+1+8,_FS_Ok> ; OK button.
	Dlg <0, 17,14, 12,18, 5,6, 1,4, (9*256)+1+8,_FS_Cancel> ; CANCEL button.
	Dlg <-1>
;
_FS_Ok	db "OK",0
_FS_Cancel	db "CANCEL",0
_FS_Up	db 2,3,0
_FS_Down	db 4,5,0
;

	efile
	end

