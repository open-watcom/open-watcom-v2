	.386
	.model small
	option oldstructs
	option proc:private

	include cw.inc

b	equ	byte ptr
w	equ	word ptr
d	equ	dword ptr
f	equ	fword ptr


;*******************************************************************************
;VESA detection return info structure.
;*******************************************************************************
VgaInfoBlock		struc
 VIB_VESASignature	db 'VESA'		;4 signature bytes
 VIB_VESAVersion	dw ?		;VESA version number
 VIB_OEMStringPtr	dd ?		;Pointer to OEM string
 VIB_Capabilities	db 4 dup (?)		;capabilities of the video environment
 VIB_VideoModePtr	dd ?		;pointer to supported Super VGA modes
 VIB_TotalMemory	dw ?		;memory size in units of 64K
VgaInfoBlock		ends


;*******************************************************************************
;VESA mode information return structure.
;*******************************************************************************
ModeInfoBlock struc
 ;mandatory information
 MIB_ModeAttributes	dw ?		;mode attributes
 MIB_WinAAttributes	db ?		;Window A attributes
 MIB_WinBAttributes	db ?		;Window B attributes
 MIB_WinGranularity	dw ?		;window granularity
 MIB_WinSize		dw ?		;window size
 MIB_WinASegment	dw ?		;Window A start segment
 MIB_WinBSegment	dw ?		;Window B start segment
 MIB_WinFuncPtr	dd ?		;pointer to window function
 MIB_BytesPerScanLine dw ?		;bytes per scan line
 ;optional information
 MIB_XResolution	dw ?		;horizontal resolution
 MIB_YResolution	dw ?		;vertical resolution
 MIB_XCharSize		db ?		;character cell width
 MIB_YCharSize		db ?		;character cell height
 MIB_NumberOfPlanes	db ?		;number of memory planes
 MIB_BitsPerPixel	db ?		;bits per pixel
 MIB_NumberOfBanks	db ?		;number of banks
 MIB_MemoryModel	db ?		;memory model type
 MIB_BankSize		db ?		;bank size in K
ModeInfoBlock	ends


	.code


;*******************************************************************************
;Have a look for VESA INT 10h BIOS extension and if present install a patch to
;make protected mode access transparent. If VESA is enabled then some fields
;will be used as near 32-bit pointers rather than seg16:offset16. Not quite the
;same as streight VESA but more useful to other code. Even if VESA is not
;detected this code will still install it's handler and cope with the
;instalation check function. This ensures that the main code can still check for
;VESA without worrying about buffering.
;
;Usage: InitVESA();
;
;Returns:
;
;All registers preserved.
;
;*******************************************************************************
InitVESA	proc	near
	pushad
;
;Need a pointer to some DOS memory to use as a transfer buffer. Might as well
;use the general purpose extender buffer.
;
	mov	ah,51h
	int	21h		;get current PSP
	push	es
	mov	es,bx
	movzx	eax,es:w[EPSP_TransReal]	;pickup the transfer buffers
	pop	es		;address.
	shl	eax,4
	mov	TransferBuffer,eax
;
;Now see if VESA is present.
;
	mov	edi,offset Int10hBuffer
	mov	Real_EAX[edi],4f00h
	mov	eax,TransferBuffer
	shr	eax,4
	mov	Real_ES[edi],ax
	mov	Real_EDI[edi],0
	mov	bl,10h
	sys	IntXX
	mov	eax,Real_EAX[edi]
	cmp	ax,4fh
	jnz	@@0
	mov	edi,TransferBuffer
	cmp	d[edi],"ASEV"
	jnz	@@0
;
;VESA detected so flag it for later code.
;
	or	VESAPresent,-1
;
;Finally, we need to patch INT 10h so that we can intercept the VESA functions
;supported by this code.
;
@@0:	mov	bl,10h
	sys	GetVect
	mov	d[OldInt10h],edx
	mov	w[OldInt10h+4],cx
	mov	edx,offset Int10hHandler
	mov	cx,cs
	sys	SetVect
;
;Exit with all registers bar flags preserved.
;
	popad
	ret
InitVESA	endp


;*******************************************************************************
;Release VESA patch if installed. Can be called safely even if the InitVESA call
;was never made.
;
;Usage: RelVESA();
;
;Returns:
;
;ALL registers preserved.
;
;*******************************************************************************
RelVESA	proc	near
	pushad
;
;Check if INT 10h was patched.
;
	mov	cx,w[OldInt10h+4]
	or	cx,cx		;was int vector patched?
	jz	@@0
	mov	edx,d[OldInt10h]
	mov	bl,10h
	sys	SetVect
	mov	w[OldInt10h+4],0
;
@@0:	popad
	ret
RelVESA	endp


;*******************************************************************************
;This is the INT 10h VESA function intercept routine. It just checks the
;function number and if it's a VESA function that requires translation calls the
;relavent routine. If VESA isn't present then the only valid function is the
;detection function (4F00h).
;*******************************************************************************
Int10hHandler	proc	near
;
;Check if it's a none VESA mode set, disable bank switching if it is.
;
	or	ah,ah
	jnz	@@3
	push	eax
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	mov	BankFlags,0
	pop	ds
	pop	eax
	jmp	@@8
;
;Check if it's a VESA function.
;
@@3:	cmp	ah,4fh
	jnz	@@8
;
;Check if it's the detection function.
;
	or	al,al
	jnz	@@0
	push	ds
	push	es
	push	ax
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	pop	ax
	call	VESA_00
	pop	es
	pop	ds
	jmp	@@9
;
;Check if VESA is present. If not then all other functions should be passed to
;the origional handler.
;
@@0:	assume ds:nothing
	cmp	cs:VESAPresent,0
	assume ds:DGROUP
	jz	@@8
;
;Check if it's the mode info function.
;
	cmp	al,1
	jnz	@@1
	push	ds
	push	es
	push	ax
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	pop	ax
	call	VESA_01
	pop	es
	pop	ds
	jmp	@@9
;
;Check if it's the set mode function.
;
@@1:	cmp	al,2
	jnz	@@2
	push	ds
	push	es
	push	ax
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	pop	ax
	call	VESA_02
	pop	es
	pop	ds
	jmp	@@9
;
;Check if it's a state size/save/restore function.
;
@@2:	cmp	al,4
	jnz	@@8
	push	ds
	push	es
	push	ax
	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	pop	ax
	call	VESA_04
	pop	es
	pop	ds
	jmp	@@9
;
;Not a function that needs translation so pass control to origional handler.
;
@@8:	assume ds:nothing
	jmp	cs:f[OldInt10h]
	assume ds:DGROUP
;
;Function has been dealt with so return results to caller.
;
@@9:	iretd
Int10hHandler	endp


;*******************************************************************************
;Get master info block. This function is supported even if there is no real VESA
;support so that the caller can still pass a protected mode memory pointer.
;*******************************************************************************
VESA_00	proc	near
	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi
;
;Better check if VESA is really supported. Forces detection to fail by returning
;EAX un-touched if no VESA without passing down to real mode.
;
	cmp	VESAPresent,0
	jz	@@9
;
;Get real VESA function to fill our own buffer.
;
	push	edi
	mov	edi,offset Int10hBuffer
	mov	Real_EAX[edi],4f00h
	mov	eax,TransferBuffer
	shr	eax,4
	mov	Real_ES[edi],ax
	mov	Real_EDI[edi],0
	mov	bl,10h
	sys	IntXX
	mov	eax,Real_EAX[edi]
	movzx	eax,ax
	pop	edi
	cmp	eax,4fh
	jnz	@@9
;
;Copy data provided into user supplied buffer.
;
	mov	esi,TransferBuffer
	mov	ecx,256/4
	cld
	push	edi
	rep	movsd
	pop	edi
;
;Set linear address of OEMStringPtr
;
	xor	edx,edx
	mov	dx,w[edi+2+VIB_OEMStringPtr]
	shl	edx,4
	xor	ecx,ecx
	mov	cx,w[edi+VIB_OEMStringPtr]
	add	edx,ecx
	mov	d[edi+VIB_OEMStringPtr],edx
;
;Set linear address of VideoModePtr
;
	xor	edx,edx
	mov	dx,w[edi+2+VIB_VideoModePtr]
	shl	edx,4
	xor	ecx,ecx
	mov	cx,w[edi+VIB_VideoModePtr]
	add	edx,ecx
	mov	d[edi+VIB_VideoModePtr],edx
;
@@9:	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	ret
VESA_00	endp


;*******************************************************************************
;Get mode info block.
;*******************************************************************************
VESA_01	proc	near
	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi
;
;Get real VESA function to fill our own buffer.
;
	push	edi
	mov	edi,offset Int10hBuffer
	mov	Real_EAX[edi],4f01h
	mov	Real_ECX[edi],ecx
	mov	eax,TransferBuffer
	shr	eax,4
	mov	Real_ES[edi],ax
	mov	Real_EDI[edi],0
	mov	bl,10h
	sys	IntXX
	mov	eax,Real_EAX[edi]
	movzx	eax,ax
	pop	edi
;
;Check it was a supported mode we asked for info about.
;
	cmp	eax,4fh
	jnz	@@9
;
;Copy data provided into user supplied buffer.
;
	push	ecx
	mov	esi,TransferBuffer
	mov	ecx,size ModeInfoBlock
	cld
	push	edi
	rep	movsb
	pop	edi
	pop	ecx
;
;Set linear address of WinFuncPtr
;
	mov	ebx,offset WinFuncHandler
	mov	d[edi+MIB_WinFuncPtr],ebx
;
;Check if details need forceing due to bad BIOS.
;
	push	eax
	mov	esi,offset Forced32kList
	mov	bx,15
@@32k:	lodsw
	cmp	ax,cx
	jz	@@Forced
	cmp	ax,-1
	jnz	@@32k
	mov	esi,offset Forced64kList
	inc	bx
@@64k:	lodsw
	cmp	ax,cx
	jz	@@Forced
	cmp	ax,-1
	jnz	@@64k
	movzx	bx,MIB_BitsPerPixel[edi]
@@Forced:	mov	MIB_BitsPerPixel[edi],bl
	pop	eax
;
@@9:	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	ret
VESA_01	endp


;*******************************************************************************
;Set mode. Need to patch this so we can make sure the bank switch code address
;is kept up to date.
;*******************************************************************************
VESA_02	proc	near
	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi
;
;Disable bank switching incase this request fails.
;
	mov	BankFlags,0
;
;Get real VESA function to set the mode.
;
	mov	edi,offset Int10hBuffer
	mov	Real_EAX[edi],4f02h
	mov	Real_EBX[edi],ebx
	push	ebx
	mov	bl,10h
	sys	IntXX
	pop	ebx
	mov	eax,Real_EAX[edi]
	movzx	eax,ax
;
;Check it was a supported mode we asked for.
;
	cmp	eax,4fh
	jnz	@@9
;
;Need to get the mode's info block so we can keep the bank switch code address
;upto date.
;
	mov	edi,offset Int10hBuffer
	mov	Real_EAX[edi],4f01h
	mov	Real_ECX[edi],ebx
	mov	Real_EDI[edi],0
	mov	eax,TransferBuffer
	shr	eax,4
	mov	Real_ES[edi],ax
	mov	bl,10h
	sys	IntXX
	mov	eax,Real_EAX[edi]
	movzx	eax,ax
;
;This shouldn't really be able to fail but I'll check anyway.
;
	cmp	eax,4fh
	jnz	@@9
;
;Copy the WinFuncPtr to somewhere useful.
;
	mov	edi,TransferBuffer
	mov	eax,d[edi+MIB_WinFuncPtr]
	mov	edi,offset WinFuncBuffer
	mov	Real_IP[edi],ax
	shr	eax,16
	mov	Real_CS[edi],ax
;
;Enable bank switching now we know where to go for it.
;
	or	BankFlags,-1		;enable bank code.
	mov	eax,4fh
;
@@9:	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	ret
VESA_02	endp


;*******************************************************************************
;State size/save/restore function support.
;*******************************************************************************
VESA_04	proc	near
;
;Work out which sub-function is required.
;
	or	dl,dl
	jz	@@StateSize
	dec	dl
	jz	@@StateSave
	dec	dl
	jz	@@StateRestore
	jmp	@@9
;
;Just want state buffer size.
;
@@StateSize:	push	edi
	mov	edi,offset Int10hBuffer
	mov	Real_EAX[edi],4f04h
	mov	Real_ECX[edi],ecx
	mov	Real_EDX[edi],edx
	mov	bl,10h
	sys	IntXX
	mov	eax,Real_EAX[edi]
	mov	ebx,Real_EBX[edi]
	movzx	eax,ax
	movzx	ebx,bx
	pop	edi
	jmp	@@9
;
;Save state function.
;
@@StateSave:	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi
;
;Get real VESA function to fill in our buffer.
;
	push	ebx
	push	ecx
	mov	edi,offset Int10hBuffer
	mov	Real_EAX[edi],eax
	mov	Real_EBX[edi],0
	mov	Real_ECX[edi],ecx
	mov	Real_EDX[edi],edx
	mov	ebx,TransferBuffer
	shr	ebx,4
	mov	Real_ES[edi],ax
	mov	bl,10h
	sys	IntXX
	mov	eax,Real_EAX[edi]
	movzx	eax,ax
	pop	ecx
	pop	ebx
;
;Get the data size.
;
	push	eax
	push	ebx
	mov	ax,4f04h
	mov	dx,0
	int	10h
	mov	ecx,ebx
	pop	ebx
	pop	eax
;
;Now copy the data into the users buffer.
;
	shl	ecx,6
	mov	edi,ebx
	mov	esi,TransferBuffer
	cld
	rep	movsb
;
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	jmp	@@9
;
;Restore state function.
;
@@StateRestore: push ebx
	push	ecx
	push	edx
	push	esi
	push	edi
;
	push	ecx
	push	edx
;
;Need the buffer size so we know how much data to copy into low memory.
;
	push	eax
	push	ebx
	mov	ax,4f04h
	mov	dx,0
	int	10h
	mov	ecx,ebx
	pop	ebx
	pop	eax
	shl	ecx,6
;
;Now copy the data into low memory ready to send to real VESA function.
;
	mov	esi,ebx
	mov	edi,TransferBuffer
	cld
	rep	movsb
	pop	edx
	pop	ecx
;
;Now get the real VESA function to restore the state.
;
	mov	edi,offset Int10hBuffer
	mov	Real_EAX[edi],4f04h
	mov	Real_EBX[edi],0
	mov	Real_ECX[edi],ecx
	mov	Real_EDX[edi],edx
	mov	eax,TransferBuffer
	shr	eax,4
	mov	Real_ES[edi],ax
	mov	bl,10h
	sys	IntXX
	mov	eax,Real_EAX[edi]
	movzx	eax,ax
;
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
;
@@9:	ret
VESA_04	endp


;*******************************************************************************
;Bank switch handler. If bank switching isn't enabled then the request is
;ignored.
;*******************************************************************************
WinFuncHandler	proc	near
	push	edi
;
;See if bank switching is allowed.
;
	cmp	BankFlags,0
	jz	@@9
;
;Call the real mode bank switch handler.
;
	mov	edi,offset WinFuncBuffer
	mov	Real_EAX[edi],4f05h
	mov	Real_EBX[edi],ebx
	mov	Real_EDX[edi],edx
	sys	FarCallReal
	mov	eax,Real_EAX[edi]
	mov	edx,Real_EDX[edi]
;
@@9:	pop	edi
	ret
WinFuncHandler	endp


	.data


OldInt10h	df 0
BankFlags	db 0
VESAPresent	db 0
TransferBuffer	dd 0

Int10hBuffer	db size RealRegsStruc dup (?)
WinFuncBuffer	db size RealRegsStruc dup (?)

Forced32kList	dw 10dh, 110h, 113h, 116h, -1
Forced64kList	dw 10eh, 111h, 114h, 117h, -1


;*******************************************************************************
;These are required to have the VESA patch applied automaticaly.
;*******************************************************************************
XI	segment word public 'DATA'
	db 0,15
	dd InitVESA
XI	ends
YI	segment word public 'DATA'
	db 0,15
	dd RelVESA
YI	ends


	end

