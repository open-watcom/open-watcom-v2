;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Load, relocate and execute the application code. LE format loader.
;
;On Entry:
;
;EBX	- Mode.
;	0 - Normal EXEC.
;	1 - Load for debug.
;	2 - Load for overlay.
;DS:EDX	- File name.
;ES:ESI	- Command line.
;CX	- Environment selector, 0 to use existing copy.
;
;On Exit:
;
;Carry set on error and AX = error code else,
;
;If Mode=0
;
;AL = ErrorLevel (returned by child's terminate)
;
;If Mode=1
;
;CX:EDX	- Entry CS:EIP
;BX:EAX	- Entry SS:ESP
;SI	- PSP.
;DI	- Auto DS.
;EBP	- Segment definition memory.
;
;If Mode=2
;
;CX:EDX	- Entry CS:EIP
;BX:EAX	- Entry SS:ESP
;SI	- PSP.
;
;Error codes:
;
;1	- DOS file access error.
;2	- Not an LE file.
;3	- Not enough memory.
;
LoadLE	proc	near
	mov	ax,ds
	pushm	ds,es,fs,gs
	assume ds:nothing
	mov	ds,cs:apiDDSeg
	assume ds:_apiCode
	;
	pushm	d[@@Name],w[@@Name+4],d[@@Flags],d[@@Command],w[@@command+4]
	pushm	w[@@Environment],w[@@Handle],w[@@PSP],d[@@ProgMem],d[@@ProgMem+4]
	pushm	d[@@Segs],d[@@ObjMem],d[@@FixupMem],d[@@ObjCount]
	pushm	d[@@ObjBase],d[@@PageCount],d[@@PageCount+4]
	pushm	d[@@EntryEIP],w[@@EntryCS],d[@@EntryESP],w[@@EntrySS]
	pushm	w[@@EntryDS],w[@@EntryES],d[@@ModLink],d[@@ModLink+4],d[@@LEOffset]
IFDEF LXWORK
	push	WORD PTR [@@LXFlag]	; MED
ENDIF
	;
	mov	d[@@Name],edx
	mov	w[@@Name+4],ax
	mov	d[@@Flags],ebx
	mov	d[@@Command],esi
	mov	w[@@Command+4],es
	mov	w[@@Environment],cx
	;
	xor	eax,eax
	mov	w[@@Handle],ax
	mov	w[@@PSP],ax
	mov	d[@@ProgMem],eax
	mov	d[@@Segs],eax
	mov	d[@@ObjMem],eax
	mov	d[@@FixupMem],eax
	mov	d[@@ModLink],eax
	mov	d[@@ModLink+4],eax
IFDEF LXWORK
	mov	WORD PTR [@@LXFlag],ax	; MED
ENDIF

;
;Try and open the file.
;
	push	ds
	lds	edx,f[@@Name]
	mov	ax,3d00h		;open, read only.
	int	21h
	pop	ds
	jc	@@no_file_error
	mov	w[@@Handle],ax	;store the handle.
;
;Check it's an MZ file.
;
	mov	bx,w[@@Handle]
	mov	edx,offset LEHeader	;somewhere to put the info.
	mov	ecx,2
	mov	ah,3fh
	int	21h
	jc	@@file_error
	cmp	ax,cx
	jnz	@@file_error
	cmp	w[LEHeader],'ZM'
	jnz	@@file_error
;
;Look for an LE offset.
;
	mov	bx,w[@@Handle]
	mov	dx,3ch
	xor	cx,cx
	mov	ax,4200h
	int	21h
	mov	edx,offset LEHeader
	mov	ecx,4
	mov	ah,3fh
	int	21h		;Fetch LE offset.
	jc	@@file_error
	cmp	ax,cx
	jnz	@@file_error
	cmp	d[LEHeader],0	;any offset?
	jz	@@file_error
	mov	eax,d[LEHeader]
	mov	d[@@LEOffset],eax
;
;Load the LE header.
;
	mov	dx,w[LEHeader]
	mov	cx,w[LEHeader+2]
	mov	ax,4200h
	int	21h		;Move to LE section.
	mov	edx,offset LEHeader
	mov	ecx,size LE_Header
	mov	ah,3fh
	int	21h
	jc	@@file_error
	cmp	ax,cx
	jnz	@@file_error
;
;Check it really is an LE file.
;
	cmp	w[LEHeader],"EL"
;	jnz	@@file_error
	jz	medclose

; add check for LX file, MED 12/18/95
IFDEF LXWORK
	mov	cs:[0],al
	cmp	w[LEHeader],"XL"
	jnz	@@file_error
	mov	cs:[0],al
ENDIF
	jmp	@@file_error

;
;Close the file again.
;
medclose:
	mov	bx,w[@@Handle]
	mov	ah,3eh
	int	21h
	mov	w[@@Handle],0
;
;Create a new PSP.
;
	push	ds
	mov	ebx,d[@@Flags]
	mov	cx,w[@@Environment]
	les	esi,f[@@command]
	lds	edx,f[@@name]
	call	CreatePSP
	pop	ds
	mov	w[@@PSP],bx
	jc	@@mem_error
;
;Open the input file again.
;
	push	ds
	lds	edx,f[@@Name]
	mov	ax,3d00h		;open, read only.
	int	21h
	pop	ds
	jc	@@file_error
	mov	w[@@Handle],ax	;store the handle.
;
;Process any EXPORT entries that need pulling in.
;
	cmp	d[LE_ResidentNames+LEHeader],0
	jz	@@NoExports

IFDEF DEBUG4
	jmp	@@NoExports
ENDIF

	mov	es,apiDSeg
	assume es:_cwMain
	mov	es,es:RealSegment
	assume es:nothing
	;
	;Set file pointer to resident names.
	;
	mov	ecx,d[LE_ResidentNames+LEHeader]
	add	ecx,d[@@LEOffset]
	mov	dx,cx
	shr	ecx,16
	mov	bx,w[@@Handle]
	mov	ax,4200h
	int	21h
	;
	;Sit in a loop reading names.
	;
	xor	ebp,ebp		;reset entry count.
	mov	edi,4		;reset bytes required.
@@ge0:	mov	edx,offset LETemp
	mov	ecx,1
	mov	ah,3fh
	int	21h
	jc	@@file_error
	cmp	ax,1
	jnz	@@file_error
	xor	eax,eax
	mov	al,[edx]
	and	al,127
	jz	@@ge1		;end of the list?
	add	eax,1		;include count byte
	add	eax,2		;include ordinal/segment
	inc	ebp		;update name count.
	add	edi,eax
	add	edi,4		;dword of value
	add	edi,4		;table entry memory required.
	mov	ecx,eax
	dec	ecx
	mov	ah,3fh
	int	21h		;read rest of the entry to skip it.
	jc	@@file_error
	cmp	ax,cx
	jnz	@@file_error
	jmp	@@ge0		;fetch all names.
	;
	;Allocate EXPORT table memory.
	;
@@ge1:	mov	ecx,edi
	sys	GetMemLinear32
	jc	@@mem_error
	mov	es:d[esi],0		;reset count.
	push	es
	mov	es,w[@@PSP]
	mov	es:d[EPSP_Exports],esi
	pop	es
	mov	edi,ebp		;get number of entries.
	shl	edi,2		;dword per entry.
	add	edi,4		;allow for count dword.
	add	edi,esi		;point to target memory.
	mov	edx,esi
	add	edx,4		;point to table memory.
	;
	;Move back to start of names again.
	;
	pushm	ecx,edx
	mov	ecx,d[LE_ResidentNames+LEHeader]
	add	ecx,d[@@LEOffset]
	mov	dx,cx
	shr	ecx,16
	mov	ax,4200h
	int	21h
	popm	ecx,edx
	;
	;Read all the names again.
	;
@@ge2:	or	ebp,ebp		;done all names?
	jz	@@ge3
	push	edx
	mov	ecx,1
	mov	edx,offset LETemp
	mov	ah,3fh
	int	21h		;get name string length.
	pop	edx
	jc	@@file_error
	cmp	ax,1
	jnz	@@file_error
	movzx	ecx,b[LETemp]
	and	cl,127		;name length.
	add	ecx,2		;include ordinal.
	mov	ah,3fh
	push	edx
	mov	edx,offset LETemp+1
	int	21h		;read rest of this entry.
	pop	edx
	jc	@@file_error
	cmp	ax,cx
	jnz	@@file_error
	inc	es:d[esi]		;update EXPORT count.
	mov	es:[edx],edi		;set this entries address.
	add	edx,4
	mov	es:d[edi],0		;clear offset.
	add	edi,4
	movzx	eax,b[LETemp]
	and	eax,127
	inc	eax
	mov	ecx,eax
	add	eax,offset LETemp
	movzx	eax,w[eax]
	mov	es:[edi],ax		;set entry table ordinal.
	add	edi,2
	push	esi
	mov	esi,offset LETemp
	rep	movsb		;copy EXPORT name.
	pop	esi
	dec	ebp
	jmp	@@ge2
@@ge3:	dec	es:d[esi]		;lose module name from the count.
;
;Get object definition memory.
;
@@NoExports:	mov	eax,size LE_OBJ	;length of an object entry.
	mul	d[LE_ObjNum+LEHeader]	;number of objects.
	mov	ecx,eax
	sys	GetMemLinear32	;Get memory.
	jc	@@mem_error		;Not enough memory.
	mov	d[@@ObjMem],esi
;
;Read object definitions.
;
	push	ecx
	mov	bx,w[@@Handle]
	mov	ecx,d[LE_ObjOffset+LEHeader] ;Get object table offset.
	add	ecx,d[@@LEOffset]
	mov	dx,cx
	shr	ecx,16
	mov	ax,4200h
	int	21h
	pop	ecx
	mov	edx,d[@@ObjMem]
	push	ds
	mov	ds,apiDSeg
	assume ds:_cwMain
	mov	ds,RealSegment
	assume ds:_apiCode
	mov	ah,3fh
	int	21h		;read definitions.
	pop	ds
	jc	@@file_error
	cmp	ax,cx
	jnz	@@file_error
;
;Work out how much memory we need for the program.
;
	mov	es,apiDSeg
	assume es:_cwMain
	mov	es,es:RealSegment
	assume es:nothing
	mov	ecx,d[LE_ObjNum+LEHeader]	;number of objects.
	mov	esi,d[@@ObjMem]
	xor	ebp,ebp		;clear memory requirement.
@@objup0:	mov	eax,es:LE_OBJ_Size[esi]
	add	eax,4095
	and	eax,not 4095		;page align objects
	mov	es:LE_OBJ_Size[esi],eax
	add	ebp,eax		;update program memory length.
	add	esi,size LE_OBJ
	dec	ecx
	jnz	@@objup0
;
;Get programs memory block.
;
	mov	ecx,ebp
	sys	GetMemLinear32	;Get memory.
	jc	@@mem_error		;Not enough memory.
	mov	d[@@ProgMem],esi
	mov	d[@@ProgMem+4],ecx
;
;Run through objects setting up load addresses.
;
	mov	edx,d[@@ProgMem]	;reset load offset.
	mov	ecx,d[LE_ObjNum+LEHeader]	;number of objects.
	mov	esi,d[@@ObjMem]
@@objup1:	mov	es:LE_OBJ_Base[esi],edx	;set load address.
	add	edx,es:LE_OBJ_Size[esi]	;update with object length.
	add	esi,size LE_OBJ
	dec	ecx
	jnz	@@objup1
;
;Get selectors.
;
	mov	ecx,d[LE_ObjNum+LEHeader]
	sys	GetSels
	jc	@@mem_error
	mov	w[@@Segs],bx		;store base selector.
	mov	w[@@Segs+2],cx	;store number of selectors.
;
;Update programs memory and selector details in PSP and variables.
;
	push	es
	mov	es,w[@@PSP]
	mov	ax,w[@@Segs]		;get base selector.
	mov	es:w[EPSP_SegBase],ax
	mov	ax,w[@@Segs+2]	;get number of selectors.
	shl	ax,3
	mov	es:w[EPSP_SegSize],ax
	mov	eax,d[@@ProgMem]	;get memory address.
	mov	es:d[EPSP_MemBase],eax
	mov	es:d[EPSP_NearBase],eax
	mov	eax,d[@@ProgMem+4]	;get memory size.
	mov	es:d[EPSP_MemSize],eax
	pop	es
;
;Fetch entry table and update EXPORT table values.
;
	push	es
	mov	es,w[@@PSP]
	cmp	es:d[EPSP_Exports],0	;any exports?
	pop	es
	jz	@@NoEntries
	push	es
	mov	es,w[@@PSP]
	mov	eax,es:d[EPSP_Exports]
	pop	es
	cmp	es:d[eax],0		;just a module name?
	jz	@@NoEntries
	;
	;Move file pointer to start of entry table.
	;
	mov	ecx,d[LE_EntryTable+LEHeader]
	add	ecx,d[@@LEOffset]
	mov	bx,w[@@Handle]
	mov	dx,cx
	shr	ecx,16
	mov	ax,4200h
	int	21h
	;
	;Work out how much work space we need.
	;
	mov	ecx,d[LE_EntryTable+LEHeader]
	cmp	d[LE_Directives+LEHeader],0
	jz	@@ge4
	sub	ecx,d[LE_Directives+LEHeader]
	jmp	@@ge5
@@ge4:	sub	ecx,d[LE_Fixups+LEHeader]
@@ge5:	neg	ecx
	sys	GetMemLinear32	;get entry table memory.
	jc	@@mem_error
	mov	edx,esi
	push	ds
	push	es
	pop	ds
	call	ReadFile		;read the entry table.
	pop	ds
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	;
	;Work through all EXPORT's setting values.
	;
	push	es
	mov	es,w[@@PSP]
	mov	esi,es:d[EPSP_Exports]
	pop	es
	mov	ecx,es:[esi]		;get number of entries.
	add	es:d[esi+4],4+2	;correct module name pointer.
	add	esi,4+4
@@exp0:	pushm	ecx,esi,edx
	mov	esi,es:[esi]		;point to this entry.
	xor	ebp,ebp
	xchg	bp,es:w[esi+4]	;get & clear ordinal.
	dec	ebp
@@exp1:	mov	bh,es:[edx]		;get bundle count.
	or	bh,bh
	jz	@@bad_entry
	mov	bl,es:[edx+1]	;get bundle type.
	add	edx,2
	mov	edi,edx		;point to object number incase we need it.
	xor	eax,eax
	mov	al,0
	cmp	bl,0
	jz	@@exp2
	add	edx,2		;skip object number.
	mov	al,3
	cmp	bl,1
	jz	@@exp2
	mov	al,5
	cmp	bl,2
	jz	@@exp2
	mov	al,5
	cmp	bl,3
	jz	@@exp2
	mov	al,7
	cmp	bl,4
	jz	@@exp2
	jmp	@@bad_entry
@@exp2:	;
@@exp3:	or	bh,bh
	jz	@@exp1		;end of this bundle.
	or	ebp,ebp		;our ordinal?
	jz	@@exp4
	add	edx,eax		;next entry.
	dec	ebp
	dec	bh
	jmp	@@exp3
	;
@@exp4:	or	bl,bl
	jz	@@bad_entry
	dec	bl
	jz	@@exp_16bit
	dec	bl

IFDEF DEBUG4
	jz	@@bad_fixup1
ENDIF

	jz	@@bad_fixup
	dec	bl
	jz	@@exp_32bit
	dec	bl
	jz	@@bad_entry
	jmp	@@bad_entry
	;
@@bad_entry:	popm	ecx,esi,edx

IFDEF DEBUG4
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	ds
	push	cs
	pop	ds
	cmp	bl,4
	jae	debugf2ae10
	mov	edx,OFFSET debugf2textl10
	jmp	debugf2loop2
debugf2ae10:
	mov	edx,OFFSET debugf2textae10
debugf2loop2:
	cmp	BYTE PTR ds:[edx],0
	je	debugf2b
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	inc	edx
	jmp	debugf2loop2
debugf2b:
	mov	edx,OFFSET debugf2textcrlf
	push	cs
	pop	ds
	mov	ecx,2
	mov	bx,1
	mov	ah,40h
	int	21h
	pop	ds
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	jmp	debugf2out

debugf2textl10	DB	'<10',0
debugf2textae10	DB	'>=10',0
debugf2textcrlf	DB	13,10

debugf2out:
	jmp	@@bad_fixup2
ENDIF

	jmp	@@bad_fixup
	;
@@exp_16bit:	movzx	eax,es:w[edi]	;get the object number.
	dec	eax
	shl	ax,3
	add	ax,w[@@Segs]
	mov	es:[esi+4],ax
	movzx	eax,es:w[edx+1]	;get the offset.
	mov	es:[esi],eax
	jmp	@@exp8
	;
@@exp_32bit:	movzx	eax,es:w[edi]
	dec	eax
	push	eax
	shl	eax,2
	mov	ebx,eax
	add	ebx,ebx
	shl	eax,2
	add	ebx,eax
	add	ebx,d[@@ObjMem]
	mov	ebx,es:LE_OBJ_Base[ebx]
	pop	eax
	shl	ax,3
	add	ax,w[@@Segs]
	mov	es:[esi+4],ax
	mov	eax,es:[edx+1]
	add	eax,ebx
	mov	es:[esi],eax
	jmp	@@exp8
	;
@@exp8:	popm	ecx,esi,edx
	add	esi,4
	dec	ecx
	jnz	@@exp0
	;
	mov	esi,edx
	sys	RelMemLinear32
;
;Read program objects.
;
@@NoEntries:	mov	ebp,d[LE_ObjNum+LEHeader]	;number of objects.
	mov	esi,d[@@ObjMem]
@@load0:	mov	eax,es:LE_OBJ_Flags[esi]	;get objects flags.
	and	eax,LE_OBJ_Flags_FillMsk	;isolate fill type.
	cmp	eax,LE_OBJ_Flags_Zero	;zero filled?
	jnz	@@load1
	;
	;Zero this objects memory.
	;
	mov	ecx,es:LE_OBJ_Size[esi]	;get objects virtual length.
	mov	edi,es:LE_OBJ_Base[esi]
	xor	eax,eax
	push	ecx
	and	ecx,3
	rep	stosb
	pop	ecx
	shr	ecx,2
	rep	stosd
	;
@@load1:	;Set file offset for data.
	;
	mov	eax,es:LE_OBJ_PageIndex[esi] ;get first page index.
	dec	eax
	mul	d[LE_PageSize+LEHeader]	;* page size.
	add	eax,d[LE_Data+LEHeader]	;data offset.
	mov	dx,ax
	shr	eax,16
	mov	cx,ax
	mov	ax,4200h
	mov	bx,w[@@Handle]
	int	21h		;set the file pointer.
	;
	;Work out how much data we're going to load.
	;
	mov	eax,es:LE_OBJ_PageNum[esi] ;get number of pages.
	mov	ebx,eax
	mul	d[LE_PageSize+LEHeader]	;* page size.
	mov	edx,es:LE_OBJ_Base[esi]	;get load address.
	xor	ecx,ecx
	or	eax,eax
	jz	@@loadz
	mov	ecx,es:LE_OBJ_Size[esi]
	add	ebx,es:LE_OBJ_PageIndex[esi] ;get base page again.
	dec	ebx
	cmp	ebx,d[LE_Pages+LEHeader]	;we getting the last page?
	jnz	@@load2
	mov	ebx,d[LE_PageSize+LEHeader]
	sub	ebx,d[LE_LastBytes+LEHeader]
	sub	eax,ebx
@@load2:	cmp	ecx,eax
	jc	@@load3
	mov	ecx,eax
	;
@@load3:	;Load the data.
	;
	mov	bx,w[@@Handle]
	push	ds
	push	es
	pop	ds
	call	ReadFile
	pop	ds
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
	;
;@@loadz:
	;Zero any memory we didn't just load to for Watcom's BSS benefit.
	;
	cmp	ecx,es:LE_OBJ_Size[esi]
	jnc	@@load4
	push	edi
	mov	edi,edx
	add	edi,ecx
	sub	ecx,es:LE_OBJ_Size[esi]
	neg	ecx
	xor	eax,eax
	push	ecx
	shr	ecx,2
	or	ecx,ecx
	jz	@@load6
@@load5:	mov	es:[edi],eax
	add	edi,4
	dec	ecx
	jnz	@@load5
@@load6:	pop	ecx
	and	ecx,3
	rep	stosb
	pop	edi
	;
@@load4:	;Next object.

@@loadz:

	;
	add	esi,size LE_OBJ
	dec	ebp
	jnz	@@load0
;
;Get fixup table memory & load fixups.
;
	mov	ecx,d[LE_FixupSize+LEHeader]
	sys	GetMemLinear32	;Get memory.
	jc	@@mem_error		;Not enough memory.
	mov	d[@@FixupMem],esi
	push	ecx
	mov	ecx,d[LE_Fixups+LEHeader]
	add	ecx,d[@@LEOffset]
	mov	dx,cx
	shr	ecx,16
	mov	bx,w[@@Handle]
	mov	ax,4200h
	int	21h		;move to fixup data.
	pop	ecx
	mov	edx,esi
	push	ds
	mov	ds,apiDSeg
	assume ds:_cwMain
	mov	ds,RealSegment
	assume ds:_apiCode
	call	ReadFile
	pop	ds
	jc	@@file_error
	cmp	eax,ecx
	jnz	@@file_error
;
;Get IMPORT module name links.
;
	mov	ecx,d[LE_ImportModNum+LEHeader]
	or	ecx,ecx
	jz	@@GotImpMods
	shl	ecx,2
	add	ecx,4
	sys	GetMemLinear32
	jc	@@mem_error
	mov	es:d[esi],0		;clear entry count for now.
	push	es
	mov	es,w[@@PSP]
	mov	es:d[EPSP_Imports],esi
	pop	es
	mov	d[@@ModLink],esi
	;
	;Work out offset in fixup data.
	;
	mov	esi,d[LE_ImportModNames+LEHeader]
	sub	esi,d[LE_Fixups+LEHeader]
	add	esi,d[@@FixupMem]
	;
	;Work through each name getting the module link address.
	;
@@NextModLnk:	cmp	d[LE_ImportModNum+LEHeader],0
	jz	@@GotImpMods
	;
	;Preserve current header state.
	;

	push	esi
	mov	ecx,size LE_Header
	sys	GetMemLinear32
	mov	ebp,esi
	pop	esi
	jc	@@mem_error
	mov	edi,ebp
	push	esi
	mov	esi,offset LEHeader
	rep	movsb
	pop	esi

	;
	;Search for this name.
	;
IFDEF DEBUG2
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	esi
	push	ds
	push	cs
	pop	ds
	mov	edx,OFFSET debugabtext1
debugabloop2:
	cmp	BYTE PTR ds:[edx],0
	je	debugabb
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	inc	edx
	jmp	debugabloop2
debugabb:
	push	es
	pop	ds
	movzx	ecx,BYTE PTR ds:[esi]
	inc	esi
;debugabloop1:
;	cmp	BYTE PTR ds:[esi],' '
;	jbe	debugaba
	mov	edx,esi
;	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
;	inc	esi
;	jmp	debugabloop1
debugaba:
	mov	edx,OFFSET debugabtext2
	push	cs
	pop	ds
	mov	ecx,2
	mov	bx,1
	mov	ah,40h
	int	21h
	pop	ds
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	jmp	debugabout

debugabtext1	DB	'Before FindModule call: ',0
debugabtext2	DB	13,10

debugabout:
ENDIF

	call	FindModule
	jc	@@error

IFDEF DEBUG2
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	esi
	push	ds
	push	cs
	pop	ds
	mov	edx,OFFSET debugactext1
debugacloop2:
	cmp	BYTE PTR ds:[edx],0
	je	debugacb
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	inc	edx
	jmp	debugacloop2
debugacb:
	push	es
	pop	ds
	movzx	ecx,BYTE PTR ds:[esi]
	inc	esi
;debugacloop1:
;	cmp	BYTE PTR ds:[esi],' '
;	jbe	debugaca
	mov	edx,esi
;	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
;	inc	esi
;	jmp	debugacloop1
debugaca:
	mov	edx,OFFSET debugactext2
	push	cs
	pop	ds
	mov	ecx,2
	mov	bx,1
	mov	ah,40h
	int	21h
	pop	ds
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	jmp	debugacout

debugactext1	DB	'After FindModule call: ',0
debugactext2	DB	13,10

debugacout:
ENDIF

	;
	push	esi
	push	edi
	mov	esi,ebp
	mov	edi,offset LEHeader
	mov	ecx,size LE_Header
	push	ds
	push	es
	pop	ds
	pop	es
	rep	movsb
	push	ds
	push	es
	pop	ds
	pop	es
	mov	esi,ebp
	sys	RelMemLinear32
	pop	edi
	pop	esi

	mov	edx,d[@@ModLink]
	mov	eax,es:[edx]		;get current count.
	shl	eax,2
	add	eax,4
	mov	es:[edx+eax],edi	;store link address.
	inc	es:d[edx]		;update link count.
	;
	movzx	ecx,es:b[esi]
	inc	ecx
	add	esi,ecx
	dec	d[LE_ImportModNum+LEHeader]
	jmp	@@NextModLnk

;
;Apply the fixups.
;
@@GotImpMods:
	mov	eax,d[LE_ObjNum+LEHeader]
	mov	d[@@ObjCount],eax
	mov	eax,d[@@ObjMem]
	mov	d[@@ObjBase],eax
	mov	d[@@EntryEIP],0
@@fix0:	;
	mov	esi,d[@@ObjBase]
	mov	ecx,es:LE_OBJ_PageNum[esi]
	or	ecx,ecx
	jz	@@fix400
	mov	d[@@PageCount],ecx
	mov	d[@@PageCount+4],0
	mov	edx,es:LE_OBJ_PageIndex[esi]
	dec	edx
	mov	ebp,edx		;Set base page map entry.
@@fix1:	;
	mov	edx,ebp
	mov	esi,d[@@FixupMem]
	mov	ecx,es:[esi+4+edx*4]	;Get next offset.
	mov	edx,es:[esi+edx*4]	;Get start offset.
	sub	ecx,edx		;Get number of bytes
	jz	@@fix4

IFDEF DEBUG4
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	ds
	push	cs
	pop	ds
	mov	edx,OFFSET debug10text1
debug10loop2:
	cmp	BYTE PTR ds:[edx],0
	je	debug10b
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	inc	edx
	jmp	debug10loop2
debug10b:
	mov	edx,OFFSET debug10text2
	push	cs
	pop	ds
	mov	ecx,2
	mov	bx,1
	mov	ah,40h
	int	21h
	pop	ds
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	jmp	debug10out

debug10text1	DB	'New fixup page...',0
debug10text2	DB	13,10

debug10out:
ENDIF

	mov	esi,d[@@FixupMem]
	add	esi,d[LE_FixupsRec+LEHeader] ;Point to fixup data.
	sub	esi,d[LE_Fixups+LEHeader]
	add	esi,edx		;Move to start of this pages fixups.
@@fix2:

	mov	al,es:[esi]		;Get type byte.
	mov	bl,al
	shr	bl,4		;Get single/multiple flag.
	mov	bh,al
	and	bh,15		;Get type.
	inc	esi
	dec	ecx
	mov	al,es:[esi]		;Get second type byte.
	mov	dl,al
	and	dl,3		;Get internal/external specifier.
	mov	dh,al
	shr	dh,2		;Get destination type.
	inc	esi
	dec	ecx
	;
	push	ebx
	and	bl,not 1
	or	bl,bl		;Check it's a single entry.
	pop	ebx

IFDEF DEBUG4
	jnz	@@bad_fixup3
ENDIF

	jnz	@@bad_fixup
	;

IFDEF DEBUG4
	test	dh,011010b		;Check for un-known bits.
	jnz	@@bad_fixup9
ENDIF


; added support for additive bit, MED 06/10/96
;	test	dh,011011b		;Check for un-known bits.
	test	dh,011010b		;Check for un-known bits.
	jnz	@@bad_fixup

	or	dl,dl		;Check it's an internal target.
	jnz	@@fixup_import

	cmp	bh,0010b		;Word segment?
	jz	@@Seg16
	cmp	bh,0111b		;32-bit offset?
	jz	@@32BitOff
	cmp	bh,0110b		;Seg:32-bit offset?
	jz	@@Seg1632BitOff
	cmp	bh,1000b		;32-bit self relative?
	jz	@@Self32Off
	cmp	bh,0101b		;16-bit offset?
	jz	@@16BitOff

; MED 12/09/96
	cmp	bh,1			; ignore fixup ???
	je	@@fix3

IFDEF DEBUG4
	jmp	@@bad_fixup4
	cmp	bh,1
	jne	around
;	add	esi,4
;	sub	ecx,4
	jmp	@@fix3
around:
ENDIF

	jmp	@@bad_fixup
;
;Fetch an external referance.
;
@@fixup_import:
	;
	;Grab the page offset.
	;
	movsx	edi,es:w[esi]
	add	esi,2
	sub	ecx,2
	;
	;Check import type.
	;
	cmp	dl,01b		;ordinal?
	jz	@@fiximp0
	cmp	dl,10b		;name?

IFDEF DEBUG4
	jnz	@@bad_fixup5
ENDIF

	jnz	@@bad_fixup
	;
	;Importing by name so find the name.
	;
	pushm	edi,ebp
	mov	ebp,d[LE_ImportNames+LEHeader]
	sub	ebp,d[LE_Fixups+LEHeader]
	movzx	eax,es:w[esi+1]
	add	ebp,eax		;point to function name.
	add	ebp,d[@@FixupMem]
	movzx	eax,es:b[esi]
	shl	eax,2
	add	eax,d[@@ModLink]
	mov	edi,es:[eax]		;point to module.
	mov	edi,es:EPSP_EXPORTS[edi]	;point to export table.

IFDEF DEBUG4
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	ebp
	push	ds
	push	cs
	pop	ds
	mov	edx,OFFSET debug2text1
debug2loop2:
	cmp	BYTE PTR ds:[edx],0
	je	debug2b
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	inc	edx
	jmp	debug2loop2
debug2b:
	push	es
	pop	ds
	movzx	ecx,BYTE PTR ds:[ebp]
	inc	ebp
debug2loop1:
;	cmp	BYTE PTR ds:[ebp],' '
;	jbe	debug2a
	mov	edx,ebp
;	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
;	inc	ebp
;	jmp	debug2loop1
debug2a:
	mov	edx,OFFSET debug2text2
	push	cs
	pop	ds
	mov	ecx,2
	mov	bx,1
	mov	ah,40h
	int	21h
	pop	ds
	pop	ebp
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	jmp	debug2out

debug2text1	DB	'FindFunction call: ',0
debug2text2	DB	13,10

debug2out:
ENDIF

	call	FindFunction
	mov	eax,edi
	popm	edi,ebp
	jc	@@file_error
	add	esi,1+2
	sub	ecx,1+2
	jmp	@@fiximp2
	;
	;Importing by ordinal so go strieght to the export.
	;
@@fiximp0:	push	edi
	movzx	edi,es:b[esi]
	shl	edi,2
	add	edi,d[@@ModLink]
	mov	edi,es:[edi]
	mov	edi,es:EPSP_EXPORTS[edi]	;point to export table.
	movzx	eax,es:w[esi+1]
	add	esi,2
	sub	ecx,2
	test	dh,100000b
	jz	@@fiximp1
	sub	esi,2
	add	ecx,2
	movzx	eax,es:b[esi+1]
	add	esi,1
	sub	ecx,1
@@fiximp1:	mov	eax,es:[edi+4+eax*4]	;point to export.
	pop	edi
	add	esi,1
	sub	ecx,1
@@fiximp2:

IFDEF DEBUG4
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	ds
	push	cs
	pop	ds
	mov	edx,OFFSET debug4text1
debug4loop2:
	cmp	BYTE PTR ds:[edx],0
	je	debug4b
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	inc	edx
	jmp	debug4loop2
debug4b:
	mov	edx,OFFSET debug4text2
	push	cs
	pop	ds
	mov	ecx,2
	mov	bx,1
	mov	ah,40h
	int	21h
	pop	ds
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	jmp	debug4out

debug4text1	DB	'Performing fixup...',0
debug4text2	DB	13,10

debug4out:
ENDIF

	;Now perform the fixup.
	;
	cmp	bh,0010b		;Word segment?
	jz	@@iSeg16
	cmp	bh,0111b		;32-bit offset?
	jz	@@i32BitOff
	cmp	bh,0110b		;Seg:32-bit offset?
	jz	@@iSeg1632BitOff
	cmp	bh,1000b		;32-bit self relative?
	jz	@@iSelf32Off


IFDEF DEBUG4
	jmp	@@bad_fixup6
ENDIF

	jmp	@@bad_fixup
	;
@@iSeg16:	;Deal with a 16-bit segment.
	;
	test	dh,4

IFDEF DEBUG4
	jnz	@@bad_fixup7
ENDIF

	jnz	@@bad_fixup
	;
	or	edi,edi
	js	@@iNeg0
	mov	ebx,d[@@ObjBase]
	mov	ebx,es:LE_OBJ_Base[ebx]
	add	edi,ebx
	mov	ebx,d[@@PageCount+4]	;Get page number.
	shl	ebx,12
	add	edi,ebx		;Point to the right page.
	mov	ax,es:[eax+4]	;Get the target segment.
	mov	es:[edi],ax		;Store target.

; MED 06/10/96
	test	dh,1	; see if additive value
	jne	@@bad_fixup	; yes, don't allow additives on segment fixups

@@iNeg0:	jmp	@@fix3
	;
@@i32BitOff:	;Deal with a 32-bit offset.
	;
	or	edi,edi
	js	@@iNeg1
	mov	ebx,d[@@ObjBase]
	mov	ebx,es:LE_OBJ_Base[ebx]
	add	edi,ebx
	mov	ebx,d[@@PageCount+4]	;Get page number.
	shl	ebx,12
	add	edi,ebx		;Point to the right page.
	mov	eax,es:[eax]
	mov	es:[edi],eax

; MED 06/10/96
	test	dh,1	; see if additive value
	je	@@fix3		; no
	movzx	eax,WORD PTR es:[esi]	; get additive value
	add	esi,2
	sub	ecx,2
	add	es:[edi],eax	;Store target.
	jmp	@@fix3

@@iNeg1:

	test	dh,1	; MED 06/12/96
	jz	@@iNeg1a
	add	esi,2
	sub	ecx,2
@@iNeg1a:

	jmp	@@fix3


@@iSelf32Off:	;Deal with a 32-bit self relative offset.

	or	edi,edi
	js	@@isfNeg1
	mov	ebx,d[@@ObjBase]
	mov	ebx,es:LE_OBJ_Base[ebx]
	add	edi,ebx
	mov	ebx,d[@@PageCount+4]	;Get page number.
	shl	ebx,12
	add	edi,ebx		;Point to the right page.
	mov	ebx,edi
	add	ebx,4
	mov	eax,es:[eax]
	sub	eax,ebx
	mov	es:[edi],eax

; MED 06/10/96
	test	dh,1	; see if additive value
	je	@@fix3		; no
	movzx	eax,WORD PTR es:[esi]	; get additive value
	add	esi,2
	sub	ecx,2
	add	es:[edi],eax	;Store target.

@@isfNeg1:	jmp	@@fix3

@@iSeg1632BitOff: ;Deal with an FWORD fixup by splitting into a seg16 and 32-bit
	;offset relocation entry.
	;
	or	edi,edi
	js	@@iNeg2
	mov	ebx,d[@@ObjBase]
	mov	ebx,es:LE_OBJ_Base[ebx]
	add	edi,ebx
	mov	ebx,d[@@PageCount+4]	;Get page number.
	shl	ebx,12
	add	edi,ebx		;Point to the right page.
	push	eax
	movzx	eax,es:w[eax+4]
	mov	es:[edi+4],ax	;Store target.
	pop	eax
	mov	eax,es:[eax]
	mov	es:[edi],eax

; MED 06/10/96
	test	dh,1	; see if additive value
	je	@@fix3		; no
	movzx	eax,WORD PTR es:[esi]	; get additive value
	add	esi,2
	sub	ecx,2
	add	es:[edi],eax	;Store target.

@@iNeg2:	jmp	@@fix3

;Deal with a 16-bit segment.
@@Seg16:

	;EBP	- Page offset within segment.
	;w[esi] - offset within page.
	;b[esi+2] - target object+1.
	;
	test	dh,4

IFDEF DEBUG4
	jnz	@@bad_fixup8
ENDIF

	jnz	@@bad_fixup
	;
	mov	edi,d[@@ObjBase]
	mov	edi,es:LE_OBJ_Base[edi]
	mov	eax,d[@@PageCount+4]	;Get page number.
	shl	eax,12
	add	edi,eax		;Point to the right page.
	movsx	eax,es:w[esi]
	or	eax,eax
	js	@@Neg0
	add	edi,eax		;Point to the right offset.
	movzx	eax,es:b[esi+2]	;Get the target segment.
	dec	eax
	shl	eax,3
	add	ax,w[@@Segs]
	mov	es:[edi],ax		;Store target.
	;
@@Neg0:	add	esi,2+1
	sub	ecx,2+1
	jmp	@@fix3
	;
@@16BitOff:	;Deal with a 16-bit offset.
	;
	;EBP	- Page offset within segment.
	;w[esi] - offset within page.
	;b[esi+2] - target object+1
	;w[esi+3] - target offset.
	;
	mov	edi,d[@@ObjBase]
	mov	edi,es:LE_OBJ_Base[edi]
	mov	eax,d[@@PageCount+4]	;Get page number.
	shl	eax,12
	add	edi,eax		;Point to the right page.
	movsx	eax,es:w[esi]
	or	eax,eax
	js	@@Neg3
	add	edi,eax		;Point to the right offset.
	mov	ax,es:w[esi+3]	;Get target offset.
	mov	es:[edi],ax
@@Neg3:	add	esi,2+1+2
	sub	ecx,2+1+2
	jmp	@@fix3
	;
@@32BitOff:	;Deal with a 32-bit offset.
	;
	;EBP	- Page offset within segment.
	;w[esi] - offset within page.
	;b[esi+2] - target object+1
	;w[esi+3] - target offset.
	;
	mov	edi,d[@@ObjBase]
	mov	edi,es:LE_OBJ_Base[edi]
	mov	eax,d[@@PageCount+4]	;Get page number.
	shl	eax,12
	add	edi,eax		;Point to the right page.
	movsx	eax,es:w[esi]
	or	eax,eax
	js	@@Neg1
	add	edi,eax		;Point to the right offset.
	movzx	eax,es:b[esi+2]	;Get the target segment.
	dec	eax
	push	edx
	mov	edx,size LE_OBJ
	mul	edx
	pop	edx
	add	eax,d[@@ObjMem]	;point to target segment details.
	mov	eax,es:LE_OBJ_Base[eax]	;Get target segments offset from start of image.

COMMENT !
	movzx	ebx,es:w[esi+3]	;Get target offset.
	test	dh,4
	jz	@@Big0
	mov	ebx,es:[esi+3]	;Get target offset.
@@Big0:	add	eax,ebx
	mov	es:[edi],eax
END COMMENT !
; MED 06/12/96, allow for additive bit
	test	dh,4
	jnz	@@Big0
	movzx	ebx,es:w[esi+3]	;Get 16-bit target offset.
	add	esi,2+1+2	; adjust offset, byte count
	sub	ecx,2+1+2

stuff1:
	add	eax,ebx
	mov	es:[edi],eax
	test	dh,1	; check for additive value
	je	@@fix3		; none
	movzx	eax,WORD PTR es:[esi]	; get additive value
	add	esi,2
	sub	ecx,2
	add	es:[edi],eax	;Store target.
	jmp	@@fix3

@@Big0:
	mov	ebx,es:[esi+3]	;Get 32-bit target offset.
	add	esi,2+1+4	; adjust offset, byte count
	sub	ecx,2+1+4
	jmp	stuff1

@@Neg1:
	add	esi,2+1+2
	sub	ecx,2+1+2

	test	dh,1	; MED 06/12/96
	jz	@@Neg1a
	add	esi,2
	sub	ecx,2
@@Neg1a:

	test	dh,4
	jz	@@fix3
	add	esi,2
	sub	ecx,2
	jmp	@@fix3

@@Self32Off:	;Deal with a 32-bit self relative offset.
	;
	;EBP	- Page offset within segment.
	;w[esi] - offset within page.
	;b[esi+2] - target object+1
	;w[esi+3] - target offset.
	;
	mov	edi,d[@@ObjBase]
	mov	ebx,es:LE_OBJ_Flags[edi]
	mov	edi,es:LE_OBJ_Base[edi]
	mov	eax,d[@@PageCount+4]	;Get page number.
	shl	eax,12
	add	edi,eax		;Point to the right page.
	movsx	eax,es:w[esi]
	or	eax,eax
	js	@@sfNeg1
	add	edi,eax		;Point to the right offset.
	mov	ebx,edi
	movzx	eax,es:b[esi+2]	;Get the target segment.
	dec	eax
	push	edx
	mov	edx,size LE_OBJ
	mul	edx
	pop	edx
	add	eax,d[@@ObjMem]	;point to target segment details.
	mov	eax,es:LE_OBJ_Base[eax]
	push	ebx
	movzx	ebx,es:w[esi+3]	;Get target offset.
	test	dh,4
	jz	@@sfBig0
	mov	ebx,es:[esi+3]	;Get target offset.
@@sfBig0:	add	eax,ebx
	pop	ebx
	add	ebx,4
	sub	eax,ebx
	mov	es:[edi],eax
	;
@@sfNeg1:	add	esi,2+1+2
	sub	ecx,2+1+2
	test	dh,4
	jz	@@fix3
	add	esi,2
	sub	ecx,2
	jmp	@@fix3
	;
@@Seg1632BitOff: ;Deal with an FWORD fixup by splitting into a seg16 and 32-bit
	;offset relocation entry.
	;
	;EBP	- Page offset within segment.
	;w[esi] - offset within page.
	;b[esi+2] - target object+1
	;w[esi+3] - target offset.
	;
	mov	edi,d[@@ObjBase]
	mov	edi,es:LE_OBJ_Base[edi]
	mov	eax,d[@@PageCount+4]	;Get page number.
	shl	eax,12
	add	edi,eax		;Point to the right page.
	movsx	eax,es:w[esi]
	or	eax,eax
	js	@@Neg2
	add	edi,eax		;Point to the right offset.
	add	edi,4		;Point to the seg bit.
	movzx	eax,es:b[esi+2]	;Get the target segment.
	dec	eax
	shl	eax,3
	add	ax,w[@@Segs]
	mov	es:[edi],ax		;Store target.
	;
	mov	edi,d[@@ObjBase]
	mov	edi,es:LE_OBJ_Base[edi]
	mov	eax,d[@@PageCount+4]	;Get page number.
	shl	eax,12
	add	edi,eax		;Point to the right page.
	movzx	eax,es:w[esi]
	add	edi,eax		;Point to the right offset.
	movzx	eax,es:b[esi+2]	;Get the target segment.
	dec	eax
	push	edx
	mov	edx,size LE_OBJ
	mul	edx
	pop	edx
	add	eax,d[@@ObjMem]	;point to target segment details.
	test	es:LE_OBJ_Flags[eax],LE_OBJ_Flags_Big
	pushf
	mov	eax,es:LE_OBJ_Base[eax]	;Get target segments offset from start of image.
	movzx	ebx,es:w[esi+3]	;Get target offset.
	test	dh,4
	jz	@@Big1
	mov	ebx,es:[esi+3]	;Get target offset.
@@Big1:	popf
	jz	@@NotFlat1
	add	ebx,eax
@@NotFlat1:	mov	es:[edi],ebx
	;
@@Neg2:	add	esi,2+1+2
	sub	ecx,2+1+2
	test	dh,4
	jz	@@fix3
	add	esi,2
	sub	ecx,2
	jmp	@@fix3
	;
@@fix3:	inc	d[@@EntryEIP]
	or	ecx,ecx
	jnz	@@fix2
	;
@@fix4:	inc	ebp
	inc	d[@@PageCount+4]
	dec	d[@@PageCount]
	jnz	@@fix1
	;
@@fix400:	add	d[@@ObjBase],size LE_OBJ
	dec	d[@@ObjCount]
	jnz	@@fix0

	mov	esi,d[@@FixupMem]
	sys	RelMemLinear32
	mov	d[@@FixupMem],0

IFDEF DEBUG2
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	ds
	push	cs
	pop	ds
	mov	edx,OFFSET debugadtext1
debugadloop2:
	cmp	BYTE PTR ds:[edx],0
	je	debugadb
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	inc	edx
	jmp	debugadloop2
debugadb:
	mov	edx,OFFSET debugadtext2
	push	cs
	pop	ds
	mov	ecx,2
	mov	bx,1
	mov	ah,40h
	int	21h
	pop	ds
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	jmp	debugadout

debugadtext1	DB	'Setup entry CS:EIP...',0
debugadtext2	DB	13,10

debugadout:
ENDIF

;
;Setup entry CS:EIP.
;
	mov	ebx,d[LE_EntryCS+LEHeader]
	or	ebx,ebx
	jz	@@NoEntryCS
	dec	ebx
	mov	eax,size LE_OBJ
	mul	ebx
	shl	ebx,3
	mov	esi,d[LE_EntryEIP+LEHeader]
	mov	edi,d[@@ObjMem]
	add	edi,eax
	add	esi,es:LE_OBJ_Base[edi]
	test	es:LE_OBJ_Flags[edi],LE_OBJ_Flags_Big	;FLAT segment?
	jnz	@@FlatEIP
	sub	esi,d[@@ProgMem]
@@FlatEIP:	add	bx,w[@@Segs]
	mov	d[@@EntryEIP],esi
@@NoEntryCS:	mov	w[@@EntryCS],bx
;
;Setup entry SS:ESP
;
	mov	ebx,d[LE_EntrySS+LEHeader]
	or	ebx,ebx
	jz	@@NoEntrySS
	dec	ebx
	mov	eax,size LE_OBJ
	mul	ebx
	shl	ebx,3
	mov	esi,d[LE_EntryESP+LEHeader]
	mov	edi,d[@@ObjMem]
	add	edi,eax
	add	esi,es:LE_OBJ_Base[edi]
	test	es:LE_OBJ_Flags[edi],LE_OBJ_Flags_Big	;FLAT segment?
	jnz	@@FlatESP
	sub	esi,d[@@ProgMem]
@@FlatESP:	add	bx,w[@@Segs]
	mov	d[@@EntryESP],esi
@@NoEntrySS:	mov	w[@@EntrySS],bx
;
;Setup entry ES & DS.
;
	mov	ax,w[@@PSP]
	mov	w[@@EntryES],ax
	mov	w[@@EntryDS],ax
	mov	eax,d[LE_AutoDS+LEHeader]
	or	eax,eax
	jz	@@NoAutoDS
	dec	eax
	shl	eax,3
	add	ax,w[@@Segs]
	mov	w[@@EntryDS],ax
;
;Convert object definitions into 3P segment definitions for CWD.
;
@@NoAutoDS:	mov	ebp,d[LE_ObjNum+LEHeader]	;number of objects.
	mov	esi,d[@@ObjMem]
	mov	edi,esi
@@makesegs0:	mov	eax,es:LE_OBJ_Flags[esi]	;Get objects flags.
	xor	ebx,ebx
	test	eax,LE_OBJ_Flags_Exec	;Executable?
	jnz	@@makesegs1
	inc	ebx		;Make it Data.
	test	eax,LE_OBJ_Flags_Write	;Writeable?
	jz	@@makesegs1
;	add	ebx,2		;Read only data.
@@makesegs1:	shl	ebx,24
	test	eax,LE_OBJ_Flags_Big	;Big bit set?
	jz	@@makesegs2
	or	ebx,1 shl 26		;Force 32-bit.
	or	ebx,1 shl 27		;assume 32-bit is FLAT.
	jmp	@@makesegs3
@@makesegs2:	or	ebx,1 shl 25		;Force 16-bit.
@@makesegs3:	mov	eax,es:LE_OBJ_Size[esi]
	cmp	eax,100000h		;>1M?
	jc	@@makesegs4
	shr	eax,12
	or	eax,1 shl 20
@@makesegs4:	or	ebx,eax		;Include length.
	mov	eax,es:LE_OBJ_Base[esi]
	sub	eax,d[@@ProgMem]	;lose load address.
	mov	es:d[edi+0],eax
	mov	es:d[edi+4],ebx
	add	esi,size LE_OBJ
	add	edi,4+4
	dec	ebp
	jnz	@@makesegs0
	;
	;Shrink OBJ memory to fit segment definitions.
	;
	mov	eax,4+4
	mul	d[LE_ObjNum+LEHeader]	;number of objects.
	mov	ecx,eax
	mov	esi,d[@@ObjMem]
	sys	ResMemLinear32
	jc	@@mem_error		;shouldn't be able to happen.
	mov	d[@@ObjMem],esi	;set new Obj mem address.
;
;Setup selectors.
;
	mov	ecx,d[LE_ObjNum+LEHeader]
	mov	esi,d[@@ObjMem]
	mov	bx,w[@@Segs]		;base selector.
@@SegLoop:	pushm	ebx,ecx,esi
	;
	mov	eax,es:[esi+4]	;Get limit.
	mov	ecx,eax
	and	ecx,0fffffh		;mask to 20 bits.
	test	eax,1 shl 20		;G bit set?
	jz	@@NoGBit
	shl	ecx,12
	or	ecx,4095
@@NoGBit:	or	ecx,ecx
	jz	@@NoDecLim
	cmp	ecx,-1
	jz	@@NoDecLim
	dec	ecx
@@NoDecLim:	mov	edx,es:[esi]		;get base.
	;
	test	eax,1 shl 27		;FLAT segment?
	jz	@@NotFLATSeg
	;
	push	fs
	mov	fs,w[@@PSP]
	mov	fs:d[EPSP_NearBase],0	;Make sure NEAR functions work.
	pop	fs
	;
	add	edx,d[@@ProgMem]
	or	ecx,-1		;Update the limit.
	xor	edx,edx
	jmp	@@DoSegSet
	;
@@NotFLATSeg:	add	edx,d[@@ProgMem]	;offset within real memory.
	;
@@DoSegSet:	sys	SetSelDet32
	;
	mov	eax,es:[esi+4]	;Get class.
	shr	eax,21		;move type into useful place.
	and	eax,0fh		;isolate type.
	or	eax,eax
	jz	@@CodeSeg
	mov	eax,es:[esi+4]	;Get type bits.
	mov	cx,0		;Set 16 bit seg.
	test	eax,1 shl 25
	jnz	@@gotBBit
	mov	cx,1
	test	eax,1 shl 26		;32 bit seg?
	jnz	@@gotBBit
	mov	cx,0		;Set 16 bit seg.
@@GotBBit:	call	_DSizeSelector
	jmp	@@SegDone
	;
@@CodeSeg:	mov	eax,es:[esi+4]	;Get type bits.
	mov	cx,0		;Set 16 bit seg.
	test	eax,1 shl 25
	jnz	@@Default
	mov	cx,1
	test	eax,1 shl 26		;32 bit seg?
	jnz	@@Default
	mov	cx,0		;Set 16 bit seg.
@@Default:	sys	CodeSel
	;
@@SegDone:	popm	ebx,ecx,esi
	add	esi,8		;next definition.
	add	ebx,8		;next selector.
	dec	ecx
	jnz	@@SegLoop
;
;Close the input file.
;

IFDEF DEBUG2
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	ds
	push	cs
	pop	ds
	mov	edx,OFFSET debugaetext1
debugaeloop2:
	cmp	BYTE PTR ds:[edx],0
	je	debugaeb
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	inc	edx
	jmp	debugaeloop2
debugaeb:
	mov	edx,OFFSET debugaetext2
	push	cs
	pop	ds
	mov	ecx,2
	mov	bx,1
	mov	ah,40h
	int	21h
	pop	ds
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	jmp	debugaeout

debugaetext1	DB	'Close Input File...',0
debugaetext2	DB	13,10

debugaeout:
ENDIF

	xor	bx,bx
	xchg	bx,w[@@Handle]
	mov	ah,3eh
	int	21h
;
;Check if this is an exec or just a load.
;
	cmp	d[@@Flags],0
	jz	@@Exec

;
;Switch back to parents PSP if this is a debug load.
;
	cmp	d[@@Flags],2
	jz	@@NoPSwitch2
	push	fs
	mov	fs,w[@@PSP]
	mov	bx,fs:w[EPSP_Parent]
	pop	fs
	mov	ah,50h
	int	21h
	mov	ebp,d[@@ObjMem]
;
;Return program details to caller.
;
@@NoPSwitch2:	mov	edx,d[@@EntryEIP]
	mov	cx,w[@@EntryCS]
	mov	eax,d[@@EntryESP]
	mov	bx,w[@@EntrySS]
	mov	si,w[@@EntryES]
	mov	di,w[@@EntryDS]
	clc
	jmp	@@exit
;
;Run it.
;
@@Exec:
	mov	eax,d[@@Flags]
	mov	ebx,d[@@EntryEIP]
	mov	cx,w[@@EntryCS]
	mov	edx,d[@@EntryESP]
	mov	si,w[@@EntrySS]
	mov	di,w[@@PSP]
	mov	bp,w[@@EntryDS]
	call	ExecModule

	clc
;
;Shut down anything still hanging around.
;
@@error:
	pushf
	push	ax
;
;Make sure file is closed.
;
	pushf
	xor	bx,bx
	xchg	bx,w[@@Handle]
	or	bx,bx
	jz	@@NoClose
	mov	ah,3eh
	int	21h
;
;Make sure all work spaces are released.
;
@@NoClose:	xor	esi,esi
	xchg	esi,d[@@ObjMem]
	or	esi,esi
	jz	@@NoObjRel
	sys	RelMemLinear32
;
;Restore previous state.
;
@@NoObjRel:	popf
	jnc	@@RelPSP
	cmp	w[@@PSP],0
	jz	@@NoRelRes
;
;Restore vectors & DPMI state.
;
@@RelPSP:	mov	eax,d[@@Flags]
	mov	bx,w[@@PSP]
	pushm	ds,ds,ds
	popm	es,fs,gs
	call	DeletePSP
;
;Return to caller.
;
@@NoRelRes:	pop	ax
	popf
	;
@@exit:
IFDEF LXWORK
	pop	WORD PTR [@@LXFlag]	; MED
ENDIF
	popm	w[@@EntryDS],w[@@EntryES],d[@@ModLink],d[@@ModLink+4],d[@@LEOffset]
	popm	d[@@EntryEIP],w[@@EntryCS],d[@@EntryESP],w[@@EntrySS]
	popm	d[@@ObjBase],d[@@PageCount],d[@@PageCount+4]
	popm	d[@@Segs],d[@@ObjMem],d[@@FixupMem],d[@@ObjCount]
	popm	w[@@Environment],w[@@Handle],w[@@PSP],d[@@ProgMem],d[@@ProgMem+4]
	popm	d[@@Name],w[@@Name+4],d[@@Flags],d[@@Command],w[@@command+4]
	;
	popm	ds,es,fs,gs

IFDEF DEBUG2
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	ds
	push	cs
	pop	ds
	mov	edx,OFFSET debugaftext1
debugafloop2:
	cmp	BYTE PTR ds:[edx],0
	je	debugafb
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	inc	edx
	jmp	debugafloop2
debugafb:
	mov	edx,OFFSET debugaftext2
	push	cs
	pop	ds
	mov	ecx,2
	mov	bx,1
	mov	ah,40h
	int	21h
	pop	ds
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	jmp	debugafout

debugaftext1	DB	'Returning from LoadLE...',0
debugaftext2	DB	13,10

debugafout:
ENDIF

	ret
;
;Not enough memory error.
;
@@mem_error:

	mov	ax,3
	stc
	jmp	@@error
;
;Couldn't find the file.
;
@@no_file_error:
	mov	ax,1
	stc
	jmp	@@error
;
;Fixup type we don't understand.
;
@@bad_fixup:
IFDEF DEBUG4
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	ds
	push	cs
	pop	ds
	mov	edx,OFFSET debug5text1
debug5loop2:
	cmp	BYTE PTR ds:[edx],0
	je	debug5b
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	inc	edx
	jmp	debug5loop2
debug5b:
	dec	esi
	mov	edx,OFFSET debug5t1
	add	edx,esi
	push	cs
	pop	ds
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	mov	edx,OFFSET debug5text2
	push	cs
	pop	ds
	mov	ecx,2
	mov	bx,1
	mov	ah,40h
	int	21h
	pop	ds
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	pop	esi
	jmp	debug5out

debug5text1	DB	' Bad fixup: ',0
debug5text2	DB	13,10
debug5t1	DB	'1','2','3','4','5','6','7','8','9','A'

@@bad_fixup1:
	push	esi
	mov	esi,1
	jmp	@@bad_fixup
@@bad_fixup2:
	push	esi
	mov	esi,2
	jmp	@@bad_fixup
@@bad_fixup3:
	push	esi
	mov	esi,3
	jmp	@@bad_fixup
@@bad_fixup4:
	push	esi
	mov	esi,4
	jmp	@@bad_fixup
@@bad_fixup5:
	push	esi
	mov	esi,5
	jmp	@@bad_fixup
@@bad_fixup6:
	push	esi
	mov	esi,6
	jmp	@@bad_fixup
@@bad_fixup7:
	push	esi
	mov	esi,7
	jmp	@@bad_fixup
@@bad_fixup8:
	push	esi
	mov	esi,8
	jmp	@@bad_fixup
@@bad_fixup9:
	push	eax
	push	ebx
	push	ecx
	push	edx
	mov	cx,8
bfloop:
	rol	dh,1
	mov	bl,dh
	and	dh,1
	mov	dl,dh
	add	dl,30h
	mov	ah,2
	int	21h
	mov	dh,bl
	dec	cx
	jne	bfloop
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	push	esi
	mov	esi,9
	jmp	@@bad_fixup

debug5out:
ENDIF

	mov	eax,d[@@EntryEIP]	;Get the relocation number.
	push	ds
	assume ds:nothing
	mov	ds,cs:apiDSeg
	assume ds:_cwMain
	if	0
	mov	b[ErrorM11_0+0]," "
	mov	b[ErrorM11_0+1]," "
	mov	b[ErrorM11_0+2]," "
	mov	ecx,8
	mov	edi,offset ErrorM11_1
	call	Bin2HexA
	endif
	assume ds:_apiCode
	pop	ds
	mov	ax,2
	stc
	jmp	@@error
;
;Not an LE file.
;
@@file_error:
IFDEF DEBUG4
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	ds
	push	cs
	pop	ds
	mov	edx,OFFSET debug7text1
debug7loop2:
	cmp	BYTE PTR ds:[edx],0
	je	debug7b
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	inc	edx
	jmp	debug7loop2
debug7b:
	mov	edx,OFFSET debug7text2
	push	cs
	pop	ds
	mov	ecx,2
	mov	bx,1
	mov	ah,40h
	int	21h
	pop	ds
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	jmp	debug7out

debug7text1	DB	'File error type 1',0
debug7text2	DB	13,10

debug7out:
ENDIF

	mov	ax,2
	stc
	jmp	@@error
;
;Corrupt file or file we don't understand.
;
@@file_error2:

IFDEF DEBUG4
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	ds
	push	cs
	pop	ds
	mov	edx,OFFSET debug8text1
debug8loop2:
	cmp	BYTE PTR ds:[edx],0
	je	debug8b
	mov	ecx,1
	mov	bx,1
	mov	ah,40h
	int	21h
	inc	edx
	jmp	debug8loop2
debug8b:
	mov	edx,OFFSET debug8text2
	push	cs
	pop	ds
	mov	ecx,2
	mov	bx,1
	mov	ah,40h
	int	21h
	pop	ds
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	jmp	debug8out

debug8text1	DB	'File error type 2',0
debug8text2	DB	13,10

debug8out:
ENDIF

	mov	ax,2
	stc
	jmp	@@error
;
@@Name:	;
	df 0
@@Flags:	;
	dd 0
@@Command:	;
	df 0
@@Environment:	;
	dw 0
@@Handle:	;
	dw 0
@@PSP:	;
	dw 0
@@LEOffset:	;
	dd 0
@@ProgMem:	;
	dd 0,0
@@Segs:	;
	dw 0,0
@@ObjMem:	;
	dd 0
@@FixupMem:	;
	dd 0
@@ObjCount:	;
	dd 0
@@ObjBase:	;
	dd 0
@@PageCount:	;
	dd 0,0
@@EntryEIP:	;
	dd 0
@@EntryCS:	;
	dw 0
@@EntryESP:	;
	dd 0
@@EntrySS:	;
	dw 0
@@EntryES:	;
	dw 0
@@EntryDS:	;
	dw 0
@@ModLink:	;
	dd 0,0
IFDEF LXWORK
@@LXFlag	DW	0	; nonzero if LX instead of LE, MED
ENDIF
LoadLE	endp


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;LE header format.
;
LE_Header		struc
LE_ID		dw ?	;"LE" text identifier.
LE_ByteOrder		db ?	;byte order, 0=little-endian, none-zero=big.
LE_WordOrder		db ?	;word order.
LE_Format		dd ?	;format level.
;
LE_CPU		dw ?	;CPU type.
LE_CPU_286		equ 1
LE_CPU_386		equ 2
LE_CPU_486		equ 3
LE_CPU_586		equ 4
LE_CPU_i860		equ 20h
LE_CPU_N11		equ 21h
LE_CPU_R2000		equ 40h
LE_CPU_R6000		equ 41h
LE_CPU_R4000		equ 42h
;
LE_OS		dw ?	;Target operating system.
LE_OS_OS2		equ 1
LE_OS_Windows		equ 2
LE_OS_DOS4		equ 3
LE_OS_Win386		equ 4
;
LE_Version		dd ?	;Module version.
;
LE_Type		dd ?	;Module type.
LE_Type_InitPer	equ 1 shl 2	;initialise per process.
LE_Type_IntFixup	equ 1 shl 4	;no internal fixups.
LE_Type_ExtFixup	equ 1 shl 5	;no external fixups.
LE_Type_NoLoad		equ 1 shl 13	;module not loadable.
LE_Type_DLL		equ 1 shl 15	;DLL
;
LE_Pages		dd ?	;number of memory pages.
LE_EntryCS		dd ?	;Entry CS object.
LE_EntryEIP		dd ?	;Entry EIP.
LE_EntrySS		dd ?	;Entry SS object.
LE_EntryESP		dd ?	;Entry ESP.
LE_PageSize		dd ?	;Page size.
LE_LastBytes		dd ?	;Bytes on last page.
LE_FixupSize		dd ?	;fixup section size.
LE_FixupChk		dd ?	;fixup section check sum.
LE_LoaderSize		dd ?	;loader section size.
LE_LoaderChk		dd ?	;loader section check sum.
LE_ObjOffset		dd ?	;offset of object table.
LE_ObjNum		dd ?	;object table entries
LE_PageMap		dd ?	;object page map table offset.
LE_IterateMap		dd ?	;object iterate data map offset.
LE_Resource		dd ?	;resource table offset
LE_ResourceNum		dd ?	;resource table entries.
LE_ResidentNames	dd ?	;resident names table offset.
LE_EntryTable		dd ?	;entry table offset.
LE_Directives		dd ?	;module directives table offset.
LE_DirectivesNum	dd ?	;module directives entries.
LE_Fixups		dd ?	;fixup page table offset.
LE_FixupsRec		dd ?	;fixup record table offset.
LE_ImportModNames	dd ?	;imported module name table offset.
LE_ImportModNum	dd ?	;imported modules count.
LE_ImportNames		dd ?	;imported procedures name table offset.
LE_PageChk		dd ?	;per-page checksum table offset.
LE_Data		dd ?	;data pages offset.
LE_PreLoadNum		dd ?	;pre-load page count.
LE_NoneRes		dd ?	;non-resident names table offset.
LE_NoneResSize		dd ?	;non-resident names table length.
LE_NoneResChk		dd ?	;non-resident names checksum.
LE_AutoDS		dd ?	;automatic data object.
LE_Debug		dd ?	;debug information offset.
LE_DebugSize		dd ?	;debug information size.
LE_PreLoadInstNum	dd ?	;pre-load instance pages number.
LE_DemandInstNum	dd ?	;demand instance pages number.
LE_HeapExtra		dd ?	;extra heap alloction.
LE_Reserved		db 20 dup (?) ;reserved.
LE_DeviceID		dw ?	;device ID (Windows VxD only).
LE_DDK		dw ?	;DDK version number.
LE_Header		ends


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;LE object format.
;
LE_OBJ		struc
LE_OBJ_Size		dd ?	;virtual size in bytes.
LE_OBJ_Base		dd ?	;relocation base address.
;
LE_OBJ_Flags		dd ?	;object flags.
LE_OBJ_Flags_Read	equ 1	;Readable.
LE_OBJ_Flags_Write	equ 2	;Writeable.
LE_OBJ_Flags_Exec	equ 4	;Executable.
LE_OBJ_Flags_Res	equ 8	;Resource.
LE_OBJ_Flags_Discard	equ 16	;Discardable.
LE_OBJ_Flags_Shared	equ 32	;Shared.
LE_OBJ_Flags_PreLoad	equ 64	;Preload.
LE_OBJ_Flags_Invalid	equ 128	;Invalid.
LE_OBJ_Flags_FillMsk	equ 256+512	;Mask for fill type bits.
LE_OBJ_Flags_Normal	equ 0	;Normal fill type.
LE_OBJ_Flags_Zero	equ 256	;Zero filled.
LE_OBJ_Flags_Res1	equ 512	;resident.
LE_OBJ_Flags_Res2	equ 256+512	;resident/contiguous.
LE_OBJ_Flags_LongLoc	equ 1024	;long lockable.
LE_OBJ_Flags_16Alias	equ 4096	;16:16_ALIAS
LE_OBJ_Flags_Big	equ 8192	;"BIG" (Huge: 32-bit)
LE_OBJ_Flags_Conform	equ 16384	;Conforming.
LE_OBJ_Flags_IOPriv	equ 32768	;"OBJECT_I/O_PRIVILEGE_LEVEL
;
LE_OBJ_PageIndex	dd ?	;page map index.
LE_OBJ_PageNum		dd ?	;page map entries.
LE_OBJ_Reserved	db 4 dup (0)	;reserved.
LE_OBJ		ends


;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;
;Somewhere to load the LE header.
;
LEHeader	LE_Header <>

LETemp	db 256 dup (0)

