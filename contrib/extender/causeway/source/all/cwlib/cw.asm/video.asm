
	include ..\cwlib.inc
	include ..\cw.inc
	scode

;-------------------------------------------------------------------------
;
;Initialise video stuff.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error and EAX=error code else EAX=0.
;
;Error codes:
;
;0	- no error.
;1	- couldn't allocate selector for video memory.
;2	- not enough memory for bitmap control table.
;3	- video system not supported, ie, not (S)VGA.
;
;ValidModeList will be filled in with all modes supported.
;
;ALL other registers preserved.
;
VideoInitialise proc near
;
;Setup a selector to access video memory with.
;
	pushm	ebx,ecx,edx,esi,edi,ebp
	sys	GetSel
	mov	eax,1
	jc	l9
	mov	VideoSelector,bx
;
;Allocate memory for bitmap control.
;
	mov	ecx,(size BMT)*MaxBMEntries
	call	malloc
	mov	eax,2
	jc	l9
	mov	BMTable,esi
	mov	edi,esi
	mov	ecx,(size BMT)*MaxBMEntries
	xor	al,al
	rep	stosb
;
;Check if VESA functions are available first.
;
	call	InitVESA
	mov	edi,offset ValidModeList
	jc	l1
	mov	BankAddress,eax
;
;Setup mode list pointer.
;
	mov	ax,4f00h
	mov	edi,offset VESABuffer
	int	10h
;
;Copy VESA mode list into local buffer.
;
	push	ds
	mov	edi,offset VESABuffer
	movzx	esi,w[edi+VIB_VideoModePtr]
	mov	ds,w[edi+VIB_VideoModePtr+2]
	mov	edi,offset ValidModeList
	cld
l0:	movsw
	cmp	w[esi-2],-1
	jnz	l0
	sub	edi,2
	pop	ds
;
;Add modes 3 & 13h to the list and then terminate it.
;
l1:	mov	ax,13h
	stosw
	mov	ax,-1
	stosw
;
;Now get current mode details incase we want to stay in text mode.
;
	mov	ah,0fh		;Get mode function.
	int	10h		;/
	movzx	eax,al
	mov	VideoMode,eax	;Store this mode.
	mov	VideoBase,0b8000h	;default to colour segment.
	cmp	al,7		;check for mono.
	jnz	@@Colour
;
;Mono display so set things accordingly.
;
@@mono:	mov	VideoBase,0b0000h	;use mono segment.
	mov	VideoMono,1		;flag mono mode.
	mov	VideoXResolution,80
	mov	VideoHardwareWidth,80*2
	mov	VideoYResolution,25
	jmp	@@Text
@@Colour:	;
	push	es
	mov	ax,40h		;Point to the video bios data
	mov	es,ax		;area to get info.
	xor	ebx,ebx		;/
	movzx	eax,es:w[ebx+4ah]	;Get the number of columns.
	mov	VideoXResolution,eax	;save it.
	shl	eax,1
	mov	VideoHardwareWidth,eax	;set line width.
	movzx	ebx,es:b[ebx+84h]	;get the number of rows.
	inc	ebx		;/
	mov	VideoYResolution,ebx	;save it.
	;
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
	movzx	eax,ax
	add	VideoBase,eax
	pop	es
	;
@@Text:	mov	VideoPixelWidth,2
	mov	VideoModeFlags,8+1	;Flag text mode, attrib & char.
;
;Setup the video memory selector base.
;
	mov	bx,VideoSelector
	mov	edx,VideoBase
	mov	ecx,65535
	sys	SetSelDet32
	;
	xor	eax,eax
l10:	popm	ebx,ecx,edx,esi,edi,ebp
	ret
	;
l9:	stc
	jmp	l10
VideoInitialise endp


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
VideoRemove	proc	near
	pushad
	call	RelVESA
	mov	ax,3
	int	10h
	popad
	xor	eax,eax
	ret
VideoRemove	endp


	sdata
;
VideoSelector	dw 0
VideoBase	dd 0
BankAddress	dd 0
CurrentBankBig	dw 0
CurrentBank	dw 0
BankShift	db 0
;
VideoPixelWidth dd ?
VideoXResolution dd ?
VideoYResolution dd ?
VideoHardwareWidth dd ?
VideoModeFlags	dd 0
VideoMode	dd -1
VideoMono	db 0
;
HardwarePalette label byte
	include palette.inc
	db 256*3 dup (-1)
SystemXlat	label byte
	db 256 dup (0)
SystemPalette	label byte
	include palette.inc
;
BMTable	dd ?
;
ValidModeList	label word
	dw 256 dup (-1)
VESABuffer	label byte
	db 256 dup (0)
ModeInfoBuffer	label byte
	db 256 dup (0)
;
Forced32kList	dw 10dh, 110h, 113h, 116h, -1
Forced64kList	dw 10eh, 111h, 114h, 117h, -1
;
MouseExcludeCall dd NearNull
MouseOffCall	dd NearNull
MouseOnCall	dd NearNull
;
InMouse	dw 0


	efile
	end

