;
;/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
;
;VESA patch example, 32-bit.
;
	include ..\cwlib.inc
	include ..\cw.inc

;
RR	struc
 RR_EDI	dd ?	;EDI
 RR_ESI	dd ?	;ESI
 RR_EBP	dd ?	;EBP
	dd ?	;Reserved.
 RR_EBX	dd ?	;EBX
 RR_EDX	dd ?	;EDX
 RR_ECX	dd ?	;ECX
 RR_EAX	dd ?	;EAX
 RR_Flags	dw ?	;FLAGS
 RR_ES	dw ?	;ES
 RR_DS	dw ?	;DS
 RR_FS	dw ?	;FS
 RR_GS	dw ?	;GS
 RR_IP	dw ?	;IP
 RR_CS	dw ?	;CS
 RR_SP	dw ?	;SP
 RR_SS	dw ?	;SS
RR	ends
;

;-----------------------------------------------------------------------------
;
;New VESA stuff.
;
	if	0
	;
	;Get Write Bank code.
	;
	mov	ax,4f0ah
	mov	bx,0fe01h
	mov	dx,0500h
	int	10h
	cmp	ax,004fh
	jnz	@@Nope
	ES:DI - Code
	DX    - Length

	;
	;Get read bank code.
	;
	mov	ax,4f0ah
	mov	bx,0fe01h
	mov	dx,0501h
	int	10h
	cmp	ax,004fh
	jnz	@@nope
	ES:DI - Code
	DX    - Length

	;
	;Set display position.
	;
	mov	ax,4f07h
	mov	bx,0	;page?
	mov	cx,X
	mov	dx,Y
	int	10h
	cmp	ax,004fh
	jnz	@@nope

	;
	;Set wide DAC
	;
	mov	ax,4f08h
	mov	bx,0800h
	int	10h
	cmp	ax,004fh
	jnz	@@nope

	;
	;Set normal DAC
	;
	mov	ax,4f08h
	mov	bx,0600h
	int	10h
	cmp	ax,004fh
	jnz	@@nope

	;
	;Get DAC width.
	;
	mov	ax,4f08h
	mov	bx,0001h
	int	10h
	cmp	ax,004fh
	jnz	@@nope
	BH    - DAC width.
	endif


	scode

;-----------------------------------------------------------------------------
;
;Have a look for VESA INT 10h BIOS extension and if present install a patch to
;make protected mode access transparent.
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
InitVESA	proc	near
	pushad
;
;Need some DOS memory to use as a transfer buffer.
;
	mov	bx,256/16
	sys	GetMemDOS
	jc	l9
	mov	w[TransferBuffer+4],dx
	mov	TransferReal,ax
;
;Now see if VESA is present.
;
	mov	edi,offset Int10hBuffer	;Somewhere for real registers.
	mov	RR_EAX[edi],4f00h
	mov	eax,d[TransferBuffer]
	mov	RR_EDI[edi],eax	;Address to put VgaInfoBlock in.
	mov	ax,TransferReal
	mov	RR_ES[edi],ax
	mov	bl,10h
	sys	IntXX		;Call the real mode handler.
	mov	eax,RR_EAX[edi]
	cmp	ax,4fh		;Valid VESA function?
	jnz	l9
	push	es
	les	edi,f[TransferBuffer]
	cmp	es:d[edi],"ASEV"	;Check for "VESA"
	pop	es
	jnz	l9
;
;Need selectors to use in OEMStringPtr and VideoModePtr.
;
	sys	GetSel		;Need selector for OEMStringPtr
	jc	l9		;field of VgaInfoBlock
	mov	OEMStringSel,bx
	sys	GetSel		;Need selector for VideoModePtr
	jc	l9		;field of VgaInfoBlock.
	mov	VideoModeSel,bx
;
;Finaly, we need to patch INT 10h so that we can intercept the VESA functions
;supported by this code.
;
	mov	bl,10h
	sys	GetVect		;Get current vector address.
	push	ds
	mov	ds,CodeSegAlias
	ifdef CWNEAR
	assume ds:_SEG
	else
	assume ds:_TEXT
	endif
	mov	d[OldInt10h],edx	;store old vector address.
	mov	w[OldInt10h+4],cx
	assume ds:DGROUP
	pop	ds
	mov	edx,offset Int10hHandler
	mov	cx,cs
	sys	SetVect		;set new handler address.
	;
@@8:	popad
;
;Return address of bank switch code address. This is a constant address so can
;be stowed away and never updated again. Can still use VESA function 01h to get
;this address via the info block though.
;
	mov	eax,offset WinFuncHandler
	clc
	ret
;
;Either not enough selectors/memory or no VESA so clean up and exit.
;
l9:	mov	bx,OEMStringSel
	or	bx,bx
	jz	l9_0
	sys	RelSel		;Lose OEMStringSel selector.
l9_0:	mov	bx,VideoModeSel
	or	bx,bx
	jz	l9_1
	sys	RelSel		;Lose VideoModeSel selector.
l9_1:	mov	dx,TransferReal
	or	dx,dx
	jz	l9_2
	sys	RelMemDOS		;Lose transfer buffer.
l9_2:	;
	popad
	xor	eax,eax
	stc
	ret
InitVESA	endp


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
RelVESA	proc	near
	pushad
;
;Check if INT 10h was patched.
;
	assume ds:nothing
	cmp	cs:w[OldInt10h+4],0	;was int vector patched?
	assume ds:DGROUP
	jz	NotPatched
;
;Restore the old INT 10h handler.
;
	push	ds
	mov	ds,CodeSegAlias
	ifdef CWNEAR
	assume ds:_SEG
	else
	assume ds:_TEXT
	endif
	mov	edx,d[OldInt10h]
	mov	cx,w[OldInt10h+4]
	mov	w[OldInt10h+4],0	;Clear incase called again.
	assume ds:DGROUP
	pop	ds
	mov	bl,10h
	sys	SetVect		;release int patch.
	;
NotPatched:	popad
	clc
	ret
RelVESA	endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Int10hHandler	proc	near
;
;Check if its a VESA function first, then see if its a function we need to
;translate pointers for, chain to old handler if its not.
;
	cmp	ah,4fh		;VESA function?
	jnz	NotVESA
	or	al,al		;INFO?
	jnz	NotINFO
	call	VESA_00
	jmp	Done
	;
NotInfo:	cmp	al,1		;Mode INFO?
	jnz	NotModeInfo
	call	VESA_01
	jmp	Done
	;
NotModeInfo:	cmp	al,2		;Set mode?
	jnz	NotModeSet
	call	VESA_02
	jmp	Done
	;
NotModeSet:	cmp	al,4		;State size/save/restore?
	jnz	NotVESA
	call	VESA_04
	jmp	Done
	;
NotVESA:	;
	assume ds:nothing
	jmp	cs:f[OldInt10h]	;pass it onto previous handler.
	assume ds:DGROUP
	;
Done:	iretd
	;
OldInt10h	df 0
Int10hHandler	endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Get info block.
;
VESA_00	proc	near
	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi
	push	ebp
	push	ds
	push	es
	push	edi
	push	es
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	;
	mov	edi,offset Int10hBuffer	;real mode register buffer.
	mov	RR_EAX[edi],4f00h	;function number.
	mov	ax,TransferReal
	mov	RR_ES[edi],ax	;ES
	mov	eax,d[TransferBuffer]
	mov	RR_EDI[edi],eax	;DI
	mov	bl,10h
	sys	IntXX		;Pass to real mode handler.
	mov	eax,RR_EAX[edi]
	pop	es		;restore target buffer address.
	pop	edi
	cmp	ax,4fh
	jnz	Info_00_0		;not a supported function!
	;
	push	ds
	push	edi
	lds	esi,f[TransferBuffer]
	mov	ecx,256/4
	cld
	rep	movsd		;Copy into real target buffer.
	pop	edi
	pop	ds
	;
	;Set selector for OEMStringPtr
	;
	push	eax
	movzx	edx,es:w[edi+2+VIB_OEMStringPtr]
	shl	edx,4
	mov	ecx,65535
	mov	bx,OEMStringSel
	sys	SetSelDet32
	mov	es:w[edi+2+VIB_OEMStringPtr],bx
	;
	;Set selector for VideoModePtr
	;
	movzx	edx,es:w[edi+2+VIB_VideoModePtr]
	shl	edx,4
	mov	ecx,65535
	mov	bx,VideoModeSel
	sys	SetSelDet32
	mov	es:w[edi+2+VIB_VideoModePtr],bx
	pop	eax
	;
Info_00_0:	pop	es
	pop	ds
	pop	ebp
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	ret
VESA_00	endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Get mode info block.
;
VESA_01	proc	near
	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi
	push	ebp
	push	ds
	push	es
	push	edi
	push	es
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	;
	mov	edi,offset Int10hBuffer	;real mode register buffer.
	mov	RR_EAX[edi],4f01h	;function number.
	mov	RR_ECX[edi],ecx	;mode number.
	mov	ax,TransferReal
	mov	RR_ES[edi],ax	;ES
	mov	eax,d[TransferBuffer]
	mov	RR_EDI[edi],eax	;DI
	mov	bl,10h
	sys	IntXX		;Pass to real mode handler.
	mov	eax,RR_EAX[edi]
	pop	es		;restore target buffer address.
	pop	edi
	cmp	ax,4fh
	jnz	Info_01_0		;not a supported function!
	;
	push	ds
	push	edi
	lds	esi,f[TransferBuffer]
	mov	ecx,size ModeInfoBlock
	cld
	rep	movsb		;Copy into real target buffer.
	pop	edi
	pop	ds
	;
	;Set selector for WinFuncPtr
	;
	mov	ebx,offset WinFuncHandler	;setup real bank switch address.
	mov	es:w[edi+MIB_WinFuncPtr],bx
	mov	es:w[edi+2+MIB_WinFuncPtr],cs
	;
Info_01_0:	pop	es
	pop	ds
	pop	ebp
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	ret
VESA_01	endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Set mode. Need to patch this so we can make sure the bank switch code address
;is kept upto date.
;
VESA_02	proc	near
	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi
	push	ebp
	push	ds
	push	es
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	;
	mov	NoBankRoutine,-1	;Default to no bank switching.
;
;Now set the mode. Have to use IntXX again otherwise we would call ourselves.
;
	mov	edi,offset Int10hBuffer
	mov	RR_EAX[edi],4f02h	;set mode.
	mov	RR_EBX[edi],ebx
	push	bx
	mov	bl,10h
	sys	IntXX
	pop	bx
	mov	eax,RR_EAX[edi]
	cmp	ax,4fh
	jnz	l02_9		;not a supported function!
	;
	test	bx,100h		;VESA mode number? Skip info
	jz	SetMode		;fetch if its not.
;
;Need to get the mode's info block so we can keep the bank switch code address
;upto date. Have to use IntXX because we want the real mode address.
;
	push	bx
	mov	edi,offset Int10hBuffer
	mov	RR_EAX[edi],4f01h	;get mode info.
	mov	RR_EBX[edi],ebx
	mov	RR_ECX[edi],ebx
	mov	RR_EDI[edi],0
	mov	RR_ESI[edi],0
	mov	RR_EDX[edi],0
	mov	ax,TransferReal
	mov	RR_ES[edi],ax
	mov	RR_DS[edi],ax
	mov	bl,10h
	sys	IntXX
	mov	eax,RR_EAX[edi]
	pop	bx
	cmp	ax,4fh
	jnz	l02_9		;not a supported function!
	push	ax
	push	bx
	push	es
	les	edi,f[TransferBuffer]
	mov	ax,es:w[edi+MIB_WinFuncPtr]	;get bank switch code address.
	mov	bx,es:w[edi+2+MIB_WinFuncPtr]
	pop	es
	mov	edi,offset WinFuncBuffer
	mov	RR_IP[edi],ax
	mov	RR_CS[edi],bx
	pop	bx
	pop	ax
	mov	NoBankRoutine,0	;enable bank code.
SetMode:
;
;All over so return status in AX.
;
l02_9:	pop	es
	pop	ds
	pop	ebp
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	ret
VESA_02	endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
VESA_04	proc	near
	cmp	dl,0
	jz	StateSize
	cmp	dl,1
	jz	StateSave
	cmp	dl,2
	jz	StateRestore
	jmp	Done_04
;
;Just want state buffer size.
;
StateSize:	push	edi
	push	es
	mov	edi,offset Int10hBuffer
	push	ax
	mov	ax,DGROUP
	mov	es,ax
	mov	es:RR_EAX[edi],4f04h
	mov	es:RR_ECX[edi],ecx
	mov	es:RR_EDX[edi],edx
	mov	bl,10h
	sys	IntXX
	mov	eax,es:RR_EAX[edi]
	mov	ebx,es:RR_EBX[edi]
	pop	es
	pop	edi
	jmp	Done_04
;
;Save state function.
;
StateSave:	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi
	push	ebp
	push	ds
	push	es
	push	ebx
	push	ecx
	push	es
	push	ds
	;
	mov	di,DGROUP
	mov	ds,di
	mov	es,di
	mov	edi,offset Int10hBuffer
	mov	ebx,d[TransferBuffer]
	mov	es:RR_EAX[edi],eax
	mov	es:RR_EBX[edi],ebx
	mov	es:RR_ECX[edi],ecx
	mov	es:RR_EDX[edi],edx
	mov	ax,TransferReal
	mov	es:RR_ES[edi],ax
	mov	bl,10h
	sys	IntXX
	mov	eax,es:RR_EAX[edi]
	pop	ds
	pop	es
	pop	ecx
	pop	ebx
	;
	push	eax
	push	ebx
	push	es
	mov	ax,4f04h
	mov	dx,0
	int	10h		;Get buffer size.
	movzx	ecx,bx
	pop	es
	pop	ebx
	pop	eax
	shl	ecx,6		;*64
	mov	edi,ebx		;destination buffer.
	mov	si,DGROUP
	mov	ds,si
	lds	esi,TransferBuffer
	cld
	rep	movsb
	pop	es
	pop	ds
	pop	ebp
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	jmp	Done_04
;
;Restore state function.
;
StateRestore:	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi
	push	ebp
	push	ds
	push	es
	;
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	es
	push	eax
	push	ebx
	push	es
	mov	ax,4f04h
	mov	dx,0
	int	10h		;Get buffer size.
	movzx	ecx,bx
	pop	es
	pop	ebx
	pop	eax
	shl	ecx,6		;*64
	mov	esi,ebx		;source buffer.
	push	es
	mov	di,DGROUP
	mov	ds,di
	les	edi,TransferBuffer
	pop	ds
	cld
	rep	movsb
	pop	es
	pop	edx
	pop	ecx
	pop	ebx
	;
	mov	di,DGROUP
	mov	ds,di
	mov	es,di
	mov	edi,offset Int10hBuffer
	mov	ebx,d[TransferBuffer]
	mov	es:RR_EAX[edi],eax
	mov	es:RR_EBX[edi],ebx
	mov	es:RR_ECX[edi],ecx
	mov	es:RR_EDX[edi],edx
	mov	ax,TransferReal
	mov	es:RR_ES[edi],ax
	mov	bl,10h
	sys	IntXX
	mov	eax,es:RR_EAX[edi]
	;
	pop	es
	pop	ds
	pop	ebp
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	;
Done_04:	ret
VESA_04	endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Bank switch handler.
;
WinFuncHandler	proc	near
	push	ebx
	push	ecx
	push	edi
	push	ds
	push	es
	;
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	cmp	NoBankRoutine,0
	jnz	NoBankCode
	;
	mov	edi,offset WinFuncBuffer
	mov	RR_EBX[edi],ebx
	mov	RR_EDX[edi],edx
	push	RR_IP[edi]
	push	RR_CS[edi]
	sys	FarCallReal
	pop	RR_CS[edi]
	pop	RR_IP[edi]
	mov	eax,RR_EAX[edi]
	mov	edx,RR_EDX[edi]
	;
NoBankCode:	pop	es
	pop	ds
	pop	edi
	pop	ecx
	pop	ebx
	ret
WinFuncHandler	endp


	sdata

NoBankRoutine	db -1
DataSegment	dw 0
TransferBuffer	df 0
TransferReal	dw 0
OEMStringSel	dw 0
VideoModeSel	dw 0
Int10hBuffer	db size RealRegsStruc dup (?)
WinFuncBuffer	db size RealRegsStruc dup (?)


	efile
	end

