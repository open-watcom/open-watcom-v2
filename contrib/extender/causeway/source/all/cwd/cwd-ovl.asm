; 2) Add CFG vars for CSEIP,REGS & Data0 window position & size.
; 4) make register display inteligent so if the window is made wider it will
;    put registers side by side.
; 6) add source search & goto functions.
; 7) Add place marker dropping/restore
;99) fix window highlight problems when removing windowpopup stuff etc.
;

	.386
	.model small
	.stack 4096

	option oldstructs

;
;Setup language equates.
;
	ifndef ENGLISH
ENGLISH	equ	0
	endif
	ifndef SPANISH
SPANISH	equ	0
	endif

	include macros.inc

	include ..\cw.inc

MaxBreaks	equ	256
MaxEIPs	equ	4096
MaxWatches	equ	64
MaxWindows	equ	256
MaxLineLength	equ	1024	;Maximum source line length.

RetStackSize	equ	64

	include ..\strucs.inc
	include disas.inc

;
;Hardware break point table entry structure.
;
HBRK	struc
HBRK_Win	dw 0	;display window handle.
HBRK_Handle	dw 0	;DPMI break point handle.
HBRK_Address	dd 0	;Linear break point address.
HBRK_Size	db 0	;DPMI size code to use.
HBRK_Type	db 0	;DPMI type code to use.
HBRK_Flags	dw 0	;padding.
HBRK	ends

	.code

;-------------------------------------------------------------------------
;
;Trap any calls to a null pointer. Mostly for instruction table mistakes
;but it might catch other problems as well.
;
CallZero	proc	near
	mov	ax,DGROUP
	mov	ds,ax
	mov	fs,ax
	mov	gs,ax
	mov	SystemError,4
	pop	eax		;Get return address.
	mov	edi,offset SErrorM04Num
	mov	ecx,8
	call	Bin2Hex
	jmp	system
CallZero	endp

;-------------------------------------------------------------------------
;
;Main entry point.
;
main	proc	near
	mov	ax,DGROUP
	mov	ds,ax
	mov	fs,ax
	mov	gs,ax
	mov	PSPSegment,es
	mov	ax,es:w[2ch]
	mov	ENVSegment,ax
	push	ds
	pop	es

	call	CheckFPU	; check for FPU presence

;
;Get 0-4G selector and current system flags.
;
	push	es
	sys	Info
	mov	si,di
	and	si,1
	and	di,65535-(16384)
	shl	si,14
	or	di,si
	mov	SystemFlags,di
	mov	RealSegment,ax
	pop	es
;
;Check for VGA.
;
	mov	SystemError,5
	mov	ax,1a00h
	int	10h
	cmp	al,1ah
	mov	ax,5
	jne	System
;
;Lock program memory.
;
	mov	SystemError,1
	push	fs
	mov	fs,PSPSegment
	mov	esi,fs:[EPSP_MemBase]
	mov	ecx,fs:[EPSP_MemSize]
	pop	fs
	sys	LockMem32
	jc	System
;
;Get a code segment alias.
;
	mov	SystemError,2
	mov	bx,cs
	sys	AliasSel
	jc	system
	mov	CodeSegAlias,ax
	mov	bx,_EXCEP
	sys	AliasSel
	jc	system
	mov	ECodeSegAlias,ax
;
;Get a selector for video memory access.
;
	sys	GetSel
	jc	system
	mov	VideoSwapSel,bx
;
;Extend programs DS limit.
;
	mov	bx,ds
	sys	GetSelDet32
	mov	ecx,-1
	sys	SetSelDet32
	sys	GetSelDet32
	cmp	ecx,-1
	jz	@@longlimit
	or	OS2TypeMalloc,-1
;
;Move stack into data segment so we can use EBP without overides.
;
@@longLimit:	mov	ax,ds
	mov	ss,ax
	mov	esp,offset DataStack
;
;Set data segments D bit according to program type.
;
	test	SystemFlags,1
	jz	@@no16stack
	mov	bx,ds
	mov	ax,000bh
	push	ds
	pop	es
	mov	edi,offset DescriptorBuffer
	int	31h
	and	DescriptorBuffer+6,255-(1 shl 6) ;clear code size bit.
	mov	ax,000ch
	int	31h
;
;Read configuration file if any.
;
@@no16stack:	call	ReadConfig
;
;Get memory for source file list.
;
	mov	ecx,4
	call	Malloc
	mov	SystemError,1
	jc	System
	mov	d[esi],0
	mov	SourceFileTable,esi
;
;Now setup the mouse.
;
	mov	ax,0
	int	33h
	cmp	ax,0
	jz	@@NoMouse
	mov	MousePresent,1	;flag mouse.
;
;Allocate mouse state save buffers.
;
	mov	ax,15h
	int	33h
	mov	SystemError,3
	mov	ecx,ebx
	call	Malloc
	jc	System
	mov	MouseUserState,esi
	call	Malloc
	jc	System
	mov	MouseOldUserState,esi
	call	Malloc
	jc	System
	mov	MouseDebugState,esi
;
;Set initial Debug mouse state to current.
;
	mov	edx,esi
	mov	ax,16h
	int	33h
;
;Set user old state to current.
;
	mov	edx,MouseOldUserState
	mov	ax,16h
	int	33h
;
;Set debug old state to current.
;
	mov	edx,MouseDebugState
	mov	ax,16h
	int	33h
;
;Allocate video state save buffers.
;
@@NoMouse:	mov	ah,0fh
	int	10h		;get current page.
	mov	UserOldPage,bh
	mov	UserOldMode,al
	;
	;Get 50-line state.
	;
	mov	UserOld50,0
	mov	ax,1130h
	xor	bx,bx
	xor	dx,dx
	push	es
	int	10h
	pop	es
	cmp	dl,49
	jnz	@@U_Not50
	or	UserOld50,-1
@@U_Not50:	;
	mov	ax,1c00h
	mov	cx,1+2+4
	int	10h		;get save buffer size.
	mov	SystemError,3
	movzx	ebx,bx
	shl	ebx,6		;want size in bytes.
	mov	ecx,ebx
	call	Malloc
	jc	System
	mov	VideoUserState,esi
	call	Malloc
	jc	System
	mov	VideoOldUserState,esi
	call	Malloc
	jc	System
	mov	VideoDebugState,esi
;
;Set initial Debug video state to current.
;
	mov	ebx,esi
	mov	ax,1c01h
	mov	cx,1+2+4
	push	ebx
	int	10h		;save state in buffer.
	pop	ebx
	mov	ax,1c02h
	mov	cx,1+2+4
	int	10h		;restore it again.
;
;Set old user video state to current.
;
	mov	ebx,VideoOldUserState
	mov	ax,1c01h
	mov	cx,1+2+4
	push	ebx
	int	10h		;save state in buffer.
	pop	ebx
	mov	ax,1c02h
	mov	cx,1+2+4
	int	10h		;restore it again.
;
;Allocate video memory save buffers.
;
	mov	ecx,65536+4+4
	call	Malloc
	mov	VideoUserBuffer,esi
	mov	d[esi+4],0
	call	Malloc
	mov	VideoOldUserBuffer,esi
	mov	d[esi+4],0
	call	Malloc
	mov	VideoDebugBuffer,esi
	mov	d[esi+4],0
;
;Store current video memory.
;
	;
	;Get current video memory contents.
	;
	mov	ah,0fh
	int	10h		;get current mode.
	mov	UserMode,al
	mov	edx,0b8000h
	mov	ecx,16384
	cmp	al,3
	jz	@@vg1
	cmp	al,4
	jz	@@vg1
	mov	edx,0b0000h
	cmp	al,7	;80*25*16 T
	jc	@@vg1
	jz	@@vg1
	mov	edx,0a0000h
	mov	ecx,65535
	cmp	al,13h	;320*200*256 G
	jz	@@vg1
	mov	edx,0b8000h
	mov	ecx,16384
@@vg1:	mov	esi,VideoOldUserBuffer
	mov	[esi],edx
	mov	[esi+4],ecx
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	cld
	mov	edi,VideoOldUserBuffer
	mov	ecx,[edi+4]
	add	edi,4+4
	xor	esi,esi
	push	ds
	mov	ds,VideoSwapSel
	rep_movsb
	pop	ds
@@vg2:	;
	;Patch INT 9 & INT 31h so we always see keyboard activity
	;and can interrupt the program.
	;
	mov	bl,9
	sys	GetVect
	test	SystemFlags,1
	jz	@@i932
	movzx	edx,dx
@@i932:	push	ds
	mov	ds,CodeSegAlias
	assume ds:_TEXT
	mov	d[OldInt09],edx
	mov	w[OldInt09+4],cx
	assume ds:DGROUP
	pop	ds
	mov	edx,offset BreakChecker
	mov	cx,cs
	mov	bl,9
	sys	SetVect
	mov	bl,31h
	sys	GetVect
	test	SystemFlags,1
	jz	@@i3132
	movzx	edx,dx
@@i3132:	push	ds
	mov	ds,CodeSegAlias
	assume ds:_TEXT
	mov	d[OldInt31],edx
	mov	w[OldInt31+4],cx
	push	es
	mov	es,cx
	mov	ax,es:[edx-2]
	pop	es
	mov	w[cwMajorVersion],ax
	assume ds:DGROUP
	pop	ds
	mov	edx,offset Int31Intercept
	mov	cx,cs
	mov	bl,31h
	sys	SetVect
;
;Check command line for CWD options and retrieve name of program to load.
;
	mov	DebugName,0
	push	es
	mov	es,PSPSegment
	mov	esi,80h
	movzx	ecx,es:b[esi]
	inc	esi
	mov	edi,offset DebugName	;default to storeing program name.
@@c0:	or	ecx,ecx
	jz	@@c1
	cmp	es:b[esi],' '
	jz	@@c2
	cmp	es:b[esi],13
	jz	@@c2
	cmp	es:b[esi],10
	jz	@@c2
	cmp	es:b[esi],'/'	;option switch?
	jnz	@@c3
	cmp	es:w[esi+1],'MD'	;mono display?
	jz	@@og0
	cmp	es:w[esi+1],'mD'
	jz	@@og0
	cmp	es:w[esi+1],'Md'
	jz	@@og0
	cmp	es:w[esi+1],'md'
	jnz	@@o0
@@og0:	mov	MonoSwap,1		;flag want mono display.
	mov	es:b[esi],' '
	mov	es:w[esi+1],'  '
	jmp	@@c0
	;
@@o0:	cmp	es:b[esi+1],'e'	;execute to label?
	jz	@@og1
	cmp	es:b[esi+1],'E'
	jnz	@@o1
@@og1:	mov	es:b[esi],' '
	mov	es:b[esi+1],' '
	add	esi,2
	pushm	edi
	mov	edi,offset EntryGotoText	;store goto expresion.
@@og1_0:	mov	al,es:[esi]
	or	al,al
	jz	@@og1_1
	cmp	al,13
	jz	@@og1_1
	cmp	al,10
	jz	@@og1_1
	cmp	al,' '
	jz	@@og1_1
	mov	[edi],al
	mov	es:b[esi]," "
	inc	edi
	inc	esi
	jmp	@@og1_0
@@og1_1:	mov	b[edi],0
	pop	edi
	jmp	@@c0
@@o1:	;
	cmp	es:b[esi+1],'a'
	jz	@@og2
	cmp	es:b[esi+1],'A'
	jnz	@@o2
@@og2:	mov	es:b[esi],' '
	mov	es:b[esi+1],' '
	add	esi,2
	mov	AssemblerMode,-1
	jmp	@@c0
	;
@@o2:	cmp	es:b[esi+1],"c"
	jz	@@og3
	cmp	es:b[esi+1],"C"
	jnz	@@o3
@@og3:	mov	es:b[esi]," "
	mov	es:b[esi+1]," "
	add	esi,2
	mov	CMode,-1
	jmp	@@c0
@@o3:	;
@@c3:	cmp	ecx,0
	jz	@@c1
	cmp	es:b[esi],' '
	jz	@@c1
	cmp	es:b[esi],13
	jz	@@c1
	cmp	es:b[esi],10
	jz	@@c1
	mov	al,es:[esi]
	mov	es:b[esi],' '
	mov	[edi],al
	inc	esi
	inc	edi
	dec	ecx
	jmp	@@c3
	;
@@c2:	inc	esi
	dec	ecx
	jmp	@@c0
@@c1:	pop	es
;
;Clean up the command line, ie, remove any spaces created by removeing options.
;
	push	es
	mov	es,PSPSegment
	mov	esi,80h
	movzx	ecx,es:b[esi]
	or	ecx,ecx
	jz	@@cl3
	inc	esi
	mov	edi,esi
@@cl0:	cmp	es:b[esi],' '
	jnz	@@cl1
	inc	esi
	dec	ecx
	jnz	@@cl0
@@cl1:	jecxz	@@cl2
	push	ecx
	push	ds
	push	es
	pop	ds
	rep	movsb		;Copy it down.
	pop	ds
	pop	ecx
@@cl2:	mov	es:b[80h],cl		;Store new length.
@@cl3:	movzx	ecx,cl
	add	ecx,81h
	mov	es:b[ecx],13		;Terminate it correctly.
	pop	es
;
;Switch to debug screen/page.
;
	call	DisasScreen
;
;Get memory for window buffers.
;
	mov	SystemError,3
	mov	ecx,32768
	call	Malloc
	jc	System
	mov	SystemError,0
;
;Initialise window stuff.
;
	mov	eax,esi
	mov	bx,MaxWindows
	call	InitWindows
	mov	WindowsOpened,-1
	mov	ScreenWidth,cl
	mov	ScreenDepth,ch
;
;open the title bar window.
;
	call	CursorOFF
	mov	MenuText,offset Copyright
	mov	cl,0
	mov	bl,0
	mov	ch,ScreenWidth
	mov	bh,1
	mov	ax,WindowClear
	mov	dl,70h
	mov	esi,0
	mov	ebp,offset MenuHandler
	call	OpenWindow
	mov	MenuHandle,bp
;
;open the help bar window.
;
	mov	HelpText,offset HelpText1
	mov	cl,0
	mov	bl,ScreenDepth
	dec	bl
	mov	ch,ScreenWidth
	mov	bh,1
	mov	ax,WindowClear
	mov	dl,70h
	mov	esi,0
	mov	ebp,offset HelpHandler
	call	OpenWindow
	mov	HelpHandle,bp
	call	PointWindow
	mov	WindowFill[esi],' '
;
;Create EXE, MAP and SYM file names.
;
	mov	ErrorNumber,4
	cmp	DebugName,0
	jz	@@4
	mov	ErrorNumber,0
	;
@@4:	;Make .EXE file name.
	;
	mov	esi,offset DebugName	;get file name mask.
	mov	edi,offset EXEFileName
	cld
	xor	al,al
@@e0:	movsb
	cmp	b[esi-1],'.'
	jnz	@@e1
	mov	al,1
@@e1:	cmp	b[esi-1],0
	jnz	@@e0
	or	al,al
	jnz	@@e2
	mov	b[edi-1],'.'
	mov	esi,offset EXEextension
@@e4:	movsb
	cmp	b[esi-1],0
	jnz	@@e4
	;
@@e2:	;Generate .MAP file name.
	;
	mov	esi,offset EXEFileName
	mov	edi,offset MapFileName
@@e3:	movsb
	cmp	b[esi-1],'.'
	jnz	@@e3
	mov	esi,offset MAPextension
@@e5:	movsb
	cmp	b[esi-1],0
	jnz	@@e5
	;
	;Generate .SYM file name.
	;
	mov	esi,offset EXEFileName
	mov	edi,offset SymFileName
@@sn0:	movsb
	cmp	b[esi-1],'.'
	jnz	@@sn0
	mov	esi,offset SYMExtension
@@sn1:	movsb
	cmp	b[esi-1],0
	jnz	@@sn1
;
;Check if we need to display an error message yet.
;
	movzx	ebx,ErrorNumber
	or	ebx,ebx
	jz	@@NoE0
	shl	ebx,2
	mov	ebx,[ErrorList+ebx]
	call	WindowPopup
	jmp	System
@@NoE0:	mov	ErrorNumber,0
;
;Patch int 10h for mode checks.
;
	mov	bl,10h
	sys	GetVect
	test	SystemFlags,1
	jz	@@v32_0
	movzx	edx,dx
@@v32_0:	push	ds
	mov	ds,CodeSegAlias
	assume ds:_Text
	mov	w[OldInt10+4],cx
	mov	d[OldInt10],edx
	assume ds:DGROUP
	pop	ds
	mov	cx,cs
	mov	edx,offset Int10Handler
	sys	SetVect
;
;Patch exception 0 interupt.
;
	mov	bl,0
	sys	GetVect
	test	SystemFlags,1
	jz	@@v32_1
	movzx	edx,dx
@@v32_1:	push	ds
	mov	ds,ECodeSegAlias
	assume ds:_EXCEP
	mov	w[OldInt00+4],cx
	mov	d[OldInt00],edx
	assume ds:DGROUP
	pop	ds
	mov	bl,0
	mov	cx,_EXCEP
	mov	edx,offset Int00Handler
	sys	SetVect
;
;Patch debug interupt.
;
	mov	bl,1
	sys	GetVect
	test	SystemFlags,1
	jz	@@v32_20
	movzx	edx,dx
@@v32_20:	push	ds
	mov	ds,ECodeSegAlias
	assume ds:_EXCEP
	mov	w[OldInt01+4],cx
	mov	d[OldInt01],edx
	assume ds:DGROUP
	pop	ds
	mov	bl,1
	mov	cx,_EXCEP
	mov	edx,offset Int01Handler
	sys	SetVect
;
;Patch trap interupt.
;
	mov	bl,3
	sys	GetVect
	test	SystemFlags,1
	jz	@@v32_2
	movzx	edx,dx
@@v32_2:	push	ds
	mov	ds,ECodeSegAlias
	assume ds:_EXCEP
	mov	w[OldInt03+4],cx
	mov	d[OldInt03],edx
	assume ds:DGROUP
	pop	ds
	mov	bl,3
	mov	cx,_EXCEP
	mov	edx,offset Int03Handler
	sys	SetVect
;
;Patch debug interupt.
;
	mov	bl,1
	sys	GetEVect
	test	SystemFlags,1
	jz	@@v32_30
	movzx	edx,dx
@@v32_30:	push	ds
	mov	ds,ECodeSegAlias
	assume ds:_EXCEP
	mov	w[OldEInt01+4],cx
	mov	d[OldEInt01],edx
	assume ds:DGROUP
	pop	ds
	mov	bl,1
	mov	cx,_EXCEP
	mov	edx,offset EInt01Handler
	sys	SetEVect
;
;Patch trap interupt.
;
	mov	bl,3
	sys	GetEVect
	test	SystemFlags,1
	jz	@@v32_3
	movzx	edx,dx
@@v32_3:	push	ds
	mov	ds,ECodeSegAlias
	assume ds:_EXCEP
	mov	w[OldEInt03+4],cx
	mov	d[OldEInt03],edx
	assume ds:DGROUP
	pop	ds
	mov	bl,3
	mov	cx,_EXCEP
	mov	edx,offset EInt03Handler
	sys	SetEVect
;
;Patch exception 12 interupt.
;
	mov	bl,12
	sys	GetEVect
	test	SystemFlags,1
	jz	@@v32_4
	movzx	edx,dx
@@v32_4:	push	ds
	mov	ds,ECodeSegAlias
	assume ds:_EXCEP
	mov	w[OldExc12+4],cx
	mov	d[OldExc12],edx
	assume ds:DGROUP
	pop	ds
	mov	bl,12
	mov	cx,_EXCEP
	mov	edx,offset Exc12Handler
	sys	SetEVect
;
;Patch exception 13 interupt.
;
	mov	bl,13
	sys	GetEVect
	test	SystemFlags,1
	jz	@@v32_5
	movzx	edx,dx
@@v32_5:	push	ds
	mov	ds,ECodeSegAlias
	assume ds:_EXCEP
	mov	w[OldExc13+4],cx
	mov	d[OldExc13],edx
	assume ds:DGROUP
	pop	ds
	mov	bl,13
	mov	cx,_EXCEP
	mov	edx,offset Exc13Handler
	sys	SetEVect
;
;Patch exception 14 interupt.
;
	mov	bl,14
	sys	GetEVect
	test	SystemFlags,1
	jz	@@v32_6
	movzx	edx,dx
@@v32_6:	push	ds
	mov	ds,ECodeSegAlias
	assume ds:_EXCEP
	mov	w[OldExc14+4],cx
	mov	d[OldExc14],edx
	assume ds:DGROUP
	pop	ds
	mov	bl,14
	mov	cx,_EXCEP
	mov	edx,offset Exc14Handler
	sys	SetEVect
;
;Patch CTRL-C handler.
;
	mov	bl,23h
	sys	GetVect
	test	SystemFlags,1
	jz	@@v32_7
	movzx	edx,dx
@@v32_7:	push	ds
	mov	ds,ECodeSegAlias
	assume ds:_EXCEP
	mov	w[OldInt23+4],cx
	mov	d[OldInt23],edx
	assume ds:DGROUP
	pop	ds
	mov	bl,23h
	mov	cx,_EXCEP
	mov	edx,offset Int23Handler
	sys	SetVect
;
;Patch terminate interrupt.
;
	mov	bl,21h
	sys	GetVect
	test	SystemFlags,1
	jz	@@Use32_0
	movzx	edx,dx
@@Use32_0:	push	ds
	mov	ds,ECodeSegAlias
	assume ds:_EXCEP
	mov	w[OldInt21+4],cx
	mov	d[OldInt21],edx
	assume ds:DGROUP
	pop	ds
	mov	bl,21h
	mov	cx,_EXCEP
	mov	edx,offset Int21Handler
	sys	SetVect
;
;Store current vector settings to ensure a clean exit.
;
	call	SaveVectors
;
;Work out length of EXE file name and then open a window with "loading" message.
;
	mov	esi,offset EXEFileName
	mov	ch,0
@@escan0:	lodsb
	inc	ch
	or	al,al
	jnz	@@escan0
	cmp	ch,14
	jnc	@@escanok
	mov	ch,14
@@escanok:	dec	ch
	add	ch,10+1	;loading:
	add	ch,2+1	;border etc.
	mov	al,ch
	shr	al,1
	mov	cl,b[VideoColumns]
	shr	cl,1
	sub	cl,al
	mov	bl,b[VideoRows]
	shr	bl,1
	sub	bl,(3+1)/2
	mov	bh,3+1
	mov	ax,WindowClear+WindowBox+WindowShad
	mov	dl,31h
	mov	dh,3fh
	mov	esi,0
	mov	ebp,0
	call	OpenWindow
	mov	TempHandle,bp
	mov	ebx,offset LoadingText
	call	PrintWindow
	mov	ebx,offset EXEFileName
	call	PrintWindow
;
;Ask CW32 to load target program ready for debugging.
;
	mov	edx,offset EXEFileName
	mov	esi,80h
	mov	es,PSPSegment
	xor	cx,cx
	mov	ax,0fffdh
	int	31h
	pushm	ds,ds,ds
	popm	es,fs,gs
	jnc	@@6
;
;Some sort of error occured so display a message.
;
	mov	ErrorNumber,ax
	movzx	ebx,ErrorNumber
	or	ebx,ebx
	jz	@@NoE1
	shl	ebx,2
	mov	ebx,[ErrorList+ebx]
	call	WindowPopup
@@NoE1:	mov	ErrorNumber,0
	mov	bp,TempHandle
	call	CloseWindow
	jmp	System
;
;Setup initial register values.
;
@@6:	mov	DebugSegs,ebp
	mov	DebugCS,cx
	mov	OldDebugCS,cx
	mov	DebugEIP,edx
	mov	OldDebugEIP,edx
	mov	DebugSS,bx
	mov	OldDebugSS,bx
	mov	DebugESP,eax
	mov	OldDebugESP,eax
	mov	DebugPSP,si
	mov	DebugDS,di
	mov	OldDebugDS,di
	mov	DebugES,si
	mov	OldDebugES,si
	pushfd
	pop	eax
	mov	DebugEFL,eax
	mov	OldDebugEFL,eax
	;
	;Setup a new transfer buffer to stop CWD interfering.
	;
	mov	bx,8192/16
	sys	GetMemDOS
	jc	@@NoBigBuffer
	push	eax
	mov	bx,DebugPSP
	mov	ah,50h
	int	21h
	pop	eax
	mov	bx,ax
	mov	ecx,8192
	sys	SetDOSTrans
	mov	bx,PSPSegment
	mov	ah,50h
	int	21h
@@NoBigBuffer:	;
;
;Fetch symbols from somewhere.
;
	call	FetchSymbols
;
;Check if another error message is needed.
;
@@se0:	movzx	ebx,ErrorNumber
	or	ebx,ebx
	jz	@@NoE2
	shl	ebx,2
	mov	ebx,[ErrorList+ebx]
	call	WindowPopup
@@NoE2:	mov	ErrorNumber,0
	mov	bp,TempHandle
	call	CloseWindow
;
;Init disasembly engine.
;
	mov	ebx,SymbolList
	call	InitDisas
	mov	ax,DebugCS
	mov	DisplayCS,ax
	mov	DisasCS,ax
	mov	eax,DebugEIP
	mov	DisplayEIP,eax
	mov	DisasEIP,eax
	mov	DisasStartEIP,-1
	mov	DisasEndEIP,0

;
;Open the register display window.
;
	mov	cl,59+5+2
	mov	bl,0+1
	mov	ch,14
	mov	bh,23
	mov	ax,WindowClear+WindowBox+WindowStatic
	mov	dl,30h
	mov	dh,3fh
	mov	esi,0
	mov	ebp,offset RegsHandler
	call	OpenWindow		;open main display window.
	mov	RegsHandle,bp
	mov	ebx,offset RegsTitle
	mov	ax,WindowJCent+WindowBox
	call	TitleWindow
;
;Open the default data watch window.
;
	mov	al,Watch_DS	;seg type.
	mov	ah,Watch_Abs	;offset type.
	mov	ebx,0	;offset
	call	OpenDataWatch
;
;Open the disasembly window.
;
	mov	eax,SourceTABSettings+0
	add	eax,4-1
	mov	WindowTabG1,ax
	mov	eax,SourceTABSettings+4
	add	eax,4-1
	mov	WindowTabG2,ax
	mov	eax,SourceTABSettings+8
	add	eax,4-1
	mov	WindowTabG3,ax
	mov	eax,SourceTABSettings+12
	add	eax,4-1
	mov	WindowTabG4,ax
	mov	eax,SourceTABSettings+16
	add	eax,4-1
	mov	WindowTabG5,ax
	mov	eax,SourceTABSettings+20
	add	eax,4-1
	mov	WindowTabG6,ax
	mov	eax,SourceTABSettings+24
	add	eax,4-1
	mov	WindowTabG7,ax
	mov	eax,SourceTABSettings+28
	add	eax,4-1
	mov	WindowTabG8,ax
	mov	cl,0
	mov	bl,0+1
	mov	ch,59+5+2
	mov	bh,23-6
	mov	ax,WindowClear+WindowBox+WindowStatic+WindowXOff
	mov	dl,30h
	mov	dh,3fh
	mov	esi,0
	mov	ebp,offset DisasHandler
	call	OpenWindow
	mov	DisasHandle,bp
;
;See if the default debug start point symbol is defined.
;
	cmp	AssemblerMode,0
	jnz	@@NoEntryError
	cmp	EntryGotoText,0
	jnz	@@nocseip
	mov	esi,offset CW_DEBUG_ENTRY_CS_EIP
	call	GetSymbolValue	;get the value
	jc	@@nocseip
	push	es
	mov	es,dx
	movzx	edx,es:w[ecx+4]
	mov	ecx,es:[ecx]
	pop	es
	mov	d[EvaluateBuffer+0],ecx
	mov	d[EvaluateBuffer+4],edx
	mov	eax,edi
	mov	edi,SymbolList
	cmp	edi,eax
	jnz	@@findsym
	add	edi,SymbolNext[edi]
	mov	SymbolList,edi
	jmp	@@gotcseip
@@findsym:	mov	esi,edi
	add	edi,SymbolNext[edi]
	cmp	edi,eax
	jnz	@@findsym
	mov	eax,SymbolNext[edi]
	cmp	eax,-1
	jz	@@endsym
	add	eax,SymbolNext[esi]
@@endsym:	mov	SymbolNext[esi],eax
	jmp	@@gotcseip
;
;See if we need to do a GOTO
;
@@nocseip:	cmp	EntryGotoText,0
	jz	@@NormalEntry
	mov	esi,offset EntryGotoText
	mov	edi,offset EvaluateBuffer
@@eg1:	movsb
	cmp	b[esi-1],0
	jnz	@@eg1
	mov	EntryGotoOK,-1
	mov	VarSizeMask,0
	mov	VarSizeMask+4,0
	call	EvaluateData
	jc	@@NormalEntry
@@gotcseip:	mov	EntryGotoText,0
	mov	eax,d[EvaluateBuffer]
	mov	DisplayEIP,eax
	mov	ax,DebugCS
	cmp	d[EvaluateBuffer+4],0
	jz	@@eg2
	mov	eax,d[EvaluateBuffer+4]
@@eg2:	mov	DisplayCS,ax
	mov	bx,DisplayCS		;Need segment linear base address
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	edx,ecx
	add	edx,DisplayEIP	;real linear address.
	mov	al,BreakType_exec
	call	SetBreakPoint
	call	CopyDebugRegs
	push	ax
	mov	al,2
	call	ExecuteInst
	pop	ax
	call	RelBreakPoint	;release it then.
	mov	ax,DebugCS
	mov	DisplayCS,ax
	mov	eax,DebugEIP
	mov	DisplayEIP,eax
	call	DisasPart
	call	RegisterDisplay	;Display current register values.
	call	UpdateWatches
	;
@@NormalEntry:	cmp	EntryGotoText,0
	jz	@@NoEntryError
	cmp	EntryGotoOK,0
	jnz	@@NoEntryError
	mov	ebx,offset BadEntryExpresion
	call	WindowPopup
	;
@@NoEntryError:
	;
@@0:	;The main loop.
	;
	mov	ax,Message_Control
	call	WindowMain
	jmp	@@0
Main	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
System	proc	near
	cmp	MousePresent,0
	jz	@@NoRelM
	call	MouseOFF
	mov	ax,0
	int	33h		;reset mouse to lose event handler.
@@NoRelM:	;
	call	RestoreVectors
	;
	cmp	ResetTimer,0
	jz	@@notimer
	xor	ax,ax
	call	LoadTimer
@@notimer:	;
	push	ds
	mov	ds,ECodeSegAlias
	assume ds:_EXCEP
	cmp	w[OldInt23+4],0
	jz	@@9
	mov	edx,d[OldInt23]
	mov	cx,w[OldInt23+4]
	mov	bl,23h
	sys	SetVect
@@9:	;
	cmp	w[OldInt21+4],0
	jz	@@0
	mov	edx,d[OldInt21]
	mov	cx,w[OldInt21+4]
	mov	bl,21h
	sys	SetVect
@@0:	;
	cmp	w[OldInt01+4],0
	jz	@@10
	mov	cx,w[OldInt01+4]
	mov	edx,d[OldInt01]
	mov	bl,1
	sys	SetVect
@@10:	;
	cmp	w[OldEInt01+4],0
	jz	@@20
	mov	cx,w[OldEInt01+4]
	mov	edx,d[OldEInt01]
	mov	bl,1
	sys	SetEVect
@@20:	;
	cmp	w[OldEInt03+4],0
	jz	@@30
	mov	cx,w[OldEInt03+4]
	mov	edx,d[OldEInt03]
	mov	bl,3
	sys	SetVect
@@30:	;
	cmp	w[OldInt03+4],0
	jz	@@1
	mov	cx,w[OldInt03+4]
	mov	edx,d[OldInt03]
	mov	bl,3
	sys	SetVect
	;
@@1:	cmp	w[OldExc12+4],0
	jz	@@2
	mov	cx,w[OldExc12+4]
	mov	edx,d[OldExc12]
	mov	bl,12
	sys	SetEVect
	;
@@2:	cmp	w[OldExc13+4],0
	jz	@@3
	mov	cx,w[OldExc13+4]
	mov	edx,d[OldExc13]
	mov	bl,13
	sys	SetEVect
@@3:	;
	cmp	w[OldExc14+4],0
	jz	@@4
	mov	cx,w[OldExc14+4]
	mov	edx,d[OldExc14]
	mov	bl,14
	sys	SetEVect
@@4:	;
	cmp	w[OldInt00+4],0
	jz	@@8
	mov	cx,w[OldInt00+4]
	mov	edx,d[OldInt00]
	mov	bl,0
	sys	SetVect
	;
@@8:	assume ds:DGROUP
	pop	ds
	cmp	w[OldInt10+4],0
	jz	@@7
	mov	edx,d[OldInt10]
	mov	cx,w[OldInt10+4]
	mov	bl,10h
	sys	SetVect
	;
@@7:	cmp	WindowsOpened,0
	jz	@@5
	call	CloseWindows
	;
	cmp	MonoSwap,0
	jz	@@NoMono
	;
	mov	esi,VideoUserBuffer
	mov	edx,[esi]
	mov	ecx,[esi+4]
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	add	esi,4+4
	xor	edi,edi
	push	es
	mov	es,VideoSwapSel
	rep_movsb
	pop	es
	;
	push	es
	mov	ax,40h
	mov	es,ax
	and	es:b[10h],11001111b
	or	es:b[10h],00100000b
	pop	es
@@NoMono:	;
	mov	al,UserOldMode
	xor	ah,ah
	int	10h
	;
	;Force font.
	;
	cmp	UserOld50,0
	jz	@@U_Not50
	mov	ax,1112h
	xor	bx,bx
	int	10h
@@U_Not50:	;
	mov	ebx,VideoOldUserState
	mov	ax,1c02h
	mov	cx,1+2+4
	int	10h
	;
	mov	esi,VideoOldUserBuffer
	mov	edx,[esi]
	mov	ecx,[esi+4]
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	add	esi,4+4
	xor	edi,edi
	push	es
	mov	es,VideoSwapSel
	rep_movsb
	pop	es
	;
	mov	ah,5
	mov	al,UserOldPage
	int	10h
@@5:	;
@@done:	assume ds:nothing
	cmp	cs:w[OldInt31+4],0
	jz	@@noi31
	mov	edx,cs:d[OldInt31]
	mov	cx,cs:w[OldInt31+4]
	mov	bl,31h
	sys	SetVect
	assume ds:DGROUP
	;
@@noi31:	assume ds:nothing
	cmp	cs:w[OldInt09+4],0
	jz	@@noi9
	mov	edx,cs:d[OldInt09]
	mov	cx,cs:w[OldInt09+4]
	mov	bl,9
	sys	SetVect
	assume ds:DGROUP
@@noi9:	;
	cmp	SystemError,0
	jz	@@6
	movzx	edx,SystemError
	mov	edx,[SErrorList+edx*4]
	call	StringPrint
	;
@@6:	movzx	eax,SystemError
	mov	ah,4ch
	int	21h
System	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;This should recieve ALL keyboard interrupts before anything else gets to see
;them.
;
BreakChecker	proc	near
	pushm	eax,ebx,ebp,ds
	mov	ax,DGROUP
	mov	ds,ax
	inc	InInt09
;
;Update the key table.
;
	in	al,60h		;get the scan code.
	mov	bl,al
	and	ebx,127		;isolate scan code.
	add	ebx,offset KeyTable
	and	al,128
	xor	al,128
	mov	[ebx],al		;set key state.
;
;Check we havn't already been here.
;
	cmp	InInt09,2
	jnc	@@old
;
;Check if anything is running.
;
	cmp	Executing,0
	jz	@@old
;
;Check if our break combination is set.
;
	mov	ebx,offset BreakKeyList
	cmp	d[ebx],0		;check if any keys in the list.
	jz	@@old
@@6:	cmp	d[ebx],0		;End of the list?
	jz	@@7
	mov	eax,d[ebx]		;Get scan code.
	cmp	b[KeyTable+eax],0
	jz	@@old
	add	ebx,4
	jmp	@@6
;
;Check if return CS:EIP & stack belong to the program we're running.
;
@@7:	pushad
	test	SystemFlags,1
	jz	@@0
	movzx	ebp,sp
	add	ebp,4+4+4+4+(4*8)+2+2+2
	movzx	ecx,w[ebp+2]		;return CS
	movzx	edx,w[ebp]		;return EIP
	jmp	@@1
	;
@@0:	mov	ebp,esp
	add	ebp,4+4+4+4+(4*8)+4+4+4
	mov	ecx,d[ebp+4]		;return CS
	mov	edx,d[ebp]
	;
@@1:	push	es
	mov	es,DebugPSP
	mov	ax,es:[EPSP_SegBase]
	pop	es
	cmp	cx,ax
	jc	@@nope
	mov	DebugEIP,edx
	mov	DebugCS,cx
;
;Want to break into the program so swollow this key press.
;
	in	al,61h
	mov	ah,al
	or	al,1 shl 7
	out	61h,al		;say comming back.
	xchg	ah,al
	out	61h,al		;code we got.
	;
	mov	al,32
	out	20h,al		;re-enable interupts.
;
;Swap the return address for our own.
;
	test	SystemFlags,1
	jz	@@2
	movzx	ebp,sp
	add	ebp,4+4+4+4+(4*8)+2+2+2
	mov	eax,offset @@3
	mov	w[ebp+2],cs		;return CS
	mov	w[ebp],ax		;return EIP
	popad
	popm	eax,ebx,ebp,ds
	iret
	;
@@2:	mov	ebp,esp
	add	ebp,4+4+4+4+(4*8)+4+4+4
	mov	w[ebp+4],cs		;return CS
	mov	d[ebp],offset @@3
	popad
	popm	eax,ebx,ebp,ds
	iretd
;
;We should be running on the applications stack with the applications registers
;now.
;
@@3:	pushfd
	pushm	eax,ebp,ds
	mov	ax,DGROUP
	mov	ds,ax
	mov	ebp,esp
	test	SystemFlags,1
	jz	@@4
	movzx	ebp,bp
@@4:	mov	eax,[ebp+4+4]
	mov	DebugEAX,eax
	mov	DebugEBX,ebx
	mov	DebugECX,ecx
	mov	DebugEDX,edx
	mov	DebugESI,esi
	mov	DebugEDI,edi
	mov	eax,[ebp+4]
	mov	DebugEBP,eax
	mov	eax,[ebp]
	mov	DebugDS,ax
	mov	DebugES,es
	mov	DebugFS,fs
	mov	DebugGS,gs
	mov	eax,[ebp+4+4+4]
	mov	DebugEFL,eax
	mov	DebugSS,ss
	mov	DebugESP,esp
	add	DebugESP,4+4+4+4
;
;Return to the debugger exec routine.
;
	mov	ax,DGROUP
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	dec	InInt09
	lss	esp,f[DebuggerESP]
	test	SystemFlags,1
	jz	@@5
	db 66h
@@5:	retf
;
@@nope:	popad
;
;Pass control to the origional handler.
;
@@old:	dec	InInt09
	popm	eax,ebx,ebp,ds
	assume ds:nothing
	jmp	cs:f[OldInt09]
	assume ds:DGROUP
OldInt09	df 0
BreakChecker	endp

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;This must be just before cwAPIpatch
;
cwIdentity	db "CAUSEWAY"
cwMajorVersion	db 0
cwMinorVersion	db 0

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;This intercepts get/set int 9 vector stuff and makes sure BreakChecker stays
;first in the chain. Also monitor exec calls.
;
Int31Intercept	proc	near
	cmp	ax,0205h		;Set vector?
	jnz	@@notset
	cmp	bl,9		;INT 9?
	jnz	@@old
	pushm	eax,edx,ds
	mov	ax,DGROUP
	mov	ds,ax
	test	SystemFlags,1
	jz	@@0
	movzx	edx,dx
@@0:	mov	ds,CodeSegAlias
	assume ds:_TEXT
	mov	d[OldInt09],edx
	mov	w[OldInt09+4],cx
	assume ds:DGROUP
	popm	eax,edx,ds
	jmp	@@ret
	;
@@notset:	cmp	ax,0204h		;Get vector?
	jnz	@@old
	cmp	bl,9		;INT 9?
	jnz	@@old
	assume ds:nothing
	mov	edx,cs:d[OldInt09]
	mov	cx,cs:w[OldInt09+4]
	assume ds:DGROUP
	;
@@ret:	pushm	eax,ebp,ds
	mov	ax,DGROUP
	mov	ds,ax
	test	SystemFlags,1
	jz	@@r32
	movzx	ebp,sp
	add	ebp,4+4+4+2+2
	and	ss:w[ebp],not 1	;clear carry.
	popm	eax,ebp,ds
	iret
@@r32:	mov	ebp,esp
	add	ebp,4+4+4+4+4
	and	ss:w[ebp],not 1	;clear carry.
	popm	eax,ebp,ds
	iretd
	;
@@old:	assume ds:nothing
	jmp	cs:f[OldInt31]
	assume ds:DGROUP
OldInt31	df 0
Int31Intercept	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Read config file if one exists.
;
ReadConfig	proc	near
	call	InitFileBuffer
	mov	edx,offset ConfigName
	call	OpenFile
	jnc	@@ConfigOK
@@NotCurrent:	;
	push	es
	sys	Info
	mov	es,bx
	mov	es,es:w[2ch]
	xor	esi,esi
	xor	ebp,ebp
@@c1:	mov	al,es:[esi]		;Get a byte.
	inc	esi		;/
	or	al,al		;End of a string?
	jnz	@@c1		;keep looking.
	mov	al,es:[esi]		;Double zero?
	or	al,al		;/
	jnz	@@c1		;keep looking.
	add	esi,3		;Skip last 0 and word count.
	mov	edi,offset ConfigPath
	pushm	ds
	pushm	ds,es
	popm	es,ds
@@c2:	movsb
	cmp	b[esi-1],'\'
	jnz	@@c3
	mov	ebp,edi
@@c3:	cmp	b[esi-1],0		;got to the end yet?
	jnz	@@c2
	or	ebp,ebp
	jnz	@@c99
	mov	ebp,edi
@@c99:	pop	ds
	pop	es
	dec	ebp
	mov	edi,ebp
	mov	es:b[edi],'\'
	inc	edi
	mov	esi,offset ConfigName
@@c4:	movsb
	cmp	b[esi-1],0
	jnz	@@c4
	;
	mov	edx,offset ConfigPath	;use new name.
	call	OpenFile
	jc	@@8		;don't have to have a config.
	;
@@ConfigOK:	mov	ConfigHandle,ax
	;
@@Read:	mov	bx,ConfigHandle
	mov	edi,offset LineBuffer
	call	ReadLine		;read a line.
	or	ecx,ecx		;end of the file?
	jz	@@7
	cmp	cx,1024
	jnc	@@90
	;
@@Scan:	;Check this line for variables.
	;
	mov	edx,offset LineBuffer
	mov	ebp,offset VariableList
	mov	edi,edx
	cmp	b[edi],';'
	jz	@@Read		;comment so ignore it.
	cmp	b[edi],0		;blank line?
	jz	@@Read
	;
@@0:	cmp	ds:d[ebp],-1		;end of the list?
	jz	@@90
	mov	esi,ds:[ebp]		;get text pointer.
	mov	edi,edx		;source data.
	;
@@1:	cmp	b[edi],'='		;end of the string?
	jnz	@@3
	cmp	b[esi],0		;end of our version as well?
	jz	@@4
	;
@@3:	cmp	b[edi],0		;end of the line?
	jz	@@90
	;
	cmp	b[esi],0		;end of the text?
	jz	@@5
	;
	mov	al,[esi]
	call	UpperChar
	xchg	ah,al
	mov	al,[edi]
	call	UpperChar
	cmp	al,ah		;match?
	jz	@@2
	;
@@5:	add	ebp,16		;next variable.
	jmp	@@0
	;
@@2:	inc	esi
	inc	edi
	jmp	@@1
	;
@@4:	inc	edi
	call	ds:d[ebp+4]		;call the handler code.
	jz	@@Read
	jmp	@@90
	;
@@7:	mov	bx,ConfigHandle	;close the file again.
	call	CloseFile
	jmp	@@10
	;
@@8:	;
@@10:	mov	ErrorNumber,0
	xor	ax,ax
	ret
	;
@@90:	;
@@9:	mov	ax,-1
	or	ax,ax
	ret
ReadConfig	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-=-=-=-=-=-=-=-=-=-=
;
;Retrieve a string and convert multiple white space into single spaces. Last white space is line
;terminator.
;
WhiteSpaceString proc near
	mov	esi,edi
	mov	_CFG_Source,esi
	mov	edi,ds:[ebp+8]	;get target address.
@@30:	mov	al,[edi]
	or	al,al
	jz	@@AtStart
	inc	edi
	jmp	@@30
@@AtStart:	;
	cmp	edi,ds:[ebp+8]
	jz	@@NoTerm
	cmp	b[edi-1],";"
	jz	@@NoTerm
	mov	b[edi],';'
	inc	edi
@@NoTerm:	;
	push	ds
	pop	es
	xor	ah,ah		;clear spacing flag.
@@0:	lodsb
	stosb
	cmp	b[esi-1],0		;end of the string?
	jz	@@1
	cmp	b[esi-1],' '		;need multiple space check?
	jz	@@2
	cmp	b[esi-1],9
	jz	@@2
	xor	ah,ah		;clear spacing flag.
	jmp	@@0
	;
@@2:	or	ah,ah		;this part 2?
	jnz	@@3
	mov	b[edi-1],' '		;make sure its a space.
	mov	ah,1		;signal spacing start.
	jmp	@@0
	;
@@3:	dec	edi		;move back to last one.
	jmp	@@0
	;
@@1:	dec	edi		;back to terminator.
	cmp	edi,ds:[ebp+8]	;back at the start yet?
	jz	@@4
	cmp	b[edi-1],' '		;trailing space?
	jnz	@@4
	dec	edi
@@4:	;
@@7:	mov	b[edi],0		;terminate the line.
	;
	xor	ax,ax
	ret
	;
@@9:	mov	ax,-1
	or	ax,ax
	ret
WhiteSpaceString endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-=-=-=-=-=-=-=-=-=-=
;
;Check a string for YES or NO. Set variable to zero if NO, -1 if YES, no change
;if not YES or NO.
;
YesNoString	proc near
	mov	esi,edi
	mov	edi,ds:[ebp+8]	;get target address.
	push	esi
@@0:	mov	al,[esi]
	or	al,al
	jz	@@1
	call	UpperChar
	mov	[esi],al
	inc	esi
	jmp	@@0
@@1:	pop	esi
	xor	eax,eax
	cmp	w[esi],"ON"
	jz	@@8
	cmp	w[esi],"NO"
	jz	@@8
	cmp	w[esi],"EY"
	jnz	@@2
	cmp	b[esi+2],"S"
	jz	@@7
@@2:	cmp	w[esi],"FO"
	jnz	@@9
	cmp	w[esi],"F"
	jnz	@@9
	;
@@7:	or	eax,-1
	;
@@8:	mov	[edi],eax
	;
@@9:	xor	ax,ax
	ret
YesNoString	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-=-=-=-=-=-=-=-=-=-=
;
;Convert comma delimited list of numbers into real values.
;
CommaString proc near
	mov	esi,edi
	mov	edi,ds:[ebp+8]	;get target address.
	;
@@0:	mov	al,[esi]
	cmp	al,";"
	jz	@@8
	or	al,al
	jz	@@8
	inc	esi
	cmp	al," "
	jz	@@0
	cmp	al,9
	jz	@@0
	dec	esi
	;
	xor	edx,edx
@@1:	mov	al,[esi]
	or	al,al
	jz	@@2
	inc	esi
	cmp	al,","
	jz	@@2
	cmp	al," "
	jz	@@2
	cmp	al,";"
	jz	@@2
	cmp	al,13
	jz	@@2
	cmp	al,10
	jz	@@2
	cmp	al,"0"
	jc	@@9
	cmp	al,"9"+1
	jnc	@@9
	sub	al,"0"
	movzx	eax,al
	shl	edx,1
	mov	ebx,edx
	shl	edx,2
	add	edx,ebx
	add	edx,eax
	jmp	@@1
	;
@@2:	mov	[edi],edx
	add	edi,4
	jmp	@@0
	;
@@8:	xor	ax,ax
	ret
	;
@@9:	mov	ax,-1
	or	ax,ax
	ret
CommaString endp

;------------------------------------------------------------------------------
;
;Loads the timer with value specified.
;
;On Entry:
;
;AX - Value to load timer with.
;
;On Exit:
;
;All registers preserved.
;
LoadTimer	proc	near
	cli
	push	eax
	push	eax
	mov	al,36h
	out	43h,al
	jmp	@@1
@@1:	jmp	@@2
@@2:	pop	eax
	out	40h,al
	mov	al,ah
	out	40h,al
	in	al,21h
	and	al,254
	out	21h,al
	pop	eax
	sti
	ret
LoadTimer	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RestartALL	proc	near
	mov	TerminationFlag,0
;
;Restore vector settings.
;
	call	RestoreVectors	;restore vectors.
	cmp	ResetTimer,0
	jz	@@notimer
	xor	ax,ax
	call	LoadTimer
@@notimer:
;
;Lose source & symbol memory
;
	xor	esi,esi
	xchg	esi,SymbolBase
	or	esi,esi
	jz	@@fl0
	call	Free
@@fl0:	xor	esi,esi
	xchg	esi,LINEList
	or	esi,esi
	jz	@@fl1
	call	Free
@@fl1:	mov	edi,SourceFileTable
	mov	ecx,[edi]
	or	ecx,ecx
	jz	@@fl3
	add	edi,4
@@fl2:	mov	ebp,[edi]
	mov	esi,ds:SFT_File[ebp]
	or	esi,esi
	jz	@@fl4
	call	Free
@@fl4:	mov	esi,ds:SFT_Lines[ebp]
	or	esi,esi
	jz	@@fl5
	call	Free
@@fl5:	add	edi,4
	dec	ecx
	jnz	@@fl2
	mov	esi,SourceFileTable
	mov	ecx,4
	call	ReMalloc
	mov	d[esi],0
	mov	SourceFileTable,esi
;
@@fl3:	mov	SourceIndex,0
;
;Close all files.
;
	mov	ax,0fffeh
	int	31h
;
;Release PSP etc.
;
	pushm	ds,ds,ds
	popm	es,fs,gs
	mov	bx,DebugPSP
	sys	RelMem		;release memory.
;
;Lose all break points.
;
	mov	esi,offset BreakPointList
	mov	ecx,MaxBreaks
@@b0:	mov	BreakFlags[esi],0
	add	esi,size BreakStruc
	loop	@@b0
;
;Swap back to user screen.
;
	or	NoContextSwitch,-1
	call	UserScreen
	mov	NoContextSwitch,0

	call	MouseOFF
	cmp	MonoSwap,0
	jz	@@NoMono
	push	es
	mov	ax,40h
	mov	es,ax
	and	es:b[10h],11001111b
	or	es:b[10h],00100000b
	pop	es
@@NoMono:	;
	mov	al,UserOldMode
	xor	ah,ah
	int	10h
	;
	;Force font.
	;
	cmp	UserOld50,0
	jz	@@U_Not50
	mov	ax,1112h
	xor	bx,bx
	int	10h
@@U_Not50:	;
	mov	ebx,VideoOldUserState
	mov	ax,1c02h
	mov	cx,1+2+4
	int	10h
	;
	mov	esi,VideoOldUserBuffer
	mov	edx,[esi]
	mov	ecx,[esi+4]
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	add	esi,4+4
	xor	edi,edi
	push	es
	mov	es,VideoSwapSel
	rep_movsb
	pop	es
	;
	mov	ah,5
	mov	al,UserOldPage
	int	10h
	;
	mov	al,FlipSwap
	push	eax
	mov	FlipSwap,1
;
;Now come back to debug screen.
;
	mov	VidSwapMode,0
	call	DisasScreen
	mov	AutoFlip,0
	pop	eax
	mov	FlipSwap,al
;
;Work out length of EXE file name and then open a window with "loading" message.
;
	mov	esi,offset EXEFileName
	mov	ch,0
@@escan0:	lodsb
	inc	ch
	or	al,al
	jnz	@@escan0
	cmp	ch,14
	jnc	@@escanok
	mov	ch,14
@@escanok:	dec	ch
	add	ch,10+1	;loading:
	add	ch,2+1	;border etc.
	mov	al,ch
	shr	al,1
	mov	cl,b[VideoColumns]
	shr	cl,1
	sub	cl,al
	mov	bl,b[VideoRows]
	shr	bl,1
	sub	bl,(3+1)/2
	mov	bh,3+1
	mov	ax,WindowClear+WindowBox+WindowShad
	mov	dl,31h
	mov	dh,3fh
	mov	esi,0
	mov	ebp,0
	call	OpenWindow
	mov	TempHandle,bp
	mov	ebx,offset LoadingText
	call	PrintWindow
	mov	ebx,offset EXEFileName
	call	PrintWindow
;
;Ask CW32 to load target program ready for debugging.
;
	mov	edx,offset EXEFileName
	mov	esi,80h
	mov	es,PSPSegment
	xor	cx,cx
	mov	ax,0fffdh
	int	31h
	pushm	ds,ds,ds
	popm	es,fs,gs
	jnc	@@6
;
;Some sort of error occured so display a message.
;
	mov	ErrorNumber,ax
	movzx	ebx,ErrorNumber
	or	ebx,ebx
	jz	@@NoE1
	shl	ebx,2
	mov	ebx,[ErrorList+ebx]
	call	WindowPopup
@@NoE1:	mov	ErrorNumber,0
	mov	bp,TempHandle
	call	CloseWindow
	jmp	System
;
;Setup initial register values.
;
@@6:	mov	DebugSegs,ebp
	mov	DebugCS,cx
	mov	OldDebugCS,cx
	mov	DebugEIP,edx
	mov	OldDebugEIP,edx
	mov	DebugSS,bx
	mov	OldDebugSS,bx
	mov	DebugESP,eax
	mov	OldDebugESP,eax
	mov	DebugPSP,si
	mov	DebugDS,di
	mov	OldDebugDS,di
	mov	DebugES,si
	mov	OldDebugES,si
	mov	DebugFS,0
	mov	OldDebugFS,0
	mov	DebugGS,0
	mov	OldDebugGS,0
	mov	DebugEAX,0
	mov	OldDebugEAX,0
	mov	DebugEBX,0
	mov	OldDebugEBX,0
	mov	DebugECX,0
	mov	OldDebugECX,0
	mov	DebugEDX,0
	mov	OldDebugEDX,0
	mov	DebugESI,0
	mov	OldDebugESI,0
	mov	DebugEDI,0
	mov	OldDebugEDI,0
	mov	DebugEBP,0
	mov	OldDebugEBP,0
	pushfd
	pop	eax
	mov	DebugEFL,eax
	mov	OldDebugEFL,eax
;
;Setup a new transfer buffer to stop CWD interfering.
;
	mov	bx,8192/16
	sys	GetMemDOS
	jc	@@NoBigBuffer
	push	eax
	mov	bx,DebugPSP
	mov	ah,50h
	int	21h
	pop	eax
	mov	bx,ax
	mov	ecx,8192
	sys	SetDOSTrans
	mov	bx,PSPSegment
	mov	ah,50h
	int	21h
@@NoBigBuffer:	;
;
;Fetch symbols from somewhere.
;
	call	FetchSymbols
;
;Check if another error message is needed.
;
@@se0:	movzx	ebx,ErrorNumber
	or	ebx,ebx
	jz	@@NoE2
	shl	ebx,2
	mov	ebx,[ErrorList+ebx]
	call	WindowPopup
@@NoE2:	mov	ErrorNumber,0
	mov	bp,TempHandle
	call	CloseWindow
;
;See if the default debug start point symbol is defined.
;
	cmp	AssemblerMode,0
	jnz	@@NoEntryError
	cmp	EntryGotoText,0
	jnz	@@nocseip
	mov	esi,offset CW_DEBUG_ENTRY_CS_EIP
	call	GetSymbolValue	;get the value
	jc	@@nocseip
	push	es
	mov	es,dx
	movzx	edx,es:w[ecx+4]
	mov	ecx,es:[ecx]
	pop	es
	mov	d[EvaluateBuffer+0],ecx
	mov	d[EvaluateBuffer+4],edx
	mov	eax,edi
	mov	edi,SymbolList
	cmp	edi,eax
	jnz	@@findsym
	add	edi,SymbolNext[edi]
	mov	SymbolList,edi
	jmp	@@gotcseip
@@findsym:	mov	esi,edi
	add	edi,SymbolNext[edi]
	cmp	edi,eax
	jnz	@@findsym
	mov	eax,SymbolNext[edi]
	cmp	eax,-1
	jz	@@endsym
	add	eax,SymbolNext[esi]
@@endsym:	mov	SymbolNext[esi],eax
	jmp	@@gotcseip
;
;See if we need to do a GOTO
;
@@nocseip:	cmp	EntryGotoText,0
	jz	@@NormalEntry
	mov	esi,offset EntryGotoText
	mov	edi,offset EvaluateBuffer
@@eg1:	movsb
	cmp	b[esi-1],0
	jnz	@@eg1
	mov	EntryGotoOK,-1
	mov	VarSizeMask,0
	mov	VarSizeMask+4,0
	call	EvaluateData
	jc	@@NormalEntry
@@gotcseip:	mov	EntryGotoText,0
	mov	eax,d[EvaluateBuffer]
	mov	DisplayEIP,eax
	mov	ax,DebugCS
	cmp	d[EvaluateBuffer+4],0
	jz	@@eg2
	mov	eax,d[EvaluateBuffer+4]
@@eg2:	mov	DisplayCS,ax
	mov	bx,DisplayCS		;Need segment linear base address
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	edx,ecx
	add	edx,DisplayEIP	;real linear address.
	mov	al,BreakType_exec
	call	SetBreakPoint
	call	CopyDebugRegs
	push	ax
	mov	al,2
	call	ExecuteInst
	pop	ax
	call	RelBreakPoint	;release it then.
	mov	ax,DebugCS
	mov	DisplayCS,ax
	mov	eax,DebugEIP
	mov	DisplayEIP,eax
	call	DisasPart
	call	RegisterDisplay	;Display current register values.
	call	UpdateWatches
	;
@@NormalEntry:	cmp	EntryGotoText,0
	jz	@@NoEntryError
	cmp	EntryGotoOK,0
	jnz	@@NoEntryError
	mov	ebx,offset BadEntryExpresion
	call	WindowPopup
	;
@@NoEntryError:
	mov	ax,DebugCS
	mov	DisplayCS,ax
	mov	eax,DebugEIP
	mov	DisplayEIP,eax
	call	DisasPart
	call	RegisterDisplay	;Display current register values.
	call	UpdateWatches
	ret
RestartALL	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Int10Handler	proc	near
	cmp	ah,0		;mode set?
	jnz	@@NotUs
	push	ds
	push	ax
	mov	ax,DGROUP
	mov	ds,ax
	cmp	Executing,0
	pop	ax
	pop	ds
	jz	@@NotUs
	push	ds
	push	ax
	mov	ax,DGROUP
	mov	ds,ax
	cmp	ForcedFlip,0
	pop	ax
	pop	ds
	jnz	@@NotUs
	pushad
	pushm	ds,es,fs,gs
	push	ax
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	pop	ax
	cmp	AutoFlip,0
	jnz	@@CheckMode
	mov	AutoFlip,1
	mov	FlipSwap,1
	jmp	@@Done
	;
@@CheckMode:	cmp	al,DisasMode
	jnz	@@Done
	mov	AutoFlip,0
	;
@@Done:	popm	ds,es,fs,gs
	popad
	assume ds:nothing
@@NotUs:	jmp	cs:f[OldInt10]
	assume ds:DGROUP
OldInt10	df 0
Int10Handler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SaveVectors	proc	near
	mov	edi,offset VectorList
	mov	d[edi],-1
	add	edi,4
	;
	mov	ebp,256
	xor	bl,bl
@@0:	pushm	ebx,edi,ebp
	sys	GetVect
	popm	ebx,edi,ebp
	mov	[edi],edx
	mov	[edi+4],cx
	add	edi,6
	inc	bl
	dec	ebp
	jnz	@@0
	;
	mov	ebp,32
	xor	bl,bl
@@1:	pushm	ebx,edi,ebp
	sys	GetEVect
	popm	ebx,edi,ebp
	mov	[edi],edx
	mov	[edi+4],cx
	add	edi,6
	inc	bl
	dec	ebp
	jnz	@@1
	;
	mov	ebp,256
	xor	bl,bl
@@2:	pushm	ebx,edi,ebp
	sys	GetRVect
	popm	ebx,edi,ebp
	mov	[edi],dx
	mov	[edi+2],cx
	add	edi,4
	inc	bl
	dec	ebp
	jnz	@@2
	;
	ret
SaveVectors	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RestoreVectors	proc	near
	mov	edi,offset VectorList
	cmp	d[edi],0
	jz	@@9
	add	edi,4
	;
	mov	ebp,256
	xor	bl,bl
@@0:	pushm	ebx,edi,ebp
	mov	edx,[edi]
	mov	cx,[edi+4]
	sys	SetVect
	popm	ebx,edi,ebp
	add	edi,6
	inc	bl
	dec	ebp
	jnz	@@0
	;
	mov	ebp,32
	xor	bl,bl
@@1:	pushm	ebx,edi,ebp
	mov	edx,[edi]
	mov	cx,[edi+4]
	sys	SetEVect
	popm	ebx,edi,ebp
	add	edi,6
	inc	bl
	dec	ebp
	jnz	@@1
	;
	mov	ebp,256
	xor	bl,bl
@@2:	pushm	ebx,edi,ebp
	mov	dx,[edi]
	mov	cx,[edi+2]
	sys	SetRVect
	popm	ebx,edi,ebp
	add	edi,4
	inc	bl
	dec	ebp
	jnz	@@2
	;
@@9:	ret
RestoreVectors	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Open up a data watch window.
;
;On Entry:-
;
;AL	- Segment type.
;AH	- Offset type.
;EBX	- offset if needed.
;CX	- selector if needed.
;
OpenDataWatch	proc	near
	push	ecx
	mov	esi,offset DataWatchList
	mov	ecx,MaxWatches
@@0:	test	WatchFlags[esi],1	;in use?
	jz	@@1
	add	esi,size WatchStruc
	loop	@@0
	pop	ecx
	jmp	@@9
	;
@@1:	pop	ecx
	mov	WatchSeg[esi],al
	mov	WatchOff[esi],ah
	mov	WatchOffset[esi],ebx
	mov	WatchSelNum[esi],cx
	mov	WatchXPos[esi],0
	mov	WatchYPos[esi],0
	mov	WatchLastX[esi],-1
	mov	WatchLastY[esi],-1
	mov	WatchMoved[esi],0
	pushad
	;
	;Generate watch number text for the title.
	;
	mov	eax,esi
	sub	eax,offset DataWatchList
	xor	edx,edx
	mov	ecx,size WatchStruc
	div	ecx
	inc	eax
	mov	edi,offset WatchTitleNumt
	mov	ecx,2
	call	Bin2Hex
	;
	;Generate seg type.
	;
	movzx	eax,WatchSeg[esi]
	push	esi
	lea	esi,[WatchSegList+eax*2]
	mov	edi,offset WatchTitleSeg
	movsw
	pop	esi
	movzx	eax,WatchOff[esi]
	cmp	al,Watch_Abs
	jz	@@2
	sub	al,Watch_EAX-1
@@2:	push	esi
	lea	esi,[WatchOffList+eax*4]
	mov	edi,offset WatchTitleOff
	movsw
	movsb
	pop	esi
	popad
	;
	;Get memory for title.
	;
	push	esi
	mov	ecx,WatchTLen
	call	Malloc
	mov	edx,esi
	pop	esi
	jc	@@9
	;
	;Copy title
	;
	push	esi
	mov	edi,edx
	mov	esi,offset WatchTitleText
	mov	ecx,WatchTLen
	rep	movsb
	pop	esi
	mov	WatchTitle[esi],edx
	push	esi
	;
	;Open the window.
	;
	mov	cl,0
	mov	bl,23-6+1
	mov	ch,59+5+2
	mov	bh,6
	mov	ax,WindowClear+WindowBox+WindowStatic
	mov	dl,30h
	mov	dh,3fh
	mov	esi,0
	mov	ebp,offset WatchHandler
	call	OpenWindow		;open main display window.
	pop	esi
	;
	;Fill in watch details.
	;
	mov	WatchHandle[esi],bp
	mov	WatchFlags[esi],1
	mov	WatchMoved[esi],0
	mov	ebx,WatchTitle[esi]
	mov	ax,WindowJCent+WindowBox
	call	TitleWindow
	mov	ax,Message_Update
	call	MessageWindow
@@9:	ret
OpenDataWatch	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Update all active watches.
;
UpdateWatches	proc	near
	mov	ecx,MaxWatches
	mov	esi,offset DataWatchList
@@0:	test	WatchFlags[esi],1
	jz	@@1
	pushm	esi,ecx
	call	UpdateWatch
	popm	esi,ecx
@@1:	add	esi,size WatchStruc
	loop	@@0
	ret
UpdateWatches	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Update display of this watch.
;
;On Entry:-
;
;ESI	- Watch entry.
;
UpdateWatch	proc	near
	call	WatchCursorOFF
	;
	;Work out source address.
	;
	call	GetWatchSource
	jc	@@9
	;
	;Display stuff.
	;
	mov	bp,WatchHandle[esi]
	cmp	WatchDisType[esi],Watch_Mixed
	jnz	@@bytes
	push	esi
	call	DisplayMixed
	pop	esi
	jmp	@@8
	;
@@bytes:	cmp	WatchDisType[esi],Watch_Bytes
	jnz	@@Text
	push	esi
	call	DisplayBytes
	pop	esi
	jmp	@@8
	;
@@Text:	cmp	WatchDisType[esi],Watch_Text
	jnz	@@words
	push	esi
	call	DisplayText
	pop	esi
	jmp	@@8
	;
@@words:	cmp	WatchDisType[esi],Watch_Words
	jnz	@@dwords
	push	esi
	call	DisplayWords
	pop	esi
	jmp	@@8
	;
@@dwords:	cmp	WatchDisType[esi],Watch_DWords
	jnz	@@8
	push	esi
	call	DisplayDWords
	pop	esi
	jmp	@@8
	;
@@8:	call	WatchCursorON
@@9:	ret
UpdateWatch	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
GetWatchSource	proc	near
	cmp	WatchSeg[esi],Watch_Abs
	jz	@@Abs
	cmp	WatchSeg[esi],Watch_CS
	jz	@@CS
	cmp	WatchSeg[esi],Watch_DS
	jz	@@DS
	cmp	WatchSeg[esi],Watch_ES
	jz	@@ES
	cmp	WatchSeg[esi],Watch_FS
	jz	@@FS
	cmp	WatchSeg[esi],Watch_GS
	jz	@@GS
	cmp	WatchSeg[esi],Watch_SS
	jz	@@SS
	jmp	@@9		;This should never be used.
	;
@@Abs:	mov	ax,WatchSelNum[esi]
	jmp	@@GotSel
@@CS:	mov	ax,DebugCS
	jmp	@@GotSel
@@DS:	mov	ax,DebugDS
	jmp	@@GotSel
@@ES:	mov	ax,DebugES
	jmp	@@GotSel
@@FS:	mov	ax,DebugFS
	jmp	@@GotSel
@@GS:	mov	ax,DebugGS
	jmp	@@GotSel
@@SS:	mov	ax,DebugSS
	;
@@GotSel:	;Get offset portion.
	;
	cmp	WatchOff[esi],Watch_Abs
	jz	@@oAbs
	cmp	WatchOff[esi],Watch_EAX
	jz	@@EAX
	cmp	WatchOff[esi],Watch_EBX
	jz	@@EBX
	cmp	WatchOff[esi],Watch_ECX
	jz	@@ECX
	cmp	WatchOff[esi],Watch_EDX
	jz	@@EDX
	cmp	WatchOff[esi],Watch_ESI
	jz	@@ESI
	cmp	WatchOff[esi],Watch_EDI
	jz	@@EDI
	cmp	WatchOff[esi],Watch_EBP
	jz	@@EBP
	cmp	WatchOff[esi],Watch_ESP
	jz	@@ESP
	cmp	WatchOff[esi],Watch_EIP
	jz	@@EIP
	cmp	WatchOff[esi],Watch_AX
	jz	@@AX
	cmp	WatchOff[esi],Watch_BX
	jz	@@BX
	cmp	WatchOff[esi],Watch_CX
	jz	@@CX
	cmp	WatchOff[esi],Watch_DX
	jz	@@DX
	cmp	WatchOff[esi],Watch_SI
	jz	@@SI
	cmp	WatchOff[esi],Watch_DI
	jz	@@DI
	cmp	WatchOff[esi],Watch_BP
	jz	@@BP
	cmp	WatchOff[esi],Watch_SP
	jz	@@SP
	cmp	WatchOff[esi],Watch_IP
	jz	@@IP
	jmp	@@9		;should never need this.
	;
@@oAbs:	mov	ebx,WatchOffset[esi]
	jmp	@@GotOff
@@EAX:	mov	ebx,DebugEAX
	jmp	@@GotOff
@@EBX:	mov	ebx,DebugEBX
	jmp	@@GotOff
@@ECX:	mov	ebx,DebugECX
	jmp	@@GotOff
@@EDX:	mov	ebx,DebugEDX
	jmp	@@GotOff
@@ESI:	mov	ebx,DebugESI
	jmp	@@GotOff
@@EDI:	mov	ebx,DebugEDI
	jmp	@@GotOff
@@EBP:	mov	ebx,DebugEBP
	jmp	@@GotOff
@@ESP:	mov	ebx,DebugESP
	jmp	@@GotOff
@@EIP:	mov	ebx,DebugEIP
	jmp	@@GotOff
@@AX:	movzx	ebx,w[DebugEAX]
	jmp	@@GotOff
@@BX:	movzx	ebx,w[DebugEBX]
	jmp	@@GotOff
@@CX:	movzx	ebx,w[DebugECX]
	jmp	@@GotOff
@@DX:	movzx	ebx,w[DebugEDX]
	jmp	@@GotOff
@@SI:	movzx	ebx,w[DebugESI]
	jmp	@@GotOff
@@DI:	movzx	ebx,w[DebugEDI]
	jmp	@@GotOff
@@BP:	movzx	ebx,w[DebugEBP]
	jmp	@@GotOff
@@SP:	movzx	ebx,w[DebugESP]
	jmp	@@GotOff
@@IP:	movzx	ebx,w[DebugEIP]
	jmp	@@GotOff
	;
@@GotOff:	;Get linear address.
	;
	pushm	ebx,esi,edi
	mov	bx,ax
	sys	GetSelDet
	pushf
	shl	ecx,16
	mov	cx,dx
	popf
	popm	ebx,esi,edi
	jc	@@9
	mov	eax,ecx
	add	ebx,ecx
	add	ebx,WatchMoved[esi]
	clc
	ret
@@9:	stc
	ret
GetWatchSource	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
WatchCursorOFF	proc	near
	pushad
	cmp	WatchLastX[esi],-1
	jz	@@9
	mov	cl,WatchLastX[esi]
	mov	ch,WatchLastY[esi]
	mov	WatchLastX[esi],-1
	mov	bp,WatchHandle[esi]
	mov	al,3
	call	BarWindow
@@9:	popad
	ret
WatchCursorOFF	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
WatchCursorON	proc	near
	pushad
	push	esi
	mov	bp,WatchHandle[esi]
	call	PointWindow
	mov	al,WindowDepth2[esi]
	pop	esi
	dec	al
	cmp	al,WatchYPos[esi]
	jnc	@@yok
	mov	WatchYPos[esi],al
@@yok:	mov	eax,WatchWidth[esi]
	cmp	WatchDisType[esi],Watch_Text
	jz	@@nxd
	shl	eax,1
@@nxd:	dec	eax
	cmp	al,WatchXPos[esi]
	jnc	@@xok
	mov	WatchXPos[esi],al
	;
@@xok:	mov	cl,WatchXPos[esi]
	mov	ch,WatchYPos[esi]
	;
	;Frig X to match display format.
	;
	cmp	WatchDisType[esi],Watch_Bytes
	jz	@@Bytes
	cmp	WatchDisType[esi],Watch_Mixed
	jz	@@Bytes
	cmp	WatchDisType[esi],Watch_Words
	jz	@@Words
	cmp	WatchDisType[esi],Watch_DWords
	jz	@@DWords
	jmp	@@oops
	;
@@Bytes:	mov	al,cl
	shr	al,1
	add	cl,al
	jmp	@@oops
	;
@@Words:	mov	al,cl
	shr	al,2
	add	cl,al
	jmp	@@oops
	;
@@Dwords:	mov	al,cl
	shr	al,3
	add	cl,al
	jmp	@@oops
	;
@@oops:	test	WatchFlags[esi],2
	jnz	@@NA
	add	cl,4+1
	test	SystemFlags,1
	jnz	@@NA
	add	cl,4
	;
@@NA:	mov	WatchLastX[esi],cl
	mov	WatchLastY[esi],ch
	mov	bp,WatchHandle[esi]
	mov	al,2
	call	BarWindow
	;
@@9:	popad
	ret
WatchCursorON	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Display window full of data.
;
;On Entry:-
;
;ESI	- Watch entry.
;BP	- Destination window.
;EAX	- Base linear address.
;EBX	- Source linear address.
;
DisplayDWords	proc	near
	mov	_DDW_Base,eax
	mov	ax,WatchFlags[esi]
	mov	_DDW_Flags,ax
	mov	_DDW_Handle,bp
	push	esi
	call	PointWindow		;need dimensions.
	;
	mov	ebp,8+1
	test	SystemFlags,1
	jz	@@Use32_0
	mov	ebp,4+1
	;
@@Use32_0:	mov	edx,8
	test	_DDW_Flags,2
	jnz	@@Use32_2
	add	edx,ebp
@@Use32_2:	;
	movzx	eax,WindowWidth2[esi]
	cmp	eax,edx
	jnc	@@ok
	mov	eax,edx
@@ok:	sub	edx,8+1
	sub	eax,edx
	xor	edx,edx
	mov	ecx,8+1
	div	ecx
	mov	ecx,eax
	movzx	ebp,WindowDepth2[esi]
	pop	esi
	shl	eax,2
	mov	WatchWidth[esi],eax
	;
	mov	_DDW_YPos,0
	mov	esi,ebx		;source address.
@@0:	pushm	ecx,ebp
	mov	edi,offset ABuffer
	test	_DDW_Flags,2
	jnz	@@1
	pushm	ecx,ebp
	;
	;display the address.
	;
	mov	eax,esi
	sub	eax,_DDW_Base
	mov	ecx,8
	test	SystemFlags,1
	jz	@@Use32_1
	mov	ecx,4
@@Use32_1:	call	Bin2Hex
	mov	b[edi],' '
	inc	edi
	mov	b[edi],0
	popm	ecx,ebp
@@1:	pushm	esi,ecx
	;
	;Display a word.
	;
	mov	ebx,-1
	mov	ax,0fffch
	int	31h
	jc	@@badAddr
	add	esi,3
	int	31h
	jc	@@badAddr
	sub	esi,3
	push	es
	mov	es,RealSegment
	mov	ebx,es:[esi]
	pop	es
@@BadAddr:	mov	eax,ebx
	mov	cl,8
	call	Bin2Hex
	mov	b[edi],' '
	inc	edi
	mov	b[edi],0
	popm	esi,ecx
	add	esi,4
	loop	@@1
	push	esi
	mov	b[edi],0
	mov	bp,_DDW_Handle
	mov	cl,0
	mov	ch,_DDW_YPos
	call	LocateWindow
	mov	ebx,offset ABuffer
	call	PrintWindow
	pop	esi
	popm	ecx,ebp
	inc	_DDW_YPos
	dec	ebp
	jnz	@@0
	ret
DisplayDWords	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Display window full of data.
;
;On Entry:-
;
;ESI	- Watch entry.
;BP	- Destination window.
;EAX	- Base linear address.
;EBX	- Source linear address.
;
DisplayWords	proc	near
	mov	_DW_Base,eax
	mov	ax,WatchFlags[esi]
	push	esi
	mov	_DW_Flags,ax
	mov	_DW_Handle,bp
	call	PointWindow		;need dimensions.
	mov	ebp,8+1
	test	SystemFlags,1
	jz	@@Use32_0
	mov	ebp,4+1
	;
@@Use32_0:	mov	edx,4
	test	_DW_Flags,2
	jnz	@@Use32_2
	add	edx,ebp
@@Use32_2:	;
	movzx	eax,WindowWidth2[esi]
	cmp	eax,edx
	jnc	@@ok
	mov	eax,edx
@@ok:	sub	edx,5
	sub	eax,edx
	xor	edx,edx
	mov	ecx,5
	div	ecx
	mov	ecx,eax
	movzx	ebp,WindowDepth2[esi]
	;
	mov	_DW_YPos,0
	pop	esi
	shl	eax,1
	mov	WatchWidth[esi],eax
	mov	esi,ebx		;source address.
@@0:	pushm	ecx,ebp
	mov	edi,offset ABuffer
	test	_DW_Flags,2
	jnz	@@1
	pushm	ecx,ebp
	;
	;display the address.
	;
	mov	eax,esi
	sub	eax,_DW_Base
	mov	ecx,8
	test	SystemFlags,1
	jz	@@Use32_1
	mov	ecx,4
@@Use32_1:	call	Bin2Hex
	mov	b[edi],' '
	inc	edi
	mov	b[edi],0
	popm	ecx,ebp
@@1:	pushm	esi,ecx
	;
	;Display a word.
	;
	mov	ebx,-1
	mov	ax,0fffch
	int	31h
	jc	@@BadAddr
	inc	esi
	int	31h
	jc	@@BadAddr
	dec	esi
	push	es
	mov	es,RealSegment
	mov	bx,es:[esi]
	pop	es
@@BadAddr:	mov	eax,ebx
	mov	cl,4
	call	Bin2Hex
	mov	b[edi],' '
	inc	edi
	mov	b[edi],0
	popm	esi,ecx
	add	esi,2
	loop	@@1
	push	esi
	mov	b[edi],0
	mov	bp,_DW_Handle
	mov	cl,0
	mov	ch,_DW_YPos
	call	LocateWindow
	mov	ebx,offset ABuffer
	call	PrintWindow
	pop	esi
	popm	ecx,ebp
	inc	_DW_YPos
	dec	ebp
	jnz	@@0
	ret
DisplayWords	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Display window full of data.
;
;On Entry:-
;
;ESI	- Watch entry.
;BP	- Destination window.
;EAX	- Base linear address.
;EBX	- Source linear address.
;
DisplayBytes	proc	near
	mov	_DB_Base,eax
	mov	ax,WatchFlags[esi]
	push	esi
	mov	_DB_Flags,ax
	mov	_DB_Handle,bp
	call	PointWindow		;need dimensions.
	mov	ebp,8+1
	test	SystemFlags,1
	jz	@@Use32_0
	mov	ebp,4+1
	;
@@Use32_0:	mov	edx,2
	test	_DB_Flags,2
	jnz	@@Use32_2
	add	edx,ebp
@@Use32_2:	;
	movzx	eax,WindowWidth2[esi]
	cmp	eax,edx
	jnc	@@ok
	mov	eax,edx
@@ok:	sub	edx,2+1
	sub	eax,edx
	xor	edx,edx
	mov	ecx,2+1
	div	ecx
	mov	ecx,eax
	movzx	ebp,WindowDepth2[esi]
	;
	mov	_DB_YPos,0
	pop	esi
	mov	WatchWidth[esi],eax
	mov	esi,ebx		;source address.
@@0:	pushm	ecx,ebp
	mov	edi,offset ABuffer
	test	_DB_Flags,2
	jnz	@@1
	pushm	ecx,ebp
	;
	;display the address.
	;
	mov	eax,esi
	sub	eax,_DB_Base
	mov	ecx,8
	test	SystemFlags,1
	jz	@@Use32_1
	mov	ecx,4
@@Use32_1:	call	Bin2Hex
	mov	b[edi],' '
	inc	edi
	mov	b[edi],0
	popm	ecx,ebp
@@1:	pushm	esi,ecx
	;
	;Display a word.
	;
	mov	ebx,-1
	mov	ax,0fffch
	int	31h
	jc	@@BadAddr
	push	es
	mov	es,RealSegment
	mov	bl,es:[esi]
	pop	es
@@BadAddr:	mov	eax,ebx
	mov	cl,2
	call	Bin2Hex
	mov	b[edi],' '
	inc	edi
	mov	b[edi],0
	popm	esi,ecx
	add	esi,1
	loop	@@1
	push	esi
	mov	b[edi],0
	mov	bp,_DB_Handle
	mov	cl,0
	mov	ch,_DB_YPos
	call	LocateWindow
	mov	ebx,offset ABuffer
	call	PrintWindow
	pop	esi
	popm	ecx,ebp
	inc	_DB_YPos
	dec	ebp
	jnz	@@0
	ret
DisplayBytes	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Display window full of data.
;
;On Entry:-
;
;ESI	- Watch entry.
;BP	- Destination window.
;EAX	- Base linear address.
;EBX	- Source linear address.
;
DisplayText	proc	near
	mov	_DT_Base,eax
	mov	ax,WatchFlags[esi]
	push	esi
	mov	_DT_Flags,ax
	mov	_DT_Handle,bp
	call	PointWindow		;need dimensions.
	mov	ebp,8+1
	test	SystemFlags,1
	jz	@@Use32_0
	mov	ebp,4+1
	;
@@Use32_0:	mov	edx,1
	test	_DT_Flags,2
	jnz	@@Use32_2
	add	edx,ebp
@@Use32_2:	;
	movzx	eax,WindowWidth2[esi]
	cmp	eax,edx
	jnc	@@ok
	mov	eax,edx
@@ok:	sub	edx,1
	sub	eax,edx
	xor	edx,edx
	mov	ecx,1
	div	ecx
	mov	ecx,eax
	movzx	ebp,WindowDepth2[esi]
	;
	mov	_DT_YPos,0
	pop	esi
	mov	WatchWidth[esi],eax
	mov	esi,ebx		;source address.
@@0:	pushm	ecx,ebp
	mov	edi,offset ABuffer
	test	_DT_Flags,2
	jnz	@@1
	pushm	ecx,ebp
	;
	;display the address.
	;
	mov	eax,esi
	sub	eax,_DT_Base
	mov	ecx,8
	test	SystemFlags,1
	jz	@@Use32_1
	mov	ecx,4
@@Use32_1:	call	Bin2Hex
	mov	b[edi],' '
	inc	edi
	mov	b[edi],0
	popm	ecx,ebp
@@1:	pushm	esi,ecx
	;
	;Display a byte.
	;
	mov	ebx,-1
	mov	ax,0fffch
	int	31h
	jc	@@BadAddr
	push	es
	mov	es,RealSegment
	mov	bl,es:[esi]
	pop	es
@@BadAddr:	mov	eax,ebx
	or	al,al
	jnz	@@ok9
	mov	al,'.'
@@ok9:	mov	b[edi],al
	inc	edi
	mov	b[edi],0
	popm	esi,ecx
	add	esi,1
	loop	@@1
	push	esi
	mov	b[edi],0
	mov	bp,_DT_Handle
	mov	cl,0
	mov	ch,_DT_YPos
	call	LocateWindow
	call	PointWindow
	mov	ax,WindowFlags[esi]
	pushm	eax,esi
	and	ax,65535-WindowTabs
	or	ax,WindowRaw
	call	ExtraWindow
	mov	ebx,offset ABuffer
	call	PrintWindow
	popm	eax,esi
	mov	WindowFlags[esi],ax
	pop	esi
	popm	ecx,ebp
	inc	_DT_YPos
	dec	ebp
	jnz	@@0
	ret
DisplayText	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Display window full of data.
;
;On Entry:-
;
;BP	- Destination window.
;EAX	- Base linear address.
;EBX	- Source linear address.
;
DisplayMixed	proc	near
	mov	_DM_Base,eax
	mov	ax,WatchFlags[esi]
	push	esi
	mov	_DM_Flags,ax
	mov	_DM_Handle,bp
	call	PointWindow		;need dimensions.
	mov	ebp,8+1	;+1+4
	test	SystemFlags,1
	jz	@@Use32_0
	mov	ebp,4+1	;+1+4
	;
@@Use32_0:	mov	edx,4
	test	_DM_Flags,2
	jnz	@@Use32_2
	add	edx,ebp
@@Use32_2:	;
	movzx	eax,WindowWidth2[esi]
	cmp	eax,edx
	jnc	@@ok
	mov	eax,edx
@@ok:	sub	edx,4
	sub	eax,edx
	shr	eax,2
	mov	ecx,eax
	push	eax
	add	al,al
	add	al,cl
	add	al,dl
	mov	_DM_APos,al
	pop	eax
	movzx	ebp,WindowDepth2[esi]
	;
	mov	_DM_YPos,0
	pop	esi
	mov	WatchWidth[esi],eax
	mov	esi,ebx		;source address.
@@0:	pushm	ecx,ebp
	mov	edi,offset ABuffer
	mov	ebx,offset BBuffer
	test	_DM_Flags,2
	jnz	@@1
	pushm	ecx,ebp
	;
	;display the address.
	;
	mov	eax,esi
	sub	eax,_DM_Base
	mov	ecx,8
	test	SystemFlags,1
	jz	@@Use32_1
	mov	ecx,4
@@Use32_1:	call	Bin2Hex
	mov	b[edi],' '
	inc	edi
	mov	b[edi],0
	popm	ecx,ebp
@@1:	pushm	esi,ecx
	;
	;Display a byte.
	;
	push	ebx
	mov	ebx,-1
	mov	ax,0fffch
	int	31h
	jc	@@BadAddr
	push	es
	mov	es,RealSegment
	mov	bl,es:[esi]
	pop	es
@@BadAddr:	mov	eax,ebx
	pop	ebx
	push	eax
	mov	cl,2
	call	Bin2Hex
	mov	b[edi],' '
	inc	edi
	mov	b[edi],0
	pop	eax
	or	al,al
	jnz	@@notz
	mov	al,'.'
@@notz:	mov	b[ebx],al
	inc	ebx
	mov	b[ebx],0
	popm	esi,ecx
	inc	esi
	loop	@@1
	push	esi
	mov	b[edi],0
	mov	bp,_DM_Handle
	mov	cl,0
	mov	ch,_DM_YPos
	call	LocateWindow
	mov	ebx,offset ABuffer
	call	PrintWindow
	mov	cl,_DM_APos
	mov	ch,_DM_YPos
	call	LocateWindow
	call	PointWindow
	mov	ax,WindowFlags[esi]
	pushm	eax,esi
	and	ax,65535-WindowTabs
	or	ax,WindowRaw
	call	ExtraWindow
	mov	ebx,offset BBuffer
	call	PrintWindow
	popm	eax,esi
	mov	WindowFlags[esi],ax
	pop	esi
	popm	ecx,ebp
	inc	_DM_YPos
	dec	ebp
	jnz	@@0
	ret
DisplayMixed	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DisasListVars	proc	near
	call	ListVars
	jc	@@None
	mov	DisplayEIP,edx
	mov	DisplayCS,cx
	mov	SourceForceCheck,1
	mov	bp,DisasHandle
	call	UpdateWindow
@@None:	ret
DisasListVars	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DataListVars	proc	near
	mov	_DLV_Handle,bp
	call	ListVars
	jc	@@None
	pushm	ecx,edx
	mov	bp,_DLV_Handle
	call	FindDataWatch
	popm	ecx,edx
	mov	WatchOffset[esi],edx
	mov	WatchOff[esi],Watch_Abs
	mov	WatchSelNum[esi],cx
	mov	WatchSeg[esi],Watch_Abs
	mov	WatchMoved[esi],0
	call	WatchCursorOFF
	call	TitleWatch
	mov	bp,_DLV_Handle
	call	UpdateWindow
@@None:	ret
DataListVars	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RegsListVars	proc	near
	call	ListVars
	jc	@@None
	movzx	esi,RegsYPos
	mov	esi,d[DebugDis2RegList+esi*4]
	mov	[esi],edx
	cmp	RegsYPos,8
	jnz	@@Show
	mov	DebugCS,cx
	mov	DisplayCS,cx
	mov	DisplayEIP,edx
	call	DisasFull
@@Show:	call	RegisterDisplay	;Display current register values.
@@None:	ret
RegsListVars	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;On Exit:-
;
;CX:EDX	- Symbol value.
;
ListVars	proc	near
	mov	_LV_Handle,0
	;
	mov	edi,SymbolList
	cmp	d[edi],-1
	jnz	@@GotSomeSyms
	mov	ebx,offset ErrorM5
	call	WindowPopup
	jmp	@@ExitNone
@@GotSomeSyms:	;
	mov	eax,offset HelpText0
	call	SetNewHelp
	;
	mov	edi,SymbolList
	xor	eax,eax		;reset longest so far.
	xor	ebx,ebx
@@gl0:	cmp	d[edi],-1
	jz	@@gl2
	movzx	ecx,SymbolTLen[edi]
	cmp	ecx,eax
	jc	@@gl1
	mov	eax,ecx
@@gl1:	inc	ebx
	add	edi,SymbolNext[edi]
	jmp	@@gl0
@@gl2:	cmp	ebx,15
	jc	@@gl3
	mov	bx,15
@@gl3:	mov	_LV_Depth,bx
	mov	_LV_Longest,al
	or	al,al
	jz	@@None
	add	_LV_Longest,2
	add	_LV_Longest,(4+1+8+1)+(1+1)
	cmp	_LV_Longest,74
	jc	@@lok
	mov	_LV_Longest,74
@@lok:	;
	mov	cl,b[VideoColumns]
	shr	cl,1
	mov	al,_LV_Longest
	add	al,3
	shr	al,1
	sub	cl,al
	mov	bl,b[VideoRows]
	shr	bl,1
	mov	al,b[_LV_Depth]
	add	al,3
	shr	al,1
	sub	bl,al
	mov	ch,_LV_Longest
	add	ch,3
	mov	bh,b[_LV_Depth]
	add	bh,3
	mov	ax,WindowClear+WindowBox+WindowStatic+WindowShad
	mov	dl,1fh
	mov	dh,7fh
	mov	esi,0
	mov	ebp,esi
	call	OpenWindow		;open main display window.
	mov	_LV_Handle,bp
	;
	mov	edi,offset _LV_SearchBuff+2
	mov	ecx,128-2
	xor	al,al
	rep	stosb
	mov	bp,_LV_Handle
	mov	ebx,offset _LV_SearchBuff
	mov	ax,WindowJCent+WindowBox
	call	BottomWindow
	mov	_LV_SearchPos,0
	;
	mov	edi,SymbolList
	mov	_LV_ListStart,edi
	mov	_LV_BarLast,-1
	mov	_LV_BarPos,0
	;
@@PrintList:	call	@@Off
	;
	mov	cx,0
	mov	ebx,15
	push	ds
	pop	es
	mov	edi,_LV_ListStart
@@pl0:	cmp	d[edi],-1
	jz	@@pl9
	pushm	ebx,ecx,edi
	call	@@printit
	popm	ebx,ecx,edi
	inc	ch
	add	edi,SymbolNext[edi]
	dec	ebx
	jnz	@@pl0
@@pl9:	;
	call	@@On
	;
@@WaitKey:	mov	ah,0
	int	16h
	;
	cmp	ah,1
	jz	@@ExitNone
	cmp	al,13
	jz	@@ExitYes
	;
	cmp	al,8
	jz	@@Delete
	;
	cmp	ah,80
	jz	@@Down
	cmp	ah,81
	jz	@@PDown
	cmp	ah,72
	jz	@@up
	cmp	ah,73
	jz	@@Pup
	;

COMMENT !
	cmp	al,"?"
	jz	@@Find
	cmp	al,'_'
	jz	@@Find
	cmp	al,'@'
	jz	@@Find
	cmp	al,'0'
END COMMENT !
	cmp	al,'!'

	jc	@@WaitKey
;	cmp	al,'9'+1
	cmp	al,'A'
	jc	@@Find
	call	UpperChar
	cmp	al,'A'
	jc	@@WaitKey
;	cmp	al,'Z'+1
	cmp	al,128
	jc	@@Find
	jmp	@@waitkey
	;
@@Delete:	cmp	_LV_SearchPos,0
	jz	@@WaitKey
	dec	_LV_SearchPos
	movzx	esi,_LV_SearchPos
	add	esi,offset _LV_SearchBuff+2
	mov	b[esi],0
	cmp	_LV_SearchPos,0
	jnz	@@Find2
	mov	bp,_LV_Handle
	call	BoxWindow
	call	ShowWinBottom
	jmp	@@WaitKey
	;
@@Find:	movzx	esi,_LV_SearchPos
	add	esi,offset _LV_SearchBuff+2
	call	UpperChar
	mov	b[esi],al
	inc	_LV_SearchPos
	;
@@Find2:	mov	ebx,_LV_ListStart
	mov	_LV_StartPos,ebx
	mov	edi,SymbolList
	mov	dx,0
@@f0:	mov	esi,offset _LV_SearchBuff+2
	cmp	d[edi],-1
	jz	@@f8
	push	edi
	movzx	ecx,SymbolTLen[edi]
	add	edi,SymbolText
@@f6:	mov	al,[edi]
	call	UpperChar
	cmp	al,[esi]
	jz	@@f1
@@f7:	pop	edi
	add	edi,SymbolNext[edi]
	jmp	@@f0
@@f1:	inc	esi
	inc	edi
	cmp	b[esi],0
	jz	@@f5
	loop	@@f6
	jmp	@@f7
	;
@@f8:	dec	_LV_SearchPos
	movzx	esi,_LV_SearchPos
	add	esi,offset _LV_SearchBuff+2
	mov	b[esi],0
	jmp	@@waitkey
	;
@@f5:	pushad
	mov	bp,_LV_Handle
	call	BoxWindow
	call	ShowWinBottom
	popad
	;
	pop	edi
	mov	esi,edi
@@f10:	movzx	ecx,_LV_Depth
@@f2:	cmp	d[edi],-1
	jz	@@f3
	add	edi,SymbolNext[edi]
	loop	@@f2
	mov	_LV_ListStart,esi
	push	dx
	call	@@Off
	pop	dx
	mov	_LV_BarPos,dx
	mov	eax,_LV_StartPos
	cmp	eax,_LV_ListStart
	jz	@@pl9
	mov	bp,_LV_Handle
	call	ClearWindow
	jmp	@@PrintList
@@f3:	mov	ebx,esi
	mov	edi,SymbolList
	cmp	edi,ebx
	jz	@@waitkey
@@f4:	mov	esi,edi
	add	edi,SymbolNext[edi]
	cmp	edi,ebx
	jnz	@@f4
	inc	dx
	mov	edi,esi
	jmp	@@f10
	;
@@Pup:	mov	edi,SymbolList
	cmp	edi,_LV_ListStart
	jz	@@waitkey
	movzx	ecx,_LV_Depth
@@psu1:	cmp	edi,_LV_ListStart
	jz	@@psu2
@@psu0:	mov	esi,edi
	add	edi,SymbolNext[edi]
	cmp	edi,_LV_ListStart
	jnz	@@psu0
	mov	_LV_ListStart,esi
	mov	edi,SymbolList
	loop	@@psu1
@@psu2:	call	@@off
	mov	bp,_LV_Handle
	call	ClearWindow
	jmp	@@PrintList
	;
@@Up:	cmp	_LV_BarPos,0
	jz	@@SUp
	call	@@Off
	dec	_LV_BarPos
	jmp	@@pl9
@@SUp:	mov	edi,SymbolList
	cmp	edi,_LV_ListStart
	jz	@@waitkey
@@su0:	mov	esi,edi
	add	edi,SymbolNext[edi]
	cmp	edi,_LV_ListStart
	jnz	@@su0
	mov	_LV_ListStart,esi
	call	@@Off
	mov	bp,_LV_Handle
	call	ScrollWindowD	;scroll the window.
	mov	cx,_LV_BarPos
	call	@@PrintEntry
	call	@@On
	jmp	@@waitkey
	;
@@PDown:	movzx	ecx,_LV_Depth
@@psd3:	mov	edx,ecx
	mov	edi,_LV_ListStart	;current position start.
@@psd0:	cmp	d[edi],-1
	jz	@@waitkey
	add	edi,SymbolNext[edi]
	loop	@@psd0		;move to bottom of current display.
	cmp	d[edi],-1		;end of the list?
	jz	@@waitkey
	;
	mov	esi,edi
	movzx	ecx,_LV_Depth	;now check what new position would be like.
@@psd1:	cmp	d[edi],-1
	jz	@@psd2
	add	edi,SymbolNext[edi]
	loop	@@psd1		;move to bottom of current display.
	mov	_LV_ListStart,esi
	call	@@off
	mov	bp,_LV_Handle
	call	ClearWindow
	jmp	@@PrintList
	;
@@psd2:	mov	ecx,edx
	dec	ecx
	jmp	@@psd3
	;
@@Down:	mov	ax,_LV_BarPos
	inc	ax
	cmp	ax,_LV_Depth
	jnc	@@SDown
	call	@@off
	inc	_LV_BarPos
	jmp	@@pl9
@@SDown:	mov	edi,_LV_ListStart
	add	edi,SymbolNext[edi]
	mov	esi,edi
	movzx	ecx,_LV_Depth
@@sd0:	cmp	d[edi],-1
	jz	@@waitkey
	add	edi,SymbolNext[edi]
	loop	@@sd0
	mov	_LV_ListStart,esi
	call	@@Off
	mov	bp,_LV_Handle
	call	ScrollWindowU	;scroll the window.
	mov	cx,_LV_BarPos
	call	@@PrintEntry
	call	@@On
	jmp	@@waitkey
	;
@@Off:	cmp	_LV_BarLast,-1
	jz	@@off1
	mov	bp,_LV_Handle
	call	PointWindow
	mov	ah,WindowAttrib[esi]
	mov	al,1
	mov	cx,_LV_BarLast
	mov	ch,cl
	call	BarWindow
	mov	_LV_BarLast,-1
@@off1:	ret
	;
@@On:	mov	bp,_LV_Handle
	call	PointWindow
	movzx	ecx,[esi].WindowDepth2
	mov	cx,_LV_BarPos
	mov	bp,_LV_Handle
	mov	_LV_BarLast,cx
	mov	ch,cl
	mov	ah,WindowAttrib[esi]
	rol	ah,4
	mov	ah,02fh
	mov	al,1
	cmp	VideoMono,0
	jz	@@pl2
	mov	ax,07001h
@@pl2:	call	BarWindow
	ret
	;
@@PrintEntry:	mov	edi,_LV_ListStart
	mov	ebx,ecx
	call	@@PointEntry
	jc	@@pre9
	mov	ch,bl
	mov	cl,0
	call	@@PrintIt
@@pre9:	ret
	;
@@PointEntry:	or	cx,cx
	jz	@@pe1
	cmp	d[edi],-1
	jz	@@pe9
	add	edi,SymbolNext[edi]
	dec	ecx
	jmp	@@PointEntry
@@pe1:	clc
	ret
@@pe9:	stc
	ret
	;
@@PrintIt:	push	edi
	mov	bp,_LV_Handle
	call	LocateWindow
	mov	edi,offset ABuffer
	movzx	ecx,_LV_Longest
	mov	al,' '
	rep	stosb
	xor	al,al
	stosb
	pop	edi
	push	edi
	mov	esi,offset ABuffer
	movzx	ecx,SymbolTLen[edi]
	xchg	esi,edi
	add	esi,SymbolText
	mov	al,' '
	stosb
	rep	movsb
	pop	edi
	;
	movzx	esi,_LV_Longest
	sub	esi,4+1+6+2+1
	add	esi,offset ABuffer
	mov	ax,SymbolSeg[edi]
	mov	ecx,4
	push	edi
	mov	edi,esi
	call	Bin2Hex
	mov	esi,edi
	pop	edi
	mov	b[esi],':'
	inc	esi
	push	edi
	mov	eax,SymbolDWord[edi]
	mov	ecx,8
	mov	edi,esi
	call	Bin2Hex
	xor	al,al
	stosb
	pop	edi
	;
	movzx	esi,_LV_Longest
	sub	esi,(4+1+6+2+1)+(1+1)
	add	esi,offset ABuffer
	movzx	eax,SymbolSeg[edi]
	lar	eax,eax
	test	eax,0000100000000000b
	mov	bl,'C'
	jnz	@@dc0
	mov	bl,'D'
@@dc0:	mov	b[esi],bl
	;
	mov	ebx,offset ABuffer
	mov	bp,_LV_Handle
	call	PrintWindow
	ret
	;
@@None:	;
@@ExitNone:	stc
	jmp	@@Exit
	;
@@ExitYes:	mov	edi,_LV_ListStart
	mov	cx,_LV_BarPos
	call	@@PointEntry
	jc	@@ExitNone
	mov	edx,SymbolDWord[edi]
	movzx	ecx,SymbolSeg[edi]
	clc
	;
@@Exit:	pushf
	pushm	ecx,edx
	mov	bp,_LV_Handle
	or	bp,bp
	jz	@@NoWindow
	call	CloseWindow
@@NoWindow:	popm	ecx,edx
	popf
	ret
ListVars	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DisasGoto	proc	near
	mov	eax,offset HelpText0
	call	SetNewHelp
	mov	cl,b[VideoColumns]
	shr	cl,1
	sub	cl,(3+40)/2
	mov	bl,b[VideoRows]
	shr	bl,1
	sub	bl,(3+1)/2
	mov	ch,40+2+1
	mov	bh,2+1+1
	mov	ax,WindowClear+WindowBox+WindowStatic+WindowShad
	mov	dl,1fh
	mov	dh,7fh
	mov	esi,0
	mov	ebp,esi
	call	OpenWindow		;open main display window.
	mov	_DG_Handle,bp
	mov	ebx,offset CSEIPGotoText
	mov	ax,WindowJLeft+WindowBox
	call	TitleWindow
	call	PointWindow
	call	ShowWinTitle
	;
	mov	cl,1	;X
	mov	ch,0	;Y
	mov	bx,40	;width.
	mov	bp,_DG_Handle
	mov	edi,offset CBuffer
	call	WindowInput
	push	eax
	mov	bp,_DG_Handle
	call	CloseWindow
	pop	eax
	cmp	ah,1		;ESC?
	jz	@@9
	cmp	b[CBuffer],0
	jz	@@9
	;
	;Check number of parameters.
	;
	mov	esi,offset CBuffer
@@0:	lodsb
	or	al,al
	jz	@@One
	cmp	al,':'
	jnz	@@0
	;
	;Two parameters if we get here.
	;
	mov	b[esi-1],0
	mov	esi,offset CBuffer
	call	WatchSegAtESI
	jnc	@@6_0
	call	WatchERegAtESI
	jc	@@5
@@6_0:	call	WatchRegToValue
	mov	DisplayCS,ax
	mov	esi,offset CBuffer
@@6:	lodsb
	or	al,al
	jnz	@@6
	jmp	@@4
	;
@@5:	;Evaluate the responce.
	;
	mov	esi,offset CBuffer
	mov	edi,offset EvaluateBuffer
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	push	esi
	mov	VarSizeMask,0
	mov	VarSizeMask+4,0
	call	EvaluateData
	pop	esi
	jc	@@99
	mov	eax,d[EvaluateBuffer]
	or	eax,eax
	jz	@@4
	mov	DisplayCS,ax
	;
@@4:	;Evaluate the responce.
	;
	push	esi
	call	WatchERegAtESI
	pop	esi
	jc	@@10
	call	WatchRegToValue
	mov	DisplayEIP,eax
	jmp	@@8
@@10:	mov	edi,offset EvaluateBuffer
@@3:	movsb
	cmp	b[esi-1],0
	jnz	@@3
	mov	VarSizeMask,0
	mov	VarSizeMask+4,0
	call	EvaluateData
	jc	@@99
	mov	eax,d[EvaluateBuffer]
	mov	DisplayEIP,eax
	jmp	@@8
	;
@@One:	;Only one parameter if we come here.
	;
	mov	esi,offset CBuffer
	call	WatchERegAtESI
	jc	@@11
	call	WatchRegToValue
	mov	DisplayEIP,eax
	jmp	@@8
	;
@@11:	mov	esi,offset CBuffer
	mov	edi,offset EvaluateBuffer
@@1:	movsb
	cmp	b[esi-1],0
	jnz	@@1
	mov	VarSizeMask,0
	mov	VarSizeMask+4,0
	call	EvaluateData
	jc	@@99
	mov	eax,d[EvaluateBuffer]
	mov	DisplayEIP,eax
	mov	ax,w[EvaluateBuffer+4]
	or	ax,ax
	jz	@@8
	mov	DisplayCS,ax
	;
@@8:	;Now update the data window's display.
	;
	mov	SourceForceCheck,1
	mov	bp,DisasHandle
	call	UpdateWindow
	jmp	@@9
	;
@@99:	mov	ebx,offset BadExpresion
	call	WindowPopup
@@9:	ret
DisasGoto	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
NullHandler	proc	near
	ret
NullHandler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
WatchRegToValue proc near
	cmp	al,Watch_CS
	jz	@@CS
	cmp	al,Watch_DS
	jz	@@DS
	cmp	al,Watch_ES
	jz	@@ES
	cmp	al,Watch_FS
	jz	@@FS
	cmp	al,Watch_GS
	jz	@@GS
	cmp	al,Watch_SS
	jz	@@SS
	cmp	al,Watch_EAX
	jz	@@EAX
	cmp	al,Watch_EBX
	jz	@@EBX
	cmp	al,Watch_ECX
	jz	@@ECX
	cmp	al,Watch_EDX
	jz	@@EDX
	cmp	al,Watch_ESI
	jz	@@ESI
	cmp	al,Watch_EDI
	jz	@@EDI
	cmp	al,Watch_EBP
	jz	@@EBP
	cmp	al,Watch_ESP
	jz	@@ESP
	cmp	al,Watch_EIP
	jz	@@EIP
	cmp	al,Watch_AX
	jz	@@AX
	cmp	al,Watch_BX
	jz	@@BX
	cmp	al,Watch_CX
	jz	@@CX
	cmp	al,Watch_DX
	jz	@@DX
	cmp	al,Watch_SI
	jz	@@SI
	cmp	al,Watch_DI
	jz	@@DI
	cmp	al,Watch_BP
	jz	@@BP
	cmp	al,Watch_SP
	jz	@@SP
	cmp	al,Watch_IP
	jz	@@IP
	xor	eax,eax
	ret
	;
@@CS:	movzx	eax,DebugCS
	jmp	@@8
@@DS:	movzx	eax,DebugDS
	jmp	@@8
@@ES:	movzx	eax,DebugES
	jmp	@@8
@@FS:	movzx	eax,DebugFS
	jmp	@@8
@@GS:	movzx	eax,DebugGS
	jmp	@@8
@@SS:	movzx	eax,DebugSS
	jmp	@@8
@@EAX:	mov	eax,DebugEAX
	jmp	@@8
@@AX:	movzx	eax,w[DebugEAX]
	jmp	@@8
@@EBX:	mov	eax,DebugEBX
	jmp	@@8
@@BX:	movzx	eax,w[DebugEBX]
	jmp	@@8
@@ECX:	mov	eax,DebugECX
	jmp	@@8
@@CX:	movzx	eax,w[DebugECX]
	jmp	@@8
@@EDX:	mov	eax,DebugEDX
	jmp	@@8
@@DX:	movzx	eax,w[DebugEDX]
	jmp	@@8
@@ESI:	mov	eax,DebugESI
	jmp	@@8
@@SI:	movzx	eax,w[DebugESI]
	jmp	@@8
@@EDI:	mov	eax,DebugEDI
	jmp	@@8
@@DI:	movzx	eax,w[DebugEDI]
	jmp	@@8
@@EBP:	mov	eax,DebugEBP
	jmp	@@8
@@BP:	movzx	eax,w[DebugEBP]
	jmp	@@8
@@ESP:	mov	eax,DebugESP
	jmp	@@8
@@SP:	movzx	eax,w[DebugESP]
	jmp	@@8
@@EIP:	mov	eax,DebugEIP
	jmp	@@8
@@IP:	movzx	eax,w[DebugEIP]
	jmp	@@8
	;
@@8:	ret
WatchRegToValue endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Prompt for new expresion to base this window on.
;
;On Entry:-
;
;BP	- Window handle.
;
DataWatchGoto	proc	near
	mov	_DWG_DHandle,bp
	mov	eax,offset HelpText0
	call	SetNewHelp
	mov	cl,b[VideoColumns]
	shr	cl,1
	sub	cl,(3+40)/2
	mov	bl,b[VideoRows]
	shr	bl,1
	sub	bl,(3+1)/2
	mov	ch,40+2
	mov	bh,3+1
	mov	ax,WindowClear+WindowBox+WindowStatic+WindowShad
	mov	dh,7fh
	mov	dl,1fh
	mov	esi,0
	mov	ebp,0
	call	OpenWindow		;open main display window.
	mov	_DWG_Handle,bp
	mov	ebx,offset CSEIPGotoText
	mov	ax,WindowJLeft+WindowBox
	call	TitleWindow
	call	ShowWinTitle
	;
	mov	cl,1	;X
	mov	ch,0	;Y
	mov	bx,40	;width.
	mov	bp,_DWG_Handle
	mov	edi,offset CBuffer
	call	WindowInput
	push	eax
	mov	bp,_DWG_Handle
	call	CloseWindow
	pop	eax
	cmp	ah,1		;ESC?
	jz	@@9
	cmp	b[CBuffer],0
	jz	@@9
	;
	;Check number of parameters.
	;
	mov	esi,offset CBuffer
@@0:	lodsb
	or	al,al
	jz	@@One
	cmp	al,':'
	jnz	@@0
	;
	;Two parameters if we get here.
	;
	mov	b[esi-1],0
	mov	esi,offset CBuffer
	call	WatchSegAtESI
	jc	@@5
	push	eax
	mov	bp,_DWG_DHandle
	call	FindDataWatch
	pop	eax
	jc	@@99
	mov	WatchSeg[esi],al
	mov	esi,offset CBuffer
@@6:	lodsb
	or	al,al
	jnz	@@6
	jmp	@@4
	;
@@5:	;Evaluate the responce.
	;
	mov	esi,offset CBuffer
	mov	edi,offset EvaluateBuffer
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	push	esi
	;
	;How about a register?
	;
	mov	esi,offset CBuffer
	call	WatchERegAtESI
	jc	@@5_0
	call	WatchRegToValue
	mov	d[EvaluateBuffer],eax
	clc
	jmp	@@5_1
	;
@@5_0:	pop	esi
	push	esi
	mov	VarSizeMask,0
	mov	VarSizeMask+4,0
	call	EvaluateData
	;
@@5_1:	pop	esi
	jc	@@99
	push	esi
	mov	bp,_DWG_DHandle
	call	FindDataWatch
	mov	ebx,esi
	pop	esi
	jc	@@99
	mov	eax,d[EvaluateBuffer]
	or	eax,eax
	jz	@@4
	mov	WatchSelNum[ebx],ax
	mov	WatchSeg[ebx],Watch_Abs
	;
@@4:	;Evaluate the responce.
	;
	push	esi
	call	WatchERegAtESI
	pop	esi
	jc	@@10
	push	eax
	mov	bp,_DWG_DHandle
	call	FindDataWatch
	pop	eax
	jc	@@99
	mov	WatchOff[esi],al
	jmp	@@8
	;
@@10:	mov	edi,offset EvaluateBuffer
@@3:	movsb
	cmp	b[esi-1],0
	jnz	@@3
	mov	VarSizeMask,0
	mov	VarSizeMask+4,0
	call	EvaluateData
	jc	@@99
	mov	bp,_DWG_DHandle
	call	FindDataWatch
	jc	@@99
	mov	eax,d[EvaluateBuffer]
	mov	WatchOffset[esi],eax
	mov	WatchOff[esi],Watch_Abs
	jmp	@@8
	;
@@One:	;Only one parameter if we come here.
	;
	mov	esi,offset CBuffer
	call	WatchERegAtESI
	jc	@@11
	push	eax
	mov	bp,_DWG_DHandle
	call	FindDataWatch
	jc	@@99
	pop	eax
	mov	WatchOff[esi],al
	jmp	@@8
	;
@@11:	mov	esi,offset CBuffer
	mov	edi,offset EvaluateBuffer
@@1:	movsb
	cmp	b[esi-1],0
	jnz	@@1
	mov	VarSizeMask,0
	mov	VarSizeMask+4,0
	call	EvaluateData
	jc	@@99
	mov	bp,_DWG_DHandle
	call	FindDataWatch
	mov	eax,d[EvaluateBuffer]
	mov	WatchOffset[esi],eax
	mov	WatchOff[esi],Watch_Abs
	mov	eax,d[EvaluateBuffer+4]
	or	eax,eax
	jz	@@8
	mov	WatchSelNum[esi],ax
	mov	WatchSeg[esi],Watch_Abs
	;
@@8:	;Now update the data window's display.
	;
	mov	bp,_DWG_DHandle
	call	FindDataWatch
	jc	@@99
	mov	WatchMoved[esi],0
	call	WatchCursorOFF
	call	TitleWatch
	mov	bp,_DWG_DHandle
	call	UpdateWindow
	jmp	@@9
	;
@@99:	mov	ebx,offset BadExpresion
	call	WindowPopup
@@9:	ret
DataWatchGoto	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
TitleWatch	proc	near
	pushad
	mov	edi,WatchTitle[esi]
	;
	;Generate seg type.
	;
	movzx	eax,WatchSeg[esi]
	pushm	esi,edi
	lea	esi,[WatchSegList+eax*2]
	add	edi,offset WatchTitleSeg-WatchTitleText
	cld
	movsw
	popm	esi,edi
	movzx	eax,WatchOff[esi]
	cmp	al,Watch_Abs
	jz	@@2
	sub	al,Watch_EAX-1
@@2:	lea	esi,[WatchOffList+eax*4]
	add	edi,offset WatchTitleOff-WatchTitleText
	movsw
	movsb
	;
	popad
	ret
TitleWatch	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
WatchSegAtESI	proc	near
	cmp	b[esi],0
	jz	@@9
	cmp	b[esi+1],0
	jz	@@9
	cmp	b[esi+2],0
	jnz	@@9
	mov	al,[esi]
	call	UpperChar
	mov	ah,al
	mov	al,[esi+1]
	call	UpperChar
	mov	bx,ax
	;
	mov	al,Watch_CS
	cmp	bx,'CS'
	jz	@@8
	mov	al,Watch_DS
	cmp	bx,'DS'
	jz	@@8
	mov	al,Watch_ES
	cmp	bx,'ES'
	jz	@@8
	mov	al,Watch_FS
	cmp	bx,'FS'
	jz	@@8
	mov	al,Watch_GS
	cmp	bx,'GS'
	jz	@@8
	mov	al,Watch_SS
	cmp	bx,'SS'
	jnz	@@9
	;
@@8:	clc
	ret
	;
@@9:	stc
	ret
WatchSegAtESI	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
WatchERegAtESI	proc	near
	cmp	b[esi],0
	jz	@@9
	cmp	b[esi+1],0
	jz	@@9
	mov	al,Watch_AX-Watch_EAX
	cmp	b[esi+2],0
	jz	@@16
	;
	cmp	b[esi+3],0
	jnz	@@9
	;
	mov	al,b[esi]
	call	UpperChar
	cmp	al,'E'
	jnz	@@9
	inc	esi
	mov	al,0
	;
@@16:	push	ax
	mov	al,[esi]
	call	UpperChar
	mov	ah,al
	mov	al,[esi+1]
	call	UpperChar
	mov	bx,ax
	pop	ax
	;
	mov	ah,Watch_EAX
	cmp	bx,'AX'
	jz	@@8
	mov	ah,Watch_EBX
	cmp	bx,'BX'
	jz	@@8
	mov	ah,Watch_ECX
	cmp	bx,'CX'
	jz	@@8
	mov	ah,Watch_EDX
	cmp	bx,'DX'
	jz	@@8
	mov	ah,Watch_ESI
	cmp	bx,'SI'
	jz	@@8
	mov	ah,Watch_EDI
	cmp	bx,'DI'
	jz	@@8
	mov	ah,Watch_EBP
	cmp	bx,'BP'
	jz	@@8
	mov	ah,Watch_ESP
	cmp	bx,'SP'
	jz	@@8
	mov	ah,Watch_EIP
	cmp	bx,"IP"
	jnz	@@9
	;
@@8:	add	al,ah
	clc
	ret
	;
@@9:	stc
	ret
WatchERegAtESI	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
FindDataWatch	proc	near
	mov	esi,offset DataWatchList
	mov	ecx,MaxWatches
@@0:	test	WatchFlags[esi],1
	jz	@@1
	cmp	bp,WatchHandle[esi]
	jz	@@2
@@1:	add	esi,size WatchStruc
	loop	@@0
	stc
	ret
@@2:	clc
	ret
FindDataWatch	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
WatchHandler	proc	near
	cmp	ax,Message_Control
	jnz	@@Update
	call	CheckExit
	push	es
	mov	es,RealSegment
	movzx	ebx,es:w[417h]
	pop	es
	mov	eax,offset HelpText5
	test	ebx,8
	jz	@@a
	mov	eax,offset HelpText5a
@@a:	call	SetNewHelp
	;
	;Check for right button.
	;
	test	MouseB,2		;right button?
	jz	@@k0
	call	DataWatchGoto
	jmp	@@9
	;
@@k0:	cmp	b[keys+1],106	;ALT-F3
	jnz	@@k20
	call	DataListVars
	jmp	@@9
	;
@@k20:	cmp	b[Keys+1],61	;F3
	jnz	@@k1
	call	DataWatchGoto
	jmp	@@9
	;
@@k1:	cmp	b[Keys+1],62	;F4
	jnz	@@k2
	call	FindDataWatch	;find this entry.
	call	WatchCursorOFF
	mov	WatchDisType[esi],Watch_Bytes
	mov	bp,WatchHandle[esi]
	call	UpdateWindow
	jmp	@@9
	;
@@k2:	cmp	b[Keys+1],63	;F5
	jnz	@@k3
	call	FindDataWatch	;find this entry.
	call	WatchCursorOFF
	mov	WatchDisType[esi],Watch_Words
	mov	bp,WatchHandle[esi]
	call	UpdateWindow
	jmp	@@9
	;
@@k3:	cmp	b[Keys+1],64	;F6
	jnz	@@k4
	call	FindDataWatch	;find this entry.
	call	WatchCursorOFF
	mov	WatchDisType[esi],Watch_DWords
	mov	bp,WatchHandle[esi]
	call	UpdateWindow
	jmp	@@9
	;
@@k4:	cmp	b[Keys+1],59	;F1
	jnz	@@k5
	call	FindDataWatch	;find this entry.
	call	WatchCursorOFF
	xor	WatchFlags[esi],2
	mov	bp,WatchHandle[esi]
	call	UpdateWindow
	jmp	@@9
	;
@@k5:	cmp	b[Keys+1],66	;F8
	jnz	@@k6
	call	FindDataWatch	;find this entry.
	call	WatchCursorOFF
	mov	WatchDisType[esi],Watch_Mixed
	mov	bp,WatchHandle[esi]
	call	UpdateWindow
	jmp	@@9
	;
@@k6:	cmp	b[Keys+1],65	;F7
	jnz	@@k7
	call	FindDataWatch	;find this entry.
	call	WatchCursorOFF
	mov	WatchDisType[esi],Watch_Text
	mov	bp,WatchHandle[esi]
	call	UpdateWindow
	jmp	@@9
	;
@@k7:	cmp	b[Keys+1],80	;down
	jnz	@@k8
	call	FindDataWatch	;find this entry.
@@k7_1:	mov	edi,esi
	mov	bp,WatchHandle[edi]
	call	PointWindow
	mov	al,WatchYPos[edi]
	inc	al
	cmp	al,WindowDepth2[esi]	;get window depth.
	jnc	@@k7_0
	mov	esi,edi
	call	WatchCursorOFF
	inc	WatchYPos[esi]
	call	WatchCursorON
	jmp	@@9
@@k7_0:	mov	eax,WatchWidth[edi]
	add	WatchMoved[edi],eax
	mov	bp,WatchHandle[edi]
	call	UpdateWindow
	jmp	@@9
	;
@@k8:	cmp	b[Keys+1],72	;up
	jnz	@@k9
	call	FindDataWatch	;find this entry.
@@k8_1:	cmp	WatchYPos[esi],0
	jz	@@k8_0
	call	WatchCursorOFF
	dec	WatchYPos[esi]
	call	WatchCursorON
	jmp	@@9
@@k8_0:	mov	eax,WatchWidth[esi]
	sub	WatchMoved[esi],eax
	mov	bp,WatchHandle[esi]
	call	UpdateWindow
	jmp	@@9
	;
@@k9:	cmp	b[Keys+1],81	;down
	jnz	@@k10
	call	FindDataWatch	;find this entry.
	mov	edi,esi
	mov	bp,WatchHandle[edi]
	call	PointWindow
	movzx	eax,WindowDepth2[esi]
	mov	ebx,WatchWidth[edi]
	mul	ebx
	add	WatchMoved[edi],eax
	mov	bp,WatchHandle[edi]
	call	UpdateWindow
	jmp	@@9
	;
@@k10:	cmp	b[Keys+1],73	;up
	jnz	@@k11
	call	FindDataWatch	;find this entry.
	mov	edi,esi
	mov	bp,WatchHandle[edi]
	call	PointWindow
	movzx	eax,WindowDepth2[esi]
	mov	ebx,WatchWidth[edi]
	mul	ebx
	sub	WatchMoved[edi],eax
	mov	bp,WatchHandle[edi]
	call	UpdateWindow
	jmp	@@9
	;
@@k11:	cmp	b[Keys+1],77	;left
	jnz	@@k12
	call	FindDataWatch	;find this entry.
@@k11_2:	movzx	eax,WatchXPos[esi]
	mov	ebx,WatchWidth[esi]
	cmp	WatchDisType[esi],Watch_Text
	jz	@@k11_1
	shl	ebx,1
@@k11_1:	inc	eax
	cmp	eax,ebx
	jnc	@@k11_0
	call	WatchCursorOFF
	inc	WatchXPos[esi]
	call	WatchCursorON
	jmp	@@9
@@k11_0:	call	WatchCursorOFF
	mov	WatchXPos[esi],0
	jmp	@@k7_1
	;
@@k12:	cmp	b[Keys+1],75	;right
	jnz	@@k13
	call	FindDataWatch	;find this entry.
	cmp	WatchXPos[esi],0
	jz	@@k12_0
	call	WatchCursorOFF
	dec	WatchXPos[esi]
	call	WatchCursorON
	jmp	@@9
@@k12_0:	mov	eax,WatchWidth[esi]
	cmp	WatchDisType[esi],Watch_Text
	jz	@@k12_1
	shl	eax,1
@@k12_1:	dec	eax
	mov	WatchXPos[esi],al
	jmp	@@k8_1
	;
@@k13:	call	FindDataWatch	;find this entry.
	cmp	WatchDisType[esi],Watch_Text
	jnz	@@Hex
	cmp	b[Keys],0
	jz	@@k14
	movzx	ebx,b[cKeys]
	mov	ecx,0ffh
	movzx	eax,WatchXPos[esi]
	jmp	@@k13_2
@@Hex:	cmp	b[Keys],'0'
	jc	@@k14
	cmp	b[Keys],'9'+1
	jc	@@k13_0
	cmp	b[Keys],'A'
	jc	@@k14
	cmp	b[Keys],'F'+1
	jnc	@@k14
	mov	al,b[Keys]
	sub	al,'A'
	add	al,10
	jmp	@@k13_1
@@k13_0:	mov	al,b[Keys]
	sub	al,'0'
@@k13_1:	movzx	ebx,al
	mov	ecx,0fh
	movzx	eax,WatchXPos[esi]	;get position.
	test	eax,1
	jnz	@@nib
	shl	ebx,4
	shl	ecx,4
@@nib:	shr	eax,1
	cmp	WatchDisType[esi],Watch_Bytes
	jz	@@k13_2
	cmp	WatchDisType[esi],Watch_Mixed
	jz	@@k13_2
	cmp	WatchDisType[esi],Watch_Words
	jz	@@Words
	cmp	WatchDisType[esi],Watch_DWords
	jz	@@DWords
	jmp	@@9
@@Words:	mov	edx,eax
	and	edx,1		;get byte number.
	xor	edx,1		;reverse it.
	and	eax,not 1
	or	eax,edx
	jmp	@@k13_2
@@DWords:	mov	edx,eax
	and	edx,3		;get byte number.
	xor	edx,3		;reverse it.
	and	eax,not 3
	or	eax,edx
@@k13_2:	pushm	eax,ebx,ecx
	call	GetWatchSource
	mov	edi,ebx
	popm	eax,ebx,ecx
	add	edi,eax
	push	eax
	mov	edx,WatchWidth[esi]
	movzx	eax,WatchYPos[esi]
	mul	edx
	add	edi,eax
	pop	eax
	xor	ecx,-1
	pushm	esi
	mov	esi,edi
	mov	ax,0fffch
	int	31h
	popm	esi
	jc	@@BadAddr
	pushm	esi
	mov	esi,edi
	add	esi,3
	int	31h
	popm	esi
	jc	@@BadAddr
	push	es
	mov	es,RealSegment
	and	es:d[edi],ecx
	or	es:d[edi],ebx
	pop	es
@@BadAddr:	push	esi
	call	UpdateWatch
	pop	esi
	jmp	@@k11_2
	;
@@k14:	cmp	b[Keys+1],68	;F10
	jnz	@@k15
	call	FindDataWatch	;find this entry.
	mov	WatchFlags[esi],0
	call	CloseWindow
	jmp	@@9
	;
@@k15:	jmp	@@9
	;
@@Update:	cmp	ax,Message_Update
	jnz	@@9
	;
	;Find out which watch entry we're talking about.
	;
	mov	esi,offset DataWatchList
	mov	ecx,MaxWatches
@@0:	test	WatchFlags[esi],1
	jz	@@1
	cmp	bp,WatchHandle[esi]
	jz	@@2
@@1:	add	esi,size WatchStruc
	loop	@@0
	jmp	@@9		;this catches first call.
@@2:	;
	;Clear the window for a start.
	;
	push	esi
	mov	bp,WatchHandle[esi]
	call	ClearWindow
	pop	esi
	call	UpdateWatch		;update this display.
;	jmp	@@9

@@9:	ret
WatchHandler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SetNewHelp	proc	near
	pushad
	cmp	eax,HelpText
	jz	@@0
	mov	HelpText,eax
	mov	bp,HelpHandle
	mov	ax,Message_Update
	call	MessageWindow
@@0:	popad
	ret
SetNewHelp	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
MenuHandler	proc	near
	cmp	ax,Message_Control
	jnz	@@Update
	call	CheckExit
	push	es
	mov	es,RealSegment
	movzx	ebx,es:w[417h]
	pop	es
	mov	eax,offset HelpText4
	test	ebx,8
	jz	@@a
	mov	eax,offset HelpText4a
@@a:	call	SetNewHelp
	jmp	@@9
	;
@@Update:	cmp	ax,Message_Update
	jnz	@@9
	call	ClearWindow
	mov	ebx,MenuText
	call	PrintWindow
	jmp	@@9
	;
@@9:	ret
MenuHandler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
HelpHandler	proc	near
	cmp	ax,Message_Control
	jnz	@@Update
	call	CheckExit
	push	es
	mov	es,RealSegment
	movzx	ebx,es:w[417h]
	pop	es
	mov	eax,offset HelpText3
	test	ebx,8
	jz	@@a
	mov	eax,offset HelpText3a
@@a:	call	SetNewHelp
	jmp	@@9
	;
@@Update:	cmp	ax,Message_Update
	jnz	@@9
	call	ClearWindow
	mov	ebx,HelpText
	call	PrintWindow
	jmp	@@9
	;
@@9:	ret
HelpHandler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DisasHandler	proc	near
	cmp	ax,Message_Control
	jnz	@@Update
	call	CheckExit
	push	es
	mov	es,RealSegment
	movzx	ebx,es:w[417h]
	pop	es

	mov	eax,offset HelpText1
	test	bl,8
	jz	@@b
;	test	bl,4
;	jz	@@a
;	mov	eax,offset HelpText1b
;	jmp	@@b

@@a:
	mov	eax,offset HelpText1a

@@b:
	call	SetNewHelp

	test	MouseB,2		;right mouse button?
	jz	@@n0
	call	DisasGoto
	jmp	@@9
	;
@@n0:	mov	ax,Keys
	;
	cmp	b[Keys+1],105	;ALT-F2
	jnz	@@NotHBrk
	call	AddHardBreak
	jmp	@@9
	;
@@NotHBrk:	cmp	b[Keys+1],66	;F8
	jnz	@@k0
	mov	al,1	;next
	jmp	@@Exec
	;
@@k0:	cmp	b[Keys+1],90	;SHIFT-F7
	jnz	@@k0_00
	mov	al,0+128
	jmp	@@Exec
	;
@@k0_00:	cmp	b[Keys+1],110	;ALT-F7
	jnz	@@k0_01
	mov	al,0+128
	jmp	@@Exec
	;
@@k0_01:	cmp	b[Keys+1],65	;F7
	jnz	@@k1
	mov	al,0	;trace.
	jmp	@@Exec
	;
@@k1:	cmp	b[Keys+1],67	;F9
	jnz	@@k2
	mov	al,2	;Run
	jmp	@@Exec
	;
@@Exec:	cmp	TerminationFlag,0
	jz	@@k0_0
@@k0_0_0:	mov	ebx,offset TerminateText
	call	WindowPopup
	jmp	@@9
	;
@@k0_0:	push	ax
	call	BarOFF
	call	CurrentOFF
	call	BreaksOFF
	pop	ax
	call	CopyDebugRegs
	call	ExecuteInst
@@ExecOver:	mov	ax,DebugCS
	mov	DisplayCS,ax
	mov	eax,DebugEIP
	mov	DisplayEIP,eax
	call	DisasPart
	call	RegisterDisplay	;Display current register values.
;	call	MessageWindow
	call	UpdateWatches
	jmp	@@9
	;
@@k2:	cmp	ah,112	;ALT-F9
	jnz	@@k89
	cmp	ERRFile,0
	jz	@@nodelerr
	mov	esi,ERRFile
	xor	ebp,ebp
	mov	bp,SFT_Handle[esi]
	push	ebp
	call	CloseWindow
	pop	ebp
	call	FindFileHandle
	push	esi
	mov	esi,SFT_Lines[esi]
	call	Free
	pop	esi
	push	esi
	mov	esi,SFT_File[esi]
	call	Free
	pop	esi
	call	Free
	mov	esi,SourceFileTable
	mov	ecx,[esi]
	mov	esi,LastFilePointer
	sub	esi,SourceFileTable
	sub	esi,4
	shr	esi,2
	sub	ecx,esi
	dec	ecx
	mov	edi,LastFilePointer
	mov	esi,edi
	add	esi,4
	cld
	rep	movsd
	mov	esi,SourceFileTable
	dec	d[esi]
	mov	ERRFile,0
@@nodelerr:	call	GenerateCWD_ERR
	mov	edx,offset eDebugName
	call	LoadSourceFile
	mov	ERRFile,ebx
	jmp	@@9
	;
@@k89:	cmp	ah,106	;ALT-F3
	jnz	@@k20
	call	DisasListVars
	jmp	@@9
	;
@@k20:	cmp	ah,80	;cursor down.
	jnz	@@k3
	call	DisplayEIPDown
	jmp	@@9
	;
@@k3:	cmp	ah,72	;cursor up?
	jnz	@@k4
	call	DisplayEIPUp
	jmp	@@9
	;
@@k4:	cmp	ah,75	;cursor left?
	jnz	@@k4_00
	mov	bp,DisasHandle
	call	PointWindow
	cmp	WindowXOffset[esi],0
	jz	@@9
	dec	WindowXOffset[esi]
	mov	bp,DisasHandle
	mov	ax,Message_Update
	call	MessageWindow
	jmp	@@9
	;
@@k4_00:	cmp	ah,77	;cursor right?
	jnz	@@k4_01
	mov	bp,DisasHandle
	call	PointWindow
	cmp	WindowXOffset[esi],64
	jnc	@@9
	inc	WindowXOffset[esi]
	mov	bp,DisasHandle
	mov	ax,Message_Update
	call	MessageWindow
	jmp	@@9
	;
@@k4_01:	cmp	b[Keys+1],59	;F1
	jnz	@@k5
;	cmp	VideoMono,0
;	jnz	@@9		;ignore when using mono.
	call	MouseOFF
	mov	NoContextSwitch,-1
	mov	VidSwapMode,0	;force a full swap.
	mov	al,ForcedFlip
	or	al,AutoFlip
	mov	FlipSwap,al
	call	UserScreen
@@k4_0:	call	GetKeys
	jz	@@k4_0
	call	DisasScreen
	mov	NoContextSwitch,0
	call	MouseON
	jmp	@@9
	;
@@k5:	cmp	b[Keys+1],64	;F6
	jnz	@@k6
	pushad
	mov	edi,offset ABuffer
	mov	esi,DebugEIP
	mov	fs,DebugCS
	call	Disasemble
	mov	DebugEIP,esi
	mov	DisplayEIP,esi
	mov	DebugCS,fs
	mov	DisplayCS,fs
	popad
	mov	SourceForceCheck,1
	call	DisasPart
	jmp	@@9
	;
@@k6:	cmp	b[Keys+1],60	;F2
	jnz	@@k7
	mov	bx,DisplayCS		;Need segment linear base address
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	edx,ecx
	add	edx,DisplayEIP	;real linear address.
	mov	al,BreakType_exec
	call	IsBreakPoint
	jc	@@k6_0
	mov	ax,bx
	call	RelBreakPoint
	mov	ax,Message_Update
	mov	bp,DisasHandle
	or	SourceNoTrack,-1
	call	MessageWindow
	jmp	@@9
@@k6_0:	call	SetBreakPoint
	or	SourceNoTrack,-1
	call	DisasPart
	jmp	@@9
	;
@@k7:	cmp	ah,73
	jnz	@@k8
	call	DisplayEIPPUp
	jmp	@@9
	;
@@k8:	cmp	ah,81
	jnz	@@k9
	call	DisplayEIPPDown
	jmp	@@9
	;
@@k9:	cmp	b[Keys+1],63	;F5
	jnz	@@k10
	mov	al,Watch_Abs	;seg type.
	mov	ah,Watch_Abs	;offset type.
	mov	ebx,0	;offset
	mov	cx,RealSegment
	call	OpenDataWatch
	jmp	@@9
	;
@@k10:	cmp	b[Keys+1],61	;F3
	jnz	@@k11
	call	DisasGoto
	jmp	@@9
	;
@@k11:	cmp	b[Keys+1],62	;F4
	jnz	@@k12
	cmp	TerminationFlag,0
	jz	@@k0_1
	mov	ebx,offset TerminateText
	call	WindowPopup
	jmp	@@9
	;
@@k0_1:	mov	bx,DisplayCS		;Need segment linear base address
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	edx,ecx
	add	edx,DisplayEIP	;real linear address.
	mov	al,BreakType_exec
	call	SetBreakPoint
	call	CopyDebugRegs
	push	ax
	mov	al,2
	call	ExecuteInst
	pop	ax
	call	RelBreakPoint	;release it then.
	jmp	@@ExecOver
	;
@@k12:	cmp	b[Keys+1],68	;F10
	jnz	@@k13
	xor	ForcedFlip,1
	xor	FlipTextAttr,0ch	; [de]highlight Flip
	pushad					; and update help text
	mov	bp,HelpHandle
	mov	ax,Message_Update
	call	MessageWindow
	popad
	jmp	@@9
	;
@@k13:	cmp	b[Keys],'+'
	jnz	@@k14
	mov	bx,DisplayCS		;Need segment linear base address
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	edx,ecx
	add	edx,DisplayEIP	;real linear address.
	mov	al,BreakType_exec
	call	IsBreakPoint
	jc	@@9
	mov	ax,bx
	call	PointBreakPoint
	inc	BreakCountDown[esi]
	call	DisasPart
	jmp	@@9
	;
@@k14:	cmp	b[Keys],'-'
	jnz	@@k15
	mov	bx,DisplayCS		;Need segment linear base address
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	edx,ecx
	add	edx,DisplayEIP	;real linear address.
	mov	al,BreakType_exec
	call	IsBreakPoint
	jc	@@9
	mov	ax,bx
	call	PointBreakPoint
	cmp	BreakCountDown[esi],0
	jz	@@9
	dec	BreakCountDown[esi]
	call	DisasPart
	jmp	@@9
	;
@@k15:	cmp	b[Keys+1],104	;Alt-F1?
	jnz	@@k16
	cmp	DontUseSource,0
	jnz	@@k15_0
	or	DontUseSource,-1
	jmp	@@k15_2
@@k15_0:	cmp	DisasSymbolTranslate,0
	jz	@@k15_1
	mov	DisasSymbolTranslate,0
	jmp	@@k15_2
@@k15_1:	mov	DisasSymbolTranslate,1
	mov	DontUseSource,0
@@k15_2:	mov	ForcedDisas,-1
	mov	SourceIndex,0
	mov	SourceForceCheck,1
	call	DisasFull
	jmp	@@9
	;
@@k16:	cmp	b[Keys+1],109
	jnz	@@k17
	cmp	TerminationFlag,0
	jnz	@@9
	cmp	RecentCallStack,0	;Anything on the stack?
	jz	@@9
	mov	edx,RecentCallStack
	call	RemRetAddress	;Take it off the stack.
	mov	al,BreakType_exec
	call	SetBreakPoint
	call	CopyDebugRegs
	push	ax
	mov	al,2
	call	ExecuteInst
	pop	ax
	call	RelBreakPoint	;release it then.
	jmp	@@ExecOver
	;
@@k17:	cmp	b[Keys+1],108	;ALT-F5?
	jnz	@@k18
	cmp	SourceIndex,0	;Looking at source?
	jz	@@9
	call	ListSourceFiles
	or	eax,eax
	jz	@@9
	cmp	eax,SourceIndex
	jz	@@9
	mov	SourceIndex,eax
	dec	eax
	shl	eax,4
	add	eax,4
	mov	esi,LINEList
	add	esi,eax
	mov	esi,[esi+4]
	mov	eax,SFT_SY[esi]
	mov	SourceStartLine,eax
	mov	eax,SFT_Y[esi]
	mov	SourceLineNum,eax
	or	SourceNoTrack,-1
	call	DisasFull
	jmp	@@9

@@k18:	cmp	b[Keys+1],107	;ALT-F4
	jnz	@@k19
	mov	eax,DebugEIP
	mov	DisplayEIP,eax
	mov	DisasEIP,eax
	mov	ax,DebugCS
	mov	DisplayCS,ax
	mov	DisasCS,ax
	mov	SourceForceCheck,1
	call	DisasFull
	jmp	@@9

@@k19:
;	cmp	b[Keys+1],94	;Ctrl-F1
;	jnz	@@k21

;@@k21:
	jmp	@@9

@@Update:	cmp	ax,Message_Update
	jnz	@@9
	mov	DisasHandle,bp
	call	DisasFull
	jmp	@@9
	;
@@9:	ret
DisasHandler	endp

	include generr.asm

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Add a hardware break point.
;
AddHardBreak	proc	near
;
;Check if any table entries are free (max 4 entries on 386).
;
	mov	esi,offset HardBreakTable
	mov	ecx,4
@@0:	cmp	HBRK_Win[esi],0	;window handle?
	jz	@@1
	add	esi,size HBRK
	dec	ecx
	jnz	@@0
	mov	ebx,offset NoHardBreaks
	call	WindowPopup
	jmp	@@9
;
;Have an entry, get some details.
;
@@1:	mov	HardBreakEntry,esi
	;
	mov	cl,b[VideoColumns]
	shr	cl,1
	sub	cl,(3+40)/2
	mov	bl,b[VideoRows]
	shr	bl,1
	sub	bl,(3+1)/2
	mov	ch,40+2
	mov	bh,3+1
	mov	ax,WindowClear+WindowBox+WindowStatic+WindowShad
	mov	dh,7fh
	mov	dl,1fh
	mov	esi,0
	mov	ebp,0
	call	OpenWindow		;open main display window.
	mov	_RG_Handle,bp
	mov	ebx,offset SetHBRKText
	mov	ax,WindowJLeft+WindowBox
	call	TitleWindow
	call	ShowWinTitle
	;
	mov	cl,1	;X
	mov	ch,0	;Y
	mov	bx,40	;width.
	mov	bp,_RG_Handle
	mov	edi,offset CBuffer
	call	WindowInput
	push	eax
	mov	bp,_RG_Handle
	call	CloseWindow
	pop	eax
	cmp	ah,1		;ESC?
	jz	@@9
	cmp	b[CBuffer],0
	jz	@@9
;
;Now process the input, should be 3 lots of data.
;
	mov	esi,offset CBuffer
	mov	edi,offset EvaluateBuffer
@@2:	movsb
	cmp	b[esi-1],0
	jz	@@3
	cmp	b[esi-1],":"
	jz	@@3
	cmp	b[esi-1],","
	jnz	@@2
@@3:	mov	b[edi-1],0
	mov	VarSizeMask,0
	mov	VarSizeMask+4,0
	push	esi
	call	EvaluateData
	pop	esi
	jc	@@99
	mov	eax,d[EvaluateBuffer]
	cmp	b[esi-1],":"
	jnz	@@6
	;
	;Got the segment, now do the offset.
	;
	mov	bx,ax
	sys	GetSelDet32
	mov	eax,edx
	mov	edi,offset EvaluateBuffer
@@4:	movsb
	cmp	b[esi-1],0
	jz	@@5
	cmp	b[esi-1],","
	jnz	@@4
@@5:	mov	b[edi-1],0
	push	eax
	push	esi
	call	EvaluateData
	pop	esi
	pop	eax
	jc	@@99
	add	eax,d[EvaluateBuffer]
	;
@@6:	mov	edi,HardBreakEntry
	mov	HBRK_Address[edi],eax
;
;Fetch the length.
;
@@10:	xor	eax,eax
	cmp	b[esi-1],0
	jz	@@7
	mov	ah,[esi]
	inc	esi
	or	ah,ah
	jz	@@7
	cmp	ah,","
	jz	@@7
	mov	al,1
	cmp	ah,"b"
	jz	@@7
	cmp	ah,"B"
	jz	@@7
	mov	al,2
	cmp	ah,"w"
	jz	@@7
	cmp	ah,"W"
	jz	@@7
	mov	al,4
	cmp	ah,"d"
	jz	@@7
	cmp	ah,"D"
	jz	@@7
	jmp	@@99
@@7:	mov	edi,HardBreakEntry
	mov	HBRK_Size[edi],al
;
;Fetch the type.
;
	xor	eax,eax
	cmp	b[esi-1],0
	jz	@@8
@@100:	mov	ah,[esi]
	inc	esi
	or	ah,ah
	jz	@@8
	cmp	ah,","
	jz	@@100
	cmp	ah,"E"
	jz	@@8
	cmp	ah,"e"
	jz	@@8
	mov	al,1
	cmp	ah,"W"
	jz	@@8
	cmp	ah,"w"
	jz	@@8
	mov	al,2
	cmp	ah,"r"
	jz	@@8
	cmp	ah,"R"
	jz	@@8
	jmp	@@99
@@8:	mov	edi,HardBreakEntry
	mov	HBRK_Type[edi],al
;
;Open a window for this break point.
;
	push	WinFocusHandle
	mov	eax,HardBreakEntry
	sub	eax,offset HardBreakTable
	mov	ecx,size HBRK
	xor	edx,edx
	div	ecx
	shl	eax,4
	mov	cl,al
	mov	bl,ScreenDepth
	sub	bl,1+3
	mov	ch,16
	mov	bh,3
	mov	ax,WindowClear+WindowBox+WindowStatic
	mov	dl,30h
	mov	dh,3fh
	mov	esi,0
	mov	ebp,offset HBRKHandler
	call	OpenWindow		;open main display window.
	mov	esi,HardBreakEntry
	mov	HBRK_Win[esi],bp
	mov	ebx,offset HBRKTitle
	mov	ax,WindowJCent+WindowBox
	call	TitleWindow
	mov	ax,Message_Update
	call	MessageWindow
	pop	WinFocusHandle
	push	bp
	mov	bp,WinFocusHandle
	call	PointWindow
	pushm	esi,ebp
	call	WinHighestPrior
	popm	esi,ebp
	inc	eax
	mov	WindowPriority[esi],eax
	mov	eax,WindowHandler[esi]
	mov	WindowFocus,eax
	pop	bp
	call	UpdateWindow
	call	BoxWindow
	call	ShowWinTitle
	call	ShowWinBottom

	mov	bp,WinFocusHandle
	call	UpdateWindow
	call	BoxWindow
	call	ShowWinTitle
	call	ShowWinBottom
	jmp	@@9
	;
@@99:	mov	ebx,offset BadExpresion
	call	WindowPopup
	;
@@9:	ret
AddHardBreak	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
HBRKHandler	proc	near
	cmp	ax,Message_Control
	jnz	@@Update
	call	CheckExit
	push	es
	mov	es,RealSegment
	movzx	ebx,es:w[417h]
	pop	es
	mov	eax,offset HelpText7
	test	ebx,8
	jz	@@a
	mov	eax,offset HelpText7a
@@a:	call	SetNewHelp
	;
	cmp	b[Keys+1],68
	jnz	@@9
	;
	;Find out which break entry we're talking about.
	;
	mov	esi,offset HardBreakTable
	mov	ecx,4
@@4:	cmp	bp,HBRK_Win[esi]
	jz	@@5
	add	esi,size HBRK
	loop	@@4
	jmp	@@9		;this catches first call.
@@5:	push	esi
	call	CloseWindow
	pop	esi
	mov	HBRK_Win[esi],0
	jmp	@@9
	;
@@Update:	cmp	ax,Message_Update
	jnz	@@9
	;
	;Find out which break entry we're talking about.
	;
	mov	esi,offset HardBreakTable
	mov	ecx,4
@@0:	cmp	bp,HBRK_Win[esi]
	jz	@@2
	add	esi,size HBRK
	loop	@@0
	jmp	@@9		;this catches first call.
@@2:	;
	;Clear the window for a start.
	;
	push	esi
	mov	bp,HBRK_Win[esi]
	call	ClearWindow
	pop	esi
	;
	;Now update the details.
	;
	mov	edi,offset CBuffer
	mov	eax,HBRK_Address[esi]
	mov	ecx,8
	call	Bin2Hex
	mov	b[edi]," "
	inc	edi
	mov	al,"B"
	cmp	HBRK_Size[esi],1
	jz	@@101
	mov	al,"W"
	cmp	HBRK_Size[esi],2
	jz	@@101
	mov	al,"D"
@@101:	mov	[edi],al
	inc	edi
	mov	b[edi]," "
	inc	edi
	mov	ax,"XE"
	cmp	HBRK_Type[esi],0
	jz	@@102
	mov	ax," W"
	cmp	HBRK_Type[esi],1
	jz	@@102
	mov	ax,"RW"
@@102:	mov	[edi],ax
	add	edi,2
	mov	b[edi],0
	mov	ebx,offset CBuffer
	call	PrintWindow
	jmp	@@9
	;
@@9:	ret
HBRKHandler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;On Exit:-
;
;CX:EDX	- Symbol value.
;
ListSourceFiles proc near
	mov	_LV_Handle,0
	;
	cmp	LINEList,0
	jz	@@Nope
	mov	eax,LINEList
	cmp	d[eax],0
	jz	@@Nope
	jmp	@@ok
	;
@@nope:	xor	eax,eax
	ret
	;
@@ok:	mov	eax,offset HelpText0
	call	SetNewHelp
	;
	mov	edi,LINEList
	add	edi,4
	xor	eax,eax		;reset longest so far.
	xor	ebx,ebx
@@gl0:	cmp	d[edi],-1
	jz	@@gl2
	cmp	d[edi+4],0
	jz	@@gl1_0
	push	edi
	mov	edi,[edi+4]
	lea	edi,SFT_Name[edi]
	xor	ecx,ecx
@@gl0_0:	cmp	b[edi],0
	jz	@@gl0_1
	inc	edi
	inc	ecx
	jmp	@@gl0_0
@@gl0_1:	pop	edi
	cmp	ecx,eax
	jc	@@gl1
	mov	eax,ecx
@@gl1:	inc	ebx
@@gl1_0:	add	edi,4+4+4+4
	jmp	@@gl0
@@gl2:	mov	_LV_Depth2,ebx
	cmp	ebx,15
	jc	@@gl3
	mov	bx,15
@@gl3:	mov	_LV_Depth,bx
	mov	_LV_Longest,al
	or	al,al
	jz	@@None
	add	_LV_Longest,2
	cmp	_LV_Longest,74
	jc	@@lok
	mov	_LV_Longest,74
@@lok:	;
	mov	cl,b[VideoColumns]
	shr	cl,1
	mov	al,_LV_Longest
	add	al,3
	shr	al,1
	sub	cl,al
	mov	bl,b[VideoRows]
	shr	bl,1
	mov	al,b[_LV_Depth]
	add	al,3
	shr	al,1
	sub	bl,al
	mov	ch,_LV_Longest
	add	ch,3
	mov	bh,b[_LV_Depth]
	add	bh,3
	mov	ax,WindowClear+WindowBox+WindowStatic+WindowShad
	mov	dl,1fh
	mov	dh,7fh
	mov	esi,0
	mov	ebp,esi
	call	OpenWindow		;open main display window.
	mov	_LV_Handle,bp
	;
	mov	edi,offset _LV_SearchBuff+2
	mov	ecx,128-2
	xor	al,al
	rep	stosb
	mov	bp,_LV_Handle
	mov	ebx,offset _LV_SearchBuff
	mov	ax,WindowJCent+WindowBox
	call	BottomWindow
	mov	_LV_SearchPos,0
	;
	mov	edi,LINEList
	add	edi,4
	mov	_LV_ListStart,edi
	mov	_LV_BarLast,-1
	mov	_LV_BarPos,0
	;
@@PrintList:	call	@@Off
	;
	mov	cx,0
	mov	ebx,15
	push	ds
	pop	es
	mov	edi,_LV_ListStart
@@pl0:	cmp	d[edi],-1
	jz	@@pl9
	pushm	ebx,ecx,edi
	call	@@printit
	popm	ebx,ecx,edi
	inc	ch
	add	edi,4+4+4+4
	dec	ebx
	jnz	@@pl0
@@pl9:	;
	call	@@On
	;
@@WaitKey:	mov	ah,0
	int	16h
	;
	cmp	ah,1
	jz	@@ExitNone
	cmp	al,13
	jz	@@ExitYes
	;
	cmp	al,8
	jz	@@Delete
	;
	cmp	ah,80
	jz	@@Down
	cmp	ah,81
	jz	@@PDown
	cmp	ah,72
	jz	@@up
	cmp	ah,73
	jz	@@Pup
	;
COMMENT !
	cmp	al,'_'
	jz	@@Find
	cmp	al,'@'
	jz	@@Find
	cmp	al,'0'
END COMMENT !

	cmp	al,'!'

	jc	@@WaitKey
;	cmp	al,'9'+1
	cmp	al,'A'
	jc	@@Find
	call	UpperChar
	cmp	al,'A'
	jc	@@WaitKey
;	cmp	al,'Z'+1
	cmp	al,128
	jc	@@Find
	jmp	@@waitkey
	;
@@Delete:	cmp	_LV_SearchPos,0
	jz	@@WaitKey
	dec	_LV_SearchPos
	movzx	esi,_LV_SearchPos
	add	esi,offset _LV_SearchBuff+2
	mov	b[esi],0
	cmp	_LV_SearchPos,0
	jnz	@@Find2
	mov	bp,_LV_Handle
	call	BoxWindow
	call	ShowWinBottom
	jmp	@@WaitKey
	;
@@Find:	movzx	esi,_LV_SearchPos
	add	esi,offset _LV_SearchBuff+2
	call	UpperChar
	mov	b[esi],al
	inc	_LV_SearchPos
	;
@@Find2:	mov	ebx,_LV_ListStart
	mov	_LV_StartPos,ebx
	mov	edi,LineList
	add	edi,4
	mov	dx,0
@@f0:	mov	esi,offset _LV_SearchBuff+2
	cmp	d[edi],-1
	jz	@@f8
	push	edi
	mov	edi,[edi+4]
	or	edi,edi
	jz	@@f7
	lea	edi,SFT_Name[edi]
	push	edi
	xor	ecx,ecx
@@f0_0:	cmp	b[edi],0
	jz	@@f0_1
	inc	edi
	inc	ecx
	jmp	@@f0_0
@@f0_1:	pop	edi
@@f6:	mov	al,[edi]
	call	UpperChar
	cmp	al,[esi]
	jz	@@f1
@@f7:	pop	edi
	add	edi,4+4+4+4
	jmp	@@f0
@@f1:	inc	esi
	inc	edi
	cmp	b[esi],0
	jz	@@f5
	loop	@@f6
	jmp	@@f7
	;
@@f8:	dec	_LV_SearchPos
	movzx	esi,_LV_SearchPos
	add	esi,offset _LV_SearchBuff+2
	mov	b[esi],0
	jmp	@@waitkey
	;
@@f5:	pushad
	mov	bp,_LV_Handle
	call	BoxWindow
	call	ShowWinBottom
	popad
	;
	pop	edi
	mov	esi,edi
@@f10:	movzx	ecx,_LV_Depth
@@f2:	cmp	d[edi],-1
	jz	@@f3
	add	edi,4+4+4+4
	loop	@@f2
	mov	_LV_ListStart,esi
	push	dx
	call	@@Off
	pop	dx
	mov	_LV_BarPos,dx
	mov	eax,_LV_StartPos
	cmp	eax,_LV_ListStart
	jz	@@pl9
	mov	bp,_LV_Handle
	call	ClearWindow
	jmp	@@PrintList
@@f3:	mov	ebx,esi
	mov	edi,LineList
	add	edi,4
	cmp	edi,ebx
	jz	@@waitkey
@@f4:	mov	esi,edi
	add	edi,4+4+4+4
	cmp	edi,ebx
	jnz	@@f4
	inc	dx
	mov	edi,esi
	jmp	@@f10
	;
@@Pup:	mov	edi,LINEList
	add	edi,4
	cmp	edi,_LV_ListStart
	jz	@@waitkey
	movzx	ecx,_LV_Depth
@@psu1:	cmp	edi,_LV_ListStart
	jz	@@psu2
@@psu0:	mov	esi,edi
	add	edi,4+4+4+4
	cmp	edi,_LV_ListStart
	jnz	@@psu0
	mov	_LV_ListStart,esi
	mov	edi,LINEList
	add	edi,4
	loop	@@psu1
@@psu2:	call	@@off
	mov	bp,_LV_Handle
	call	ClearWindow
	jmp	@@PrintList
	;
@@Up:	cmp	_LV_BarPos,0
	jz	@@SUp
	call	@@Off
	dec	_LV_BarPos
	jmp	@@pl9
@@SUp:	mov	edi,LINEList
	add	edi,4
	cmp	edi,_LV_ListStart
	jz	@@waitkey
@@su0:	mov	esi,edi
	add	edi,4+4+4+4
	cmp	edi,_LV_ListStart
	jnz	@@su0
	mov	_LV_ListStart,esi
	call	@@Off
	mov	bp,_LV_Handle
	call	ScrollWindowD	;scroll the window.
	mov	cx,_LV_BarPos
	call	@@PrintEntry
	call	@@On
	jmp	@@waitkey
	;
@@PDown:	movzx	ecx,_LV_Depth
@@psd3:	mov	edx,ecx
	mov	edi,_LV_ListStart	;current position start.
@@psd0:	cmp	d[edi],-1
	jz	@@waitkey
	add	edi,4+4+4+4
	loop	@@psd0		;move to bottom of current display.
	cmp	d[edi],-1		;end of the list?
	jz	@@waitkey
	;
	mov	esi,edi
	movzx	ecx,_LV_Depth	;now check what new position would be like.
@@psd1:	cmp	d[edi],-1
	jz	@@psd2
	add	edi,4+4+4+4
	loop	@@psd1		;move to bottom of current display.
	mov	_LV_ListStart,esi
	call	@@off
	mov	bp,_LV_Handle
	call	ClearWindow
	jmp	@@PrintList
	;
@@psd2:	mov	ecx,edx
	dec	ecx
	jmp	@@psd3
	;
@@Down:	mov	ax,_LV_BarPos
	inc	ax
	cmp	ax,_LV_Depth
	jnc	@@SDown
	call	@@off
	inc	_LV_BarPos
	jmp	@@pl9
@@SDown:	mov	edi,_LV_ListStart
	add	edi,4+4+4+4
	mov	esi,edi
	movzx	ecx,_LV_Depth
@@sd0:	cmp	d[edi],-1
	jz	@@waitkey
	add	edi,4+4+4+4
	loop	@@sd0
	mov	_LV_ListStart,esi
	call	@@Off
	mov	bp,_LV_Handle
	call	ScrollWindowU	;scroll the window.
	mov	cx,_LV_BarPos
	call	@@PrintEntry
	call	@@On
	jmp	@@waitkey
	;
@@Off:	cmp	_LV_BarLast,-1
	jz	@@off1
	mov	bp,_LV_Handle
	call	PointWindow
	mov	ah,WindowAttrib[esi]
	mov	al,1
	mov	cx,_LV_BarLast
	mov	ch,cl
	call	BarWindow
	mov	_LV_BarLast,-1
@@off1:	ret
	;
@@On:	mov	bp,_LV_Handle
	call	PointWindow
	movzx	ecx,[esi].WindowDepth2
	mov	cx,_LV_BarPos
	mov	bp,_LV_Handle
	mov	_LV_BarLast,cx
	mov	ch,cl
	mov	ah,WindowAttrib[esi]
	rol	ah,4
	mov	ah,02fh
	mov	al,1
	cmp	VideoMono,0
	jz	@@pl2
	mov	ax,07001h
@@pl2:	call	BarWindow
	ret
	;
@@PrintEntry:	mov	edi,_LV_ListStart
	mov	ebx,ecx
	call	@@PointEntry
	jc	@@pre9
	mov	ch,bl
	mov	cl,0
	call	@@PrintIt
@@pre9:	ret
	;
@@PointEntry:	or	cx,cx
	jz	@@pe1
	cmp	d[edi],-1
	jz	@@pe9
	add	edi,4+4+4+4
	dec	ecx
	jmp	@@PointEntry
@@pe1:	clc
	ret
@@pe9:	stc
	ret
	;
@@PrintIt:	push	edi
	mov	bp,_LV_Handle
	call	LocateWindow
	mov	edi,offset ABuffer
	movzx	ecx,_LV_Longest
	mov	al,' '
	rep	stosb
	xor	al,al
	stosb
	pop	edi
	push	edi
	mov	esi,[edi+4]
	lea	esi,SFT_Name[esi]
	mov	edi,offset ABuffer
	mov	al,' '
	stosb
@@pi0:	movsb
	cmp	b[esi-1],0
	jnz	@@pi0
	mov	b[edi-1]," "
	pop	edi
	;
	mov	ebx,offset ABuffer
	mov	bp,_LV_Handle
	call	PrintWindow
	ret
	;
@@None:	;
@@ExitNone:	xor	eax,eax
	jmp	@@Exit
	;
@@ExitYes:	mov	edi,_LV_ListStart
	mov	cx,_LV_BarPos
	call	@@PointEntry
	jc	@@ExitNone
	mov	edi,_LV_ListStart
	sub	edi,LINEList
	sub	edi,4
	shr	edi,4
	movzx	eax,_LV_BarPos
	inc	edi
	add	eax,edi
	;
@@Exit:	pushm	ecx,edx,eax
	mov	bp,_LV_Handle
	or	bp,bp
	jz	@@NoWindow
	call	CloseWindow
@@NoWindow:	popm	ecx,edx,eax
	ret
ListSourceFiles endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DisasFull	proc	near
	call	BarOFF
	call	CurrentOFF
	call	BreaksOFF
	call	GenDisas
	call	DisplayDisas		;Update disasembly window.
	call	BreaksON
	call	CurrentON
	call	BarON
	ret
DisasFull	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DisasPart	proc	near
	call	BarOFF
	call	CurrentOFF
	call	BreaksOFF
	call	GenDisas
	cmp	DisasGened,0
	jz	@@0
	call	DisplayDisas		;Update disasembly window.
@@0:	call	BreaksON
	call	CurrentON
	call	BarON
	ret
DisasPart	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RegsHandler	proc	near
	cmp	ax,Message_Control
	jnz	@@Update
	call	CheckExit
	push	es
	mov	es,RealSegment
	movzx	ebx,es:w[417h]
	pop	es
	mov	eax,offset HelpText2
	test	ebx,8
	jz	@@a
	mov	eax,offset HelpText2a
@@a:
	call	SetNewHelp

;Check for right button.

	test	MouseB,2		;right button?
	jz	@@n0
	call	RegsGoto
	jmp	@@9
	;
@@n0:	cmp	b[keys+1],106
	jnz	@@k20
	call	RegsListVars
	jmp	@@9
	;
@@k20:	cmp	b[Keys+1],75	;left
	jnz	@@k0
	cmp	RegsXPos,0
	jnz	@@k_0
	call	RegisterOFF
	mov	RegsXPos,7
	jmp	@@k2_1
@@k_0:	call	RegisterOFF
	dec	RegsXPos
	call	RegisterON
	jmp	@@9
	;
@@k0:	cmp	b[Keys+1],77	;right
	jnz	@@k1
@@k0_0:	cmp	RegsXPos,7
	jc	@@k0_1
	call	RegisterOFF
	mov	RegsXPos,0
	jmp	@@k1_0
@@k0_1:	call	RegisterOFF
	inc	RegsXPos
	call	RegisterON
	jmp	@@9
	;
@@k1:	cmp	b[Keys+1],80	;down
	jnz	@@k2
@@k1_0:	cmp	RegsYPos,9
	jc	@@k1_1
	call	RegisterOFF
	mov	RegsYPos,0
	call	RegisterON
	jmp	@@9
@@k1_1:	call	RegisterOFF
	inc	RegsYPos
	call	RegisterON
	jmp	@@9
	;
@@k2:	cmp	b[Keys+1],72	;up
	jnz	@@k3
@@k2_1:	cmp	RegsYPos,0
	jnz	@@k2_0
	call	RegisterOFF
	mov	RegsYPos,9
	call	RegisterON
	jmp	@@9
@@k2_0:	call	RegisterOFF
	dec	RegsYPos
	call	RegisterON
	jmp	@@9
	;
@@k3:	cmp	b[Keys],'0'
	jc	@@k4
	cmp	b[Keys],'9'+1
	jnc	@@k3_0
	mov	al,b[Keys]
	sub	al,'0'
	jmp	@@k3_1
@@k3_0:	cmp	b[Keys],'A'
	jc	@@k4
	cmp	b[Keys],'F'+1
	jnc	@@k4
	mov	al,b[Keys]
	sub	al,'A'
	add	al,10
@@k3_1:	movzx	esi,RegsYPos
	mov	esi,d[DebugDis2RegList+esi*4]
	movzx	eax,al
	mov	ebx,0fh
	movzx	ecx,RegsXPos
	and	ecx,7
	xor	ecx,111b
	shl	ecx,2
	shl	eax,cl
	shl	ebx,cl
	xor	ebx,-1
	and	d[esi],ebx
	or	d[esi],eax
	call	RegisterDisplay	;Display current register values.
	jmp	@@k0_0
	;
@@k4:	cmp	b[Keys+1],61	; F3
	jnz	@@k5
	call	RegsGoto
	jmp	@@9
	;
@@k5:	cmp	b[Keys],'+'
	jnz	@@k6
	movzx	esi,RegsYPos
	mov	esi,d[DebugDis2RegList+esi*4]
	inc	d[esi]
	call	RegisterDisplay	;Display current register values.
	jmp	@@9
	;
@@k6:	cmp	b[Keys],'-'
	jnz	@@k7
	movzx	esi,RegsYPos
	mov	esi,d[DebugDis2RegList+esi*4]
	dec	d[esi]
	call	RegisterDisplay	;Display current register values.
	jmp	@@9
	;
@@k7:
	cmp	b[Keys+1],104	;F1?
	jnz	@@k8
	xor	RegsSelTranslate,1
	call	RegisterDisplay	;Display current register values.
	jmp	@@9

@@k8:
	cmp	b[Keys+1],63	;F5?
	jnz	@@k9
	call	FPUDisplay
	jmp	@@9

@@k9:
	jmp	@@9

@@Update:
	cmp	ax,Message_Update
	jnz	@@9
	mov	RegsHandle,bp
	call	RegisterDisplay	;Display current register values.

@@9:	ret
RegsHandler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Prompt for new expresion to base this window on.
;
;On Entry:-
;
;BP	- Window handle.
;
RegsGoto	proc	near
	mov	_RG_DHandle,bp
	mov	eax,offset HelpText0
	call	SetNewHelp
	mov	cl,b[VideoColumns]
	shr	cl,1
	sub	cl,(3+40)/2
	mov	bl,b[VideoRows]
	shr	bl,1
	sub	bl,(3+1)/2
	mov	ch,40+2
	mov	bh,3+1
	mov	ax,WindowClear+WindowBox+WindowStatic+WindowShad
	mov	dh,7fh
	mov	dl,1fh
	mov	esi,0
	mov	ebp,0
	call	OpenWindow		;open main display window.
	mov	_RG_Handle,bp
	mov	ebx,offset RegValText
	mov	ax,WindowJLeft+WindowBox
	call	TitleWindow
	call	ShowWinTitle
	;
	mov	cl,1	;X
	mov	ch,0	;Y
	mov	bx,40	;width.
	mov	bp,_RG_Handle
	mov	edi,offset CBuffer
	call	WindowInput
	push	eax
	mov	bp,_RG_Handle
	call	CloseWindow
	pop	eax
	cmp	ah,1		;ESC?
	jz	@@9
	cmp	b[CBuffer],0
	jz	@@9
	mov	esi,offset CBuffer
	mov	edi,offset EvaluateBuffer
@@1:	movsb
	cmp	b[esi-1],0
	jnz	@@1
	mov	VarSizeMask,0
	mov	VarSizeMask+4,0
	call	EvaluateData
	jc	@@99
	movzx	esi,RegsYPos
	mov	esi,d[DebugDis2RegList+esi*4]
	mov	eax,d[EvaluateBuffer]
	mov	[esi],eax
	call	RegisterDisplay	;Display current register values.
	jmp	@@9
@@99:	mov	ebx,offset BadExpresion
	call	WindowPopup
@@9:	ret
RegsGoto	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RegisterOFF	proc	near
	pushad
	cmp	RegsLastX,-1
	jz	@@9
	mov	cl,RegsLastX
	mov	ch,RegsLastY
	mov	RegsLastX,-1
	mov	bp,RegsHandle
	mov	al,3
	call	BarWindow
@@9:	popad
	ret
RegisterOFF	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RegisterON	proc	near
	pushad
	;
	mov	cl,RegsXPos
	mov	ch,RegsYPos
	add	cl,4
	add	ch,1
	mov	RegsLastX,cl
	mov	RegsLastY,ch
	mov	bp,RegsHandle
	mov	al,2
	call	BarWindow
	;
@@9:	popad
	ret
RegisterON	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
CheckExit	proc	near
	cmp	w[Keys],45*256
	jz	System
	cmp	b[Keys+1],113
	jnz	@@k0
	call	RestartALL
	jmp	@@k9
	;
@@k0:	cmp	b[Keys+1],111
	jnz	@@k1
	call	GetFileName
	cmp	ah,1
	jz	@@k9
	mov	edx,offset CBuffer
	call	LoadSourceFile
@@k1:	;
@@k9:	ret
CheckExit	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
GetFileName	proc	near
	mov	eax,offset HelpText0
	call	SetNewHelp
	mov	b[CBuffer],0
	mov	cl,b[VideoColumns]
	shr	cl,1
	sub	cl,(3+40)/2
	mov	bl,b[VideoRows]
	shr	bl,1
	sub	bl,(3+1)/2
	mov	ch,40+2+1
	mov	bh,2+1+1
	mov	ax,WindowClear+WindowBox+WindowStatic+WindowShad
	mov	dl,1fh
	mov	dh,7fh
	mov	esi,0
	mov	ebp,esi
	call	OpenWindow		;open main display window.
	push	bp
	mov	ebx,offset LoadFileText
	mov	ax,WindowJLeft+WindowBox
	call	TitleWindow
	call	PointWindow
	call	ShowWinTitle
	pop	bp
	push	bp
	mov	cl,1	;X
	mov	ch,0	;Y
	mov	bx,40	;width.
	mov	edi,offset CBuffer
	call	WindowInput
	pop	bp
	push	eax
	call	CloseWindow
	pop	eax
	ret
GetFileName	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Load a source file and add it to the list.
;
;On Entry:
;
;EDX	- File name.
;
;On Exit:
;
;EAX	- status
;	0 - no error.
;	1 - file error.
;	2 - not enough memory.
;EBX	- file table pointer.
;
LoadSourceFile	proc	near
;
;Find out how big the file is.
;
	mov	_LSF_FileMem,0
	mov	_LSF_LineMem,0
	mov	_LSF_FileName,edx
	mov	ax,3d00h
	int	21h
	jc	@@file_error
	mov	bx,ax
	mov	al,2
	xor	cx,cx
	mov	dx,cx
	call	SetFilePointer	;find out how long the file is.
	shl	edx,16
	mov	dx,cx
	push	edx
	mov	al,0
	xor	cx,cx
	mov	dx,cx
	call	SetFilePointer	;move back to the start of the file.
	pop	ecx
	;
	inc	ecx
	call	malloc		;try and allocate the memory.
	jnc	@@0
	mov	ah,3eh		;close the file again.
	int	21h
	jmp	@@mem_error
	;
@@0:	dec	ecx
	mov	_LSF_FileMem,esi
	mov	b[esi+ecx],0		;terminate the file.
	mov	edi,esi
	push	ecx
	call	ReadFile		;read the file.
	pop	ecx
	push	eax
	mov	ah,3eh
	int	21h		;close the file.
	pop	eax
	cmp	eax,ecx		;did we read enough?
	jnz	@@file_error
	;
	mov	ecx,4		;line count size.
	call	malloc
	jc	@@mem_error
	mov	d[esi],0		;clear lines so far.
	mov	_LSF_LineMem,esi
	;
	mov	esi,_LSF_FileMem
@@1:	mov	edi,esi		;store line start.
@@2:	lodsb
	cmp	al,10		;check for end of line.
	jz	@@3
	cmp	al,13		;check for end of line.
	jz	@@4
	or	al,al		;check for end of file.
	jz	@@5
	jmp	@@2
	;
@@3:	mov	b[esi-1],0		;terminate the line.
	cmp	b[esi],13
	jnz	@@5
	inc	esi
	jmp	@@5
@@4:	mov	b[esi-1],0		;terminate the line.
	cmp	b[esi],10
	jnz	@@5
	inc	esi
@@5:	;
	push	esi
	push	eax		;save terminating character.
	mov	esi,_LSF_LineMem
	mov	ecx,d[esi]		;get line count
	or	cl,cl
	clc
	jnz	@@5_0
	add	ecx,256
	shl	ecx,2		;dword entries.
	add	ecx,4		;allow for count dword.
	call	remalloc		;re allocate memory.
@@5_0:	mov	ebx,esi
	pop	eax
	pop	esi
	jc	@@mem_error
	mov	_LSF_LineMem,ebx
	mov	ecx,[ebx]		;get line count again.
	inc	d[ebx]		;update number of lines.
	shl	ecx,2		;dword entries.
	add	ecx,4		;allow for line count.
	mov	[ebx+ecx],edi	;store line address.
	;
	or	al,al		;did we find the end of the file?
	jnz	@@1		;find all lines.
;
;Now we need to add this file to the file table.
;
	mov	ecx,size SFT
	call	Malloc
	jc	@@mem_error
	mov	edi,esi
	xor	al,al
	cld
	rep	stosb
	mov	ebx,esi
	mov	esi,SourceFileTable
	mov	ecx,[esi]
	inc	ecx
	shl	ecx,2
	add	ecx,4
	call	ReMalloc
	jc	@@mem_error
	mov	SourceFileTable,esi
	inc	d[esi]
	sub	ecx,4
	add	esi,ecx
	mov	d[esi],ebx		;store pointer to this struc.
	mov	esi,ebx
	;
	;Now fill in the details.
	;
	mov	eax,_LSF_LineMem
	mov	SFT_Lines[esi],eax
	mov	_LSF_LineMem,0
	mov	eax,_LSF_FileMem
	mov	SFT_File[esi],eax
	mov	_LSF_FileMem,0
	mov	SFT_X[esi],0
	mov	SFT_Y[esi],0
	push	esi
	lea	edi,SFT_Name[esi]
	mov	esi,_LSF_FileName
@@8:	movsb
	cmp	b[esi-1],0
	jnz	@@8
	pop	esi
	;
	push	esi
	cmp	SourceFileWindow,0
	jnz	@@8_0
	mov	SourceOpen,esi
	mov	cl,0
	mov	bl,0+1
	mov	ch,59+5+2
	mov	bh,23-6
	mov	ax,WindowClear+WindowBox+WindowStatic
	mov	dl,30h
	mov	dh,3fh
	mov	esi,0
	mov	ebp,offset FilesHandler
	call	OpenWindow		;open the window.
	pop	esi
	mov	SFT_Handle[esi],bp	;store the handle.
	lea	ebx,SFT_Name[esi]
	mov	ax,WindowJCent+WindowBox
	push	esi
	call	TitleWindow		;add the windows title.
@@8_0:	pop	ebx
	;
	xor	eax,eax
	jmp	@@exit
	;
@@file_error:	mov	eax,1
	jmp	@@exit
	;
@@mem_error:	mov	eax,2
	jmp	@@exit
	;
@@exit:	push	eax
	cmp	_LSF_FileMem,0
	jz	@@exit0
	mov	esi,_LSF_FileMem
	call	Free
@@exit0:	cmp	_LSF_LineMem,0
	jz	@@exit1
	mov	esi,_LSF_LineMem
	call	Free
@@exit1:	pop	eax
	ret
LoadSourceFile	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DisplaySourceFile proc near
	call	FindFileHandle
	call	ClearWindow		;start by cleearing the window.
	push	esi
	call	PointWindow
	movzx	ebx,WindowDepth2[esi]
	pop	esi
	;
	mov	edx,SFT_Y[esi]
	mov	esi,SFT_Lines[esi]	;get line list.
	mov	ecx,[esi]
	sub	ecx,edx
	cmp	ecx,ebx
	jc	@@2
	mov	ecx,ebx
@@2:	add	esi,4
	jecxz	@@9
	shl	edx,2
	add	esi,edx
	;
@@3:	mov	ebx,[esi]
	call	PrintWindow
	mov	ebx,offset CarriageReturn
	call	PrintWindow
	add	esi,4
	loop	@@3
	;
@@9:	ret
DisplaySourceFile endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
FindFileHandle	proc	near
	push	ecx
	push	eax
	mov	esi,SourceFileTable
	mov	ecx,[esi]
	add	esi,4
@@0:	push	esi
	mov	esi,[esi]
	cmp	SFT_Handle[esi],bp
	jz	@@1
	pop	esi
	add	esi,4
	dec	ecx
	jnz	@@0
	xor	bp,bp
	;
@@1:	pop	LastFilePointer
	pop	eax
	pop	ecx
	ret
FindFileHandle	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
FilesHandler	proc	near
	cmp	ax,Message_Control
	jnz	@@Update
	call	CheckExit
	push	es
	mov	es,RealSegment
	movzx	ebx,es:w[417h]
	pop	es
	mov	eax,offset HelpText6
	test	ebx,8
	jz	@@a
	mov	eax,offset HelpText6a
@@a:	call	SetNewHelp
	;
	cmp	b[keys+1],80
	jnz	@@k0
	call	PointWindow
	movzx	ecx,WindowDepth2[esi]
	call	FindFileHandle
	add	ecx,SFT_Y[esi]
	mov	edi,SFT_Lines[esi]
	cmp	ecx,[edi]
	jnc	@@9
	inc	SFT_Y[esi]
	call	ScrollWindowU
	shl	ecx,2
	add	edi,4
	add	edi,ecx
	call	PointWindow
	movzx	ecx,WindowDepth2[esi]
	dec	ecx
	xchg	ch,cl
	call	LocateWindow
	mov	ebx,[edi]
	call	PrintWindow
	jmp	@@9
	;
@@k0:	cmp	b[keys+1],72
	jnz	@@k1
	call	FindFileHandle
	cmp	SFT_Y[esi],0
	jz	@@9
	dec	SFT_Y[esi]
	push	esi
	push	ebp
	call	ScrollWindowD
	pop	ebp
	push	ebp
	xor	cx,cx
	call	LocateWindow
	pop	ebp
	pop	esi
	mov	ecx,SFT_Y[esi]
	shl	ecx,2
	mov	esi,SFT_Lines[esi]
	add	esi,4
	add	esi,ecx
	mov	ebx,[esi]
	call	PrintWindow
	jmp	@@9
	;
@@k1:	cmp	b[Keys+1],81
	jnz	@@k2
	call	PointWindow
	movzx	ecx,WindowDepth2[esi]
	call	FindFileHandle
	add	ecx,ecx
	add	ecx,SFT_Y[esi]
	inc	ecx
	mov	edi,SFT_Lines[esi]
	cmp	ecx,[edi]
	jc	@@k1_0
	jz	@@k1_0
	push	esi
	call	PointWindow
	movzx	ecx,WindowDepth2[esi]
	pop	esi
	sub	ecx,[edi]
	neg	ecx
	mov	SFT_Y[esi],ecx
	jmp	@@k1_1
@@k1_0:	push	esi
	call	PointWindow
	movzx	ecx,WindowDepth2[esi]
	pop	esi
	add	SFT_Y[esi],ecx
@@k1_1:	call	DisplaySourceFile
	jmp	@@9
	;
@@k2:	cmp	b[Keys+1],73
	jnz	@@k3
	call	PointWindow
	movzx	ecx,WindowDepth2[esi]
	call	FindFileHandle
	cmp	SFT_Y[esi],ecx
	jnc	@@k2_0
	mov	SFT_Y[esi],0
	jmp	@@k2_1
@@k2_0:	sub	SFT_Y[esi],ecx
@@k2_1:	call	DisplaySourceFile
	jmp	@@9
	;
@@k3:	cmp	b[Keys+1],79
	jnz	@@k4
	call	PointWindow
	movzx	ecx,WindowDepth2[esi]
	call	FindFileHandle
	mov	edi,SFT_Lines[esi]
	sub	ecx,[edi]
	neg	ecx
	mov	SFT_Y[esi],ecx
	call	DisplaySourceFile
	jmp	@@9
	;
@@k4:	cmp	b[Keys+1],71
	jnz	@@k5
	call	FindFileHandle
	mov	SFT_Y[esi],0
	call	DisplaySourceFile
	jmp	@@9
	;
@@k5:	cmp	b[Keys+1],68
	jnz	@@k6
	pushad
	call	FindFileHandle
	cmp	esi,ERRFile
	jnz	@@noterr
	mov	ERRFile,0
@@noterr:	popad
	push	ebp
	call	CloseWindow
	pop	ebp
	call	FindFileHandle
	push	esi
	mov	esi,SFT_Lines[esi]
	call	Free
	pop	esi
	push	esi
	mov	esi,SFT_File[esi]
	call	Free
	pop	esi
	call	Free
	mov	esi,SourceFileTable
	mov	ecx,[esi]
	mov	esi,LastFilePointer
	sub	esi,SourceFileTable
	sub	esi,4
	shr	esi,2
	sub	ecx,esi
	dec	ecx
	mov	edi,LastFilePointer
	mov	esi,edi
	add	esi,4
	cld
	rep	movsd
	mov	esi,SourceFileTable
	dec	d[esi]
	jmp	@@9
	;
@@k6:	jmp	@@9
	;
@@Update:	cmp	ax,Message_Update
	jnz	@@9
	cmp	SourceOpen,0
	jz	@@0
	mov	esi,SourceOpen
	mov	SourceOpen,0
	mov	SFT_Handle[esi],bp
@@0:	call	DisplaySourceFile
	jmp	@@9
	;
@@9:	ret
FilesHandler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Move the display EIP down a line.
;
DisplayEIPDown	proc	near
	cmp	SourceIndex,0	;Useing source?
	jz	@@OldWay
;
;Get window depth.
;
	mov	bp,DisasHandle
	call	PointWindow
	movzx	ecx,[esi].WindowDepth2
;
;See if enough source left to move us down.
;
	mov	esi,SourceIndex	;Get the source file index.
	dec	esi		;Make it zero based.
	shl	esi,4		;4 dwords per entry.
	add	esi,4		;skip count dword.
	add	esi,LINEList
	mov	esi,[esi+4]		;point to file table entry.
	mov	esi,SFT_Lines[esi]	;Point to line list.
	mov	edx,[esi]		;Get entry count.
	add	esi,4		;Skip entry count.
	mov	ebx,SourceLineNum	;get line number.
	inc	ebx
	cmp	ebx,edx
	jnc	@@Exit		;can't move any more.
	mov	SourceLineNum,ebx
;
;See if we should move the start line number and re-do the entire display.
;
	mov	eax,SourceStartLine
	add	eax,ecx
	cmp	ebx,eax		;reached the bottom yet?
	jc	@@Part
	inc	SourceStartLine	;update starting line number.
;
;Just update the current position cursor.
;
@@Part:	or	SourceNoTrack,-1
	call	DisasPart
	jmp	@@Exit
;
;Use old method of movement.
;
@@OldWay:	mov	fs,DisplayCS
	mov	esi,DisplayEIP
	mov	edi,offset ABuffer
	mov	b[edi],0
	call	Disasemble
@@check:	cmp	esi,DisasEndEIP
	jc	@@ok
	pushm	esi,fs
	mov	fs,DisasCS
	mov	esi,DisasStartEIP
	mov	edi,offset ABuffer
	mov	b[edi],0
	call	Disasemble
	mov	DisplayEIP,esi
	mov	DisasOldDepth,-1
	call	GenDisas
	call	DisplayDisas
	popm	esi,fs
	jmp	@@Check
@@ok:	mov	DisplayCS,fs
	mov	DisplayEIP,esi
	call	DisasPart
	mov	edx,DisasStartEIP
	mov	cx,DisplayCS
	call	AddCSEIP
@@Exit:	ret
DisplayEIPDown	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DisplayEIPPDown proc near
;
;Get the window depth.
;
	mov	bp,DisasHandle
	call	PointWindow
	movzx	ecx,[esi].WindowDepth2
;
;Check what the display type is.
;
	cmp	SourceIndex,0	;Useing source?
	jz	@@OldWay
;
;See if enough source left to move us down.
;
	mov	esi,SourceIndex	;Get the source file index.
	dec	esi		;Make it zero based.
	shl	esi,4		;4 dwords per entry.
	add	esi,4		;skip count dword.
	add	esi,LINEList		;point to the source code.
	mov	esi,[esi+4]
	mov	esi,SFT_Lines[esi]	;Point to line list.
	mov	edx,[esi]		;Get entry count.
	mov	ebx,SourceStartLine	;get start line number.
	add	ebx,ecx
	mov	eax,ebx
	add	eax,ecx		;enough space for whole page?
	cmp	eax,edx		;too far?
	jl	@@0
	mov	ebx,edx		;get last line number +1
	sub	ebx,ecx		;move back a page.
	or	ebx,ebx
	jns	@@0		;check we didn't go negative.
	xor	ebx,ebx
@@0:	mov	SourceStartLine,ebx
;
;Move display line down a page.
;
	mov	ebx,SourceLineNum	;get current line number.
	add	ebx,ecx
	cmp	ebx,edx		;too far?
	jc	@@1
	mov	ebx,edx		;get last line number +1
	dec	ebx		;zero relative.
	or	ebx,ebx
	jns	@@1		;check we didn't go negative.
	xor	ebx,ebx
@@1:	mov	SourceLineNum,ebx
;
;Update the display.
;
	or	SourceNoTrack,-1
	call	DisasPart
	jmp	@@Exit
;
;Work out what to do from disasembly.
;
@@OldWay:	;
@@k8_0:	push	ecx
	mov	fs,DisplayCS
	mov	esi,DisplayEIP
	mov	edi,offset ABuffer
	mov	b[edi],0
	call	Disasemble
	mov	DisplayCS,fs
	mov	DisplayEIP,esi
	pop	ecx
	loop	@@k8_0
	call	DisasPart
	mov	edx,DisasStartEIP
	mov	cx,DisplayCS
	call	AddCSEIP
@@Exit:	ret
DisplayEIPPDown endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Move the display EIP up a line.
;
DisplayEIPUp	proc	near
	cmp	SourceIndex,0	;Useing source?
	jz	@@OldWay
;
;Move source position.
;
	mov	esi,SourceIndex	;Get the source file index.
	dec	esi		;Make it zero based.
	shl	esi,4		;4 dwords per entry.
	add	esi,4		;skip count dword.
	add	esi,LINEList		;index into the list.
	mov	esi,[esi+4]
	mov	esi,SFT_Lines[esi]	;Point to line list.
	mov	edx,[esi]		;Get entry count.
	mov	ebx,SourceLineNum	;get current line number.
	or	ebx,ebx		;anywhere to go?
	jz	@@Exit
	dec	SourceLineNum	;update line number.
	cmp	ebx,SourceStartLine	;were we at top of the page?
	jnz	@@Part
;
;update display start line number.
;
	dec	SourceStartLine
;
;Update the display.
;
@@Part:	or	SourceNoTrack,-1
	call	DisasPart
	jmp	@@Exit
;
;Use good old fashioned disasembly to work it out.
;
@@OldWay:	call	FindLastEIP
	mov	DisplayEIP,eax
	call	DisasPart
	mov	edx,DisasStartEIP
	mov	cx,DisplayCS
	call	AddCSEIP
@@Exit:	ret
DisplayEIPUp	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Move the display up a page.
;
DisplayEIPPUp	proc	near
	cmp	SourceIndex,0
	jz	@@OldWay
	;
	mov	bp,DisasHandle
	call	PointWindow
	movzx	ecx,[esi].WindowDepth2
;
;Point to this files details.
;
	mov	esi,SourceIndex	;Get the source file index.
	dec	esi		;Make it zero based.
	shl	esi,4		;4 dwords per entry.
	add	esi,4		;skip count dword.
	add	esi,LINEList		;index into the list.
	mov	esi,[esi+4]
	mov	esi,SFT_Lines[esi]	;Point to line list.
	mov	edx,[esi]		;Get entry count.
;
;Move the display start line.
;
	mov	ebx,SourceStartLine	;get start line number.
	sub	ebx,ecx
	jns	@@0
	xor	ebx,ebx
@@0:	mov	SourceStartLine,ebx
;
;Move the current line.
;
	mov	ebx,SourceLineNum
	sub	ebx,ecx
	jns	@@1
	xor	ebx,ebx
@@1:	mov	SourceLineNum,ebx
;
;Update the display.
;
	or	SourceNoTrack,-1
	call	DisasPart
	jmp	@@Exit
;
;Use good old fasioned disasembly to work it out.
;
@@OldWay:	mov	bp,DisasHandle
	call	PointWindow
	movzx	ecx,[esi].WindowDepth2
@@k7_0:	push	ecx
	call	FindLastEIP
	mov	DisplayEIP,eax
	pop	ecx
	loop	@@k7_0
	call	DisasPart
	mov	edx,DisasStartEIP
	mov	cx,DisplayCS
	call	AddCSEIP
@@Exit:	ret
DisplayEIPPUp	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
FindLastEIP	proc	near
;
;See if we're still in the display CS:EIP list.
;
	mov	bp,DisasHandle
	call	PointWindow
	movzx	ecx,[esi].WindowDepth2
	mov	eax,DisplayEIP
	mov	esi,offset DisasEIPList
	xor	edx,edx
@@t1:	cmp	eax,[esi]
	jz	@@t2
	inc	edx
	add	esi,4
	loop	@@t1
	jmp	@@FindIt
	;
@@t2:	;Is there one above this?
	;
@@t3:	or	edx,edx
	jz	@@FindIt
	sub	esi,4
	mov	eax,[esi]
	dec	edx
	cmp	eax,-1
	jz	@@t3
	ret
;
;See if there is a symbol real close above current position, if there is, use
;it otherwise do it the unreliable way.
;
	;
@@FindIt:	;Get current EIP
	;
	mov	dx,DisplayCS
	mov	eax,DisplayEIP
	;
	;Set default search starting point.
	;
	mov	edi,0
	cmp	dx,DebugCS
	jnz	@@4
	cmp	eax,DebugEIP		;debug point above or below?
	jc	@@4
	jz	@@4
	mov	edi,DebugEIP
@@4:	mov	ecx,DisplayEIP
	sub	ecx,edi
	;
	;Search for nearest symbol above now.
	;
	mov	esi,SymbolList
@@0:	cmp	d[esi],-1		;end of the list?
	jz	@@1
	cmp	dx,SymbolSeg[esi]
	jnz	@@2
	cmp	eax,SymbolDWord[esi]	;right value?
	jc	@@2
	jz	@@2
	mov	ebx,eax
	sub	ebx,SymbolDWord[esi]
	cmp	ebx,ecx		;closest so far?
	jnc	@@2
	mov	ecx,ebx
	mov	edi,SymbolDWord[esi]
@@2:	add	esi,SymbolNext[esi]
	jmp	@@0
@@1:	;
	;See if CS:EIP list has a better alternative.
	;
	mov	esi,offset SearchCSEIPList
	mov	ebp,MaxEIPs
@@5:	cmp	dx,[esi+4]		;same segment?
	jnz	@@6
	cmp	eax,[esi]		;right direction?
	jc	@@6
	jz	@@6
	mov	ebx,eax
	sub	ebx,[esi]		;get differance.
	cmp	ebx,ecx		;closest so far?
	jnc	@@6
	mov	ecx,ebx
	mov	edi,[esi]
@@6:	add	esi,6
	dec	ebp
	jnz	@@5
	;
	;See if its near enough.
	;
	mov	eax,edi
	mov	esi,edi
	sub	edi,DisplayEIP
	neg	edi
	cmp	edi,100
	jnc	@@NotNearEnough
	;
	;Disasemble from nearest value until we get to current position
	;and store.
	;
	mov	fs,DisplayCS
@@3:	mov	edi,offset ABuffer
	mov	b[edi],0
	push	esi
	call	Disasemble
	pop	eax
	cmp	esi,DisplayEIP
	jc	@@3
	ret
;
;What we've got to work with:-
;
;Current CS:EIP.
;
;Move back predetermind amount, disasemble till current position reached or
;passed. If passed then start again with the pointer moved on. If an invalid
;code is generated then do the same.
;
@@NotNearEnough:
	mov	esi,DisplayEIP	;get current position.
	cmp	esi,20		;check can move back 15 bytes.
	jnc	@@n0
	xor	esi,esi		;reset to start of segment.
@@n0:	sub	esi,20		;move back to new starting point.
	mov	fs,DisplayCS
	;
	mov	edx,esi
	mov	ebp,edx
@@n2:	push	ebp
	push	edx
	mov	edi,offset ABuffer
	mov	b[edi],0
	push	esi
	call	Disasemble
	mov	ecx,eax
	pop	eax
	pop	edx
	pop	ebp
	;
	cmp	cx,Retcode_INV	;invalid code generated?
	jz	@@n1
	cmp	esi,DisplayEIP
	jc	@@n2
	jz	@@GotOne
@@n1:	inc	edx
	mov	esi,edx
	cmp	edx,DisplayEIP	;this shouldn't happen.
	jc	@@n2
@@n3:	;
@@GotOne:	ret
FindLastEIP	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
AddCSEIP	proc	near
	mov	esi,offset SearchCSEIPList
	mov	ebp,MaxEIPs
	mov	ebx,-1
	mov	edi,0
	and	cx,0fffch
@@0:	cmp	w[esi+4],0
	jnz	@@nz
	or	edi,edi
	jnz	@@nz
	mov	edi,esi
@@nz:	cmp	cx,[esi+4]		;right segment?
	jnz	@@n
	cmp	edx,[esi]
	jc	@@b
@@p:	mov	eax,edx
	sub	eax,[esi]
	jmp	@@c
@@b:	mov	eax,[esi]
	sub	eax,edx
@@c:	cmp	eax,ebx
	jnc	@@n
	mov	ebx,eax
@@n:	add	esi,6
	dec	ebp
	jnz	@@0
	cmp	ebx,256
	jc	@@nope
	or	edi,edi
	jz	@@nope
	mov	[edi+4],cx
	mov	[edi],edx
@@nope:	ret
AddCSEIP	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Fetch symbols from map file etc.
;
FetchSymbols	proc	near
	mov	edx,offset SymFileName
	mov	ax,3d02h
	int	21h
	jc	@@LookMap
	mov	SymHandle,ax
	;
	mov	edx,offset SymIDSpace
	mov	bx,ax
	mov	cx,8
	mov	ah,3fh
	int	21h
	jc	@@close_sym
	cmp	ax,8
	jnz	@@close_sym
	cmp	d[SymIDSpace],"SDWC"
	jnz	@@close_sym
	cmp	w[SymIDSpace+4],"MY"
	jnz	@@close_sym
	;
	;Check SYM version.
	;
	cmp	w[SymIDSpace+6],"20"
	jz	@@sym_v_ok
;
;Problem with SYM file so close it and revert to MAP scanning.
;
@@close_sym:	mov	bx,SymHandle
	mov	SymHandle,0
	mov	ah,3eh
	int	21h
	jmp	@@LookMap
;
;We appear to have a valid SYM format so try loading the file.
;
@@sym_v_ok:	mov	bp,TempHandle
	call	ClearWindow
	mov	ebx,offset LoadingText
	call	PrintWindow
	mov	ebx,offset SymFileName
	call	PrintWindow
	;
	mov	bx,SymHandle
	mov	ax,4202h
	xor	cx,cx
	mov	dx,cx
	int	21h
	mov	bx,dx
	shl	ebx,16
	mov	bx,ax
	sub	ebx,8		;allow for ID
	;
	or	ebx,ebx		;check for zero length file.
	jnz	@@notzero
	mov	ErrorNumber,5
	ret
	;
@@notzero:	push	ebx
	mov	ecx,ebx
	mov	SymbolList+4,ecx
	add	ecx,4		;allow for terminator.
	call	Malloc
	pop	ebx
	jc	@@9
	pushm	ebx,esi
	mov	bx,SymHandle
	mov	ax,4200h
	xor	cx,cx
	mov	dx,8		;allow for ID
	int	21h
	popm	ebx,esi
	mov	edi,esi
	mov	ecx,ebx
	mov	bx,SymHandle
	pushm	ecx,edi
	call	ReadFile
	popm	ecx,edx
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	SymbolList,edx
	mov	SymbolBase,edx
	mov	bx,SymHandle
	mov	ah,3eh
	int	21h
	;
	;Now process SYM file details.
	;
	push	fs
	mov	fs,DebugPSP
	mov	ax,fs:w[EPSP_SegBase]
	pop	fs
	mov	esi,SymbolList
@@sym0:	cmp	SymbolType[esi],255
	jz	@@sym2_0
	cmp	SymbolType[esi],0
	jnz	@@sym2
@@sym2_0:	shl	SymbolSeg[esi],3
	add	SymbolSeg[esi],ax
	;
@@sym2:	cmp	d[esi],-1		;end of the list?
	jz	@@sym9
	;
	add	esi,SymbolNext[esi]
	jmp	@@sym0
	;
@@sym9:	;Now do bodge to remove -1 from last entry and make a new last entry.
	;
	mov	eax,SymbolList+4	;get file length.
	add	eax,SymbolList
	sub	eax,esi		;get length of last entry.
	mov	[esi],eax		;update it.
	add	esi,eax
	mov	d[esi],-1		;set new end of list.
	;
	mov	ErrorNumber,0
	jmp	@@FLATUpdate
@@LookMap:	call	InitFileBuffer
	;
	;Try and open the map file.
	;
	mov	ErrorNumber,5
	mov	edx,offset MapFileName
	mov	ax,3d02h
	int	21h
	jc	@@9
	mov	MapHandle,ax		;store the handle.
	mov	bp,TempHandle
	call	ClearWindow
	mov	ebx,offset LoadingText
	call	PrintWindow
	mov	ebx,offset MapFileName
	call	PrintWindow
	;
	;Look for segment list ID string.
	;
@@LookID:	mov	edi,offset LineBuffer
	mov	bx,MapHandle
	call	ReadLine		;read a line from the map file.
	jc	@@9
	or	ax,ax		;anything to look at?
	jnz	@@9
	cmp	cx,1024
	jnc	@@90
	or	cx,cx
	jz	@@9
	;
	;See if right text on this line.
	;
	mov	esi,offset SymHeaderText
	mov	edi,offset LineBuffer
	;
@@0:	cmp	b[esi],' '		;need to skip white space.
	jz	@@1
	cmp	b[esi],9
	jnz	@@2
@@1:	inc	esi
	jmp	@@0
	;
@@2:	cmp	b[edi],' '		;skip white space.
	jz	@@3
	cmp	b[esi],9
	jnz	@@4
@@3:	inc	edi
	jmp	@@2
	;
@@4:	cmp	b[esi],0		;end of the header string?
	jz	@@5
	cmp	b[edi],0		;end of the line buffer?
	jz	@@LookID
	;
	mov	al,[esi]
	call	UpperChar		;case insensitive search.
	xchg	ah,al
	mov	al,[edi]
	call	UpperChar
	cmp	al,ah
	jnz	@@LookID		;next line if they don't match
	inc	esi
	inc	edi
	jmp	@@0		;next char if they do.
	;
@@5:	;Find the next none blank line.
	;
	mov	bx,MapHandle
	mov	edi,offset LineBuffer
	call	ReadLine		;read another line.
	jc	@@9
	or	ax,ax		;did we get anything?
	jnz	@@DoneSyms		;bloody strange! No segments to process.
	or	cx,cx		;blank line?
	jz	@@5		;keep reading till something happens.
	cmp	cx,1024
	jnc	@@90
	cmp	b[LineBuffer],0
	jz	@@5
	;
	;Looks like we can start fetching symbol values at last.
	;
	mov	SymbolSize,0
	;
@@LookSym:	mov	esi,offset LineBuffer	;source data.
	mov	edi,offset SymLayout-1	;definition of data layout.
	mov	ebx,offset CurrentSym
	mov	SymbolDWord[ebx],0	;reset linear base.
	mov	SymbolSeg[ebx],0	;reset byte limit.
	mov	SymbolTLen[ebx],0
	mov	SymbolNext[ebx],-1
	mov	SymbolText[ebx],0

	mov	SymbolType[ebx],0	; MED 12/15/95
	;
@@6:	cmp	b[esi],' '		;skip leading white space.
	jz	@@7
	cmp	b[esi],9
	jnz	@@8
@@7:	inc	esi
	jmp	@@6
	;
@@8:	inc	edi		;move to next item on the list.
	cmp	b[edi],-1		;finished scan?
	jz	@@NextSym
	cmp	b[edi],0		;ignoring this column?
	jz	@@Ignore
	cmp	b[edi],1		;start address?
	jz	@@Start
	jmp	@@Name		;must be 2 (name) then.
	;
@@Ignore:	cmp	b[esi],' '		;scan till more white space.
	jz	@@6		;check next item in the list.
	cmp	b[esi],9
	jz	@@6		;check next item in the list.
	inc	esi
	jmp	@@Ignore
	;
@@Start:	xor	edx,edx		;reset acumulated value.

@@11:
	movzx	eax,b[esi]		;fetch a digit.
	or	al,al
	jz	@@10		;finished geting value so store it.
	cmp	al,' '
	jz	@@10		;finished geting value so store it.
	cmp	al,':'
	jnz	@@NotSeg
	;

COMMENT !
	push	fs
	mov	fs,DebugPSP
	add	edx,fs:d[EPSP_MemBase]
	pop	fs

	pushm	ecx,esi
	push	fs
	mov	fs,DebugPSP
	movzx	ecx,fs:w[EPSP_SegSize]
	pop	fs
	jecxz	@@dump
	;
	push	fs
	mov	fs,DebugPSP
	mov	bx,fs:w[EPSP_SegBase]
	pop	fs
END COMMENT !
	pushm	ecx,esi
	push	edx
	mov	bx,DebugSS
	sys	GetSelDet32
	test	edx,edx	; check if FLAT stack
	pop	edx			; KEEP COMPARE FLAG STATUS
	jnz	notflat		; no
	dec	edx			; make segment number relative zero
	shl	edx,3		; 8/bytes descriptor
	push	fs
	mov	fs,DebugPSP
	add	dx,fs:w[EPSP_SegBase]	; offset from base descriptor
	mov	ebx,edx		; ebx holds selector value
	pop	fs
	jmp	@@gotseg

notflat:
	shl	edx,4		; convert seg to absolute address
	push	fs
	mov	fs,DebugPSP
	add	edx,fs:d[EPSP_MemBase]
	movzx	ecx,fs:w[EPSP_SegSize]
	mov	bx,fs:w[EPSP_SegBase]
	pop	fs
	jecxz	@@dump

	mov	ebp,edx

@@findseg:
	sys	GetSelDet32
	cmp	ebp,edx
	jz	@@gotseg
	add	ebx,8
	loop	@@findseg
	xor	edx,edx
	jmp	@@dump
	;
@@gotseg:
	mov	edx,ebx
@@dump:
	popm	ecx,esi
	;

;	push	ebx
;	mov	ebx,offset CurrentSym
;	mov	SymbolSeg[ebx],dx	;store start address.
;	pop	ebx
	mov	eax,OFFSET CurrentSym
	mov	SymbolSeg[eax],dx	; store start address

	inc	esi
	xor	edx,edx
	jmp	@@11
	;
@@NotSeg:
	call	ASCII2Bin
	jc	@@90
	cmp	al,'H'		;end of the number?
	jz	@@30
	shl	edx,4		;update acumulated value.
	add	edx,eax		;/
	inc	esi
	jmp	@@11		;keep reading till we run out.
@@30:	inc	esi		;skip 'H'
@@10:	push	ebx
	mov	ebx,offset CurrentSym
	mov	SymbolDWord[ebx],edx	;store start address.
	pop	ebx
	jmp	@@6		;do next item along.
	;
@@Name:	;Copy name text.
	;
	push	edi
	xor	ecx,ecx
	mov	edi,offset CurrentSym
	add	edi,SymbolText
@@101:	movsb
	inc	ecx
	cmp	b[esi-1],0
	jz	@@102
	cmp	b[esi-1],' '
	jz	@@102
	cmp	b[esi-1],9
	jnz	@@101
@@102:	mov	b[edi-1],0
	dec	ecx
	mov	edi,offset CurrentSym
	mov	SymbolTLen[edi],cl
	pop	edi
	jmp	@@6
	;
@@NextSym:	mov	ebx,offset CurrentSym
	movzx	ecx,SymbolTLen[ebx]	;get string length.
	add	ecx,size SymbolStruc-1
	mov	SymbolNext[ebx],ecx
	add	ecx,4
	mov	ebx,ecx
	;
	cmp	SymbolSize,0
	jnz	@@GotMem
	push	ebx
	call	Malloc
	pop	ecx
	jc	@@DoneSyms
	mov	SymbolList,esi
	mov	SymbolBase,esi
	mov	d[esi],-1
	jmp	@@UseMem
	;
@@GotMem:	push	ebx
	add	ecx,SymbolSize
	mov	esi,SymbolList
	call	ReMalloc
	pop	ecx
	jc	@@DoneSyms
	mov	SymbolList,esi
	mov	SymbolBase,esi
	;
@@UseMem:	mov	edi,SymbolList
	add	edi,SymbolSize
	mov	esi,offset CurrentSym
	sub	ecx,4
	push	ecx
	cld
	rep	movsb
	pop	ecx
	mov	d[edi],-1
	add	SymbolSize,ecx
	;
	mov	bx,MapHandle
	mov	edi,offset LineBuffer
	call	ReadLine
	jc	@@9
	or	ax,ax		;EOF?
	jnz	@@DoneSyms
	cmp	cx,1024
	jnc	@@90
	cmp	b[LineBuffer],0
	jz	@@DoneMAPSyms
	or	cx,cx		;Blank line?
	jnz	@@LookSym		;Fetch next segment value.
	;
@@DoneMAPSyms:	mov	ErrorNumber,0

; MED, fall through to flat update 12/15/95
;	jmp	@@DoneSyms

	;
@@FLATUpdate:	;Now try and make symbols work if we're in FLAT mode.
	;
	mov	bx,DebugSS
	sys	GetSelDet32
	or	edx,edx		;FLAT stack seg?
	jnz	@@fs9
	;
	mov	RegsSelTranslate,0
	;
	mov	esi,SymbolList
@@fs0:	cmp	d[esi],-1		;end of the list?
	jz	@@fs9
	push	esi
	cmp	SymbolType[esi],255
	jz	@@fs2
	cmp	SymbolType[esi],0
	jnz	@@fs1
@@fs2:	movzx	eax,SymbolSeg[esi]
	push	fs
	mov	fs,DebugPSP
	movzx	ebx,fs:w[EPSP_SegBase]
	mov	edx,fs:d[EPSP_MemBase]
	pop	fs
	sub	eax,ebx		;Need relative value.
	and	eax,not 7		;Need an index.
	add	eax,DebugSegs	;Get pointer to this segment.
	push	es
	mov	es,RealSegment
	mov	eax,es:[eax]		;Get segment base offset.
	pop	es
	add	eax,edx		;add in load address.
	add	SymbolDWord[esi],eax
@@fs1:	pop	esi
	add	esi,SymbolNext[esi]
	jmp	@@fs0
@@fs9:	;
	;Now scan for line number records and make a list of them.
	;
	mov	ErrorNumber,3
	mov	ecx,4
	call	Malloc
	jc	@@9
	mov	LINEList,esi
	mov	d[esi],0
	;
	;Scan the SYM stuff.
	;
	mov	esi,SymbolList
	mov	LastSymbol,0
@@ln0:	cmp	d[esi],-1		;end of the list?
	jz	@@ln9
	push	esi
	cmp	SymbolType[esi],255
	clc
	jnz	@@ln100
	push	esi
	mov	esi,LINEList
	mov	ecx,[esi]
	inc	ecx
	shl	ecx,4		;4 dwords per entry.
	add	ecx,4		;include the count dword.
	call	ReMalloc
	mov	eax,esi
	pop	esi
	jc	@@ln1
	mov	LINEList,eax
	inc	d[eax]		;bump the count.
	sub	ecx,16		;move back to new entry.
	add	ecx,eax
	mov	d[ecx],esi		;store pointer to this record.
	mov	d[ecx+4],0
	mov	d[ecx+8],0
	mov	d[ecx+12],0
	;
	;Try and load the source file specified.
	;
	pushm	ecx,esi
	pushm	ecx,esi,edi
	movzx	ecx,SymbolTLen[esi]
	lea	esi,SymbolText[esi]	;point to the file name.
	mov	edi,offset FileNameSpace
	rep	movsb
	xor	al,al
	stosb
	popm	ecx,esi,edi
	popm	ecx,esi
;
;Take this symbol out of the list.
;
	pushad
	add	esi,SymbolNext[esi]	;point to next symbol.
	mov	edi,LastSymbol
	sub	esi,edi
	or	edi,edi
	jnz	@@rems0
	mov	edi,offset SymbolList
@@rems0:	mov	[edi],esi
	popad
;
;Reset source path pointer.
;
	mov	SourcePathPointer,offset SourceFilePath
	;
	push	SourceFileWindow
	or	SourceFileWindow,-1	;don't want a window.
;
;Build a file name from source path list.
;
@@NewName:	pushm	ecx,esi
	;
	mov	esi,SourcePathPointer
	cmp	b[esi],0
	stc
	jz	@@nn4
	mov	edi,offset FileNameSpace2
@@nn0:	movsb
	cmp	b[esi-1],";"
	jz	@@nn1
	cmp	b[esi-1],0
	jnz	@@nn0
@@nn1:	dec	edi
	cmp	edi,offset FileNameSpace2
	jz	@@nn2
	cmp	b[edi-1],"\"
	jz	@@nn2
	mov	b[edi],"\"
	inc	edi
@@nn2:	mov	SourcePathPointer,esi
	mov	esi,offset FileNameSpace
@@nn3:	movsb
	cmp	b[esi-1],0
	jnz	@@nn3
	jmp	@@nn5
	;
@@nn4:	popm	ecx,esi
	pop	SourceFileWindow
	mov	eax,LINEList
	dec	d[eax]
	clc
	jmp	@@ln1
	;
	;Check if we already have this file.
	;
@@nn5:	mov	edx,offset FileNameSpace2
	call	FindSourceFile
	jc	@@fndit0
	popm	ecx,esi
	pop	SourceFileWindow
	movzx	eax,SymbolTLen[esi]
	add	eax,size SymbolStruc
	add	eax,esi
	cmp	d[eax],0
	jz	@@shity
	clc
	jmp	@@ln2
@@shity:	mov	eax,LINEList
	dec	d[eax]
	clc
	jmp	@@ln1
@@fndit0:	;
	mov	edx,offset FileNameSpace2
	call	OpenFile
	jnc	@@nn6
	popm	ecx,esi
	jmp	@@NewName
@@nn6:	mov	bx,ax
	call	CloseFile
	;
	pushad
	mov	bp,TempHandle
	call	ClearWindow
	mov	ebx,offset LoadingText
	call	PrintWindow
	mov	ebx,offset FileNameSpace	;use base file name.
	call	PrintWindow
	popad
	;
	mov	edx,offset FileNameSpace2
	call	LoadSourceFile
	popm	ecx,esi
	pop	SourceFileWindow
	cmp	eax,1		;file error?
	clc
	jz	@@ln1
	mov	ErrorNumber,3
	or	eax,eax
	jz	@@ln2
	stc
	jmp	@@ln1
	;
@@ln2_0:	popm	ecx,esi
	pop	SourceFileWindow
	;
@@ln2:	;Add this file to the entry.
	;
	mov	[ecx+4],ebx		;store file list address.
	clc
	jmp	@@ln1
	;
@@ln100:	mov	LastSymbol,esi
	;
@@ln1:	pop	esi
	jc	@@9
	add	esi,SymbolNext[esi]
	jmp	@@ln0
@@ln9:	;
	mov	esi,LINEList
	or	esi,esi
	jz	@@ln90
	mov	ecx,[esi]
	inc	ecx
	shl	ecx,4		;4 dwords per entry.
	add	ecx,4		;include the count dword.
	call	ReMalloc
	mov	eax,esi
	jc	@@ln90
	mov	LINEList,eax
	sub	ecx,16		;move back to new entry.
	add	ecx,eax
	or	d[ecx],-1		;store pointer to this record.
	;
@@ln90:	mov	ErrorNumber,0
	clc
@@90:	;
@@9:	;
@@DoneSyms:	ret
FetchSymbols	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-=-=-=-=-=-=-=-=-=
FindSourceFile	proc	near
	pushm	eax,ecx,edx,esi,edi,ebp
	mov	esi,SourceFileTable
	mov	ecx,[esi]
	add	esi,4
@@0:	or	ecx,ecx
	jz	@@9
	mov	edi,[esi]
	lea	edi,SFT_Name[edi]
	mov	ebx,edx
@@1:	mov	al,[edi]
	mov	ah,[ebx]
	call	UpperChar
	xchg	ah,al
	call	UpperChar
	cmp	al,ah
	jnz	@@2
	inc	edi
	inc	ebx
	or	al,al
	jz	@@8
	jmp	@@1
@@2:	add	esi,4
	dec	ecx
	jmp	@@0
	;
@@8:	mov	ebx,[esi]
	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popm	eax,ecx,edx,esi,edi,ebp
	ret
FindSourceFile	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-=-=-=-=-=-=-=-=-=
ASCII2Bin	proc	near
;
;Convert hex digit in AL into binary value.
;
	call	UpperChar
	cmp	al,'H'
	jz	@@8
	cmp	al,'0'
	jc	@@9
	cmp	al,'A'
	jc	@@Dec
	cmp	al,'F'+1
	jnc	@@9
	sub	al,'A'-10
	jmp	@@8
@@Dec:	cmp	al,'9'+1
	jnc	@@9
	sub	al,'0'
@@8:	clc
	ret
	;
@@9:	stc
	ret
ASCII2Bin	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-=-=-=-=-=-=-=-=-=
UpperChar	proc	near
;
;Convert a character into upper case.
;
;On Entry:-
;AL - Character to convert.
;
;On Exit:-
;AL - New character code.
;
	cmp	al,61h		; 'a'
	jb	@@0
	cmp	al,7Ah		; 'z'
	ja	@@0
	and	al,5Fh
@@0:	ret
UpperChar	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;See if we're supposed to be useing source.
;
GenDisas	proc	near
	cmp	DontUseSource,0
	jnz	@@Change
	;
	mov	bp,DisasHandle
	call	PointWindow
	movzx	ecx,[esi].WindowDepth2
	mov	ebx,offset DisasEIPList
	mov	edi,offset DisasBuffer
	call	GenDisasSource
	jnc	@@200
	cmp	SourceIndex,0	;we're we useing source before?
	mov	SourceIndex,0
	jnz	@@0
	;
@@Change:	cmp	ForcedDisas,0
	jnz	@@00
	;
	mov	DisasGened,0
	mov	ax,DisplayCS
	cmp	ax,DisasCS
	jnz	@@0
	mov	eax,DisplayEIP
	cmp	eax,DisasStartEIP
	jc	@@0
	cmp	eax,DisasEndEIP
	jnc	@@0
	mov	bp,DisasHandle
	call	PointWindow
	mov	al,WindowDepth2[esi]
	cmp	al,DisasOldDepth
	jz	@@2
	;
@@0:	mov	bp,DisasHandle
	call	PointWindow
	mov	al,WindowDepth2[esi]
	mov	DisasOldDepth,al
	mov	eax,DisplayEIP
	mov	DisasStartEIP,eax
	mov	DisasEIP,eax
	mov	ax,DisplayCS
	mov	DisasCS,ax
	jmp	@@01
	;
@@00:	mov	ForcedDisas,0
	;
@@01:	mov	bp,DisasHandle
	call	PointWindow
	call	BoxWindow
	mov	ebx,offset DisasTitle
	mov	ax,WindowJCent+WindowBox
	call	TitleWindow
	;
	mov	bp,DisasHandle
	call	PointWindow
	movzx	ecx,[esi].WindowDepth2
	mov	ebx,offset DisasEIPList
	mov	edi,offset DisasBuffer
;
;Not useing source for this section so do it the old way.
;
	mov	fs,DisplayCS
	mov	esi,DisasStartEIP
	mov	DisasGened,-1
	;
@@1:	pushm	ebx,ecx
	mov	dx,fs
	mov	eax,esi
	push	esi
	call	Bin2Symbol
	mov	eax,esi
	pop	esi
	jc	@@3
	push	esi
	mov	esi,eax
	cld
	mov	ax,2f01h
	cmp	VideoMono,0
	jz	@@dm0
	mov	ax,07001h
@@dm0:	stosw
	rep	movsb
	pushad
	mov	bp,DisasHandle
	call	PointWindow
	mov	ah,WindowAttrib[esi]
	mov	al,1
	stosw
	popad
	add	edi,2
	pop	esi
	mov	b[edi+0],13
	mov	b[edi+1],10
	mov	b[edi+2],0
	add	edi,2
	mov	d[ebx],-1
	popm	ebx,ecx
	add	ebx,4
	loop	@@5
	jmp	@@4
	;
@@5:	pushm	ebx,ecx
@@3:	mov	[ebx],esi
	mov	DisasLastEIP,esi
	push	SymbolTranslate
	mov	eax,DisasSymbolTranslate
	mov	SymbolTranslate,eax
	call	Disasemble
	pop	SymbolTranslate
	mov	b[edi+0],13
	mov	b[edi+1],10
	mov	b[edi+2],0
	add	edi,2
	;
	popm	ebx,ecx
	add	ebx,4
	dec	ecx
	jnz	@@1
@@4:	mov	DisasEndEIP,esi
	jmp	@@2
	;
@@200:	mov	eax,SourceIndex
	dec	eax
	shl	eax,4
	add	eax,4
	mov	esi,LINEList
	add	esi,eax
	mov	esi,[esi+4]
	mov	eax,SourceStartLine
	mov	SFT_SY[esi],eax
	mov	eax,SourceLineNum
	mov	SFT_Y[esi],eax
@@2:	;
	mov	eax,SourceIndex
	mov	SourceIndex+4,eax
	ret
GenDisas	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Display source in the disasembly window.
;
;On Entry:
;
;ECX	- Lines to generate.
;EBX	- EIP list
;EDI	- Output buffer.
;
GenDisasSource	proc	near
	pushad
	mov	DisasGened,0
	cmp	LINEList,0
	jz	@@Nope
	mov	eax,LINEList
	cmp	d[eax],0
	jz	@@Nope
	cmp	SourceIndex,0
	jz	@@Find
;
;Is this a window size change?
;
	mov	bp,DisasHandle
	call	PointWindow
	mov	al,WindowDepth2[esi]
	cmp	al,DisasOldDepth
	jnz	@@yesf		;just re-generate.
	;
	cmp	SourceForceCheck,0
	jnz	@@yesf
	;
	cmp	SourceNoTrack,0
	jnz	@@Check
;
;Is this a window size change?
;
@@yesf:	mov	bp,DisasHandle
	call	PointWindow
	mov	al,WindowDepth2[esi]
	cmp	al,DisasOldDepth
	jnz	@@Gen		;just re-generate.
;
;Check that the display EIP falls within the indicated display.
;
	mov	esi,SourceIndex	;Get the source file index.
	dec	esi		;Make it zero based.
	shl	esi,4		;dword per entry.
	add	esi,4		;skip count dword.
	add	esi,LINEList		;Point to the source line list.
	cmp	d[esi+4],0		;Any line list here?
	jz	@@Find
	mov	esi,[esi]		;Point to SymbolStruc
	mov	ax,SymbolSeg[esi]	;Get segment
	cmp	ax,DisplayCS
	jnz	@@Find
	;
	pushm	ebx,ecx,edi
	mov	edi,ebx
	movzx	eax,SymbolTLen[esi]
	dec	eax
	add	eax,size SymbolStruc
	mov	ebx,SymbolDword[esi]
	add	esi,eax		;Move to EIP list.
	mov	ebp,[esi]		;Get number of entries.
	add	esi,4
	mov	edx,SourceStartLine	;Display start point.
	inc	edx
@@s3:	pushm	esi,ebp
@@s4:	or	ebp,ebp		;End of the table?
	jz	@@s7
	cmp	edx,[esi]		;Reached this line yet?
	jz	@@s6
	add	esi,4+4		;Next line entry.
	dec	ebp
	jmp	@@s4
@@s6:	mov	eax,ebx
	add	eax,[esi+4]		;Add in this EIP value.
	cmp	eax,DisplayEIP	;right EIP?
	jnz	@@s7
	popm	esi,ebp
	popm	ebx,ecx,edi
	jmp	@@Got		;We found it so generate the listing.
@@s7:	popm	esi,ebp
	add	edi,4
	inc	edx		;Next line number.
	dec	ecx
	jnz	@@s3
	popm	ebx,ecx,edi
;
;CS:EIP is not in current source list so scan all available files for the right
;value and setup the display to suit.
;
@@Find:	pushm	fs,esi
	mov	fs,DisplayCS
	mov	esi,DisplayEIP
	call	FindSourceFSESI
	popm	fs,esi
	jnc	@@Found
;
;Couldn't find what we're after so revert to normal disasembly.
;
@@Nope:	mov	SourceForceCheck,0
	mov	SourceIndex,0
	popad
	stc
	ret
;
;We found it so position the source as best we can.
;
@@Found:	mov	eax,SourceSearchIndex
	inc	eax
	mov	SourceIndex,eax
	;
	mov	edx,SourceSearchLine
	dec	edx		;start a bit above if we can.
	push	esi
	mov	esi,SourceIndex	;Get the source file index.
	dec	esi		;Make it zero based.
	shl	esi,4		;4 dwords per entry.
	add	esi,4		;skip count dword.
	add	esi,LINEList		;Point to the source line list.
	mov	esi,[esi+4]		;Point to the file structure.
	mov	esi,SFT_Lines[esi]
	mov	esi,[esi]		;Get number of lines.
	mov	eax,edx
	add	eax,ecx
	cmp	eax,esi
	jl	@@f1
	sub	eax,esi
	sub	edx,eax
@@f1:	pop	esi
	or	edx,edx
	jns	@@f0
	xor	edx,edx
@@f0:	mov	SourceStartLine,edx
	;
	mov	edx,SourceSearchLine
	inc	edx
	or	SourceIndex+4,-1
;
;We found the EIP now set the line index.
;
@@Got:	dec	edx
	mov	SourceLineNum,edx	;Set line number.
	;
@@Check:	mov	eax,SourceIndex
	cmp	eax,SourceIndex+4
	jnz	@@Gen
	mov	eax,SourceStartLine
	cmp	eax,SourceStartLine+4
	jz	@@Done
	;
@@Gen:	mov	eax,SourceIndex
	mov	SourceIndex+4,eax
	mov	eax,SourceStartLine
	mov	SourceStartLine+4,eax
	or	DisasGened,-1	;make sure it's displayed.
;
;Set display windows title.
;
	pushad
	mov	esi,SourceIndex	;Get the source file index.
	dec	esi		;Make it zero based.
	shl	esi,4		;4 dwords per entry.
	add	esi,4		;skip count dword.
	add	esi,LINEList		;Point to the source line list.
	mov	esi,[esi+4]		;Point to the file structure.
	lea	esi,SFT_Name[esi]	;point to the file name.
	mov	edi,offset DisasTitleSpace+2
	mov	ebx,esi
@@st0:	mov	al,[esi]
	inc	esi
	cmp	al,"\"
	jnz	@@st1
	mov	ebx,esi
@@st1:	or	al,al
	jnz	@@st0
	mov	esi,ebx
@@st2:	movsb
	cmp	b[esi-1],0
	jnz	@@st2
	mov	bp,DisasHandle
	call	BoxWindow
	mov	ebx,offset DisasTitleSpace
	mov	ax,WindowJCent+WindowBox
	call	TitleWindow
	popad
;
;Find the line data for this source and set DisplayCS
;
	mov	esi,SourceIndex	;Get the source file index.
	dec	esi		;Make it zero based.
	shl	esi,4		;4 dwords per entry.
	add	esi,4		;skip count dword.
	add	esi,LINEList		;Point to the source line list.
	mov	esi,[esi]		;Point to SymbolStruc
	mov	ax,SymbolSeg[esi]	;Get segment
	mov	DisplayCS,ax
	mov	DisasCS,ax
;
;Now build the EIP table.
;
	pushm	ebx,ecx,edi
	mov	GotStartEIP,0
	mov	edi,offset DisasEIPList
	movzx	eax,SymbolTLen[esi]
	dec	eax
	add	eax,size SymbolStruc
	mov	ebx,SymbolDword[esi]
	add	esi,eax		;Move to EIP list.
	mov	ebp,[esi]		;Get number of entries.
	add	esi,4
	mov	edx,SourceStartLine	;Display start point.
	inc	edx
@@3:	pushm	esi,ebp
@@4:	or	ebp,ebp		;End of the table?
	jz	@@5
	cmp	edx,[esi]		;Reached this line yet?
	jz	@@6
	add	esi,4+4		;Next line entry.
	dec	ebp
	jmp	@@4
@@5:	mov	d[edi],-1
	jmp	@@7
@@6:	mov	eax,ebx
	add	eax,[esi+4]		;Add in this EIP value.
	mov	[edi],eax
	cmp	GotStartEIP,0
	jnz	@@7
	mov	DisasStartEIP,eax
	or	GotStartEIP,-1
@@7:	popm	esi,ebp
	add	edi,4
	inc	edx		;Next line number.
	dec	ecx
	jnz	@@3
	popm	ebx,ecx,edi
;
;Update the display buffer.
;
	mov	esi,SourceIndex	;Get the source file index.
	dec	esi		;Make it zero based.
	shl	esi,4		;dword per entry.
	add	esi,4		;skip count dword.
	add	esi,LINEList		;index into the list.
	mov	esi,[esi+4]
	mov	esi,SFT_Lines[esi]	;Point to line list.
	mov	edx,[esi]		;Get entry count.
	add	esi,4		;Skip entry count.
	mov	ebx,SourceStartLine
@@0:	cmp	ebx,edx		;Past end of the file?
	jnc	@@2
	push	esi
	mov	esi,[esi+ebx*4]	;Point to the line data.
	mov	eax,"    "
	stosd
@@1:	movsb
	cmp	b[esi-1],0
	jnz	@@1
	mov	b[edi-1],13
	mov	b[edi],10
	mov	b[edi+1],0
	inc	edi
	pop	esi
@@2:	inc	ebx		;Update the line number.
	dec	ecx
	jnz	@@0		;Do enough lines.
	;
@@Done:	mov	bp,DisasHandle
	call	PointWindow
	mov	al,WindowDepth2[esi]
	mov	DisasOldDepth,al
	;
	mov	SourceForceCheck,0
	;
	popad
	clc
	ret
GenDisasSource	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;See if any of the source files can supply the line for this CS:EIP
;
;FS:ESI	- Current CS:EIP value.
;
FindSourceFSESI proc near
	pushad
	mov	edi,LINEList		;Point to the source line list.
	mov	ebp,[edi]		;Get number of entries.
	add	edi,4
	sub	edi,16
	inc	ebp
	mov	SourceSearchIndex,-1
@@LookSource0:	inc	SourceSearchIndex
	dec	ebp		;Anything left to look at?
	jz	@@NoSource1
	add	edi,16		;Point to new entry.
	cmp	d[edi+4],0		;Did we get a file?
	jz	@@LookSource0
	mov	ebx,[edi]		;Point to SymbolStruc
	mov	ax,fs
	cmp	ax,SymbolSeg[ebx]	;Right segment?
	jnz	@@LookSource0
	;
	;See if any lines have the right EIP.
	;
	movzx	ecx,SymbolTLen[ebx]
	dec	ecx
	add	ecx,size SymbolStruc
	mov	eax,SymbolDword[ebx]
	add	ebx,ecx		;Move to EIP list.
	mov	ecx,[ebx]		;Get number of entries.
	add	ebx,4
	xor	edx,edx
@@LookSource1:	or	ecx,ecx
	jz	@@NoSource2
	push	eax
	add	eax,[ebx+4]		;Add in this EIP value.
	cmp	eax,esi
	pop	eax
	jz	@@GotSource0
	add	ebx,4+4
	inc	edx
	dec	ecx
	jmp	@@LookSource1
@@NoSource2:	jmp	@@LookSource0
@@NoSource1:	popad
@@NoSource0:	stc
	ret
;
;We found a matching line so return the relavent details.
;
;EDX	- Entry number.
;
@@GotSource0:	mov	SourceSearchLineIndex,edx
	mov	edx,[ebx]
	dec	edx
	mov	SourceSearchLine,edx
	popad
	clc
	ret
FindSourceFSESI endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;See if any of the source files can supply the line for this CS:EIP
;
;FS:ESI	- Current CS:EIP value.
;
RangeSourceFSESI proc near
	pushad
	mov	edi,LINEList		;Point to the source line list.
	mov	ebp,[edi]		;Get number of entries.
	add	edi,4
	sub	edi,16
	inc	ebp
@@LookSource0:	dec	ebp		;Anything left to look at?
	jz	@@NoSource1
	add	edi,16		;Point to new entry.
	cmp	d[edi+4],0		;Did we get a file?
	jz	@@LookSource0
	mov	ebx,[edi]		;Point to SymbolStruc
	mov	ax,fs
	cmp	ax,SymbolSeg[ebx]	;Right segment?
	jnz	@@LookSource0
	;
	movzx	ecx,SymbolTLen[ebx]
	dec	ecx
	add	ecx,size SymbolStruc
	mov	eax,SymbolDword[ebx]
	add	ebx,ecx		;Move to EIP list.
	mov	ecx,[ebx]		;Get number of entries.
	add	ebx,4
	xor	edx,edx
	or	ecx,ecx
	jz	@@NoSource2
;
;See if 1st entry is ok
;
	push	eax
	add	eax,[ebx+4]		;Add in this EIP value.
	cmp	esi,eax
	pop	eax
	jc	@@NoSource2
;
;See if last entry is any use.
;
	dec	ecx
	shl	ecx,3
	add	ebx,ecx
	push	eax
	add	eax,[ebx+4]		;Add in this EIP value.
	cmp	eax,esi
	pop	eax
	jnc	@@GotSource0
@@NoSource2:	jmp	@@LookSource0
@@NoSource1:	popad
@@NoSource0:	stc
	ret
;
;We found a matching line so return the relavent details.
;
;EDX	- Entry number.
;
@@GotSource0:	popad
	clc
	ret
RangeSourceFSESI endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DisplayDisas	proc	near
	mov	bp,DisasHandle
	call	ClearWindow
	mov	ebx,offset DisasBuffer
	mov	bp,DisasHandle
	call	PrintWindow
	ret
DisplayDisas	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
CurrentOFF	proc	near
	cmp	DisasCurrent,-1
	jz	@@0
	mov	bp,DisasHandle
	call	PointWindow
	mov	cx,DisasCurrent
	mov	ch,cl
	mov	cl,23
	cmp	SourceIndex,0
	jz	@@1
	xor	cl,cl
@@1:	call	LocateWindow
	mov	ebx,offset SpaceText
	call	PrintWindow
	mov	ah,WindowAttrib[esi]
	mov	al,1
	mov	cx,DisasCurrent
	mov	ch,cl
	call	BarWindow
	mov	DisasCurrent,-1
	;
@@0:	ret
CurrentOFF	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
BarOFF	proc	near
	cmp	DisasBar,-1
	jz	@@0
	mov	bp,DisasHandle
	call	PointWindow
	mov	ah,WindowAttrib[esi]
	mov	al,1
	mov	cx,DisasBar
	mov	ch,cl
	call	BarWindow
	mov	DisasBar,-1
	;
@@0:	ret
BarOFF	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
BreaksOFF	proc	near
	mov	esi,offset BreakDisList
@@0:	cmp	w[esi],-1
	jz	@@1
	push	esi
	mov	bp,DisasHandle
	mov	cx,[esi]
	call	PointWindow
	mov	ah,WindowAttrib[esi]
	mov	al,1
	mov	ch,cl
	call	BarWindow

	mov	ch,cl
	mov	cl,23-5
	cmp	SourceIndex,0
	jz	@@2
	xor	cl,cl
@@2:	call	LocateWindow
	mov	ebx,offset SpaceText6
	cmp	SourceIndex,0
	jz	@@3
	add	ebx,2
@@3:	call	PrintWindow

	pop	esi
	add	esi,2
	jmp	@@0
	;
@@1:	mov	BreakDisList,-1
	ret
BreaksOFF	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
CurrentON	proc	near
	mov	ax,DebugCS
	cmp	ax,DisasCS
	jnz	@@9
	;
	mov	bp,DisasHandle
	call	PointWindow
	movzx	ecx,[esi].WindowDepth2
	mov	eax,DebugEIP
	mov	esi,offset DisasEIPList
	xor	edx,edx
@@1:	cmp	eax,[esi]
	jz	@@2
	inc	edx
	add	esi,4
	loop	@@1
	jmp	@@9
	;
@@2:	mov	cx,dx
	mov	DisasCurrent,cx
	mov	ch,cl
	mov	bp,DisasHandle
	mov	cl,23
	cmp	SourceIndex,0
	jz	@@4
	xor	cl,cl
@@4:	call	LocateWindow
	mov	ebx,offset CurrentText
	call	PrintWindow
	mov	ax,02001h
	cmp	VideoMono,0
	jz	@@nm0
	mov	ax,07001h
@@nm0:	call	BarWindow
	;
@@9:	ret
CurrentON	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
BarON	proc	near
	cmp	SourceIndex,0
	jz	@@OldWay
	mov	bp,DisasHandle
	call	PointWindow
	movzx	ecx,[esi].WindowDepth2
	mov	edx,SourceLineNum
	sub	edx,SourceStartLine
	cmp	edx,ecx
	jc	@@n3
	mov	edx,ecx
	dec	edx
	mov	eax,edx
	add	eax,SourceStartLine
	mov	SourceLineNum,eax
@@n3:	sub	ecx,edx
	mov	eax,DisasEIP
	mov	ebx,edx
	shl	ebx,2
	add	ebx,offset DisasEIPList
@@n0:	or	ecx,ecx
	jz	@@n2
	cmp	d[ebx],-1
	jnz	@@n1
	add	ebx,4
	dec	ecx
	jmp	@@n0
@@n1:	mov	eax,[ebx]
@@n2:	mov	DisplayEIP,eax
	mov	DisasEIP,eax
	jmp	@@2
	;
@@OldWay:	mov	bp,DisasHandle
	call	PointWindow
	movzx	ecx,[esi].WindowDepth2
	mov	eax,DisplayEIP
	mov	esi,offset DisasEIPList
	xor	edx,edx
@@1:	cmp	eax,[esi]
	jz	@@2
	inc	edx
	add	esi,4
	loop	@@1
	jmp	@@9
	;
@@2:	mov	cx,dx
	mov	bp,DisasHandle
	mov	DisasBar,cx
	mov	ch,cl
	call	PointWindow
	mov	ah,WindowAttrib[esi]
	rol	ah,4
	mov	ah,1fh
	mov	al,1
	cmp	VideoMono,0
	jz	@@nm0
	mov	ax,07001h
@@nm0:	call	BarWindow
	;
@@9:	ret
BarON	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
BreaksON	proc	near
	mov	edi,offset BreakDisList
	mov	esi,offset BreakPointList
	mov	ecx,MaxBreaks
@@0:	test	BreakFlags[esi],BreakInUse
	jz	@@1
	cmp	BreakType[esi],BreakType_exec
	jnz	@@1
	pushm	ecx,esi,edi
	mov	bx,DisplayCS		;Need segment linear base address
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	edx,ecx
	mov	bp,DisasHandle
	call	PointWindow
	movzx	ebp,WindowDepth2[esi]
	popm	ecx,esi,edi
	mov	ebx,offset DisasEIPList
	push	edi
	xor	edi,edi
@@2:	mov	eax,edx
	add	eax,[ebx]
	cmp	eax,BreakLinear[esi]
	jnz	@@3
	mov	eax,edi
	pop	edi
	mov	[edi],ax
	add	edi,2
	pushm	ecx,esi,edi
	push	esi
	mov	bp,DisasHandle
	mov	ch,al
	mov	cl,23-5
	mov	b[BreakText+5],"*"
	cmp	SourceIndex,0
	jz	@@4
	xor	cl,cl
	mov	b[BreakText+5],0
@@4:	call	LocateWindow
	pop	esi
	pushad
	mov	eax,BreakCountDown[esi]
	mov	edi,offset BreakText+2
	mov	ecx,2
	call	Bin2Hex
	mov	ebx,offset BreakText
	cmp	SourceIndex,0
	jz	@@5
	inc	ebx
@@5:	call	PrintWindow
	popad
	mov	ax,04001h
	cmp	VideoMono,0
	jz	@@nm0
	mov	ax,07001h
@@nm0:	call	BarWindow
	popm	ecx,esi,edi
	jmp	@@1
	;
@@3:	add	ebx,4
	inc	edi
	dec	ebp
	jnz	@@2
	pop	edi
	;
@@1:	add	esi,size BreakStruc
	dec	ecx
	jnz	@@0
	mov	w[edi],-1
	ret
BreaksON	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;See if break point exists.
;
;On Entry:-
;
;AL	- Type
;EDX	- Linear address.
;
;On Exit:-
;
;Carry set if no break point.
;
;BX	- Break point handle.
;
IsBreakPoint	proc	near
	mov	esi,offset BreakPointList
	mov	ecx,MaxBreaks
	xor	ebx,ebx
@@0:	test	BreakFlags[esi],BreakInUse
	jz	@@1
	test	BreakFlags[esi],BreakHidden
	jnz	@@1
	cmp	BreakType[esi],al
	jnz	@@1
	cmp	BreakLinear[esi],edx
	jnz	@@1
	clc
	ret
	;
@@1:	add	esi,size BreakStruc
	inc	bx
	loop	@@0
	stc
	ret
IsBreakPoint	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;See if break point exists.
;
;On Entry:-
;
;AL	- Type
;EDX	- Linear address.
;
;On Exit:-
;
;Carry set if no break point.
;
;BX	- Break point handle.
;
IsBreakPointB	proc	near
	mov	esi,offset BreakPointList+(size BreakStruc*(MaxBreaks-1))
	mov	ecx,MaxBreaks
	mov	ebx,ecx
	dec	ebx
@@0:	test	BreakFlags[esi],BreakInUse
	jz	@@1
	test	BreakFlags[esi],BreakHidden
	jnz	@@1
	cmp	BreakType[esi],al
	jnz	@@1
	cmp	BreakLinear[esi],edx
	jnz	@@1
	clc
	ret
	;
@@1:	sub	esi,size BreakStruc
	dec	bx
	loop	@@0
	stc
	ret
IsBreakPointB	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Setup a break point.
;
;On entry:-
;
;AL	- type.
;EDX	- Linear address.
;
;On Exit:-
;
;AX	- Handle.
;
SetBreakPoint	proc	near
	mov	esi,offset BreakPointList
	mov	ecx,MaxBreaks
	xor	ebx,ebx
@@0:	test	BreakFlags[esi],BreakInUse
	jz	@@1
	add	esi,size BreakStruc
	inc	bx
	loop	@@0
	mov	ax,-1
	ret
	;
@@1:	;Have break point entry, set details.
	;
	mov	BreakFlags[esi],BreakInUse
	mov	BreakType[esi],al
	mov	BreakLinear[esi],edx
	mov	BreakCountDown[esi],0
	mov	BreakCount[esi],0
	mov	ax,bx
	ret
SetBreakPoint	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Setup a break point.
;
;On entry:-
;
;AL	- type.
;EDX	- Linear address.
;
;On Exit:-
;
;AX	- Handle.
;
SetBreakPointB	proc	near
	mov	esi,offset BreakPointList+(size BreakStruc*(MaxBreaks-1))
	mov	ecx,MaxBreaks
	mov	ebx,ecx
	dec	ebx
@@0:	test	BreakFlags[esi],BreakInUse
	jz	@@1
	sub	esi,size BreakStruc
	dec	bx
	loop	@@0
	mov	ax,-1
	ret
	;
@@1:	;Have break point entry, set details.
	;
	mov	BreakFlags[esi],BreakInUse
	mov	BreakType[esi],al
	mov	BreakLinear[esi],edx
	mov	ax,bx
	ret
SetBreakPointB	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Point to a break point entry.
;
;On Entry:-
;
;AX	- Handle.
;
PointBreakPoint proc	near
	pushm	eax,ebx
	mov	bx,size BreakStruc
	mul	bx
	movzx	esi,ax
	add	esi,offset BreakPointList
	popm	eax,ebx
	ret
PointBreakPoint endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Hide a break point entry.
;
;On Entry:-
;
;AX	- Handle.
;
HideBreakPoint	proc	near
	pushm	eax,ebx,esi
	mov	bx,size BreakStruc
	mul	bx
	movzx	esi,ax
	add	esi,offset BreakPointList
	or	BreakFlags[esi],BreakHidden
	popm	eax,ebx,esi
	ret
HideBreakPoint	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Release a break point entry.
;
;On Entry:-
;
;AX	- Handle.
;
RelBreakPoint	proc	near
	mov	bx,size BreakStruc
	mul	bx
	movzx	esi,ax
	add	esi,offset BreakPointList
	mov	BreakFlags[esi],0
	ret
RelBreakPoint	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Activate all break points.
;
SetBreakPoints	proc	near
	mov	esi,offset BreakPointList
	mov	ecx,MaxBreaks
@@0:	test	BreakFlags[esi],BreakInUse
	jz	@@1
	test	BreakFlags[esi],BreakHidden
	jnz	@@1
	cmp	BreakType[esi],BreakType_exec
	jz	@@exec
	jmp	@@1
	;
@@exec:	or	BreakFlags[esi],BreakActive
	push	es
	mov	es,RealSegment
	mov	edi,BreakLinear[esi]
	mov	al,es:[edi]		;Get current value.
	mov	BreakOld[esi],al
	mov	es:b[edi],0cch	;int 3
	pop	es
	;
@@1:	add	esi,size BreakStruc
	loop	@@0
	ret
SetBreakPoints	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Remove all active break points.
;
RelBreakPoints	proc	near
	mov	esi,offset BreakPointList+(size BreakStruc*(MaxBreaks-1))
	mov	ecx,MaxBreaks
@@0:	test	BreakFlags[esi],BreakInUse
	jz	@@1
	and	BreakFlags[esi],not BreakHidden
	test	BreakFlags[esi],BreakActive
	jz	@@1
	cmp	BreakType[esi],BreakType_exec
	jz	@@exec
	jmp	@@1
	;
@@exec:	push	es
	mov	es,RealSegment
	mov	edi,BreakLinear[esi]
	mov	al,BreakOld[esi]
	mov	es:[edi],al		;set current value.
	pop	es
	;
@@1:	sub	esi,size BreakStruc
	loop	@@0
	ret
RelBreakPoints	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Add a return address to the call stack. The bottom entry in the table is lost.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
AddRetAddress	proc	near
	pushad
	mov	edi,offset RecentCallStack+((4*RetStackSize)-4)
	mov	esi,offset RecentCallStack+((4*RetStackSize)-8)
	mov	ecx,RetStackSize-1
	std
	rep	movsd
	cld
	mov	RecentCallStack,edx
	popad
	ret
AddRetAddress	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Remove top entry from the call stack.
;
RemRetAddress	proc	near
	pushad
	mov	esi,offset RecentCallStack+4
	mov	edi,offset RecentCallStack
	mov	ecx,RetStackSize-1
	rep	movsd
	mov	RecentCallStack+((4*RetStackSize)-4),0
	popad
	ret
RemRetAddress	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Execute next instruction.
;
;On Entry:-
;
;Debug??	- Register variables.
;
;AL	- Mode.
;	0 - Trace.
;	1 - Next.
;	2 - Go.
;	3 - Special trace.
;
;On Exit:-
;
;Debug??	- Register variables updated.
;
ExecuteInst	proc	near
	pushm	w[ExecBreakHandle],w[ExecBreakHandle+2],w[DebugMode],w[VidSwapMode]
	pushm	DebuggerSS,DebuggerESP
	mov	ah,al
	and	al,127
	and	ah,128
	mov	DebugMode,al
	mov	DebugMode+1,ah
	or	VidSwapMode,-1	;default to no swap.
	push	w[DebugMode]
@@ExecAgain:	;
	mov	al,ForcedFlip
	or	al,AutoFlip
	mov	FlipSwap,al
	;
	;Get the instruction length & type.
	;
	mov	fs,DebugCS
	mov	esi,DebugEIP
@@DisLoop:	mov	edi,offset ABuffer
	mov	b[edi],0
	call	Disasemble
	mov	DebugRetCode,ax
	mov	NextCS,fs
	mov	NextEIP,esi
	mov	TargetCS,cx
	mov	TargetEIP,edx
	mov	TargetCS2,gs
	mov	TargetEIP2,ebp
	mov	TargetMode,ebx
;
;If doing source then we need to know here.
;
	cmp	SourceIndex,0
	jz	@@NoSource0
	cmp	LINEList,0
	jz	@@NoSource0
;
;If it's an F8 (next) then we need to do extra stuff.
;
	cmp	DebugMode,1
	jnz	@@NoSource0
;
;Find this instructions line data so we can find out what the next lines EIP is.
;
	push	esi
	mov	esi,DebugEIP
	call	FindSourceFSESI
	pop	esi
	jc	@@NoSource0		;shouldn't be able to happen.
	mov	eax,SourceSearchLine
	mov	ExecSourceLine,eax
;
;Get line pointer and see what's what.
;
@@sFind0:	mov	edi,SourceSearchIndex	;Get the source file index.
	shl	edi,4		;4 dwords per entry.
	add	edi,4		;skip count dword.
	add	edi,LINEList
	mov	edi,[edi]		;point to SymbolStruc
	mov	edx,SymbolDword[edi]
	movzx	ecx,SymbolTLen[edi]
	dec	ecx
	add	ecx,size SymbolStruc
	add	edi,ecx		;Move to EIP list.
	inc	SourceSearchLineIndex
	mov	eax,SourceSearchLineIndex
	cmp	eax,[edi]		;Another line?
	jnc	@@NoSource0
	add	edi,4
	shl	eax,3
	add	edi,eax		;Point to this line.
	mov	eax,[edi]		;Get line number.
	cmp	eax,ExecSourceLine
	jz	@@sFind0
	add	edx,[edi+4]		;Add in this EIP value.
	mov	NextEIP,edx
;
;Check swap mode needed.
;
@@NoSource0:	cmp	DebugMode,3
	jz	@@Trace
	cmp	DebugRetCode,RetCode_INT
	jnz	@@NoLookInt0
	cmp	DebugMode+1,0
	jz	@@SetSwapChk
@@NoLookInt0:	cmp	DebugRetCode,RetCode_INOUT
	jz	@@SetSwapChk
	cmp	DebugMode,4
	jz	@@ChkSwap
	cmp	DebugMode,0
	jnz	@@SetSwapChk
@@ChkSwap:	;
	test	bl,128
	jz	@@SetNoSwap
	and	bl,127
	or	bl,bl
	jz	@@SetNoSwap
	;
	cmp	DebugRetCode,RetCode_MOVS
	jnz	@@Normalea
	mov	bx,TargetCS2
	sys	GetSelDet32
	jc	@@SetSwapChk
	add	edx,TargetEIP2
	cmp	edx,0a0000h
	jc	@@Normalea
	cmp	edx,0c0003h+1
	jc	@@SetSwapChk
	;
@@Normalea:	mov	bx,TargetCS
	sys	GetSelDet32
	jc	@@SetSwapChk
	add	edx,TargetEIP
	cmp	edx,0a0000h
	jc	@@SetNoSwap
	cmp	edx,0c0003h+1
	jc	@@SetSwapChk
@@SetNoSwap:	jmp	@@DoneSwapChk
@@SetSwapChk:	mov	VidSwapMode,0
	;
@@DoneSwapChk:	;Set things up for desired mode.
	;
	test	DebugMode+1,128
	jnz	@@NormalINT
	cmp	DebugRetCode,RetCode_INT
	jz	@@Next		;force next.
@@NormalINT:	cmp	DebugMode,4
	jz	@@Trace
	cmp	DebugMode,0		;trace?
	jz	@@Trace
	cmp	DebugMode,1		;Next?
	jz	@@Next
	jmp	@@Go
	;
@@Next:	;Setup for [N]ext.
	;
	cmp	DebugRetCode,RetCode_JMP
	jz	@@Trace		;Force trace.
	;
	cmp	DebugRetCode,RetCode_JMPea
	jc	@@Next_1
	cmp	DebugRetCode,RetCode_JMPFea32+1
	jc	@@Trace		;Force trace.
	;
@@Next_1:	cmp	DebugRetCode,RetCode_SS
	jz	@@Trace		;Force trace.
	cmp	DebugRetCode,RetCode_RET
	jc	@@Next_0
	cmp	DebugRetCode,RetCode_IRET32+1
	jc	@@Trace		;Force trace.
@@Next_0:	;
	;Seems we really do mean [N]ext.
	;
	cmp	DebugMode,4
	jz	@@Next_00
	mov	DebugMode,1
@@Next_00:	mov	ax,NextCS
	mov	TargetCS,ax
	mov	eax,NextEIP
	mov	TargetEIP,eax
	jmp	@@Go
	;
@@Trace:	;Setup for [T]race.
	;
	cmp	DebugRetCode,RetCode_INT
	jz	@@Trace_INT
	cmp	DebugRetCode,RetCode_JMP	;JMP's need to use target.
	jz	@@Trace_0
	cmp	DebugRetCode,RetCode_Jcc	;Jcc's need to use target.
	jz	@@Trace_0
	cmp	DebugRetCode,RetCode_CALL	;CALL's need to use target.
	jz	@@Trace_0
	;
	cmp	DebugRetCode,RetCode_SS
	jnz	@@Trace_1
	mov	fs,NextCS
	mov	esi,NextEIP
	mov	edi,offset ABuffer
	mov	b[edi],0
	call	Disasemble
	mov	DebugRetCode,ax
	mov	NextCS,fs
	mov	NextEIP,esi
	mov	TargetCS,cx
	mov	TargetEIP,edx
	jmp	@@Trace
	;
@@Trace_1:	;Look for JMP/CALL w|d|f[EA]
	;
	cmp	DebugRetCode,RetCode_JMPea
	jc	@@Trace_3
	cmp	DebugRetCode,RetCode_CALLFea32+1
	jnc	@@Trace_3
	;
	;Have to retrieve target address from [EA]
	;
	test	TargetMode,128
	jnz	@@Trace_j100
	mov	ax,DebugCS
	mov	TargetCS,ax
	jmp	@@Trace_2
	;
@@Trace_j100:	cmp	DebugRetCode,RetCode_JMPea
	jnz	@@Trace_j0
@@Trace_c0:	push	es
	mov	es,TargetCS
	mov	ebx,TargetEIP
	movzx	ebx,es:w[ebx]
	mov	TargetEIP,ebx
	mov	NextEIP,ebx
	mov	ax,NextCS
	mov	TargetCS,ax
	pop	es
	jmp	@@Trace_2
	;
@@Trace_j0:	cmp	DebugRetCode,RetCode_JMPea32
	jnz	@@Trace_j1
@@Trace_c1:	push	es
	mov	es,TargetCS
	mov	ebx,TargetEIP
	mov	ebx,es:d[ebx]
	mov	TargetEIP,ebx
	mov	NextEIP,ebx
	mov	ax,NextCS
	mov	TargetCS,ax
	pop	es
	jmp	@@Trace_2
	;
@@Trace_j1:	cmp	DebugRetCode,RetCode_JMPFea
	jnz	@@Trace_j2
@@Trace_c2:	push	es
	mov	es,TargetCS
	mov	ebx,TargetEIP
	mov	ax,es:w[ebx+2]
	movzx	ebx,es:w[ebx]
	mov	TargetCS,ax
	mov	NextCS,ax
	mov	TargetEIP,ebx
	mov	NextEIP,ebx
	pop	es
	jmp	@@Trace_2
	;
@@Trace_j2:	cmp	DebugRetCode,RetCode_JMPFea32
	jnz	@@Trace_j3
@@Trace_c3:	push	es
	mov	es,TargetCS
	mov	ebx,TargetEIP
	mov	ax,es:w[ebx+4]
	mov	ebx,es:d[ebx]
	mov	TargetCS,ax
	mov	NextCS,ax
	mov	TargetEIP,ebx
	mov	NextEIP,ebx
	pop	es
	jmp	@@Trace_2
	;
@@Trace_j3:	cmp	DebugRetCode,RetCode_CALLea
	jz	@@Trace_c0
	cmp	DebugRetCode,RetCode_CALLea32
	jz	@@Trace_c1
	cmp	DebugRetCode,RetCode_CALLFea
	jz	@@Trace_c2
	cmp	DebugRetCode,RetCode_CALLFea32
	jz	@@Trace_c3
	;
@@Trace_3:	;Set target addr to next.
	;
	mov	ax,NextCS
	mov	TargetCS,ax
	mov	eax,NextEIP
	mov	TargetEIP,eax
@@Trace_2:	;
	cmp	DebugRetCode,RetCode_RET
	jc	@@Trace_0
	cmp	DebugRetCode,RetCode_IRET32+1
	jnc	@@Trace_0		;Force trace.
	;
	;Need to retrieve target address from stack.
	;
	cmp	DebugRetCode,RetCode_RET
	jnz	@@Trace_r0
@@Trace_r2:	push	es
	mov	es,DebugSS
	mov	ebx,DebugESP
	test	SystemFlags,1
	jz	@@TR320
	movzx	ebx,bx
@@TR320:	movzx	eax,es:w[ebx]
	mov	TargetEIP,eax
	pop	es
	jmp	@@Trace_0
	;
@@Trace_r0:	cmp	DebugRetCode,RetCode_RET32
	jnz	@@Trace_r1
@@Trace_r3:	push	es
	mov	es,DebugSS
	mov	ebx,DebugESP
	test	SystemFlags,1
	jz	@@TR321
	movzx	ebx,bx
@@TR321:	mov	eax,es:d[ebx]
	mov	TargetEIP,eax
	pop	es
	jmp	@@Trace_0
	;
@@Trace_r1:	cmp	DebugRetCode,RetCode_RETnn
	jz	@@Trace_r2
	cmp	DebugRetCode,RetCode_RETnn32
	jz	@@Trace_r3
	;
	cmp	DebugRetCode,RetCode_RETF
	jnz	@@Trace_r4
@@Trace_r6:	push	es
	mov	es,DebugSS
	mov	ebx,DebugESP
	test	SystemFlags,1
	jz	@@TR322
	movzx	ebx,bx
@@TR322:	movzx	eax,es:w[ebx]
	mov	TargetEIP,eax
	mov	ax,es:[ebx+2]
	mov	TargetCS,ax
	pop	es
	jmp	@@Trace_0
	;
@@Trace_r4:	cmp	DebugRetCode,RetCode_RETF32
	jnz	@@Trace_r5
@@Trace_r7:	push	es
	mov	es,DebugSS
	mov	ebx,DebugESP
	test	SystemFlags,1
	jz	@@TR323
	movzx	ebx,bx
@@TR323:	mov	eax,es:[ebx]
	mov	TargetEIP,eax
	mov	ax,es:[ebx+4]
	mov	TargetCS,ax
	pop	es
	jmp	@@Trace_0
	;
@@Trace_r5:	cmp	DebugRetCode,RetCode_RETFnn
	jz	@@Trace_r6
	cmp	DebugRetCode,RetCode_RETFnn32
	jz	@@Trace_r7
	cmp	DebugRetCode,RetCode_IRET
	jz	@@Trace_r6
	cmp	DebugRetCode,RetCode_IRET32
	jz	@@Trace_r7
	jmp	@@Trace_0
	;
@@Trace_INT:	test	DebugMode+1,128
	jz	@@Next_0
	push	es
	mov	ebx,DebugEIP
	mov	es,DebugCS
	test	SystemFlags,1
	jz	@@Trace_INT0
	movzx	ebx,bx
@@Trace_INT0:	mov	al,3
	cmp	es:b[ebx],0ceh	;Check for INTO
	jz	@@Trace_INT4
	cmp	es:b[ebx],0cch	;Check for INT 3
	jz	@@Trace_INT4
	inc	ebx
	mov	al,es:[ebx]		;Get int number.
@@Trace_INT4:	pop	es
	mov	bl,al
	cmp	bl,3
	jz	@@Trace_INT3
	jmp	@@Trace_INT2
@@Trace_INT3:	mov	VidSwapMode,0
	jmp	@@Next_0
@@Trace_INT2:	sys	GetVect
	test	SystemFlags,1
	jz	@@Trace_INT1
	movzx	edx,dx
@@Trace_INT1:	mov	TargetEIP,edx
	mov	TargetCS,cx
	mov	NextEIP,edx
	mov	NextCS,cx
	;
@@Trace_0:	cmp	DebugMode,3
	jz	@@Go
	cmp	DebugMode,4
	jz	@@Go
	mov	DebugMode,0
	;
@@Go:	;exec type has been descided/setup so get on with it.
	;
	;
	;Check if we should register a call address.
	;
	cmp	DebugMode,4
	jz	@@FRegCall
	cmp	DebugMode,0
	jnz	@@NoRemCall
@@FRegCall:	cmp	DebugRetCode,RetCode_CALL	;CALL's need to use target.
	jz	@@RegCall
	cmp	DebugRetCode,RetCode_CALLea
	jc	@@NoRegCall
	cmp	DebugRetCode,RetCode_CALLFea32+1
	jnc	@@NoRegCall
@@RegCall:	pushad
	mov	bx,NextCS
	sys	GetSelDet32
	add	edx,NextEIP
	call	AddRetAddress
	popad
	;
@@NoRegCall:	;Check if we should lose a RET.
	;
	cmp	DebugRetCode,RetCode_RET
	jc	@@NoRemCall
	cmp	DebugRetCode,RetCode_RETFnn32+1
	jnc	@@NoRemCall
	call	RemRetAddress
@@NoRemCall:	;
	cmp	DebugMode,3
	jz	@@NoSwitch0
	;
	call	UserScreen
@@NoSwitch0:	;
	xor	eax,eax
	mov	ax,cs
	push	eax
	mov	eax,offset @@3	;store return address for int 3.
	push	eax
	mov	w[DebuggerSS],ss
	mov	d[DebuggerESP],esp
	mov	w[ExecBreakHandle+4],-1
	mov	bx,DebugCS		;Need segment linear base address
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	edx,ecx
	add	edx,DebugEIP		;real linear address.
	mov	al,BreakType_exec
	call	IsBreakPoint
	jc	@@NoBreakAtCSEIP
	mov	ax,bx
	call	PointBreakPoint
	inc	BreakCountDown[esi]	;force an extra cycle.
	mov	w[ExecBreakHandle+4],ax
@@NoBreakAtCSEIP:
	;Check for break point or trace.
	;
	mov	w[ExecBreakHandle],-1
	mov	w[ExecBreakHandle+2],-1
	cmp	DebugMode,2		;[G]o?
	jz	@@DoneBreak
	;
@@GetBreak:	;Setup a brake point at next execution address.
	;
	mov	bx,NextCS		;Need segment linear base address
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	edx,ecx
	add	edx,NextEIP		;real linear address.
	mov	al,BreakType_exec
	call	SetBreakPointB
	mov	w[ExecBreakHandle],ax
	;
	mov	bx,TargetCS		;Need segment linear base address
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	edx,ecx
	add	edx,TargetEIP	;real linear address.
	mov	al,BreakType_exec
	call	SetBreakPointB
	mov	w[ExecBreakHandle+2],ax
	;
@@DoneBreak:	;Go to it.
	;
	;
	;Switch CW context.
	;
	mov	bx,DebugPSP
	mov	ah,50h
	int	21h
	call	SetBreakPoints	;install ALL break points.
;
;Install HBRK's
;
	pushad
	mov	esi,offset HardBreakTable
	mov	ebp,4
@@hbrk0:	mov	HBRK_Flags[esi],0
	cmp	HBRK_Win[esi],0
	jz	@@hbrk1
	mov	ax,0b00h
	mov	ebx,HBRK_Address[esi]
	mov	cx,bx
	shr	ebx,16
	mov	dl,HBRK_Size[esi]
	mov	dh,HBRK_Type[esi]
	int	31h
	jc	@@hbrk1
	mov	HBRK_Handle[esi],bx
	or	HBRK_Flags[esi],-1
@@hbrk1:	add	esi,size HBRK
	dec	ebp
	jnz	@@hbrk0
	popad
	;
	mov	Executing,1
	mov	ExceptionFlag,-1
	;
	lss	esp,f[DebugESP]
	push	d[DebugEFL]
	push	d[DebugCS]
	push	d[DebugEIP]
	mov	eax,DebugEAX
	mov	ebx,DebugEBX
	mov	ecx,DebugECX
	mov	edx,DebugEDX
	mov	esi,DebugESI
	mov	edi,DebugEDI
	mov	ebp,DebugEBP
	mov	gs,DebugGS
	mov	fs,DebugFS
	mov	es,DebugES
	mov	ds,DebugDS
	iretd
	;
@@3:	mov	Executing,0
;
;Remove HBRK's
;
	pushad
	mov	esi,offset HardBreakTable
	mov	ebp,4
@@hbrk2:	cmp	HBRK_Flags[esi],0
	jz	@@hbrk3
	mov	bx,HBRK_Handle[esi]
	mov	ax,0b01h
	int	31h
	mov	HBRK_Flags[esi],0
@@hbrk3:	add	esi,size HBRK
	dec	ebp
	jnz	@@hbrk2
	popad
	call	RelBreakPoints	;release ALL break points.
	;
	mov	ah,62h
	int	21h
	mov	DebugPSP,bx
	;
	mov	bx,PSPSegment
	mov	ah,50h
	int	21h
	;
	;Check if the break point that brought us back here has had
	;enough iterations.
	;
	mov	bx,DebugCS		;Need segment linear base address
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	edx,ecx
	add	edx,DebugEIP		;real linear address.
	mov	al,BreakType_exec
	call	IsBreakPointB	;get last one.
	jc	@@NoBreakHere	;must be an exception.
	mov	ax,bx
	call	PointBreakPoint
	cmp	ax,w[ExecBreakHandle+4]	;this the one updated?
	jz	@@bok
	cmp	w[ExecBreakHandle+4],-1	;did we update one?
	jz	@@bok
	pushad
	mov	ax,w[ExecBreakHandle+4]
	call	PointBreakPoint
	dec	BreakCountDown[esi]	;update counter.
	popad
@@bok:	cmp	BreakCountDown[esi],0
	jz	@@BreakDone
	dec	BreakCountDown[esi]	;update counter.
	;
	;Need to hide this break point for 1 instruction and then put it
	;back and continue as before.
	;
	call	HideBreakPoint
	mov	al,3		;do a trace.
	call	ExecuteInst
	xor	eax,eax
	mov	ax,cs
	push	eax
	mov	eax,offset @@3	;store return address for int 3.
	push	eax
	mov	w[DebuggerSS],ss
	mov	d[DebuggerESP],esp
	jmp	@@DoneBreak		;go back to normal.
	;
@@BreakDone:	mov	eax,BreakCount[esi]
	mov	BreakCountDown[esi],eax	;reset counter.
@@NoBreakHere:	;
	mov	ax,w[ExecBreakHandle]
	cmp	ax,-1		;used break point?
	jz	@@4
	call	RelBreakPoint	;release it then.
	;
@@4:	mov	ax,w[ExecBreakHandle+2]
	cmp	ax,-1
	jz	@@6
	call	RelBreakPoint	;release it then.
@@6:	cmp	DebugMode,3
	jz	@@GotSource
;
;Check if we're doing source stuff.
;
	cmp	DontUseSource,0
	jnz	@@GotSource
	cmp	LINEList,0
	jz	@@GotSource
;
;Don't bother with source stuff if an exception or termination occured.
;
	cmp	ExceptionFlag,-1
	jnz	@@GotSource
	cmp	TerminationFlag,0
	jnz	@@GotSource
;
;Check if the new CS:EIP matches a source line.
;
	mov	fs,DebugCS
	mov	esi,DebugEIP
	call	FindSourceFSESI
	jnc	@@GotSource
;
;See if new CS:EIP is within range of a source entry.
;
	cmp	CMode,0
	jnz	@@ForceSource
	call	RangeSourceFSESI
	jc	@@GotSource
;
;CS:EIP is in range of a source entry so we send it back to keep executing
;instructions till we get somewhere useful.
;
@@ForceSource:	mov	DebugMode,4
	mov	DebugMode+1,0
	jmp	@@ExecAgain
@@GotSource:	pop	w[DebugMode]
;
;Retrieve user context.
;
	cmp	DebugMode,3
	jz	@@NoSwitch1
	call	DisasScreen
@@NoSwitch1:

	mov	al,20h	; MED 08/06/96, re-enable interrupts
	out	20h,al

;
;Check if an exception was generated.
;
	cmp	ExceptionFlag,-1
	jz	@@5
	movzx	eax,ExceptionFlag
	mov	edi,offset ExceptionNumt
	mov	ecx,2
	call	Bin2Hex
	mov	ebx,offset ExceptionText
	call	WindowPopup
	mov	ExceptionFlag,-1
	;
@@5:	;Check if it terminated.
	;
	cmp	TerminationFlag,0
	jz	@@7
	mov	al,TerminateCode
	mov	edi,offset TerminateNum
	mov	ecx,2
	call	Bin2Hex
	mov	ebx,offset TerminateText
	call	WindowPopup
@@7:	mov	SourceForceCheck,1
	popm	DebuggerSS,DebuggerESP
	popm	w[ExecBreakHandle],w[ExecBreakHandle+2],w[DebugMode],w[VidSwapMode]
	ret
ExecuteInst	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
CopyDebugRegs	proc	near
	push	eax
	mov	eax,DebugEAX
	mov	OldDebugEAX,eax
	mov	eax,DebugEBX
	mov	OldDebugEBX,eax
	mov	eax,DebugECX
	mov	OldDebugECX,eax
	mov	eax,DebugEDX
	mov	OldDebugEDX,eax
	mov	eax,DebugESI
	mov	OldDebugESI,eax
	mov	eax,DebugEDI
	mov	OldDebugEDI,eax
	mov	eax,DebugEBP
	mov	OldDebugEBP,eax
	mov	eax,DebugESP
	mov	OldDebugESP,eax
	mov	eax,DebugEFL
	mov	OldDebugEFL,eax
	mov	eax,DebugEIP
	mov	OldDebugEIP,eax
	mov	ax,DebugCS
	mov	OldDebugCS,ax
	mov	ax,DebugDS
	mov	OldDebugDS,ax
	mov	ax,DebugES
	mov	OldDebugES,ax
	mov	ax,DebugFS
	mov	OldDebugFS,ax
	mov	ax,DebugGS
	mov	OldDebugGS,ax
	mov	ax,DebugSS
	mov	OldDebugSS,ax
	pop	eax
	ret
CopyDebugRegs	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RegisterDisplay proc near
;
;display registers.
;
	call	RegisterOFF
	xor	cx,cx
	mov	bp,RegsHandle
	call	LocateWindow
	;
	mov	eax,DebugEAX
	mov	ecx,8
	mov	edi,offset DebugEAXt
	mov	b[edi-5],30h
	cmp	eax,OldDebugEAX
	jz	@@50
	mov	b[edi-5],3fh
@@50:	call	Bin2Hex
	mov	eax,DebugEBX
	mov	ecx,8
	mov	edi,offset DebugEBXt
	mov	b[edi-5],30h
	cmp	eax,OldDebugEBX
	jz	@@51
	mov	b[edi-5],3fh
@@51:	call	Bin2Hex
	mov	eax,DebugECX
	mov	ecx,8
	mov	edi,offset DebugECXt
	mov	b[edi-5],30h
	cmp	eax,OldDebugECX
	jz	@@52
	mov	b[edi-5],3fh
@@52:	call	Bin2Hex
	mov	eax,DebugEDX
	mov	ecx,8
	mov	edi,offset DebugEDXt
	mov	b[edi-5],30h
	cmp	eax,OldDebugEDX
	jz	@@53
	mov	b[edi-5],3fh
@@53:	call	Bin2Hex
	mov	eax,DebugESI
	mov	ecx,8
	mov	edi,offset DebugESIt
	mov	b[edi-5],30h
	cmp	eax,OldDebugESI
	jz	@@54
	mov	b[edi-5],3fh
@@54:	call	Bin2Hex
	mov	eax,DebugEDI
	mov	ecx,8
	mov	edi,offset DebugEDIt
	mov	b[edi-5],30h
	cmp	eax,OldDebugEDI
	jz	@@55
	mov	b[edi-5],3fh
@@55:	call	Bin2Hex
	mov	eax,DebugEBP
	mov	ecx,8
	mov	edi,offset DebugEBPt
	mov	b[edi-5],30h
	cmp	eax,OldDebugEBP
	jz	@@56
	mov	b[edi-5],3fh
@@56:	call	Bin2Hex
	mov	eax,DebugESP
	mov	ecx,8
	mov	edi,offset DebugESPt
	mov	b[edi-5],30h
	cmp	eax,OldDebugESP
	jz	@@57
	mov	b[edi-5],3fh
@@57:	call	Bin2Hex
	mov	eax,DebugEIP
	mov	ecx,8
	mov	edi,offset DebugEIPt
	mov	b[edi-5],30h
	cmp	eax,OldDebugEIP
	jz	@@58
	mov	b[edi-5],3fh
@@58:	call	Bin2Hex
	mov	eax,DebugEFL
	mov	ecx,8
	mov	edi,offset DebugEFLt
	mov	b[edi-5],30h
	cmp	eax,OldDebugEFL
	jz	@@59
	mov	b[edi-5],3fh
@@59:	call	Bin2Hex
	;
	mov	ax,DebugCS
	mov	edi,offset DebugCSt
	mov	b[edi-5],30h
	cmp	ax,OldDebugCS
	jz	@@60
	mov	b[edi-5],3fh
@@60:	call	DebugSegment
	mov	ecx,8
	cmp	RegsSelTranslate,0
	jz	@@0_0
	cmp	eax,-1
	jnz	@@0
@@0_0:	mov	ax,DebugCS
	mov	d[edi],'xxxx'
	add	edi,4
	mov	ecx,4
@@0:	call	Bin2Hex
	;
	mov	ax,DebugDS
	mov	edi,offset DebugDSt
	mov	b[edi-5],30h
	cmp	ax,OldDebugDS
	jz	@@61
	mov	b[edi-5],3fh
@@61:	call	DebugSegment
	mov	ecx,8
	cmp	RegsSelTranslate,0
	jz	@@1_0
	cmp	eax,-1
	jnz	@@1
@@1_0:	mov	ax,DebugDS
	mov	d[edi],'xxxx'
	add	edi,4
	mov	ecx,4
@@1:	call	Bin2Hex

	mov	ax,DebugES
	mov	edi,offset DebugESt
	mov	b[edi-5],30h
	cmp	ax,OldDebugES
	jz	@@62
	mov	b[edi-5],3fh
@@62:	call	DebugSegment
	mov	ecx,8
	cmp	RegsSelTranslate,0
	jz	@@2_0
	cmp	eax,-1
	jnz	@@2
@@2_0:	mov	ax,DebugES
	mov	d[edi],'xxxx'
	add	edi,4
	mov	ecx,4
@@2:	call	Bin2Hex

	mov	ax,DebugFS
	mov	edi,offset DebugFSt
	mov	b[edi-5],30h
	cmp	ax,OldDebugFS
	jz	@@63
	mov	b[edi-5],3fh
@@63:	call	DebugSegment
	mov	ecx,8
	cmp	RegsSelTranslate,0
	jz	@@3_0
	cmp	eax,-1
	jnz	@@3
@@3_0:	mov	ax,DebugFS
	mov	d[edi],'xxxx'
	add	edi,4
	mov	ecx,4
@@3:	call	Bin2Hex

	mov	ax,DebugGS
	mov	edi,offset DebugGSt
	mov	b[edi-5],30h
	cmp	ax,OldDebugGS
	jz	@@64
	mov	b[edi-5],3fh
@@64:	call	DebugSegment
	mov	ecx,8
	cmp	RegsSelTranslate,0
	jz	@@4_0
	cmp	eax,-1
	jnz	@@4
@@4_0:	mov	ax,DebugGS
	mov	d[edi],'xxxx'
	add	edi,4
	mov	ecx,4
@@4:	call	Bin2Hex

	mov	ax,DebugSS
	mov	edi,offset DebugSSt
	mov	b[edi-5],30h
	cmp	ax,OldDebugSS
	jz	@@65
	mov	b[edi-5],3fh
@@65:	call	DebugSegment
	mov	ecx,8
	cmp	RegsSelTranslate,0
	jz	@@5_0
	cmp	eax,-1
	jnz	@@5
@@5_0:	mov	ax,DebugSS
	mov	d[edi],'xxxx'
	add	edi,4
	mov	ecx,4
@@5:	call	Bin2Hex
	;
	mov	edi,offset DebugEFLtl
@@6:	cmp	b[edi],0
	jz	@@7
	mov	b[edi],' '
	inc	edi
	jmp	@@6
	;
@@7:	mov	edi,offset DebugEFLtl+1
	mov	eax,DebugEFL
	test	eax,1
	jz	@@8
	mov	b[edi],'C'
@@8:	add	edi,1
	test	eax,4
	jz	@@9
	mov	b[edi],'P'
@@9:	add	edi,1
	test	eax,16
	jz	@@10
	mov	b[edi],'A'
@@10:	add	edi,1
	test	eax,64
	jz	@@11
	mov	b[edi],'Z'
@@11:	add	edi,1
	test	eax,128
	jz	@@12
	mov	b[edi],'S'
@@12:	add	edi,1
	test	eax,1024
	jz	@@13
	mov	b[edi],'D'
@@13:	add	edi,1
	test	eax,2048
	jz	@@14
	mov	b[edi],'O'
@@14:	add	edi,1
	test	eax,1 shl 9
	jz	@@15
	mov	b[edi],"I"
@@15:	add	edi,1
	;
	;Do current EA stuff.
	;
	mov	edi,offset DebugEAt
	mov	ecx,14
	mov	al,' '
	rep	stosb
	;
	mov	fs,DebugCS
	mov	esi,DebugEIP
	mov	edi,offset ABuffer
	mov	b[edi],0
	call	Disasemble
	test	bl,128
	jz	@@ea9
	and	bl,127
	test	bl,bl
	jz	@@ea9

	test	cx,cx		;Check selector value.
	jz	@@ea9

	pushm	bx,edx
	mov	bx,cx		;Need segment linear base address
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	esi,ecx
	popm	bx,edx
	add	esi,edx
	mov	edx,esi
	mov	ax,0fffch
	int	31h
	jc	@@ea9
	add	esi,5
	mov	ax,0fffch
	int	31h
	jc	@@ea9
	;
	mov	edi,offset DebugEAt
	push	es
	mov	es,RealSegment
	;
	cmp	bl,1
	jz	@@ea1
	cmp	bl,2
	jz	@@ea2
	cmp	bl,4
	jz	@@ea4
	cmp	bl,5
	jz	@@ea5
	cmp	bl,6
	jz	@@ea6
	cmp	bl,8
	jz	@@ea8_8
	cmp	bl,10
	jz	@@ea10
	jmp	@@ea9
	;
@@ea1:	add	edi,5
	mov	al,es:[edx]
	mov	ecx,2
	jmp	@@ea8
@@ea2:	add	edi,4
	mov	ax,es:[edx]
	mov	ecx,4
	jmp	@@ea8
@@ea4:	add	edi,2
	mov	eax,es:[edx]
	mov	ecx,8
	jmp	@@ea8
@@ea5:	mov	ax,es:[edx+2]
	mov	cx,es:[edx]
	pop	es
	push	cx
	mov	ecx,4
	call	Bin2Hex
	mov	b[edi],':'
	inc	edi
	pop	ax
	mov	ecx,4
	jmp	@@ea7
@@ea6:	mov	ecx,es:[edx]
	mov	ax,es:[edx+4]
	pop	es
	push	ecx
	mov	ecx,4
	call	Bin2Hex
	mov	b[edi],':'
	inc	edi
	pop	eax
	mov	ecx,8
	jmp	@@ea7
@@ea8_8:	mov	eax,es:[edx+4]
	mov	ecx,8
	mov	bx,es
	pop	es
	call	Bin2Hex
	push	es
	mov	es,bx
	mov	eax,es:[edx]
	mov	ecx,8
	jmp	@@ea8
@@ea10:	mov	eax,es:[edx+6]
	mov	ecx,8
	mov	bx,es
	pop	es
	call	Bin2Hex
	push	es
	mov	es,bx
	mov	eax,es:[edx+2]
	mov	ecx,8
	mov	bx,es
	pop	es
	call	Bin2Hex
	push	es
	mov	es,bx
	mov	ax,es:[edx]
	mov	ecx,4
@@ea8:	pop	es
@@ea7:	call	Bin2Hex

@@ea9:
	push	ds
	pop	es

	call	GetFPUValues

;Now print the results.
	mov	edi,offset DebugList
	mov	al,RegsFPUDisplay
	test	al,IsFPUFlag
	je	@@p0		; no FPU or no FPU display
	mov	edi,OFFSET DebugListFPU

@@p0:
	cmp	d[edi],-1
	jz	@@p1
	mov	ebx,[edi]
	pushm	edi
	mov	bp,RegsHandle
	call	PrintWindow
	popm	edi
	add	edi,4
	jmp	@@p0
	;
@@p1:	call	RegisterON
	ret
RegisterDisplay endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-=-=-=-=-=-=-=-=-=-=
;
;Convert segment value into real & application relative.
;
;On Entry:-
;
;AX	- Selector.
;EDI	- Buffer address.
;
DebugSegment	proc	near
	pushm	edi,fs
	mov	fs,DebugPSP
	mov	ebx,-1
	mov	dx,ax
	cmp	ax,fs:w[EPSP_SegBase]	;inside application selector space?
	jc	@@9
	mov	cx,fs:w[EPSP_SegSize]
	shl	cx,3		;8 bytes per selector.
	add	cx,fs:w[EPSP_SegBase]	;add in base selector.
	cmp	ax,cx
	jnc	@@9		;outside application startup selectors.
	mov	bx,dx
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	mov	eax,ecx
	sub	eax,fs:d[EPSP_MemBase]	;get offset within application.
	mov	ebx,eax
@@9:	mov	eax,ebx
	popm	edi,fs
	ret
DebugSegment	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DisasScreen	proc	near
;
;Switch to disasemblers screen context.
;
	cmp	VidSwapMode,0
	jnz	@@None
	cmp	WhichScreen,-1
	jz	@@None
	mov	WhichScreen,-1
	cmp	NoContextSwitch,0
	jnz	@@NoSwitch0
	cmp	MousePresent,0
	jz	@@NoMSaveRes
	;
	mov	edx,MouseUserState
	mov	ax,16h
	int	33h
	mov	edx,MouseDebugState
	mov	ax,17h
	int	33h
@@NoMSaveRes:	;
	mov	bl,09h
	sys	GetVect
	mov	d[UserInt09h],edx
	mov	w[UserInt09h+4],cx
	;
	cmp	w[DisasInt09h+4],0
	jz	@@NoInt09Rest
	mov	edx,d[DisasInt09h]
	mov	cx,w[DisasInt09h+4]
	mov	bl,09h
	sys	SetVect
@@NoInt09Rest:	;
@@NoSwitch0:	cmp	MonoSwap,0
	jz	@@NoMono
	mov	ebx,VideoUserState
	mov	ax,1c01h
	mov	cx,1+2+4
	int	10h		;save state in buffer.
	mov	ebx,VideoUserState
	mov	ax,1c02h
	mov	cx,1+2+4
	int	10h		;restore it again.
	mov	ah,0fh
	int	10h		;get current page.
	mov	UserPage,bh
	mov	UserMode,al
	push	es
	mov	ax,40h
	mov	es,ax
	and	es:b[10h],11001111b
	or	es:b[10h],00110000b
	pop	es
	cmp	FirstMono,0
	jnz	@@noforce
	mov	FirstMono,1
	;
	;Get current video memory contents.
	;
	mov	edx,0b0000h
	mov	ecx,80*25*2
	mov	esi,VideoUserBuffer
	mov	[esi],edx
	mov	[esi+4],ecx
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	cld
	mov	edi,VideoUserBuffer
	mov	ecx,[edi+4]
	add	edi,4+4
	xor	esi,esi
	push	ds
	mov	ds,VideoSwapSel
	rep	movsb
	pop	ds
	mov	ax,0087h
	int	10h
@@noforce:	;
	mov	DX,03BFH		;Configuration switch port
	MOV	AL,0	;2
	OUT	DX,AL		;Allow graphics mode setting
	mov	dx,03b8h
	mov	al,1+8	;ah
	out	dx,al
	;
	;Get current video memory contents.
	;
	cmp	FirstMono,1
	jnz	@@nofgrab
	inc	FirstMono
	jmp	@@fgrab
@@nofgrab:	mov	edx,0b0000h
	mov	ecx,80*25*2
	mov	esi,VideoUserBuffer
	mov	[esi],edx
	mov	[esi+4],ecx
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	cld
	mov	edi,VideoUserBuffer
	mov	ecx,[edi+4]
	add	edi,4+4
	xor	esi,esi
	push	ds
	mov	ds,VideoSwapSel
	rep	movsb
	pop	ds
@@fgrab:	;
	;Restore debug memory.
	;
	mov	esi,VideoDebugBuffer
	mov	edx,[esi]
	mov	ecx,[esi+4]
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	mov	esi,VideoDebugBuffer
	mov	ecx,[esi+4]
	add	esi,4+4
	xor	edi,edi
	push	es
	mov	es,VideoSwapSel
	rep	movsb
	pop	es
	cmp	NoContextSwitch,0
	jnz	@@NoSwitch1
	call	MouseON
@@NoSwitch1:	ret
@@NoMono:	;
@@0:	;Get current video state.
	;
	mov	ebx,VideoUserState
	mov	ax,1c01h
	mov	cx,1+2+4
	int	10h		;save state in buffer.
	mov	ebx,VideoUserState
	mov	ax,1c02h
	mov	cx,1+2+4
	int	10h		;restore it again.
	;
@@2:	;Get current video memory contents.
	;
	mov	ah,0fh
	int	10h		;get current mode.
	mov	UserMode,al
	mov	edx,0b8000h
	mov	ecx,16384
	cmp	al,3
	jz	@@1
	cmp	al,4
	jz	@@1
	mov	edx,0b0000h
	cmp	al,7	;80*25*16 T
	jc	@@1
	jz	@@1
	mov	edx,0a0000h
	mov	ecx,65535
	cmp	al,13h	;320*200*256 G
	jz	@@1
	mov	edx,0b8000h
	mov	ecx,16384
@@1:	mov	esi,VideoUserBuffer
	mov	[esi],edx
	mov	[esi+4],ecx
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	cld
	mov	edi,VideoUserBuffer
	mov	ecx,[edi+4]
	add	edi,4+4
	xor	esi,esi
	push	ds
	mov	ds,VideoSwapSel
	rep	movsb
	pop	ds
	;
	;Get user page.
	;
	mov	ah,0fh
	int	10h		;get current page.
	mov	UserMode,al
	mov	UserPage,bh
	;
	;Get 50-line state.
	;
	mov	User50,0
	mov	ax,1130h
	xor	bx,bx
	xor	dx,dx
	push	es
	int	10h
	pop	es
	cmp	dl,49
	jnz	@@U_Not50
	or	User50,-1
@@U_Not50:	;
	;Check flip type needed.
	;
	cmp	FlipSwap,0
	jz	@@4
	;
	;Force disas mode.
	;
	mov	al,DisasMode
	or	al,80h
	mov	ah,0
	int	10h
	;
	;Force disas font.
	;
	cmp	Disas50,0
	jz	@@D_Not50
	mov	ax,1112h
	xor	bx,bx
	int	10h
@@D_Not50:	;
@@4:	;Restore debug state.
	;
	mov	ebx,VideoDebugState
	mov	ax,1c02h
	mov	cx,1+2+4
	int	10h
	;
@@3:	;Restore debug memory.
	;
	mov	esi,VideoDebugBuffer
	mov	edx,[esi]
	mov	ecx,[esi+4]
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	mov	esi,VideoDebugBuffer
	mov	ecx,[esi+4]
	add	esi,4+4
	xor	edi,edi
	push	es
	mov	es,VideoSwapSel
	rep	movsb
	pop	es
	;
	;Force disas page.
	;
	mov	ah,5
	mov	al,DisasPage
	int	10h
	cmp	NoContextSwitch,0
	jnz	@@NoSwitch2
	call	MouseON
@@NoSwitch2:	;
@@None:	ret
DisasScreen	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
UserScreen	proc	near
;
;Switch to users screen context.
;
	cmp	VidSwapMode,0
	jnz	@@None
	cmp	WhichScreen,0
	jz	@@None
	mov	WhichScreen,0
	cmp	NoContextSwitch,0
	jnz	@@NoSwitch0
	call	MouseOFF
	cmp	MousePresent,0
	jz	@@NoMSaveRes
	;
	mov	edx,MouseDebugState
	mov	ax,16h
	int	33h
	mov	edx,MouseUserState
	mov	ax,17h
	int	33h
@@NoMSaveRes:	;
	mov	bl,09h
	sys	GetVect
	mov	d[DisasInt09h],edx
	mov	w[DisasInt09h+4],cx
	;
	cmp	w[UserInt09h+4],0
	jz	@@NoInt09Rest
	mov	edx,d[UserInt09h]
	mov	cx,w[UserInt09h+4]
	mov	bl,09h
	sys	SetVect
@@NoInt09Rest:	;
@@NoSwitch0:	cmp	MonoSwap,0
	jz	@@NoMono
	;
	;Get current video memory contents.
	;
	mov	edx,0b0000h
	mov	ecx,25*80*2
	mov	esi,VideoDebugBuffer
	mov	[esi],edx
	mov	[esi+4],ecx
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	mov	edi,VideoDebugBuffer
	mov	ecx,[esi+4]
	add	edi,4+4
	xor	esi,esi
	push	ds
	mov	ds,VideoSwapSel
	rep_movsb
	pop	ds
	;
	;Restore video memory.
	;
	mov	esi,VideoUserBuffer
	mov	edx,[esi]
	mov	ecx,[esi+4]
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	add	esi,4+4
	xor	edi,edi
	push	es
	mov	es,VideoSwapSel
	rep	movsb
	pop	es
	push	es
	mov	ax,40h
	mov	es,ax
	and	es:b[10h],11001111b
	or	es:b[10h],00100000b
	pop	es
	;
	;Restore video state.
	;
	mov	ebx,VideoUserState
	mov	ax,1c02h
	mov	cx,1+2+4
	int	10h
	ret
@@NoMono:	;
@@0:	;Get current video state.
	;
	mov	ebx,VideoDebugState
	mov	ax,1c01h
	mov	cx,1+2+4
	int	10h		;save state in buffer.
	;
	mov	ebx,VideoDebugState
	mov	ax,1c02h
	mov	cx,1+2+4
	int	10h		;restore it again.
	;
@@2:	;Get current video memory contents.
	;
	mov	ah,0fh
	int	10h		;get current mode.
	mov	edx,0b8000h
	mov	ecx,16384
	cmp	al,3
	jz	@@1
	cmp	al,4
	jz	@@1
	mov	edx,0b0000h
	cmp	al,7	;80*25*16 T
	jc	@@1
	jz	@@1
	mov	edx,0a0000h
	mov	ecx,65535
	cmp	al,13h	;320*200*256 G
	jz	@@1
	mov	edx,0b8000h
	mov	ecx,16384
@@1:	mov	esi,VideoDebugBuffer
	mov	[esi],edx
	mov	[esi+4],ecx
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	mov	edi,VideoDebugBuffer
	mov	ecx,[esi+4]
	add	edi,4+4
	xor	esi,esi
	push	ds
	mov	ds,VideoSwapSel
	rep_movsb
	pop	ds
	;
	;Get user page.
	;
	mov	ah,0fh
	int	10h		;get current page.
	mov	DisasPage,bh
	mov	DisasMode,al
	;
	;Get 50-line state.
	;
	mov	Disas50,0
	mov	ax,1130h
	xor	bx,bx
	xor	dx,dx
	push	es
	int	10h
	pop	es
	cmp	dl,49
	jnz	@@D_Not50
	or	Disas50,-1
@@D_Not50:	;
	;Check flip type.
	;
	cmp	FlipSwap,0
	jz	@@4
	mov	al,UserMode
	or	al,80h
	xor	ah,ah
	int	10h
	;
	;Force disas font.
	;
	cmp	User50,0
	jz	@@U_Not50
	mov	ax,1112h
	xor	bx,bx
	int	10h
@@U_Not50:	;
@@4:	;Restore video state.
	;
	mov	ebx,VideoUserState
	mov	ax,1c02h
	mov	cx,1+2+4
	int	10h
	;
@@3:	;Restore video memory.
	;
	mov	esi,VideoUserBuffer
	mov	edx,[esi]
	mov	ecx,[esi+4]
	mov	bx,VideoSwapSel
	sys	SetSelDet32
	add	esi,4+4
	xor	edi,edi
	push	es
	mov	es,VideoSwapSel
	rep	movsb
	pop	es
	;
	;Force disas page.
	;
	mov	ah,5
	mov	al,UserPage
	int	10h
@@None:	ret
UserScreen	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Bord	proc	near
	pushm	ax,dx
	mov	ah,al
	mov	dx,3dah
	in	al,dx
	mov	dl,0c0h
	mov	al,11h
	out	dx,al
	mov	al,ah
	out	dx,al
	mov	al,20h
	out	dx,al
	popm	ax,dx
	ret
Bord	endp

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
Malloc	proc	near
	pushm	eax,ebx,ecx,edx
l2:	sys	GetMemLinear32
	jc	l0
	mov	bx,ds
	push	ecx
	sys	GetSelDet32
	pop	ecx
	cmp	esi,edx		;below CWD?
	jnc	l3
	cmp	OS2TypeMalloc,0
	jnz	l2
l3:	sub	esi,edx
	clc
	jmp	l1
l0:	xor	esi,esi
	stc
l1:	popm	eax,ebx,ecx,edx
	ret
Malloc	endp

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
ReMalloc	proc	near
	pushm	eax,ebx,ecx,edx
	push	ecx
	mov	bx,ds
	sys	GetSelDet32
	jc	l0
	add	esi,edx
	pop	ecx
	sys	ResMemLinear32
	jc	l0
	mov	bx,ds
	sys	GetSelDet32
	jc	l0
	sub	esi,edx
	clc
	jmp	l1
l0:	xor	esi,esi
	stc
l1:	popm	eax,ebx,ecx,edx
	ret
ReMalloc	endp

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
Free	proc	near
	pushm	eax,ebx,ecx,edx,esi
	mov	bx,ds
	sys	GetSelDet32
	add	esi,edx
	sys	RelMemLinear32
	popm	eax,ebx,ecx,edx,esi
	ret
Free	endp

	include files.asm
	include print.asm
	include getkeys.asm
	include win.asm
	include evaluate.asm
	include disas.asm
	include	fpu.asm

	.data

	db 2048 dup (?)
DataStack	dd ?
	public DataStack

DescriptorBuffer db 8 dup (?)

WindowSplits	db 2048 dup (?)
;
WindowRegionFlag dw 0
WindowRegionX	dw ?
WindowRegionY	dw ?
WindowRegionWidth dw ?
WindowRegionDepth dw ?

MouseX	dw ?
MouseY	dw ?
MouseB	dw ?

VLineBuffer	db 4096 dup (?)

MousePresent	db 0
MouseFlag	db 0
MousePointOff	db 0
MousePointYN	db 0
MousePointNm	db 0
OldPointNm	db 0
MousePointBt	dw 0
MousePointX1	dw 0
MousePointX2	dw -32767
MousePointY1	dw 0
MousePointY2	dw -32767
MouseMickeyX	dw 0
MouseMickeyY	dw 0
MouseScaleX	dw 8
MouseScaleY	dw 8
InMouse	dw 0
;
MouseStore	dw ?
;
DisasBuffer	db 8192 dup (?)
ABuffer	db 1024 dup (?)
BBuffer	db 1024 dup (?)
CBuffer	db 1024 dup (?)
CarriageReturn	db 13,10,0
;
DebugMode	db 0,0
DebugRetCode	dw 0
;
DebugBuffer	label byte
DebugEBP	dd 0
DebugEDI	dd 0
DebugESI	dd 0
DebugEDX	dd 0
DebugECX	dd 0
DebugEBX	dd 0
DebugEAX	dd 0
DebugGS	dw 0
DebugFS	dw 0
DebugES	dw 0
DebugDS	dw 0
DebugEIP	dd ?
DebugCS	dw ?,0
DebugEFL	dd ?
DebugESP	dd ?
DebugSS	dw ?,?
DebugTR	dw ?
;
OldDebugEBP	dd 0
OldDebugEDI	dd 0
OldDebugESI	dd 0
OldDebugEDX	dd 0
OldDebugECX	dd 0
OldDebugEBX	dd 0
OldDebugEAX	dd 0
OldDebugGS	dw 0
OldDebugFS	dw 0
OldDebugES	dw 0
OldDebugDS	dw 0
OldDebugEIP	dd 0
OldDebugCS	dw 0
OldDebugEFL	dd 0
OldDebugESP	dd 0
OldDebugSS	dw 0

Debug2ESP	dd ?
Debug2SS	dw ?,?

NextCS	dw ?
NextEIP	dd ?

TargetCS	dw ?
TargetEIP	dd ?
TargetCS2	dw ?
TargetEIP2	dd ?
TargetMode	dd ?

DebugList	DD	DebugNulltp
	DD	DebugEAXtp
	DD	DebugTrailer
	DD	DebugEBXtp
	DD	DebugTrailer
	DD	DebugECXtp
	DD	DebugTrailer
	DD	DebugEDXtp
	DD	DebugTrailer
	DD	DebugESPtp
	DD	DebugTrailer
	DD	DebugEBPtp
	DD	DebugTrailer
	DD	DebugESItp
	DD	DebugTrailer
	DD	DebugEDItp
	DD	DebugTrailer
	DD	DebugEIPtp
	DD	DebugTrailer
	DD	DebugEFLtp
	DD	DebugTrailer
	DD	DebugDStp
	DD	DebugTrailer
	DD	DebugEStp
	DD	DebugTrailer
	DD	DebugFStp
	DD	DebugTrailer
	DD	DebugGStp
	DD	DebugTrailer
	DD	DebugSStp
	DD	DebugTrailer
	DD	DebugCStp
	DD	DebugTrailer
	DD	DebugNulltp
	DD	DebugEFLtl
	DD	DebugTrailer
	DD	DebugNulltp
	DD	DebugEAt
	DD	DebugTrailer
	DD	-1

DebugDis2RegList dd DebugEAX,DebugEBX,DebugECX,DebugEDX,DebugESP,DebugEBP
	dd DebugESI,DebugEDI,DebugEIP,DebugEFL

DebugNulltp	db 13,10,0
DebugEAXtp	db 1,30h,'EAX='
DebugEAXt	db '00000000',0
DebugEBXtp	db 1,30h,'EBX='
DebugEBXt	db '00000000',0
DebugECXtp	db 1,30h,'ECX='
DebugECXt	db '00000000',0
DebugEDXtp	db 1,30h,'EDX='
DebugEDXt	db '00000000',0
DebugESItp	db 1,30h,'ESI='
DebugESIt	db '00000000',0
DebugEDItp	db 1,30h,'EDI='
DebugEDIt	db '00000000',0
DebugEBPtp	db 1,30h,'EBP='
DebugEBPt	db '00000000',0
DebugESPtp	db 1,30h,'ESP='
DebugESPt	db '00000000',0
DebugEIPtp	db 1,30h,'EIP='
DebugEIPt	db '00000000',0
DebugEFLtp	db 1,30h,'EFL='
DebugEFLt	db '00000000',0
DebugCStp	db 1,30h,' CS='
DebugCSt	db '00000000',0
DebugDStp	db 1,30h,' DS='
DebugDSt	db '00000000',0
DebugEStp	db 1,30h,' ES='
DebugESt	db '00000000',0
DebugFStp	db 1,30h,' FS='
DebugFSt	db '00000000',0
DebugGStp	db 1,30h,' GS='
DebugGSt	db '00000000',0
DebugSStp	db 1,30h,' SS='
DebugSSt	db '00000000',0
;	db 1,30h
DebugEFLtl	db '         ',0
;	db 1,30h
DebugEAt	DB	'              ',0
DebugTrailer	DB	13,10,1,30h,0

DebugListFPU	DD	DebugNulltp
	DD	DebugEAXtp
	DD	DebugST0tp
	DD	DebugTrailer
	DD	DebugEBXtp
	DD	DebugST1tp
	DD	DebugTrailer
	DD	DebugECXtp
	DD	DebugST2tp
	DD	DebugTrailer
	DD	DebugEDXtp
	DD	DebugST3tp
	DD	DebugTrailer
	DD	DebugESPtp
	DD	DebugST4tp
	DD	DebugTrailer
	DD	DebugEBPtp
	DD	DebugST5tp
	DD	DebugTrailer
	DD	DebugESItp
	DD	DebugST6tp
	DD	DebugTrailer
	DD	DebugEDItp
	DD	DebugST7tp
	DD	DebugTrailer
	DD	DebugEIPtp
	DD	DebugTrailer
	DD	DebugEFLtp
	DD	DebugSTATtp
	DD	DebugTrailer
	DD	DebugDStp
	DD	DebugCONTtp
	DD	DebugTrailer
	DD	DebugEStp
	DD	DebugTAGtp
	DD	DebugTrailer
	DD	DebugFStp
	DD	DebugIPTRtp
	DD	DebugTrailer
	DD	DebugGStp
	DD	DebugOPTRtp
	DD	DebugTrailer
	DD	DebugSStp
	DD	DebugTrailer
	DD	DebugCStp
	DD	DebugTrailer
	DD	DebugNulltp
	DD	DebugEFLtl
	DD	DebugTrailer
	DD	DebugNulltp
	DD	DebugEAt
	DD	DebugTrailer
	DD	-1

DebugST0tp	DB	'  ',1,30h,'ST(0)='
DebugST0t	DB	'+0.00000000000000000E+0000',0
DebugST1tp	DB	'  ',1,30h,'ST(1)='
DebugST1t	DB	'+0.00000000000000000E+0000',0
DebugST2tp	DB	'  ',1,30h,'ST(2)='
DebugST2t	DB	'+0.00000000000000000E+0000',0
DebugST3tp	DB	'  ',1,30h,'ST(3)='
DebugST3t	DB	'+0.00000000000000000E+0000',0
DebugST4tp	DB	'  ',1,30h,'ST(4)='
DebugST4t	DB	'+0.00000000000000000E+0000',0
DebugST5tp	DB	'  ',1,30h,'ST(5)='
DebugST5t	DB	'+0.00000000000000000E+0000',0
DebugST6tp	DB	'  ',1,30h,'ST(6)='
DebugST6t	DB	'+0.00000000000000000E+0000',0
DebugST7tp	DB	'  ',1,30h,'ST(7)='
DebugST7t	DB	'+0.00000000000000000E+0000',0
DebugSTATtp	DB	'  ',1,30h,' STATUS='
DebugSTATt	DB	'0000',0
DebugCONTtp	DB	'  ',1,30h,'CONTROL='
DebugCONTt	DB	'0000',0
DebugTAGtp	DB	'  ',1,30h,'    TAG='
DebugTAGt	DB	'0000',0
DebugIPTRtp	DB	'  ',1,30h,'   IPTR='
DebugIPTRSt	DB	'0000:'
DebugIPTROt	DB	'00000000',0
DebugOPTRtp	DB	'  ',1,30h,'   OPTR='
DebugOPTRSt	DB	'0000:'
DebugOPTROt	DB	'00000000',0

DebuggerESP	dd ?
DebuggerSS	dw ?

DebugInst1	db ?
DebugAdd1	dd ?
DebugInst2	db ?
DebugAdd2	dd ?

InInt09	db 0
beep	db 7,"$"

TermNormal	label byte
	if ENGLISH
	db 'Program ended.',13,10,0
	elseif SPANISH
	db "Programa finalizado.",13,10,0
	endif
;
LoadError	label byte
	if ENGLISH
	db 13,10,13,10,'Error loading program to debug.',13,10,13,10,0
	elseif SPANISH
	db 13,10,13,10,"Error en la carga del fichero a depurar.",13,10,13,10,0
	endif
;
TerminationFlag db 0
;
DebugName	db 32 dup (0)
ErrHandle	dd 0
;
NewHeader	db size NewHeaderStruc dup (0)
;
ExeSignature	db ?	;00 Identifier text.
	db ?	;01 /
ExeLength	dw ?	;02 Length of file MOD 512
	dw ?	;04 Length of file in 512 byte blocks.
ExeRelocNum	dw ?	;06 Number of relocation items.
ExeHeaderSize	dw ?	;08 Length of header in 16 byte paragraphs.
ExeMinAlloc	dw ?	;0A Minimum number of para's needed above program.
ExeMaxAlloc	dw ?	;0C Maximum number of para's needed above program.
ExeStackSeg	dw ?	;0E Segment displacement of stack module.
ExeEntrySP	dw ?	;10 value for SP at entry.
ExeCheckSum	dw ?	;12 Check sum...
ExeEntryIP	dw ?	;14 Contents of IP at entry.
ExeEntryCS	dw ?	;16 Segment displacement of CS at entry.
ExeRelocFirst	dw ?	;18 First relocation item offset.
ExeOverlayNum	db ?	;1A Overlay number.

ErrorLevel	db 0

DebugPSP	dw ?

RealSegment	dw ?
SystemFlags	dw 0

MenuHandle	dw ?
MenuText	dd ?
CopyRight	db ' CauseWay debugger v2.07, Copyright 1992-98 Michael Devore; All rights reserved ',0
HelpHandle	dw ?

HelpText	dd ?

HelpText0	db '     ',1,74h,'ESC',1,70h,' - Cancel',0

HelpText1	label byte
	db 1,74h,'F1',1,70h,'-User'
	db ' ',1,74h,'F2',1,70h,'-BkPt'
	db ' ',1,74h,'F3',1,70h,'-GoTo'
	db ' ',1,74h,'F4',1,70h,'-Here'
	db ' ',1,74h,'F5',1,70h,'-Data'
	db ' ',1,74h,'F6',1,70h,'-Skip'
	db ' ',1,74h,'F7',1,70h,'-Trce'
	db ' ',1,74h,'F8',1,70h,'-Step'
	db ' ',1,74h,'F9',1,70h,'-Run '
	DB	' ',1,74h,'F10',1
FlipTextAttr	DB	70h,'-Flip',0

HelpText1a	label byte
	db 1,74h,'F1',1,70h,'-Syms'
	db ' ',1,74h,'F2',1,70h,'-HBrk'
	db ' ',1,74h,'F3',1,70h,'-List'
	db ' ',1,74h,'F4',1,70h,'-EIP '
	db ' ',1,74h,'F5',1,70h,'-Srce'
	db ' ',1,74h,'F6',1,70h,'-Ret '
	db ' ',1,74h,'F7',1,70h,'-Into'
	db ' ',1,74h,'F8',1,70h,'-View'
	db ' ',1,74h,'F9',1,70h,'-Res '
	db ' ',1,74h,'F10',1,70h,'-Rest',0

COMMENT !
HelpText1b	label byte
	db 1,74h,'F1',1,70h,'-Bin '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'   ',1,70h,'     ',0
END COMMENT !

HelpText2	label byte
	db 1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'F3',1,70h,'-Expr'
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'F5',1,70h,'-FPU ',0
;
HelpText2a	label byte
	db 1,74h,'F1',1,70h,'-Syms'
	db ' ',1,74h,'F3',1,70h,'-List'
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'F8',1,70h,'-View'
	db ' ',1,74h,' X',1,70h,'-Exit'
	db ' ',1,74h,'F10',1,70h,'-Rest',0
;
HelpText3	db 0
;
HelpText3a	db 1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'F8',1,70h,'-View'
	db ' ',1,74h,' X',1,70h,'-Exit'
	db ' ',1,74h,'F10',1,70h,'-Rest',0
;
HelpText4	db 0
;
HelpText4a	db 1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'F8',1,70h,'-View'
	db ' ',1,74h,' X',1,70h,'-Exit'
	db ' ',1,74h,'F10',1,70h,'-Rest',0
;
HelpText5	label byte
	db 1,74h,'F1',1,70h,'-Addr'
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'F3',1,70h,'-Goto'
	db ' ',1,74h,'F4',1,70h,'-Byte'
	db ' ',1,74h,'F5',1,70h,'-Word'
	db ' ',1,74h,'F6',1,70h,'-Dwrd'
	db ' ',1,74h,'F7',1,70h,'-Text'
	db ' ',1,74h,'F8',1,70h,'-Mixd'
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'F10',1,70h,'-Clse',0
;
HelpText5a	label byte
	db 1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'F3',1,70h,'-List'
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'F8',1,70h,'-View'
	db ' ',1,74h,' X',1,70h,'-Exit'
	db ' ',1,74h,'F10',1,70h,'-Rest',0
;
HelpText6	label byte
	db 1,74h,'  ',1,74h,18h,19h,'   '
	db ' ',1,74h,'  ',1,74h,'PgDn '
	db ' ',1,74h,'  ',1,74h,'PgUp '
	db ' ',1,74h,'  ',1,74h,'Home '
	db ' ',1,74h,'  ',1,74h,'End  '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'F10',1,70h,'-Clse',0
;
HelpText6a	label byte
	db 1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'F8',1,70h,'-View'
	db ' ',1,74h,' X',1,70h,'-Exit'
	db ' ',1,74h,'F10',1,70h,'-Rest',0
;
HelpText7	label byte
	db 1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,74h,'     '
	db ' ',1,74h,'  ',1,74h,'     '
	db ' ',1,74h,'  ',1,74h,'     '
	db ' ',1,74h,'  ',1,74h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'F10',1,70h,'-Clse',0
;
HelpText7a	label byte
	db 1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,'  ',1,70h,'     '
	db ' ',1,74h,' X',1,70h,'-Exit'
	db ' ',1,74h,'   ',1,70h,'     ',0
;
DisasHandle	dw ?
DisasTitle	db 1,3fh,'CS:EIP',0
RegsHandle	dw ?
RegsTitle	label byte
	if ENGLISH
	db 1,3fh,'Registers',0
	elseif SPANISH
	db 1,3fh,"Registros",0
	endif
;
DisasCount	dw 0,0,-1
DisasFresh	db -1
DisasCS	dw ?
DisasEIP	dd ?
DisasStartEIP	dd -1
DisasLastEIP	dd ?
DisasEndEIP	dd 0
;
SymbolSize	dd 0
SymbolBase	dd 0
SymbolList	dd Symbol1,0
Symbol1	dd -1
;
EXEFileName	db 64 dup (?)
EXEextension	db 'EXE',0
SYMFileName	db 64 dup (?)
SymHandle	dw 0
SYMextension	db 'SYM',0
MAPFileName	db 64 dup (?)
MapHandle	dw 0
MAPextension	db 'MAP',0
;
SymIDSpace	db 8 dup (0)
;
LineBuffer	db 1024 dup (?)
;
	db -1
SymHeaderText	db ' address publics by name',0
	db 128 dup (0)
;
CurrentSym	db 256 dup (?)
;
SymLayout	db 1,2,-1
	db 256 dup (-1)
;
DisasState	dd 0,0,0,0
Disas50	db 0
DisasPage	db 0
DisasMode	db 3
UserMode	db 0
UserOldMode	db 0
UserOld50	db 0
UserState	dd 0,0,0,0
User50	db 0
UserPage	db 0
UserOldPage	db 0
;
ForcedFlip	db 0
AutoFlip	db 0
FlipSwap	db 0
;
VidSwapMode	dw 0
;
ErrorNumber	dw 0
ErrorList	dd ErrorM0,ErrorM1,ErrorM2,ErrorM3,ErrorM4,ErrorM5
ErrorM0	db 13,10,' Application ready for debugging. ',13,10,13,10,0
ErrorM1	label byte
	if ENGLISH
	db 13,10,' DOS reported an error while reading the file. ',13,10,13,10,0
	elseif SPANISH
	db 13,10,"El DOS inform¢ de un error al leer el fichero.",13,10,13,10,0
	endif
ErrorM2	label byte
	if ENGLISH
	db 13,10,' File specified is not a CauseWay application. ',13,10,13,10,0
	elseif SPANISH
	db 13,10," El fichero especificado no es una aplicaci¢n CauseWay. ",13,10,13,10,0
	endif
ErrorM3	label byte
	if ENGLISH
	db 13,10,' Not enough memory to load application. ',13,10,13,10,0
	elseif SPANISH
	db 13,10," Memoria insuficiente para cargar la aplicaci¢n. ",13,10,13,10,0
	endif
ErrorM4	label byte
	if ENGLISH
	db 13,10,' No EXE file specified to load. ',13,10,13,10,0
	elseif SPANISH
	db 13,10," No se especific¢ ning£n fichero EXE para cargar. ",13,10,13,10,0
	endif
ErrorM5	label byte
	if ENGLISH
	db 13,10,' Could not find any symbols. ',13,10,13,10,0
	elseif SPANISH
	db 13,10," Pueden no encontrarse algunos s¡mbolos. ",13,10,13,10,0
	endif
;
BreakStruc	struc
BreakFlags	db 0
BreakInUse	equ	1	;entry in use.
BreakActive	equ	2	;break active.
BreakHidden	equ	4
BreakType	db 0
BreakType_exec	equ	0
BreakSize	db 0
BreakLinear	dd 0
BreakOld	db 0
BreakCountDown	dd 0
BreakCount	dd 0
BreakStruc	ends
;
BreakPointList	label byte
	db size BreakStruc*MaxBreaks dup (0)
;
ExecBreakHandle dw -1,-1,-1,0
;
DisasEIPList	dd 256 dup (0)
;
DisplayCS	dw ?
DisplayEIP	dd ?
DisasOldDepth	db -1
;
DisasBar	dw -1,0,0
DisasCurrent	dw -1,0,0
;
CurrentText	db '>',0
SpaceText	db ' ',0
SpaceText6	db '      ',0
BreakText	db ' [00]*',0
;
WindowsOpened	db 0
ScreenWidth	db ?
ScreenDepth	db ?
;
BreakDisList	dw 512 dup (-1)
;
Executing	db 0
ExceptionFlag	db -1
;
ExceptionText	label byte
	if ENGLISH
	db 13,10,' Exception: '
	elseif SPANISH
	db 13,10," Excepci¢n: "
	endif
ExceptionNumt	db '00h ',13,10,13,10,0
;
VectorList	dd 0
	df 256 dup (?)
	df 32 dup (?)
	dd 256 dup (?)
;
DisasGened	db 0
;
SearchCSEIPList df MaxEIPs dup (0)
;
DataWatchList	db size WatchStruc * MaxWatches dup (?)
;
WatchTitleText	db 1,3fh,'Data #'
WatchTitleNumt	db '00 - '
WatchTitleSeg	db 'DS:'
WatchTitleOff	db '???',0
WatchTLen	equ $-offset WatchTitleText
;
WatchSegList	db '??','CS','DS','ES','FS','GS','SS'
WatchOffList	db '??? ','EAX ','EBX ','ECX ','EDX ','ESI ','EDI ','EBP ','ESP ','EIP'
	db ' AX',0,' BX',0,' CX',0,' DX',0,' SI',0,' DI',0,' BP',0,' SP',0,' IP',0
;
SetHBRKText	db 1,7fh,"Enter HBRK values",0
;
CSEIPGotoText	label byte
	if ENGLISH
	db 1,07fh,'Enter address to goto',0
	elseif SPANISH
	db 1,07fh,"Introduzca la direcci¢n donde ir.",0
	endif
RegValText	label byte
	if ENGLISH
	db 1,07fh,'Enter new value',0
	elseif SPANISH
	db 1,07fh,"Introduzca el nuevo valor.",0
	endif
LoadFileText	label byte
	if ENGLISH
	db 1,07fh,'Enter file name to load',0
	elseif SPANISH
	db 1,07fh,"Introduzca el nombre del fichero a cargar.",0
	endif
;
TempHandle	dw ?
;
LoadingText	label byte
	if ENGLISH
	db ' Loading: ',0
	elseif SPANISH
	db "Cargando: ",0
	endif
;
KeyTable	db 128 dup (0)	;keypress table.
BreakKeyList	dd 1dh,38h,126 dup (0)
;
SystemError	dw 0
SErrorList	dd 0,SErrorM01,SErrorM02,SErrorM03,SErrorM04,SErrorM05
SErrorM01	label byte
	if ENGLISH
	db 'Insufficient physical memory to lock CWD in place.',13,10,0
	elseif SPANISH
	db "Insuficiente memoria f¡sica para bloquear CWD.",13,10,0
	endif
SErrorM02	label byte
	if ENGLISH
	db 'Insufficient selectors for CWD to startup.',13,10,0
	elseif SPANISH
	db "Insuficientes selectores para arrancar CWD.",13,10,0
	endif
SErrorM03	label byte
	if ENGLISH
	db 'Insufficient logical memory for CWD to startup.',13,10,0
	elseif SPANISH
	db "Insuficiente memeria l¢gica para arrancar CWD.",13,10,0
	endif
SErrorM04	label byte
	if ENGLISH
	db 'Internal null pointer called from: '
SErrorM04num	db '00000000, contact technical support.',13,10,0
	elseif SPANISH
	db "Puntero nulo interno llamado desde: '
SErrorM04num	db "00000000, contacte con soporte t‚cnico.",13,10,0
	endif
SErrorM05	label byte
	if ENGLISH
	db 'A VGA video card is required to run CWD.',13,10,0
	elseif SPANISH
	db "Se requiere VGA para ejecutar CWD.",13,10,0
	endif
;
TerminateCode	db 0
TerminateText	db 1,71h
	if ENGLISH
	db ' Application finished, return code: '
	elseif SPANISH
	db " Aplicaci¢n finalizada, c¢digo de retorno: "
	endif
TerminateNum	db '00 ',13,10,0
;
BadExpresion	label byte
	if ENGLISH
	db ' Invalid expression ',13,10,0
	elseif SPANISH
	db " Expresi¢n no v lida. ",13,10,0
	endif
BadEntryExpresion label byte
	if ENGLISH
	db ' Invalid /E expression ',13,10,0
	elseif SPANISH
	db " Expresi¢n /E no v lida. ",13,10,0
	endif
;
NoHardBreaks	db "All HBRK's are in use",13,10,0
;
cKeys	db ?
;
RegsXPos	db 0
RegsYPos	db 0
RegsLastX	db -1
RegsLastY	db -1
;
MonoSwap	db 0
;
UserInt09h	df 0
DisasInt09h	df 0
;
NoContextSwitch db 0
;
EntryGotoOK	db 0
EntryGotoText	db 128 dup (?)
;
SymBuffer	db 256 dup (?)
;
VideoSwapSel	dw ?
;
_DDW_Base	dd ?
_DDW_Flags	dw ?
_DDW_Handle	dw ?
_DDW_YPos	db ?
;
_DW_Base	dd ?
_DW_Flags	dw ?
_DW_Handle	dw ?
_DW_YPos	db ?
;
_DB_Base	dd ?
_DB_Flags	dw ?
_DB_Handle	dw ?
_DB_YPos	db ?
;
_DT_Base	dd ?
_DT_Flags	dw ?
_DT_Handle	dw ?
_DT_YPos	db ?
;
_DM_Base	dd ?
_DM_Flags	dw ?
_DM_Handle	dw ?
_DM_YPos	db ?
_DM_APos	db ?
;
_DLV_Handle	dw ?
;
_LV_Handle	dw ?
_LV_ListStart	dd ?
_LV_StartPos	dd ?
_LV_Longest	db ?
_LV_BarLast	dw ?
_LV_BarPos	dw ?
_LV_Depth	dw 15
_LV_Depth2	dd 0
_LV_SearchBuff	db 1,7fh,128 dup (0)
_LV_SearchPos	db 0
;
_DG_Handle	dw ?
_DG_DHandle	dw ?
;
_DWG_Handle	dw ?
_DWG_DHandle	dw ?
;
_RG_Handle	dw ?
_RG_DHandle	dw ?
;
CodeSegAlias	dw ?
ECodeSegAlias	dw ?
PSPSegment	dw ?
ENVSegment	dw ?
;
HBRKTitle	db "HBRK",0
HardBreakEntry	dd 0
HardBreakTable	db 4*size HBRK dup (0)
;
DisasSymbolTranslate dd 1
ForcedDisas	db 0
SymbolTranslate dd 1
RegsSelTranslate db 1

VideoUserState	dd ?
VideoOldUserState dd ?
VideoUserBuffer dd ?
VideoOldUserBuffer dd ?
VideoDebugState dd ?
VideoDebugBuffer dd ?

MouseUserState	dd ?
MouseOldUserState dd 0
MouseDebugState dd ?

EXEEnvExecString db 256 dup (?)

CW_DEBUG_ENTRY_CS_EIP db "CW_DEBUG_ENTRY_CS_EIP",0

SFT	struc
 SFT_Lines	dd 0
 SFT_File	dd 0
 SFT_Handle	dw 0
 SFT_SY	dd 0
 SFT_Y	dd 0
 SFT_X	dd 0
 SFT_Name	db 256 dup (0)
SFT	ends
;
SourceFileTable dd 0
;
SourceOpen	dd 0
;
_LSF_FileMem	dd ?
_LSF_LineMem	dd ?
_LSF_FileName	dd ?
;
AssemblerMode	db 0
CMode	db 0
;
DebugSegs	dd 0
;
RecentCallStack dd RetStackSize dup (0)
;
LINEList	dd 0,0
SourceFileWindow dd 0
;
LastFilePointer dd 0
;
FileNameSpace	db 256 dup (0)
FileNameSpace2	db 256 dup (0)
FileNameSpace3	db 256 dup (0)
;
OS2TypeMalloc	db 0
;
SourceIndex	dd 0,-1
SourceStartLine dd 0,-1
SourceLineNum	dd 0,0
;
SourceSearchIndex dd 0
SourceSearchLine dd 0
SourceSearchLineIndex dd 0
;
SourceNoTrack	db -1
SourceForceCheck db 0
;
ConfigName	db 'cwd.cfg',0
ConfigPath	db 128 dup (?)
ConfigHandle	dw ?
;
VariableList	label dword
	dd SourcePathVAR,WhiteSpaceString,SourceFilePath,0
	dd SourceTABsVAR,CommaString,SourceTABSettings,0
	dd BreakKeyVAR,CommaString,BreakKeyList,0
	dd ResetTimerVAR,YesNoString,ResetTimer,0
	dd -1
;
_CFG_Source	dd 0
SourcePathVAR	db "SourcePath",0
SourcePathPointer dd 0
SourceFilePath	db ";",1023 dup (0)
;
SourceTABsVAR	db "SourceTABs",0
SourceTABSettings dd 0+1,8+1,16+1,24+1,32+1,40+1,48+1,56+1
	dd 64+1,72+1,80+1,88+1,96+1,104+1,112+1,120+1
	dd 16 dup (120+1)
;
BreakKeyVAR	db "BreakKeys",0
;
ResetTimerVAR	db "ResetTimer",0
ResetTimer	dd -1
;
WhichScreen	db 0
ScreenSwapped	dd 0
;
ExecSourceLine	dd 0
DontUseSource	db 0
;
GotStartEIP	db 0
;
DisasTitleSpace db 1,3fh, 256 dup (0)
;
LastSymbol	dd 0
;
FirstMono	db 0

DebugCSApp	dd ?
DebugDSApp	dd ?
DebugESApp	dd ?
DebugFSApp	dd ?
DebugGSApp	dd ?
DebugSSApp	dd ?
;
DebugExceptionIndex dw 0
DebugExceptionCode dd 0
;
eDebugName	db 'cwd.err',0

CSEIPDebugText	DB	"CS:EIP > ",0
SSESPDebugText	DB	"SS:ESP > ",0
SSEBPDebugText	DB	"SS:EBP > ",0

;
ResHeader	db 13,10
	db '   Application resource tracking details   ',13,10
	db '===========================================',13,10,'$'
ResHeaderLen	equ	($-1)-ResHeader
;
SelHeader	db 13,10,13,10
	db 'Selectors',13,10
	db '=========',13,10,13,10
	db ' sel   base     limit  type D mem count',13,10
	db '----------------------------------------',13,10,'$'
SelHeaderLen	equ ($-1)-SelHeader
;
TotalSelsText	db 13,10,"Total selectors: "
TotalSelsNum	db "0000",13,10
TotalSelsTLen	equ	$-TotalSelsText
;
MemHeader	db 13,10,13,10
	db 'Linear memory blocks',13,10
	db '====================',13,10,13,10
	db ' handle    base    length ',13,10
	db '--------------------------',13,10,'$'
MemHeaderLen	equ ($-1)-MemHeader
;
TotalMemText	db 13,10,"Total Linear memory: "
TotalMemNum1	db "00000000 ("
TotalMemNum2	db "00000000) in "
TotalMemNum3	db "00000000 blocks",13,10
TotalMemTLen	equ	$-TotalMemText
;
MouseEHeader	db 13,10,13,10
	db "Mouse event target: ","$"
MouseEHeaderLen equ ($-1)-MouseEHeader
DosMemHeader	db 13,10,13,10
	db "DOS memory blocks",13,10
	db "=================",13,10,13,10
	db " sel   base    length ",13,10
	db "----------------------",13,10,"$"
DosMemHeaderLen equ ($-1)-DosMemHeader
CallBackHeader	db 13,10,13,10
	db "Call-Backs",13,10
	db "==========",13,10,13,10
	db "  real        target",13,10
	db "-----------------------",13,10,"$"
CallBackHeaderLen equ ($-1)-CallBackHeader
LockHeader	db 13,10,13,10
	db 'Linear memory locked',13,10
	db '====================',13,10,13,10
	db '  base    length ',13,10
	db '-----------------',13,10,'$'
LockHeaderLen	equ ($-1)-LockHeader
;
PIntHeader	db 13,10,13,10
	db 'Protected mode interrupt vectors',13,10
	db '================================',13,10,13,10
	db 'No sel   offset ',13,10
	db '----------------',13,10,'$'
PIntHeaderLen	equ ($-1)-PIntHeader
;
EIntHeader	db 13,10,13,10
	db 'Protected mode exception vectors',13,10
	db '================================',13,10,13,10
	db 'No sel   offset ',13,10
	db '----------------',13,10,'$'
EIntHeaderLen	equ ($-1)-EIntHeader
;
RIntHeader	db 13,10,13,10
	db 'Real mode interrupt vectors',13,10
	db '===========================',13,10,13,10
	db 'No seg offset',13,10
	db '-------------',13,10,'$'
RIntHeaderLen	equ ($-1)-RIntHeader
;
TotalLinearMem	dd 0,0,0
TotalSelectors	dd 0
;
eDebugHeader	db 1024 dup (0)
eDebugRegs	db 13,10,13,10
	db 'EAX='
eDebugEAXt	db '00000000 '
	db 'EBX='
eDebugEBXt	db '00000000 '
	db 'ECX='
eDebugECXt	db '00000000 '
	db 'EDX='
eDebugEDXt	db '00000000 '
	db 'ESI='
eDebugESIt	db '00000000 '
	db 13,10
	db 'EDI='
eDebugEDIt	db '00000000 '
	db 'EBP='
eDebugEBPt	db '00000000 '
	db 'ESP='
eDebugESPt	db '00000000 '
	db 'EIP='
eDebugEIPt	db '00000000 '
	db 'EFL='
eDebugEFLt	db '00000000 '
	db 13,10,13,10
	db 'CS='
eDebugCSt	db '0000-'
eDebugCSAppt	db '00000000 '
	db 'DS='
eDebugDSt	db '0000-'
eDebugDSAppt	db '00000000 '
	db 'ES='
eDebugESt	db '0000-'
eDebugESAppt	db '00000000 '
	db 13,10
	db 'FS='
eDebugFSt	db '0000-'
eDebugFSAppt	db '00000000 '
	db 'GS='
eDebugGSt	db '0000-'
eDebugGSAppt	db '00000000 '
	db 'SS='
eDebugSSt	db '0000-'
eDebugSSAppt	db '00000000 '
	db 13,10,13,10
	db 'CR0='
eDebugCR0t	db '00000000 '
	db 'CR2='
eDebugCR2t	db '00000000 '
	db 'CR3='
eDebugCR3t	db '00000000 '
	db 'TR='
eDebugTRt	db '0000'
	db 13,10,13,10
	db 'Info flags='
DebugSysFlags	db '00000000 '
	db 13,10,13,10,'$'
eDebugHeaderEnd	label byte

DebugCR0	dd 0
DebugCR2	dd 0
DebugCR3	dd 0

ERRFile	dd 0

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
_EXCEP	segment para public 'code' use16
	assume cs:_EXCEP, ds:DGROUP

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Int00Handler	proc	near
	pushm	eax,ebp,ds,es
	mov	ax,DGROUP		;make our data addresable.
	mov	ds,ax
	cmp	Executing,0
	jz	@@Old
	mov	Executing,0
	mov	ebp,esp		;make stack addresable.
	test	SystemFlags,1
	jz	@@Use32_0
	movzx	ebp,bp		;use 16 bit stack.
@@Use32_0:	;
	;Need a stack alias for DPMI.
	;
	mov	ax,ss
	mov	es,ax
	;
	;Retrieve general registers.
	;
	mov	eax,es:[ebp+4+2+2]
	mov	DebugEAX,eax
	mov	DebugEBX,ebx
	mov	DebugECX,ecx
	mov	DebugEDX,edx
	mov	DebugESI,esi
	mov	DebugEDI,edi
	mov	eax,es:[ebp+2+2]
	mov	DebugEBP,eax
	mov	ax,es:[ebp+2]
	mov	DebugDS,ax
	mov	ax,es:[ebp+0]
	mov	DebugES,ax
	mov	DebugFS,fs
	mov	DebugGS,gs
	;
	;Fetch origional Flags:CS:EIP,SS:ESP
	;
	test	SystemFlags,1
	jz	@@Use32_1
	mov	ax,es:[ebp+(4+4+2+2)+(2+2)]
	mov	w[DebugEFL],ax
	mov	ax,es:[ebp+(4+4+2+2)+(2)]
	mov	DebugCS,ax
	movzx	eax,es:w[ebp+(4+4+2+2)+(0)]
	mov	DebugEIP,eax
	mov	DebugSS,ss
	mov	DebugESP,esp
	add	DebugESP,(4+4+2+2)+(2+2+2)
	jmp	@@Use0_1
	;
@@Use32_1:	mov	eax,es:[ebp+(4+4+2+2)+(4+4)]
	mov	DebugEFL,eax
	mov	eax,es:[ebp+(4+4+2+2)+(4)]
	mov	DebugCS,ax
	mov	eax,es:[ebp+(4+4+2+2)+(0)]
	mov	DebugEIP,eax
	mov	DebugSS,ss
	mov	DebugESP,esp
	add	DebugESP,(4+4+2+2)+(4+4+4)
	jmp	@@Use0_1
	;
@@Use0_1:	mov	ExceptionFlag,0
	;
	;Now modify origional CS:EIP,SS:ESP values and return control
	;to this code via interupt structure to restore stacks.
	;
	test	SystemFlags,1
	jz	@@Use32_2
	mov	eax,offset @@Use0_2
	mov	es:[ebp+(4+4+2+2)+(0)],ax
	mov	es:w[ebp+(4+4+2+2)+(2)],_EXCEP
	popm	eax,ebp,ds,es
	iret
	;
@@Use32_2:	mov	eax,offset @@Use0_2
	mov	es:[ebp+(4+4+2+2)+(0)],eax
	mov	es:w[ebp+(4+4+2+2)+(4)],_EXCEP
	popm	eax,ebp,ds,es
	iretd
	;
@@Use0_2:	;Now return control to exec caller.
	;
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	lss	esp,f[DebuggerESP]
	db 66h
	retf
;
@@Old:	popm	eax,ebp,ds,es
	assume ds:nothing
	jmp	cs:f[OldInt00]
	assume ds:DGROUP
;
OldInt00	df 0
Int00Handler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Int01Handler	proc	near
	pushm	eax,ebp,ds,es
	mov	ax,DGROUP		;make our data addresable.
	mov	ds,ax
	cmp	Executing,0
	jz	@@Old
	call	IsHardBreak
	jnz	@@Old
	mov	Executing,0
	mov	ExceptionFlag,1
	mov	ebp,esp		;make stack addresable.
	test	SystemFlags,1
	jz	@@Use32_0
	movzx	ebp,bp		;use 16 bit stack.
@@Use32_0:	;
	;Need a stack alias for DPMI.
	;
	mov	ax,ss
	mov	es,ax
	;
	;Retrieve general registers.
	;
	mov	eax,es:[ebp+4+2+2]
	mov	DebugEAX,eax
	mov	DebugEBX,ebx
	mov	DebugECX,ecx
	mov	DebugEDX,edx
	mov	DebugESI,esi
	mov	DebugEDI,edi
	mov	eax,es:[ebp+2+2]
	mov	DebugEBP,eax
	mov	ax,es:[ebp+2]
	mov	DebugDS,ax
	mov	ax,es:[ebp+0]
	mov	DebugES,ax
	mov	DebugFS,fs
	mov	DebugGS,gs
	;
	;Fetch origional Flags:CS:EIP,SS:ESP
	;
	test	SystemFlags,1
	jz	@@Use32_1
	mov	ax,es:[ebp+(4+4+2+2)+(2+2)]
	mov	w[DebugEFL],ax
	mov	ax,es:[ebp+(4+4+2+2)+(2)]
	mov	DebugCS,ax
	movzx	eax,es:w[ebp+(4+4+2+2)+(0)]
	mov	DebugEIP,eax
	mov	DebugSS,ss
	mov	DebugESP,esp
	add	DebugESP,(4+4+2+2)+(2+2+2)
	jmp	@@Use0_1
	;
@@Use32_1:	mov	eax,es:[ebp+(4+4+2+2)+(4+4)]
	mov	DebugEFL,eax
	mov	eax,es:[ebp+(4+4+2+2)+(4)]
	mov	DebugCS,ax
	mov	eax,es:[ebp+(4+4+2+2)+(0)]
	mov	DebugEIP,eax
	mov	DebugSS,ss
	mov	DebugESP,esp
	add	DebugESP,(4+4+2+2)+(4+4+4)
	jmp	@@Use0_1
	;
@@Use0_1:
;	cmp	ExceptionFlag,-1
;	jnz	@@NoEIPDec
;	dec	DebugEIP		;account for int 3 instruction length.
;	cmp	TerminationFlag,-1
;	jnz	@@NoEIPDec
;	dec	DebugEIP
@@NoEIPDec:	;
	;Now modify origional CS:EIP,SS:ESP values and return control
	;to this code via interupt structure to restore stacks.
	;
	test	SystemFlags,1
	jz	@@Use32_2
	mov	eax,offset @@Use0_2
	mov	es:[ebp+(4+4+2+2)+(0)],ax
	mov	es:w[ebp+(4+4+2+2)+(2)],_EXCEP
	popm	eax,ebp,ds,es
	iret
	;
@@Use32_2:	mov	eax,offset @@Use0_2
	mov	es:[ebp+(4+4+2+2)+(0)],eax
	mov	es:w[ebp+(4+4+2+2)+(4)],_EXCEP
	popm	eax,ebp,ds,es
	iretd
	;
@@Use0_2:	;Now return control to exec caller.
	;
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	lss	esp,f[DebuggerESP]
	db 66h
	retf
;
@@Old:	popm	eax,ebp,ds,es
	assume ds:nothing
	jmp	cs:f[OldInt01]
	assume ds:DGROUP
;
OldInt01	df 0
Int01Handler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Int03Handler	proc	near
	pushm	eax,ebp,ds,es
	mov	ax,DGROUP		;make our data addresable.
	mov	ds,ax
	cmp	Executing,0
	jz	@@Old
	mov	Executing,0
	mov	ebp,esp		;make stack addresable.
	test	SystemFlags,1
	jz	@@Use32_0
	movzx	ebp,bp		;use 16 bit stack.

@@Use32_0:
	;Need a stack alias for DPMI.
	;
	mov	ax,ss
	mov	es,ax
	;
	;Retrieve general registers.
	;
	mov	eax,es:[ebp+4+2+2]
	mov	DebugEAX,eax
	mov	DebugEBX,ebx
	mov	DebugECX,ecx
	mov	DebugEDX,edx
	mov	DebugESI,esi
	mov	DebugEDI,edi
	mov	eax,es:[ebp+2+2]
	mov	DebugEBP,eax
	mov	ax,es:[ebp+2]
	mov	DebugDS,ax
	mov	ax,es:[ebp+0]
	mov	DebugES,ax
	mov	DebugFS,fs
	mov	DebugGS,gs

;Fetch original Flags:CS:EIP,SS:ESP
	test	SystemFlags,1
	jz	@@Use32_1

; MED 08/15/96
; attempt to allow debugging of hardware interrupt handler
;  which has extra values placed on stack
	mov	ax,ss
	cmp	ax,KernalSS
	jne	med2a
	mov	ax,es:[ebp+(4+4+2+2)+(2+2)+(2+2+2)]
	mov	w[DebugEFL],ax
	mov	ax,es:[ebp+(4+4+2+2)+(2)+(2+2+2)]
	mov	DebugCS,ax
	movzx	eax,es:w[ebp+(4+4+2+2)+(0)+(2+2+2)]
	mov	DebugEIP,eax
	mov	DebugSS,ss
	mov	DebugESP,esp
;	add	DebugESP,(4+4+2+2)+(2+2+2)+(2+2+2)
	add	DebugESP,(4+4+2+2)+(2+2+2)
	jmp	@@Use0_1
med2a:

	mov	ax,es:[ebp+(4+4+2+2)+(2+2)]
	mov	w[DebugEFL],ax
	mov	ax,es:[ebp+(4+4+2+2)+(2)]
	mov	DebugCS,ax
	movzx	eax,es:w[ebp+(4+4+2+2)+(0)]
	mov	DebugEIP,eax
	mov	DebugSS,ss
	mov	DebugESP,esp
	add	DebugESP,(4+4+2+2)+(2+2+2)
	jmp	@@Use0_1

@@Use32_1:

; MED 08/15/96
; attempt to allow debugging of hardware interrupt handler
;  which has extra values placed on stack
	mov	ax,ss
	cmp	ax,KernalSS
	jne	med2b
	mov	eax,es:[ebp+(4+4+2+2)+(4+4)+(4+4+4)]
	mov	DebugEFL,eax
	mov	eax,es:[ebp+(4+4+2+2)+(4)+(4+4+4)]
	mov	DebugCS,ax
	mov	eax,es:[ebp+(4+4+2+2)+(0)+(4+4+4)]
	mov	DebugEIP,eax
	mov	DebugSS,ss
	mov	DebugESP,esp
;	add	DebugESP,(4+4+2+2)+(4+4+4)+(4+4+4)
	add	DebugESP,(4+4+2+2)+(4+4+4)
	jmp	@@Use0_1
med2b:

	mov	eax,es:[ebp+(4+4+2+2)+(4+4)]
	mov	DebugEFL,eax
	mov	eax,es:[ebp+(4+4+2+2)+(4)]
	mov	DebugCS,ax
	mov	eax,es:[ebp+(4+4+2+2)+(0)]
	mov	DebugEIP,eax
	mov	DebugSS,ss
	mov	DebugESP,esp
	add	DebugESP,(4+4+2+2)+(4+4+4)
	jmp	@@Use0_1

@@Use0_1:
	cmp	ExceptionFlag,-1
	jnz	@@NoEIPDec
	dec	DebugEIP		;account for int 3 instruction length.
	cmp	TerminationFlag,-1
	jnz	@@NoEIPDec
	dec	DebugEIP
@@NoEIPDec:	;
	;Now modify origional CS:EIP,SS:ESP values and return control
	;to this code via interupt structure to restore stacks.
	;
	test	SystemFlags,1
	jz	@@Use32_2
	mov	eax,offset @@Use0_2
	mov	es:[ebp+(4+4+2+2)+(0)],ax
	mov	es:w[ebp+(4+4+2+2)+(2)],_EXCEP
	popm	eax,ebp,ds,es
	iret
	;
@@Use32_2:	mov	eax,offset @@Use0_2
	mov	es:[ebp+(4+4+2+2)+(0)],eax
	mov	es:w[ebp+(4+4+2+2)+(4)],_EXCEP
	popm	eax,ebp,ds,es
	iretd
	;
@@Use0_2:	;Now return control to exec caller.
	;
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	lss	esp,f[DebuggerESP]
	db 66h
	retf
;
@@Old:	popm	eax,ebp,ds,es
	assume ds:nothing
	jmp	cs:f[OldInt03]
	assume ds:DGROUP
;
OldInt03	df 0
Int03Handler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Int23Handler	proc	near
	pushm	ds,eax
	mov	ax,DGROUP
	mov	ds,ax
	cmp	Executing,0
	jnz	@@Close
	test	SystemFlags,1
	popm	ds,eax
	jz	@@Use32_10
	iret
@@Use32_10:	iretd
	;
@@Close:
	mov	TerminationFlag,-1
	mov	TerminateCode,0
	popm	ds,eax

	pushm	eax,ebp,ds,es
	mov	ax,DGROUP		;make our data addressable.
	mov	ds,ax
	mov	ebp,esp		;make stack addressable.
	test	SystemFlags,1
	jz	@@Use32_0
	movzx	ebp,bp		;use 16 bit stack.
@@Use32_0:

	;Need a stack alias for DPMI.
	mov	ax,ss
	mov	es,ax
	;
	;Now modify original CS:EIP,SS:ESP values and return control
	;to this code via interrupt structure to restore stacks.
	;
	test	SystemFlags,1
	jz	@@Use32_2
	mov	eax,offset @@Use0_2
	mov	es:[ebp+(4+4+2+2)+(0)],ax
	mov	es:w[ebp+(4+4+2+2)+(2)],_EXCEP
	popm	eax,ebp,ds,es
	iret

@@Use32_2:	mov	eax,offset @@Use0_2
	mov	es:[ebp+(4+4+2+2)+(0)],eax
	mov	es:w[ebp+(4+4+2+2)+(4)],_EXCEP
	popm	eax,ebp,ds,es
	iretd

@@Use0_2:	;Now return control to exec caller.
	;
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	lss	esp,f[DebuggerESP]
	db 66h
	retf
OldInt23	df 0
Int23Handler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Int21Handler	proc	near
	pushf
	cmp	ah,4ch	;terminate?
	jnz	@@Old

	pushm	ax,bx,ds
	mov	ah,62h
	int	21h
	mov	ds,bx
	lds	bx,ds:[EPSP_ExecCount]
	cmp	b[bx],0
	popm	ax,bx,ds
	jnz	@@Old

	popf
	pushm	ds,eax
	mov	ax,DGROUP
	mov	ds,ax
	mov	TerminationFlag,-1
	mov	eax,esp
	test	SystemFlags,1
	jz	@@Use32
	movzx	eax,ax
@@Use32:	mov	al,ss:[eax]
	mov	TerminateCode,al
	popm	ds,eax
	jmp	Int03Handler
@@Old:	;
	popf
	assume ds:nothing
	jmp	cs:f[OldInt21]
	assume ds:DGROUP
OldInt21	df 0
Int21Handler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
EInt01Handler	proc	near
	pushm	eax,ebp,ds,es
	mov	ax,DGROUP		;make our data addresable.
	mov	ds,ax
	cmp	Executing,0
	jz	@@Old
	call	IsHardBreak
	jnz	@@Old
	mov	Executing,0
	mov	ExceptionFlag,1
	mov	ebp,esp		;make stack addresable.
	test	SystemFlags,1
	jz	@@Use32_0
	movzx	ebp,bp		;use 16 bit stack.
@@Use32_0:	;
	mov	ax,ss
	mov	es,ax
	;
	;Retrieve general registers.
	;
	mov	eax,es:[ebp+2+2+4]
	mov	DebugEAX,eax
	mov	DebugEBX,ebx
	mov	DebugECX,ecx
	mov	DebugEDX,edx
	mov	DebugESI,esi
	mov	DebugEDI,edi
	mov	eax,es:[ebp+2+2]
	mov	DebugEBP,eax
	mov	ax,es:[ebp+2]
	mov	DebugDS,ax
	mov	ax,es:[ebp+0]
	mov	DebugES,ax
	mov	DebugFS,fs
	mov	DebugGS,gs
	;
	;Fetch origional Flags:CS:EIP,SS:ESP
	;
	test	SystemFlags,1
	jz	@@Use32_1
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2+2)+2]
	mov	w[DebugEFL],ax
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2)+2]
	mov	DebugCS,ax
	movzx	eax,es:w[ebp+(4+4+2)+(2+2+2)+(0)+2]
	mov	DebugEIP,eax
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2+2+2)+(2)+2]
	mov	DebugSS,ax
	movzx	eax,es:w[ebp+(4+4+2)+(2+2+2)+(2+2+2)+(0)+2]
	mov	DebugESP,eax
	jmp	@@Use0_1
	;
@@Use32_1:	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(4+4)+2]
	mov	DebugEFL,eax
	mov	ax,es:[ebp+(4+4+2)+(4+4+4)+(4)+2]
	mov	DebugCS,ax
	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(0)+2]
	mov	DebugEIP,eax
	mov	ax,es:[ebp+(4+4+2)+(4+4+4)+(4+4+4)+(4)+2]
	mov	DebugSS,ax
	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(4+4+4)+(0)+2]
	mov	DebugESP,eax
	;
@@Use0_1:
;	cmp	ExceptionFlag,-1
;	jnz	@@NoEIPDec
;	dec	DebugEIP		;account for int 3 instruction length.
;	cmp	TerminationFlag,-1
;	jnz	@@NoEIPDec
;	dec	DebugEIP
@@NoEIPDec:	;
	;Now modify origional CS:EIP,SS:ESP values and return control
	;to this code via interupt structure to restore stacks.
	;
	test	SystemFlags,1
	jz	@@Use32_2
	mov	eax,offset @@Use0_2
	mov	es:w[ebp+(4+4+2)+(2+2+2)+(0)+2],ax
	mov	es:w[ebp+(4+4+2)+(2+2+2)+(2)+2],_EXCEP
	and	es:w[ebp+(4+4+2)+(2+2+2)+(2+2)+2],65535-256
	popm	eax,ebp,ds,es
	retf
	;
@@Use32_2:	mov	eax,offset @@Use0_2
	mov	es:d[ebp+(4+4+2)+(4+4+4)+(0)+2],eax
	mov	es:w[ebp+(4+4+2)+(4+4+4)+(4)+2],_EXCEP
	and	es:w[ebp+(4+4+2)+(4+4+4)+(4+4)+2],65535-256
	popm	eax,ebp,ds,es
	db 66h
	retf
	;
@@Use0_2:	;Now return control to exec caller.
	;
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	lss	esp,f[DebuggerESP]
	db 66h
	retf
;
@@Old:	popm	eax,ebp,ds,es
	assume ds:nothing
	jmp	cs:f[OldEInt01]
	assume ds:DGROUP
;
OldEInt01	df 0
EInt01Handler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
EInt03Handler	proc	near
	pushm	eax,ebp,ds,es
	mov	ax,DGROUP		;make our data addresable.
	mov	ds,ax
	cmp	Executing,0
	jz	@@Old
	mov	Executing,0
	mov	ebp,esp		;make stack addresable.
	test	SystemFlags,1
	jz	@@Use32_0
	movzx	ebp,bp		;use 16 bit stack.
@@Use32_0:	;
	mov	ax,ss
	mov	es,ax
	;
	;Retrieve general registers.
	;
	mov	eax,es:[ebp+2+2+4]
	mov	DebugEAX,eax
	mov	DebugEBX,ebx
	mov	DebugECX,ecx
	mov	DebugEDX,edx
	mov	DebugESI,esi
	mov	DebugEDI,edi
	mov	eax,es:[ebp+2+2]
	mov	DebugEBP,eax
	mov	ax,es:[ebp+2]
	mov	DebugDS,ax
	mov	ax,es:[ebp+0]
	mov	DebugES,ax
	mov	DebugFS,fs
	mov	DebugGS,gs
	;
	;Fetch origional Flags:CS:EIP,SS:ESP
	;
	test	SystemFlags,1
	jz	@@Use32_1
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2+2)+2]
	mov	w[DebugEFL],ax
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2)+2]
	mov	DebugCS,ax
	movzx	eax,es:w[ebp+(4+4+2)+(2+2+2)+(0)+2]
	mov	DebugEIP,eax
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2+2+2)+(2)+2]
	mov	DebugSS,ax
	movzx	eax,es:w[ebp+(4+4+2)+(2+2+2)+(2+2+2)+(0)+2]
	mov	DebugESP,eax
	jmp	@@Use0_1
	;
@@Use32_1:	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(4+4)+2]
	mov	DebugEFL,eax
	mov	ax,es:[ebp+(4+4+2)+(4+4+4)+(4)+2]
	mov	DebugCS,ax
	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(0)+2]
	mov	DebugEIP,eax
	mov	ax,es:[ebp+(4+4+2)+(4+4+4)+(4+4+4)+(4)+2]
	mov	DebugSS,ax
	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(4+4+4)+(0)+2]
	mov	DebugESP,eax
	;
@@Use0_1:	cmp	ExceptionFlag,-1
	jnz	@@NoEIPDec
	dec	DebugEIP		;account for int 3 instruction length.
	cmp	TerminationFlag,-1
	jnz	@@NoEIPDec
	dec	DebugEIP
@@NoEIPDec:	;
	;Now modify origional CS:EIP,SS:ESP values and return control
	;to this code via interupt structure to restore stacks.
	;
	test	SystemFlags,1
	jz	@@Use32_2
	mov	eax,offset @@Use0_2
	mov	es:w[ebp+(4+4+2)+(2+2+2)+(0)+2],ax
	mov	es:w[ebp+(4+4+2)+(2+2+2)+(2)+2],_EXCEP
	and	es:w[ebp+(4+4+2)+(2+2+2)+(2+2)+2],65535-256
	popm	eax,ebp,ds,es
	retf
	;
@@Use32_2:	mov	eax,offset @@Use0_2
	mov	es:d[ebp+(4+4+2)+(4+4+4)+(0)+2],eax
	mov	es:w[ebp+(4+4+2)+(4+4+4)+(4)+2],_EXCEP
	and	es:w[ebp+(4+4+2)+(4+4+4)+(4+4)+2],65535-256
	popm	eax,ebp,ds,es
	db 66h
	retf
	;
@@Use0_2:	;Now return control to exec caller.
	;
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	lss	esp,f[DebuggerESP]
	db 66h
	retf
;
@@Old:	popm	eax,ebp,ds,es
	assume ds:nothing
	jmp	cs:f[OldEInt03]
	assume ds:DGROUP
;
OldEInt03	df 0
EInt03Handler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Exc12Handler	proc	near
;
;Exception has been generated.
;
	pushm	eax,ebp,ds,es
	mov	ax,DGROUP		;make our data addresable.
	mov	ds,ax
	cmp	Executing,0
	jz	@@Old
	mov	Executing,0
	mov	ExceptionFlag,12
	mov	ebp,esp		;make stack addresable.
	test	SystemFlags,1
	jz	@@Use32_0
	movzx	ebp,bp		;use 16 bit stack.
@@Use32_0:	;
	;Need a stack alias for DPMI.
	;
	mov	ax,ss
	mov	es,ax
	;
	;Retrieve general registers.
	;
	mov	eax,es:[ebp+4+2+2]
	mov	DebugEAX,eax
	mov	DebugEBX,ebx
	mov	DebugECX,ecx
	mov	DebugEDX,edx
	mov	DebugESI,esi
	mov	DebugEDI,edi
	mov	eax,es:[ebp+2+2]
	mov	DebugEBP,eax
	mov	ax,es:[ebp+2]
	mov	DebugDS,ax
	mov	ax,es:[ebp+0]
	mov	DebugES,ax
	mov	DebugFS,fs
	mov	DebugGS,gs
	;
	;Fetch origional Flags:CS:EIP,SS:ESP
	;
	test	SystemFlags,1
	jz	@@Use32_1
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2+2)+2]
	mov	w[DebugEFL],ax
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2)+2]
	mov	DebugCS,ax
	movzx	eax,es:w[ebp+(4+4+2)+(2+2+2)+(0)+2]
	mov	DebugEIP,eax
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2+2+2)+(2)+2]
	mov	DebugSS,ax
	movzx	eax,es:w[ebp+(4+4+2)+(2+2+2)+(2+2+2)+(0)+2]
	mov	DebugESP,eax
	jmp	@@Use0_1
	;
@@Use32_1:	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(4+4)+2]
	mov	DebugEFL,eax
	mov	ax,es:[ebp+(4+4+2)+(4+4+4)+(4)+2]
	mov	DebugCS,ax
	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(0)+2]
	mov	DebugEIP,eax
	mov	ax,es:[ebp+(4+4+2)+(4+4+4)+(4+4+4)+(4)+2]
	mov	DebugSS,ax
	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(4+4+4)+(0)+2]
	mov	DebugESP,eax
	;
@@Use0_1:	;Now modify origional CS:EIP,SS:ESP values and return control
	;to this code via interupt structure to restore stacks.
	;
	test	SystemFlags,1
	jz	@@Use32_2
	mov	eax,offset @@Use0_2
	mov	es:w[ebp+(4+4+2)+(2+2+2)+(0)+2],ax
	mov	es:w[ebp+(4+4+2)+(2+2+2)+(2)+2],_EXCEP
	and	es:w[ebp+(4+4+2)+(2+2+2)+(2+2)+2],65535-256
	popm	eax,ebp,ds,es
	retf
	;
@@Use32_2:	mov	eax,offset @@Use0_2
	mov	es:d[ebp+(4+4+2)+(4+4+4)+(0)+2],eax
	mov	es:w[ebp+(4+4+2)+(4+4+4)+(4)+2],_EXCEP
	and	es:w[ebp+(4+4+2)+(4+4+4)+(4+4)+2],65535-256
	popm	eax,ebp,ds,es
	db 66h
	retf
	;
@@Use0_2:	;Now return control to exec caller.
	;
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	lss	esp,f[DebuggerESP]
	db 66h
	retf
	;
@@Old:	popm	eax,ebp,ds
	assume ds:nothing
	jmp	cs:f[OldExc12]
	assume ds:DGROUP
;
OldExc12	df 0
Exc12Handler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Exc13Handler	proc	near
;
;Exception has been generated.
;
	pushm	eax,ebp,ds,es
	mov	ax,DGROUP		;make our data addresable.
	mov	ds,ax
	cmp	Executing,0
	jz	@@Old
	mov	Executing,0
	mov	ExceptionFlag,13
	mov	ebp,esp		;make stack addresable.
	test	SystemFlags,1
	jz	@@Use32_0
	movzx	ebp,bp		;use 16 bit stack.
@@Use32_0:	;
	;Need a stack alias for DPMI.
	;
	mov	ax,ss
	mov	es,ax
	;
	;Retrieve general registers.
	;
	mov	eax,es:[ebp+4+2+2]
	mov	DebugEAX,eax
	mov	DebugEBX,ebx
	mov	DebugECX,ecx
	mov	DebugEDX,edx
	mov	DebugESI,esi
	mov	DebugEDI,edi
	mov	eax,es:[ebp+2+2]
	mov	DebugEBP,eax
	mov	ax,es:[ebp+2]
	mov	DebugDS,ax
	mov	ax,es:[ebp+0]
	mov	DebugES,ax
	mov	DebugFS,fs
	mov	DebugGS,gs
	;
	;Fetch origional Flags:CS:EIP,SS:ESP
	;
	test	SystemFlags,1
	jz	@@Use32_1
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2+2)+2]
	mov	w[DebugEFL],ax
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2)+2]
	mov	DebugCS,ax
	movzx	eax,es:w[ebp+(4+4+2)+(2+2+2)+(0)+2]
	mov	DebugEIP,eax
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2+2+2)+(2)+2]
	mov	DebugSS,ax
	movzx	eax,es:w[ebp+(4+4+2)+(2+2+2)+(2+2+2)+(0)+2]
	mov	DebugESP,eax
	jmp	@@Use0_1
	;
@@Use32_1:	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(4+4)+2]
	mov	DebugEFL,eax
	mov	ax,es:[ebp+(4+4+2)+(4+4+4)+(4)+2]
	mov	DebugCS,ax
	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(0)+2]
	mov	DebugEIP,eax
	mov	ax,es:[ebp+(4+4+2)+(4+4+4)+(4+4+4)+(4)+2]
	mov	DebugSS,ax
	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(4+4+4)+(0)+2]
	mov	DebugESP,eax
	;
@@Use0_1:	;Now modify origional CS:EIP,SS:ESP values and return control
	;to this code via interupt structure to restore stacks.
	;
	test	SystemFlags,1
	jz	@@Use32_2
	mov	eax,offset @@Use0_2
	mov	es:w[ebp+(4+4+2)+(2+2+2)+(0)+2],ax
	mov	es:w[ebp+(4+4+2)+(2+2+2)+(2)+2],_EXCEP
	popm	eax,ebp,ds,es
	retf
	;
@@Use32_2:	mov	eax,offset @@Use0_2
	mov	es:d[ebp+(4+4+2)+(4+4+4)+(0)+2],eax
	mov	es:w[ebp+(4+4+2)+(4+4+4)+(4)+2],_EXCEP
	popm	eax,ebp,ds,es
	db 66h
	retf
	;
@@Use0_2:	;Now return control to exec caller.
	;
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	lss	esp,f[DebuggerESP]
	db 66h
	retf
	;
@@Old:	popm	eax,ebp,ds,es
	assume ds:nothing
	jmp	cs:f[OldExc13]
	assume ds:DGROUP
;
OldExc13	df 0
Exc13Handler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Exc14Handler	proc	near
;
;Exception has been generated.
;
	pushm	eax,ebp,ds,es
	mov	ax,DGROUP		;make our data addresable.
	mov	ds,ax
	cmp	Executing,0
	jz	@@Old
	mov	Executing,0
	mov	ExceptionFlag,14
	mov	ebp,esp		;make stack addresable.
	test	SystemFlags,1
	jz	@@Use32_0
	movzx	ebp,bp		;use 16 bit stack.
@@Use32_0:	;
	;Need a stack alias for DPMI.
	;
	mov	ax,ss
	mov	es,ax
	;
	;Retrieve general registers.
	;
	mov	eax,es:[ebp+4+2+2]
	mov	DebugEAX,eax
	mov	DebugEBX,ebx
	mov	DebugECX,ecx
	mov	DebugEDX,edx
	mov	DebugESI,esi
	mov	DebugEDI,edi
	mov	eax,es:[ebp+2+2]
	mov	DebugEBP,eax
	mov	ax,es:[ebp+2]
	mov	DebugDS,ax
	mov	ax,es:[ebp+0]
	mov	DebugES,ax
	mov	DebugFS,fs
	mov	DebugGS,gs
	;
	;Fetch origional Flags:CS:EIP,SS:ESP
	;
	test	SystemFlags,1
	jz	@@Use32_1
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2+2)+2]
	mov	w[DebugEFL],ax
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2)+2]
	mov	DebugCS,ax
	movzx	eax,es:w[ebp+(4+4+2)+(2+2+2)+(0)+2]
	mov	DebugEIP,eax
	mov	ax,es:[ebp+(4+4+2)+(2+2+2)+(2+2+2)+(2)+2]
	mov	DebugSS,ax
	movzx	eax,es:w[ebp+(4+4+2)+(2+2+2)+(2+2+2)+(0)+2]
	mov	DebugESP,eax
	jmp	@@Use0_1
	;
@@Use32_1:	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(4+4)+2]
	mov	DebugEFL,eax
	mov	ax,es:[ebp+(4+4+2)+(4+4+4)+(4)+2]
	mov	DebugCS,ax
	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(0)+2]
	mov	DebugEIP,eax
	mov	ax,es:[ebp+(4+4+2)+(4+4+4)+(4+4+4)+(4)+2]
	mov	DebugSS,ax
	mov	eax,es:[ebp+(4+4+2)+(4+4+4)+(4+4+4)+(0)+2]
	mov	DebugESP,eax
	;
@@Use0_1:	;Now modify origional CS:EIP,SS:ESP values and return control
	;to this code via interupt structure to restore stacks.
	;
	test	SystemFlags,1
	jz	@@Use32_2
	mov	eax,offset @@Use0_2
	mov	es:w[ebp+(4+4+2)+(2+2+2)+(0)+2],ax
	mov	es:w[ebp+(4+4+2)+(2+2+2)+(2)+2],_EXCEP
	popm	eax,ebp,ds,es
	retf
	;
@@Use32_2:	mov	eax,offset @@Use0_2
	mov	es:d[ebp+(4+4+2)+(4+4+4)+(0)+2],eax
	mov	es:w[ebp+(4+4+2)+(4+4+4)+(4)+2],_EXCEP
	popm	eax,ebp,ds,es
	db 66h
	retf
	;
@@Use0_2:	;Now return control to exec caller.
	;
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	lss	esp,f[DebuggerESP]
	db 66h
	retf
	;
@@Old:	popm	eax,ebp,ds,es
	assume ds:nothing
	jmp	cs:f[OldExc14]
	assume ds:DGROUP
;
OldExc14	df 0
Exc14Handler	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
BordE	proc	near
	pushm	ax,dx
	mov	ah,al
	mov	dx,3dah
	in	al,dx
	mov	dl,0c0h
	mov	al,11h
	out	dx,al
	mov	al,ah
	out	dx,al
	mov	al,20h
	out	dx,al
	popm	ax,dx
	ret
BordE	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Check if hardware break point executed.
;
IsHardBreak	proc	near
	pushad
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	mov	esi,offset HardBreakTable
	mov	ecx,4
@@0:	cmp	HBRK_Flags[esi],0
	jz	@@1
	mov	bx,HBRK_Handle[esi]
	mov	ax,0b02h
	int	31h
	jc	@@1
	test	ax,1
	jnz	@@8
@@1:	add	esi,size HBRK
	dec	ecx
	jnz	@@0
	or	eax,-1
	jmp	@@10
	;
@@8:	xor	eax,eax
	;
@@10:	pop	ds
	popad
	ret
IsHardBreak	endp

_EXCEP	ends

	end main
