;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ The windows database/structure/flags.			     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

Message_Control equ 0
Message_Update	equ	1
Message_Close	equ	2
Message_Up	equ	3
Message_Down	equ	4
Message_Left	equ	5
Message_Right	equ	6

WindowBase	struc
WindowNum	dw ?
WindowFlags	dw ?
WindowFlags2	dw ?
WindowSlot	db ?
WindowX1	db ?
WindowX2	db ?
WindowY1	db ?
WindowY2	db ?
WindowWidth1	db ?
WindowWidth2	db ?
WindowDepth1	db ?
WindowDepth2	db ?
WindowAttrib	db ?
WindowFill	db ?
WindowAttBox	db ?
WindowAttOld	db ?
WindowPrintX	db ?
WindowPrintY	db ?
WindowXOffset	db ?
WindowYOffset	db ?
WindowTab1	dw ?
WindowTab2	dw ?
WindowTab3	dw ?
WindowTab4	dw ?
WindowTab5	dw ?
WindowTab6	dw ?
WindowTab7	dw ?
WindowTab8	dw ?
WindowPriority	dd ?
WindowHandler	dd ?
WindowTitle	dd ?
WindowTFlags	dw ?
WindowBottom	dd ?
WindowBFlags	dw ?
WindowBase	ends

WindowStorage	struc
WStorageOffset	dd ?
WStorageSize	dd ?
WindowStorage	ends

WindowGrab	equ	1
WindowClear	equ	2
WindowBox	equ	4
WindowShad	equ	8
WindowZoom	equ	16
WindowStatic	equ	64
WindowWrap	equ	128
WindowHigh	equ	256
WindowReverse	equ	512
WindowFlash	equ	1024
WindowXOff	equ	2048
WindowYOff	equ	4096
WindowIgnore	equ	8192
WindowRaw	equ	16384
WindowInUse	equ	32768

WindowHidden	equ	1

WindowSpace	equ	2
WindowJLeft	equ	0
WindowJCent	equ	8
WindowJRig	equ	16

WindowTabs	equ	1
WindowGlobal	equ	2


	.code


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³InitWindows	The screen windowing initialisation code.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ EAX	- Address for background storage.		     ³
;³ BX	- Number of database entries to initialise.		     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³Variouse screen descriptors will have been initialised &,		     ³
;³						     ³
;³ AL	- Video mode.				     ³
;³ AH	- Mono flag.				     ³
;³ BL	- Page number.				     ³
;³ CL	- Columns.				     ³
;³ CH	- Rows.				     ³
;³ SI	- Size of a page.				     ³
;³ DI	- Width of a line.				     ³
;³ ES	- Video segment.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
InitWindows	proc	near
	cld
	mov	WStorageSeg,eax	;Set segment for storage.
	mov	MaximumWindows,bx
	mov	WindowDBase,eax	;Set offset to Database.
	mov	cx,size WindowBase
	mov	ax,MaximumWindows
	mul	cx
	movzx	eax,ax
	mov	ecx,eax
	add	eax,WStorageSeg
	mov	WindowsStorage,eax	;Store pointer to storage table.
	mov	edi,WindowDBase
	xor	al,al
	rep	stosb
	mov	cx,size WindowStorage
	mov	ax,MaximumWindows
	mul	cx
	movzx	eax,ax
	mov	ecx,eax
	add	eax,WindowsStorage
	mov	WStorageStack,eax	;reset offset.
	mov	edi,WindowsStorage
	xor	al,al
	rep	stosb
	;
	cmp	MonoSwap,0
	jnz	@@0
	;
	mov	ah,0fh		;Get mode function.
	int	10h		;/
	mov	VideoMode,al		;Store this mode.
	mov	VideoPage,bh		;Store this page.
	mov	VideoSegment,0b8000h	;default to colour segment.
	cmp	al,7		;check for mono.
	jnz	@@1
	;
@@0:	mov	VideoMode,7
	mov	VideoPage,0
	mov	VideoSegment,0b0000h	;use mono segment.
	mov	VideoMono,1		;flag mono mode.
	mov	VideoColumns,80
	mov	VideoWidth,80*2
	mov	VideoRows,25
	mov	VideoPageSize,80*2*25
	jmp	@@2
@@1:	;
	push	es
	mov	ax,40h		;Point to the video bios data
	mov	es,ax		;area to get info.
	xor	bx,bx		;/
	mov	ax,es:[bx+4ah]	;Get the number of columns.
	mov	VideoColumns,ax	;save it.
	mov	w[VideoWidth],ax
	add	w[VideoWidth],ax	;set line width.
	mov	bl,es:[bx+84h]	;get the number of rows.
	xor	bh,bh		;/
	inc	bx		;/
	mov	VideoRows,bx		;save it.
	mul	bx		;get page size
	add	ax,ax		;double up for attributes.
	add	ax,127
	and	ax,0ffffh-127
	mov	VideoPageSize,ax	;set the page size.
	pop	es
	;
@@2:	cmp	VideoMono,0
	jnz	@@3
	;
	pushm	ax,dx,es
	mov	ax,40h
	mov	es,ax
	mov	dx,es:[63h]		;Get 6845 address.
	mov	al,0ch		;write the start address.
	out	dx,al		;/
	inc	dx
	in	al,dx
	dec	dx
	mov	ah,al
	mov	al,0dh		;/
	out	dx,al		;/
	inc	dx
	in	al,dx
	shl	ax,1
	movzx	edi,ax
	add	VideoSegment,edi
	popm	ax,dx,es
@@3:	;
	sys	GetSel
	mov	VideoBase,bx
	mov	ecx,65535
	mov	edx,VideoSegment
	sys	SetSelDet32
	;
	cmp	MousePresent,0
	jz	@@0w
	;
	;Setup mouse event call back address.
	;
	push	es
	push	cs
	pop	es
	mov	edx,offset MouseEvent
	mov	ecx,-1
	mov	ax,0ch
	int	33h
	pop	es
	;
@@0w:	mov	cl,0
	mov	bl,0
	mov	ch,b[VideoColumns]
	mov	bh,b[VideoRows]
	mov	ax,WindowClear	;+WindowGrab
	mov	dl,71h
	mov	esi,0
	mov	ebp,offset WindowMain
	call	OpenWindow
	mov	bp,0
	call	PointWindow
	mov	WindowFill[esi],176
	call	ClearWindow
	;
	mov	ax,VideoColumns
	mov	cl,al
	mov	ax,VideoRows
	mov	ch,al
	mov	al,VideoMode
	mov	ah,VideoMono
	mov	bl,VideoPage
	mov	si,VideoPageSize
	mov	di,w[VideoWidth]
	ret
InitWindows	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³WindowMain	  Main window messages come here.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window handle.				     ³
;³ AX	- Message number.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers except DS,CS,SS corrupted.			     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
WindowMain	proc	near
	pushad
	cmp	ax,Message_Control
	jnz	@@0

	call	GetKeys
	mov	Keys,0
	jz	@@2
	mov	cKeys,al
	cmp	al,61h		; 'a'
	jb	@@1
	cmp	al,7Ah		; 'z'
	ja	@@1
	and	al,5Fh		;convert to upper case.
@@1:	mov	Keys,ax
	cmp	b[Keys],9		;TAB?
	jnz	@@NotTab
	;
	;Need to switch window.
	;
	mov	bp,WinFocusHandle
	call	PointWindow
	movzx	ecx,MaximumWindows
	sub	cx,WinFocusHandle
	mov	bp,WinFocusHandle
	inc	bp
	dec	ecx
	jecxz @@t2
@@t0:	call	PointWindow
	test	WindowFlags[esi],WindowInUse
	jz	@@t1
	test	WindowFlags[esi],WindowBox
	jnz	@@t3
@@t1:	inc	bp
	loop	@@t0
@@t2:	mov	bp,0
	movzx	ecx,WinFocusHandle
	jmp	@@t0
	;
@@t3:	push	WinFocusHandle
	mov	WinFocusHandle,bp
	mov	eax,WindowHandler[esi]
	mov	WindowFocus,eax
	pushm	esi,ebp
	call	WinHighestPrior
	popm	esi,ebp
	inc	eax
	mov	WindowPriority[esi],eax
	call	UpdateWindow
	pop	bp
	call	BoxWindow
	call	ShowWinTitle
	call	ShowWinBottom
	jmp	@@0
@@NotTab:	cmp	w[keys],15*256
	jnz	@@NotBTab
	;
	;Need to switch window.
	;
	mov	bp,WinFocusHandle
	call	PointWindow
	movzx	ecx,WinFocusHandle
	mov	bp,WinFocusHandle
	dec	bp
	js	@@tt2
	jecxz @@tt2
	dec	ecx
	jecxz @@tt2
@@tt0:	call	PointWindow
	test	WindowFlags[esi],WindowInUse
	jz	@@tt1
	test	WindowFlags[esi],WindowBox
	jnz	@@tt3
@@tt1:	dec	bp
	loop	@@tt0
@@tt2:	mov	bp,MaxWindows
	dec	bp
	mov	ecx,MaxWindows
	sub	cx,WinFocusHandle
	jmp	@@tt0
	;
@@tt3:	push	WinFocusHandle
	mov	WinFocusHandle,bp
	mov	eax,WindowHandler[esi]
	mov	WindowFocus,eax
	pushm	esi,ebp
	call	WinHighestPrior
	popm	esi,ebp
	inc	eax
	mov	WindowPriority[esi],eax
	call	UpdateWindow
	pop	bp
	call	BoxWindow
	call	ShowWinTitle
	call	ShowWinBottom
	jmp	@@0
@@NotBTab:	;
@@2:	call	GetMouse
	shr	cx,3
	shr	dx,3
	mov	MouseX,cx
	mov	MouseY,dx
	mov	MouseB,bx
	test	bx,1
	jz	@@3
	;
	;button press so see which window we're in.
	;
	movzx	ecx,MaximumWindows
	mov	bp,0
@@4:	call	PointWindow
	test	WindowFlags[esi],WindowInUse
	jz	@@6
	call	SetWindowLimits
	pushm	cx,bp
	mov	cl,b[MouseX]
	mov	ch,b[MouseY]
	call	WindowChXY
	popm	cx,bp
	call	InWindowLimits
	jnc	@@5
	call	RelWindowLimits
@@6:	inc	bp
	loop	@@4
	jmp	@@3
@@5:	call	RelWindowLimits
	or	bp,bp
	jz	@@3
	;
	;Check which bit of the window we're playing with.
	;
	call	PointWindow
	;
	;Make sure this window is forground first.
	;
	cmp	WindowPriority[esi],-1
	jz	@@OnTop
	pushm	ebx,ecx,edx,esi,ebp
	call	WinHighestPrior
	popm	ebx,ecx,edx,esi,ebp
	cmp	eax,WindowPriority[esi]
	jc	@@OnTop
	jz	@@OnTop
	inc	eax
	mov	WindowPriority[esi],eax
	pushad
	mov	cl,WindowX1[esi]
	mov	ch,WindowY1[esi]
	mov	dl,WindowWidth1[esi]
	mov	dh,WindowDepth1[esi]
	push	esi
	call	WindowRegionUpdate
	pop	esi
	mov	eax,WindowPriority[esi]
	dec	eax
	call	WinFindPrior
	call	PointWindow
	test	WindowFlags[esi],WindowBox
	jz	@@NoBox
	call	BoxWindow
@@NoBox:	call	ShowWinTitle
	call	ShowWinBottom
	popad
@@TopWait:	call	GetMouse
	test	bx,1
	jnz	@@TopWait
	mov	eax,WindowHandler[esi]
	or	eax,eax
	jz	@@0
	mov	WindowFocus,eax
	mov	ax,WindowNum[esi]
	mov	WinFocusHandle,ax
	jmp	@@0
	;
@@OnTop:	test	WindowFlags[esi],WindowBox
	jz	@@3
	mov	cl,WindowX1[esi]
	mov	ch,WindowY1[esi]
	mov	dl,WindowWidth1[esi]
	mov	dh,WindowDepth1[esi]
	mov	al,b[MouseX]
	mov	ah,cl
	add	ah,dl
	dec	ah
	cmp	al,ah
	jnz	@@NotReSize
	mov	al,b[MouseY]
	mov	ah,ch
	add	ah,dh
	dec	ah
	cmp	al,ah
	jnz	@@NotReSize
	mov	_WM_OneOn,0
	mov	_WM_LastX,-1
@@Resize:	pushm	esi,bp
	call	GetMouse
	popm	esi,bp
	shr	cx,3
	shr	dx,3
	cmp	cx,_WM_LastX
	jnz	@@Res0
	cmp	dx,_WM_LastY
	jnz	@@Res0
	test	bx,1
	jnz	@@Resize
@@Res0:	mov	_WM_LastX,cx
	mov	_WM_LastY,dx
	cmp	_WM_OneOn,0
	jz	@@Res1
	mov	_WM_OneOn,0
	pushad
	mov	cl,WindowX1[esi]
	mov	ch,WindowY1[esi]
	mov	dx,_WM_OldX
	mov	ax,_WM_OldY
	mov	dh,al
	mov	edi,offset VLineBuffer
	mov	al,1
	call	VideoBox
	popad
@@Res1:	test	bx,1
	jz	@@ResDone
	sub	cx,MouseX		;Get DX
	sub	dx,MouseY		;Get DY
	add	cl,WindowWidth1[esi]
	add	dl,WindowDepth1[esi]
	movsx	cx,cl
	movsx	dx,dl
	cmp	cx,3
	jc	@@ResClipX
	mov	_WM_OldX,cx
@@ResClipX:	cmp	dx,3
	jc	@@ResClipY
	mov	_WM_OldY,dx
@@ResClipY:	mov	_WM_OneOn,-1
	pushad
	mov	cl,WindowX1[esi]
	mov	ch,WindowY1[esi]
	mov	dx,_WM_OldX
	mov	ax,_WM_OldY
	mov	dh,al
	mov	edi,offset VLineBuffer
	mov	al,0
	mov	ah,32h+8
	call	VideoBox
	popad
	jmp	@@Resize
@@ResDone:	mov	cx,_WM_OldX
	mov	dx,_WM_OldY
	mov	ax,cx
	mov	bl,WindowWidth1[esi]
	sub	al,WindowWidth1[esi]
	mov	WindowWidth1[esi],cl
	add	WindowWidth2[esi],al
	mov	ax,dx
	mov	bh,WindowDepth1[esi]
	sub	al,WindowDepth1[esi]
	mov	WindowDepth1[esi],dl
	add	WindowDepth2[esi],al
	mov	cl,WindowX1[esi]
	mov	ch,WindowY1[esi]
	mov	dl,WindowWidth1[esi]
	mov	dh,WindowDepth1[esi]
	cmp	dl,bl
	jnc	@@Res2
	mov	dl,bl
@@Res2:	cmp	dh,bh
	jnc	@@Res3
	mov	dh,bh
@@Res3:	call	WindowRegionUpdate
	jmp	@@0
@@NotReSize:	;
	mov	al,b[MouseY]
	cmp	al,ch
	jnz	@@NotMove
	mov	_WM_OneOn,0
	mov	_WM_LastX,-1
	pushad
	or	WindowFlags2[esi],WindowHidden
	mov	cl,WindowX1[esi]
	mov	ch,WindowY1[esi]
	mov	dl,WindowWidth1[esi]
	mov	dh,WindowDepth1[esi]
	call	WindowRegionUpdate
	popad
@@Move:	pushm	esi,ebp
	call	GetMouse
	popm	esi,ebp
	shr	cx,3
	shr	dx,3
	cmp	cx,_WM_LastX
	jnz	@@Mov0
	cmp	dx,_WM_LastY
	jnz	@@Mov0
	test	bx,1
	jnz	@@Move
@@Mov0:	mov	_WM_LastX,cx
	mov	_WM_LastY,dx
	cmp	_WM_OneOn,0
	jz	@@Mov2
	mov	_WM_OneOn,0
	pushad
	mov	cx,_WM_OldX
	mov	dx,_WM_OldY
	mov	ch,dl
	mov	dl,WindowWidth1[esi]
	mov	dh,WindowDepth1[esi]
	mov	edi,offset VLineBuffer
	mov	al,1
	call	VideoBox
	popad
@@Mov2:	sub	cx,MouseX		;Get DX
	sub	dx,MouseY		;Get DY
	add	cl,WindowX1[esi]
	add	dl,WindowY1[esi]
	cmp	cl,b[VideoColumns]
	jnc	@@MovClipX
	mov	al,cl
	add	al,WindowWidth1[esi]
	cmp	al,b[VideoColumns]
	jz	@@xxx0
	jnc	@@MovClipX
@@xxx0:	mov	_WM_OldX,cx
@@MovClipX:	cmp	dl,b[VideoRows]
	jnc	@@MovClipY
	mov	al,dl
	add	al,WindowDepth1[esi]
	cmp	al,b[VideoRows]
	jz	@@xxx1
	jnc	@@MovClipY
@@xxx1:	mov	_WM_OldY,dx
@@MovClipY:	test	bx,1
	jz	@@MovDone
	mov	cx,_WM_OldX
	mov	dx,_WM_OldY
	pushad
	mov	ch,dl
	mov	dl,WindowWidth1[esi]
	mov	dh,WindowDepth1[esi]
	mov	edi,offset VLineBuffer
	mov	al,0
	mov	ah,32h+8
	call	VideoBox
	popad
	mov	_WM_OneOn,-1
	jmp	@@Move
@@MovDone:	and	WindowFlags2[esi],65535-WindowHidden
	mov	cx,_WM_OldX
	mov	dx,_WM_OldY
	mov	al,cl
	mov	ah,dl
	sub	al,WindowX1[esi]
	sub	ah,WindowY1[esi]
	mov	WindowX1[esi],cl
	mov	WindowY1[esi],dl
	add	WindowX2[esi],al
	add	WindowY2[esi],ah
	mov	cl,WindowX1[esi]
	mov	ch,WindowY1[esi]
	mov	dl,WindowWidth1[esi]
	mov	dh,WindowDepth1[esi]
	call	WindowRegionUpdate
	jmp	@@0
@@NotMove:	;
@@3:	cmp	WindowFocus,0
	jz	@@0
	cmp	WindowFocus,offset WindowMain
	jz	@@0
	mov	ax,Message_Control
	mov	bp,WinFocusHandle
	call	d[WindowFocus]
@@0:	popad
	ret
WindowMain	endp


VideoBox	proc	near
	pushad
	mov	_VB_Atts,ah
	mov	_VB_Flags,al
	mov	_VB_Store,edi
	call	MouseOFF
	push	w[WindowCurrent]	;save current window stuff.
	mov	w[WindowCurrent],0
	mov	esi,offset WindowSplits
	mov	WindowSplitFlags[esi],0
	mov	WindowSplitX[esi],0
	mov	WindowSplitY[esi],0
	mov	ax,VideoColumns
	mov	WindowSplitW[esi],ax
	mov	ax,VideoRows
	mov	WindowSplitD[esi],ax
	add	esi,size WindowSplitStruc
	mov	WindowSplitFlags[esi],-1
	call	WindowChXY
	mov	ah,_VB_Atts		;get the fill colour.
	movzx	ecx,dl		;set up the width.
	movzx	ebp,dh		;/
	call	WindowAMod		;modify attribute.
	mov	esi,_VB_Store
	;
	dec	ecx
	dec	ecx
	dec	ebp
	dec	ebp
	;
	cld
	pushm	edi,ecx
	mov	al,218		;Ú
	call	InWindowLimits
	jc	@@1
	push	es
	mov	es,VideoBase
	cmp	_VB_Flags,1
	jnz	@@l0
	lodsw
	jmp	@@s0
@@l0:	mov	bx,es:[edi]
	mov	[esi],bx
	add	esi,2
@@s0:	stosw
	pop	es
	jmp	@@2
@@1:	add	edi,2
@@2:	mov	al,196		;Ä
@@3:	call	InWindowLimits
	jc	@@4
	push	es
	mov	es,VideoBase
	cmp	_VB_Flags,1
	jnz	@@l1
	lodsw
	jmp	@@s1
@@l1:	mov	bx,es:[edi]
	mov	[esi],bx
	add	esi,2
@@s1:	stosw
	pop	es
	jmp	@@5
@@4:	add	edi,2
@@5:	loop	@@3
	mov	al,191		;¿
	call	InWindowLimits
	jc	@@6
	push	es
	mov	es,VideoBase
	cmp	_VB_Flags,1
	jnz	@@l2
	lodsw
	jmp	@@s2
@@l2:	mov	bx,es:[edi]
	mov	[esi],bx
	add	esi,2
@@s2:	stosw
	pop	es
@@6:	popm	edi,ecx
	add	edi,VideoWidth
	;
@@0:	push	edi
	mov	al,179		;³
	call	InWindowLimits
	jc	@@7
	push	es
	mov	es,VideoBase
	cmp	_VB_Flags,1
	jnz	@@l3
	lodsw
	jmp	@@s3
@@l3:	mov	bx,es:[edi]
	mov	[esi],bx
	add	esi,2
@@s3:	stosw
	pop	es
	jmp	@@8
@@7:	add	edi,2
@@8:	add	edi,ecx
	add	edi,ecx
	call	InWindowLimits
	jc	@@9
	push	es
	mov	es,VideoBase
	cmp	_VB_Flags,1
	jnz	@@l4
	lodsw
	jmp	@@s4
@@l4:	mov	bx,es:[edi]
	mov	[esi],bx
	add	esi,2
@@s4:	stosw
	pop	es
@@9:	pop	edi
	add	edi,VideoWidth
	dec	bp
	jnz	@@0
	;
	mov	al,192		;À
	call	InWindowLimits
	jc	@@10
	push	es
	mov	es,VideoBase
	cmp	_VB_Flags,1
	jnz	@@l5
	lodsw
	jmp	@@s5
@@l5:	mov	bx,es:[edi]
	mov	[esi],bx
	add	esi,2
@@s5:	stosw
	pop	es
	jmp	@@11
@@10:	add	edi,2
@@11:	mov	al,196		;Ä
@@12:	call	InWindowLimits
	jc	@@13
	push	es
	mov	es,VideoBase
	cmp	_VB_Flags,1
	jnz	@@l6
	lodsw
	jmp	@@s6
@@l6:	mov	bx,es:[edi]
	mov	[esi],bx
	add	esi,2
@@s6:	stosw
	pop	es
	jmp	@@14
@@13:	add	edi,2
@@14:	loop	@@12
	mov	al,217		;Ù
	call	InWindowLimits
	jc	@@15
	push	es
	mov	es,VideoBase
	cmp	_VB_Flags,1
	jnz	@@l7
	lodsw
	jmp	@@s7
@@l7:	mov	bx,es:[edi]
	mov	[esi],bx
	add	esi,2
@@s7:	stosw
	pop	es
@@15:	;
	pop	WindowCurrent
	mov	bp,WindowCurrent
	call	CalcWindowSplits
	call	MouseON
	popad
	ret
VideoBox	endp


WinHighestPrior proc near
	mov	bp,0
	movzx	ecx,MaximumWindows
	xor	eax,eax
@@0:	call	PointWindow
	test	WindowFlags[esi],WindowInUse
	jz	@@1
	cmp	WindowPriority[esi],-1
	jz	@@1
	cmp	eax,WindowPriority[esi]
	jnc	@@1
	mov	eax,WindowPriority[esi]
@@1:	inc	bp
	loop	@@0
	ret
WinHighestPrior endp


WinFindPrior	proc	near
	pushm	eax,ecx,esi
	mov	bp,0
	movzx	ecx,MaximumWindows
@@0:	call	PointWindow
	test	WindowFlags[esi],WindowInUse
	jz	@@1
	cmp	eax,WindowPriority[esi]
	jz	@@2
@@1:	inc	bp
	loop	@@0
	stc
	jmp	@@Done
@@2:	clc
@@Done:	popm	eax,ecx,esi
	ret
WinFindPrior	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³WindowPopup	Popup a window, display message & wait for a key.	     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ EBX	- Message.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers except DS,CS,SS corrupted.			     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
WindowPopup	proc	near
	pushad
	mov	esi,ebx
	mov	_WPU_Attribs,0
	xor	edx,edx		;Max width so far.
	xor	ecx,ecx		;This width.
	xor	ebp,ebp		;depth.
@@0:	lodsb
	cmp	al,0
	jz	@@4
	cmp	al,1
	jz	@@2
	cmp	al,2
	jz	@@2
	cmp	al,3
	jz	@@2
	cmp	al,4
	jz	@@2
	cmp	al,13
	jz	@@13
	inc	ecx
	jmp	@@2
@@13:	inc	esi		;skip 10
	inc	ebp
	cmp	ecx,edx		;biggest width?
	jc	@@3
	mov	edx,ecx
	jmp	@@3
@@1:	inc	esi
	jmp	@@3
@@2:	add	esi,_WPU_Attribs
@@3:	jmp	@@0
	;
@@4:	push	ebx
	add	edx,2+1		;border
	add	ebp,2+1		;border
	mov	cl,ScreenWidth
	shr	cl,1
	mov	al,dl
	shr	al,1
	sub	cl,al
	mov	bl,ScreenDepth
	shr	bl,1
	mov	ax,bp
	shr	al,1
	sub	bl,al
	mov	ch,dl
	mov	ax,bp
	mov	bh,al
	mov	ax,WindowClear+WindowBox+WindowShad
	mov	dh,7fh
	mov	dl,7ch
	mov	di,0001h
	mov	esi,0
	mov	ebp,esi
	call	OpenWindow
	pop	ebx
	call	PrintWindow
	push	ebp
@@5:	call	GetKeys
	jz	@@5
	pop	ebp
	call	CloseWindow
	;
	popad
	ret
WindowPopup	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³OpenWindow	  Initialise an individual window code.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ AX	- Option to open the window with.(bit significant)	     ³
;³ 						     ³
;³           0 - Grab area underneath the window.	(WindowGrab)	     ³
;³           1 - Clear window to set attribute.	(WindowClear)     ³
;³           2 - Box window.(Left,Right,Top,Bottom)	(WindowBox)	     ³
;³           3 - Shadow the window.		(WindowShad)	     ³
;³           4 - Zoom the window.	(NA)		(WindowZoom)	     ³
;³           6 - Don't allow scrolling.		(WindowStatic)    ³
;³           7 - Word wrap text.			(WindowWrap)	     ³
;³           8	- Highlight switch.		(WindowHigh)	     ³
;³           9 - Inverse switch.			(WindowReverse)   ³
;³          10 - Flash switch.			(WindowFlash)     ³
;³          14 - Ignore control chars.		(WindowRaw)	     ³
;³						     ³
;³ BL	- Y co-ordinate for top.			     ³
;³ BH	- Window Depth.				     ³
;³ CL	- X co-ordinate for left.			     ³
;³ CH	- Window Width.				     ³
;³ DL	- Attribute to clear to.			     ³
;³ DH	- Attribute for box.			     ³
;³ DI	- Attribute for shadow (Low), char for shadow (high)	     ³
;³ESI	- Initial priority. 0 = auto asigned.		     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³ BP	- Window number assigned.			     ³
;³ ESI	- Pointer to window database.			     ³
;³						     ³
;³All registers except DS,CS,SS corrupted.			     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
OpenWindow	proc	near
	mov	_OW_ThisPriority,esi
	mov	_OW_ThisHandler,ebp
	;
	call	FreeWindow		;find a free window entry
	jnz	@@9
	;
	cmp	bp,0
	jz	@@NoBodge
	pushad
	push	esi
	call	WinHighestPrior
	call	WinFindPrior
	call	PointWindow
	mov	eax,WindowPriority[esi]
	test	WindowFlags[esi],WindowBox
	pop	esi
	jz	@@NoBox
	inc	eax
	mov	WindowPriority[esi],eax
	mov	WindowFlags[esi],WindowInUse+WindowBox
	call	BoxWindow
	call	ShowWinTitle
	call	ShowWinBottom
	mov	WindowPriority[esi],0
@@NoBox:	popad
@@NoBodge:	;
	mov	WindowXOffset[esi],0
	mov	WindowFlags2[esi],0
	mov	WindowTitle[esi],0
	mov	WindowTFlags[esi],0
	mov	WindowBottom[esi],0
	mov	WindowBFlags[esi],0
	mov	WindowFill[esi],0
	or	ax,WindowInUse	;flag window in use.
	mov	[esi].WindowFlags,ax	;store window setup.
	mov	[esi].WindowNum,bp	;store the window number
	mov	[esi].WindowX1,cl	;store co-ordinates.
	mov	[esi].WindowX2,cl	;/
	mov	[esi].WindowY1,bl	;/
	mov	[esi].WindowY2,bl	;/
	mov	[esi].WindowWidth1,ch	;Setup width depth.
	mov	[esi].WindowWidth2,ch	;/
	mov	[esi].WindowDepth1,bh	;/
	mov	[esi].WindowDepth2,bh	;/
	mov	[esi].WindowAttrib,dl	;Set clearing attribute.
	mov	[esi].WindowAttBox,dh	;Set box attribute.
	mov	[esi].WindowPrintX,0	;Initialise print position.
	mov	[esi].WindowPrintY,0	;/
	mov	[esi].WindowXOffset,0	;reset x,y offsets.
	mov	[esi].WindowYOffset,0	;reset x,y offsets.
	;
	mov	ax,WindowTabG1	;set tab positions.
	mov	[esi].WindowTab1,ax	;/
	mov	ax,WindowTabG2	;/
	mov	[esi].WindowTab2,ax	;/
	mov	ax,WindowTabG3	;/
	mov	[esi].WindowTab3,ax	;/
	mov	ax,WindowTabG4	;/
	mov	[esi].WindowTab4,ax	;/
	mov	ax,WindowTabG5	;/
	mov	[esi].WindowTab5,ax	;/
	mov	ax,WindowTabG6	;/
	mov	[esi].WindowTab6,ax	;/
	mov	ax,WindowTabG7	;/
	mov	[esi].WindowTab7,ax	;/
	mov	ax,WindowTabG8	;/
	mov	[esi].WindowTab8,ax	;/
	;
	mov	eax,_OW_ThisHandler
	mov	WindowHandler[esi],eax
	mov	WindowFocus,eax
	mov	ax,WindowNum[esi]
	mov	WinFocusHandle,ax
@@7:	;
	pushm	ebx,ecx,edx,esi,edi,ebp
	call	WinHighestPrior
	popm	ebx,ecx,edx,esi,edi,ebp
	inc	eax
	or	bp,bp
	jnz	@@3
	mov	eax,WindowPriorityCount
	inc	WindowPriorityCount
@@3:	mov	[esi].WindowPriority,eax
	;
@@6:	pushm	edi		;/
	mov	cl,[esi].WindowX1	;attribute at window top left.
	mov	ch,[esi].WindowY1	;/
	call	WindowChXY		;/
	mov	al,1[edi]		;/
	popm	edi		;/
	mov	[esi].WindowAttOld,al	;/
	;
@@0:	mov	ax,[esi].WindowFlags
	test	ax,WindowBox		;Boxing the window?
	jz	@@1
	inc	[esi].WindowX2
	inc	[esi].WindowY2
	sub	[esi].WindowWidth2,2
	sub	[esi].WindowDepth2,2
	;
@@1:	test	ax,WindowShad	;Shadow on window?
	jz	@@2
	dec	[esi].WindowWidth2
	dec	[esi].WindowDepth2
	;
@@2:	mov	WindowSlot[esi],-1
	mov	bp,[esi].WindowNum	;Get the window number
	pushad
	call	UpdateWindow
	popad
	;
	mov	cl,[esi].WindowPrintX	;get print position.
	mov	ch,[esi].WindowPrintY
	add	cl,[esi].WindowX2	;add window position.
	add	ch,[esi].WindowY2
	mov	WindowPlastX,cl
	mov	WindowPlastY,ch	;set last print position.
	;
	xor	ax,ax
	ret
@@9:	mov	ax,-1
	or	ax,ax
	ret
OpenWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³MessageWindow   Send a message to a window.			     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³ AX 	- Message.				     ³
;³ 						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
MessageWindow	proc	near
	push	esi
	call	PointWindow
	push	WindowHandler[esi]
	pop	_MW_CallAdd
	pop	esi
	cmp	_MW_CallAdd,0
	jz	@@0
	call	_MW_CallAdd
@@0:	ret
MessageWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³UpdateWindow	  Display the window.			     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³ 						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
UpdateWindow	proc	near
	push	esi
	call	PointWindow
	test	[esi].WindowFlags,WindowGrab
	jz	@@3
	call	GrabWindow		;Grab the background.
@@3:	test	[esi].WindowFlags,WindowClear
	jz	@@4
	call	ClearWindow		;Clear the window.
@@4:	test	[esi].WindowFlags,WindowBox
	jz	@@5
	call	BoxWindow		;Box the window.
@@5:	call	ShowWinTitle
	call	ShowWinBottom
	test	[esi].WindowFlags,WindowShad
	jz	@@8
	call	ShadowWindow		;Shadow the window.
@@8:	pop	esi
	pushad
	mov	ax,Message_Update
	call	MessageWindow
	popad
	ret
UpdateWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ExtraWindow	  Allows setting of some window variables.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³ AX	- Options to update the window with.(bit significant)	     ³
;³ 						     ³
;³ 						     ³
;³           1 - Set TAB positions.	(WindowTabs)		     ³
;³						     ³
;³ For TAB's:-						     ³
;³						     ³
;³           3 - Make tabs global.	(WindowGlobal)	     ³
;³						     ³
;³ BX	- TAB position 1				     ³
;³ CX	- TAB position 2				     ³
;³ DX	- TAB position 4				     ³
;³ SI	- TAB position 5				     ³
;³						     ³
;³ For Flags:-						     ³
;³ 						     ³
;³           6 - Don't allow scrolling.	(WindowStatic)	     ³
;³           7 - Word wrap text.		(WindowWrap)		     ³
;³           8	- Highlight switch.	(WindowHigh)		     ³
;³           9 - Inverse switch.		(WindowReverse)	     ³
;³          10 - Flash switch.		(WindowFlash)	     ³
;³          11 - X offset switch		(WindowXOff)		     ³
;³          12 - Y offset switch		(WindowYOff)		     ³
;³          13 - Ignore print position check.	(WindowIgnore)	     ³
;³          14 - Ignore control chars.	(WindowRaw)		     ³
;³           						     ³
;³ CL	- X offset value.(If any)			     ³
;³ CH	- Y offset value. "   "			     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
ExtraWindow	proc	near
	pushm	eax,ebx,esi,edi
	mov	di,si		;save tab position 4
	call	PointWindow
	;
	test	ax,WindowTabs	;Tab settings?
	jz	@@0
	mov	[esi].WindowTab1,bx
	mov	[esi].WindowTab2,cx
	mov	[esi].WindowTab3,dx
	mov	[esi].WindowTab4,di
	;
	test	ax,WindowGlobal
	jz	@@9
	mov	WindowTabG1,bx
	mov	WindowTabG2,cx
	mov	WindowTabG3,dx
	mov	WindowTabG4,di
	mov	bp,0
	mov	si,MaximumWindows
@@1:	pushm	bp,si
	call	PointWindow
	mov	[esi].WindowTab1,bx	;force tab settings.
	mov	[esi].WindowTab2,cx	;/
	mov	[esi].WindowTab3,dx	;/
	mov	[esi].WindowTab4,di	;/
	popm	bp,si
	inc	bp
	dec	si
	jnz	@@1
	jmp	@@9
	;
@@0:	and	ax,65535-WindowInUse-WindowClear-WindowBox
	and	ax,65535-WindowShad-WindowZoom-WindowTabs-WindowGlobal
	mov	bx,WindowWrap+WindowHigh+WindowReverse+WindowFlash
	or	bx,WindowXOff+WindowYOff+WindowRaw+WindowStatic
	or	bx,WindowIgnore
	xor	bx,-1
	and	[esi].WindowFlags,bx
	or	[esi].WindowFlags,ax
	mov	[esi].WindowXOffset,cl
	mov	[esi].WindowYOffset,ch
@@9:	popm	eax,ebx,esi,edi
	ret
ExtraWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³TitleWindow	  Print text in top line of window.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ EBX	- String to print.				     ³
;³ BP	- Window Number.				     ³
;³ AX	- Options to use.(bit significant)		     ³
;³ 						     ³
;³           1 - Space either side.		(WindowSpace)     ³
;³           2 - Box window.(Left,Right,Top,Bottom)	(WindowBox)	     ³
;³         4&5 - Justification mode.			     ³
;³ 		|				     ³
;³	  00 - Left.			(WindowJLeft)     ³
;³ 	  01 - Centered.			(WindowJCent)     ³
;³ 	  10 - Right.			(WindowJRig)	     ³
;³ 						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
TitleWindow	proc	near
	pushm	eax,ebx,ecx,edx,esi,edi,ebp
	mov	di,ax		;preserve options.
	call	PointWindow
	mov	WindowTitle[esi],ebx
	mov	WindowTFlags[esi],di
	popm	eax,ebx,ecx,edx,esi,edi,ebp
	call	ShowWinTitle
	call	ShowWinBottom
	ret
TitleWindow	endp


ShowWinTitle	proc	near
	pushm	eax,ebx,ecx,edx,esi,edi,ebp
	call	PointWindow
	mov	ebx,WindowTitle[esi]
	or	ebx,ebx
	jz	@@99
	;
	mov	di,WindowTFlags[esi]
	mov	ax,[esi].WindowFlags	;save flags.
	mov	cl,[esi].WindowPrintX	;save windows print position.
	mov	ch,[esi].WindowPrintY
	mov	dl,[esi].WindowXOffset
	mov	dh,[esi].WindowYOffset
	mov	[esi].WindowXOffset,0
	mov	[esi].WindowYOffset,0
	pushm	ax,cx,dx
	mov	al,WindowPLastX	;save last print position.
	mov	ah,WindowPLastY
	push	ax
	mov	al,[esi].WindowAttrib
	push	ax
	;
	mov	ebp,offset _SWT_Buffer
	cmp	b[ebx],1
	jnz	@@NoAtts
	mov	al,[ebx]
	mov	ds:[ebp],al
	inc	ebx
	inc	ebp
	mov	al,[ebx]
	mov	ds:[ebp],al
	mov	_SWT_Type,0
	pushad
	pushm	esi,ebp
	call	WinHighestPrior
	popm	esi,ebp
	cmp	eax,WindowPriority[esi]
	jz	@@AtOK
	and	ds:b[ebp],0f0h
	mov	_SWT_Type,1
@@AtOK:	popad
	inc	ebx
	inc	ebp
@@NoAtts:	;
	xor	dx,dx
	test	di,WindowBox
	jz	@@0
	mov	ds:b[ebp],180	;´
	cmp	_SWT_Type,0
	jnz	@@Lok
	mov	ds:b[ebp],181
@@Lok:	inc	ebp
	inc	dx
@@0:	test	di,WindowSpace
	jz	@@1
	mov	ds:b[ebp],' '	;
	inc	ebp
	inc	dx
	;
@@1:	push	ebx
@@2:	mov	al,[ebx]
	inc	ebx
	or	al,al
	jz	@@3
	mov	ds:[ebp],al
	inc	ebp
	cmp	al,16
	jc	@@2		;don't count control chars.
	inc	dx
	jmp	@@2
	;
@@3:	pop	ebx
	test	di,WindowSpace
	jz	@@4
	mov	ds:b[ebp],' '	;
	inc	dx
	inc	ebp
@@4:	test	di,WindowBox
	jz	@@5
	mov	ds:b[ebp],195	;Ã
	cmp	_SWT_Type,0
	jnz	@@Rok
	mov	ds:b[ebp],198
@@Rok:	inc	ebp
	inc	dx
@@5:	mov	ds:b[ebp],0		;terminate the string.
	;
	mov	ax,di
	and	ax,WindowJCent+WindowJRig	;isolate justification mode.
	shr	ax,1
	shr	ax,1
	shr	ax,1		;down the bottom.
	;
	or	ax,ax		;left justify?
	jnz	@@6
	xor	cx,cx		;top left.
	jmp	@@8
	;
@@6:	dec	ax		;central justification?
	jnz	@@7
	mov	cl,[esi].WindowWidth2
	shr	cl,1		;width /2
	shr	dl,1
	sub	cl,dl
	xor	ch,ch
	jmp	@@8
	;
@@7:	dec	ax		;right justify?
	jnz	@@8
	mov	cl,[esi].WindowWidth2	;window width.
	sub	cl,dl		;reduce by width of text.
	;
@@8:	test	[esi].WindowFlags,WindowBox
	jz	@@9
	dec	ch
@@9:	;
	mov	bp,[esi].WindowNum	;get window number again.
	call	LocateWindow		;set our position.
	mov	ax,WindowIgnore
	call	ExtraWindow		;turn checking off.
	;
	mov	ebx,offset _SWT_Buffer
	call	PrintWindow
	;
	pop	ax
	mov	[esi].WindowAttrib,al
	pop	ax
	mov	WindowPLastX,al	;restore last print position.
	mov	WindowPLastY,ah
	popm	ax,cx,dx
	mov	[esi].WindowXOffset,dl
	mov	[esi].WindowYOffset,dh
	mov	[esi].WindowPrintX,cl	;restore windows print position.
	mov	[esi].WindowPrintY,ch
	mov	[esi].WindowFlags,ax	;restore flags.
@@99:	popm	eax,ebx,ecx,edx,esi,edi,ebp
	ret
ShowWinTitle	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³BottomWindow	  Print text in bottom line of window.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ EBX	- String to print.				     ³
;³ BP	- Window Number.				     ³
;³ AX	- Options to use.(bit significant)		     ³
;³ 						     ³
;³           1 - Space either side.		(WindowSpace)     ³
;³           2 - Box window.(Left,Right,Top,Bottom)	(WindowBox)	     ³
;³         4&5 - Justification mode.			     ³
;³ 		|				     ³
;³	  00 - Left.			(WindowJLeft)     ³
;³ 	  01 - Centered.			(WindowJCent)     ³
;³ 	  10 - Right.			(WindowJRig)	     ³
;³ 						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
BottomWindow	proc	near
	pushm	eax,ebx,ecx,edx,esi,edi,ebp
	mov	di,ax		;preserve options.
	call	PointWindow
	mov	WindowBottom[esi],ebx
	mov	WindowBFlags[esi],di
	popm	eax,ebx,ecx,edx,esi,edi,ebp
	call	ShowWinBottom
	ret
BottomWindow	endp


ShowWinBottom	proc	near
	pushm	eax,ebx,ecx,edx,esi,edi,ebp
	call	PointWindow
	mov	ebx,WindowBottom[esi]
	or	ebx,ebx
	jz	@@99
	;
	mov	di,WindowBFlags[esi]
	mov	ax,[esi].WindowFlags	;save flags.
	mov	cl,[esi].WindowPrintX	;save windows print position.
	mov	ch,[esi].WindowPrintY
	mov	dl,[esi].WindowXOffset
	mov	dh,[esi].WindowYOffset
	mov	[esi].WindowXOffset,0
	mov	[esi].WindowYOffset,0
	pushm	ax,cx,dx
	mov	al,WindowPLastX	;save last print position.
	mov	ah,WindowPLastY
	push	ax
	mov	al,[esi].WindowAttrib
	push	ax
	;
	mov	ebp,offset _SWB_Buffer
	cmp	b[ebx],1
	jnz	@@NoAtts
	mov	al,[ebx]
	mov	ds:[ebp],al
	inc	ebx
	inc	ebp
	mov	al,[ebx]
	mov	ds:[ebp],al
	mov	_SWB_Type,0
	pushad
	pushm	esi,ebp
	call	WinHighestPrior
	popm	esi,ebp
	cmp	eax,WindowPriority[esi]
	jz	@@AtOK
	and	ds:b[ebp],0f0h
	mov	_SWB_Type,1
@@AtOK:	popad
	inc	ebx
	inc	ebp
@@NoAtts:	;
	xor	dx,dx
	test	di,WindowBox
	jz	@@0
	mov	ds:b[ebp],180	;´
	cmp	_SWB_Type,0
	jnz	@@Lok
	mov	ds:b[ebp],181
@@Lok:	inc	ebp
	inc	dx
@@0:	test	di,WindowSpace
	jz	@@1
	mov	ds:b[ebp],' '	;
	inc	ebp
	inc	dx
	;
@@1:	push	ebx
@@2:	mov	al,[ebx]
	inc	ebx
	or	al,al
	jz	@@3
	mov	ds:[ebp],al
	inc	ebp
	cmp	al,16
	jc	@@2		;don't count control chars.
	inc	dx
	jmp	@@2
	;
@@3:	pop	ebx
	test	di,WindowSpace
	jz	@@4
	mov	ds:b[ebp],' '	;
	inc	dx
	inc	ebp
@@4:	test	di,WindowBox
	jz	@@5
	mov	ds:b[ebp],195	;Ã
	cmp	_SWB_Type,0
	jnz	@@Rok
	mov	ds:b[ebp],198
@@Rok:	inc	ebp
	inc	dx
@@5:	mov	ds:b[ebp],0		;terminate the string.
	;
	mov	ax,di
	and	ax,WindowJCent+WindowJRig	;isolate justification mode.
	shr	ax,1
	shr	ax,1
	shr	ax,1		;down the bottom.
	;
	or	ax,ax		;left justify?
	jnz	@@6
	xor	cx,cx		;top left.
	jmp	@@8
	;
@@6:	dec	ax		;central justification?
	jnz	@@7
	mov	cl,[esi].WindowWidth2
	shr	cl,1		;width /2
	shr	dl,1
	sub	cl,dl
	xor	ch,ch
	jmp	@@8
	;
@@7:	dec	ax		;right justify?
	jnz	@@8
	mov	cl,[esi].WindowWidth2	;window width.
	sub	cl,dl		;reduce by width of text.
	;
@@8:	test	[esi].WindowFlags,WindowBox
	jz	@@9
	dec	ch
@@9:	;
	mov	bp,[esi].WindowNum	;get window number again.
	add	ch,WindowDepth2[esi]
	inc	ch
	call	LocateWindow		;set our position.
	mov	ax,WindowIgnore
	call	ExtraWindow		;turn checking off.
	;
	mov	ebx,offset _SWB_Buffer
	call	PrintWindow
	;
	pop	ax
	mov	[esi].WindowAttrib,al
	pop	ax
	mov	WindowPLastX,al	;restore last print position.
	mov	WindowPLastY,ah
	popm	ax,cx,dx
	mov	[esi].WindowXOffset,dl
	mov	[esi].WindowYOffset,dh
	mov	[esi].WindowPrintX,cl	;restore windows print position.
	mov	[esi].WindowPrintY,ch
	mov	[esi].WindowFlags,ax	;restore flags.
@@99:	popm	eax,ebx,ecx,edx,esi,edi,ebp
	ret
ShowWinBottom	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³BarWindow	Reverses the attributes of the line specified.	     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ AX	- Mode. 0-normal,1-Force colour, AH=colour.		     ³
;³ BP	- Window Number.				     ³
;³ CH	- Line to highlight.			     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
BarWindow	proc	near
	call	MouseOFF
	pushm	ecx,esi,edi,ebp,edx
	call	SetWindowLimits
	push	eax
	call	PointWindow		;point to this database.
	cmp	al,3
	jz	@@is2
	cmp	al,2
	jnz	@@not2
@@is2:	add	cl,[esi].WindowX2
	jmp	@@done2
@@not2:	mov	cl,[esi].WindowX2
@@done2:	add	ch,[esi].WindowY2
	mov	dl,WindowY2[esi]
	pop	eax
	cmp	ch,dl
	jl	@@Nope
	add	dl,WindowDepth2[esi]
	cmp	ch,dl
	jge	@@Nope
	push	eax
	call	WindowChXY		;get a screen address.
	movzx	ecx,[esi].WindowWidth2	;setup width to scan.
	inc	edi
	pop	eax
	;
	cmp	al,3
	jz	@@m0
	cmp	al,2
	jnz	@@0
@@m0:	mov	ecx,1
	;
@@0:	call	InWindowLimits
	jc	@@200
	;
	push	es
	mov	es,VideoBase
	cmp	al,3
	jnz	@@m1
	mov	ecx,1
	cmp	VideoMono,0
	jz	@@1
	mov	es:b[edi],07h
	jmp	@@2
	;
@@m1:	cmp	al,2
	jnz	@@3
	mov	ecx,1
	cmp	VideoMono,0
	jz	@@1
	mov	es:b[edi],070h
	jmp	@@2
@@3:	cmp	al,0
	jz	@@1
	mov	es:b[edi],ah
	jmp	@@2
	;
@@1:	mov	ah,es:[edi]
	rol	ah,4
	mov	es:[edi],ah
@@2:	pop	es
@@200:	add	edi,2
	loop	@@0
	;
@@Nope:	call	RelWindowLimits
	popm	ecx,esi,edi,ebp,edx
	call	MouseON
	ret
BarWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³PrintWindow	Prints text in a window. If the text won't fit width ways, it  ³
;³	will be cut short. If the print position goes past the bottom  ³
;³	or the top of the window then the window display will be       ³
;³	scrolled till it fits.			     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³ EBX	- String to print.				     ³
;³						     ³
;³						     ³
;³ A 1 followed by the desired attribute will change the window pen/paper     ³
;³ print colour until another selection is made.		     ³
;³						     ³
;³ A 2 will toggle the highlight flag on or off. When the highlight flag is on³
;³ all characters following will be printed with there highlight bit reversed ³
;³ until the flag is reset.				     ³
;³						     ³
;³ A 3 will toggle the inverse flag on or off. When the inverse flag is ON,   ³
;³ all characters following will be printed with reversed pen/paper until the ³
;³ flag is reset.					     ³
;³						     ³
;³ A 4 will toggle the flash flag on or off. When the flash flag is ON all    ³
;³ characters following will be printed with there flash bit reversed until   ³
;³ the flag is reset.				     ³
;³						     ³
;³ A 5 will signal attribute info acompanies every character.	     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³AL   - Variouse bits for results indication.			     ³
;³   0 - Vertical scroll was required. (Even with WindowStatic set)	     ³
;³   1 - Horizontal print position past right edge.		     ³
;³						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
PrintWindow	proc	near
	call	MouseOFF
	pushm	eax,ebx,ecx,edx,esi,edi,ebp

	cld		; MED 11/04/95, needed in case application has std'ed

	call	SetWindowLimits
	call	PointWindow		;point to this windows database.
	mov	d[_PW_Attribs],0
@@0:	movzx	ecx,[esi].WindowXOffset
	mov	d[_PW_XCount],ecx
	mov	d[_PW_Tabs],0
	;
	mov	cl,[esi].WindowPrintX
	add	cl,[esi].WindowX2
	mov	ch,[esi].WindowPrintY	;get co-ords for this window.
	add	ch,[esi].WindowY2	;get offset into screen.
	call	WindowChXY		;get screen offset.
@@3:	cmp	d[_PW_Tabs],0	;still doing tabs?
	jz	@@1
	dec	d[_PW_Tabs]
	mov	al," "
	jmp	@@2
	;
@@1:	cmp	b[ebx],0
	jz	@@9
	mov	al,[ebx]
	inc	ebx
	add	ebx,d[_PW_Attribs]
;
;Check for a control char.
;
@@2:	test	[esi].WindowFlags,WindowRaw
	jnz	@@4
	cmp	al,16
	jnc	@@4
	movzx	eax,al
	jmp	cs:d[@@PControl+eax*4]
;
;Within X offset?
;
@@4:	test	[esi].WindowFlags,WindowXOff
	jz	@@5
	cmp	[_PW_XCount],0
	jz	@@5
	dec	[_PW_XCount]
	jmp	@@3
;
;Within window?
;
@@5:	test	[esi].WindowFlags,WindowIgnore
	jnz	@@6
	;
	mov	cl,[esi].WindowPrintY
	cmp	cl,[esi].WindowDepth2
	jnc	@@13
	mov	cl,[esi].WindowPrintX
	cmp	cl,[esi].WindowWidth2
	jnc	@@13
@@6:	;
	mov	ah,[esi].WindowAttrib	;get the attribute.
	cmp	d[_PW_Attribs],0
	jz	@@noAttribsH
	mov	ah,[ebx+1]
@@NoAttribsH:	test	[esi].WindowFlags,WindowReverse
	jz	@@10
	mov	dh,ah
	and	ah,077h		;turn flash/highlight OFF.
	and	dh,88h		;isolate flash & highlight.
	ror	ah,1		;swap pen & paper.
	ror	ah,1
	ror	ah,1
	ror	ah,1
	or	ah,dh		;put flash/highlight back.
@@10:	test	[esi].WindowFlags,WindowHigh
	jz	@@11
	xor	ah,08h		;toggle bright bit.
@@11:	test	[esi].WindowFlags,WindowFlash
	jz	@@12
	xor	ah,80h		;toggle flash bit.
@@12:	call	InWindowLimits
	jc	@@13
	push	es
	mov	es,VideoBase
	stosw
	pop	es
	sub	edi,2
@@13:	add	edi,2
	inc	[esi].WindowPrintX
	cmp	[esi].WindowPrintX,255
	jnz	@@3
	dec	[esi].WindowPrintX	;make sure its not TO big.
	jmp	@@0		;keep going.
;
@@9:	mov	cl,[esi].WindowPrintX	;get print position.
	mov	ch,[esi].WindowPrintY
	add	cl,[esi].WindowX2	;add window position.
	add	ch,[esi].WindowY2
	mov	WindowPlastX,cl
	mov	WindowPlastY,ch	;set last print position.
	;
	call	RelWindowLimits
	popm	eax,ebx,ecx,edx,esi,edi,ebp
	call	MouseON
	ret
@@PControl:	;
	dd @@Dummy		;
	dd @@101		;attribute change.
	dd @@102		;Highlight toggle.
	dd @@103		;Reverse toggle.
	dd @@104		;flash toggle.
	dd @@105		;
	dd @@Dummy		;
	dd @@Dummy		;
	dd @@Dummy		;
	dd @@109		;Tab control
	dd @@110		;Line feed.
	dd @@Dummy		;
	dd @@Dummy		;
	dd @@113		;carriage return.
	dd @@Dummy		;
	dd @@Dummy
	;
@@Dummy:	jmp	@@3
	;
@@101:	dec	ebx
	sub	ebx,d[_PW_Attribs]
	mov	al,[ebx+1]		;get new attribute.
	add	ebx,2
	mov	[esi].WindowAttrib,al
	jmp	@@3
	;
@@102:	dec	ebx
	sub	ebx,d[_PW_Attribs]
	inc	ebx
	xor	[esi].WindowFlags,WindowHigh
	jmp	@@3
	;
@@103:	dec	ebx
	sub	ebx,d[_PW_Attribs]
	inc	ebx
	xor	[esi].WindowFlags,WindowReverse
	jmp	@@3
	;
@@104:	dec	ebx
	sub	ebx,d[_PW_Attribs]
	inc	ebx
	xor	[esi].WindowFlags,WindowFlash
	jmp	@@3
	;
@@105:	dec	ebx
	sub	ebx,d[_PW_Attribs]
	inc	ebx
	mov	d[_PW_Attribs],1
	jmp	@@3
	;
@@109:	pushm	eax,ebx,ecx,edx,esi,edi,ebp
	mov	bp,[esi].WindowNum	;get this windows number.
	mov	bl,[esi].WindowPrintX	;get current print position.
	xor	bh,bh		;/
	test	[esi].WindowFlags,WindowXOff
	jz	@@109_1
	movzx	ax,[esi].WindowXOffset
	add	bx,ax
	sub	bx,w[_PW_XCount]
@@109_1:	mov	ax,[esi].WindowTab1	;get first tab position.
	cmp	bx,ax		;reached tab 1?
	jl	@@109_0
	mov	ax,[esi].WindowTab2
	cmp	bx,ax		;reached tab 2?
	jl	@@109_0
	mov	ax,[esi].WindowTab3
	cmp	bx,ax		;reached tab 3?
	jl	@@109_0
	mov	ax,[esi].WindowTab4
	cmp	bx,ax		;reached tab 4?
	jl	@@109_0
	mov	ax,bx
	and	ax,65535-7
	add	ax,8
@@109_0:	mov	cx,ax
	sub	cx,bx		;get number of spaces.
	movzx	ecx,cx
	mov	d[_PW_Tabs],ecx
	popm	eax,ebx,ecx,edx,esi,edi,ebp
	jmp	@@3
	;
@@110:	dec	ebx
	sub	ebx,d[_PW_Attribs]
	inc	ebx
	inc	[esi].WindowPrintY	;move down a line.
	jmp	@@0
	;
@@113:	dec	ebx
	sub	ebx,d[_PW_Attribs]
	mov	[esi].WindowPrintX,0	;reset to left side.
	inc	ebx
	jmp	@@0		;try again.
PrintWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³LocateWindow	Sets the print position for a window.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³ CL	- X co-ordinate.				     ³
;³ CH	- Y co-ordinate.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
LocateWindow	proc	near
	pushm	ecx,esi,ebp
	call	PointWindow
	mov	[esi].WindowPrintX,cl
	mov	[esi].WindowPrintY,ch
	add	cl,[esi].WindowX2
	add	ch,[esi].WindowY2
	mov	WindowPLastX,cl
	mov	WindowPLastY,ch
	popm	ecx,esi,ebp
	ret
LocateWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ScrollWindowU Scrolls a window display up.			     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
ScrollWindowU	proc	near
	call	MouseOFF
	pushm	eax,ecx,esi,edi,ebp
	call	PointWindow
	call	SetWindowLimits
	dec	[esi].WindowPrintY
	mov	cl,[esi].WindowX2	;get co-ords for this window.
	mov	ch,[esi].WindowY2
	call	WindowChXY
	movzx	ecx,[esi].WindowWidth2	;get width.
	mov	al,[esi].WindowDepth2	;get depth.
	xor	ah,ah
	mov	bp,ax
	pushm	esi
	mov	esi,edi
	add	esi,VideoWidth
	cld
	dec	bp
	jz	@@1
@@0:	pushm	ecx,esi,edi
@@2:	call	InWindowLimits
	jc	@@3
	pushm	ds,es
	mov	es,VideoBase
	mov	ds,VideoBase
	movsw
	popm	ds,es
	jmp	@@4
@@3:	add	esi,2
	add	edi,2
@@4:	loop	@@2
	popm	ecx,esi,edi
	add	esi,VideoWidth
	add	edi,VideoWidth
	dec	bp
	jnz	@@0
@@1:	popm	esi
	mov	al,0
	mov	ah,[esi].WindowAttrib
	push	es
	mov	es,VideoBase
	rep	stosw
	pop	es
	call	RelWindowLimits
	popm	eax,ecx,esi,edi,ebp
	call	MouseON
	ret
ScrollWindowU	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ScrollWindowD Scrolls a window display down.			     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
ScrollWindowD	proc	near
	call	MouseOFF
	pushm	eax,ecx,esi,edi,ebp
	call	SetWindowLimits
	call	PointWindow
	inc	[esi].WindowPrintY
	mov	cl,[esi].WindowX2	;get co-ords for this window.
	mov	ch,[esi].WindowY2
	add	ch,[esi].WindowDepth2	;point to bottom line.
	dec	ch
	call	WindowChXY
	movzx	ecx,[esi].WindowWidth2	;get width.
	xor	ch,ch
	mov	al,[esi].WindowDepth2	;get depth.
	xor	ah,ah
	mov	bp,ax
	pushm	esi
	mov	esi,edi
	sub	esi,VideoWidth
	cld
	dec	bp
	jz	@@1
@@0:	pushm	ecx,esi,edi
@@2:	call	InWindowLimits
	jc	@@3
	pushm	ds,es
	mov	es,VideoBase
	mov	ds,VideoBase
	movsw
	popm	ds,es
	jmp	@@4
@@3:	add	esi,2
	add	edi,2
@@4:	loop	@@2
	popm	ecx,esi,edi
	sub	esi,VideoWidth
	sub	edi,VideoWidth
	dec	bp
	jnz	@@0
@@1:	popm	esi
	mov	al,0
	mov	ah,[esi].WindowAttrib
	push	es
	mov	es,VideoBase
	rep	stosw
	pop	es
	popm	eax,ecx,esi,edi,ebp
	call	RelWindowLimits
	call	MouseON
	ret
ScrollWindowD	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³CloseWindow	Closes a window. The windows entry in the database is released ³
;³	for future use. If the background was grabbed then it will be  ³
;³	restored. If not, the area will be cleared to the attribute    ³
;³	that was in the top left corner before the window was placed.  ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
CloseWindow	proc	near
	pushm	eax,esi
	push	bp
	mov	ax,Message_Close
	call	MessageWindow
	pop	bp
	call	PointWindow		;point to this windows entry.
	test	WindowFlags[esi],WindowGrab
	mov	WindowFlags[esi],0	;reset this entry.
	jz	@@8
	cmp	bp,0
	jnz	@@8
	push	bp
	call	RestoreWindow
	pop	bp
	jmp	@@9
	;
@@8:	push	esi
	call	WinHighestPrior
	call	WinFindPrior
	call	PointWindow
	mov	eax,WindowHandler[esi]
	mov	WindowFocus,eax
	mov	ax,WindowNum[esi]
	mov	WinFocusHandle,ax
	test	WindowFlags[esi],WindowBox
	jz	@@10
	call	BoxWindow
	call	ShowWinTitle
	call	ShowWinBottom
@@10:	pop	esi
	mov	cl,WindowX1[esi]
	mov	ch,WindowY1[esi]
	mov	dl,WindowWidth1[esi]
	mov	dh,WindowDepth1[esi]
	call	WindowRegionUpdate
	;
@@9:	popm	eax,esi
	ret
CloseWindow	endp


WindowRegionUpdate proc near
;
;Update a region of the screen.
;
	push	WindowRegionFlag
	pushm	WindowRegionX,WindowRegionY
	pushm	WindowRegionWidth,WindowRegionDepth
	;
	movzx	ax,cl
	mov	w[WindowRegionX],ax
	movzx	ax,ch
	mov	w[WindowRegionY],ax
	movzx	ax,dl
	mov	w[WindowRegionWidth],ax
	movzx	ax,dh
	mov	w[WindowRegionDepth],ax
	mov	WindowRegionFlag,-1
	;
	movzx	ecx,MaximumWindows
	mov	bp,0
@@0:	pushm	ecx,ebp
	call	PointWindow
	test	WindowFlags[esi],WindowInUse
	jz	@@1
	;
	;See if this window overlaps this region at all.
	;
	movzx	ax,WindowX1[esi]
	mov	bx,WindowRegionX
	add	bx,WindowRegionWidth
	cmp	ax,bx		;Left edge beyond right edge?
	jge	@@1
	movzx	cx,WindowWidth1[esi]
	add	ax,cx
	sub	bx,WindowRegionWidth
	cmp	ax,bx		;Right edge below left edge?
	jle	@@1
	movzx	ax,WindowY1[esi]
	mov	bx,WindowRegionY
	add	bx,WindowRegionDepth
	cmp	ax,bx		;Top edge beyond bottom edge?
	jge	@@1
	movzx	cx,WindowDepth1[esi]
	add	ax,cx
	sub	bx,WindowRegionDepth
	cmp	ax,bx		;Bottom edge below Top edge?
	jle	@@1
	;
	call	UpdateWindow
	;
@@1:	popm	ecx,ebp
	inc	ebp
	loop	@@0
	;
	popm	WindowRegionWidth,WindowRegionDepth
	popm	WindowRegionX,WindowRegionY
	pop	WindowRegionFlag
	ret
WindowRegionUpdate endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³CloseWindows	Closes all windows.			     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ No entry conditions.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
CloseWindows	proc	near
	pushm	esi,ebp
	mov	bp,0
	call	PointWindow		;point to this windows entry.
	mov	WindowFill[esi],0
	mov	bp,MaximumWindows	;shut them down in reverse order.
	dec	bp
@@0:	call	PointWindow		;point to this windows entry.
	test	[esi].WindowFlags,WindowInUse
	jz	@@1
	call	CloseWindow
@@1:	dec	bp
	jns	@@0
	popm	esi,ebp
	ret
CloseWindows	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ClearWindow	Clears a window.				     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
ClearWindow	proc	near
	call	MouseOFF
	pushm	eax,ecx,esi,edi,ebp
	call	SetWindowLimits
	call	PointWindow		;point to this windows entry.
	;
	mov	cl,[esi].WindowX2	;set up the co-ords.
	mov	ch,[esi].WindowY2	;/
	call	WindowChXY
	movzx	ecx,[esi].WindowWidth2	;set up the width.
	mov	al,[esi].WindowDepth2	;set up the depth.
	xor	ah,ah		;/
	mov	bp,ax		;/
	mov	ah,[esi].WindowAttrib	;get the fill colour.
	mov	al,WindowFill[esi]	;and fill with 0's.
	;
	cld
@@0:	pushm	ecx,edi
@@1:	call	InWindowLimits
	jc	@@2
	push	es
	mov	es,VideoBase
	stosw			;clear this line.
	pop	es
	jmp	@@3
@@2:	add	edi,2
@@3:	loop	@@1
	popm	ecx,edi
	add	edi,VideoWidth	;move down a line.
	dec	bp
	jnz	@@0		;do depth.
	;
	mov	[esi].WindowPrintX,0	;Reset printing co-ordinates.
	mov	[esi].WindowPrintY,0
	;
	call	RelWindowLimits
	popm	eax,ecx,esi,edi,ebp
	call	MouseON
	ret
ClearWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³FillWindow	Fills a window.				     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³ AL	- Fill character.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
FillWindow	proc	near
	call	MouseOFF
	pushm	eax,ecx,edi,ebp
	call	SetWindowLimits
	call	PointWindow		;point to this windows entry.
	;
	push	ax
	mov	cl,[esi].WindowX2	;set up the co-ords.
	mov	ch,[esi].WindowY2	;/
	call	WindowChXY
	movzx	ecx,[esi].WindowWidth2	;set up the width.
	mov	al,[esi].WindowDepth2	;set up the depth.
	xor	ah,ah		;/
	mov	bp,ax		;/
	pop	ax		;get fill character.
	mov	ah,[esi].WindowAttrib	;get the fill colour.
	call	WindowAMod		;modify attribute.
	;
	cld
@@0:	pushm	ecx,edi
@@1:	call	InWindowLimits
	jc	@@2
	push	es
	mov	es,VideoBase
	stosw			;clear this line.
	pop	es
	jmp	@@3
@@2:	add	edi,2
@@3:	loop	@@1
	popm	ecx,edi
	add	edi,VideoWidth	;move down a line.
	dec	bp
	jnz	@@0		;do depth.
	;
	mov	[esi].WindowPrintX,0	;Reset printing co-ordinates.
	mov	[esi].WindowPrintY,0
	;
	call	RelWindowLimits
	popm	eax,ecx,edi,ebp
	call	MouseON
	ret
FillWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³BoxWindow	Puts a box around a window.			     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
BoxWindow	proc	near
	call	MouseOFF
	pushm	eax,ecx,esi,edi,ebp
	call	SetWindowLimits
	call	PointWindow		;point to this windows entry.
	mov	cl,[esi].WindowX2	;set up the co-ords.
	mov	ch,[esi].WindowY2	;/
	dec	cl
	dec	ch
	call	WindowChXY
	mov	b[_BW_Type+1],0
	cmp	WindowHandler[esi],0
	jnz	@@ok2
	mov	b[_BW_Type+1],1
@@ok2:	movzx	ecx,[esi].WindowWidth2	;set up the width.
	mov	al,[esi].WindowDepth2	;set up the depth.
	xor	ah,ah		;/
	mov	bp,ax		;/
	mov	ah,[esi].WindowAttBox	;get the fill colour.
	xor	al,al		;and fill with 0's.
	call	WindowAMod		;modify attribute.
	;
	pushad
	push	esi
	call	WinHighestPrior
	pop	esi
	mov	b[_BW_Type],0
	cmp	eax,WindowPriority[esi]
	jnz	@@low
	mov	b[_BW_Type],1
@@low:	popad
	cmp	b[_BW_Type],0
	jnz	@@ok
	and	ah,0f0h
@@ok:	;
	cld
	pushm	edi,ecx
	mov	al,218		;Ú
	cmp	b[_BW_Type],0
	jz	@@s0
	mov	al,201
@@s0:	call	InWindowLimits
	jc	@@1
	push	es
	mov	es,VideoBase
	stosw
	pop	es
	jmp	@@2
@@1:	add	edi,2
@@2:	mov	al,196		;Ä
	cmp	b[_BW_Type],0
	jz	@@3
	mov	al,205
@@3:	call	InWindowLimits
	jc	@@4
	push	es
	mov	es,VideoBase
	stosw
	pop	es
	jmp	@@5
@@4:	add	edi,2
@@5:	loop	@@3
	mov	al,191		;¿
	cmp	b[_BW_Type],0
	jz	@@s1
	mov	al,187
@@s1:	call	InWindowLimits
	jc	@@6
	push	es
	mov	es,VideoBase
	stosw
	pop	es
@@6:	popm	edi,ecx
	add	edi,VideoWidth
	;
@@0:	push	edi
	mov	al,179		;³
	cmp	b[_BW_Type],0
	jz	@@s2
	mov	al,186
@@s2:	call	InWindowLimits
	jc	@@7
	push	es
	mov	es,VideoBase
	stosw
	pop	es
	jmp	@@8
@@7:	add	edi,2
@@8:	add	edi,ecx
	add	edi,ecx
	call	InWindowLimits
	jc	@@9
	push	es
	mov	es,VideoBase
	stosw
	pop	es
@@9:	pop	edi
	add	edi,VideoWidth
	dec	bp
	jnz	@@0
	;
	mov	al,192		;À
	cmp	b[_BW_Type],0
	jz	@@s3
	mov	al,200
@@s3:	call	InWindowLimits
	jc	@@10
	push	es
	mov	es,VideoBase
	stosw
	pop	es
	jmp	@@11
@@10:	add	edi,2
@@11:	mov	al,196		;Ä
	cmp	b[_BW_Type],0
	jz	@@12
	mov	al,205
@@12:	call	InWindowLimits
	jc	@@13
	push	es
	mov	es,VideoBase
	stosw
	pop	es
	jmp	@@14
@@13:	add	edi,2
@@14:	loop	@@12
	mov	al,217		;Ù
	cmp	b[_BW_Type],0
	jz	@@s4
	mov	al,241		;ñ
	cmp	b[_BW_Type+1],0
	jz	@@s5
	mov	al,188
@@s5:	;
@@s4:	call	InWindowLimits
	jc	@@15
	push	es
	mov	es,VideoBase
	stosw
	pop	es
@@15:	;
	call	RelWindowLimits
	popm	eax,ecx,esi,edi,ebp
	call	MouseON
	ret
BoxWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ShadowWindow	Shadows a window.				     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
ShadowWindow	proc	near
	call	MouseOFF
	pushm	eax,ecx,esi,edi
	call	SetWindowLimits
	call	PointWindow		;point to this windows entry.
	mov	cl,[esi].WindowX1	;set up the co-ords.
	mov	ch,[esi].WindowY1	;/
	add	cl,[esi].WindowWidth1
	dec	cl
	call	WindowChXY
	movzx	ecx,[esi].WindowDepth1	;set up the depth.
	dec	ecx
	xor	ah,ah
	mov	al,223
	add	edi,VideoWidth
	dec	ecx
	jz	@@1
	xor	al,al
@@0:	push	edi
	call	@@Shadow
	pop	edi
	add	edi,VideoWidth
	loop	@@0
@@1:	;
	mov	cl,[esi].WindowX1	;set up the co-ords.
	mov	ch,[esi].WindowY1	;/
	add	ch,[esi].WindowDepth1
	dec	ch
	inc	cl
	call	WindowChXY
	movzx	ecx,[esi].WindowWidth1	;set up the width.
	dec	ecx
	mov	al,220		;get the fill colour/char.
@@2:	call	@@Shadow
	loop	@@2
	;
	call	RelWindowLimits
	popm	eax,ecx,esi,edi
	call	MouseON
	ret
	;
@@Shadow:	;replace attribs at DI with sensible ones.
	;
	call	InWindowLimits
	jc	@@3
	push	es
	mov	es,VideoBase
	mov	es:b[edi+1],08h
	pop	es
	add	edi,2
	ret
@@3:	add	edi,2
	ret
ShadowWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³GrabWindow	Grabs the screen area under a window.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
GrabWindow	proc	near
	call	MouseOFF
	pushm	eax,ebx,ecx,edx,esi,edi,ebp
	;
	cmp	bp,0
	jnz	@@4
	cmp	WindowSlot[esi],-1
	jnz	@@4
	;
@@3:	call	PointWindow		;point to this database.
	mov	al,[esi].WindowWidth1
	xor	ah,ah
	add	ax,ax
	mov	bl,[esi].WindowDepth1
	xor	bh,bh
	mul	bx		;get storage size required.
	movzx	eax,ax
	xor	bx,bx
	mov	edi,WindowsStorage	;point at storage table.
@@0:	cmp	[edi].WStorageOffset,0	;this entry in use?
	jz	@@1
	add	edi,size WindowStorage	;move to next entry.
	inc	bx		;update counter.
	jmp	@@0
	;
@@1:	mov	[esi].WindowSlot,bl	;store storage slot.
	mov	[edi].WStorageSize,eax	;store size.
	mov	ecx,WStorageStack
	mov	[edi].WStorageOffset,ecx	;set pointer to data.
	add	WStorageStack,eax	;move pointer on.
	push	ecx
	mov	cl,[esi].WindowX1
	mov	ch,[esi].WindowY1
	call	WindowChXY
	movzx	ecx,[esi].WindowWidth1
	mov	al,[esi].WindowDepth1
	xor	ah,ah
	mov	bp,ax
	mov	esi,edi
	pop	edi
	;
	push	ds
	pop	es
	cld
@@2:	push	ds
	mov	ds,VideoBase
	pushm	esi,ecx
	rep	movsw		;copy width.
	popm	esi,ecx
	pop	ds
	add	esi,VideoWidth	;down a line.
	dec	bp
	jnz	@@2		;do depth.
	;
@@4:	popm	eax,ebx,ecx,edx,esi,edi,ebp
	call	MouseON
	ret
GrabWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³RestoreWindow  Restores the screen area under a window.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window Number.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
RestoreWindow	proc	near
	call	MouseOFF
	pushm	eax,ebx,ecx,edx,esi,edi,ebp
	;
	call	PointWindow		;point to this database.
	mov	bl,[esi].WindowSlot
	xor	bh,bh
	mov	ax,size WindowStorage
	mul	bx		;offset to this storage entry.
	mov	ebx,WindowsStorage	;/
	movzx	eax,ax
	add	ebx,eax		;/
	;
	mov	cl,[esi].WindowX1
	mov	ch,[esi].WindowY1
	call	WindowChXY
	movzx	ecx,[esi].WindowWidth1	;get window width.
	mov	al,[esi].WindowDepth1	;get window depth.
	xor	ah,ah
	mov	bp,ax
	;
	cld
	mov	esi,[ebx].WStorageOffset	;data source.
@@0:	push	es
	mov	es,VideoBase
	pushm	edi,ecx
	rep	movsw		;copy line.
	popm	edi,ecx
	pop	es
	add	edi,VideoWidth	;down a line.
	dec	bp
	jnz	@@0		;do depth.
	;
	mov	edi,[ebx].WStorageOffset	;point at current data.
	mov	esi,edi
	mov	eax,[ebx].WStorageSize
	add	esi,eax		;point at next data.
	mov	ecx,esi
	sub	ecx,WStorageStack	;get size to copy.
	or	ecx,ecx
	jz	@@3
	rep	movsb		;copy it all down
	mov	ecx,[ebx].WStorageSize
	mov	eax,[ebx].WStorageOffset
	mov	bp,MaximumWindows
	mov	esi,WindowsStorage	;point at database.
@@1:	cmp	eax,[esi].WStorageOffset
	jle	@@2
	sub	[esi].WStorageOffset,ecx	;update pointer.
@@2:	add	esi,size WindowStorage	;move to next entry.
	dec	bp
	jnz	@@1		;do all entrys.
	;
@@3:	mov	[ebx].WStorageOffset,0	;mark this entry as free.
	mov	ecx,[ebx].WStorageSize
	sub	WStorageStack,ecx
	popm	eax,ebx,ecx,edx,esi,edi,ebp
	call	MouseON
	ret
RestoreWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³SetWindowLimits	Work out limits for this windows display.	     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window number.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All other registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
SetWindowLimits proc near
	pop	d[_SWL_RetAdd]
	push	w[WindowCurrent]	;save current window stuff.
	mov	WindowCurrent,bp
	call	CalcWindowSplits
	push	d[_SWL_RetAdd]
	ret
SetWindowLimits endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³RelWindowLimits	Restore last windows limits.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All other registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
RelWindowLimits proc near
	pop	d[_RWL_RetAdd]
	pop	w[WindowCurrent]
	push	bp
	mov	bp,WindowCurrent
	call	CalcWindowSplits
	pop	bp
	push	d[_RWL_RetAdd]
	ret
RelWindowLimits endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³InWindowLimits	Check if inside window limits.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ EDI	- Video address to write to.			     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³Carry set if not a valid address.			     ³
;³						     ³
;³All other registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
InWindowLimits proc near
	pushad
	mov	bp,WindowCurrent
	call	PointWindow
	test	WindowFlags2[esi],WindowHidden
	jnz	@@9
	;
	mov	eax,edi
	xor	ecx,ecx
	call	WindowChXY
	sub	eax,edi
	mov	ebx,d[VideoWidth]
	xor	edx,edx
	div	ebx
	shr	edx,1
	mov	ecx,edx		;Get the X
	mov	ebx,eax		;Get the Y
	;
	;Check region first.
	;
	cmp	WindowRegionFlag,0
	jz	@@2
	cmp	cx,WindowRegionX
	jl	@@9
	cmp	bx,WindowRegionY
	jl	@@9
	mov	ax,WindowRegionX
	add	ax,WindowRegionWidth
	cmp	cx,ax
	jge	@@9
	mov	ax,WindowRegionY
	add	ax,WindowRegionDepth
	cmp	bx,ax
	jge	@@9
	;
@@2:	;Check inside screen.
	;
	or	cx,cx
	js	@@9
	or	bx,bx
	js	@@9
	cmp	cx,w[VideoColumns]
	jge	@@9
	cmp	bx,w[VideoRows]
	jge	@@9
	;
	mov	esi,offset WindowSplits
@@0:	cmp	WindowSplitFlags[esi],-1
	jz	@@9
	cmp	cx,WindowSplitX[esi]
	jl	@@1
	cmp	bx,WindowSplitY[esi]
	jl	@@1
	mov	ax,WindowSplitX[esi]
	add	ax,WindowSplitW[esi]
	dec	ax
	cmp	ax,cx
	jl	@@1
	mov	ax,WindowSplitY[esi]
	add	ax,WindowSplitD[esi]
	dec	ax
	cmp	ax,bx
	jl	@@1
	jmp	@@8
@@1:	add	esi,size WindowSplitStruc
	jmp	@@0
	;
@@8:	popad
	clc
	ret
	;
@@9:	popad
	stc
	ret
InWindowLimits endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³CalcWindowSplits	Calculate window limits.			     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³BP	- Window handle.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³All other registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
CalcWindowSplits proc near
	pushad
	call	PointWindow
	mov	edi,offset WindowSplits
	mov	WindowSplitFlags[edi],-1
	test	WindowFlags2[esi],WindowHidden
	jnz	@@99
	;
	movzx	ax,WindowX1[esi]
	mov	WindowSplitX[edi],ax
	movzx	ax,WindowY1[esi]
	mov	WindowSplitY[edi],ax
	movzx	ax,WindowWidth1[esi]
	mov	WindowSplitW[edi],ax
	movzx	ax,WindowDepth1[esi]
	mov	WindowSplitD[edi],ax
	mov	WindowSplitFlags[edi],0
	add	edi,size WindowSplitStruc
	mov	WindowSplitFlags[edi],-1
	mov	d[_CWS_SplitPos],edi
	mov	d[_CWS_ThisWindow],esi
	mov	eax,WindowPriority[esi]
	mov	d[_CWS_ThisPriority],eax
	;
	mov	bp,0
	movzx	ecx,MaximumWindows
	mov	edi,offset WindowSplits
	;
@@0:	pushm	ecx,edi,ebp
	call	PointWindow
	mov	ebp,edi
	mov	edi,esi		;List of window structures.
	cmp	edi,d[_CWS_ThisWindow]	;Checking against source?
	jz	@@8		;Dont want to clip against ourselves.
	test	WindowFlags[edi],WindowInUse
	jz	@@8
	test	WindowFlags2[edi],WindowHidden
	jnz	@@8
	;
@@1:	cmp	ds:WindowSplitFlags[ebp],-1	;End of the list?
	jz	@@8
	mov	w[_CWS_Left],0		;Clear offsets.
	mov	w[_CWS_Right],0
	;
	mov	eax,WindowPriority[edi]	;Fetch this windows priority.
	cmp	eax,d[_CWS_ThisPriority]
	jc	@@7
	;
	movzx	cx,WindowX1[edi]
	movzx	bx,WindowY1[edi]
	movzx	dx,WindowWidth1[edi]
	movzx	si,WindowDepth1[edi]
	mov	w[_CWS_XCoord],cx
	mov	w[_CWS_YCoord],bx
	mov	w[_CWS_Width],dx
	mov	w[_CWS_Depth],si
	;
	;Check if coords overlap.
	;
	mov	ax,ds:WindowSplitX[ebp]
	mov	bx,w[_CWS_XCoord]
	add	bx,w[_CWS_Width]
	cmp	ax,bx		;Left edge beyond right edge?
	jge	@@7
	add	ax,ds:WindowSplitW[ebp]
	sub	bx,w[_CWS_Width]
	cmp	ax,bx		;Right edge below left edge?
	jle	@@7
	mov	ax,ds:WindowSplitY[ebp]
	mov	bx,w[_CWS_YCoord]
	add	bx,w[_CWS_Depth]
	cmp	ax,bx		;Top edge beyond bottom edge?
	jge	@@7
	add	ax,ds:WindowSplitD[ebp]
	sub	bx,w[_CWS_Depth]
	cmp	ax,bx		;Bottom edge below Top edge?
	jle	@@7
	;
	;Need to chop it up into possible 4 pieces.
	;
	mov	esi,d[_CWS_SplitPos]	;current next free position.
	;
	mov	ax,w[_CWS_XCoord]	;Get starting X
	sub	ax,ds:WindowSplitX[ebp]	;Get width of left side.
	js	@@NoLeft
	jz	@@NoLeft
	mov	w[_CWS_Left],ax		;Store for Top & Bottom.
	mov	WindowSplitW[esi],ax
	mov	ax,ds:WindowSplitX[ebp]
	mov	WindowSplitX[esi],ax
	mov	ax,ds:WindowSplitY[ebp]
	mov	WindowSplitY[esi],ax
	mov	ax,ds:WindowSplitD[ebp]
	mov	WindowSplitD[esi],ax
	mov	WindowSplitFlags[esi],0
	add	esi,size WindowSplitStruc	;Next entry
	;
@@NoLeft:	mov	ax,ds:WindowSplitX[ebp]	;this one.
	add	ax,ds:WindowSplitW[ebp]
	mov	bx,w[_CWS_XCoord]	;other.
	add	bx,w[_CWS_Width]
	sub	ax,bx
	js	@@NoRight
	jz	@@NoRight
	mov	w[_CWS_Right],ax
	mov	bx,ds:WindowSplitW[ebp]
	sub	bx,ax		;Get width differance.
	mov	WindowSplitW[esi],ax	;set new width.
	mov	ax,ds:WindowSplitX[ebp]
	add	ax,bx
	mov	WindowSplitX[esi],ax
	mov	ax,ds:WindowSplitY[ebp]
	mov	WindowSplitY[esi],ax
	mov	ax,ds:WindowSplitD[ebp]
	mov	WindowSplitD[esi],ax
	mov	WindowSplitFlags[esi],0
	add	esi,size WindowSplitStruc	;Next entry
	;
@@NoRight:	mov	ax,w[_CWS_YCoord]	;Get starting Y
	sub	ax,ds:WindowSplitY[ebp]	;Get depth of top.
	js	@@NoTop
	jz	@@NoTop
	mov	WindowSplitD[esi],ax
	mov	ax,ds:WindowSplitX[ebp]
	add	ax,w[_CWS_Left]
	mov	WindowSplitX[esi],ax
	mov	ax,ds:WindowSplitY[ebp]
	mov	WindowSplitY[esi],ax
	mov	ax,ds:WindowSplitW[ebp]
	sub	ax,w[_CWS_Left]
	sub	ax,w[_CWS_Right]
	mov	WindowSplitW[esi],ax
	mov	WindowSplitFlags[esi],0
	add	esi,size WindowSplitStruc	;Next entry
	;
@@NoTop:	mov	ax,ds:WindowSplitY[ebp]	;this one.
	add	ax,ds:WindowSplitD[ebp]
	mov	bx,w[_CWS_YCoord]	;other.
	add	bx,w[_CWS_Depth]
	sub	ax,bx
	js	@@NoBottom
	jz	@@NoBottom
	mov	bx,ds:WindowSplitD[ebp]
	sub	bx,ax		;Get width differance.
	mov	WindowSplitD[esi],ax	;set new width.
	mov	ax,ds:WindowSplitY[ebp]
	add	ax,bx
	mov	WindowSplitY[esi],ax
	mov	ax,ds:WindowSplitX[ebp]
	add	ax,w[_CWS_Left]
	mov	WindowSplitX[esi],ax
	mov	ax,ds:WindowSplitW[ebp]
	sub	ax,w[_CWS_Left]
	sub	ax,w[_CWS_Right]
	mov	WindowSplitW[esi],ax
	mov	WindowSplitFlags[esi],0
	add	esi,size WindowSplitStruc	;Next entry
	;
@@NoBottom:	mov	WindowSplitFlags[esi],-1	;Terminate table again.
	mov	d[_CWS_SplitPos],esi
	sub	d[_CWS_SplitPos],size WindowSplitStruc	;Account for loss of origional entry.
	;
	push	edi
	mov	edi,ebp		;Source split entry.
	mov	ecx,esi		;Entry after last.
	mov	esi,ebp		;Source split entry.
	add	esi,size WindowSplitStruc	;First entry to copy down.
	sub	ecx,esi		;length of data to copy.
	add	ecx,2		;include terminator.
	shr	ecx,1		;divisible by 2 for speed.
	pushm	ds,es,ds,ds
	popm	ds,es
	cld
	rep	movsw		;Copy it all down.
	popm	ds,es
	pop	edi		;Get window pointer back.
	jmp	@@1		;Check again in the same place.
	;
@@7:	add	ebp,size WindowSplitStruc	;Move to next split entry.
	jmp	@@1
	;
@@8:	popm	ecx,edi,ebp
	inc	bp		;Move to next window entry.
	dec	ecx
	jnz	@@0
	;
@@99:	popad
	ret
CalcWindowSplits endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³WindowAMod	Modify attribute to make it leagal for Mono display.	     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ AH	- Attribute to modify.			     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³ AH	- Modified attribute.			     ³
;³						     ³
;³All other registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
WindowAMod	proc	near
	cmp	VideoMono,0		;any modification required?
	jz	@@9
	test	ah,0f0h		;check paper.
	jz	@@0
	and	ah,6fh		;lose bit 0
@@0:	test	ah,0fh		;check pen.
	jz	@@2
	test	ah,08h		;bright attribute?
	jnz	@@1
	and	ah,0fh
	or	ah,07h		;force white pen.
	jmp	@@2
@@1:	or	ah,0fh		;force bright pen.
@@2:	cmp	ah,77h		;check for white on white.
	jnz	@@9
	and	ah,0fh		;force white on black.
@@9:	ret
WindowAMod	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³WindowChXY	Calculates a screen memory address.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ CL	- X co-ordinate.				     ³
;³ CH	- Y co-ordinate.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³ EDI	- Screen Address.				     ³
;³						     ³
;³All other registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
WindowChXY	proc	near
	pushm	eax,ebx,ecx,edx
	;
	movzx	bx,ch
	mov	ax,w[VideoWidth]	;get line width.
	imul	bx		;offset down the screen.
	shl	edx,16
	mov	dx,ax
	movzx	ebx,cl
	add	ebx,ebx		;double up for attributes.
	add	edx,ebx		;offset accross the screen.
	mov	edi,edx		;add in this offset.
	popm	eax,ebx,ecx,edx
	ret
WindowChXY	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³PointWindow	Point to a windows database.			     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³ BP	- Window number.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³ ESI	- Window Database.				     ³
;³						     ³
;³All other registers preserved.				     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
PointWindow	proc	near
	pushm	eax,edx,ebp
	mov	ax,size WindowBase
	mul	bp
	movzx	eax,ax
	mov	esi,WindowDBase
	add	esi,eax
	popm	eax,edx,ebp
	ret
PointWindow	endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³FreeWindow	Searches for an unused database entry.		     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Entry:-						     ³
;³						     ³
;³No entry conditions.				     ³
;³						     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³On Exit:-						     ³
;³						     ³
;³If Zero then,					     ³
;³						     ³
;³ BP is window number assigned, ESI points to entry.		     ³
;³						     ³
;³else,						     ³
;³						     ³
;³ No entry free.					     ³
;³						     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
FreeWindow	proc	near
	mov	esi,WindowDBase	;Make database addresable.
	xor	bp,bp
@@0:	test	[esi].WindowFlags,WindowInUse
	jz	@@8
	inc	bp
	add	esi,size WindowBase	;move to the next entry.
	cmp	bp,MaximumWindows
	jc	@@0
	jmp	@@9		;no free entry found.
@@8:	push	bp
	xor	bp,bp
	pop	bp
	ret
@@9:	mov	bp,-1
	or	bp,bp
	ret
FreeWindow	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Get some input.
;
;On Entry:-
;
;CL	- X
;CH	- Y
;BX	- Width.
;BP	- Window handle.
;EDI	- Buffer.
;
WindowInput	proc	near
	mov	b[_WI_XPos],cl
	mov	b[_WI_YPos],ch
	mov	w[_WI_Width],bx
	mov	d[_WI_Buffer],edi
	mov	w[_WI_Handle],bp
	mov	b[edi],0
	;
	call	GetCursorPos
	mov	w[_WI_Cursor],dx
	;
	mov	b[_WI_XOff],0
	;
@@print:	mov	bp,w[_WI_Handle]
	mov	cl,b[_WI_XPos]
	mov	ch,b[_WI_YPos]
	call	LocateWindow
	mov	ebx,d[_WI_Buffer]
	call	PrintWindow
	call	PointWindow
	mov	dl,WindowX2[esi]
	mov	dh,WindowY2[esi]
	add	dl,b[_WI_XPos]
	add	dl,b[_WI_XOff]
	add	dh,b[_WI_YPos]
	call	SetCursorPos
	;
@@main:	call	GetKeys
	mov	Keys,0
	jz	@@1
	mov	b[_WI_ASCII],al
	cmp	al,61h		; 'a'
	jb	@@0
	cmp	al,7Ah		; 'z'
	ja	@@0
	and	al,5Fh		;convert to upper case.
@@0:	mov	Keys,ax
@@1:	cmp	b[Keys+1],1		;ESC?
	jz	@@9
	;
	cmp	b[Keys],","
	jz	@@Put
	cmp	b[Keys],'.'
	jz	@@Put
	cmp	b[Keys],'\'
	jz	@@Put
	cmp	b[Keys],'@'
	jc	@@2
	cmp	b[Keys],'Z'+1
	jc	@@Put
@@2:	cmp	b[Keys],'0'
	jc	@@3
	cmp	b[Keys],'9'+1
	jc	@@Put
@@3:	cmp	b[Keys],':'
	jz	@@Put
	cmp	b[Keys],'?'
	jz	@@Put
	cmp	b[Keys],'_'
	jz	@@Put
	cmp	b[Keys],'!'
	jz	@@Put
	cmp	b[Keys],'"'
	jz	@@Put
	cmp	b[Keys],'$'
	jz	@@Put
	cmp	b[Keys],'&'
	jz	@@Put
	cmp	b[Keys],'*'
	jz	@@Put
	cmp	b[Keys],'('
	jz	@@Put
	cmp	b[Keys],')'
	jz	@@Put
	cmp	b[Keys],'-'
	jz	@@Put
	cmp	b[Keys],'+'
	jz	@@Put
	cmp	b[Keys],'<'
	jz	@@Put
	cmp	b[Keys],'>'
	jz	@@Put
	cmp	b[Keys],'/'
	jz	@@Put
	;
	cmp	b[Keys],13
	jz	@@9
	;
	cmp	b[Keys],8
	jnz	@@4
	cmp	b[_WI_XOff],0
	jz	@@main		;ignore it.
	dec	b[_WI_XOff]
	movzx	edi,b[_WI_XOff]
	add	edi,d[_WI_Buffer]
	mov	b[edi],' '
	mov	b[edi+1],0
	jmp	@@print
	;
@@4:	jmp	@@main
	;
@@Put:	movzx	edi,b[_WI_XOff]
	cmp	di,w[_WI_Width]
	jz	@@main		;ignore it.
	add	edi,d[_WI_Buffer]
	inc	b[_WI_XOff]
	mov	al,b[_WI_ASCII]
	mov	[edi],al
	mov	b[edi+1],0
	jmp	@@print
	;
@@9:	mov	dx,w[_WI_Cursor]
	call	SetCursorPos
	mov	ax,Keys
	ret
;
WindowInput	endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Get the current cursor position from the bios.
;
;On Exit:-
;
;DL - X
;DH - Y
;
GetCursorPos	proc	near
	mov	ah,03h		;Get cursor position.
	mov	bh,DisasPage		;/
	int	10h		;/
	ret
GetCursorPos	endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Set the cursor position.
;
;On Entry:-
;
;DL - X
;DH - Y
;
SetCursorPos	proc	near
	mov	ah,02h		;Set cursor position.
	mov	bh,DisasPage		;/
	int	10h		;/
	ret
SetCursorPos	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Move the cursor OFF screen.
;
CursorOFF	proc	near
	mov	dl,0
	mov	dh,127
	call	SetCursorPos
	ret
CursorOFF	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Turn the mouse pointer ON. Will increment the mouse flag byte. The mouse
;pointer is only displayed when this flag is > 0.
;
;On Entry:-
;
;AX	- Flags. (significant if bit set)
;
MouseOn	proc	near
	inc	InMouse
	inc	MouseFlag		;update the counter.
	cmp	MouseFlag,1
	jl	@@1
	jg	@@1
	pushad
	call	MousePOn		;put the mouse on.
	popad
@@1:	dec	InMouse
	ret
MouseOn	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Turn the mouse pointer OFF. Will decrement the mouse flag byte. The mouse
;pointer is only displayed when this flag is > 0.
;
;On Entry:-
;
;AX	- Flags. (significant if bit set)
;
MouseOff	proc	near
	inc	InMouse
	dec	MouseFlag		;update the counter.
	cmp	MouseFlag,0
	jl	@@1
	jg	@@1
	pushad
	call	MousePOFF		;Remove the mouse.
	popad
@@1:	dec	InMouse
	ret
MouseOff	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
MouseEvent	proc	near
	pushm	ds,es,fs,gs
	pushad
	push	ax
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	pop	ax
	;
	cmp	InMouse,0
	jnz	@@9
	;
	call	MouseUpdate
@@9:	;
@@10:	popad
	test	SystemFlags,1
	popm	ds,es,fs,gs
	jnz	@@16
	retf
@@16:	;
	db 66h
	retf
MouseEvent	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Get the current position & button state for the mouse.
;
;On Entry:-
;
;AX	- Flags (significant if bit set)
;
;On Exit:-
;
;BX	- Button state.
;CX	- X co-ordinate.
;DX	- Y co-ordinate.
;
GetMouse	proc	near
	push	ax
	mov	cx,MousePointX1
	mov	dx,MousePointY1
	mov	bx,MousePointBt
	shl	cx,3
	shl	dx,3
	pop	ax
	ret
GetMouse	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Update the mouse pointer position visually. Put it in its new position and
;display the new graphic.
;
MouseUpdate	proc	near
	inc	InMouse
	call	MouseHandler
	cmp	MousePointOff,0	;Forced removal?
	jnz	@@0
	mov	ax,MousePointX1	;Current position
	cmp	ax,MousePointX2	;Last position.
	jnz	@@0
	mov	ax,MousePointY1	;Current position.
	cmp	ax,MousePointY2	;Last position.
	jz	@@1
@@0:	mov	MousePointOff,0	;Clear forced removal flag.
	call	MousePOff		;Remove last pointer.
	call	MousePOn		;Put a new pointer on.
@@1:	dec	InMouse
	ret
MouseUpdate	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;This gets called every time there is mouse motion or a timer interupt. Fetches
;mouse status from the mouse driver and updates the co-ordinates acordingly.
;
MouseHandler	proc	near
	mov	ax,3
	mov	bx,0
	int	33h
	mov	MousePointBt,bx	;Store button status.
	mov	ax,0bh
	mov	cx,0
	mov	dx,0
	int	33h
	mov	si,cx
 	mov	di,dx
	;
	;Calculate X displacement
	;
	add	si,MouseMickeyX
	mov	MouseMickeyX,si
	mov	bp,si
	test	bp,32768
	jz	@@5
	neg	si
@@5:	mov	ax,si
	mov	cx,MouseScaleX
	xor	dx,dx
	div	cx
	mov	si,ax
	mul	cx
	test	bp,32768
	jz	@@6
	neg	si
	neg	ax
@@6:	add	si,MousePointX1
	sub	MouseMickeyX,ax	;Reset displacement
	mov	ax,0
	mov	bx,ax
	neg	ax
	mov	MousePointX1,ax
	cmp	si,ax
	jl	@@3
	sub	bx,w[VideoColumns]
	inc	bx
	neg	bx
	mov	MousePointX1,bx
	cmp	si,bx
	jg	@@3
	mov	MousePointX1,si
@@3:	;
	;Calculate Y displacement
	;
	add	di,MouseMickeyY
	mov	MouseMickeyY,di
	mov	bp,di
	test	bp,32768
	jz	@@7
	neg	di
@@7:	xor	dx,dx
	mov	ax,di
	mov	cx,MouseScaleY
	div	cx
	mov	di,ax
	mul	cx
	test	bp,32768
	jz	@@8
	neg	di
	neg	ax
@@8:	add	di,MousePointY1
	sub	MouseMickeyY,ax	;Reset displacement
	mov	ax,0
	mov	bx,ax
	neg	ax
	mov	MousePointY1,ax
	cmp	di,ax
	jl	@@4
	sub	bx,w[VideoRows]
	inc	bx
	neg	bx
	mov	MousePointY1,bx
	cmp	di,bx
	jg	@@4
	mov	MousePointY1,di
@@4:	ret
MouseHandler	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
MousePOn	proc	near
	cmp	MouseFlag,0
	jle	@@9
	cmp	VideoSegment,0
	jz	@@9
	cmp	MousePointYN,0
	jnz	@@9
	;
	mov	ax,MousePointX1
	mov	MousePointX2,ax
	mov	cl,al
	mov	ax,MousePointY1
	mov	MousePointY2,ax	;Store coordinates for taking off
	mov	ch,al
	call	WindowChXY
	push	es
	mov	es,VideoBase
	mov	ax,es:[edi]
	mov	MouseStore,ax
	rol	es:b[edi+1],4
	and	es:b[edi+1],07fh
	mov	es:b[edi+0],' '
	cmp	VideoMono,0
	jz	@@nom
	mov	es:b[edi+0],'*'
@@nom:	pop	es
	mov	MousePointYN,-1	;flag that its on there.
	;
@@9:	ret
MousePOn	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
MousePOff	proc	near
	cmp	MousePointYN,0
	jz	@@9
	;
	mov	MousePointYN,0	;flag its been taken off
	;
	mov	ax,MousePointX2
	mov	cl,al
	mov	ax,MousePointY2
	mov	ch,al
	call	WindowChXY
	mov	ax,MouseStore
	push	es
	mov	es,VideoBase
	mov	es:[edi],ax
	pop	es
	;
@@9:	mov	MousePointX2,-32767
	ret
MousePOff	endp


WindowSplitStruc struc
WindowSplitFlags db ?
WindowSplitX	dw ?
WindowSplitY	dw ?
WindowSplitW	dw ?
WindowSplitD	dw ?
WindowSplitStruc ends


	.data


WindowPLastX	db ?		;Last print X
WindowPLastY	db ?		;Last print Y

MaximumWindows	dw ?		;maximum number of windows.
WStorageStack	dd ?		;point to storage space.
WStorageSeg	dd ?		;pointer to storage segment.
WindowDBase	dd ?		;pointer to database.
WindowsStorage	dd ?		;pointer to table.
WindowTabG1	dw 0		;global tab settings.
WindowTabG2	dw 0		;/
WindowTabG3	dw 0		;/
WindowTabG4	dw 0		;/
WindowTabG5	dw 0		;/
WindowTabG6	dw 0		;/
WindowTabG7	dw 0		;/
WindowTabG8	dw 0		;/
WindowPriorityCount dd 0
WindowCurrent	dw 0
WindowFocus	dd 0
WinFocusHandle	dw 0

TabWindow1	dw 0
TabWindow2	dw 0
TabWindow3	dw 0
TabWindow4	dw 0

VideoMode	db ?			;BIOS mode in operation.
VideoMono	db ?			;Mono mode flag.
VideoPage	db 0			;Current page offset.
VideoColumns	dw ?			;Number of columns.
VideoWidth	dd 0			;Width of a line.
VideoRows	dw ?			;Number of rows.
VideoPageSize	dw ?			;Size of each page.
VideoSegment	dd ?			;Segment of video memory.
VideoBase	dw ?

_WM_OldX	dw ?
_WM_OldY	dw ?
_WM_OneOn	dw ?
_WM_LastX	dw ?
_WM_LastY	dw ?

_VB_Atts	db ?
_VB_Flags	db ?
_VB_Store	dd ?

_WPU_Attribs	dd 0

_OW_ThisPriority dd ?
_OW_ThisHandler dd ?

_MW_CallAdd	dd ?

_SWT_Type	db ?
_SWT_Buffer	db 128 dup (0)

_SWB_Type	db ?
_SWB_Buffer	db 128 dup (0)

_PW_Attribs	dd 0
_PW_Tabs	dd 0
_PW_XCount	db 0

_PW_XCount2	db 0
_PW_YCount1	db 0,0
_PW_ScrollV	db 0
_PW_ScrollH	db 0
_PW_TabLine	db '        ',0,64 dup (0)

_BW_Type	db ?,?

_SWL_RetAdd	dd ?

_RWL_RetAdd	dd ?

_CWS_XCoord	dw ?
_CWS_YCoord	dw ?
_CWS_Width	dw ?
_CWS_Depth	dw ?
_CWS_Left	dw ?
_CWS_Right	dw ?
_CWS_ThisPriority dd ?
_CWS_ThisWindow dd ?
_CWS_SplitPos	dd ?

_WI_XOff	db ?
_WI_XPos	db ?
_WI_YPos	db ?
_WI_Width	dw ?
_WI_Handle	dw ?
_WI_Buffer	dd ?
_WI_ASCII	db ?
_WI_Cursor	dw ?

