	include ..\cwlib.inc
	scode

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
VideoFillRectangle:
_VideoFillRectangle proc syscall Flags:dword, FillValue:dword, XCoord:dword, \
	YCoord:dword, Wide:dword, Depth:dword
	local	@@MouseHandle:dword,@@YBreak:dword,@@Depth:dword
	pushad
;
;Check for invalid width/depth.
;
	cmp	Wide,0
	js	@@FillFinished
	jz	@@FillFinished
	cmp	Depth,0
	js	@@FillFinished
	jz	@@FillFinished
;
;Clip left.
;
@@Left:	cmp	XCoord,0		;check for neg value.
	jns	@@Right
	mov	eax,XCoord
	neg	eax
	sub	Wide,eax		;adjust width.
	js	@@FillFinished
	jz	@@FillFinished
	mov	XCoord,0		;reset coord.
;
;Clip right.
;
@@Right:	mov	eax,XCoord
	add	eax,Wide		;get right edge.
	cmp	eax,VideoXResolution
	jle	@@Top
	sub	eax,VideoXResolution
	sub	Wide,eax		;adjust width.
	js	@@FillFinished
	jz	@@FillFinished
;
;Clip top.
;
@@Top:	cmp	YCoord,0		;check for neg value.
	jns	@@Bottom
	mov	eax,YCoord
	neg	eax
	sub	Depth,eax		;adjust depth.
	js	@@FillFinished
	jz	@@FillFinished
	mov	YCoord,0		;reset coord.
;
;Clip bottom.
;
@@Bottom:	mov	eax,YCoord
	add	eax,Depth		;get bottom edge.
	cmp	eax,VideoYResolution
	jle	@@DoneClip
	sub	eax,VideoYResolution
	sub	Depth,eax
	js	@@FillFinished
	jz	@@FillFinished
;
;Pass coords etc to mouse exclude code.
;
@@DoneClip:	push	ebp
	mov	eax,0		;setting it.
	mov	ecx,XCoord
	mov	ebx,YCoord
	mov	edx,Wide
	mov	ebp,Depth
	call	MouseExcludeCall
	pop	ebp
	mov	@@MouseHandle,ebx
;
;Convert fill value to suit mode.
;
	mov	eax,VideoModeFlags
	and	eax,15
	jnz	@@rgbs
	test	Flags,1		;24-bit value?
	jz	@@GetAddr
;
;Need to convert RGB value to nearest hardware pen number.
;
	mov	eax,FillValue
	call	SearchRGB		;Find nearest value.
	mov	FillValue,eax
	jmp	@@GetAddr
;
;Check if FillValue is a pen number or RGB.
;
@@rgbs:	test	eax,8		;Text mode?
	jnz	@@GetAddr
	test	Flags,1
	jnz	@@GotRGB
;
;Convert pen number into RGB values.
;
	movzx	eax,b[FillValue]
	mov	eax,d[HardwarePalette+eax+eax*2]
	shl	eax,2
	mov	bl,al
	shr	eax,8
	xchg	ah,bl
	shl	eax,8
	mov	al,bl
	mov	FillValue,eax
;
;Convert RGB to suit mode.
;
@@GotRGB:	mov	eax,VideoModeFlags
	and	eax,15
	dec	eax
	jz	@@16m_2_32k
	dec	eax
	jz	@@16m_2_64k
	jmp	@@GetAddr
;
;Convert RGB to 15 bit.
;
@@16m_2_32k:	mov	ebx,FillValue
	shl	ebx,8
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,5
	mov	FillValue,eax
	jmp	@@GetAddr
;
;Convert RGB to 16 bit.
;
@@16m_2_64k:	mov	ebx,FillValue
	shl	ebx,8
	xor	eax,eax
	shld	eax,ebx,5
	shl	ebx,8
	shld	eax,ebx,6
	shl	ebx,8
	shld	eax,ebx,5
	mov	FillValue,eax
;
;Calculate start screen address.
;
@@GetAddr:	mov	eax,YCoord
	mul	VideoHardwareWidth
	mov	edi,eax
	mov	eax,XCoord
	mul	VideoPixelWidth
	add	edi,eax
;
;work out line we split on in this bank.
;
@@CarryON:	bank	edi		;make sure we start in the right bank.
	mov	eax,edi		;get destination bank.
	mov	ax,-1		;and extremity in low word.
	mov	ecx,VideoHardwareWidth	;width of each line.
	xor	edx,edx
	div	ecx		;get last Y coord.
	mov	@@YBreak,eax		;line to break on.
;
;Now work out how much of this block we can do as normal.
;
	sub	eax,YCoord		;get distance from current position.
	jz	@@Force
	js	@@Force
	mov	@@Depth,eax		;and set depth.
	cmp	eax,Depth		;>than remaining depth?
	jc	@@ok0
@@Force:	mov	eax,Depth		;get whats left.
	mov	@@Depth,eax
@@ok0:	cmp	@@Depth,0
	jz	@@FillDone
	js	@@FillDone
	mov	eax,YCoord
	cmp	eax,@@YBreak
	jnz	@@NotThisLine
	mov	@@Depth,1
;
;Setup clean/broken flag and other details.
;
@@NotThisLine:	mov	esi,FillValue
	mov	ebx,YCoord
	xor	ebx,@@YBreak
	mov	ecx,Wide
	mov	edx,@@Depth
;
;Now select appropriate fill routine.
;
	push	es
	mov	es,VideoSelector
	mov	eax,VideoModeFlags
	and	eax,15
	call	d[FillRoutines+eax*4]
	pop	es
;
;Move onto next section.
;
	mov	eax,@@Depth
	add	YCoord,eax
	sub	Depth,eax
	jnz	@@CarryON
;
;Restore the mouse and exit.
;
@@FillDone:	mov	ebx,@@MouseHandle
	mov	eax,1
	call	MouseExcludeCall
;
;exit for not on screen.
;
@@FillFinished: popad
	ret
_VideoFillRectangle endp


;-------------------------------------------------------------------------
;
;Fill rectangle in 256 colour mode.
;
;On Entry:
;
;EBX	- status, 0-broken, !=0-clean.
;ESI	- Fill value.
;ECX	- Width.
;EDX	- Depth.
;EDI	- offset.
;
vFillRect256	proc
	mov	eax,esi
;
;Check which version we need to use.
;
	or	ebx,ebx		;broken line?
	jz	@@Broken
;
;Do un-broken section.
;
	sub	edi,d[CurrentBankBig]
@@0:	pushm	ecx,edi
	rep_stosb
	popm	ecx,edi
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@0
	add	edi,d[CurrentBankBig]
	ret
;
;Do broken line.
;
@@Broken:	pushm	ecx,edi
	sub	edi,d[CurrentBankBig]
	movsx	eax,di
	neg	eax
	cmp	eax,ecx
	jc	@@2
	mov	eax,ecx
@@2:	push	ecx
	push	eax
	mov	ecx,eax
	mov	eax,esi
	rep_stosb
	pop	eax
	pop	ecx
	sub	ecx,eax
	jz	@@3
	;
@@1:	add	edi,d[CurrentBankBig]
	bank	edi
	sub	edi,d[CurrentBankBig]
	mov	eax,esi
	rep_stosb
	;
@@3:	popm	ecx,edi
	add	edi,VideoHardwareWidth
	ret
vFillRect256	endp


;-------------------------------------------------------------------------
;
;Fill rectangle in 32k or 64k colour mode.
;
;On Entry:
;
;EBX	- status, 0-broken, !=0-clean.
;ESI	- Fill value.
;ECX	- Width.
;EDX	- Depth.
;EDI	- offset.
;
vFillRect32k	proc	near
	mov	eax,esi
;
;See which version we need.
;
	or	ebx,ebx
	jz	@@Broken
;
;Do un-broken section.
;
	sub	edi,d[CurrentBankBig]
@@0:	pushm	ecx,edi
	rep_stosw
	popm	ecx,edi
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@0
	add	edi,d[CurrentBankBig]
	ret
;
;Do broken section.
;
@@Broken:	pushm	ecx,edi
	sub	edi,d[CurrentBankBig]
	movsx	eax,di
	neg	eax
	shr	eax,1
	cmp	eax,ecx
	jc	@@1
	mov	eax,ecx
@@1:	push	ecx
	push	eax
	mov	ecx,eax
	mov	eax,esi
	rep_stosw
	pop	eax
	pop	ecx
	sub	ecx,eax
	jz	@@2
	;
	add	edi,d[CurrentBankBig]
	bank	edi
	sub	edi,d[CurrentBankBig]
	mov	eax,esi
	rep_stosw
	;
@@2:	popm	ecx,edi
	add	edi,VideoHardwareWidth
	ret
vFillRect32k	endp


;-------------------------------------------------------------------------
;
;Fill rectangle in 16m colour mode.
;
;On Entry:
;
;EBX	- status, 0-broken, !=0-clean.
;ESI	- Fill value.
;ECX	- Width.
;EDX	- Depth.
;EDI	- offset.
;
vFillRect16m	proc	near
	mov	eax,esi
	or	ebx,ebx
	jz	@@Broken
	mov	ebx,eax
	shr	ebx,16
;
;Do unbroken section.
;
	sub	edi,d[CurrentBankBig]
@@0:	pushm	ecx,edi
@@1:	stosw
	mov	es:[edi],bl
	inc	edi
	loop	@@1
	popm	ecx,edi
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@0
	add	edi,d[CurrentBankBig]
	ret
;
;Do broken section.
;
@@Broken:	mov	ebx,eax
	shr	ebx,16
	pushm	ecx,edi
	lea	ecx,[ecx+ecx*2]
@@2:	bank	edi
	sub	edi,d[CurrentBankBig]
	mov	es:[edi],al
	mov	bh,al
	mov	al,ah
	mov	ah,bl
	mov	bl,bh
	inc	edi
	add	edi,d[CurrentBankBig]
	dec	ecx
	jnz	@@2
	popm	ecx,edi
	add	edi,VideoHardwareWidth
	ret
vFillRect16m	endp


;-------------------------------------------------------------------------
;
;Fill rectangle in 32k or 64k colour mode.
;
;On Entry:
;
;EBX	- status, 0-broken, !=0-clean.
;ESI	- Fill value.
;ECX	- Width.
;EDX	- Depth.
;EDI	- offset.
;
vFillRect16m32	proc	near
	mov	eax,esi
	and	eax,0ffffffh
	or	ebx,ebx
	jz	@@Broken
;
;Do unbroken section.
;
	sub	edi,d[CurrentBankBig]
@@0:	pushm	ecx,edi
	rep	stosd
	popm	ecx,edi
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@0
	add	edi,d[CurrentBankBig]
	ret
;
;Do broken section.
;
@@Broken:	pushm	ecx,edi
@@1:	bank	edi
	sub	edi,d[CurrentBankBig]
	stosd
	add	edi,d[CurrentBankBig]
	loop	@@1
	popm	ecx,edi
	add	edi,VideoHardwareWidth
	ret
vFillRect16m32	endp


;-------------------------------------------------------------------------
;
;Fill rectangle in text mode.
;
;On Entry:
;
;EBX	- status, 0-broken, !=0-clean.
;ESI	- Fill value.
;ECX	- Width.
;EDX	- Depth.
;EDI	- offset.
;
vFillRectText	proc
	mov	eax,esi
	sub	edi,d[CurrentBankBig]
@@0:	pushm	ecx,edi
	rep_stosw
	popm	ecx,edi
	add	edi,VideoHardwareWidth
	dec	edx
	jnz	@@0
	add	edi,d[CurrentBankBig]
	ret
vFillRectText	endp


	sdata

FillRoutines	dd vFillRect256,vFillRect32k,vFillRect32k,vFillRect16m
	dd vFillRect16m32,3 dup (NearNull)
	dd NearNull
	dd vFillRectText
	dd 6 dup (NearNull)


	efile
	end

