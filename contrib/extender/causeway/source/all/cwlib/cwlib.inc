;------------------------------------------------------------------------------
;Some simple macro's to let us use either "model small" or "near" with MASM or
;TASM.
;------------------------------------------------------------------------------

	ifdef TASM				;declare the memory model if it's
	locals @@				;needed at this point.
	jumps
	else
	option oldstructs
	option proc:private
	endif
	.386
	.model small

scode	macro
	.code
	endm

sdata	macro
	.data
	endm

efile	macro
	endm

extdef	macro var				;Declare a variable/routine as
	ifdef TASM				;external and public depending on
	global var				;whats required. "var" is the name of
	else					;the symbol to declare.
	externdef var
	endif
	endm


;------------------------------------------------------------------------------
;Some more general purpose macro's to keep things simple.
;------------------------------------------------------------------------------

pushm	MACRO	R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16
	IRP	X,<R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16> 	;REPEAT FOR EACH PARM
	IFNB	<X>			;IF THIS PARM NOT BLANK
	PUSH	X			;SAVE THE REGISTER
	ENDIF				;END IFNB
	ENDM				;END IRP
	ENDM

popm	macro	R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16
	IRP	X,<R16,R15,R14,R13,R12,R11,R10,R9,R8,R7,R6,R5,R4,R3,R2,R1> ;REPEAT FOR EACH PARM
	IFNB	<X>				;IF THIS PARM NOT BLANK
	POP	X				;POP THE REGISTER
	ENDIF					;END IFNB
	ENDM					;END IRP
	ENDM

rep_movsb	macro
	push	ecx
	shr	ecx,2
	rep	movsd
	pop	ecx
	and	ecx,3
	rep	movsb
	endm

rep_movsw	macro
	push	ecx
	shr	ecx,1
	rep	movsd
	pop	ecx
	and	ecx,1
	rep	movsw
	endm

rep_stosb	macro
	push	eax
	mov	ah,al
	push	ax
	shl	eax,16
	pop	ax
	push	ecx
	shr	ecx,2
	rep	stosd
	pop	ecx
	and	ecx,3
	rep	stosb
	pop	eax
	endm

rep_stosw	macro
	push	eax
	push	ax
	shl	eax,16
	pop	ax
	push	ecx
	shr	ecx,1
	rep	stosd
	pop	ecx
	and	ecx,1
	rep	stosw
	pop	eax
	endm


;------------------------------------------------------------------------------
;Some useful equates.
;------------------------------------------------------------------------------

b	equ	byte ptr
w	equ	word ptr
d	equ	dword ptr
f	equ	fword ptr

SK_SHIFTR	equ	1
SK_SHIFTL	equ	2
SK_CTRL	equ	4
SK_ALT	equ	8
SK_SCROLL	equ	16
SK_NUM	equ	32
SK_CAPS	equ	64
SK_INS	equ	128


;-----------------------------------------------------------------------------
;Switch video memory bank number. This will only be needed if you are writing
;low level video access code.
;-----------------------------------------------------------------------------
Bank	macro bankreg
	local __noswitch
	push	eax
	mov	eax,bankreg			;Get huge video memory address.
	shr	eax,16			;isolate the bank number.
	cmp	ax,ss:CurrentBank		;same as we're already useing?
	jz	__noswitch
	inc	ss:InMouse			;stop mouse event interfering.
	pushm	eax,ebx,ecx,edx
	mov	bh,0
	mov	bl,0				;set both read and write bank numbers
	mov	dx,ax				;to be the same. At some point I'll
	mov	cl,ss:BankShift
	shl	dx,cl
	call	ss:bankaddress		;change this so that the second call
	inc	bl				;isn't made if it's not needed.
	call	ss:bankaddress
	popm	eax,ebx,ecx,edx
	mov	ss:CurrentBank,ax		;update the bank number.
	dec	ss:InMouse			;allow mouse stuff access again.
__noswitch:	pop	eax
	endm


;-----------------------------------------------------------------------------
;Some video mode equates. It's better to use the mode list built during
;initialisation incase a mode is supported that isn't listed here but these
;give easy access to known modes.
;
;Note: Entries with a value of 0 indicate I haven't found out the mode numbers
;      YET.
;-------------------------------------------------------------------------
Video320x200x256	equ	13h
Video640x400x256	equ	100h
Video640x480x256	equ	101h
Video800x600x256	equ	103h
Video1024x768x256	equ	105h
Video1152x1024x256	equ	122h
Video1280x1024x256	equ	107h
Video1600x1200x256	equ	124h
;
Video320x200x32k	equ	10dh
Video640x400x32k	equ	0
Video640x480x32k	equ	110h
Video800x600x32k	equ	113h
Video1024x768x32k	equ	116h
Video1280x1024x32k	equ	0
;
Video320x200x64k	equ	10eh
Video640x400x64k	equ	0
Video640x480x64k	equ	111h
Video800x600x64k	equ	114h
Video1024x768x64k	equ	117h
Video1280x1024x64k	equ	0
;
Video320x200x16m	equ	0
Video640x400x16m	equ	0
Video640x480x16m	equ	112h
Video800x600x16m	equ	115h
Video1024x768x16m	equ	0
Video1280x1024x16m	equ	0


;-----------------------------------------------------------------------------
;VESA structures returned by several VESA INT 10h functions. Only needed if
;you do low level access stuff.
;-----------------------------------------------------------------------------
VgaInfoBlock		struc
 VIB_VESASignature	db 'VESA'		;4 signature bytes
 VIB_VESAVersion	dw ?			;VESA version number
 VIB_OEMStringPtr	dd ?			;Pointer to OEM string
 VIB_Capabilities	db 4 dup (?)	;capabilities of the video environment
 VIB_VideoModePtr	dd ?			;pointer to supported Super VGA modes
VgaInfoBlock		ends

ModeInfoBlock struc
 ;mandatory information
 MIB_ModeAttributes	dw ?		;mode attributes
 MIB_WinAAttributes	db ?		;Window A attributes
 MIB_WinBAttributes	db ?		;Window B attributes
 MIB_WinGranularity	dw ?		;window granularity
 MIB_WinSize			dw ?		;window size
 MIB_WinASegment		dw ?		;Window A start segment
 MIB_WinBSegment		dw ?		;Window B start segment
 MIB_WinFuncPtr		dd ?		;pointer to window function
 MIB_BytesPerScanLine	dw ?		;bytes per scan line
 ;optional information
 MIB_XResolution		dw ?		;horizontal resolution
 MIB_YResolution		dw ?		;vertical resolution
 MIB_XCharSize		db ?		;character cell width
 MIB_YCharSize		db ?		;character cell height
 MIB_NumberOfPlanes	db ?		;number of memory planes
 MIB_BitsPerPixel		db ?		;bits per pixel
 MIB_NumberOfBanks		db ?		;number of banks
 MIB_MemoryModel		db ?		;memory model type
 MIB_BankSize		db ?		;bank size in K
ModeInfoBlock	ends


;-----------------------------------------------------------------------------
;"system" bitmap re-mapping control stuff.
;-----------------------------------------------------------------------------
MaxBMEntries		equ 1024	;number of control entries allocated.

BMT		struc
 BMT_Flags		dd ?		;Flags.
 BMT_InUse		equ	1	;Is this entry in use.
 BMT_Pointer		dd ?		;Pointer to bitmap data.
 BMT_Duplicate	dd ?		;Pointer to duplicate.
BMT		ends


;-----------------------------------------------------------------------------
;Bitmap header used/expected for all bitmap related functions.
;-----------------------------------------------------------------------------
BM	struc
 BM_Flags	dd 0	;control flags,
		;0-3 = data format, 0=256, 1=32k, 2=64k, 3=16m.
 BM_PWide	dd 0	;Pixel width. Used for fast calculations.
 BM_Wide	dd 0	;width (in pixels) of bitmap.
 BM_Depth	dd 0	;depth (in pixels) of bitmap.
 BM_XOffset	dd 0	;X offset, only used by fonts, can be used for anything.
 BM_YOffset	dd 0	;Y offset, only used by fonts, can be used for anything.
	dd 0	;reserved.
	dd 0	;reserved.
BM	ends


;-----------------------------------------------------------------------------
;Window related stuff.
;-----------------------------------------------------------------------------
Split	struc
 Split_XCoord dd 0
 Split_YCoord dd 0
 Split_Wide	dd 0
 Split_Depth	dd 0
Split	ends

MaxSplits	equ 32	;maximum number of splits per window.

Win	struc
 Win_Priority dd 0		;Display priority.
 Win_XCoord	dd 0		;X coord.
 Win_YCoord	dd 0		;Y coord.
 Win_Wide	dd 0		;Width.
 Win_Depth	dd 0		;Depth.
 Win_Handler	dd 0		;Window control handler.
 Win_Splits	db (size Split)*MaxSplits dup (0) ;Split list.
Win	ends

MaxWinWindows equ 256	;maximum number of windows.

WinMess_Draw	 equ 0	;re-draw of window required.
WinMess_Close equ 1	;window is being closed.


;-------------------------------------------------------------------------
;Dialog stuff.
;-------------------------------------------------------------------------
DlgMain	struc
DlgM_FWide	dd ?
DlgM_PWide	dd ?
DlgM_FDepth	dd ?
DlgM_PDepth	dd ?
DlgMain	ends

Dlg	struc
Dlg_State	dd ?
Dlg_FXCoord	dd ?
Dlg_PXCoord	dd ?
Dlg_FYCoord	dd ?
Dlg_PYCoord	dd ?
Dlg_FWide	dd ?
Dlg_PWide	dd ?
Dlg_FDepth	dd ?
Dlg_PDepth	dd ?
Dlg_Flags	dd 0
Dlg_Text	dd 0
Dlg	ends


;-------------------------------------------------------------------------
;Some general purpose variables from STARTUP.ASM
;-------------------------------------------------------------------------
	extdef Startup:near
	extdef _Main:near
	extdef CodeSegAlias:word
	extdef ZeroSelector:word
	extdef DataLinearBase:dword
	extdef PSPSegment:word
	extdef ENVSegment:word
	extdef ExecutionPath:byte
	extdef ExecutionName:byte


;-----------------------------------------------------------------------------
;Some useful variables from the video related code.
;-----------------------------------------------------------------------------
	extdef TimerCount:dword		;timer tick count.
	extdef VideoSelector:word	;selector for video memory access.
	extdef VideoBase:dword		;linear address for video memory.

	extdef BankAddress:dword	;bank switch routine address.
	extdef CurrentBankBig:word
	extdef CurrentBank:word		;current hardware bank selected.

	extdef VideoHardwareWidth:dword
	extdef VideoPixelWidth:dword	;bytes per pixel in video memory.
	extdef VideoXResolution:dword	;X resolution of current mode.
	extdef VideoYResolution:dword	;Y resolution of current mode.
	extdef VideoModeFlags:dword	;as yet undefined flags.
	extdef VideoMode:dword		;current video mode number.
	extdef Forced32kList:word	;table of 32k modes.
	extdef Forced64kList:word	;table of 64k modes.
	extdef BankShift:byte		;bank shift.

	extdef HardwarePalette:byte	;palette last sent to video hardware.
	extdef SystemXlat:byte		;table to translate "system" palette
						;to real hardware palette.
	extdef SystemPalette:byte	;palette used for "system" stuff and
						;used as a referance for conversion
						;between "system" and hardware.

	extdef ValidModeList:word	;list of available modes.
	extdef VESABuffer:byte		;temp buffer.
	extdef ModeInfoBuffer:byte	;temp buffer.

	extdef MouseExcludeCall:dword	;The next 3 are pointers to routines.
	extdef MouseOffCall:dword	;Normally they point to a RET but
	extdef MouseOnCall:dword	;are patched when useing MOUSE stuff
						;in this library so that they do
						;what you would expect. The normal
						;mouse routines should be called
						;unless you want code to work
						;without forceing the mouse modules
						;to be included.

	extdef InMouse:word		;None zero while inside a mouse
						;routine OR bank switch code. DON'T
						;call bank switch code from hardware
						;interrupt handlers when this is
						;none zero.

	extdef BMTable:dword		;Address of "system" bitmap control
						;table.


	extdef WinWindowList:dword	;List of window pointers.
	extdef BaseWindow:dword		;Base window.

	extdef WindowRegionFlag:dword	;Forced region clipping?
	extdef WindowRegionX:dword	;definition of region to clip to.
	extdef WindowRegionY:dword
	extdef WindowRegionW:dword
	extdef WindowRegionD:dword

	extdef FontBuffer:byte		;somewhere to play with characters.


;-------------------------------------------------------------------------
;Some useful font oriented stuff.
;-------------------------------------------------------------------------
	extdef FontTable:dword
	extdef SystemFont:dword
	extdef FontWidth:dword
	extdef FontDepth:dword


;-------------------------------------------------------------------------
;Name: Malloc
;-------------------------------------------------------------------------
;
;Allocate some DS relative memory.
;
;On Entry:
;
;ECX	- Bytes required.
;
;On Exit:
;
;Carry set on error and ESI=0 else,
;
;ESI	- DS relative address of allocated memory.
;
;All other registers preserved.
;
;-------------------------------------------------------------------------
	extdef Malloc:near


;-------------------------------------------------------------------------
;Name: ReMalloc
;-------------------------------------------------------------------------
;
;Re-size previously allocated memory.
;
;On Entry:
;
;ECX	- New size.
;ESI	- Current address of memory.
;
;On Exit:
;
;Carry set on error and ESI=0 else,
;
;ESI	- New address of memory.
;
;All other registers preserved.
;
;-------------------------------------------------------------------------
	extdef ReMalloc:near


;-------------------------------------------------------------------------
;Name: Free
;-------------------------------------------------------------------------
;
;Release some memory.
;
;On Entry:
;
;ESI	- Address to release.
;
;On Exit:
;
;All registers preserved.
;
;-------------------------------------------------------------------------
	extdef Free:near


;-------------------------------------------------------------------------
;Name: OpenFile
;-------------------------------------------------------------------------
;
;Open a file.
;
;On Entry:
;
;EDX	- File name.
;
;On Exit:
;
;Carry set on error and EBX=0 else,
;
;EBX	- File handle.
;
;-------------------------------------------------------------------------
	extdef OpenFile:near


;-------------------------------------------------------------------------
;Name: CreateFile
;-------------------------------------------------------------------------
;
;Create a file.
;
;On Entry:
;
;EDX	- File name.
;
;On Exit:
;
;Carry set on error and EBX=0 else,
;
;EBX	- File handle.
;
;-------------------------------------------------------------------------
	extdef CreateFile:near


;-------------------------------------------------------------------------
;Name: CloseFile
;-------------------------------------------------------------------------
;
;Close a file.
;
;On Entry:
;
;EBX	- file handle.
;
;-------------------------------------------------------------------------
	extdef CloseFile:near


;-------------------------------------------------------------------------
;Name: DeleteFile
;-------------------------------------------------------------------------
;
;Delete a file.
;
;On Entry:
;
;EDX	- File name to delete.
;
;On Exit:
;
;Carry set on error.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef DeleteFile:near


;-------------------------------------------------------------------------
;Name: RenameFile
;-------------------------------------------------------------------------
;
;Rename a file.
;
;On Entry:
;
;EDX	- Source file name.
;EDI	- New file name.
;
;On Exit:
;
;Carry set on error.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef RenameFile:near


;-------------------------------------------------------------------------
;Name: ReadFile
;-------------------------------------------------------------------------
;
;Read some data from a file.
;
;On Entry:
;
;EDX	- Address to read to.
;ECX	- length to read.
;EBX	- file handle.
;
;On Exit:
;
;EAX	- bytes read.
;
;-------------------------------------------------------------------------
	extdef ReadFile:near


;-------------------------------------------------------------------------
;Name: WriteFile
;-------------------------------------------------------------------------
;
;Write some data to a file.
;
;On Entry:
;
;EDX	- Address to write from.
;ECX	- Length to write.
;EBX	- file handle.
;
;On Exit:
;
;EAX	- Bytes written.
;
;-------------------------------------------------------------------------
	extdef WriteFile:near


;-------------------------------------------------------------------------
;Name: SetFilePointer
;-------------------------------------------------------------------------
;
;Set the file pointer position for a file.
;
;On Entry:
;
;AL	- method.
;EBX	- handle.
;ECX	- position.
;
;Methods are:
;
;0	- Absolute offset from start.
;1	- signed offset from current position.
;2	- signed offset from end of file.
;
;On Exit:
;
;ECX	- absolute offset from start of file.
;
;-------------------------------------------------------------------------
	extdef SetFilePointer:near


;-------------------------------------------------------------------------
;Name: InitFileBuffer
;-------------------------------------------------------------------------
;
;Initialise file buffer.
;
;On Entry:
;
;EBX	- File handle.
;
;On Exit:
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef InitFileBuffer:near


;-------------------------------------------------------------------------
;Name: ReadBufferByte
;-------------------------------------------------------------------------
;
;Read a byte from the file buffer.
;
;On Entry:
;
;None
;
;On Exit:
;
;AH	- Bytes read, ie, AH=0 indicates EOF or an error.
;AL	- Byte read.
;
;eg, if AX=0 then EOF reached.
;
;Don't mix normal ReadFile call's with this call, they don't take account of
;each other.
;
;-------------------------------------------------------------------------
	extdef ReadBufferByte:near


;-------------------------------------------------------------------------
;Name: ReadBufferLine
;-------------------------------------------------------------------------
;
;Read a line of text from input file specified.
;
;On Entry:
;
;EDI	- Buffer.
;
;On Exit:
;
;AL	- status.
;	0 - OK.
;	1 - EOF.
;
;ECX	- Bytes read.
;
;-------------------------------------------------------------------------
	extdef ReadBufferLine:near


;-------------------------------------------------------------------------
;Name: MakeFileList
;-------------------------------------------------------------------------------
;
;Generate list of files that match a given file mask. Wild cards permitted.
;
;On Entry:
;
;EDX	- File name mask (can include a path).
;CX	- search attributes.
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
; Next come the actual file name entries.
;
;-------------------------------------------------------------------------
	extdef MakeFileList:near


;-------------------------------------------------------------------------
;Name: ULFileList
;-------------------------------------------------------------------------------
;
;Convert normal names to lower case and directory names to upper case.
;
;On Entry:
;
;ESI	- pointer to file list.
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
;-------------------------------------------------------------------------
	extdef ULFileList:near


;-------------------------------------------------------------------------
;Name: SortFileList
;-------------------------------------------------------------------------------
;
;Sort a file list into alpha-numeric order.
;
;On Entry:
;
;ESI	- pointer to file list to sort.
;
;On Exit:
;
;ALL registers preserved.
;
;File names are not case converted so upper case names will appear at the top
;of the list.
;
;-------------------------------------------------------------------------
	extdef SortFileList:near


;-------------------------------------------------------------------------------
;Name: FileListMakeName
;-------------------------------------------------------------------------
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
;name which is what this routine does.
;
;-------------------------------------------------------------------------
	extdef FileListMakeName:near


;-------------------------------------------------------------------------
;Name: GetPath
;-------------------------------------------------------------------------
;
;Get current drive and path.
;
;On Entry:
;
;EDX	- pointer to buffer.
;
;On Exit:
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef GetPath:near


;-------------------------------------------------------------------------
;Name: SetPath
;-------------------------------------------------------------------------
;
;Set current drive and path.
;
;On Entry:
;
;EDX	- pointer to path string.
;
;On Exit:
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef SetPath:near


;-------------------------------------------------------------------------
;Name: GetDrive
;-------------------------------------------------------------------------
;
;Get the current drive.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;AL	- Drive code.
;
;-------------------------------------------------------------------------
	extdef GetDrive:near


;-------------------------------------------------------------------------
;Name: SetDrive
;-------------------------------------------------------------------------
;
;Set the current drive.
;
;On Entry:
;
;AL	- drive to set.
;
;On Exit:
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef SetDrive:near


;-------------------------------------------------------------------------
;Name: GetDrives
;-------------------------------------------------------------------------
;
;Build a list of all valid drives on the system.
;
;On Entry:
;
;EDX	- Address to build list of drives.
;
;On Exit:
;
;ECX	- number of drives.
;
;ALL other registers preserved.
;
;The drive list uses real drive codes (not ASCII). Each entry uses 1 byte
;and the list is terminated with -1.
;
;-------------------------------------------------------------------------
	extdef GetDrives:near


;-------------------------------------------------------------------------
;Name: PrintString
;-------------------------------------------------------------------------
;
;Print null terminated string on screen via DOS.
;
;On Entry:
;
;ESI	- pointer to string to print.
;
;On Exit:
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef PrintString:near


;-------------------------------------------------------------------------
;Name: UpperString
;-------------------------------------------------------------------------
;
;Convert a string into upper case.
;
;On Entry:
;
;ESI	- String to convert.
;
;On Exit:
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef UpperString:near


;-------------------------------------------------------------------------
;Name: CopyString
;-------------------------------------------------------------------------
;
;Copy a string from one place to another.
;
;On Entry:
;
;ESI	- Source string.
;EDI	- Destination string.
;
;On Exit:
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef CopyString:near


;-------------------------------------------------------------------------
;Name: AppendString
;-------------------------------------------------------------------------
;
;Append a string to another string.
;
;On Entry:
;
;ESI	- source string.
;EDI	- destination string to append to.
;
;On Exit:
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef AppendString:near


;------------------------------------------------------------------------------
;Name: LenString
;------------------------------------------------------------------------------
;
;Work out length of a string.
;
;On Entry:
;
;ESI	- pointer to string.
;
;On Exit:
;
;EAX	- length of string.
;
;ALL other registers preserved.
;
;------------------------------------------------------------------------------
	extdef LenString:near


;-------------------------------------------------------------------------
;Name: UpperChar
;------------------------------------------------------------------------------
;
;Convert character to upper case.
;
;On Entry:
;
;AL	- character code.
;
;On Exit:
;
;AL	- upper case character code.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef UpperChar:near


;-------------------------------------------------------------------------
;Name: Beep
;-------------------------------------------------------------------------
;
;Beep, useful for simplistic debugging.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef Beep:near


;-------------------------------------------------------------------------
;Name: Bin2Dec
;-------------------------------------------------------------------------
;
;Convert number into decimal ASCII.
;
;On Entry:
;
;EAX	- Number to convert.
;EDI	- Buffer to put it in.
;
;On Exit:
;
;EDI	- Next available byte in buffer.
;
;-------------------------------------------------------------------------
	extdef Bin2Dec:near


;-------------------------------------------------------------------------
;Name: Bin2Hex
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
;-------------------------------------------------------------------------
	extdef Bin2Hex:near


;-------------------------------------------------------------------------
;Name: ReadCommand
;-------------------------------------------------------------------------
;
;Read the command line tail for parameters.
;
;supports / or - or + as switch/option settings. Options and names may be in
;any order, text can be tagged onto options, names must be in correct sequence
;for current program.
;
;Each character ( 33 to 127) has an entry in OptionTable & OptionPointers.
;
;The first entry is a byte, and is 0 for OFF & none-zero for ON.
;The second entry is a pointer to any additional text specified, 0 means none.
;Entries 0-32 are reserved for text not preceded by - or + or /. These are
;intended to be file names, but neadn't be.
;All text entries are 0 terminated.
;
;OptionCounter	 - Total command line parameters (files & switches).
;OptionTable	 - Switch table, ASCII code is index to check.
;OptionPointers - Pointer table, ASCII code*4 is index to use.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
;-------------------------------------------------------------------------
	extdef ReadCommand:near
	extdef OptionCounter:byte
	extdef OptionTable:byte
	extdef OptionPointers:dword


;-------------------------------------------------------------------------
;Name: Delay
;-------------------------------------------------------------------------
;
;Wait for a while.
;
;On Entry:
;
;ECX	- time to wait for.
;
;-------------------------------------------------------------------------
	extdef Delay:near


;-------------------------------------------------------------------------
;Name: GetKey
;-------------------------------------------------------------------------
;
;Get key press info.
;
;On Entry:
;
;none.
;
;On Exit:
;
;AL	- ASCII character code.
;AH	- Hardware key code.
;EAX high word	- Shift keys status word from 417h.
;
;-------------------------------------------------------------------------
	extdef GetKey:near


;-------------------------------------------------------------------------
;Name: WaitKey
;-------------------------------------------------------------------------
;
;Wait for a key press.
;
;On Entry:
;
;none.
;
;On Exit:
;
;EAX	- Key code, same format as GetKey.
;
;-------------------------------------------------------------------------
	extdef WaitKey:near


;-------------------------------------------------------------------------
;Name: NearNull
;-------------------------------------------------------------------------
;
;Dummy routine for variables that need a call value that does nothing.
;
;No entry or exit conditions, just does a RET.
;
;-------------------------------------------------------------------------
	extdef NearNull:near


;-----------------------------------------------------------------------------
;Name: VideoInitialise
;-------------------------------------------------------------------------
;
;Initialise video stuff, includeing VESA support if detected.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error and EAX=error code.
;
;Error codes:
;
;0	- no error.
;1	- couldn't allocate selector for video memory.
;2	- not enough memory for bitmap control table.
;3	- video system not supported, ie, not (S)VGA.
;
;ALL other registers preserved.
;
;-----------------------------------------------------------------------------
	extdef VideoInitialise:near


;-----------------------------------------------------------------------------
;Name: VideoRemove
;-------------------------------------------------------------------------
;
;Remove video stuff, includeing VESA support if installed.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;ALL registers preserved.
;
;-----------------------------------------------------------------------------
	extdef VideoRemove:near


;-----------------------------------------------------------------------------
;Name: InitVESA
;-----------------------------------------------------------------------------
;
;Have a look for VESA INT 10h BIOS extension and if present install a patch to
;make protected mode access transparent. This is only needed if you don't use
;VideoInitialise.
;
;On Entry:
;
;nothing
;
;On Exit:
;
;Carry set on error and EAX=0 else,
;
;EAX	- Address of bank switch code. Always != 0.
;
;ALL other registers preserved.
;
	extdef InitVESA:near


;-----------------------------------------------------------------------------
;Name: RelVESA
;-----------------------------------------------------------------------------
;
;Release VESA BIOS patch if installed.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;
;Notes: This routine can safely be called even if InitVESA hasn't been
;       called or didn't install due to lack of VESA.
;
	extdef RelVESA:near


;-------------------------------------------------------------------------
;Name: SetDACs
;-------------------------------------------------------------------------
;
;Set the value's in VGA colour DAC's.
;
;On Entry:
;
;EBX	- Starting DAC number.
;ECX	- Number of DAC's to set.
;ESI	- Pallet information
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef SetDACs:near


;-------------------------------------------------------------------------
;Name: SearchRGB
;-------------------------------------------------------------------------
;
;Find closest match in HardwarePalette to RGB value specified. Must be more
;accurate ways of doing it than this but I don't know about them!
;
;On Entry:
;
;EAX	- RGB value.
;
;On Exit:
;
;EAX	- Nearest pen value in HardwarePalette
;
;-------------------------------------------------------------------------
	extdef SearchRGB:near


;------------------------------------------------------------------------------
;Name: RemapSystemBitmap
;------------------------------------------------------------------------------
;
;Copy and re-map "system" bitmap duplicate.
;
;On Entry:
;
;ESI	- pointer to "system" bitmap table entry.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;------------------------------------------------------------------------------
	extdef RemapSystemBitmap:near


;------------------------------------------------------------------------------
;Name: RemapSystemBitmaps
;------------------------------------------------------------------------------
;
;Re-map all bitmaps registered as useing "system" palette.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;------------------------------------------------------------------------------
	extdef RemapSystemBitmaps:near


;-------------------------------------------------------------------------
;Name: RegisterSystemBitmap
;-------------------------------------------------------------------------
;
;Register a bitmap that needs to be re-mapped whenever the hardware palette
;changes so it stays in the system palette scheme.
;
;On Entry:
;
;ESI	- bitmap.
;
;On Exit:
;
;EAX	- handle.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef RegisterSystemBitmap:near


;-------------------------------------------------------------------------
;Name: ReleaseSystemBitmap
;-------------------------------------------------------------------------
;
;Release a bitmap previously regeistered with the bitmap manager. Duplicate
;memory is free'd and entry in table is marked free.
;
;On Entry:
;
;ESI	- Handle
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef ReleaseSystemBitmap:near


;-------------------------------------------------------------------------
;Name: VideoSetPalette
;-------------------------------------------------------------------------
;
;Set hardware palette. Generates new system to hardware translation table
;and re-maps all "system" bitmaps.
;
;On Entry:
;
;EAX	- Base
;ECX	- Number.
;ESI	- data.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef VideoSetPalette:near


;-------------------------------------------------------------------------
;Name: VideoSetMode
;-------------------------------------------------------------------------
;
;Attempt to set specified video mode.
;
;On Entry:
;
;EAX	- Mode.
;
;On Exit:
;
;EAX	- zero on error else mode number.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef VideoSetMode:near


;-------------------------------------------------------------------------
;Name: BuildModeList
;-------------------------------------------------------------------------
;
;Build a text list of modes in the buffer specified.
;
;On Entry:
;
;ESI	- buffer to use.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
;-------------------------------------------------------------------------
	extdef BuildModeList:near


;-------------------------------------------------------------------------
;Name: VideoSys2Hard
;-------------------------------------------------------------------------
;
;Convert system pen number into hardware pen number.
;
;On Entry:
;
;AL	- pen number to convert.
;
;On Exit:
;
;AL	- new pen number.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef VideoSys2Hard:near


;-------------------------------------------------------------------------
;Name: Bord
;-------------------------------------------------------------------------
;
;Change the border colour. Provided mainly for simplistic debugging.
;
;On Entry:
;
;AL	- colour to set.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef Bord:near


;-------------------------------------------------------------------------
;Name: VideoSetPixel
;-------------------------------------------------------------------------
;
;Set a pixel to specified colour. Cheats by useing VideoFillRectangle for now.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - FillValue is RGB.
;EBX	- Fill value.
;ECX	- X co-ord.
;EDX	- Y co-ord.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
;-------------------------------------------------------------------------
	extdef VideoSetPixel:near


;-------------------------------------------------------------------------
;Name: VideoFillRectangle
;-------------------------------------------------------------------------
;
;Clear a rectangle of video memory to specified value.
;
;On Entry:
;
;C style stack with following parameters,
;
;flags	- Flags, bit significant if set.
;	0 - FillValue is RGB.
;FillValue	- fill value to use.
;XCoord	- X co-ord to start at.
;YCoord	- Y co-ord to start at.
;Wide	- width to fill.
;Depth	- depth to fill.
;
;On Exit:
;
;Nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef VideoFillRectangle:near


;-------------------------------------------------------------------------
;Name: VideoDrawLine
;-------------------------------------------------------------------------
;
;Draw a line in specified colour between specified points.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - Fill value is RGB.
;	1 - Fill type, 0-normal, 1-store data, 2-replace data.
;	2 -/
;EBX	- Fill value or data pointer depending on flags.
;ECX	- start X co-ord.
;EDX	- start Y co-ord.
;ESI	- end X co-ord.
;EDI	- end Y co-ord.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
;Notes:
;
;If type 1 is selected it is the callers responsibility to ensure that the
;buffer pointed to by EBX is big enough.
;
;-------------------------------------------------------------------------
	extdef VideoDrawLine:near


;-------------------------------------------------------------------------
;Name: VideoBlitBitmap
;-------------------------------------------------------------------------
;
;Blit a bitmap to/from video memory.
;
;On Entry:
;
;C style stack with following parameters,
;
;flags	- flags, bit significant if set.
;	0 - blit video to system memory.
;	1 - OR bitmap on, chroma value is used.
;bitmap	- pointer to bitmap.
;xcoord	- X co-ord to blit at.
;ycoord	- Y co-ord to blit at.
;xoff	- X offset within bitmap.
;yoff	- Y offset within bitmap.
;wide	- width to blit.
;depth	- depth to blit.
;chroma	- chroma value to use when OR'ing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;Note: If bit 0 of flags is set all other flag bits are ignored.
;
;-------------------------------------------------------------------------
	extdef VideoBlitBitmap:near


;-------------------------------------------------------------------------------
;Name: MouseInitialise
;-------------------------------------------------------------------------------
;
;Initialise mouse stuff.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;EAX	- mouse status.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------------
	extdef MouseInitialise:near


;-------------------------------------------------------------------------------
;Name: MouseRemove
;-------------------------------------------------------------------------------
;
;Release mouse stuff.
;
;On Entry:
;
;Nothing.
;
;On Exit:
;
;Nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------------
	extdef MouseRemove:near


;-------------------------------------------------------------------------------
;Name: MouseOn
;-------------------------------------------------------------------------------
;
;Turn the mouse pointer ON. Will increment the mouse flag byte. The mouse
;pointer is only displayed when this flag is > 0.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------------
	extdef MouseOn:near


;-------------------------------------------------------------------------------
;Name: MouseOff
;-------------------------------------------------------------------------------
;
;Turn the mouse pointer OFF. Will decrement the mouse flag byte. The mouse
;pointer is only displayed when this flag is > 0.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------------
	extdef MouseOff:near


;-------------------------------------------------------------------------
;Name: MouseSetType
;-------------------------------------------------------------------------
;
;Set the type of graphic used for the mouse pointer. Only 2 are currently
;supported but others can be added to the table.
;
;On Entry:
;
;AL	- Pointer number.
;
;On Exit:
;
;AL	- Old pointer number.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef MouseSetType:near


;-------------------------------------------------------------------------
;Name: MouseGet
;-------------------------------------------------------------------------
;
;Get the current position & button state for the mouse.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;EBX	- button state.
;ECX	- X coord.
;EDX	- Y coord.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef MouseGet:near


;-------------------------------------------------------------------------
;Name: MouseExclude
;-------------------------------------------------------------------------
;
;Set/Remove mouse exclusion zone.
;
;On Entry:
;
;AX	- Mode.
;	0 - Set exclusion zone.
;	1 - Remove exclusion zone.
;
;If mode 0,
;
;EBX	- X coord.
;ECX	- Y coord.
;EDX	- Width.
;EBP	- depth.
;
;If mode 1,
;
;EBX	- Exclusion handle.
;
;On Exit:
;
;If mode 0,
;
;EBX	- Exclusion handle.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef MouseExclude:near


;-------------------------------------------------------------------------
;Name: WinInitialise
;-------------------------------------------------------------------------
;
;Initialise Window's stuff so base handler gets a lookin.
;
;-------------------------------------------------------------------------
	extdef WinInitialise:near


;-------------------------------------------------------------------------
;Name: WinOpenWindow
;-------------------------------------------------------------------------
;
;Initialise a window structure and add it to the list of windows in service.
;
;On Entry:
;
;C style stack parameters as follows:
;
;flags	- undefined at the moment, set to zero.
;priority	- priority to assign.
;xcoord	- x co-ord for window.
;ycoord	- y co-ord for window.
;wide	- width of window in pixels.
;depth	- depth of window in pixels.
;handler	- address of handler (0 for none).
;
;On Exit:
;
;EAX	- pointer to window structure (handle).
;
;ALL other registers presereved.
;
;-------------------------------------------------------------------------
	extdef WinOpenWindow:near


;-------------------------------------------------------------------------
;Name: CalculateSplits
;-------------------------------------------------------------------------
;
;Calculate splits for all windows.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef CalculateSplits:near


;-------------------------------------------------------------------------------
;Name: WinMessageWindow
;-------------------------------------------------------------------------------
;
;Send a message to a window.
;
;On Entry:
;
;EAX	- message
;ESI	- window
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------------
	extdef WinMessageWindow:near


;------------------------------------------------------------------------------
;Name: WindowRegionClip
;------------------------------------------------------------------------------
;
;Clip a set of coords to current overiding region if enabled.
;
;On Entry:
;
;ECX	- X coord.
;EDX	- Y coord.
;ESI	- width.
;EDI	- depth.
;
;On Exit:
;
;Carry set if region completely obscured else,
;
;ECX	- clipped X coord.
;EDX	- clipped Y coord.
;ESI	- clipped width.
;EDI	- clipped depth.
;
;ALL other registers preserved.
;
;------------------------------------------------------------------------------
	extdef WindowRegionClip:near


;-------------------------------------------------------------------------------
;Name: WindowRegionUpdate
;-------------------------------------------------------------------------------
;
;Update a particular rectangle of the display.
;
;On Entry:
;
;ECX	- X coord.
;EDX	- Y coord.
;ESI	- width.
;EDI	- depth.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------------
	extdef WindowRegionUpdate:near


;-------------------------------------------------------------------------
;Name: WinCloseWindow
;-------------------------------------------------------------------------
;
;Close a window and update the area underneath it if possible.
;
;On Entry:
;
;EAX	- Pointer to window structure (handle).
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef WinCloseWindow:near


;-------------------------------------------------------------------------
;Name: WinSetPixel
;-------------------------------------------------------------------------
;
;Set a pixel to specified colour. Cheats by useing WinFillRectangle for now.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - FillValue is RGB.
;EBX	- Fill value.
;ECX	- X co-ord.
;EDX	- Y co-ord.
;EBP	- Bitmap.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
;-------------------------------------------------------------------------
	extdef WinSetPixel:near


;-------------------------------------------------------------------------
;Name: WinFillRectangle
;-------------------------------------------------------------------------
;
;Fill a rectangular region of specified window.
;
;On Entry:
;
;C style stack parameters as follows,
;
;window	- pointer to window structure (handle).
;flags	- flags, bit significant if set.
;	0 - fillvalue is RGB.
;fillvalue	- fill value to use.
;xcoord	- x co-ord to fill at.
;ycoord	- y co-ord to fill at.
;wide	- width in pixels.
;depth	- depth in pixels.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef WinFillRectangle:near


;-------------------------------------------------------------------------
;Name: WinDrawLine
;-------------------------------------------------------------------------
;
;Draw a line in specified colour between specified points.
;
;On Entry:
;
;C style stack parameters as follows,
;
;flags	- flags, bit significant if set.
;colour	- fill colour.
;x1	- start X co-ord.
;y1	- start Y co-ord.
;x2	- end x co-ord.
;y2	- end y co-ord.
;window	- window to draw in.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
;Notes:
;
;If type 1 is selected it is the callers responsibility to ensure that the
;buffer pointed to by EBX is big enough.
;
;-------------------------------------------------------------------------
	extdef WinDrawLine:near


;-------------------------------------------------------------------------
;Name: WinBlitBitmap
;-------------------------------------------------------------------------
;
;Blit a bitmap into a window.
;
;On Entry:
;
;C style stack parameters as follows,
;
;window	- pointer to window structure (handle).
;flags	- control flags, bit significant if set.
;	0 - N/A
;	1 - OR the bitmap on, use chroma.
;bitmap	- pointer to bitmap.
;xcoord	- x co-ord to blit at within window.
;ycoord	- y co-ord to blit at within window.
;xoff	- x offset within bitmap to blit from.
;yoff	- y offset within bitmap to blit from.
;wide	- with in pixels.
;Depth	- depth in pixels.
;chroma	- Chroma key if needed.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef WinBlitBitmap:near


;-------------------------------------------------------------------------
;Name: WinPrintWindow
;-------------------------------------------------------------------------
;
;Print a string of bitmaps in a window.
;
;On Entry:
;
;C style stack parameters as follows,
;
;window	- pointer to window structure (handle).
;flags	- control flags, bit significant if set,
;	0 - N/A
;	1 - OR bitmaps on, use chroma.
;	4 - used fixed spaceing.
;pen	- pen to print in.
;font	- font table to use.
;string	- string to print.
;xcoord	- x co-ord to print at.
;ycoord	- y co-ord to print at.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef WinPrintWindow:near


;-------------------------------------------------------------------------
;Name: WinWhichWindow
;-------------------------------------------------------------------------
;
;See which (if any) window the co-ords fall inside and return relative values
;
;On Entry:
;
;ECX	- Xcoord
;EDX	- Ycoord
;
;On Exit:
;
;EAX	- Window structure pointer (handle), -1 for none.
;ECX	- relative X coord.
;EDX	- relative Y coord.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef WinWhichWindow:near


;------------------------------------------------------------------------------
;Name: CreateBitmap
;------------------------------------------------------------------------------
;
;Create a new bitmap.
;
;On Entry:
;
;EBX	- bitmap flags entry.
;ECX	- bitmap width.
;EDX	- bitmap depth.
;
;On Exit:
;
;Carry set on error and EAX=0 else,
;
;EAX	- pointer to bitmap.
;
;ALL other registers preserved.
;
;------------------------------------------------------------------------------
	extdef CreateBitmap:near


;-------------------------------------------------------------------------
;Name: BitmapSetPixel
;-------------------------------------------------------------------------
;
;Set a pixel to specified colour. Cheats by useing BitmapFillRectangle for now.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - FillValue is RGB.
;EBX	- Fill value.
;ECX	- X co-ord.
;EDX	- Y co-ord.
;EBP	- Bitmap.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
;-------------------------------------------------------------------------
	extdef BitmapSetPixel:near


;-------------------------------------------------------------------------
;Name: BitmapFillRectangle
;-------------------------------------------------------------------------
;
;Fill a rectangle of a bitmap.
;
;On Entry:
;
;flags	- Flags.
;fillvalue	- Fill value.
;xcoord	- X co-ord.
;ycoord	- Y co-ord.
;wide	- width.
;depth	- depth.
;bitmap	- bitmap.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
;-------------------------------------------------------------------------
	extdef BitmapFillRectangle:near


;-------------------------------------------------------------------------
;Name: BitmapDrawLine
;-------------------------------------------------------------------------
;
;Draw a line in specified colour between specified points.
;
;On Entry:
;
;C style stack parameters as follows,
;
;flags	- flags, bit significant if set.
;colour	- fill colour.
;x1	- start X co-ord.
;y1	- start Y co-ord.
;x2	- end x co-ord.
;y2	- end y co-ord.
;bitmap	- bitmap to draw on.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
;Notes:
;
;If type 1 is selected it is the callers responsibility to ensure that the
;buffer pointed to by EBX is big enough.
;
;-------------------------------------------------------------------------
	extdef BitmapDrawLine:near


;-------------------------------------------------------------------------
;Name: BitmapBlitBitmap
;-------------------------------------------------------------------------
;
;Blit a bitmap into another bitmap.
;
;On Entry:
;
;C style stack parameters as follows:
;
;Flags	- Flags, bit significant if set, may be combined.
;	0-Reserved.
;	1-OR the bitmap, "Chroma" contains transparent colour.
;	2-Scaled blit, DWide & DDepth contain destination sizes.
;SBitmap	-Pointer to source bitmap.
;DBitmap	-Pointer to destination bitmap.
;XCoord	-X co-ord in destination to blit to.
;YCoord	-Y co-ord in destination to blit to.
;XOff	-X offset within source to blit from.
;YOff	-Y offset within source to blit from.
;Wide	-Width to blit, -1 for entire source width.
;Depth	-Depth to blit, -1 for entire destination width.
;DWide	-Destination width of blit when scaleing.
;DDepth	-Destination depth of blit when scaleing.
;Chroma	-Transparent colour when OR'ing.
;
;On Exit:
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef BitmapBlitBitmap:near


;-------------------------------------------------------------------------
;Name: BitmapXFlip
;------------------------------------------------------------------------------
;
;Flip a bitmap from left to right, eg, around Y axis, but affecting it in the X
;
;On Entry:
;
;ESI	- bitmap
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
;-------------------------------------------------------------------------
	extdef BitmapXFlip:near


;-------------------------------------------------------------------------
;Name: BitmapYFlip
;------------------------------------------------------------------------------
;
;Flip a bitmap from top to bottom, eg, around X axis, but affecting it in the Y
;
;On Entry:
;
;ESI	- bitmap
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
;-------------------------------------------------------------------------
	extdef BitmapYFlip:near


;-------------------------------------------------------------------------
;Name: ButtonWindow
;-------------------------------------------------------------------------
;
;Make a piece of window look like a button.
;
;On Entry:
;
;EAX	- Type.
;ECX	- X coord
;EBX	- Y coord
;EDX	- width.
;EDI	- depth.
;ESI	- Pointer to window structure.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef ButtonWindow:near


;-------------------------------------------------------------------------
;Name: DrawDialog
;-------------------------------------------------------------------------
;
;Draw a dialog entry.
;
;On Entry:
;
;ECX	- Button number, -1 to do all.
;ESI	- Dialog.
;EDI	- Window.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef DrawDialog:near


;-------------------------------------------------------------------------
;Name: DialogGetButtonD
;-------------------------------------------------------------------------
;
;Get depth of a dialog entry.
;
;On Entry:
;
;ECX	- Entry number.
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Depth.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef DialogGetButtonD:near


;-------------------------------------------------------------------------
;Name: DialogGetButtonW
;-------------------------------------------------------------------------
;
;Get width of a dialog entry.
;
;On Entry:
;
;ECX	- Entry number.
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Width.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef DialogGetButtonW:near


;-------------------------------------------------------------------------
;Name: DialogGetButtonX
;-------------------------------------------------------------------------
;
;Get X coord of a dialog entry.
;
;On Entry:
;
;ECX	- Entry number.
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- X coord.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef DialogGetButtonX:near


;-------------------------------------------------------------------------
;Name: DialogGetButtonY
;-------------------------------------------------------------------------
;
;Get Y coord of a dialog entry.
;
;On Entry:
;
;ECX	- Entry number.
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Y coord.
;
;ALL other registers preserved,
;
;-------------------------------------------------------------------------
	extdef DialogGetButtonY:near


;-------------------------------------------------------------------------
;Name: DialogGetDepth
;-------------------------------------------------------------------------
;
;Get main depth of a dialog.
;
;On Entry:
;
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Depth.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef DialogGetDepth:near


;-------------------------------------------------------------------------
;Name: DialogGetState
;-------------------------------------------------------------------------
;
;Get state field of a dialog entry.
;
;On Entry:
;
;ECX	- Entry number.
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- State.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef DialogGetState:near


;-------------------------------------------------------------------------
;Name: DialogGetWidth
;-------------------------------------------------------------------------
;
;Get main width of a dialog.
;
;On Entry:
;
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;EAX	- Width.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef DialogGetWidth:near


;-------------------------------------------------------------------------
;Name: DialogSetState
;-------------------------------------------------------------------------
;
;Set state field of a dialog entry.
;
;On Entry:
;
;EAX	- State to set.
;ECX	- Entry number.
;ESI	- Pointer to dialog structure.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;-------------------------------------------------------------------------
	extdef DialogSetState:near


;-------------------------------------------------------------------------
;Name: DialogWhich
;-------------------------------------------------------------------------
;
;Work out which section of a dialog the specified co-ords relate to.
;
;On Entry:
;
;ECX	- X coord.
;EDX	- Y coord.
;ESI	- Dialog.
;
;On Exit:
;
;EAX	- Entry number, -1 for none.
;ECX	- Relative X coord.
;EDX	- Relative Y coord.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef DialogWhich:near


;-------------------------------------------------------------------------
;Name: LoadFont
;-------------------------------------------------------------------------
;
;Load font file into a slot.
;
;On Entry:
;
;EDX	- file name.
;ECX	- slot number.
;
;On Exit:
;
;EAX	- status.
;	0 - no error.
;	1 - file not found or I/O error.
;	2 - not enough memory.
;
;ALL other registers preserved.
;
;-------------------------------------------------------------------------
	extdef LoadFont:near


;------------------------------------------------------------------------------
;Name: StringLenPixels
;------------------------------------------------------------------------------
;
;Work out the pixel length of a string.
;
;On Entry:
;
;ESI	- String to scan.
;EDI	- Font to use.
;
;On Exit:
;
;EAX	- Length in pixels.
;
;ALL other registers preserved.
;
;------------------------------------------------------------------------------
	extdef StringLenPixels:near


;-------------------------------------------------------------------------
;Name: DlgMessageOKCANCEL
;-------------------------------------------------------------------------
;
;Display a box with specified message and OK/CANCEL buttons.
;
;On Entry:
;
;ESI	- Message to display.
;
;On Exit:
;
;EAX	- status.
;	0 - Cancel.
;	!=0 - OK.
;
;All other registers preserved.
;
;-------------------------------------------------------------------------
	extdef DlgMessageOKCANCEL:near


;-------------------------------------------------------------------------
;Name: dlgFileSelector
;-------------------------------------------------------------------------
;
;Display a file selector and get a list of file names from the user.
;
;On Entry:
;
;C style stack parameters as follows,
;
;flags	- Flags, bit significant if set.
;	0 - Allow multiple selections with SHIFT & CTRL.
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
;-------------------------------------------------------------------------
	extdef dlgFileSelector:near


;------------------------------------------------------------------------------
;Name: LoadImageFile
;------------------------------------------------------------------------------
;
;Load an image file.
;
;On Entry:
;
;EDX	- pointer to name of file to load.
;
;On Exit:
;
;EAX	- Status,
;	0 - No error.
;	1 - File access error.
;	2 - Not enough memory.
;	3 - Bad file format.
;ESI	- Bitmap pointer.
;
;------------------------------------------------------------------------------
	extdef LoadImageFile:near


;------------------------------------------------------------------------------
;Name: SaveImageFile
;------------------------------------------------------------------------------
;
;Save an image file.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - compress
;	1 - alternate format 1
;ECX	- Format to use,
;	0 - auto
;	1 - IFF/LBM/ILB
;	2 - GIF
;	3 - TGA
;EDX	- pointer to name of file to create.
;ESI	- pointer to bitmap.
;
;On Exit:
;
;EAX	- Status,
;	0 - No error.
;	1 - File access error.
;	2 - Not enough memory.
;	3 - format error.
;
;All other registers preserved.
;
;------------------------------------------------------------------------------
	extdef SaveImageFile:near


;------------------------------------------------------------------------------
;Name: LoadIFF
;------------------------------------------------------------------------------
;
;Loads and unpacks an IFF file.
;
;On Entry:
;
;EDX	- pointer to name of file to load.
;
;On Exit:
;
;EAX	- Status,
;	0 - No error.
;	1 - File access error.
;	2 - Not enough memory.
;	3 - Bad file format.
;ESI	- Bitmap pointer.
;
;ALL other registers preserved.
;
;------------------------------------------------------------------------------
	extdef LoadIFF:near


;------------------------------------------------------------------------------
;Name: SaveIFF
;------------------------------------------------------------------------------
;
;Encodes and writes a bitmap in IFF format.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - compress output.
;	1 - old planar format.
;EDX	- file name.
;ESI	- Bitmap to write.
;
;On Exit:
;
;EAX	- status.
;	0 - no error.
;	1 - file error.
;	2 - not enough memory (for buffers).
;	3 - format error.
;
;ALL other registers preserved.
;
;------------------------------------------------------------------------------
	extdef SaveIFF:near


;------------------------------------------------------------------------------
;Name: LoadGIF
;------------------------------------------------------------------------------
;
;Loads and unpacks a gif file.
;
;On Entry:
;
;EDX	- File name.
;
;On Exit:
;
;EAX	- Status,
;	0 - No error.
;	1 - File access error.
;	2 - Not enough memory.
;	3 - Bad file format.
;ESI	- pointer to bitmap.
;
;------------------------------------------------------------------------------
	extdef LoadGif:near


;------------------------------------------------------------------------------
;Name: SaveGIF
;------------------------------------------------------------------------------
;
;Write an image into GIF format.
;
;On Entry:-
;
;EAX	- Flags, ignored.
;EDX	- File name to use.
;ESI	- Bitmap pointer.
;
;On Exit:
;
;EAX	- status.
;	0 - no error.
;	1 - file error.
;	2 - not enough memory (for buffers).
;	3 - format error.
;
;All other registers preserved.
;
;------------------------------------------------------------------------------
	extdef SaveGif:near


;------------------------------------------------------------------------------
;Name: LoadTGA
;------------------------------------------------------------------------------
;
;Load a TGA file.
;
;On Entry:-
;
;DS:EDX	- File name.
;
;On Exit:-
;
;EAX	- Status,
;	0 - No error.
;	1 - File access error.
;	2 - Not enough memory.
;	3 - Bad file format.
;ESI	- Address of bitmap loaded.
;
;ALL other registers preserved.
;
;------------------------------------------------------------------------------
	extdef LoadTGA:near


;------------------------------------------------------------------------------
;Name: SaveTGA
;------------------------------------------------------------------------------
;
;Save a TGA file.
;
;On Entry:
;
;EAX	- Flags,
;	0 - Compress.
;EDX	- File name.
;ESI	- bitmap to compress.
;
;On Exit:
;
;EAX	- status.
;	0 - no error.
;	1 - file error.
;	2 - not enough memory (for buffers).
;	3 - format error.
;
;All other registers preserved.
;
;------------------------------------------------------------------------------
	extdef SaveTGA:near


;------------------------------------------------------------------------------
;Name: LoadPCX
;------------------------------------------------------------------------------
;
;Load a PCX file.
;
;On Entry:-
;
;EDX	- File name.
;
;On Exit:-
;
;EAX	- Status,
;	0 - No error.
;	1 - File access error.
;	2 - Not enough memory.
;	3 - Bad file format.
;ESI	- Address of bitmap loaded.
;
;ALL other registers preserved.
;
;------------------------------------------------------------------------------
	extdef LoadPCX:near


;******************************************************************************
;Loads and unpacks a BM image file.
;
;On Entry:
;
;EDX	- pointer to name of file to load.
;
;On Exit:
;
;EAX	- Status,
;	0 - No error.
;	1 - File access error.
;	2 - Not enough memory.
;	3 - Bad file format.
;ESI	- Bitmap pointer.
;
;ALL other registers preserved.
;
;******************************************************************************
	extdef LoadBM:near


;******************************************************************************
;Encodes and writes a bitmap in BM format.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - compress output (ignored).
;EDX	- file name.
;ESI	- Bitmap to write.
;
;On Exit:
;
;EAX	- status.
;	0 - no error.
;	1 - file error.
;	2 - not enough memory (for buffers).
;	3 - format error.
;
;All other registers preserved.
;
;******************************************************************************
	extdef SaveBM:near
