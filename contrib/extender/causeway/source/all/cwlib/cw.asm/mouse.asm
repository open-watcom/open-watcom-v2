	include ..\cwlib.inc
	include ..\cw.inc

ME	struc
 ME_Flags	dw ?
 ME_XCoord	dw ?
 ME_YCoord	dw ?
 ME_Wide	dw ?
 ME_Depth	dw ?
ME	ends

MaxExcludes		equ	16
MaxExcludeSplits	equ	32

	scode

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
_MouseInitialise:
	public _MouseInitialise
MouseInitialise proc	near
	pushad
	;
	push	ebp
	mov	ax,0
	int	33h
	cmp	ax,0
	jz	@@0
	mov	MousePresent,1	;flag mouse.
@@0:	pop	ebp
	;
	;Register pointer Bitmaps with Bitmap manager.
	;
	mov	esi,offset MouseDetails	;point to table
@@1:	cmp	d[esi],-1
	jz	@@2
	push	esi
	mov	esi,[esi]		;point to the table.
	add	esi,4+4		;skip animation and hot spot
@@3:	cmp	d[esi],-1		;end of the list?
	jz	@@4
	push	esi
	mov	esi,[esi]
	call	RegisterSystemBitmap
	pop	esi
	add	esi,4
	jmp	@@3
@@4:	pop	esi
	add	esi,4
	jmp	@@1
	;
@@2:	;Setup mouse event call back address.
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
	;Patch timer interupt for animation module.
	;
	mov	bl,8
	sys	GetVect
	push	ds
	mov	ds,CodeSegAlias
	assume ds:@CurSeg
	mov	d[OldTimer],edx
	mov	w[OldTimer+4],cx
	assume ds:DGROUP
	pop	ds
	mov	bl,8
	mov	cx,cs
	mov	edx,offset TimerInterupt
	sys	SetVect
	;
	mov	MouseOffCall,offset MouseOff
	mov	MouseOnCall,offset MouseOn
	mov	MouseExcludeCall,offset MouseExclude
	;
	popad
	movzx	eax,MousePresent
	ret
MouseInitialise endp


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
_MouseRemove:
	public MouseRemove
MouseRemove	proc	near
	pushad
	;
	cmp	w[OldTimer+4],0	;check timer was patched.
	jz	@@0
	;
	mov	ax,0		;reset mouse interupt mask/pointers.
	int	33h
	;
	mov	edx,d[OldTimer]
	mov	cx,w[OldTimer+4]
	mov	bl,8
	sys	SetVect
	;
@@0:	popad
	ret
MouseRemove	endp


;-------------------------------------------------------------------------------
TimerInterupt	proc	far
	pushm	ds,ax
	mov	ax,DGROUP
	mov	ds,ax
	;
	inc	TimerCount
	;
	mov	AnimWaiting,0	;clear animation flag.
	cmp	InMouse,0
	jnz	@@0
	cmp	w[TimerStackStore+4],0
	jnz	@@0
	pushad
	pushm	es,fs,gs,ds
	pop	es
	;
	mov	d[TimerStackStore],esp
	mov	w[TimerStackStore+4],ss
	mov	ax,ds
	mov	ss,ax
	mov	esp,offset TimerStack
	;
	call	MouseUpdate
	;
	cmp	MouseLeftDnCnt,0
	jz	@@d0
	dec	MouseLeftDnCnt
	jnz	@@d0
	and	MouseBtDouble,not (1+8)
@@d0:	;
	lss	esp,f[TimerStackStore]
	mov	w[TimerStackStore+4],0
	popm	es,fs,gs
	popad
@@0:	;
	popm	ds,ax
	assume ds:nothing
	jmp	cs:f[OldTimer]
	assume ds:DGROUP
OldTimer	df ?
TimerInterupt	endp


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
_MouseOn:
	public _MouseOn
MouseOn	proc	near
	inc	InMouse
	mov	MouseAnmCount,0	;reset animation offset
	mov	MouseAnmCount2,-1	;reset animation offset
	mov	MousePointAn,0	;/
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
_MouseOff:
	public _MouseOff
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
;AL	- Previous type.
;
;ALL other registers preserved.
;
MouseSetType	proc	near
	pushad
	inc	InMouse
	mov	ah,MousePointNm
	mov	OldPointNm,ah
	cmp	al,ah
	jz	@@9
	;
	pushad
	call	MouseOff
	popad
	;
	mov	MousePointNm,al
	mov	MousePointAn,0
	mov	MouseAnmCount,0	;reset animation offset
	mov	MouseAnmCount2,-1	;reset animation offset
	;
	;Now fiddle the mouse position so that the new hot spot is in
	;the same position as the old hot spot!
	;
	;If this isn't done, changing the mouse pointer number will
	;change the co-ordinates returned by GetMouse without the
	;pointer having moved. This could cause great confusion for
	;the main program!
	;
	;Get the old pointers offsets.
	;
	movzx	ebx,OldPointNm
	shl	ebx,2
	add	ebx,offset MouseDetails
	test	VideoModeFlags,8
	jz	@@Grph0
	sub	ebx,offset MouseDetails
	add	ebx,offset MouseDetailsT
@@Grph0:	mov	ebx,[ebx]		;get real table.
	add	ebx,4		;skip animation table.
	mov	cx,0[ebx]
	mov	dx,2[ebx]		;get x,y offsets
	;
	;Point to the new offsets
	;
	movzx	ebx,MousePointNm
	shl	ebx,2
	add	ebx,offset MouseDetails
	test	VideoModeFlags,8
	jz	@@Grph1
	sub	ebx,offset MouseDetails
	add	ebx,offset MouseDetailsT
@@Grph1:	mov	ebx,[ebx]
	add	ebx,4		;skip animation pointer.
	sub	cx,0[ebx]
	sub	dx,2[ebx]		;get x,y differance.
	;
	;Add offsets to REAL mouse co-ordinates.
	;
	add	cx,MousePointX1
	add	dx,MousePointY1
	mov	MousePointX1,cx
	mov	MousePointY1,dx
	;
	call	MouseOn
	;
@@9:	dec	InMouse
	popad
	mov	al,OldPointNm
	ret
MouseSetType	endp


;-------------------------------------------------------------------------
MouseEvent	proc	far
	pushm	ds,es,fs,gs
	pushad
	push	ax
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	pop	ax
	;
	cmp	w[EventStackStore+4],0
	jnz	@@9
	cmp	InMouse,0
	jnz	@@9
	inc	InMouse
	;
	mov	d[EventStackStore],esp
	mov	w[EventStackStore+4],ss
	mov	ax,ds
	mov	ss,ax
	mov	esp,offset EventStack
	;
	call	MouseUpdate
	lss	esp,f[EventStackStore]
	mov	w[EventStackStore+4],0
	dec	InMouse
@@9:	;
@@10:	popad
	popm	ds,es,fs,gs
	retf
MouseEvent	endp


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
MouseGet	proc	near
	mov	cx,MousePointX1
	mov	dx,MousePointY1
	movzx	ebx,MousePointNm
	shl	ebx,2
	add	ebx,offset MouseDetails
	test	VideoModeFlags,8
	jz	@@Grph0
	sub	ebx,offset MouseDetails
	add	ebx,offset MouseDetailsT
@@Grph0:	mov	ebx,[ebx]		;get real type.
	add	ebx,4		;skip animation pointer.
	add	cx,0[ebx]
	add	dx,2[ebx]
	mov	bx,MousePointBt
	movzx	ebx,bx
	movzx	ecx,cx
	movzx	edx,dx
	ret
MouseGet	endp


;-------------------------------------------------------------------------
;
;Update the mouse pointer position visually. Put it in its new position and
;display the new graphic.
;
MouseUpdate	proc	near
	inc	InMouse
	call	MouseAnimate		;Animate the pointer.
	call	MouseHandler
	cmp	MousePointOff,0	;Forced removal?
	jnz	@@0
	mov	al,MouseAnmCount	;update counter.
	cmp	al,MouseAnmCount2
	mov	MouseAnmCount2,al
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


;-------------------------------------------------------------------------
;
;This gets called every time there is mouse motion or a timer interupt. Fetches
;mouse status from the mouse driver and updates the co-ordinates acordingly.
;
MouseHandler	proc	near
	mov	ax,3
	mov	bx,0
	int	33h
	mov	MousePointBt,bx
;
;Update left button double click status.
;
	test	MousePointBt,1	;Left button press?
	jz	@@l1
	cmp	MouseLeftDnCnt,0	;Second press?
	jnz	@@l0
	;
	;Hasn't been pressed within timeout period.
	;
	mov	eax,MouseDoubleTime
	mov	MouseLeftDnCnt,eax
	or	MouseBtDouble,1
	jmp	@@l9
@@l0:	;
	;Has been pressed within timeout period.
	;
	test	MouseBtDouble,1	;Was it released?
	jnz	@@l9
	or	MouseBtDouble,8	;Signal double click.
	jmp	@@l9
	;
@@l1:	test	MouseBtDouble,8
	jz	@@l2
	;
	;After effect of a double click.
	;
	and	MouseBtDouble,not (1+8)
	mov	MouseLeftDnCnt,0
	jmp	@@l9
@@l2:	;
	;Just reset the button press toggle incase it's the first
	;stage of a double press.
	;
	and	MouseBtDouble,not 1
@@l9:	;
	mov	ax,MouseBtDouble
	and	ax,8+16+32
	or	MousePointBt,ax
;
;Update X/Y stuff.
;
	mov	ax,0bh
	mov	cx,0
	mov	dx,0
	int	33h
	mov	si,cx
 	mov	di,dx
	;
	;Calculate X displacement
	;
	mov	cx,MouseScaleX
	cmp	VideoXResolution,320+1
	jc	@@5
	sub	cx,2
	cmp	VideoXResolution,640+1
	jc	@@5
;	dec	cx
	cmp	VideoXResolution,800+1
	jc	@@5
;	dec	cx
@@5:	mov	ax,si
	add	ax,ax
	add	ax,MouseMickeyX	;add remainder from last time.
	cwd
	idiv	cx
	mov	MouseMickeyX,dx	;Reset displacement
	mov	si,ax
	add	si,MousePointX1
	;
	movzx	ebx,MousePointNm
	shl	ebx,2
	add	ebx,offset MouseDetails
	test	VideoModeFlags,8
	jz	@@Grph0
	sub	ebx,offset MouseDetails
	add	ebx,offset MouseDetailsT
@@Grph0:	mov	ebx,[ebx]		;get real table.
	add	ebx,4		;skip animation pointer.
	mov	ax,[ebx]
	mov	bx,ax
	neg	ax
	mov	MousePointX1,ax
	cmp	si,ax
	jl	@@3
	sub	ebx,VideoXResolution
	inc	bx
	neg	bx
	mov	MousePointX1,bx
	cmp	si,bx
	jg	@@3
	mov	MousePointX1,si
@@3:	;
	;Calculate Y displacement
	;
	mov	cx,MouseScaleY
	cmp	VideoYResolution,200+1
	jc	@@7
	dec	cx
	dec	cx
	cmp	VideoYResolution,480+1
	jc	@@7
;	dec	cx
	cmp	VideoYResolution,600+1
	jc	@@7
;	dec	cx
@@7:	mov	ax,di
	add	ax,ax
	add	ax,MouseMickeyY
	cwd
	idiv	cx
	mov	MouseMickeyY,dx	;Reset displacement
	mov	di,ax
	add	di,MousePointY1
	;
	movzx	ebx,MousePointNm
	shl	ebx,2
	add	ebx,offset MouseDetails
	test	VideoModeFlags,8
	jz	@@Grph1
	sub	ebx,offset MouseDetails
	add	ebx,offset MouseDetailsT
@@Grph1:	mov	ebx,[ebx]		;get real table.
	add	ebx,4		;skip animation pointer.
	mov	ax,2[ebx]
	mov	bx,ax
	neg	ax
	mov	MousePointY1,ax
	cmp	di,ax
	jl	@@4
	sub	ebx,VideoYResolution
	inc	bx
	neg	bx
	mov	MousePointY1,bx
	cmp	di,bx
	jg	@@4
	mov	MousePointY1,di
@@4:	ret
MouseHandler	endp


;-------------------------------------------------------------------------
;
;Animate the mouse pointer.
;
MouseAnimate	proc	near
	cmp	AnimWaiting,0
	jnz	@@9
	mov	AnimWaiting,1
@@0:	movzx	ebx,MousePointNm
	shl	ebx,2
	add	ebx,offset MouseDetails
	test	VideoModeFlags,8
	jz	@@Grph0
	sub	ebx,offset MouseDetails
	add	ebx,offset MouseDetailsT
@@Grph0:	mov	ebx,[ebx]		;get real table.
	mov	ebx,[ebx]		;get animation pointer.
	cmp	ebx,-1
	jz	@@9
	movzx	eax,MouseAnmCount	;get animation offset
	add	ebx,eax		;point to new animation.
	inc	MouseAnmCount	;update counter.
	mov	al,[ebx]		;get animation number.
	cmp	al,-1		;end of list?
	jnz	@@1
	mov	MouseAnmCount,0	;reset counter.
	jmp	@@0		;go back to the start.
@@1:	cmp	al,MousePointAn	;same as current?
	jz	@@9
	mov	MousePointAn,al	;set new animation.
	mov	MousePointOff,1	;force display of new symbol.
@@9:	ret
MouseAnimate	endp


;-------------------------------------------------------------------------
MousePOn	proc	near
	pushad
	cmp	VideoMode,-1		;make sure video will be receptive.
	jz	@@9
	cmp	MouseFlag,0		;pointer on display?
	jle	@@9
	cmp	MousePointYN,0
	jnz	@@9
	;
	;Preserve current video bank.
	;
	movzx	ebx,MousePointNm	;get pointer type.
	shl	ebx,2
	add	ebx,offset MouseDetails
	test	VideoModeFlags,8
	jz	@@Grph0
	sub	ebx,offset MouseDetails
	add	ebx,offset MouseDetailsT
@@Grph0:	mov	ebx,[ebx]
	add	ebx,4+4		;skip animation pointer & hot spot.
	movzx	eax,MousePointAn	;add in the animation.
	shl	eax,2
	add	ebx,eax
	mov	esi,[ebx]
	mov	cx,MousePointX1
	mov	bx,MousePointY1
	mov	MousePointX2,cx
	mov	MousePointY2,bx	;Store coordinates for taking off
	mov	edi,offset MouseExcludeList	;List of exclude structures.
	mov	ebp,MaxExcludes
	;
@@0:	test	ME_Flags[edi],1	;exclude entry in use?
	jz	@@1
	;
	;Check if coords overlap.
	;
	mov	ax,cx
	mov	dx,ME_XCoord[edi]
	add	dx,ME_Wide[edi]
	cmp	ax,dx		;Left edge beyond right edge?
	jg	@@1
	add	eax,BM_Wide[esi]
	sub	dx,ME_Wide[edi]
	cmp	ax,dx		;Right edge below left edge?
	jl	@@1
	mov	ax,bx
	mov	dx,ME_YCoord[edi]
	add	dx,ME_Depth[edi]
	cmp	ax,dx		;Top edge beyond bottom edge?
	jg	@@1
	add	eax,BM_Depth[esi]
	sub	dx,ME_Depth[edi]
	cmp	ax,dx		;Bottom edge below Top edge?
	jl	@@1
	jmp	@@9
	;
@@1:	add	edi,size ME		;Next entry
	dec	ebp
	jnz	@@0
	;
	push	CurrentBank
	;
	movsx	ecx,cx
	movsx	ebx,bx
	pushad
	mov	edi,offset MouseStore
	mov	eax,BM_Wide[esi]
	mov	BM_Wide[edi],eax
	mov	eax,BM_Depth[esi]
	mov	BM_Depth[edi],eax
	push	0
	push	32768
	push	32768
	push	0
	push	0
	push	ebx
	push	ecx
	push	edi
	push	1
	call	VideoBlitBitmap
	add	esp,4*9
	popad
	push	0
	push	32768
	push	32768
	push	0
	push	0
	push	ebx
	push	ecx
	push	esi
	push	2
	call	VideoBlitBitmap
	add	esp,4*9
	mov	MousePointYN,-1	;flag that its on there.
	;
	mov	ax,CurrentBank
	pop	CurrentBank
	cmp	ax,CurrentBank
	jz	@@8
	mov	bh,0
	mov	bl,0
	mov	dx,CurrentBank
	mov	cl,BankShift
	shl	dx,cl
	call	d[BankAddress]
	mov	bh,0
	mov	bl,1
	mov	dx,CurrentBank
	mov	cl,BankShift
	shl	dx,cl
	call	d[BankAddress]
@@8:	;
@@9:	popad
	ret
MousePOn	endp


;-------------------------------------------------------------------------
MousePOff	proc	near
	pushad
	cmp	MousePointYN,0
	jz	@@9
	push	CurrentBank
	;
	mov	MousePointYN,0	;flag its been taken off
	mov	esi,offset MouseStore
	movsx	ecx,MousePointX2
	movsx	ebx,MousePointY2
	push	0
	push	32767
	push	32767
	push	0
	push	0
	push	ebx
	push	ecx
	push	esi
	push	0
	call	VideoBlitBitmap
	add	esp,4*9
	;
	mov	ax,CurrentBank
	pop	CurrentBank
	cmp	ax,CurrentBank
	jz	@@8
	mov	bh,0
	mov	bl,0
	mov	dx,CurrentBank
	mov	cl,BankShift
	shl	dx,cl
	call	d[BankAddress]
	mov	bh,0
	mov	bl,1
	mov	dx,CurrentBank
	mov	cl,BankShift
	shl	dx,cl
	call	d[BankAddress]
@@8:	;
@@9:	mov	MousePointX2,-32767
	popad
	ret
MousePOff	endp


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
MouseExclude	proc	near
	cmp	InMouse,0
	jnz	@@9
	inc	InMouse
	pushm	eax,ecx,edx,esi,edi,ebp
	or	ax,ax
	jnz	@@Rel
	mov	esi,offset MouseExcludeList
@@0:	test	ME_Flags[esi],1
	jz	@@1
	add	esi,size ME
	jmp	@@0
@@1:	mov	ME_XCoord[esi],cx
	mov	ME_YCoord[esi],bx
	mov	ME_Wide[esi],dx
	mov	ME_Depth[esi],bp
	mov	ME_Flags[esi],1		;claim this as ours.
	push	esi
	movzx	ebx,MousePointNm	;get pointer type.
	shl	ebx,2
	add	ebx,offset MouseDetails
	test	VideoModeFlags,8
	jz	@@Grph0
	sub	ebx,offset MouseDetails
	add	ebx,offset MouseDetailsT
@@Grph0:	mov	ebx,[ebx]
	add	ebx,4+4		;skip animation pointer & hot spot.
	movzx	eax,MousePointAn	;add in the animation.
	shl	eax,2
	add	ebx,eax
	mov	esi,[ebx]
	mov	cx,MousePointX1
	mov	bx,MousePointY1
	mov	edx,BM_Wide[esi]
	mov	ebp,BM_Depth[esi]
	pop	esi
	;
	;Check if coords overlap.
	;
	mov	ax,ME_XCoord[esi]
	mov	di,cx
	add	di,dx
	cmp	ax,di		;Left edge beyond right edge?
	jge	@@2
	add	ax,ME_Wide[esi]
	sub	di,dx
	cmp	ax,di		;Right edge below left edge?
	jle	@@2
	mov	ax,ME_YCoord[esi]
	mov	di,bx
	add	di,bp
	cmp	ax,di		;Top edge beyond bottom edge?
	jge	@@2
	add	ax,ME_Depth[esi]
	sub	di,bp
	cmp	ax,di		;Bottom edge below Top edge?
	jle	@@2
	;
	;Need to force updating of the mouse.
	;
	push	esi
	mov	MousePointOff,1	;Forced removal.
	call	MouseUpdate
	pop	esi
@@2:	mov	ebx,esi
	jmp	@@Um
	;
@@Rel:	cmp	ax,1
	jnz	@@Um
	mov	esi,ebx
	mov	ME_Flags[esi],0
	push	esi
	movzx	ebx,MousePointNm	;get pointer type.
	shl	ebx,2
	add	ebx,offset MouseDetails
	test	VideoModeFlags,8
	jz	@@Grph1
	sub	ebx,offset MouseDetails
	add	ebx,offset MouseDetailsT
@@Grph1:	mov	ebx,[ebx]
	add	ebx,4+4		;skip animation pointer & hot spot.
	movzx	eax,MousePointAn	;add in the animation.
	shl	eax,2
	add	ebx,eax
	mov	esi,[ebx]
	mov	cx,MousePointX1
	mov	bx,MousePointY1
	mov	edx,BM_Wide[esi]
	mov	ebp,BM_Depth[esi]
	pop	esi
	;
	;Check if coords overlap.
	;
	mov	ax,ME_XCoord[esi]
	mov	di,cx
	add	di,dx
	cmp	ax,di		;Left edge beyond right edge?
	jge	@@3
	add	ax,ME_Wide[esi]
	sub	di,dx
	cmp	ax,di		;Right edge below left edge?
	jle	@@3
	mov	ax,ME_YCoord[esi]
	mov	di,bx
	add	di,bp
	cmp	ax,di		;Top edge beyond bottom edge?
	jge	@@3
	add	ax,ME_Depth[esi]
	sub	di,bp
	cmp	ax,di		;Bottom edge below Top edge?
	jle	@@3
	;
	;Need to force updating of the mouse.
	;
	mov	MousePointOff,1	;Forced removal.
	call	MouseUpdate
@@3:	jmp	@@Um
	;
@@Um:	popm	eax,ecx,edx,esi,edi,ebp
	dec	InMouse
@@9:	ret
MouseExclude	endp


	sdata
;
;/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
;
AnimWaiting	db 0
MousePresent	db 0
MouseFlag	db 0
MousePointOff	db 0
MousePointYN	db 0
MousePointNm	db 0
OldPointNm	db 0
MousePointAn	db 0
MousePointMd	db 0
MousePointBt	dw 0
MouseBtDouble	dw 0
MouseDoubleTime dd 7
MouseLeftDnCnt	dd 0
MouseDBounce	dw 0
MouseCBounce	dw 0
MousePointX1	dw 320/2
MousePointX2	dw -32767
MousePointY1	dw 200/2
MousePointY2	dw -32767
MouseAnmCount	db 0
MouseAnmCount2	db -1
MouseMickeyX	dw 0
MouseMickeyY	dw 0
MouseScaleX	dw 4
MouseScaleY	dw 4
;
MouseStore	BM <>
	db 2048 dup (0)
;
MouseDetails	dd MousePoint1,MousePoint0,-1
MouseDetailsT	dd MousePoint1T,MousePoint0T,-1
;
MousePoint0	dd MouseTmAnim
	dw 8,0
	dd Timer00,Timer01,Timer02,Timer03,Timer04,Timer05,Timer06,Timer07
	dd Timer08,Timer09,Timer0a,Timer0b,Timer0c,Timer0d,Timer0e,Timer0f
	dd Timer10,Timer11,-1
MouseTmAnim	db 0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12
	db 13,13,14,14,15,15,16,16,17,17,17,17,17,17,17,17,17,17,17,-1
	;
MousePoint0T	dd MouseTmAnimT
	dw 0,0
	dd Timer00T,Timer01T,Timer02T,Timer03T,-1
MouseTmAnimT	db 0,0,1,1,2,2,3,3,-1
	;
	include timer.gsc
Timer00T	BM <8+1,2,1,1>
	db 196,65
Timer01T	BM <8+1,2,1,1>
	db "\",65
Timer02T	BM <8+1,2,1,1>
	db 179,65
Timer03T	BM <8+1,2,1,1>
	db "/",65
;
MousePoint1	dd -1
	dw 0,0
	dd Arrow00,-1
	;
MousePoint1T	dd -1
	dw 0,0
	dd Arrow00T,-1
	;
	include arrow.gsc
Arrow00T	BM <8+1,2,1,1>
	db 64,65
;
MouseExcludeList db size ME*MaxExcludes dup (0)
;
EventStackStore df 0
	db 1024 dup (0)
EventStack	label byte
;
TimerStackStore df 0
	db 1024 dup (0)
TimerStack	label	byte
;
	public TimerCount
TimerCount	dd 0
;
	efile
	end

