	.386
	.model small
	.stack 1024
	option oldstructs
	include cw.inc

b	equ	byte ptr
w	equ	word ptr
d	equ	dword ptr

NewHeaderStruc	struc
NewID	db '3P'	;identifier.
NewSize	dd 0		;byte size of 3P section of file.
NewLength	dd 0		;byte size of exe image data.
NewAlloc	dd 0		;byte size of program.
NewSegments	dw 0		;number of segment definitions.
NewRelocs	dd 0		;number of relocation table entries.
NewEntryEIP	dd 0		;entry offset.
NewEntryCS	dw 0		;segment list entry number for entry CS.
NewEntryESP	dd 0		;ESP offset.
NewEntrySS	dw 0		;segment list entry number for SS.
NewFlags	dd 0		;Control flags.
NewAutoStack	dd 0		;Auto stack size.
NewAutoDS	dw 0		;Auto DS segment number +1
NewExports	dd 0		;Length of EXPORT section.
NewImports	dd 0		;Length of IMPORT section.
NewImportModCnt dd 0		;Number of IMPORT modules.
NewReserved	db 64-NewReserved dup (?)
NewHeaderStruc	ends

	.code

;*******************************************************************************
;
;Main entry point..
;
;*******************************************************************************
Start	proc	near
	mov	PSPSegment,es
	push	ds
	pop	es
	mov	ax,0ff00h
	mov	dx,0078h
	int	21h
;
;Say hello.
;
	mov	edx,offset Copyright
	mov	ah,9
	int	21h
;
;Parse command line.
;
	call	ReadCommand
	mov	ErrorNumber,1
	cmp	eax,1
	jc	@@exit
	cmp	OptionTable+"L",0
	jnz	@@8
	cmp	eax,2
	jc	@@exit
;
;Do whatever it is we're being asked to do.
;
@@8:	cmp	OptionTable+"A",0
	jz	@@0
	call	AddModule
	jmp	@@exit
@@0:	cmp	OptionTable+"D",0
	jz	@@1
	call	DeleteModule
	jmp	@@exit
@@1:	cmp	OptionTable+"L",0
	jz	@@2
	call	ListModules
	jmp	@@exit
@@2:	cmp	OptionTable+"E",0
	jz	@@3
	call	ExtractModule
	jmp	@@exit
@@3:	cmp	OptionTable+"I",0
	jz	@@4
	call	ImportObjModule
	jmp	@@exit
@@4:	mov	ErrorNumber,2
;
;Make sure temp file is deleted.
;
@@exit:	mov	edx,offset TMPName
	mov	ah,41h
	int	21h
;
;Display error message and exit.
;
	xor	edx,edx
	mov	dl,ErrorNumber
	mov	edx,[Errorlist+edx*4]
	mov	ah,9
	int	21h
	mov	al,ErrorNumber
	mov	ah,4ch
	int	21h
Start	endp


;*******************************************************************************
;
;Add or update a module.
;
;*******************************************************************************
AddModule	proc	near
;
;Get new modules name.
;
	;
	;Open the file.
	;
	mov	edx,OptionPointers+4
	mov	ax,3d02h
	int	21h
	mov	ErrorNumber,3
	jc	@@9
	mov	ModHandle,eax
	mov	ebx,eax
	;
@@0:	;Read header so we can figure out the format.
	;
	mov	edx,offset NewHeader
	mov	ecx,size NewHeader
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	;Check the format.
	;
	cmp	w[NewID+edx],"P3"
	jz	@@2
	cmp	w[NewID+edx],"ZM"
	mov	ErrorNumber,5
	jnz	@@9
	;
	;Have an MZ stub so find out how long it is and skip it.
	;
	xor	eax,eax
	mov	ax,[edx+2+2]
	dec	eax
	shl	eax,8
	xor	ecx,ecx
	mov	cx,[edx+2]
	or	ecx,ecx
	jnz	@@1
	add	eax,512
@@1:	add	ecx,eax
	sub	ecx,size NewHeaderStruc
	mov	dx,cx
	shr	ecx,16
	mov	ax,4201h
	int	21h
	jmp	@@0
@@2:	mov	edi,edx
	;
	;Check this module has exports.
	;
	cmp	NewExports[edi],0
	mov	ErrorNumber,6
	jz	@@9
	;
	;Preserve offset of this modules real start.
	;
	xor	cx,cx
	xor	dx,dx
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	sub	edx,size NewHeaderStruc
	mov	ModOffset,edx
	;
	;Skip segment definitions.
	;
	movzx	edx,NewSegments[edi]
	shl	edx,3
	sys	cwcInfo
	jc	@@3p0
	mov	edx,eax
@@3p0:	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	;
	;Skip relocations.
	;
	mov	edx,NewRelocs[edi]
	shl	edx,2
	or	edx,edx
	jz	@@3p1
	sys	cwcInfo
	jc	@@3p1
	mov	edx,eax
@@3p1:	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	;
	;Load export details.
	;
	mov	ecx,NewExports[edi]
	sys	GetMemLinear32
	mov	ErrorNumber,7
	jc	@@9
	mov	edx,ecx
	sys	cwcInfo
	jc	@@3p2
	push	edi
	mov	edi,esi
	sys	cwcLoad
	pop	edi
	mov	ErrorNumber,4
	jc	@@9
	jmp	@@3p4
@@3p2:	mov	ecx,edx
	mov	edx,esi
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
@@3p4:	;Copy the module name.
	;
	push	esi
	add	esi,[esi+4]		;get offset of module name.
	movzx	ecx,b[esi]		;get name length.
	inc	ecx
	mov	edi,offset ModName
	rep	movsb
	pop	esi
	;
	;Clean this bit up.
	;
	sys	RelMemLinear32
	mov	ecx,ModOffset
	mov	dx,cx
	shr	ecx,16
	mov	ax,4200h
	int	21h
	mov	eax,d[NewHeader+NewSize]
	mov	ModLength,eax
;
;Open/Create the library.
;
	mov	edx,OptionPointers
	mov	ax,3d02h
	int	21h
	jnc	@@3
	mov	ax,3c00h
	xor	cx,cx
	int	21h
	mov	ErrorNumber,8
	jc	@@9
	mov	LibHandle,eax
	jmp	@@11
@@3:	mov	LibHandle,eax
	mov	ebx,eax
;
;Scan modules for name that matches the one we're adding/replaceing.
;
@@4:	mov	edx,offset NewHeader
	mov	ecx,size NewHeaderStruc
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	or	eax,eax		;EOF?
	jz	@@10
	cmp	eax,ecx
	jnz	@@9
	cmp	w[NewID+edx],"P3"
	jz	@@6
	cmp	w[NewID+edx],"ZM"
	jnz	@@10		;assume 3P EOF
	;
	;Have an MZ stub so find out how long it is and skip it.
	;
	xor	eax,eax
	mov	ax,[edx+2+2]
	dec	eax
	shl	eax,8
	xor	ecx,ecx
	mov	cx,[edx+2]
	or	ecx,ecx
	jnz	@@5
	add	eax,512
@@5:	add	ecx,eax
	sub	ecx,size NewHeaderStruc
	mov	dx,cx
	shr	ecx,16
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	mov	LibOffset,edx
	jmp	@@4
@@6:	mov	edi,edx
	;
	;Store this modules file offset in case it has the right name.
	;
	xor	cx,cx
	xor	dx,dx
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	sub	edx,size NewHeaderStruc
	mov	LibOffset,edx
	;
	;Skip segment definitions.
	;
	movzx	edx,NewSegments[edi]
	shl	edx,3
	sys	cwcInfo
	jc	@@03p0
	mov	edx,eax
@@03p0:	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	;
	;Skip relocations.
	;
	mov	edx,NewRelocs[edi]
	shl	edx,2
	or	edx,edx
	jz	@@03p1
	sys	cwcInfo
	jc	@@03p1
	mov	edx,eax
@@03p1:	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	;
	;Load export details.
	;
	mov	ecx,NewExports[edi]
	sys	GetMemLinear32
	mov	ErrorNumber,7
	jc	@@9
	mov	edx,ecx
	sys	cwcInfo
	jc	@@03p2
	push	edi
	mov	edi,esi
	sys	cwcLoad
	pop	edi
	mov	ErrorNumber,4
	jc	@@9
	jmp	@@03p4
@@03p2:	mov	ecx,edx
	mov	edx,esi
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
@@03p4:	;Copy the module name.
	;
	push	esi
	add	esi,[esi+4]		;get offset of module name.
	movzx	ecx,b[esi]		;get name length.
	inc	ecx
	mov	edi,offset LibName
	rep	movsb
	pop	esi
	;
	;Lose export memory.
	;
	sys	RelMemLinear32
	;
	;Compare the module names.
	;
	mov	esi,offset ModName
	mov	edi,offset LibName
	xor	ecx,ecx
	mov	cl,[esi]
	cmp	cl,[edi]
	jnz	@@7
	inc	esi
	inc	edi
	repe	cmpsb
	jz	@@8
	;
@@7:	;Move past this module.
	;
	mov	edx,d[NewHeader+NewSize]
	add	edx,LibOffset
	mov	LibOffset,edx
	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4200h
	int	21h
	jmp	@@4
	;
@@8:	;Set things up ready for this module to be replaced.
	;
	mov	eax,d[NewHeader+NewSize]
	mov	LibIgnore,eax
	;
	;Find out how much trailing data there is.
	;
	xor	cx,cx
	xor	dx,dx
	mov	ax,4202h
	int	21h
	shl	edx,16
	mov	dx,ax
	sub	edx,LibOffset
	sub	edx,d[NewHeader+NewSize]
	mov	LibTrail,edx
	jmp	@@11
;
;Either EOF'd before we found a matching name or we reached the end of the 3P
;chain.
;
	;
@@10:	;Set current file offset as copy to point.
	;
	xor	cx,cx
	xor	dx,dx
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	mov	LibOffset,edx
	;
	;Anything else should be copied strieght through.
	;
	xor	cx,cx
	xor	dx,dx
	mov	ax,4202h
	int	21h
	shl	edx,16
	mov	dx,ax
	sub	edx,LibOffset
	mov	LibTrail,edx
;
;Time to generate the updated library.
;
	;
@@11:	;Create a dummy output file.
	;
	mov	edx,offset TMPName
	xor	cx,cx
	mov	ax,3c00h
	int	21h
	mov	ErrorNumber,9
	jc	@@9
	mov	TMPHandle,eax
	mov	ebx,eax
	;
	;Copy leading data.
	;
	xor	dx,dx
	xor	cx,cx
	mov	ebx,LibHandle
	mov	ax,4200h
	int	21h
	mov	ebp,LibOffset
@@12:	or	ebp,ebp
	jz	@@14
	mov	ecx,4096
	cmp	ecx,ebp
	jc	@@13
	mov	ecx,ebp
@@13:	mov	edx,offset IOBuffer
	mov	ebx,LibHandle
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	ebx,TMPHandle
	mov	ah,40h
	int	21h
	jc	@@9
	mov	ErrorNumber,10
	cmp	eax,ecx
	jnz	@@9
	sub	ebp,eax
	jmp	@@12
	;
@@14:	;Copy new module.
	;
	mov	edx,ModOffset
	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ebx,ModHandle
	mov	ax,4200h
	int	21h
	mov	ebp,ModLength
@@15:	or	ebp,ebp
	jz	@@17
	mov	ecx,4096
	cmp	ecx,ebp
	jc	@@16
	mov	ecx,ebp
@@16:	mov	edx,offset IOBuffer
	mov	ebx,ModHandle
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	ebx,TMPHandle
	mov	ah,40h
	int	21h
	jc	@@9
	mov	ErrorNumber,10
	cmp	eax,ecx
	jnz	@@9
	sub	ebp,eax
	jmp	@@15
	;
@@17:	;Copy any trailing library data.
	;
	mov	ebx,LibHandle
	mov	edx,LibIgnore
	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	mov	ebp,LibTrail
@@18:	or	ebp,ebp
	jz	@@20
	mov	ecx,4096
	cmp	ecx,ebp
	jc	@@19
	mov	ecx,ebp
@@19:	mov	edx,offset IOBuffer
	mov	ebx,LibHandle
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	ebx,TMPHandle
	mov	ah,40h
	int	21h
	jc	@@9
	mov	ErrorNumber,10
	cmp	eax,ecx
	jnz	@@9
	sub	ebp,eax
	jmp	@@18
;
;Close all the files.
;
@@20:	mov	ebx,ModHandle
	mov	ah,3eh
	int	21h
	mov	ebx,LibHandle
	mov	ah,3eh
	int	21h
	mov	ebx,TMPHandle
	mov	ah,3eh
	int	21h
;
;Delete the origional library file.
;
	mov	edx,OptionPointers
	mov	ah,41h
	int	21h
;
;Rename temp file as library file.
;
	mov	edx,offset TMPName
	mov	edi,OptionPointers
	mov	ah,56h
	int	21h
;
;Return success to caller.
;
	mov	ErrorNumber,0
@@9:	ret
AddModule	endp


;*******************************************************************************
;
;Delete a module.
;
;*******************************************************************************
DeleteModule	proc	near
;
;Get module name to delete in right format.
;
	mov	esi,OptionPointers+4
	xor	ecx,ecx
@@0:	mov	al,[esi]
	or	al,al
	jz	@@1
	inc	esi
	inc	ecx
	jmp	@@0
@@1:	mov	esi,OptionPointers+4
	mov	edi,offset ModName
	mov	b[edi],cl
	inc	edi
@@2:	mov	al,[esi]
	cmp	al,61h		; 'a'
	jb	@@3
	cmp	al,7Ah		; 'z'
	ja	@@3
	and	al,5Fh		;convert to upper case.
@@3:	mov	[edi],al
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@2
;
;Open the library.
;
	mov	edx,OptionPointers
	mov	ax,3d02h
	int	21h
	mov	ErrorNumber,11
	jc	@@9
	mov	LibHandle,eax
	mov	ebx,eax
;
;Scan modules for name that matches the one we're deleteing.
;
@@4:	mov	edx,offset NewHeader
	mov	ecx,size NewHeaderStruc
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	mov	ErrorNumber,11
	or	eax,eax		;EOF?
	jz	@@9
	mov	ErrorNumber,4
	cmp	eax,ecx
	jnz	@@9
	cmp	w[NewID+edx],"P3"
	jz	@@6
	mov	ErrorNumber,11
	cmp	w[NewID+edx],"ZM"
	jnz	@@9		;assume 3P EOF
	;
	;Have an MZ stub so find out how long it is and skip it.
	;
	xor	eax,eax
	mov	ax,[edx+2+2]
	dec	eax
	shl	eax,8
	xor	ecx,ecx
	mov	cx,[edx+2]
	or	ecx,ecx
	jnz	@@5
	add	eax,512
@@5:	add	ecx,eax
	sub	ecx,size NewHeaderStruc
	mov	dx,cx
	shr	ecx,16
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	mov	LibOffset,edx
	jmp	@@4
@@6:	mov	edi,edx
	;
	;Store this modules file offset in case it has the right name.
	;
	xor	cx,cx
	xor	dx,dx
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	sub	edx,size NewHeaderStruc
	mov	LibOffset,edx
	;
	;Skip segment definitions.
	;
	movzx	edx,NewSegments[edi]
	shl	edx,3
	sys	cwcInfo
	jc	@@03p0
	mov	edx,eax
@@03p0:	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	;
	;Skip relocations.
	;
	mov	edx,NewRelocs[edi]
	shl	edx,2
	or	edx,edx
	jz	@@03p1
	sys	cwcInfo
	jc	@@03p1
	mov	edx,eax
@@03p1:	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	;
	;Load export details.
	;
	mov	ecx,NewExports[edi]
	sys	GetMemLinear32
	mov	ErrorNumber,7
	jc	@@9
	mov	edx,ecx
	sys	cwcInfo
	jc	@@03p2
	push	edi
	mov	edi,esi
	sys	cwcLoad
	pop	edi
	mov	ErrorNumber,4
	jc	@@9
	jmp	@@03p4
@@03p2:	mov	ecx,edx
	mov	edx,esi
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
@@03p4:	;Copy the module name.
	;
	push	esi
	add	esi,[esi+4]		;get offset of module name.
	movzx	ecx,b[esi]		;get name length.
	inc	ecx
	mov	edi,offset LibName
	rep	movsb
	pop	esi
	;
	;Lose export memory.
	;
	sys	RelMemLinear32
	;
	;Compare the module names.
	;
	mov	esi,offset ModName
	mov	edi,offset LibName
	xor	ecx,ecx
	mov	cl,[esi]
	cmp	cl,[edi]
	jnz	@@7
	inc	esi
	inc	edi
	repe	cmpsb
	jz	@@8
	;
@@7:	;Move past this module.
	;
	mov	edx,d[NewHeader+NewSize]
	add	edx,LibOffset
	mov	LibOffset,edx
	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4200h
	int	21h
	jmp	@@4
	;
@@8:	;Set things up ready for this module to be deleted.
	;
	mov	eax,d[NewHeader+NewSize]
	mov	LibIgnore,eax
	;
	;Find out how much trailing data there is.
	;
	xor	cx,cx
	xor	dx,dx
	mov	ax,4202h
	int	21h
	shl	edx,16
	mov	dx,ax
	sub	edx,LibOffset
	sub	edx,d[NewHeader+NewSize]
	mov	LibTrail,edx
;
;Time to generate the updated library.
;
	;
@@11:	;Create a dummy output file.
	;
	mov	edx,offset TMPName
	xor	cx,cx
	mov	ax,3c00h
	int	21h
	mov	ErrorNumber,9
	jc	@@9
	mov	TMPHandle,eax
	mov	ebx,eax
	;
	;Copy leading data.
	;
	xor	dx,dx
	xor	cx,cx
	mov	ebx,LibHandle
	mov	ax,4200h
	int	21h
	mov	ebp,LibOffset
@@12:	or	ebp,ebp
	jz	@@14
	mov	ecx,4096
	cmp	ecx,ebp
	jc	@@13
	mov	ecx,ebp
@@13:	mov	edx,offset IOBuffer
	mov	ebx,LibHandle
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	ebx,TMPHandle
	mov	ah,40h
	int	21h
	jc	@@9
	mov	ErrorNumber,10
	cmp	eax,ecx
	jnz	@@9
	sub	ebp,eax
	jmp	@@12
	;
@@14:	;Copy any trailing library data.
	;
	mov	ebx,LibHandle
	mov	edx,LibIgnore
	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	mov	ebp,LibTrail
@@18:	or	ebp,ebp
	jz	@@20
	mov	ecx,4096
	cmp	ecx,ebp
	jc	@@19
	mov	ecx,ebp
@@19:	mov	edx,offset IOBuffer
	mov	ebx,LibHandle
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	ebx,TMPHandle
	mov	ah,40h
	int	21h
	jc	@@9
	mov	ErrorNumber,10
	cmp	eax,ecx
	jnz	@@9
	sub	ebp,eax
	jmp	@@18
;
;Close all the files.
;
@@20:	mov	ebx,LibHandle
	mov	ah,3eh
	int	21h
	mov	ebx,TMPHandle
	mov	ah,3eh
	int	21h
;
;Delete the origional library file.
;
	mov	edx,OptionPointers
	mov	ah,41h
	int	21h
;
;Rename temp file as library file.
;
	mov	edx,offset TMPName
	mov	edi,OptionPointers
	mov	ah,56h
	int	21h
;
;Return success to caller.
;
	mov	ErrorNumber,0
@@9:	ret
DeleteModule	endp


;*******************************************************************************
;
;List modules and exports/imports within a module.
;
;*******************************************************************************
ListModules	proc	near
;
;Open the library.
;
	mov	edx,OptionPointers
	mov	ax,3d02h
	int	21h
	mov	ErrorNumber,11
	jc	@@9
	mov	LibHandle,eax
	mov	ebx,eax
;
;Scan modules for names.
;
@@4:	mov	edx,offset NewHeader
	mov	ecx,size NewHeaderStruc
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	or	eax,eax		;EOF?
	jz	@@20
	mov	ErrorNumber,4
	cmp	eax,ecx
	jnz	@@9
	cmp	w[NewID+edx],"P3"
	jz	@@6
	cmp	w[NewID+edx],"ZM"
	jnz	@@20		;assume 3P EOF
	;
	;Have an MZ stub so find out how long it is and skip it.
	;
	xor	eax,eax
	mov	ax,[edx+2+2]
	dec	eax
	shl	eax,8
	xor	ecx,ecx
	mov	cx,[edx+2]
	or	ecx,ecx
	jnz	@@5
	add	eax,512
@@5:	add	ecx,eax
	sub	ecx,size NewHeaderStruc
	mov	dx,cx
	shr	ecx,16
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	mov	LibOffset,edx
	jmp	@@4
@@6:	mov	edi,edx
	;
	;Store this modules file offset.
	;
	xor	cx,cx
	xor	dx,dx
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	sub	edx,size NewHeaderStruc
	mov	LibOffset,edx
	;
	;Skip segment definitions.
	;
	movzx	edx,NewSegments[edi]
	shl	edx,3
	sys	cwcInfo
	jc	@@03p0
	mov	edx,eax
@@03p0:	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	;
	;Skip relocations.
	;
	mov	edx,NewRelocs[edi]
	shl	edx,2
	or	edx,edx
	jz	@@03p1
	sys	cwcInfo
	jc	@@03p1
	mov	edx,eax
@@03p1:	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	;
	;Load export details.
	;
	mov	ecx,NewExports[edi]
	sys	GetMemLinear32
	mov	ErrorNumber,7
	jc	@@9
	mov	edx,ecx
	sys	cwcInfo
	jc	@@03p2
	push	edi
	mov	edi,esi
	sys	cwcLoad
	pop	edi
	mov	ErrorNumber,4
	jc	@@9
	jmp	@@03p4
@@03p2:	mov	ecx,edx
	mov	edx,esi
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
@@03p4:	;Copy the module name.
	;
	push	esi
	add	esi,[esi+4]		;get offset of module name.
	movzx	ecx,b[esi]		;get name length.
	inc	esi
	mov	edi,offset LibName
	rep	movsb
	mov	b[edi],13
	mov	b[edi+1],10
	mov	b[edi+2],"$"
	pop	esi
	;
	;Print the module name.
	;
	mov	edx,offset LibName
	mov	ah,9
	int	21h
	;
	;Now print all the exported symbols.
	;
	push	esi
	mov	edx,esi
	mov	ebp,[esi]		;get number of entries.
	add	esi,4+4
@@10:	push	esi
	push	edx
	mov	esi,[esi]
	add	esi,edx
	add	esi,4+2		;skip value.
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset IOBuffer
	mov	d[edi],"    "
	add	edi,4
	rep	movsb
	mov	b[edi],13
	mov	b[edi+1],10
	mov	b[edi+2],"$"
	mov	edx,offset IOBuffer
	mov	ah,9
	int	21h
	pop	edx
	pop	esi
	add	esi,4
	dec	ebp
	jnz	@@10
	pop	esi
	;
	mov	edx,offset CarriageReturn
	mov	ah,9
	int	21h
	;
	;Lose export memory.
	;
	sys	RelMemLinear32
	;
	;Move past this module.
	;
	mov	edx,d[NewHeader+NewSize]
	add	edx,LibOffset
	mov	LibOffset,edx
	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4200h
	int	21h
	jmp	@@4
;
;Close all the files.
;
@@20:	mov	ebx,LibHandle
	mov	ah,3eh
	int	21h
;
;Return success to caller.
;
	mov	ErrorNumber,0
@@9:	ret
ListModules	endp


;*******************************************************************************
;
;Extract a module.
;
;*******************************************************************************
ExtractModule	proc	near
;
;Get module name to extract in right format.
;
	mov	esi,OptionPointers+4
	xor	ecx,ecx
@@0:	mov	al,[esi]
	or	al,al
	jz	@@1
	inc	esi
	inc	ecx
	jmp	@@0
@@1:	mov	esi,OptionPointers+4
	mov	edi,offset ModName
	mov	b[edi],cl
	inc	edi
@@2:	mov	al,[esi]
	cmp	al,61h		; 'a'
	jb	@@3
	cmp	al,7Ah		; 'z'
	ja	@@3
	and	al,5Fh		;convert to upper case.
@@3:	mov	[edi],al
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@2
;
;Open the library.
;
	mov	edx,OptionPointers
	mov	ax,3d02h
	int	21h
	mov	ErrorNumber,11
	jc	@@9
	mov	LibHandle,eax
	mov	ebx,eax
;
;Scan modules for name that matches the one we're extracting.
;
@@4:	mov	edx,offset NewHeader
	mov	ecx,size NewHeaderStruc
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	mov	ErrorNumber,11
	or	eax,eax		;EOF?
	jz	@@9
	mov	ErrorNumber,4
	cmp	eax,ecx
	jnz	@@9
	cmp	w[NewID+edx],"P3"
	jz	@@6
	mov	ErrorNumber,11
	cmp	w[NewID+edx],"ZM"
	jnz	@@9		;assume 3P EOF
	;
	;Have an MZ stub so find out how long it is and skip it.
	;
	xor	eax,eax
	mov	ax,[edx+2+2]
	dec	eax
	shl	eax,8
	xor	ecx,ecx
	mov	cx,[edx+2]
	or	ecx,ecx
	jnz	@@5
	add	eax,512
@@5:	add	ecx,eax
	sub	ecx,size NewHeaderStruc
	mov	dx,cx
	shr	ecx,16
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	mov	LibOffset,edx
	jmp	@@4
@@6:	mov	edi,edx
	;
	;Store this modules file offset in case it has the right name.
	;
	xor	cx,cx
	xor	dx,dx
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	sub	edx,size NewHeaderStruc
	mov	LibOffset,edx
	;
	;Skip segment definitions.
	;
	movzx	edx,NewSegments[edi]
	shl	edx,3
	sys	cwcInfo
	jc	@@03p0
	mov	edx,eax
@@03p0:	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	;
	;Skip relocations.
	;
	mov	edx,NewRelocs[edi]
	shl	edx,2
	or	edx,edx
	jz	@@03p1
	sys	cwcInfo
	jc	@@03p1
	mov	edx,eax
@@03p1:	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	;
	;Load export details.
	;
	mov	ecx,NewExports[edi]
	sys	GetMemLinear32
	mov	ErrorNumber,7
	jc	@@9
	mov	edx,ecx
	sys	cwcInfo
	jc	@@03p2
	push	edi
	mov	edi,esi
	sys	cwcLoad
	pop	edi
	mov	ErrorNumber,4
	jc	@@9
	jmp	@@03p4
@@03p2:	mov	ecx,edx
	mov	edx,esi
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
@@03p4:	;Copy the module name.
	;
	push	esi
	add	esi,[esi+4]		;get offset of module name.
	movzx	ecx,b[esi]		;get name length.
	inc	ecx
	mov	edi,offset LibName
	rep	movsb
	pop	esi
	;
	;Lose export memory.
	;
	sys	RelMemLinear32
	;
	;Compare the module names.
	;
	mov	esi,offset ModName
	mov	edi,offset LibName
	xor	ecx,ecx
	mov	cl,[esi]
	cmp	cl,[edi]
	jnz	@@7
	inc	esi
	inc	edi
	repe	cmpsb
	jz	@@8
	;
@@7:	;Move past this module.
	;
	mov	edx,d[NewHeader+NewSize]
	add	edx,LibOffset
	mov	LibOffset,edx
	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4200h
	int	21h
	jmp	@@4
	;
@@8:	;Set things up ready for this module to be extracted.
	;
	mov	eax,d[NewHeader+NewSize]
	mov	LibIgnore,eax
	;
	;Find out how much trailing data there is.
	;
	xor	cx,cx
	xor	dx,dx
	mov	ax,4202h
	int	21h
	shl	edx,16
	mov	dx,ax
	sub	edx,LibOffset
	sub	edx,d[NewHeader+NewSize]
	mov	LibTrail,edx
;
;Time to generate the extracted file.
;
	;
@@11:	;Create the output file.
	;
	mov	edx,OptionPointers+4+4
	or	edx,edx
	jnz	@@12
	mov	edx,offset ModName
	mov	esi,offset ModName
	mov	edi,esi
	inc	esi
	movzx	ecx,b[edi]
	cmp	ecx,8
	jc	@@10
	mov	ecx,8
@@10:	rep	movsb
	mov	b[edi],"."
	mov	b[edi+1],"d"
	mov	b[edi+2],"l"
	mov	b[edi+3],"l"
	mov	b[edi+4],0
@@12:	;
	xor	cx,cx
	mov	ax,3c00h
	int	21h
	mov	ErrorNumber,9
	jc	@@9
	mov	TMPHandle,eax
	;
	;Copy desired module.
	;
	mov	ebx,LibHandle
	mov	edx,LibOffset
	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4200h
	int	21h
	mov	ebp,LibIgnore
@@18:	or	ebp,ebp
	jz	@@20
	mov	ecx,4096
	cmp	ecx,ebp
	jc	@@19
	mov	ecx,ebp
@@19:	mov	edx,offset IOBuffer
	mov	ebx,LibHandle
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	ebx,TMPHandle
	mov	ah,40h
	int	21h
	jc	@@9
	mov	ErrorNumber,10
	cmp	eax,ecx
	jnz	@@9
	sub	ebp,eax
	jmp	@@18
;
;Close all the files.
;
@@20:	mov	ebx,LibHandle
	mov	ah,3eh
	int	21h
	mov	ebx,TMPHandle
	mov	ah,3eh
	int	21h
;
;Return success to caller.
;
	mov	ErrorNumber,0
@@9:	ret
ExtractModule	endp


;*******************************************************************************
;
;Create an OBJ file for all modules in specified DLL.
;
;*******************************************************************************
ImportObjModule proc	near
;
;Create the output file.
;
	mov	edx,OptionPointers+4
	xor	cx,cx
	mov	ax,3c00h
	int	21h
	mov	ErrorNumber,12
	jc	@@9
	mov	TMPHandle,eax
	mov	ebx,eax
;
;Open the library.
;
	mov	edx,OptionPointers
	mov	ax,3d00h
	int	21h
	mov	ErrorNumber,11
	jc	@@9
	mov	LibHandle,eax
	mov	ebx,eax
;
;Scan modules for names.
;
@@4:	mov	ebx,LibHandle
	mov	edx,offset NewHeader
	mov	ecx,size NewHeaderStruc
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	or	eax,eax		;EOF?
	jz	@@20
	mov	ErrorNumber,4
	cmp	eax,ecx
	jnz	@@9
	cmp	w[NewID+edx],"P3"
	jz	@@6
	cmp	w[NewID+edx],"ZM"
	jnz	@@20		;assume 3P EOF
	;
	;Have an MZ stub so find out how long it is and skip it.
	;
	xor	eax,eax
	mov	ax,[edx+2+2]
	dec	eax
	shl	eax,8
	xor	ecx,ecx
	mov	cx,[edx+2]
	or	ecx,ecx
	jnz	@@5
	add	eax,512
@@5:	add	ecx,eax
	sub	ecx,size NewHeaderStruc
	mov	dx,cx
	shr	ecx,16
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	mov	LibOffset,edx
	jmp	@@4
@@6:	mov	edi,edx
	;
	;Store this modules file offset.
	;
	xor	cx,cx
	xor	dx,dx
	mov	ax,4201h
	int	21h
	shl	edx,16
	mov	dx,ax
	sub	edx,size NewHeaderStruc
	mov	LibOffset,edx
	;
	;Skip segment definitions.
	;
	movzx	edx,NewSegments[edi]
	shl	edx,3
	sys	cwcInfo
	jc	@@03p0
	mov	edx,eax
@@03p0:	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	;
	;Skip relocations.
	;
	mov	edx,NewRelocs[edi]
	shl	edx,2
	or	edx,edx
	jz	@@03p1
	sys	cwcInfo
	jc	@@03p1
	mov	edx,eax
@@03p1:	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4201h
	int	21h
	;
	;Load export details.
	;
	mov	ecx,NewExports[edi]
	sys	GetMemLinear32
	mov	ErrorNumber,7
	jc	@@9
	mov	edx,ecx
	sys	cwcInfo
	jc	@@03p2
	push	edi
	mov	edi,esi
	sys	cwcLoad
	pop	edi
	mov	ErrorNumber,4
	jc	@@9
	jmp	@@03p4
@@03p2:	mov	ecx,edx
	mov	edx,esi
	mov	ah,3fh
	int	21h
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
@@03p4:	;Generate the THEADR.
	;
	push	esi
	add	esi,[esi+4]		;get offset of module name.
	movzx	ecx,b[esi]		;get name length.
	inc	ecx
	mov	ModOffset,esi
	mov	ModLength,ecx
	mov	edi,offset IOBuffer
	xor	ah,ah
	mov	b[edi],80h
	add	ah,[edi]
	inc	edi
	mov	[edi],cx
	add	w[edi],1
	add	ah,[edi]
	add	ah,[edi+1]
	add	edi,2
@@30:	mov	al,[esi]
	add	ah,al
	mov	[edi],al
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@30
	not	ah
	mov	[edi],ah
	inc	edi
	pop	esi
	mov	edx,offset IOBuffer
	mov	ecx,edi
	sub	ecx,edx
	mov	ebx,TMPHandle
	mov	ah,40h
	int	21h
	;
	;Now generate all the exported symbols.
	;
	mov	ModCount,1
	push	esi
	mov	edx,esi
	mov	ebp,[esi]		;get number of entries.
	add	esi,4+4
@@10:	push	esi
	push	edx
	mov	esi,[esi]
	add	esi,edx
	add	esi,4+2		;skip value.
	movzx	ecx,b[esi]
	inc	ecx
	mov	edi,offset IOBuffer
	;
	;Do the EXTDEF
	;
	push	ecx
	push	esi
	xor	ah,ah
	mov	b[edi],8ch
	add	ah,[edi]
	inc	edi
	mov	[edi],cx
	add	w[edi],1+1
	add	ah,[edi]
	add	ah,[edi+1]
	add	edi,2
@@31:	mov	al,[esi]
	add	ah,al
	mov	[edi],al
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@31
	mov	b[edi],0
	add	ah,[edi]
	inc	edi
	not	ah
	mov	[edi],ah
	inc	edi
	pop	esi
	pop	ecx
	;
	;Do the IMPDEF
	;
	xor	ah,ah
	mov	b[edi],88h
	add	ah,[edi]
	inc	edi
	mov	[edi],cx
	add	w[edi],1+1+1+1+2+1
	mov	edx,ModLength
	add	w[edi],dx
	add	ah,[edi]
	add	ah,[edi+1]
	add	edi,2
	mov	b[edi],0
	add	ah,[edi]
	inc	edi
	mov	b[edi],0a0h
	add	ah,[edi]
	inc	edi
	mov	b[edi],1
	add	ah,[edi]
	inc	edi
	mov	b[edi],1
	add	ah,[edi]
	inc	edi
@@32:	mov	al,[esi]
	add	ah,al
	mov	[edi],al
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@32
	mov	esi,ModOffset
	mov	ecx,ModLength
@@33:	mov	al,[esi]
	add	ah,al
	mov	[edi],al
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@33
	mov	edx,ModCount
	mov	[edi],dx
	add	ah,[edi]
	add	ah,[edi+1]
	add	edi,2
	not	ah
	mov	[edi],ah
	inc	edi
	;
	;Write the results.
	;
	mov	edx,offset IOBuffer
	mov	ecx,edi
	sub	ecx,edx
	mov	ah,40h
	mov	ebx,TMPHandle
	int	21h
	;
	pop	edx
	pop	esi
	inc	ModCount
	add	esi,4
	dec	ebp
	jnz	@@10
	pop	esi
	;
	;Do the MODEND.
	;
	mov	edi,offset IOBuffer
	xor	ah,ah
	mov	b[edi],8ah
	add	ah,[edi]
	inc	edi
	mov	w[edi],2
	add	ah,[edi]
	add	ah,[edi+1]
	add	edi,2
	mov	b[edi],0
	add	ah,[edi]
	inc	edi
	not	ah
	mov	[edi],ah
	inc	edi
	mov	edx,offset IOBuffer
	mov	ecx,edi
	sub	ecx,edx
	mov	ebx,TMPHandle
	mov	ah,40h
	int	21h
	;
	;Lose export memory.
	;
	sys	RelMemLinear32
	;
	;Move past this module.
	;
	mov	edx,d[NewHeader+NewSize]
	add	edx,LibOffset
	mov	LibOffset,edx
	mov	ebx,LibHandle
	mov	cx,dx
	shr	edx,16
	xchg	cx,dx
	mov	ax,4200h
	int	21h
	jmp	@@4
;
;Close all the files.
;
@@20:	mov	ebx,LibHandle
	mov	ah,3eh
	int	21h
	mov	ebx,TMPHandle
	mov	ah,3eh
	int	21h
;
;Return success to caller.
;
	mov	ErrorNumber,0
@@9:	ret
ImportObjModule endp


;*******************************************************************************
;Read the command line tail for parameters.
;
;supports / or - or + as switch/option settings. Options and names may be in
;any order, text can be tagged onto options, names must be in correct sequence
;for current program.
;
;Each character ( 33 to 127) has an entry in OptionTable & OptionTable+128.
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
;*******************************************************************************
ReadCommand	proc	near
	pushad
	;
	;Get PSP's linear address so we can point at the command tail.
	;
	mov	bx,PSPSegment
	sys	GetSelDet32
	mov	esi,edx
	add	esi,80h
	;
	;Parse command tail.
	;
	mov	edi,offset OptionText
	mov	OptionPointer,edi
	movzx	ecx,b[esi]
	inc	esi
	cmp	ecx,2
	jc	l9		;not long enough!
	mov	b[esi+ecx],0		;terminate the tail.
	;
l0:	mov	al,[esi]		;need to skip leading spaces.
	inc	esi		;/
	or	al,al		;/
	jz	l9		;/
	cmp	al,' '		;/
	jz	l0		;/
	dec	esi		;/
	;
l1:	cmp	b[esi],'/'		;option switch?
	jz	lOption		;/
	cmp	b[esi],'-'		;/
	jz	lOption		;/
	cmp	b[esi],'+'		;/
	jz	lOption		;/
	;
l2:	xor	ebx,ebx		;/
	mov	bl,OptionCounter	;Get file entry number.
	inc	OptionCounter	;/
	shl	ebx,2		;/
	add	ebx,offset OptionTable+128 ;/
	mov	edi,OptionPointer	;Current free space pointer.
	mov	[ebx],edi		;update table entry.
	;
	xor	cl,cl
l3:	cmp	b[esi],0		;end of name?
	jz	l4		;/
	cmp	b[esi],' '		;/
	jz	l4		;/
	mov	al,[esi]		;Copy this character.
	mov	[edi],al		;/
	inc	esi		;/
	inc	edi		;/
	mov	cl,1		;flag SOMETHING found.
	jmp	l3		;keep fetching them.
	;
l4:	mov	b[edi],0		;Terminate the name.
	inc	edi		;/
	mov	OptionPointer,edi	;Update table pointer.
	;
	or	cl,cl		;Make sure we found something.
	jnz	l0		;Go look for more info.
	dec	OptionPointer
	dec	OptionCounter	;move pointer/counter back.
	xor	ebx,ebx
	mov	bl,OptionCounter	;Get file entry number.
	shl	ebx,2		;/
	add	ebx,offset OptionTable+128 ;/
	mov	w[ebx],0		;reset table entry.
	jmp	l0
	;
lOption:	mov	ah,[esi]		;Get switch character.
	inc	esi
l5:	cmp	b[esi],0		;check for end of line.
	jz	l9		;/
	cmp	b[esi],' '		;skip spaces.
	jnz	l6		;/
	inc	esi		;/
	jmp	l5		;/
	;
l6:	mov	al,[esi]		;get the switched character.
	and	al,127
	inc	esi
	cmp	al,61h		; 'a'
	jb	l12
	cmp	al,7Ah		; 'z'
	ja	l12
	and	al,5Fh		;convert to upper case.
l12:	xor	ebx,ebx
	mov	bl,al
	add	ebx,offset OptionTable	;Index into the table.
	cmp	ah,'-'
	jnz	l7
	xor	ah,ah		;Convert '-' to zero.
l7:	mov	[ebx],ah		;Set flag accordingly.
	;
	cmp	b[esi],' '		;check for assosiated text.
	jz	l0
	cmp	b[esi],0
	jz	l9
	cmp	b[esi],'='
	jz	l900
	cmp	b[esi],':'		;allow colon as seperator.
	jnz	l8
l900:	inc	esi		;skip colon.
	;
l8:	xor	ebx,ebx
	mov	bl,al		;Get the option number again.
	shl	ebx,2		; &
	add	ebx,offset OptionTable+128 ;index into the table.
	mov	edi,OptionPointer	;current position in the table.
	mov	[ebx],edi		;store pointer in the table.
	;
	mov	ah," "
	cmp	b[esi],'"'
	jnz	l10
	mov	ah,'"'
	inc	esi
l10:	cmp	b[esi],0		;end of line?
	jz	l9
	cmp	b[esi],ah		;end of text?
	jz	l11
	mov	al,[esi]
	mov	[edi],al
	inc	esi
	inc	edi
	jmp	l10
	;
l11:	mov	b[edi],0		;terminate string.
	inc	edi
	inc	esi
	mov	OptionPointer,edi	;store new text pointer.
	jmp	l0		;scan some more text.
l9:	popad
	movzx	eax,OptionCounter
	ret
ReadCommand	endp


	.data

Copyright	db 13,10
	db 79 dup ("Ä"),13,10
	db "3PLib v2.00 Copyright 1995 Michael Devore; All rights reserved.",13,10
	db 79 dup ("Ä"),13,10
	db 13,10
	db "$"

PSPSegment	dw ?

ErrorList	dd ErrorM00,ErrorM01,ErrorM02,ErrorM03,ErrorM04,ErrorM05,ErrorM06,ErrorM07
	dd ErrorM08,ErrorM09,ErrorM10,ErrorM11,ErrorM12
ErrorNumber	db 0
ErrorM00	db "Operation completed successfully.",13,10,"$"
ErrorM01	db "Usage:",13,10
	db 13,10
	db "3plib commands lib_name mod_name [out_name]",13,10
	db 13,10
	db "commands:",13,10
	db 13,10
	db " A - Add/replace module in library.",13,10
	db " D - Delete module in library.",13,10
	db " E - Extract module from library.",13,10
	db " I - write Import object file.",13,10
	db " L - List modules in library.",13,10
	db 13,10
	db "lib_name  name of library to process.",13,10
	db "mod_name  name of module to process.",13,10
	db "out_name  name of extracted module file.",13,10
	db 13,10
	db "commands should be prefixed with /, - or +",13,10
	db 13,10
	db "$"
ErrorM02	db "Unknown command.",13,10,"$"
ErrorM03	db "Can't find module specified.",13,10,"$"
ErrorM04	db "DOS reported an error during file access.",13,10,"$"
ErrorM05	db "Unknown file format.",13,10,"$"
ErrorM06	db "Module has no name.",13,10,"$"
ErrorM07	db "Not enough memory.",13,10,"$"
ErrorM08	db "Unable to open/create library specified.",13,10,"$"
ErrorM09	db "Unable to create new library.",13,10,"$"
ErrorM10	db "Disk full.",13,10,"$"
ErrorM11	db "Unable to find module specified.",13,10,"$"
ErrorM12	db "Unable to create Import object specified.",13,10,"$"

OptionCounter	db 0
OptionTable	db 128 dup (0)
OptionPointers	dd 128 dup (0)
OptionPointer	dd ?
OptionText	db 256 dup (0)

ModHandle	dd 0
ModOffset	dd 0
ModLength	dd 0
ModCount	dd 0
ModName	db 256 dup (0)

LibHandle	dd 0
LibOffset	dd 0
LibName	db 256 dup (0)
LibIgnore	dd 0
LibTrail	dd 0

TMPHandle	dd 0
TMPName	db "qkxutzpa.drm",0

NewHeader	db size NewHeaderStruc dup (0)

IOBuffer	db 4096 dup (0)

CarriageReturn	db 13,10,"$"

	end	Start

