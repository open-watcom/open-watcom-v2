;
;Add binary tree type sorting to publics and externs to speed addition and
; searching. Use CARS as a speed test.
;Make DOSSEG work, add command line switch as well.
;Use proper algorythm for LIB searching.
;Stop it writing un-needed data at the end of the program.
;Add a /stub option so can change loaders etc.
;
	.386
	.model small
	.stack 1024
	option oldstructs

	include ..\cw.inc

	include cwl.inc
	include ..\strucs.inc

	.code


CW_DEBUG_ENTRY_CS_EIP df Main
	public CW_DEBUG_ENTRY_CS_EIP


;------------------------------------------------------------------------------
;
;Main entry point.
;
Main	proc	near
	mov	ErrorESP,esp		;keep this for un-controled exit.
	;
	mov	esi,offset Copyright	;announce ourselves.
	call	PrintString
;
;See what's on the command line.
;
	call	ParseCommandLine
	jc	System		;Do we understand them?
	mov	ErrorNumber,1
	cmp	ObjFiles,0		;anything specified?
	jz	System
	mov	ebp,ObjFiles
	cmp	d[ebp],0
	jz	System
;
;Get initial memory for various lists.
;
	call	InitialGlobalAlloc	;Allocate initial memory for global lists.
	jc	System
;
;Parse the DEF file.
;
	call	ParseDefFile
	jc	System
;
;Do initialisation type stuff.
;
	call	InitLIBPath		;Setup LIB environment stuff.
	jc	System
	call	LoadObjs		;Load all the obj files specified.
	jc	System
	call	LoadLibs		;Load all the LIB files specified.
	jc	System
	call	GetEXEName		;See if we need a derived EXE file name.
	call	GetSYMName		;Derive SYM name.
	call	GetMAPName		;Derive MAP name.
	;
	call	AddInternalPUBDEFs	;Make sure internal PUDEFs are
	jc	System		;available for WLINK emulation.
;
;Get on with first pass at processing the data.
;
	call	InitialPassOne	;Deal with specified OBJ's.
	jc	System
	call	ResolveEXTDEF	;resolve all EXTDEF's and pull
	jc	System		;in LIB modules.
	;
	call	SortSEGDEFs		;Sort all the segments into desired order.
	jc	System
	call	FindStack		;Find/Create the stack segment.
	jc	System
	call	AllocateSegMem	;Work through global segments
	jc	System		;allocateing memory for them.
	;
	call	SortGROUPs		;Sort the groups.
	jc	System
	call	UpdateInternalPUBDEFs	;update internal symbols.
	jc	System
;
;Update EXPORT values etc.
;
	call	UpdateEXPORTS
	jc	System
;
;Now all the modules that are needed should be present so get
;on with the real processing.
;
	call	ObjPassTwo		;Fill segments & do fixups.
	jc	System
	;
	call	ApplyGROUPs		;Extend GROUP member limits.
	jc	System
;
;Time to produce the EXE.
;
	call	Write3PFile		;Write the output file.
	jc	System
;
;Lose segment memory to make space for line numbers etc.
;
	call	ReleaseSegMem
	jc	System
;
;Now write additional files (MAP/SYM)
;
	call	WriteMAPFile		;Write the MAP file.
	jc	System
	call	WriteSYMFile		;Write the symbols.
	jc	System
	;
	mov	ErrorNumber,0
	jmp	System
Main	endp


;------------------------------------------------------------------------------
;
;Main exit point.
;
System	proc	near
	mov	esp,ErrorESP
	cmp	ErrorCount,0		;Any errors yet?
	jz	@@0
	mov	esi,offset CarriageReturn
	call	PrintString
	cmp	ErrorNumber,0
	jnz	@@0
	mov	eax,ErrorCount
	mov	edi,offset ErrorM32
	call	Bin2Dec
	mov	ErrorName,offset ErrorM32_0
	mov	ErrorNumber,32
@@0:	mov	eax,ErrorNumber
	call	PrintError
	ret
System	endp


;------------------------------------------------------------------------------
;
;Display error message indicated by "ErrorNumber" along with info required for
;particular message. Very crude way of dumping relavent info but...
;
PrintError	proc	near
	pushad
	mov	esi,ErrorNumber
	mov	esi,[ErrorList+esi*4]
	call	PrintString
	;
	cmp	ErrorNumber,3	;File name needed?
	jz	@@ShowName
	cmp	ErrorNumber,4
	jz	@@ShowName
	cmp	ErrorNumber,5
	jz	@@ShowName
	cmp	ErrorNumber,6
	jz	@@ShowName
	cmp	ErrorNumber,7
	jz	@@ShowName
	cmp	ErrorNumber,9
	jz	@@ShowName
	cmp	ErrorNumber,13
	jz	@@ShowName
	cmp	ErrorNumber,15
	jz	@@ShowName
	cmp	ErrorNumber,16
	jz	@@ShowName
	cmp	ErrorNumber,17
	jz	@@ShowName
	cmp	ErrorNumber,18
	jz	@@ShowName
	cmp	ErrorNumber,19
	jz	@@ShowName
	cmp	ErrorNumber,20
	jz	@@ShowName
	cmp	ErrorNumber,22
	jz	@@ShowName
	cmp	ErrorNumber,23
	jz	@@ShowName
	cmp	ErrorNumber,24
	jz	@@ShowName
	cmp	ErrorNumber,26
	jz	@@ShowName
	cmp	ErrorNumber,29
	jz	@@ShowName
	cmp	ErrorNumber,30
	jz	@@ShowName
	cmp	ErrorNumber,32
	jz	@@ShowName
	cmp	ErrorNumber,33
	jz	@@ShowName
	cmp	ErrorNumber,34
	jz	@@ShowName
	cmp	ErrorNumber,35
	jz	@@ShowName
	jmp	@@NoName
	;
@@ShowName:	mov	esi,ErrorName
	or	esi,esi
	jnz	@@sn0
	mov	esi,offset NoNameString
@@sn0:	call	PrintString
	;
	cmp	ErrorNumber,5
	jz	@@ShowSource
	cmp	ErrorNumber,6
	jz	@@ShowSource
	cmp	ErrorNumber,7
	jz	@@ShowSource
	cmp	ErrorNumber,9
	jz	@@ShowSource
	cmp	ErrorNumber,13
	jz	@@ShowSource
	cmp	ErrorNumber,15
	jz	@@ShowSource
	cmp	ErrorNumber,16
	jz	@@ShowSource
	cmp	ErrorNumber,17
	jz	@@ShowSource
	cmp	ErrorNumber,18
	jz	@@ShowSource
	cmp	ErrorNumber,19
	jz	@@ShowSource
	cmp	ErrorNumber,20
	jz	@@ShowSource
	cmp	ErrorNumber,22
	jz	@@ShowSource
	cmp	ErrorNumber,23
	jz	@@ShowSource
	cmp	ErrorNumber,26
	jz	@@ShowSource
	cmp	ErrorNumber,29
	jz	@@ShowSource
	cmp	ErrorNumber,30
	jz	@@ShowSource
	cmp	ErrorNumber,33
	jz	@@ShowSource
	cmp	ErrorNumber,34
	jz	@@ShowSource
	cmp	ErrorNumber,35
	jz	@@ShowSource
	jmp	@@NoSource
	;
@@ShowSource:	cmp	ErrorName+4,0
	jz	@@NoSource
	mov	esi,offset SpaceString
	call	PrintString
	mov	esi,offset ParenOpenString
	call	PrintString
	mov	esi,ErrorName+4
	call	PrintString
	mov	esi,offset ParenCloseString
	call	PrintString
@@NoParen1:	;
@@NoSource:	cmp	ErrorNumber,13
	jz	@@ShowSymbol
	cmp	ErrorNumber,15
	jz	@@ShowSymbol
	cmp	ErrorNumber,16
	jz	@@ShowSymbol
	cmp	ErrorNumber,17
	jz	@@ShowSymbol
	cmp	ErrorNumber,18
	jz	@@ShowSymbol
	cmp	ErrorNumber,22
	jz	@@ShowSymbol
	cmp	ErrorNumber,26
	jz	@@ShowSymbol
	cmp	ErrorNumber,29
	jz	@@ShowSymbol
	cmp	ErrorNumber,30
	jz	@@ShowSymbol
	cmp	ErrorNumber,33
	jz	@@ShowSymbol
	cmp	ErrorNumber,34
	jz	@@ShowSymbol
	cmp	ErrorNumber,35
	jz	@@ShowSymbol
	jmp	@@NoSymbol
	;
@@ShowSymbol:	cmp	ErrorName+8,0
	jz	@@NoSymbol
	mov	esi,offset SpaceString
	call	PrintString
	mov	esi,ErrorName+8
	call	PrintString
	;
@@NoSymbol:	cmp	ErrorNumber,29
	jz	@@NoName
	mov	esi,offset CarriageReturn
	call	PrintString
	;
@@NoName:	inc	ErrorCount
	mov	ErrorNumber,0
	mov	ErrorName,0
	mov	ErrorName+4,0
	mov	ErrorName+8,0
	popad
	ret
PrintError	endp


;------------------------------------------------------------------------------
;
;Work through the EXPORT list setting entry values.
;
UpdateEXPORTS	proc	near
	pushad
;
;Check if there are any EXPORTs to process.
;
	mov	edi,EXPORTList
	mov	ebp,[edi]
	add	edi,4
@@0:	or	ebp,ebp
	jz	@@8
	pushm	edi,ebp
	mov	edi,[edi]
;
;Work through the list setting values.
;
@@1:	mov	esi,PUBLICList
	mov	ebp,[esi]
	add	esi,4
@@2:	or	ebp,ebp
	jz	@@6
	mov	ebx,[esi]		;point to PUBLIC
	cmp	PUB.PScope[ebx],0	;local?
	jnz	@@5
	mov	ebx,PUB.PName[ebx]	;point to the name.
	movzx	ecx,EXPORT.EXP_TextLen[edi]
	cmp	cl,[ebx]		;right length?
	jnz	@@5
	lea	edx,[edi+size EXPORT]
	inc	ebx
@@3:	mov	al,[ebx]
	mov	ah,[edx]
	cmp	CaseSensitive,0
	jnz	@@4
	call	UpperChar
	xchg	ah,al
	call	UpperChar
	xchg	ah,al
@@4:	cmp	al,ah
	jnz	@@5
	inc	ebx
	inc	edx
	dec	ecx
	jnz	@@3
;
;Found the matching PUBLIC so set EXPORT details.
;
	mov	ebx,[esi]
	mov	eax,PUB.PValue[ebx]
	mov	EXPORT.EXP_Offset[edi],eax
	mov	eax,PUB.PSEG[ebx]
	or	eax,eax
	jnz	@@11
;
;Use GROUP index to work out the segment.
;
	dec	eax
	shl	eax,2
	add	eax,4
	mov	edx,PUB.PObj[ebx]
	add	eax,Obj.MGroups[edx]	;index to this entry.
	mov	eax,[eax]		;get global GROUP index.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip name & count dword.
	add	eax,GROUPList	;index to this group.
	mov	eax,[eax]		;point to this group list.
	add	eax,4+4		;skip name and count dwords.
	mov	eax,[eax]		;get base LSEG
;
;Find the target SEGDEF in the local SEGDEF list and fetch
;the base offset.
;
	pushm	ecx,edi
	mov	edi,Obj.MSegs[edx]	;point to local SEGDEF's
	mov	ecx,[edi]		;get number of entries.
	add	edi,4
@@fl30_0:	cmp	eax,OSeg.OGSeg[edi]	;right global number?
	jz	@@fl30_1
	add	edi,size OSeg
	dec	ecx
	jnz	@@fl30_0
	jmp	@@fl30_2
@@fl30_1:	mov	ecx,OSeg.OBase[edi]	;get local offset.
	add	EXPORT.EXP_Offset[edi],ecx
@@fl30_2:	popm	ecx,edi
	jmp	@@14
;
;Use SEGDEF index to work out segment.
;
@@11:	dec	eax
	shl	eax,4
	add	eax,4
	mov	edx,PUB.PObj[ebx]
	add	eax,Obj.MSegs[edx]
	mov	edx,OSeg.OBase[eax]	;get local offset.
	add	EXPORT.EXP_Offset[edi],edx
	mov	eax,OSeg.OGSeg[eax]	;get global SEGDEF number.
	jmp	@@14
;
@@14:	mov	EXPORT.EXP_Seg[edi],ax
	jmp	@@7
;
;Move to next PUBLIC entry.
;
@@5:	add	esi,4
	dec	ebp
	jmp	@@2
;
;Display a warning about this EXPORT not being found.
;
@@6:	mov	ErrorNumber,22
	mov	edi,offset ErrorNameSpace1
	mov	ErrorName+4,edi
	mov	eax,DEFLineNumber
	call	Bin2Dec
	call	PrintError
;
;Move to next EXPORT entry.
;
@@7:	popm	edi,ebp
	add	edi,4
	dec	ebp
	jmp	@@0
;
;Make sure this module has a name.
;
@@8:	cmp	ModuleName,0
	jnz	@@110
;
;Construct a name from the output file name.
;
	mov	esi,EXEFiles
	mov	esi,[esi+4]
	;
	;Lose any path.
	;
	mov	edi,esi
@@120:	inc	esi
	cmp	b[esi-1],0
	jz	@@130
	cmp	b[esi-1],"\"
	jnz	@@120
	mov	edi,esi
	jmp	@@120
@@130:	;
	;Scan the length.
	;
	mov	esi,edi
	xor	ecx,ecx
@@140:	cmp	b[esi],0
	jz	@@150
	cmp	b[esi],"."
	jz	@@150
	inc	esi
	inc	ecx
	jmp	@@140
	;
@@150:	;Get some memory for the name.
	;
	inc	ecx
	call	Malloc
	mov	ErrorNumber,2
	jc	@@9
	mov	ModuleName,esi
	xchg	esi,edi
	dec	ecx
	mov	b[edi],cl
	inc	edi
	rep	movsb
;
@@110:	clc
	jmp	@@10
;
@@9_0:	popm	edi,ebp
@@9:	stc
@@10:	popad
	ret
UpdateEXPORTS	endp


;------------------------------------------------------------------------------
;
;Parse DEF file if any specified.
;
ParseDefFile	proc	near
	pushad
	cmp	DefFiles,0
	jz	@@8
	mov	edi,DefFiles
	cmp	d[edi],0
	jz	@@8
	mov	ebp,[edi]		;Get number of files to process.
	add	edi,4
@@0:
;
;Try and open the file.
;
	mov	edx,[edi]		;Point to the file name.
	mov	ErrorName,edx
	mov	ErrorNumber,3
	call	OpenFile
	jc	@@9
;
;Initialise buffered reading for this file.
;
	call	InitFileBuffer
	pushm	edi,ebp
;
;Now sit in a loop processing commands.
;
	mov	DEFLineNumber,0
@@1:	mov	edi,offset DEFLineBuffer
	call	ReadBufferLine
	mov	ErrorNumber,4
	jc	@@9_0
	cmp	ecx,-1		;EOF?
	jz	@@7
	inc	DEFLineNumber
	or	ecx,ecx		;Anything to process?
	jz	@@1
;
;Check if this is a comment line.
;
	cmp	b[edi],"#"
	jz	@@1
;
;Skip white space.
;
@@2:	inc	edi
	cmp	b[edi-1]," "
	jz	@@2
	cmp	b[edi-1],9
	jz	@@2
	dec	edi
;
;See if we understand the command.
;
	mov	esi,offset DEFCommands
@@3:	cmp	d[esi],0
	jnz	@@4
	mov	esi,edi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
@@3_0:	movsb
	cmp	b[esi-1],0
	jz	@@3_1
	cmp	b[esi-1]," "
	jz	@@3_1
	cmp	b[esi-1],9
	jz	@@3_1
	jmp	@@3_0
@@3_1:	mov	b[edi-1],0
	mov	ErrorNumber,7
	call	PrintError
	jmp	@@1
;
;See if this is the right command.
;
@@4:	mov	ebx,[esi]		;point to command string.
	push	edi
@@5:	mov	al,[ebx]
	call	UpperChar
	mov	ah,al
	mov	al,[edi]
	cmp	al," "
	jz	@@5_0
	cmp	al,9
	jz	@@5_0
	jmp	@@5_1
@@5_0:	xor	al,al
@@5_1:	call	UpperChar
	cmp	al,ah
	jnz	@@6
	or	al,al
	jz	@@11
	inc	edi
	inc	ebx
	jmp	@@5
@@6:	pop	edi
	add	esi,4*4
	jmp	@@3
;
;We have the command so call it's processor.
;
@@11:	mov	ebx,DEFFiles
	mov	eax,[ebx]
	pushad
	call	d[esi+4]
	popad
	pop	edi
	jc	@@9_0
	popm	edi,ebp
	sub	edi,ebx
	add	edi,DEFFiles
	mov	ebx,DEFFiles
	mov	ebx,[ebx]
	sub	ebx,eax
	add	ebp,ebx
	pushm	edi,ebp
	jmp	@@1
;
;Move onto the next DEF file.
;
@@7:	popm	edi,ebp
	add	edi,4
	dec	ebp
	jnz	@@0
;
@@8:	clc
	jmp	@@10
;
@@9_0:	popm	edi,ebp
@@9:	stc
@@10:	popad
	ret
ParseDefFile	endp


;*******************************************************************************
;
;Process a DEF file INCLUDE command.
;
;On Entry:
;
;EDI	- rest of command statement.
;
DEF_IncludeCOM	proc	near
;
;Skip any remaining white space.
;
@@0:	inc	edi
	cmp	b[edi-1]," "
	jz	@@0
	cmp	b[edi-1],9
	jz	@@0
	dec	edi
;
;Scan the length of the symbol.
;
	mov	ebx,edi
@@1:	cmp	b[edi],0
	jz	@@2
	cmp	b[edi]," "
	jz	@@2
	cmp	b[edi],9
	jz	@@2
	inc	edi
	jmp	@@1
;
;Terminate the name.
;
@@2:	mov	b[edi],0
;
;Add this entry to the DEF file list.
;
	mov	edi,ebx
	mov	esi,offset DEFFiles
	call	AddFileName2List	;Add this name.
	mov	ErrorNumber,2
	jc	@@9
;
@@8:	clc
	jmp	@@10
;
@@9:	stc
;
@@10:	ret
DEF_IncludeCOM	endp


;*******************************************************************************
;
;Process a DEF file NAME command.
;
;On Entry:
;
;EDI	- rest of command statement.
;
DEF_NameCOM	proc	near
	push	esi
	mov	esi,edi
	call	UpperString
	pop	esi
;
;Skip any remaining white space.
;
@@0:	inc	edi
	cmp	b[edi-1]," "
	jz	@@0
	cmp	b[edi-1],9
	jz	@@0
	dec	edi
;
;Scan the length of the symbol.
;
	xor	ecx,ecx
	mov	ebx,edi
@@1:	cmp	b[edi],0
	jz	@@2
	cmp	b[edi]," "
	jz	@@2
	cmp	b[edi],9
	jz	@@2
	inc	ecx
	inc	edi
	jmp	@@1
;
;Get some memory for this text.
;
@@2:	inc	ecx
	call	Malloc
	mov	ErrorNumber,2
	jc	@@9
;
;Copy text through to allocated memory.
;
	push	esi
	mov	edi,esi
	mov	esi,ebx
	dec	ecx
	mov	b[edi],cl
	inc	edi
	rep	movsb
	pop	esi
;
;Set modules name string.
;
	mov	ModuleName,esi
;
@@8:	clc
	jmp	@@10
;
@@9:	stc
;
@@10:	ret
DEF_NameCOM	endp


;*******************************************************************************
;
;Process a DEF file EXPORT command.
;
;On Entry:
;
;EDI	- rest of command statement.
;
DEF_ExportCOM	proc	near
	push	esi
	mov	esi,edi
	call	UpperString
	pop	esi
;
;Skip any remaining white space.
;
@@0:	inc	edi
	cmp	b[edi-1]," "
	jz	@@0
	cmp	b[edi-1],9
	jz	@@0
	dec	edi
;
;Scan the length of the symbol.
;
	xor	ecx,ecx
	mov	ebx,edi
@@1:	cmp	b[edi],0
	jz	@@2
	cmp	b[edi]," "
	jz	@@2
	cmp	b[edi],9
	jz	@@2
	inc	ecx
	inc	edi
	jmp	@@1
;
;Get some memory for this symbol.
;
@@2:	add	ecx,size EXPORT
	call	Malloc
	mov	ErrorNumber,2
	jc	@@9
;
;Make sure EXPORT is initialised to at least zero's.
;
	pushm	ecx,edi
	xor	eax,eax
	mov	ecx,size EXPORT
	mov	edi,esi
	rep	stosb
	popm	ecx,edi
;
;Store text length.
;
	sub	ecx,size EXPORT
	mov	EXPORT.EXP_TextLen[esi],cl
	mov	edx,esi
	mov	edi,esi
	mov	esi,ebx
	add	edi,size EXPORT
;
;Copy symbol.
;
@@3:	mov	al,[esi]
	mov	[edi],al
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@3
;
;Expand the EXPORT list.
;
	ExpandList EXPORTList,ListBumpG
	mov	ErrorNumber,2
	jc	@@9
;
;Add this entry.
;
	mov	esi,EXPORTList
	mov	ecx,[esi]
	inc	d[esi]	
	mov	d[esi+4+ecx*4],edx
;
@@8:	clc
	jmp	@@10
;
@@9:	stc
;
@@10:	ret
DEF_ExportCOM	endp


;*******************************************************************************
;
;Process a DEF file IMPORT command.
;
;On Entry:
;
;EDI	- rest of command statement.
;
DEF_ImportCOM	proc	near
	mov	IMPORTInternal,0
	push	esi
	mov	esi,edi
	call	UpperString
	pop	esi
;
;Get a new entry for this IMPORT.
;
	mov	ecx,size IMPORT
	call	Malloc
	mov	ErrorNumber,2
	jc	@@9
	ExpandList IMPORTList,ListBumpG
	mov	ErrorNumber,2
	jc	@@9
	mov	ebx,IMPORTList
	mov	eax,[ebx]		;get entry number.
	inc	d[ebx]
	mov	d[ebx+4+eax*4],esi	;store pointer to this entry.
	mov	IMPORTTemp,esi
	push	edi
	mov	edi,esi
	mov	ecx,size IMPORT
	xor	eax,eax
	rep	stosb
	pop	edi
;
;Skip any remaining white space.
;
@@0:	inc	edi
	cmp	b[edi-1]," "
	jz	@@0
	cmp	b[edi-1],9
	jz	@@0
	dec	edi
;
;Scan the length of the module name.
;
	xor	edx,edx
	mov	ebx,edi
@@1:	cmp	b[edi],0
	jz	@@2
	cmp	b[edi]," "
	jz	@@2_0
	cmp	b[edi],9
	jz	@@2_0
	cmp	b[edi],"."
	jz	@@3
	inc	edx
	inc	edi
	jmp	@@1
;
;May be specifying an internal name.
;
@@2_0:	cmp	IMPORTInternal,0
	jnz	@@2		;already got an internal name.
	cmp	b[edi+1],0
	jz	@@2
	mov	IMPORTInternal,ebx
	mov	IMPORTInternal+4,edx
	jmp	@@0
;
;Not the format we were expecting so print a warning and ignore this line.
;
@@2:	mov	ErrorNumber,33
	pushad
	mov	edi,offset ErrorNameSpace1
	mov	ErrorName+4,edi
	mov	eax,DEFLineNumber
	call	Bin2Dec
	call	PrintError
	popad
	jmp	@@8
;
;See if this module name is already in the list.
;
@@3:	inc	edi		;skip the period
	mov	esi,IMPORTModules
	mov	ebp,[esi]
	add	esi,4
@@4:	or	ebp,ebp
	jz	@@11
	pushm	esi,ebx
	mov	esi,[esi]
	cmp	dl,[esi]		;right length?
	jnz	@@7
	mov	ecx,edx
	inc	esi
@@5:	mov	al,[esi]
	mov	ah,[ebx]
	call	UpperChar
	xchg	ah,al
	call	UpperChar
	xchg	ah,al
	cmp	al,ah
	jnz	@@7
	inc	esi
	inc	ebx
	dec	ecx
	jnz	@@5
	popm	esi,ebx
	mov	eax,IMPORTModules
	mov	eax,[eax]
	sub	eax,ebp		;Get entry number.
	jmp	@@12		;found a match.
@@7:	popm	esi,ebx
	add	esi,4
	dec	ebp
	jmp	@@4
;
;Need to add a new entry to the list.
;
@@11:	mov	ecx,edx
	inc	ecx
	;
	mov	eax,ecx
	add	eax,4
	add	IMPORTLength,eax	;add space for this entry.
	add	IMPORTLength+4,eax
	add	IMPORTLength+8,eax
	;
	call	Malloc		;get memory for module name string.
	mov	ErrorNumber,2
	jc	@@9
	dec	ecx
	mov	b[esi],cl
	pushm	esi,edi
	mov	edi,esi
	inc	edi
	mov	esi,ebx
@@modname0:	mov	al,[esi]
	inc	esi
	call	UpperChar
	mov	[edi],al
	inc	edi
	dec	ecx
	jnz	@@modname0
	popm	esi,edi
	;
	ExpandList IMPORTModules,ListBumpG
	mov	ErrorNumber,2
	jc	@@9
	;
	mov	ebx,IMPORTModules
	mov	eax,[ebx]		;get entry number.
	inc	d[ebx]
	mov	d[ebx+4+eax*4],esi	;store pointer to this name.
;
;Update this IMPORT with the module name index.
;
@@12:	mov	esi,IMPORTTemp
	mov	IMPORT.IMP_Module[esi],eax
;
;Now scan the length of the symbol name.
;
	mov	ebx,edi
	xor	edx,edx
@@13:	cmp	b[edi],0
	jz	@@14
	cmp	b[edi]," "
	jz	@@14
	cmp	b[edi],9
	jz	@@14
	inc	edi
	inc	edx
	jmp	@@13
;
;Set "internal" name for this import.
;
@@14:	pushad
	cmp	IMPORTInternal,0
	jz	@@14_0
	mov	ebx,IMPORTInternal
	mov	edx,IMPORTInternal+4
@@14_0:	mov	ecx,edx
	inc	ecx
	call	Malloc
	jc	@@14_1
	mov	edi,esi
	mov	esi,IMPORTTemp
	mov	IMPORT.IMP_IName[esi],edi
	dec	ecx
	mov	[edi],cl
	inc	edi
	mov	esi,ebx
	rep	movsb
	clc
@@14_1:	popad
	jnc	@@14_2
	mov	ErrorNumber,2
	call	PrintError
	jmp	@@8
;
;Check if the symbol is actually an ordinal number.
;
@@14_2:	mov	edi,ebx
	cmp	b[edi],"0"
	jc	@@21
	cmp	b[edi],"9"+1
	jnc	@@21
	;
	;Check an internal name has been set.
	;
	cmp	IMPORTInternal,0
	jnz	@@25
	pushad
	mov	ErrorNumber,34
	mov	edi,offset ErrorNameSpace1
	mov	ErrorName+4,edi
	mov	eax,DEFLineNumber
	call	Bin2Dec
	call	PrintError
	popad
	jmp	@@8
	;
@@25:	;Get the ordinal number.
	;
	mov	ebx,edi
	xor	ecx,ecx
@@22:	cmp	b[edi],0
	jz	@@24
	cmp	b[edi]," "
	jz	@@24
	cmp	b[edi],9
	jz	@@24
	cmp	b[edi],"0"
	jc	@@23
	cmp	b[edi],"9"+1
	jnc	@@23
	mov	eax,ecx
	shl	eax,1
	shl	ecx,3
	add	ecx,eax
	xor	eax,eax
	mov	al,[edi]
	sub	al,"0"
	add	ecx,eax
	inc	edi
	jmp	@@22
	;
@@23:	pushad
	mov	ErrorNumber,35
	mov	edi,offset ErrorNameSpace1
	mov	ErrorName+4,edi
	mov	eax,DEFLineNumber
	call	Bin2Dec
	call	PrintError
	popad
	jmp	@@8
	;
@@24:	or	ecx,ecx
	jz	@@23
	mov	esi,IMPORTTemp
	mov	IMPORT.IMP_Name[esi],ecx	;set name ordinal.
	or	IMPORT.IMP_Flags[esi],128	;flag as ordinal.
	jmp	@@8
;
;Check if this name already exists.
;
@@21:	mov	esi,IMPORTNames
	mov	ebp,[esi]
	add	esi,4
@@15:	or	ebp,ebp
	jz	@@19
	pushm	ebx,esi
	mov	esi,[esi]
	cmp	dl,[esi]		;right length?
	jnz	@@18
	inc	esi
	mov	ecx,edx
@@16:	mov	al,[esi]
	mov	ah,[ebx]
	cmp	CaseSensitive,0
	jnz	@@17
	call	UpperChar
	xchg	ah,al
	call	UpperChar
	xchg	ah,al
@@17:	cmp	al,ah
	jnz	@@18
	inc	esi
	inc	ebx
	or	al,al		;end of the symbol?
	jnz	@@16
	popm	ebx,esi
	mov	eax,IMPORTNames
	mov	eax,[eax]
	sub	eax,ebp		;get entry number.
	jmp	@@20
@@18:	popm	ebx,esi
	add	esi,4
	dec	ebp
	jmp	@@15
;
;Need to add this name to the list.
;
@@19:	mov	ecx,edx
	inc	ecx
	;
	mov	eax,ecx
	add	eax,4
	add	IMPORTLength,eax	;add space for this entry.
	add	IMPORTLength+8,eax
	;
	call	Malloc		;get memory for module name string.
	mov	ErrorNumber,2
	jc	@@9
	dec	ecx
	mov	b[esi],cl
	pushm	esi,edi
	mov	edi,esi
	inc	edi
	mov	esi,ebx
	rep	movsb		;copy symbol name.
	popm	esi,edi
	;
	ExpandList IMPORTNames,ListBumpG
	mov	ErrorNumber,2
	jc	@@9
	;
	mov	ebx,IMPORTNames
	mov	eax,[ebx]		;get entry number.
	inc	d[ebx]
	mov	d[ebx+4+eax*4],esi	;store pointer to this name.
;
;Update this IMPORT with the symbol name index.
;
@@20:	mov	esi,IMPORTTemp
	mov	IMPORT.IMP_Name[esi],eax
;
@@8:	clc
	jmp	@@10
;
@@9:	stc
;
@@10:	ret
DEF_ImportCOM	endp


;*******************************************************************************
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
;*******************************************************************************
InitFileBuffer	proc	near
	mov	FileBufferCount,0	;reset buffer count.
	mov	FileBufferHandle,ebx	;store handle.
	ret
InitFileBuffer	endp


;*******************************************************************************
;Read a line of text from input file specified.
;
;On Entry:
;
;EDI	- Buffer.
;
;On Exit:
;
;Carry set on error else,
;
;ECX	- Buffer length, -1=EOF.
;
;*******************************************************************************
ReadBufferLine	proc	near
	public ReadBufferLine
	pushm	eax,ebx,edx,edi
	mov	ebx,FileBufferHandle
	xor	ecx,ecx		;reset bytes so far count.
;
;Read a byte loop point.
;
l0:	call	ReadBufferByte
	jc	l9
	mov	[edi],al
	or	eax,eax		;read anything?
	jz	CheckEOF
	cmp	b[edi],13		;EOL?
	jz	CheckLF
	cmp	b[edi],26
	jz	SoftEOF		;move to the end of the file, then do an EOL.
	inc	ecx
	inc	edi
	cmp	ecx,1024		;line getting a bit long?
	jnc	SoftEOF
	jmp	l0		;keep going.
;
;Found a CR so look for the coresponding LF.
;
CheckLF:	mov	b[edi],0		;terminate this line.
	inc	edi
	call	ReadBufferByte
	jc	l9
	mov	b[edi],al
	or	eax,eax		;read anything?
	jz	l9		;no LF here is an error.
	cmp	b[edi],10		;/
	jnz	l9		;/
	jmp	l8
;
;Move to the end of the source file to make it look like we really read everything.
;
SoftEOF:	pushm	ebx,ecx,edi
	mov	ebx,FileBufferHandle
	xor	ecx,ecx
	mov	al,2
	call	SetFilePointer
	popm	ebx,ecx,edi		;fall through to treat like hard EOF.
	mov	FileBufferCount,0
;
;Check if we should return EOF this time or next.
;
CheckEOF:	or	ecx,ecx		;read anything on this line yet?
	jnz	l8		;do EOL this time.
;
;Return EOF this time.
;
EOF:	or	ecx,-1
;
;Terminate the line.
;
l8:	mov	b[edi],0		;terminate the line.
	clc
	jmp	l10
;
;Some sort of error occured so make sure carry is set.
;
l9:	stc
;
l10:	popm	eax,ebx,edx,edi
	ret
ReadBufferLine	endp


;*******************************************************************************
;Read a byte from the file buffer.
;
;On Entry:
;
;None
;
;On Exit:
;
;Carry set on error else,
;
;AH	- Bytes read, ie, AH=0 indicates EOF otherwise AH=1
;AL	- Byte read.
;
;eg, if EAX=0 and no carry then EOF reached.
;
;Don't mix normal ReadFile call's with this call, they don't take account of
;each other.
;
;*******************************************************************************
ReadBufferByte	proc	near
	xor	eax,eax
	cmp	FileBufferCount,0	;anything in the buffer?
	jnz	r0
	;
	;Need to re-fill the buffer.
	;
	pushm	eax,ebx,ecx,edx
	mov	edx,offset FileBuffer
	mov	ebx,FileBufferHandle
	mov	ecx,1024
	call	ReadFile
	mov	FileBufferCount,eax
	mov	FileBufferPosition,offset FileBuffer
	popm	eax,ebx,ecx,edx
	jc	r8
	;
r0:	cmp	FileBufferCount,0	;still zero?
	jz	r8
	push	esi
	mov	esi,FileBufferPosition
	inc	FileBufferPosition
	dec	FileBufferCount
	mov	al,[esi]
	inc	ah
	pop	esi
	clc
	;
r8:	ret
ReadBufferByte	endp


;------------------------------------------------------------------------------
;
;Add some linker supplied PUBDEF's for WLINK compatibility.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
AddInternalPUBDEFs proc near
	pushad
;
;Add _end
;
	ExpandList PUBLICList,ListBumpG
	mov	ErrorNumber,2
	jc	@@9
	mov	edi,PUBLICList
	mov	eax,[edi]
	inc	d[edi]		;update number of entries.
	lea	edi,[edi+4+eax*4]	;point to new entry.
	mov	[edi],offset InternalPUBDEF__end
	or	d[PUB.PFlags+InternalPUBDEF__end],2
;
;Add _edata
;
	ExpandList PUBLICList,ListBumpG
	mov	ErrorNumber,2
	jc	@@9
	mov	edi,PUBLICList
	mov	eax,[edi]
	inc	d[edi]		;update number of entries.
	lea	edi,[edi+4+eax*4]	;point to new entry.
	mov	[edi],offset InternalPUBDEF__edata
	or	d[PUB.PFlags+InternalPUBDEF__edata],2
;
;Add _estack
;
	ExpandList PUBLICList,ListBumpG
	mov	ErrorNumber,2
	jc	@@9
	mov	edi,PUBLICList
	mov	eax,[edi]
	inc	d[edi]		;update number of entries.
	lea	edi,[edi+4+eax*4]	;point to new entry.
	mov	[edi],offset InternalPUBDEF__estack
	or	d[PUB.PFlags+InternalPUBDEF__estack],2
;
	clc
	jmp	@@10
;
@@9:	stc
@@10:	popad
	ret
AddInternalPUBDEFs endp


;------------------------------------------------------------------------------
;
;Update internaly maintained PUBDEFs.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
UpdateInternalPUBDEFs proc near
	pushad
;
;Work out _end values.
;
	mov	esi,offset SegClassBSS
	xor	edi,edi
	call	FindLastSEGDEFClass	;Look for last class of BSS
	jc	@@0
	inc	eax		;Publics use +1
	mov	edi,offset InternalPUBDEF__end
	mov	PUB.PSeg[edi],eax
	dec	eax
	shl	eax,2
	add	eax,4
	add	eax,SEGDEFList
	mov	eax,[eax]
	mov	eax,LSeg.SLength[eax]
	mov	PUB.PValue[edi],eax
;
;Work out _edata values.
;
@@0:	mov	esi,offset SegClassBSS
	xor	edi,edi
	call	FindSEGDEFClass	;Look for first class of BSS
	jc	@@1
	inc	eax		;Publics use +1
	mov	edi,offset InternalPUBDEF__edata
	mov	PUB.PSeg[edi],eax
;
;Work out _estack values.
;
@@1:	mov	esi,offset SegClassSTACK
	xor	edi,edi
	call	FindLastSEGDEFClass	;Look for last class of BSS
	jc	@@2
	inc	eax		;Publics use +1
	mov	edi,offset InternalPUBDEF__estack
	mov	PUB.PSeg[edi],eax
	dec	eax
	shl	eax,2
	add	eax,4
	add	eax,SEGDEFList
	mov	eax,[eax]
	mov	eax,LSeg.SLength[eax]
	mov	PUB.PValue[edi],eax
;
@@2:	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
UpdateInternalPUBDEFs endp


;------------------------------------------------------------------------------
;
;Works it's way through initial object list calling object pass one processing
;routine(s)
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
InitialPassOne	proc	near
	pushad
	mov	edi,ObjList		;Point to list.
	mov	ebp,[edi]		;Get number of entries.
	add	edi,4		;Point to real data.
@@0:	push	edi
	mov	edi,[edi]
	call	ObjPassOne		;Do pass one processing.
	pop	edi
	jc	@@9
	add	edi,4
	dec	ebp
	jnz	@@0
	clc
	jmp	@@10
@@9:	stc
@@10:	popad
	ret
InitialPassOne	endp


;------------------------------------------------------------------------------
;
;Do pass one processing of an object module. This includes checking we know
;about all record types, checking all checksums, extracting LIB names and
;registering PUBDEF and EXTDEF records.
;
;On Entry:
;
;EDI	- Module struc.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
ObjPassOne	proc	near
	pushad
	mov	esi,Obj.Module[edi]	;point to current module.
	mov	ebp,Obj.MLength[edi]	;get length so we can check for over-run.
	mov	PharLapFlag,0
	jmp	ObjPassOneProcess
ObjPassOne	endp


;------------------------------------------------------------------------------
;
;This is the processing header stuff. It checks record type validity and
;optionaly the checksum.
;
ObjPassOneProcess proc near
	push	ebp
	;
@@2:	;Check this record doesn't extend past the end of the file.
	;
	movzx	eax,w[esi+1]		;Get record length.
	add	eax,3		;Include type and length.
	cmp	ebp,eax		;Longer than what we have left?
	jnc	@@3
	;
	;Try and work out what the module name is.
	;
	mov	ErrorNumber,5
	mov	eax,Obj.MFileName[edi]
	mov	ErrorName,eax
	mov	esi,Obj.MName[edi]
	or	esi,esi
	jz	ObjPassOneError
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
	jmp	ObjPassOneError
	;
@@3:	;Check the checksum.
	;
	cmp	CHKSums,0
	jz	@@4
	movzx	ecx,w[esi+1]		;Get record length.
	add	ecx,3		;Include type and length.
	xor	al,al
	push	esi
@@3_0:	add	al,[esi]		;Update checksum.
	inc	esi
	dec	ecx
	jnz	@@3_0
	pop	esi
	or	al,al		;Should be zero at this point.
	jz	@@4
	mov	ErrorNumber,6
	;
	;Try and work out what the module name is.
	;
	mov	ErrorNumber,5
	mov	eax,Obj.MFileName[edi]
	mov	ErrorName,eax
	mov	esi,Obj.MName[edi]
	or	esi,esi
	jz	ObjPassOneError
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
	jmp	ObjPassOneError
	;
@@4:	;Now we can process this record if it's relavent at this point.
	;
	xor	eax,eax
	mov	al,[esi]		;Get the record type.
	mov	ObjPassOneType,al
	movzx	edx,w[esi+1]
	add	esi,3
	pop	ebp
	sub	ebp,3
	sub	ebp,edx		;Update length remaining count.
	push	ebp
	;
	jmp	d[ObjPassOneTable+eax*4]	;Pass to relavent routine.
ObjPassOneProcess endp


;------------------------------------------------------------------------------
;
;Deal with an entry we're not really interested in.
;
ObjPassOneNext	proc	near
	add	esi,edx
	pop	ebp
	jmp	ObjPassOneProcess
ObjPassOneNext	endp


;------------------------------------------------------------------------------
;
;Bad (un-recognised) record type found, display info and exit.
;
ObjPassOneBad	proc	near
	push	edi
	mov	edi,offset ErrorM19_N
	mov	ecx,2
	call	Bin2Hex
	pop	edi
	mov	ErrorNumber,19
	;
	;Try and work out what the module name is.
	;
	mov	eax,Obj.MFileName[edi]
	mov	ErrorName,eax
	mov	esi,Obj.MName[edi]
	or	esi,esi
	jz	ObjPassOneError
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
;	call	ShowInvalidObjRecType
	jmp	ObjPassOneError
ObjPassOneBad	endp


;------------------------------------------------------------------------------
;
;Some sort of error has occured so set carry and exit.
;
ObjPassOneError proc near
	stc
	jmp	ObjPassOneExit
ObjPassOneError endp


;------------------------------------------------------------------------------
;
;Everything went well so clear carry and exit.
;
ObjPassOneOK	proc	near
	clc
	jmp	ObjPassOneExit
ObjPassOneOK	endp


;------------------------------------------------------------------------------
;
;Clean up the stack and exit.
;
ObjPassOneExit	proc near
	pop	ebp
	popad
	ret
ObjPassOneExit	endp


;------------------------------------------------------------------------------
;
;Get modules source file name for debug info storage.
;
ObjPassOne_THEADR proc near
	mov	Obj.MName[edi],esi
	jmp	ObjPassOneNext
ObjPassOne_THEADR endp


;------------------------------------------------------------------------------
;
;Process a COMENT record.
;
ObjPassOne_COMENT proc near
	mov	al,[esi+1]		;Get the comment type.
	cmp	al,COM_PHARLAP	;32-bit default?
	jnz	@@com0
	or	PharLapFlag,-1
	jmp	ObjPassOneNext
	;
@@com0:	cmp	al,COM_LIBDEF	;LIB?
	jnz	@@com1
	cmp	DefaultLibraries,0
	jz	ObjPassOneNext	;Skip it for /NOD
	add	esi,2		;Point to the name.
	sub	edx,2
	dec	edx		;Don't include the checksum.
	push	edi
	mov	edi,offset FileNameSpace
	xor	ebx,ebx
	mov	ecx,edx
@@com0_1:	mov	al,[esi]
	mov	[edi],al
	inc	esi
	inc	edi
	cmp	al,"."
	jnz	@@com0_2
	mov	ebx,edi
@@com0_2:	cmp	al,"\"
	jnz	@@com0_3
	xor	ebx,ebx
@@com0_3:	dec	ecx
	jnz	@@com0_1
	or	ebx,ebx
	jnz	@@com0_4
	push	esi
	mov	esi,offset ExtensionList+12
	mov	ecx,4
	rep	movsb
	pop	esi
@@com0_4:	xor	al,al
	stosb			;has to be zero terminated.
	pop	edi
	inc	esi		;skip the checksum.
	;
	mov	edx,offset FileNameSpace
	call	LoadLIBFile
	jc	ObjPassOneError
	xor	edx,edx
	jmp	ObjPassOneNext
	;
@@com1:	cmp	al,0a2h		;pass one marker?
	jnz	@@com2
	jmp	ObjPassOneOK		;pretend we finished.
	;
@@com2:	cmp	al,COM_DOSSEG
	jnz	@@com3
	or	DOSSEGSegmentOrder,-1
	jmp	ObjPassOneNext
	;
@@com3:	cmp	al,COM_OMF1		;OMF extension 1?
	jnz	@@com4
	cmp	b[esi+2],1		;IMPDEF?
	jnz	@@com3_0
	;
	;Create a text IMPORT command and pass to IMPORT processor.
	;
	pushad
	mov	edi,offset DefLineBuffer
	mov	b[edi+0],"I"
	mov	b[edi+1],"M"
	mov	b[edi+2],"P"
	mov	b[edi+3],"O"
	mov	b[edi+4],"R"
	mov	b[edi+5],"T"
	mov	b[edi+6]," "
	add	edi,7
	;
	;Do local name.
	;
	lea	edx,[esi+1+1+1+1]
	movzx	ecx,b[edx]
	inc	edx
@@com3_1_0:	mov	al,[edx]
	mov	[edi],al
	inc	edx
	inc	edi
	dec	ecx
	jnz	@@com3_1_0
	mov	b[edi]," "
	inc	edi
	;
	;Do module name.
	;
	lea	edx,[esi+1+1+1+1]
	movzx	eax,b[edx]
	inc	eax
	add	edx,eax
	movzx	ecx,b[edx]
	inc	edx
@@com3_1_1:	mov	al,[edx]
	mov	[edi],al
	inc	edx
	inc	edi
	dec	ecx
	jnz	@@com3_1_1
	mov	b[edi],"."
	inc	edi
	;
	;Do external name or ordinal.
	;
	lea	edx,[esi+1+1+1+1]
	movzx	eax,b[edx]
	inc	eax
	add	edx,eax
	movzx	eax,b[edx]
	inc	eax
	add	edx,eax
	cmp	b[esi+3],0
	jz	@@com3_1_2
	;
	;Generate ordinal number.
	;
	movzx	eax,w[edx]
	call	Bin2Dec
	jmp	@@com3_1_4
	;
@@com3_1_2:	;Generate external name.
	;
	movzx	ecx,b[edx]
	inc	edx
	or	ecx,ecx
	jnz	@@com3_1_3
	lea	edx,[esi+1+1+1+1]	;use local name.
	movzx	ecx,b[edx]
	inc	edx
@@com3_1_3:	mov	al,[edx]
	mov	[edi],al
	inc	edx
	inc	edi
	dec	ecx
	jnz	@@com3_1_1
	;
@@com3_1_4:	;Call IMPORT handler.
	;
	mov	b[edi],0
	mov	edi,offset DefLineBuffer+6
	call	DEF_ImportCOM
	popad
	jmp	ObjPassOneNext
	;
@@com3_0:	jmp	ObjPassOneNext
	;
@@com4:	jmp	ObjPassOneNext
ObjPassOne_COMENT endp


;------------------------------------------------------------------------------
;
;Deal with a PUBDEF record.
;
ObjPassOne_PUBDEF proc near
	cmp	PharLapFlag,0
	jz	@@pdx0
	or	ObjPassOneType,1
@@pdx0:	;
	dec	edx		;Don't include checksum.
	GetIndex		;Get the group index.
	mov	PUBDEF_GRP,eax
	GetIndex		;Get the SEGDEF index.
	mov	PUBDEF_SEG,eax
	;
	;Get the FRAME number if needed.
	;
	mov	eax,PUBDEF_GRP
	or	eax,PUBDEF_SEG
	jnz	@@pd2
	xor	eax,eax
	mov	ax,w[esi]
	add	esi,2
	sub	edx,2
	mov	PUBDEF_FRAME,eax
	;
@@pd2:	;Now sit in a loop getting names and values.
	;
	or	edx,edx		;Anything left?
	jz	@@pd9
	;
	;Check this isn't a duplicate symbol.
	;
	mov	PUBDEFObj,edi
	xor	eax,eax
	mov	al,ObjPassOneType
	and	al,not 1		;lose size bit.
	xor	al,PUBDEF
;	not	al
	mov	PUBDEFScope,eax
	call	FindGlobalPUBDEF
	jc	@@pd2_0
	;
	;oops, already got this symbol so issue a warning.
	;
	;Show this entries details.
	;
	mov	ErrorNumber,29
	push	esi
	mov	esi,Obj.MFileName[edi]
	mov	ErrorName,esi
	mov	esi,Obj.MName[edi]
	mov	ErrorName+4,esi
	or	esi,esi
	jz	@@NoSName0
	push	ecx
	push	edi
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
	pop	edi
	pop	ecx
@@NoSName0:	pop	esi
	pushm	ecx,esi,edi
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace3
	mov	ErrorName+8,edi
	mov	b[edi],13
	inc	edi
	mov	b[edi],10
	inc	edi
	rep	movsb
	mov	b[edi],0
	popm	ecx,esi,edi
	call	PrintError
	;
	;Show origional entries details.
	;
	mov	ErrorNumber,30
	pushad
	shl	eax,2		;Write name of module that has
	add	eax,4		;origional definition.
	add	eax,PUBLICList
	mov	esi,[eax]
	push	esi
	mov	esi,PUB.PObj[esi]
	mov	eax,Obj.MFileName[esi]
	mov	ErrorName,eax
	mov	esi,Obj.MName[esi]
	mov	ErrorName+4,esi
	or	esi,esi
	jz	@@NoSName1
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
@@NoSName1:	pop	esi
	popad
	mov	ErrorName+8,0
	call	PrintError
	dec	ErrorCount
	;
@@pd2_0:	;Expand the list.
	;
	ExpandList PUBLICList,ListBumpG
	mov	ErrorNumber,2
	jc	ObjPassOneError
	;
	;Get PUB struc.
	;
	push	esi
	mov	ecx,size PUB
	call	Malloc
	mov	eax,esi
	pop	esi
	mov	ErrorNumber,2
	jc	ObjPassOneError
	push	edi
	mov	edi,eax
	push	eax
	xor	al,al
	rep	stosb
	;
	;Add this entry to the list.
	;
	mov	edi,PUBLICList
	mov	eax,[edi]
	inc	d[edi]		;update number of entries.
	lea	edi,[edi+4+eax*4]	;point to new entry.
	pop	eax
	mov	[edi],eax
	mov	ebx,eax
	pop	edi
	;
	;Set this entries details.
	;
	mov	PUB.PObj[ebx],edi
	mov	eax,PUBDEF_GRP
	mov	PUB.PGRP[ebx],eax
	mov	eax,PUBDEF_SEG
	mov	PUB.PSEG[ebx],eax
	mov	eax,PUBDEF_FRAME
	mov	PUB.PFRAME[ebx],eax
	mov	PUB.PName[ebx],esi
	;
	;Skip name.
	;
	movzx	eax,b[esi]
	inc	eax
	add	esi,eax
	sub	edx,eax
	;
	;Now get the value.
	;
	xor	eax,eax
	mov	ax,w[esi]
	add	esi,2
	sub	edx,2
	test	ObjPassOneType,1	;32-bit version?
	jz	@@pd5
	rol	eax,16
	mov	ax,w[esi]
	add	esi,2
	sub	edx,2
	rol	eax,16
@@pd5:	mov	PUB.PValue[ebx],eax
	;
	;Get the type.
	;
	GetIndex
	mov	PUB.PType[ebx],eax
	;
	;Set the scope.
	;
	xor	eax,eax
	mov	al,ObjPassOneType
	and	al,not 1		;lose size bit.
	cmp	al,LPUBDEF
	mov	al,-1
	jz	@@0
	xor	al,al
@@0:	mov	PUB.PScope[ebx],eax	;store scope.
	;
	jmp	@@pd2
	;
@@pd9:	inc	esi
	xor	edx,edx
	jmp	ObjPassOneNext
ObjPassOne_PUBDEF endp


;------------------------------------------------------------------------------
;
;Do EXTDEF processing.
;
ObjPassOne_EXTDEF proc near
	dec	edx		;Don't include checksum.
	;
@@ed0:	or	edx,edx
	jz	@@ed9
	;
	;See if this external already exists.
	;
	call	FindGlobalEXTDEF
	jnc	@@ed1
	call	AddGlobalEXTDEF
	mov	ErrorNumber,2
	jc	ObjPassOneError
	;
@@ed1:	;Expand the list so we can add this entry.
	;
	ExpandList Obj.MExtDefs[edi],ListBumpL
	mov	ErrorNumber,2
	jc	ObjPassOneError
	;
	;Add this entry to the list.
	;
	mov	ebx,Obj.MExtDefs[edi]
	mov	ecx,[ebx]
	inc	d[ebx]
	mov	[ebx+4+ecx*4],eax
	;
	;Set the scope.
	;
	mov	ebx,EXTDEFList
	lea	ebx,[ebx+4+eax*4]
	mov	ebx,[ebx]
	xor	eax,eax
	mov	al,ObjPassOneType
	and	al,not 1		;lose size bit.
	cmp	al,LEXTDEF
	mov	al,-1
	jz	@@0
	xor	al,al
@@0:	mov	EXT.EScope[ebx],eax	;store scope.
	mov	EXT.EObj[ebx],edi
	;
	;Skip name.
	;
	movzx	eax,b[esi]
	inc	eax
	add	esi,eax
	sub	edx,eax
	;
	;Skip the type index.
	;
	GetIndex
	;
	jmp	@@ed0
	;
@@ed9:	inc	esi
	xor	edx,edx
	jmp	ObjPassOneNext
ObjPassOne_EXTDEF endp


;------------------------------------------------------------------------------
;
;Process LNAMES list.
;
ObjPassOne_LNAMES proc near
	dec	edx		;allow for check sum.
@@l0:	call	FindGlobalLNAME	;Find this LNAME if it exists.
	jnc	@@l1
	call	AddGlobalLNAME	;Add a new LNAME.
	mov	ErrorNumber,2
	jc	ObjPassOneError
@@l1:	ExpandList Obj.MLNAMES[edi],ListBumpL
	mov	ErrorNumber,2
	jc	ObjPassOneError
	push	esi
	mov	esi,Obj.MLNAMES[edi]
	mov	ecx,[esi]
	inc	d[esi]		;update count.
	mov	[esi+4+ecx*4],eax	;store new LNAME index.
	pop	esi
	;
	movzx	eax,b[esi]		;Get length.
	inc	eax
	add	esi,eax
	sub	edx,eax
	;
	or	edx,edx		;end of the list?
	jnz	@@l0
	;
	inc	esi
	xor	edx,edx
	jmp	ObjPassOneNext
ObjPassOne_LNAMES endp


;------------------------------------------------------------------------------
;
;Process segment definition.
;
ObjPassOne_SEGDEF proc near
	cmp	PharLapFlag,0
	jz	@@s2
	mov	ObjPassOneType,SEGDEFL
@@s2:	;
	pushm	esi,edx
	add	esi,3
	cmp	ObjPassOneType,SEGDEFL
	jnz	@@s0
	add	esi,2
@@s0:	GetIndex
	popm	esi,edx
	dec	eax		;make it relative to zero.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip dword count field.
	add	eax,Obj.MLnames[edi]	;index into local list.
	mov	eax,[eax]		;Get global name index.
	;
	call	FindGlobalSEGDEF	;See if we already have this segment.
	jnc	@@s1
	mov	ErrorNumber,2
	call	AddGlobalSEGDEF	;Add a new entry, details to be filled in later.
	jc	ObjPassOneError
@@s1:	mov	edx,Obj.MSegs[edi]	;Point to SEGDEF list.
	;
	;Really aught to be checking that the new segments type matches
	;the existing segments type if this isn't first definition but
	;we don't bother for now.
	;
	push	esi
	mov	esi,edx		;Need to make space for another
	mov	ecx,[esi]		;entry.
	inc	ecx
	shl	ecx,4
	add	ecx,4		;Include entry count dword.
	call	ReMalloc
	mov	edx,esi
	pop	esi
	mov	ErrorNumber,2
	jc	ObjPassOneError
	mov	Obj.MSegs[edi],edx	;Set SEGDEF pointer again.
	mov	ebx,[edx]
	inc	d[edx]		;update number of entries.
	shl	ebx,4
	add	edx,4		;Skip entry count.
	add	edx,ebx		;Point to new entry.
	mov	OSeg.OGSeg[edx],eax	;Store global segdef index.
	mov	OSeg.OLength[edx],0	;Clear length.
	;
	;EAX	- global SEGDEF index.
	;ESI	- SEGDEF details.
	;EDI	- Module details.
	;EDX	- module SEGDEF entry.
	;EBP	- global SEGDEF entry.
	;ECX	- Module length remaining.
	;
	mov	al,[esi]
	and	al,11100000b
	shr	al,5
	mov	LSeg.SAlign[ebp],al	;Set align type.
	mov	al,[esi]
	and	al,00011100b
	shr	al,2
	mov	LSeg.SCombine[ebp],al	;Set combine type.
	mov	al,[esi]
	and	al,00000010b
	shr	al,1
	mov	LSeg.SBig[ebp],al	;Set BIG flag.
	mov	al,[esi]
	and	al,00000001b
	cmp	ObjPassOneType,SEGDEFL
	jnz	@@NoForceP
	or	al,1
@@NoForceP:	mov	LSeg.SPage[ebp],al	;Set PAGE flag.
	inc	esi
	;
	;Get base position and align it.
	;
	movzx	eax,LSeg.SAlign[ebp]	;Get align type again.
	mov	ErrorNumber,23
	or	eax,eax
	jz	@@BadAlignType
	mov	ErrorNumber,23
	cmp	eax,5+1
	jnc	@@BadAlignType
	mov	ebx,LSeg.SPosition[ebp]	;get base position.
	add	ebx,[SEGDEFAlignTable+0+eax*8]
	and	ebx,[SEGDEFAlignTable+4+eax*8]
	mov	OSeg.OBase[edx],ebx	;Set local to global base offset.
	xchg	LSeg.SPosition[ebp],ebx	;set base position.
	sub	ebx,LSeg.SPosition[ebp]
	neg	ebx
	add	LSeg.SLength[ebp],ebx	;update global segments length with align gap.
	;
	movzx	eax,w[esi]		;Get length.
	add	esi,2
	cmp	ObjPassOneType,SEGDEFL
	jnz	@@s6
	rol	eax,16
	mov	ax,w[esi]		;Get length +2
	rol	eax,16
	add	esi,2
	;
@@s6:	add	LSeg.SLength[ebp],eax
	mov	OSeg.OLength[edx],eax
	add	LSeg.SPosition[ebp],eax	;Update base position.
	;
	GetIndex
	dec	eax		;make it relative to 0.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip dword count field.
	add	eax,Obj.MLnames[edi]	;index into local list.
	mov	eax,[eax]		;Get global name index.
	mov	LSeg.SName[ebp],eax	;Store name index.
	;
	GetIndex
	dec	eax		;make it relative to zero.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip dword count field.
	add	eax,Obj.MLnames[edi]	;index into local list.
	mov	eax,[eax]		;Get global name index.
	mov	LSeg.SClass[ebp],eax	;Store class index.
	;
	pushad
	mov	edx,eax
	shl	edx,2
	add	edx,4		;Skip count dword.
	add	edx,LNAMEList
	mov	edx,[edx]		;Point to class text.
	;
	;Check for CODE
	;
	cmp	b[edx],4		;needs at least 4 characters.
	jc	@@scf0
	mov	esi,edx
	inc	esi
	mov	al,[esi]
	inc	esi
	call	UpperChar
	shl	eax,8
	mov	al,[esi]
	inc	esi
	call	UpperChar
	shl	eax,8
	mov	al,[esi]
	inc	esi
	call	UpperChar
	shl	eax,8
	mov	al,[esi]
	inc	esi
	call	UpperChar
	cmp	eax,"CODE"
	jnz	@@scf0
	or	LSeg.SFlags[ebp],1	;set code bit.
	jmp	@@scf1
	;
	;Check for STACK
	;
@@scf0:	cmp	b[edx],5		;needs at least 5 characters.
	jc	@@scf1
	mov	esi,edx
	inc	esi
	mov	al,[esi]
	inc	esi
	call	UpperChar
	shl	eax,8
	mov	al,[esi]
	inc	esi
	call	UpperChar
	shl	eax,8
	mov	al,[esi]
	inc	esi
	call	UpperChar
	shl	eax,8
	mov	al,[esi]
	inc	esi
	call	UpperChar
	cmp	eax,"STAC"
	jnz	@@scf1
	mov	al,[esi]
	call	UpperChar
	cmp	al,"K"
	jnz	@@scf1
	or	LSeg.SFlags[ebp],2	;set stack bit.
	;
@@scf1:	popad
	;
	GetIndex
	dec	eax		;make it relative to zero.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip dword count field.
	add	eax,Obj.MLnames[edi]	;index into local list.
	mov	eax,[eax]		;Get global name index.
	mov	LSeg.SOverlay[ebp],eax	;Store overlay name index.
	;
	inc	esi		;skip checksum.
	xor	edx,edx
	jmp	ObjPassOneNext
;
;Display error message about align type.
;
@@BadAlignType:
	pushad
	mov	eax,Obj.MFileName[edi]
	mov	ErrorName,eax
	mov	esi,Obj.MName[edi]
	or	esi,esi
	jz	@@brc0
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
@@brc0:	popad
	jmp	ObjPassOneError
ObjPassOne_SEGDEF endp


;------------------------------------------------------------------------------
;
;Process group definition.
;
ObjPassOne_GRPDEF proc near
	dec	edx		;Allow for checksum at the end.
	;
	GetIndex
	dec	eax		;make it relative to zero.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip dword count field.
	add	eax,Obj.MLnames[edi]	;index into local list.
	mov	eax,[eax]		;Get global name index.
	call	FindGlobalGROUP
	jnc	@@g1
	call	AddGlobalGROUP
	mov	ErrorNumber,2
	jc	ObjPassOneError
	;
@@g1:	mov	ebp,Obj.MGROUPS[edi]	;Get local GROUP pointer.
	push	esi
	mov	esi,ebp
	mov	ecx,[esi]		;Get number of entries.
	inc	ecx		;Include new entry.
	shl	ecx,2		;dword per entry.
	add	ecx,4		;include count dword.
	call	ReMalloc		;Get more memory.
	mov	ebp,esi
	pop	esi
	mov	ErrorNumber,2
	jc	ObjPassOneError
	mov	Obj.MGROUPS[edi],ebp	;Set new GROUP pointer.
	;
	push	eax
	mov	eax,[ebp]		;Get current count.
	inc	d[ebp]		;Update the count.
	add	ebp,4		;skip count dword.
	shl	eax,2		;dword per entry.
	add	ebp,eax		;point to this entry.
	pop	eax
	mov	[ebp],eax		;store GROUP list index number.
	;
@@g3:	or	edx,edx		;Done all entries?
	jz	@@g4
	movzx	ebp,w[esi]		;Get segment type/index.
	sub	edx,2
	add	esi,2
	shr	ebp,8
	dec	ebp		;make it relative to zero.
	shl	ebp,4		;16 bytes per entry.
	add	ebp,4		;skip count dword.
	add	ebp,Obj.MSegs[edi]	;index to this translation entry.
	mov	ebp,OSeg.OGSeg[ebp]	;get global SEGDEF number.
	call	AppendToGlobalGROUP	;Add this entry to the GROUP.
	mov	ErrorNumber,2
	jc	ObjPassOneError
	jmp	@@g3
	;
@@g4:	inc	esi
	xor	edx,edx
	jmp	ObjPassOneNext
ObjPassOne_GRPDEF endp


;------------------------------------------------------------------------------
;
;Deal with a MODEND record.
;
ObjPassOne_MODEND proc near
	jmp	ObjPassOneOK
ObjPassOne_MODEND endp


;------------------------------------------------------------------------------
;
;Show details of an invalid (un-recognised) object record type.
;
;On Entry:
;
;ESI	- Record
;EDI	- Obj structure.
;
;On Exit:
;
;nothing.
;
ShowInvalidObjRecType proc near
	mov	edi,offset ErrorM19_N
	mov	ecx,2
	call	Bin2Hex
	mov	ErrorNumber,19
	;
	cmp	Obj.MName[edi],0
	jz	@@9
	mov	esi,Obj.MName[edi]
	mov	edi,offset SYMSpace
	movzx	ecx,b[esi]
	inc	esi
	rep	movsb
	mov	al," "
	stosb
	xor	al,al
	stosb
@@9:	ret
ShowInvalidObjRecType endp


;------------------------------------------------------------------------------
;
;Search current PUBDEF list for this name.
;
;On Entry:
;
;ESI	- Name to find.
;
;On Exit:
;
;Carry set on error else,
;
;EAX	- Global list index.
;
;All other registers preserved.
;
FindGlobalPUBDEF proc near
	pushad
	mov	edx,PUBLICList
	mov	ebp,[edx]		;Get number of entries.
	add	edx,4
	xor	ebx,ebx		;reset index number.
@@0:	or	ebp,ebp		;Anything left?
	jz	@@9
	mov	edi,[edx]		;point to this PUBDEF
	;
	mov	LocalCaseSensitive,0
	cmp	PUBDEFScope,0
	jz	@@6
	or	LocalCaseSensitive,-1
	mov	eax,PUB.PObj[edi]
	cmp	eax,PUBDEFObj
	jnz	@@4
	;
@@6:	cmp	PUB.PScope[edi],0
	jz	@@5
	or	LocalCaseSensitive,-1
	mov	eax,PUB.PObj[edi]
	cmp	eax,PUBDEFObj
	jnz	@@4
	;
@@5:	mov	edi,PUB.PName[edi]	;point to name.
	xor	ecx,ecx
	mov	cl,[edi]		;Get the length.
	cmp	cl,[esi]		;right length?
	jnz	@@4
	push	esi
	inc	esi
	inc	edi
@@1:	mov	al,[esi]
	mov	ah,[edi]
	inc	esi
	inc	edi
	cmp	CaseSensitive,0
	jnz	@@2
	cmp	LocalCaseSensitive,0
	jnz	@@2
	call	UpperChar
	xchg	ah,al
	call	UpperChar
@@2:	cmp	al,ah
	jnz	@@3
	dec	ecx
	jnz	@@1
	;
	;We found a match.
	;
	pop	esi
	mov	[esp+28],ebx		;store index.
	clc
	jmp	@@10
	;
@@3:	pop	esi
@@4:	add	edx,4		;point to next entry.
	inc	ebx		;update index.
	dec	ebp
	jmp	@@0
	;
@@9:	stc
@@10:	popad
	ret
FindGlobalPUBDEF endp


;------------------------------------------------------------------------------
;
;Search current EXTDEF list for this name.
;
;On Entry:
;
;ESI	- Name to find.
;
;On Exit:
;
;Carry set on error else,
;
;EAX	- Global list index.
;
;All other registers preserved.
;
FindGlobalEXTDEF proc near
	pushad
	mov	edx,EXTDEFList
	mov	ebp,[edx]		;Get number of entries.
	add	edx,4
	xor	ebx,ebx		;reset index number.
@@0:	or	ebp,ebp		;Anything left?
	jz	@@9
	mov	edi,[edx]		;point to this EXTDEF.
	xor	ecx,ecx
	mov	cl,[edi]		;Get the length.
	cmp	cl,[esi]		;right length?
	jnz	@@4
	push	esi
	inc	esi
	inc	edi
@@1:	mov	al,[esi]
	mov	ah,[edi]
	inc	esi
	inc	edi
	cmp	CaseSensitive,0
	jnz	@@2
	call	UpperChar
	xchg	ah,al
	call	UpperChar
@@2:	cmp	al,ah
	jnz	@@3
	dec	ecx
	jnz	@@1
	;
	;We found a match.
	;
	pop	esi
	mov	[esp+28],ebx		;store index.
	clc
	jmp	@@10
	;
@@3:	pop	esi
@@4:	add	edx,4		;point to next entry.
	inc	ebx		;update index.
	dec	ebp
	jmp	@@0
	;
@@9:	stc
@@10:	popad
	ret
FindGlobalEXTDEF endp


;------------------------------------------------------------------------------
;
;Add an entry to the global EXTDEF list.
;
;On Entry:
;
;ESI	- Name.
;
;On Exit:
;
;Carry set on error else,
;
;EAX	- Name index.
;
;All other registers preserved.
;
AddGlobalEXTDEF proc near
	pushad
	ExpandList EXTDEFList,ListBumpG	;grow the list.
	jc	@@9
	push	esi
	mov	ecx,size EXT
	call	Malloc
	mov	edx,esi
	pop	esi
	jc	@@9
	mov	edi,edx
	mov	ecx,size EXT
	xor	al,al
	rep	stosb
	mov	edi,EXTDEFList
	mov	eax,[edi]
	inc	d[edi]
	mov	[edi+4+eax*4],edx	;store EXT pointer.
	mov	[esp+28],eax		;store index.
	;
	mov	EXT.EName[edx],esi	;store name pointer.
	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
AddGlobalEXTDEF endp


;------------------------------------------------------------------------------
;
;Find a GROUP in the global list.
;
;On Entry:
;
;EAX	- GROUP name LNAME index.
;
;On Exit:
;
;Carry set if not found else,
;
;EAX	- GROUP list index.
;
FindGlobalGROUP proc near
	pushm	ebx,ecx,edx,esi,edi,ebp
	cmp	GROUPList,0		;Got a list yet?
	jz	@@8
	mov	esi,GROUPList	;Point to the list.
	mov	ecx,[esi]		;Get number of entries.
	add	esi,4		;Skip count dword.
	or	ecx,ecx		;Any entries left?
	jz	@@8
	xor	edx,edx
@@0:	mov	edi,[esi]		;Point to GROUP entry.
	cmp	eax,[edi]		;Right LNAME index?
	jz	@@1
	add	esi,4		;Move to next entry.
	inc	edx		;Update index number.
	dec	ecx
	jnz	@@0
	jmp	@@8		;Didn't find it.
	;
@@1:	mov	eax,edx		;Get the index number.
	clc
	jmp	@@9
@@8:	stc
@@9:	popm	ebx,ecx,edx,esi,edi,ebp
	ret
FindGlobalGROUP endp


;------------------------------------------------------------------------------
;
;Add a GROUP to the global list.
;
;On Entry:
;
;EAX	- GROUP name LNAME index.
;
;On Exit:
;
;Carry set on error else,
;
;EAX	- GROUP list index.
;
AddGlobalGROUP	proc	near
	pushm	ebx,ecx,edx,esi,edi,ebp
	cmp	GROUPList,0
	jnz	@@0
	mov	ecx,4		;Get memory for the initial
	call	Malloc		;GROUP list.
	jc	@@9
	mov	d[esi],0		;Clear count dword.
	mov	GROUPList,esi	;Store GROUP pointer.
	;
@@0:	mov	ecx,8		;Initial GROUP size.
	call	Malloc
	jc	@@9
	mov	edi,esi		;Save pointer to GROUP.
	mov	d[edi],eax		;Store name index.
	mov	d[edi+4],0		;Clear segment count.
	;
	mov	esi,GROUPList	;Point to list.
	mov	ecx,[esi]		;Get current entries.
	inc	ecx		;include new entry.
	shl	ecx,2		;dword per entry.
	add	ecx,4		;include count dword.
	call	ReMalloc
	jnc	@@1
	mov	esi,edi
	call	Free		;Release GROUP memory.
	jmp	@@9
	;
@@1:	mov	GROUPList,esi	;store new list address.
	mov	eax,[esi]		;get new entry number.
	inc	d[esi]		;update count.
	add	esi,4		;skip count dword.
	mov	ebx,eax
	shl	ebx,2		;dword per entry.
	add	esi,ebx		;point to new entry.
	mov	[esi],edi		;point to GROUP definition.
	clc
	;
@@9:	popm	ebx,ecx,edx,esi,edi,ebp
	ret
AddGlobalGROUP	endp


;------------------------------------------------------------------------------
;
;Add a segment to a global GROUP definition.
;
;On Entry:
;
;EAX	- GROUP list index.
;EBP	- Value to add.
;
;On Exit:
;
;Carry set on error else,
;
;nothing.
;
AppendToGlobalGROUP proc near
	pushad
	pushad
	;
	;Check if we already have this value.
	;
	mov	edi,GROUPList
	mov	esi,[edi+4+eax*4]	;Point to this entry.
	mov	ecx,[esi+4]		;Get current entries.
	add	esi,4+4
@@0:	or	ecx,ecx
	jz	@@1
	cmp	ebp,[esi]
	jz	@@2
	add	esi,4
	dec	ecx
	jmp	@@0
@@1:	clc
	jmp	@@3
@@2:	stc
@@3:	popad
	jc	@@8
	mov	edi,GROUPList
	mov	esi,[edi+4+eax*4]	;Point to this entry.
	mov	ecx,[esi+4]		;Get current entries.
	inc	ecx
	shl	ecx,2		;dword per entry.
	add	ecx,4+4		;include header.
	call	ReMalloc
	jc	@@9
	mov	[edi+4+eax*4],esi	;store new address.
	mov	eax,[esi+4]		;Get current count.
	inc	d[esi+4]		;Update count.
	mov	[esi+4+4+eax*4],ebp	;store new value.
@@8:	clc
@@9:	popad
	ret
AppendToGlobalGROUP endp


;------------------------------------------------------------------------------
;
;Find a global SEGDEF that matches input.
;
;On Entry:
;
;EAX	- Segment name index (LNAME).
;
;On Exit:
;
;Carry set if not found else,
;
;EAX	- SEGDEF list index.
;EBP	- Pointer to SEGDEF entry.
;
FindGlobalSEGDEF proc near
	pushm	ebx,ecx,edx,esi,edi
	cmp	SEGDEFList,0		;Got any SEGDEF's yet?
	jz	@@8
	mov	esi,SEGDEFList
	mov	ecx,[esi]		;Get number of entries.
	or	ecx,ecx
	jz	@@8		;No entries yet.
	add	esi,4
	xor	edx,edx		;Reset index number.
@@0:	mov	edi,[esi]		;Point to this SEGDEF.
	cmp	eax,LSeg.SName[edi]	;Right name index?
	jz	@@1
	add	esi,4
	inc	edx
	dec	ecx
	jnz	@@0
	jmp	@@8
	;
@@1:	mov	eax,edx		;Get SEGDEF index.
	mov	ebp,edi		;Get SEGDEF pointer.
	;
	clc
	jmp	@@9
@@8:	stc
@@9:	popm	ebx,ecx,edx,esi,edi
	ret
FindGlobalSEGDEF endp


;------------------------------------------------------------------------------
;
;Add a global SEGDEF to the list.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;EAX	- SEGDEF list index.
;EBP	- Pointer to blank SEGDEF entry.
;
AddGlobalSEGDEF proc near
	pushm	ebx,ecx,edx,esi,edi
	mov	ecx,size LSeg	;Need memory for actual SEGDEF
	call	Malloc		;details.
	jc	@@8
	push	eax
	mov	edi,esi
	xor	al,al
	rep	stosb		;Clear it all to nothing.
	pop	eax
	mov	edi,esi
	;
	ExpandList SEGDEFList,ListBumpG
	jnc	@@1
	mov	esi,edi		;Have to free SEGDEF memory to
	call	Free		;keep things clean.
	jmp	@@8
	;
@@1:	mov	esi,SEGDEFList	;Get new list address.
	mov	eax,[esi]		;Get new entries index number.
	inc	d[esi]
	mov	ebp,edi		;Set pointer to LSEG.
	mov	[esi+4+eax*4],edi	;store new SEGDEF in the list.
	;
	clc
	jmp	@@9
@@8:	stc
@@9:	popm	ebx,ecx,edx,esi,edi
	ret
AddGlobalSEGDEF endp


;------------------------------------------------------------------------------
;
;Search global LNAME list for specified name.
;
;On Entry:
;
;ESI	- Name string includeing length byte.
;
;On Exit:
;
;Carry set if not found else,
;
;EAX	- LNAME list index.
;
;All other registers preserved.
;
FindGlobalLNAME proc	near
	pushm	ecx,edx,edi,ebp
	mov	ebp,LNAMEList
	mov	ecx,[ebp]		;get number of entries.
	or	ecx,ecx
	jz	@@9
	add	ebp,4
	xor	edx,edx
@@0:	mov	edi,[ebp]
	movzx	eax,b[edi]		;get length of string.
	cmp	al,[esi]		;check against target.
	jnz	@@3
	or	eax,eax		;weed out zero length names.
	jz	@@4
	pushm	eax,ecx,esi
	inc	esi		;skip length bytes.
	inc	edi
	mov	ecx,eax		;get length to compare.
	;
@@1:	;Case insensitive comparison.
	;
	mov	al,[esi]
	call	UpperChar
	mov	ah,al
	mov	al,[edi]
	call	UpperChar
	cmp	al,ah
	jnz	@@2
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@1
	;
@@2:	popm	eax,ecx,esi
	jz	@@4
	;
@@3:	add	ebp,4		;move to next entry.
	inc	edx		;update index counter.
	dec	ecx
	jnz	@@0		;keep going till we run out.
	jmp	@@9
	;
@@4:	mov	eax,edx		;get entry number.
	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popm	ecx,edx,edi,ebp
	ret
FindGlobalLNAME endp


;------------------------------------------------------------------------------
;
;Add an entry to the global LNAME list.
;
;On Entry:
;
;ESI	- Name string includeing length byte.
;
;On Exit:
;
;Carry set on error else,
;
;EAX	- LNAME list index.
;
;All other registers preserved.
;
AddGlobalLNAME	proc	near
	pushm	ecx,edi
	ExpandList LNAMEList,ListBumpG
	;
	mov	edi,LNAMEList
	mov	ecx,[edi]
	inc	d[edi]		;update entry counter.
	mov	[edi+4+ecx*4],esi	;store LNAME pointer.
	;
	mov	eax,ecx
	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popm	ecx,edi
	ret
AddGlobalLNAME	endp


;------------------------------------------------------------------------------
;
;Allocate memory for empty global lists.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
InitialGlobalAlloc proc near
	pushad
	mov	ErrorNumber,2
	xor	eax,eax
	mov	ecx,4+(ListBumpG*4)
	;
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	ObjList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	LibList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	LNAMEList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	SEGDEFList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	GROUPList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	PUBLICList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	EXTDEFList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	RelocationList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	IRelocationList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	LINEList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	LINNUMList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	EXPORTList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	IMPORTList,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	IMPORTModules,esi
	call	Malloc
	jc	@@9
	mov	d[esi],eax
	mov	IMPORTNames,esi
	;
	clc
	jmp	@@10
@@9:	stc
@@10:	popad
	ret
InitialGlobalAlloc endp


;------------------------------------------------------------------------------
;
;Find matching PUBDEF entries for all EXTDEF entries. If not already present
;then search LIB files for a module that declares it. If another module is
;used then add it to the list and call ObjPassOne to register it's symbols etc
;before carrying on with the others. New modules will always be added to the
;end of the list so we can be sure of seeing them.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
ResolveEXTDEF	proc	near
	pushad
	mov	edi,EXTDEFList
	mov	ebp,[edi]		;get number of entries.
	or	ebp,ebp
	jz	@@8
	add	edi,4
@@0:	pushm	edi,ebp
	mov	edi,[edi]
	mov	eax,EXT.EScope[edi]
	mov	EXTSearchScope,eax
	mov	eax,EXT.EObj[edi]
	mov	EXTSearchObj,eax
	mov	edi,EXT.EName[edi]	;point to the name.
;
;See if this symbol figures in the IMPORT list.
;
	cmp	EXTSearchScope,0	;must not be a local EXTERN
	jnz	@@i9		;for an IMPORT match.
	mov	esi,IMPORTList
	mov	ebp,[esi]
	add	esi,4
@@i0:	or	ebp,ebp		;end of the list?
	jz	@@i9
	pushm	esi,edi
	mov	esi,[esi]		;point to IMPORT
	mov	esi,IMPORT.IMP_IName[esi]	;point to IMPORT internal name string.
	xor	ecx,ecx
	mov	cl,[esi]
	cmp	cl,[edi]		;right length?
	jnz	@@i3
	inc	esi
	inc	edi
@@i1:	mov	al,[esi]
	mov	ah,[edi]
	cmp	CaseSensitive,0
	jnz	@@i2
	call	UpperChar
	xchg	ah,al
	call	UpperChar
	xchg	ah,al
@@i2:	cmp	al,ah
	jnz	@@i3
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@i1
;
;Matching IMPORT so update this EXTERN.
;
	popm	esi,edi
	popm	edi,ebp
	pushm	edi,ebp
	mov	edi,[edi]
	mov	eax,[esi]
	mov	EXT.EPub[edi],eax	;save pointer to IMPORT
	or	EXT.EFlags[edi],1	;mark it as an IMPORT
	jmp	@@4
;
;Move to next IMPORT entry.
;
@@i3:	popm	esi,edi
	add	esi,4
	dec	ebp
	jmp	@@i0
;
;Scan the PUBLIC list.
;
@@i9:	mov	esi,PUBLICList
	mov	ebp,[esi]
	or	ebp,ebp
	jz	@@5
	add	esi,4
@@1:	pushm	esi,edi
	mov	esi,[esi]
	;
	;Check if this is a local and if current PUBDEF is same.
	;
	mov	LocalCaseSensitive,0
	cmp	EXTSearchScope,0
	jz	@@1_1
	cmp	PUB.PScope[esi],0
	jz	@@3
	mov	eax,PUB.PObj[esi]
	cmp	eax,EXTSearchObj
	jnz	@@3
	or	LocalCaseSensitive,-1
	jmp	@@1_0
	;
@@1_1:	cmp	PUB.PScope[esi],0
	jz	@@1_0
	cmp	EXTSearchScope,0
	jz	@@3
	mov	eax,PUB.PObj[esi]
	cmp	eax,EXTSearchObj
	jnz	@@3
	or	LocalCaseSensitive,-1
	;
@@1_0:	mov	esi,PUB.PName[esi]
	xor	ecx,ecx
	mov	cl,[edi]
	cmp	cl,[esi]		;Correct length?
	jnz	@@3
	inc	edi
	inc	esi
@@2:	mov	al,[esi]
	mov	ah,[edi]
	cmp	CaseSensitive,0
	jnz	@@2_0
	cmp	LocalCaseSensitive,0
	jnz	@@2_0
	call	UpperChar
	xchg	al,ah
	call	UpperChar
@@2_0:	cmp	al,ah
	jnz	@@3
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@2
	;
	;We have a match.
	;
	popm	esi,edi
	mov	eax,esi
	popm	edi,ebp
	pushm	edi,ebp
	mov	edi,[edi]
	mov	eax,[eax]
	mov	EXT.EPub[edi],eax	;save pointer to public.
	or	PUB.PFlags[eax],1	;flag public used.
	jmp	@@4
@@3:	popm	esi,edi
	add	esi,4
	dec	ebp
	jnz	@@1
	;
@@5:	;We need to scan the LIB files to see if anything there can
	;resolve this symbol. Should be useing some fancy search
	;algorythm but we'll stick with a serial search until speed
	;becomes a problem.
	;
	popm	edi,ebp
	pushm	edi,ebp
	mov	edi,[edi]		;Point to EXTDEF entry.
	mov	edi,EXT.EName[edi]	;/
	mov	esi,LibList
	mov	ebp,[esi]		;Get number of entries.
	add	esi,4
	or	ebp,ebp		;just incase
	jz	@@l9
@@l0:	pushm	esi,ebp
	mov	esi,Lib.LAddress[esi]	;Point to LIB.
	movzx	ecx,w[esi+7]		;Get number of directory pages.
	add	esi,[esi+3]		;Get directory offset.
@@l1:	pushm	ecx,esi
	mov	ecx,37		;Maximum entries to look at.
	mov	edx,esi
@@l2:	pushm	ecx,esi
	movzx	eax,b[esi]		;Get public offset
	or	eax,eax
	jz	@@l5		;nothing to look at.
	shl	eax,1
	mov	esi,edx
	add	esi,eax		;Point to public
	xor	eax,eax
	mov	al,[esi]
	cmp	al,[edi]		;Right length?
	jnz	@@l5
	pushm	ecx,edi
	mov	ecx,eax
	inc	edi
	inc	esi
@@l3:	mov	al,[esi]
	mov	ah,[edi]
	cmp	CaseSensitive,0
	jnz	@@l3_0
	call	UpperChar
	xchg	ah,al
	call	UpperChar
@@l3_0:	cmp	al,ah
	jnz	@@l4
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@l3
	;
	;We found a match.
	;
	movzx	eax,w[esi]		;Get the page number for this module.
	popm	ecx,edi
	popm	ecx,esi
	popm	ecx,esi
	popm	esi,ebp
	mov	edx,Lib.LAddress[esi]	;Point to LIB.
	movzx	edx,w[edx+1]		;Get page length.
	add	edx,3
	mul	edx		;Get offset to this OBJ.
	add	eax,Lib.LAddress[esi]
	mov	ecx,Lib.LAddress[esi]
	add	ecx,Lib.LLength[esi]
	sub	ecx,eax		;Set maximum length.
	push	edx
	lea	edx,Lib.LName[esi]
	mov	esi,eax
	call	AddObjModule		;add this module to the list.
	pop	edx
	jc	@@l99
	mov	edi,ObjList
	mov	eax,[edi]		;get number of entries.
	dec	eax		;Don't count this entry.
	lea	edi,[edi+4+eax*4]
	mov	edi,[edi]		;point to new entry.
	mov	eax,EXTDEFList
	mov	EXTDEFList+4,eax
	mov	eax,[eax]		;Get number of entries.
	mov	EXTDEFList+8,eax
	call	ObjPassOne		;Extract PUBDEF's and EXTDEF's etc.
	jc	@@l99
	popm	edi,ebp
	sub	edi,EXTDEFList+4
	add	edi,EXTDEFList
	mov	eax,EXTDEFList
	mov	eax,[eax]		;Get number of entries now.
	sub	eax,EXTDEFList+8	;Get number of new entries.
	add	ebp,eax		;Make sure they get seen.
	jmp	@@0		;Start again for this entry.
	;
@@l4:	popm	ecx,edi
@@l5:	popm	ecx,esi
	inc	esi
	dec	ecx
	jnz	@@l2
	popm	ecx,esi
	add	esi,512
	dec	ecx
	jnz	@@l1
	popm	esi,ebp
	add	esi,size Lib
	dec	ebp
	jnz	@@l0
;
;Didn't find anything so print a warning.
;
@@l9:	mov	ErrorNumber,26
	mov	ErrorName,0
	mov	ErrorName+4,0
	pushm	ecx,esi,edi
	mov	esi,edi
	mov	edi,offset ErrorNameSpace3
	mov	ErrorName+8,edi
	movzx	ecx,b[esi]
	inc	esi
	rep	movsb
	mov	b[edi],0
	popm	ecx,esi,edi
	popm	edi,ebp
	pushm	edi,ebp
	pushad
	mov	edi,[edi]		;Point to EXTDEF entry.
	mov	esi,EXT.EObj[edi]	;/
	push	esi
	mov	esi,Obj.MFileName[esi]
	mov	ErrorName,esi
	pop	esi
	mov	esi,Obj.MName[esi]
	or	esi,esi
	jz	@@NoSName
	movzx	ecx,b[esi]
	inc	esi
	push	edi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
	pop	edi
@@NoSName:	;
	mov	EXT.EPub[edi],offset DummyPUB	;point to PUBDEF that fits.
	;
	call	PrintError
	popad
	jmp	@@4
	;
@@l99:	popm	edi,ebp
	jmp	@@9
	;
@@4:	popm	edi,ebp
	add	edi,4
	dec	ebp
	jnz	@@0
	;
@@8:	clc
	jmp	@@10
@@9:	stc
@@10:	popad
	ret
ResolveEXTDEF	endp


;------------------------------------------------------------------------------
;
;Parse the command line for object file names etc.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;All registers preserved.
;
ParseCommandLine proc near
	push	fs
	pushad
;
;Get PSP pointer so we can parse things.
;
	mov	ah,51h		;Get current PSP address so we
	int	21h		;can find the environment strings.
	mov	fs,bx
	mov	esi,80h
	movzx	ecx,fs:b[esi]	;Get line length.
	inc	esi
	mov	fs:b[esi+ecx],0	;Terminate the line.
;
;Allocate a new block of memory for the command line.
;
	pushm	ecx,esi
	inc	ecx
	add	ecx,4
	call	Malloc
	mov	edi,esi
	popm	ecx,esi
	mov	ErrorNumber,2
	jc	@@9
;
;Copy current command line into new memory.
;
	pushm	ecx,edi
	add	edi,4
	pushm	ds,fs
	pop	ds
	rep	movsb
	pop	ds
	popm	ecx,edi
;
;Set line length and point at it.
;
	mov	[edi],ecx
	mov	CommandLine,edi
	mov	esi,edi
	add	esi,4
	xor	edx,edx		;Reset entry counter.
;
;Now parse the command line.
;
@@0:	or	ecx,ecx		;Anything left to parse?
	jz	@@8
	mov	al,[esi]		;Grab a byte.
	inc	esi
	dec	ecx
	or	al,al		;End of the line?
	jz	@@8
	cmp	al," "		;space?
	jz	@@0
	cmp	al,"+"		;seperator?
	jz	@@0
	cmp	al,","		;List delimiter?
	jz	@@NextList
	cmp	al,"/"		;Option selection?
	jz	@@GetOption
	cmp	al,"@"		;Responce file?
	jz	@@Responce
	;
	;Must be a file name.
	;
	mov	edi,offset FileNameSpace
	xor	ah,ah
	jmp	@@2
@@1:	or	ecx,ecx		;End of the line?
	jz	@@3
	mov	al,[esi]		;grab a character.
	or	al,al		;End of the line?
	jz	@@3
	cmp	al,","		;List seperator?
	jz	@@3
	cmp	al," "		;name seperator?
	jz	@@3
	cmp	al,"+"		;name seperator?
	jz	@@3
	inc	esi
	dec	ecx
	cmp	al,"."		;Extension?
	jnz	@@1_0
	or	ah,-1		;Flag we have an extension.
@@1_0:	cmp	al,"\"
	jnz	@@2
	xor	ah,ah		;clear extension flag.
@@2:	mov	[edi],al
	inc	edi
	jmp	@@1
@@3:	or	ah,ah		;Need to add default extension?
	jnz	@@4
	push	esi
	lea	esi,[ExtensionList+edx*4]	;Point to current default extension.
	movsd			;Set extension.
	pop	esi
@@4:	mov	b[edi],0		;Terminate the name.
	push	esi
	mov	esi,[NamesList+edx*4]	;Point to name table.
	mov	edi,offset FileNameSpace
	call	AddFileName2List	;Add this name.
	pop	esi
	mov	ErrorNumber,2
	jc	@@9
	jmp	@@0		;Do the next bit.
	;
@@Responce:	;Read file in and insert it into the command line.
	;
	mov	b[esi-1]," "
	mov	edi,offset FileNameSpace
@@r0:	or	ecx,ecx
	jz	@@r1
	mov	al,[esi]		;Build a file name to load and
	cmp	al," "		;insert.
	jz	@@r1
	cmp	al,"+"
	jz	@@r1
	cmp	al,","
	jz	@@r1
	or	al,al
	jz	@@r1
	mov	b[esi]," "
	inc	esi
	dec	ecx
	mov	[edi],al
	inc	edi
	jmp	@@r0
@@r1:	mov	b[edi],0
	mov	ResponceCOM,esi
	mov	ResponceLEN,ecx
	mov	ResponceCNT,edx
	;
	;Load the file.
	;
	mov	edx,offset FileNameSpace	;open the file.
	mov	ErrorName,edx
	call	OpenFile
	mov	ErrorNumber,3
	jc	@@9
	xor	ecx,ecx		;move to the end of the file.
	mov	al,2
	call	SetFilePointer
	push	ecx
	xor	ecx,ecx
	mov	al,cl
	call	SetFilePointer	;back to start of file again.
	pop	ecx
	inc	ecx
	call	Malloc		;get memory to put the file in.
	mov	ErrorNumber,2
	jc	@@9
	dec	ecx
	mov	edx,esi
	call	ReadFile		;read the file in.
	mov	ErrorNumber,4
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	call	CloseFile		;close the file.
	mov	b[esi+ecx],0		;make sure it's terminated.
	mov	ErrorName,0
	;
	;Scan responce file and lose CR/LF's
	;
	push	esi
	mov	edi,esi
	xor	ah,ah
@@r2:	movsb
	cmp	b[esi-1],0		;end of the line?
	jz	@@r4
	cmp	b[esi-1],13
	jz	@@r5
	cmp	b[esi-1],10
	jz	@@r3
	cmp	b[esi-1],1ah
	jz	@@r3
	cmp	b[esi-1],"+"
	jnz	@@r6
	or	ah,-1
	jmp	@@r2
@@r3:	mov	b[edi-1]," "
	jmp	@@r2
@@r5:	mov	b[edi-1],","
	or	ah,ah
	jz	@@r2
	mov	b[edi-1]," "
	xor	ah,ah
	jmp	@@r2
@@r6:	xor	ah,ah
	jmp	@@r2
@@r4:	pop	esi
	;
	;Expand command line memory block.
	;
	push	esi
	mov	esi,CommandLine
	push	ecx
	add	ecx,[esi]
	add	ecx,4
	call	ReMalloc
	pop	ecx
	mov	edx,esi
	pop	esi
	mov	ErrorNumber,2
	jc	@@9
	mov	eax,CommandLine
	mov	CommandLine,edx
	add	d[edx],ecx
	sub	eax,ResponceCOM
	neg	eax
	add	eax,edx
	mov	ResponceCOM,eax
	;
	;Move remaining command line to make space for new command line.
	;
	pushm	ecx,esi
	mov	esi,ResponceCOM
	mov	edi,esi
	add	esi,ecx
	mov	ecx,ResponceLEN
	rep	movsb
	popm	ecx,esi
	;
	;Copy responce file into command line.
	;
	pushm	ecx,esi
	mov	edi,ResponceCOM
	rep	movsb
	popm	ecx,esi
	;
	;Release the responce file memory.
	;
	call	Free
	;
	;Now go back to processing the command line.
	;
	mov	esi,ResponceCOM
	add	ecx,ResponceLEN
	mov	edx,ResponceCNT
	jmp	@@0
	;
@@NextList:	;Move us to the next list.
	;
	inc	edx
	mov	ErrorNumber,21
	cmp	edx,4+1
	jnc	@@9
	jmp	@@0
	;
@@GetOption:	;Scan text for an option.
	;
	mov	ebp,offset OptionSwitches	;point to list of options.
@@go0:	mov	edi,[ebp]
	cmp	edi,-1
	jz	@@skipswitch
	pushm	esi,edi
	mov	edi,[edi]		;point to text.
@@go1:	mov	al,[esi]
	or	al,al
	jz	@@go2
	cmp	al," "
	jz	@@go2
	cmp	al,","
	jz	@@go2
	cmp	al,":"
	jz	@@go2
	call	UpperChar
	mov	ah,[edi]
	or	ah,ah
	jz	@@go3
	cmp	al,ah
	jnz	@@go3
	inc	esi
	inc	edi
	jmp	@@go1		;keep looking.
@@go2:	cmp	b[edi],0		;end of this string as well?
	jz	@@go4
@@go3:	popm	esi,edi
	add	ebp,4		;next entry.
	jmp	@@go0
	;
@@skipswitch:	;Command line option we don't recognise so issue a warning.
	;
	mov	ErrorNumber,24
	mov	edi,offset ErrorNameSpace1
	mov	ErrorName,edi
	pushm	ecx,esi
@@ss0:	or	ecx,ecx
	jz	@@ss1
	mov	al,[esi]
	cmp	al," "
	jz	@@ss1
	cmp	al,","
	jz	@@ss1
	mov	[edi],al
	inc	edi
	inc	esi
	dec	ecx
	jmp	@@ss0
@@ss1:	mov	b[edi],0
	popm	ecx,esi
	call	PrintError
	jmp	@@go5
	;
@@go4:	;Found the option so deal with it.
	;
	push	edx
	mov	edx,esi
	inc	edx
	mov	edi,[ebp]
	mov	esi,[edi+8]		;point to variable.
	call	d[edi+4]		;call handler.
	pop	edx
	popm	esi,edi
	jc	@@9		;oops.
	;
@@go5:	;Now step past the option text.
	;
	mov	al,[esi]
	or	al,al
	jz	@@go6
	cmp	al," "
	jz	@@go6
	cmp	al,","
	jz	@@go6
	inc	esi
	dec	ecx
	jnz	@@go5
@@go6:	jmp	@@0
	;
@@8:	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	pop	fs
	ret
ParseCommandLine endp


;------------------------------------------------------------------------------
;
;Set the value of an option.
;
;On Entry:
;
;EDX	- String to scan.
;ESI	- Variable to set.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
SetOptionValue	proc near
	pushad
	;
	mov	edi,esi
	mov	esi,edx
	xor	edx,edx
@@0:	xor	eax,eax
	mov	al,[esi]
	inc	esi
	cmp	al,0
	jz	@@1
	cmp	al," "
	jz	@@1
	cmp	al,"0"
	jc	@@9
	cmp	al,"9"+1
	jnc	@@9
	shl	edx,1
	mov	ebx,edx
	shl	edx,2
	add	edx,ebx
	sub	al,"0"
	add	edx,eax
	jmp	@@0
	;
@@1:	mov	[edi],edx
	mov	ErrorNumber,0
	clc
	jmp	@@10
	;
@@9:	mov	ErrorNumber,31
	stc
@@10:	popad
	ret
SetOptionValue	endp


;------------------------------------------------------------------------------
;
;Set an option to 0.
;
;On Entry:
;
;ESI	- Variable to set.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
SetOptionZero	proc near
	mov	d[esi],0
	clc
	ret
SetOptionZero	endp


;------------------------------------------------------------------------------
;
;Set an option to -1.
;
;On Entry:
;
;ESI	- Variable to set.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
SetOptionNoneZero proc near
	or	d[esi],-1
	clc
	ret
SetOptionNoneZero endp


;------------------------------------------------------------------------------
;
;Add a file name to the appropriate list.
;
;On Entry:
;
;ESI	- File name list to add entry to.
;EDI	- Name to add.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
AddFileName2List proc near
	pushad
	cmp	d[esi],0		;Been used yet?
	jnz	@@0
	push	esi
	mov	ecx,4
	call	Malloc		;Get initial list memory.
	mov	eax,esi
	pop	esi
	jc	@@9
	mov	[esi],eax		;Store list address.
	mov	d[eax],0		;Clear entry count.
	;
@@0:	push	esi
	mov	esi,[esi]		;Get real list address.
	mov	ecx,[esi]		;Get number of entries.
	inc	ecx		;+1 for new entry.
	shl	ecx,2		;dword per entry.
	add	ecx,4		;include count dword.
	call	ReMalloc		;Get extra entry.
	mov	ebp,esi
	pop	esi
	mov	ErrorNumber,2
	jc	@@9
	mov	[esi],ebp
	;
	pushm	esi,edi
	xor	ecx,ecx
@@1:	inc	ecx
	inc	edi
	cmp	b[edi-1],0
	jnz	@@1
	call	Malloc		;Try to get memory for the name.
	mov	eax,esi
	popm	esi,edi
	mov	ErrorNumber,2
	jc	@@9
	;
	push	esi
	mov	esi,edi
	mov	edi,eax
	rep	movsb		;Copy the name into storage.
	pop	esi
	mov	esi,[esi]
	mov	ecx,[esi]
	inc	d[esi]		;update counter.
	lea	esi,[esi+4+ecx*4]	;point to new entry.
	mov	[esi],eax		;set memory pointer.
	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
AddFileName2List endp


;------------------------------------------------------------------------------
;
;Work out which LSEG the stack should have and what the ESP value should be.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
FindStack	proc	near
	pushad
	mov	esi,SEGDEFList	;Point to segment definitions.
	mov	ebp,[esi]		;Get number of entries.
	add	esi,4
	xor	edx,edx		;Clear SS value.
@@0:	mov	edi,[esi]		;Point to this SEGDEF
	mov	eax,LSeg.SClass[edi]	;Get class LNAME index.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip count dword.
	add	eax,LNAMEList	;Point to this entry.
	mov	eax,[eax]		;Point to this LNAME.
	cmp	b[eax],5		;Long enough?
	jc	@@1
	inc	eax
	cmp	d[eax],"CATS"
	jnz	@@1
	cmp	b[eax+4],"K"
	jnz	@@1
	mov	ProgramEntrySS,edx
	mov	eax,LSeg.SLength[edi]	;Get segment length.
	mov	ProgramEntryESP,eax
	or	eax,eax
	jz	@@2		;Force a stack.
	clc
	jmp	@@9
	;
@@1:	inc	edx		;update LSEG number.
	add	esi,4		;point to next SEGDEF address.
	dec	ebp
	jnz	@@0
	;
@@2:	;Looks like we'll have to force a stack.
	;
	cmp	ForcedStack,0
	jnz	@@16Bit
;
;Find out if this is a NEAR program.
;
	mov	eax,ProgramEntryCS
	shl	eax,2
	add	eax,4
	add	eax,SEGDEFList
	mov	eax,[eax]
	mov	eax,LSeg.SClass[eax]
	shl	eax,2
	add	eax,4
	add	eax,LNAMEList
	mov	eax,[eax]
	cmp	b[eax],4
	jnz	@@NotNear0
	cmp	d[eax+1],"RAEN"
	jnz	@@NotNear0
	;
	;Extend the base segment to include a stack.
	;
	mov	edi,SEGDEFList
	mov	edi,[edi+4]		;Point to first SEGDEF
	mov	esi,LSeg.SAddress[edi]
	mov	ecx,LSeg.SILength[edi]
	add	ecx,AutoStackSize
;	call	ReMalloc
;	mov	ErrorNumber,2
;	jc	@@9
;	mov	LSeg.SAddress[edi],esi
	mov	eax,AutoStackSize
	add	LSeg.SLength[edi],eax
	add	LSeg.SILength[edi],eax
	add	LSeg.SLimit[edi],eax
	mov	ProgramEntrySS,0
	mov	ProgramEntryESP,ecx
	or	IsNEAR,-1
	jmp	@@16Bit
;
;Not a "NEAR" program so just get on with a normal search.
;
@@NotNear0:	mov	esi,SEGDEFList	;point to segment definition table.
	mov	ecx,[esi]		;get number of entries.
	inc	ecx		;allow for extra entry.
	shl	ecx,2		;dword per entry.
	add	ecx,4		;include count dword.
	call	ReMalloc
	mov	ErrorNumber,2
	jc	@@9
	mov	SEGDEFList,esi
	;
	mov	ecx,size LSeg	;get some memory for the new
	call	Malloc		;segment definition.
	jc	@@9
	mov	edi,esi
	xor	al,al
	rep	stosb
	mov	edi,esi
	;
	mov	esi,SEGDEFList
	mov	ecx,[esi]
	mov	ProgramEntrySS,ecx
	inc	d[esi]		;update number of entries.
	shl	ecx,2
	add	ecx,4
	add	esi,ecx		;point to this entry.
	mov	[esi],edi
	;
	mov	ecx,AutoStackSize
;	call	Malloc
;	jc	@@9
	mov	LSeg.SAddress[edi],0	;esi
	mov	LSeg.SLength[edi],ecx
;	mov	LSeg.SILength[edi],ecx
;	mov	LSeg.SLimit[edi],ecx
	mov	LSeg.SAlign[edi],3
	mov	ProgramEntryESP,ecx
;	push	edi
;	mov	edi,esi
;	xor	al,al
;	rep	stosb
;	pop	edi
	;
;	sub	ebp,4		;point to last segment.
;	mov	ebp,[ebp]
;	mov	eax,LSeg.SBase[ebp]
;	add	eax,LSeg.SILength[ebp]
;	mov	LSeg.SBase[edi],eax
	;
	mov	esi,offset SegClassSTACK
	call	FindGlobalLNAME
	jnc	@@3
	call	AddGlobalLNAME
	jc	@@9
@@3:	mov	LSeg.SClass[edi],eax
	mov	LSeg.SName[edi],eax
	;
	;Now set the big bit based on entry CS big bit.
	;
	mov	eax,ProgramEntryCS
	shl	eax,2
	add	eax,4
	add	eax,SEGDEFList
	mov	eax,[eax]
	cmp	LSeg.SPage[eax],0	;32-bit segment?
	jz	@@16Bit
	mov	LSeg.SPage[edi],1	;Mark it as 32-bit.
@@16Bit:	;
	clc
	;
@@9:	popad
	ret
FindStack	endp


;------------------------------------------------------------------------------
;
;Work through segments extending their limits to the end of the GROUP they're
;in if any.
;
ApplyGROUPs	proc	near
	pushad
	mov	edi,GROUPList
	mov	ecx,[edi]
	or	ecx,ecx
	jz	@@9
	mov	esi,SEGDEFList
	mov	ebp,[esi]
	or	ebp,ebp
	jz	@@9
	add	esi,4
	xor	eax,eax		;Current SEGDEF number.
@@0:	mov	edi,GROUPList
	mov	ecx,[edi]
	or	ecx,ecx
	jz	@@2
	add	edi,4
@@1:	pushm	ecx,edi
	mov	edi,[edi]		;point to GROUP details.
	mov	ecx,[edi+4]
	add	edi,4+4
	repne	scasd		;This SEGDEF exist?
	popm	ecx,edi
	jz	@@3
	add	edi,4
	dec	ecx
	jnz	@@1
@@2:	add	esi,4
	inc	eax
	dec	ebp
	jnz	@@0
	jmp	@@9
	;
@@3:	;Work out what the limit really needs to be.
	;
	mov	edi,[edi]		;point to this GROUP
	mov	ecx,[edi+4]		;get number of entries.
	add	edi,4+4
	dec	ecx
	mov	edi,[edi+ecx*4]	;get last SEGDEF number.
	shl	edi,2
	add	edi,4
	add	edi,SEGDEFList
	mov	edi,[edi]		;point to this SEGDEF
	push	esi
	mov	esi,[esi]
	mov	ecx,LSeg.SBase[edi]
	add	ecx,LSeg.SLength[edi]
	sub	ecx,LSeg.SBase[esi]
	mov	LSeg.SLimit[esi],ecx
	pop	esi
	jmp	@@2
	;
@@9:	clc
	popad
	ret
ApplyGROUPs	endp


;------------------------------------------------------------------------------
;
;Sort segments so same class segments appear together. Additional sorting is
;done to maintain WLINK (Watcom) compatibility with overall class order.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
SortSEGDEFs	proc	near
	local @@SEGDEFList:dword, @@SEGDEFXlat:dword, @@NewPos:dword, \
	@@NewXlat:dword
	pushad
	;
	;Get some memory for the new SEGDEF list.
	;
	mov	edi,SEGDEFList
	mov	ecx,[edi]		;Get number of entries.
	shl	ecx,2		;dword entries.
	add	ecx,4		;include count dword.
	call	Malloc		;get new list memory.
	mov	ErrorNumber,2
	jc	@@9
	mov	@@SEGDEFList,esi
	mov	eax,[edi]
	mov	[esi],eax
	mov	@@NewPos,0
	;
	;Get some memory for the XLAT table.
	;
	sub	ecx,4
	call	Malloc		;get translation list memory.
	mov	ErrorNumber,2
	jc	@@9
	mov	@@SEGDEFXlat,esi
	mov	@@NewXLat,esi
;
;Time to do the segment ordering.
;
	;
	;Find BEGTEXT if it exists.
	;
	mov	esi,offset SegClassCODE
	mov	edi,offset SegNameBEGTEXT
	call	FindSEGDEFClass	;Look for class of CODE
	jnc	@@1
	;
@@0:	;Now find all CODE class segments.
	;
	mov	esi,offset SegClassCODE
	xor	edi,edi
	call	FindSEGDEFClass	;Look for class of CODE
	jc	@@2
@@1:	mov	esi,SEGDEFList
	mov	ebx,[esi+4+eax*4]	;Get pointer to SEGDEF.
	mov	d[esi+4+eax*4],0	;Remove this entry.
	mov	esi,@@SEGDEFList
	mov	ecx,@@NewPos
	mov	[esi+4+ecx*4],ebx	;Store SEGDEF pointer.
	mov	esi,@@NewXlat
	mov	[esi+eax*4],ecx	;Store old 2 new translation.
	inc	@@NewPos
	jmp	@@0
	;
@@2:	;Find all BEGDATA class segments.
	;
	mov	esi,offset SegClassBEGDATA
	xor	edi,edi
	call	FindSEGDEFClass	;Look for class of BEGDATA
	jc	@@4
	mov	esi,SEGDEFList
	mov	ebx,[esi+4+eax*4]	;Get pointer to SEGDEF.
	mov	d[esi+4+eax*4],0	;Remove this entry.
	mov	esi,@@SEGDEFList
	mov	ecx,@@NewPos
	mov	[esi+4+ecx*4],ebx	;Store SEGDEF pointer.
	mov	esi,@@NewXlat
	mov	[esi+eax*4],ecx	;Store old 2 new translation.
	inc	@@NewPos
	jmp	@@2
	;
@@4:	;Find all DATA class segments.
	;
	mov	esi,offset SegClassDATA
	xor	edi,edi
	call	FindSEGDEFClass	;Look for class of DATA
	jc	@@6
	mov	esi,SEGDEFList
	mov	ebx,[esi+4+eax*4]	;Get pointer to SEGDEF.
	mov	d[esi+4+eax*4],0	;Remove this entry.
	mov	esi,@@SEGDEFList
	mov	ecx,@@NewPos
	mov	[esi+4+ecx*4],ebx	;Store SEGDEF pointer.
	mov	esi,@@NewXlat
	mov	[esi+eax*4],ecx	;Store old 2 new translation.
	inc	@@NewPos
	jmp	@@4
	;
@@6:	;Find all remaining none BSS or STACK segments.
	;
	mov	esi,offset SegClassOTHER
	xor	edi,edi
	call	FindSEGDEFClass
	jc	@@12
	mov	esi,SEGDEFList
	mov	ebx,[esi+4+eax*4]	;Get pointer to SEGDEF.
	mov	d[esi+4+eax*4],0	;Remove this entry.
	mov	esi,@@SEGDEFList
	mov	ecx,@@NewPos
	mov	[esi+4+ecx*4],ebx	;Store SEGDEF pointer.
	mov	esi,@@NewXlat
	mov	[esi+eax*4],ecx	;Store old 2 new translation.
	inc	@@NewPos
	jmp	@@6
	;
@@12:	;Find all BSS class segments.
	;
	mov	esi,offset SegClassBSS
	xor	edi,edi
	call	FindSEGDEFClass
	jc	@@14
	mov	esi,SEGDEFList
	mov	ebx,[esi+4+eax*4]	;Get pointer to SEGDEF.
	mov	d[esi+4+eax*4],0	;Remove this entry.
	mov	esi,@@SEGDEFList
	mov	ecx,@@NewPos
	mov	[esi+4+ecx*4],ebx	;Store SEGDEF pointer.
	mov	esi,@@NewXlat
	mov	[esi+eax*4],ecx	;Store old 2 new translation.
	inc	@@NewPos
	jmp	@@12
	;
@@14:	;Find all STACK class segments.
	;
	xor	edx,edx
@@14_0:	mov	esi,offset SegClassSTACK
	xor	edi,edi
	call	FindSEGDEFClass
	jc	@@16
	mov	esi,SEGDEFList
	mov	ebx,[esi+4+eax*4]	;Get pointer to SEGDEF.
	add	edx,LSEG.SLength[ebx]
	mov	d[esi+4+eax*4],0	;Remove this entry.
	mov	esi,@@SEGDEFList
	mov	ecx,@@NewPos
	mov	[esi+4+ecx*4],ebx	;Store SEGDEF pointer.
	mov	esi,@@NewXlat
	mov	[esi+eax*4],ecx	;Store old 2 new translation.
	inc	@@NewPos
	jmp	@@14_0
;
;Now check if stack needs extending.
;
@@16:	or	edx,edx
	jz	@@16_0
	sub	edx,AutoStackSize
	jns	@@16_0
	neg	edx
	mov	eax,@@NewPos
	dec	eax
	shl	eax,2
	add	eax,4
	add	eax,@@SEGDEFList
	mov	eax,[eax]
	add	LSEG.SLength[eax],edx
;
;Now work through all the OBJList entries updateing the local to global
;translation list.
;
@@16_0:	mov	ebx,@@NewXlat	;Point to translation table.
	mov	esi,OBJList
	mov	ecx,[esi]		;Get number of entries.
	add	esi,4
@@17:	mov	edi,[esi]		;Point to this OBJ entry.
	mov	edi,Obj.MSegs[edi]	;Point to local segment definitions.
	mov	edx,[edi]		;Get number of entries.
	or	edx,edx
	jz	@@19
	add	edi,4
@@18:	mov	eax,OSeg.OGSeg[edi]	;Get global segment number.
	mov	eax,[ebx+eax*4]	;Get new global segment number.
	mov	OSeg.OGSeg[edi],eax
	add	edi,size OSeg
	dec	edx
	jnz	@@18
@@19:	add	esi,4		;Next OBJ entry.
	dec	ecx
	jnz	@@17
;
;Now work through all the GROUP definitions updateing the SEGDEF numbers.
;
	mov	ebx,@@NewXlat
	mov	esi,GROUPList
	mov	ecx,[esi]		;Get number of entries.
	or	ecx,ecx
	jz	@@23
	add	esi,4
@@20:	mov	edi,[esi]		;Point to this GROUP entry.
	mov	edx,[edi+4]		;Get number of entries.
	or	edx,edx
	jz	@@22
	add	edi,4+4		;Point to details.
@@21:	mov	eax,[edi]		;Get old global segment number.
	mov	eax,[ebx+eax*4]	;Get new global segment number.
	mov	[edi],eax
	add	edi,4
	dec	edx
	jnz	@@21
@@22:	add	esi,4		;Next GROUP entry.
	dec	ecx
	jnz	@@20
;
;Release old list memory and store new pointer.
;
@@23:	mov	esi,SEGDEFList
	call	Free
	mov	eax,@@SEGDEFList
	mov	SEGDEFList,eax
	mov	esi,@@NewXlat
	call	Free
	;
@@8:	clc
	jmp	@@10
@@9:	stc
@@10:	popad
	ret
SortSEGDEFs	endp


;------------------------------------------------------------------------------
;
;Find first segment of specified name and class.
;
;On Entry:
;
;ESI	- Class string to find.
;EDI	- Name string. (0 for no name search).
;
;On Exit:
;
;Carry set if no match else,
;
;EAX	- SEGDEFList entry number of match.
;
;All other registers preserved.
;
;NOTE:
;
;The name string is searched for an exact match. If the name pointer is zero
;then the name part of the search will be ignored. The class string is
;searched at every possible position.
;
FindSEGDEFClass proc near
	pushm	ebx,ecx,edx,esi,edi,ebp
	mov	ebx,SEGDEFList
	mov	ecx,[ebx]		;Get number of entries.
	add	ebx,4
@@0:	pushm	ebx,ecx
;
;Check this entry is still in use.
;
	cmp	d[ebx],0		;Blank entry?
	jz	@@7
;
;Check if the name is right.
;
	or	edi,edi		;Any name search?
	jz	@@3
	mov	edx,[ebx]		;Point to SEGDEF entry.
	mov	edx,LSeg.SName[edx]	;Get SEGDEF LNAME index
	shl	edx,2
	add	edx,4		;Skip count dword.
	add	edx,LNAMEList
	mov	edx,[edx]		;Point to name text.
	xor	ecx,ecx
	mov	cl,[edx]
	cmp	cl,[edi]		;Correct length?
	jnz	@@7
	push	edi
	inc	edi
	inc	edx
@@1:	mov	al,[edx]
	call	UpperChar
	cmp	al,[edi]		;We got a match?
	jnz	@@2
	inc	edx
	inc	edi
	dec	ecx
	jnz	@@1
	pop	edi
	jmp	@@3		;Name is OK.
@@2:	pop	edi
	jmp	@@7
;
;Check if CLASS string is in here.
;
@@3:	mov	edx,[ebx]		;Point to SEGDEF entry.
	mov	edx,LSeg.SClass[edx]
	shl	edx,2
	add	edx,4		;Skip count dword.
	add	edx,LNAMEList
	mov	edx,[edx]		;Point to class text.
;
;If match string is "*" we have to look for, and exclude,
;BSS and STACK
;
	cmp	b[esi],1
	jnz	@@12
	cmp	b[esi+1],"*"
	jnz	@@12
	pushm	esi,edi
	mov	esi,offset SegClassBSS
	mov	edi,esi
	jmp	@@3
	;
@@13:	mov	esi,offset SegClassSTACK
	mov	edi,esi
	jmp	@@3
;
;Search this CLASS name for specified string.
;
@@12:	xor	ecx,ecx
	mov	cl,[edx]
	mov	ch,[esi]
	inc	edx
	inc	esi		;Skip length byte.
	;
@@4:	cmp	cl,ch		;Enough characters left?
	jc	@@11
	pushm	ecx,edx,esi
@@5:	mov	al,[edx]
	call	UpperChar
	cmp	al,[esi]		;Got a match?
	jnz	@@6
	inc	esi
	inc	edx
	dec	cl
	jnz	@@5
	popm	ecx,edx,esi
	;
	;Were we doing an exclusion?
	;
	cmp	edi,offset SegClassSTACK
	jz	@@14
	cmp	edi,offset SegClassBSS
	jnz	@@8		;We got a good enough match.
@@14:	popm	esi,edi
	jmp	@@7
	;
@@6:	popm	ecx,edx,esi
	inc	edx
	dec	cl
	jmp	@@4
	;
@@11:	dec	esi		;Move back to length byte.
	cmp	edi,offset SegClassBSS
	jz	@@13
	cmp	edi,offset SegClassSTACK
	jnz	@@7
	popm	esi,edi
	jmp	@@8		;This one is OK because it's not BSS and not STACK
	;
@@7:	popm	ebx,ecx
	add	ebx,4		;Next SEGDEF
	dec	ecx
	jnz	@@0
	jmp	@@9
	;
@@8:	popm	ebx,ecx
	sub	ebx,SEGDEFList
	sub	ebx,4
	shr	ebx,2
	mov	eax,ebx		;Set the entry number.
	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popm	ebx,ecx,edx,esi,edi,ebp
	ret
FindSEGDEFClass endp


;------------------------------------------------------------------------------
;
;Find last segment of specified name and class.
;
;On Entry:
;
;ESI	- Class string to find.
;EDI	- Name string. (0 for no name search).
;
;On Exit:
;
;Carry set if no match else,
;
;EAX	- SEGDEFList entry number of match.
;
;All other registers preserved.
;
;NOTE:
;
;The name string is searched for an exact match. If the name pointer is zero
;then the name part of the search will be ignored. The class string is
;searched at every possible position.
;
FindLastSEGDEFClass proc near
	pushm	ebx,ecx,edx,esi,edi,ebp
	mov	ebx,SEGDEFList
	mov	ecx,[ebx]		;Get number of entries.
	add	ebx,4
	or	ebp,-1		;reset found flag.
@@0:	pushm	ebx,ecx
;
;Check this entry is still in use.
;
	cmp	d[ebx],0		;Blank entry?
	jz	@@7
;
;Check if the name is right.
;
	or	edi,edi		;Any name search?
	jz	@@3
	mov	edx,[ebx]		;Point to SEGDEF entry.
	mov	edx,LSeg.SName[edx]	;Get SEGDEF LNAME index
	shl	edx,2
	add	edx,4		;Skip count dword.
	add	edx,LNAMEList
	mov	edx,[edx]		;Point to name text.
	xor	ecx,ecx
	mov	cl,[edx]
	cmp	cl,[edi]		;Correct length?
	jnz	@@7
	push	edi
	inc	edi
	inc	edx
@@1:	mov	al,[edx]
	call	UpperChar
	cmp	al,[edi]		;We got a match?
	jnz	@@2
	inc	edx
	inc	edi
	dec	ecx
	jnz	@@1
	pop	edi
	jmp	@@3		;Name is OK.
@@2:	pop	edi
	jmp	@@7
;
;Check if CLASS string is in here.
;
@@3:	mov	edx,[ebx]		;Point to SEGDEF entry.
	mov	edx,LSeg.SClass[edx]
	shl	edx,2
	add	edx,4		;Skip count dword.
	add	edx,LNAMEList
	mov	edx,[edx]		;Point to class text.
;
;If match string is "*" we have to look for, and exclude,
;BSS and STACK
;
	cmp	b[esi+1],"*"
	jnz	@@12
	pushm	esi,edi
	mov	esi,offset SegClassBSS
	mov	edi,esi
	jmp	@@12
	;
@@13:	mov	esi,offset SegClassSTACK
	mov	edi,esi
	jmp	@@12
;
;Search this CLASS name for specified string.
;
@@12:	xor	ecx,ecx
	mov	cl,[edx]
	mov	ch,[esi]
	inc	edx
	inc	esi		;Skip length byte.
	;
@@4:	cmp	cl,ch		;Enough characters left?
	jc	@@11
	pushm	ecx,edx,esi
@@5:	mov	al,[edx]
	call	UpperChar
	cmp	al,[esi]		;Got a match?
	jnz	@@6
	inc	esi
	inc	edx
	dec	cl
	jnz	@@5
	popm	ecx,edx,esi
	;
	;Were we doing an exclusion?
	;
	cmp	edi,offset SegClassBSS
	jz	@@14
	cmp	edi,offset SegClassSTACK
	jnz	@@8		;We got a good enough match.
@@14:	popm	esi,edi
	jmp	@@7
	;
@@6:	popm	ecx,edx,esi
	inc	edx
	dec	cl
	jmp	@@4
	;
@@11:	dec	esi		;Move back to length byte.
	cmp	edi,offset SegClassBSS
	jz	@@13
	cmp	edi,offset SegClassSTACK
	jnz	@@7
	popm	esi,edi
	;
@@7:	popm	ebx,ecx
	add	ebx,4		;Next SEGDEF
	dec	ecx
	jnz	@@0
	cmp	ebp,-1		;Find anything?
	jz	@@9
	mov	eax,ebp
	clc
	jmp	@@10
	;
@@8:	popm	ebx,ecx
	pushm	ebx,ecx
	sub	ebx,SEGDEFList
	sub	ebx,4
	shr	ebx,2
	mov	ebp,ebx		;Set the entry number.
	jmp	@@7
	;
@@9:	stc
@@10:	popm	ebx,ecx,edx,esi,edi,ebp
	ret
FindLastSEGDEFClass endp


;------------------------------------------------------------------------------
;
;Sort all the groups so that the segments listed in each group run from lowest
;to highest in terms of position within the output image.
;
SortGROUPs	proc	near
	pushad
	;
	;See if FLAT GROUP exists and if we should add one.
	;
	cmp	FlatSegments,0
	jz	@@NoForceFLAT
	mov	esi,GROUPList
	mov	ebp,[esi]		;Get number of entries.
	or	ebp,ebp
	jz	@@lf3
	add	esi,4
@@lf0:	mov	edi,[esi]		;Point to group definition.
	mov	eax,[edi]		;get LNAME number.
	shl	eax,2
	add	eax,4
	add	eax,LNAMEList
	mov	eax,[eax]		;Point to LNAME
	cmp	b[eax],4
	jnz	@@lf1
	inc	eax
	cmp	d[eax],"TALF"	;FLAT?
	jz	@@NoForceFLAT
@@lf1:	add	esi,4
	dec	ebp
	jnz	@@lf0
	;
	;No FLAT GROUP so we'll have to add one.
@@lf3:	;
	;First we need to add an LNAME of FLAT.
	;
	mov	esi,offset FLAT_LNAME
	call	AddGlobalLNAME
	mov	ErrorNumber,2
	jc	@@9
	;
	;Now create the new GROUP entry.
	;
	call	AddGlobalGROUP
	mov	ErrorNumber,2
	jc	@@9
	;
	;Define this GROUP's local to global translation in all modules.
	;
	mov	edi,OBJList
	mov	ebp,[edi]		;Get number of entries.
	add	edi,4
@@lf2:	mov	esi,[edi]		;Point to this OBJ entry.
	mov	esi,Obj.MGroups[esi]	;Point to local segment definitions.
	mov	ecx,[esi]		;get number of entries.
	inc	ecx
	shl	ecx,2
	add	ecx,4
	call	ReMalloc
	mov	ErrorNumber,2
	jc	@@9
	sub	ecx,4
	mov	[esi+ecx],eax	;Set new GROUP translation.
	inc	d[esi]		;update entry count.
	mov	ecx,esi
	mov	esi,[edi]
	mov	Obj.MGroups[esi],ecx	;Set new GROUP list address.
	add	edi,4
	dec	ebp
	jnz	@@lf2
@@NoForceFLAT:	;
	mov	esi,GROUPList
	mov	ebp,[esi]		;Get number of entries.
	or	ebp,ebp
	jz	@@8
	add	esi,4
	;
	;See if FLAT group needs filling in.
	;
	pushm	esi,ebp
@@f0:	mov	edi,[esi]		;Point to group definition.
	cmp	d[edi+4],0
	jnz	@@f9		;already has entries.
	mov	eax,[edi]		;get LNAME number.
	shl	eax,2
	add	eax,4
	add	eax,LNAMEList
	mov	eax,[eax]		;Point to LNAME
	cmp	b[eax],4
	jnz	@@f9
	inc	eax
	cmp	d[eax],"TALF"	;FLAT?
	jnz	@@f9
	;
	;Add ALL segment numbers to this GROUP.
	;
	push	esi
	mov	esi,edi
	mov	ecx,SEGDEFList
	mov	ecx,[ecx]		;Get number of segments.
	shl	ecx,2
	add	ecx,4+4
	call	ReMalloc		;Get new memory.
	mov	edi,esi
	pop	esi
	jnc	@@f1
	popm	esi,ebp
	mov	ErrorNumber,2
	jmp	@@9
@@f1:	mov	[esi],edi		;Update table entry.
	mov	ecx,SEGDEFList
	mov	ecx,[ecx]		;Get number of segments.
	mov	[edi+4],ecx
	add	edi,4+4
	xor	eax,eax
@@f2:	mov	d[edi],eax		;set the segment number.
	add	edi,4
	inc	eax		;next segment.
	dec	ecx
	jnz	@@f2
	;
@@f9:	add	esi,4
	dec	ebp
	jnz	@@f0
	popm	esi,ebp
	;
	;Now make sure the segments are arranged in the correct order.
	;
@@0:	pushm	esi,ebp
	mov	edi,[esi]		;Point to group definition.
	;
	;DGROUP present?
	;
	mov	eax,[edi]		;get LNAME number.
	shl	eax,2
	add	eax,4
	add	eax,LNAMEList
	mov	eax,[eax]		;Point to LNAME
	cmp	b[eax],6
	jnz	@@dg0
	inc	eax
	cmp	d[eax],"ORGD"	;DGROUP?
	jnz	@@dg0
	cmp	w[eax+4],"PU"
	jnz	@@dg0
	mov	AutoDSGROUP,edi
@@dg0:	;
	mov	ecx,[edi+4]		;Get number of entries.
	add	edi,4+4		;Skip to real data.
	or	ecx,ecx
	jz	@@4
	dec	ecx
	jz	@@4
@@1:	pushm	edi,ecx
	xor	ebp,ebp		;Entries moved flag.
@@2:	mov	eax,[edi]		;Get SEGDEF number.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip count entry.
	add	eax,SEGDEFList
	mov	eax,[eax]		;Point to this SEGDEF.
	mov	ebx,LSeg.SBase[eax]	;Get base offset.
	mov	eax,LSeg.SLength[eax]
	add	eax,ebx
	push	eax
	mov	ebx,[edi+4]		;Get SEGDEF number.
	shl	ebx,2		;dword per entry.
	add	ebx,4		;skip counter.
	add	ebx,SEGDEFList
	mov	ebx,[ebx]		;point to this SEGDEF.
	mov	eax,LSeg.SBase[ebx]	;get base offset.
	mov	ebx,LSeg.SLength[ebx]
	add	ebx,eax
	pop	eax
	cmp	ebx,eax
	jnc	@@3		;Leave them as they are.
	mov	eax,[edi]
	xchg	eax,[edi+4]		;Swap them round.
	mov	[edi],eax
	inc	ebp
@@3:	add	edi,4
	dec	ecx
	jnz	@@2
	popm	edi,ecx
	or	ebp,ebp		;Anything changed?
	jnz	@@1
@@4:	popm	esi,ebp
	add	esi,4
	dec	ebp
	jnz	@@0
	;
@@8:	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
SortGROUPs	endp
	

;------------------------------------------------------------------------------
;
;Work through all modules filling segments and applying fixups.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
ObjPassTwo	proc	near
	pushad
	mov	FixupEntryESP,esp
	;
	mov	edi,ObjList		;Point to list.
	mov	ebp,[edi]		;Get number of entries.
	add	edi,4		;Point to real data.
	jmp	ObjPassTwoProcessObj
ObjPassTwo	endp


;------------------------------------------------------------------------------
;
;Process next OBJ module.
;
ObjPassTwoProcessObj proc near
	pushm	edi,ebp
	mov	PharLapFlag,0
	mov	edi,[edi]		;point to this entry.
	mov	esi,Obj.Module[edi]	;point to current module.
	mov	ecx,Obj.MLength[edi]	;get length so we can check for over-run.
	mov	ObjPassTwoModule,edi
	;
	jmp	ObjPassTwoProcess
ObjPassTwoProcessObj endp


;------------------------------------------------------------------------------
;
;Process next OBJ record.
;
ObjPassTwoProcess proc near
	;
	;Check the checksum.
	;
	cmp	CHKSums,0
	jz	@@1_4
	movzx	ecx,w[esi+1]		;Get record length.
	add	ecx,3		;Include type and length.
	xor	al,al
	push	esi
@@1_3:	add	al,[esi]		;Update checksum.
	inc	esi
	dec	ecx
	jnz	@@1_3
	pop	esi
	or	al,al		;Should be zero at this point.
	jz	@@1_4
	mov	ErrorNumber,6
	jmp	ObjPassTwoError
	;
@@1_4:	;Now get on with processing this record.
	;
	xor	eax,eax
	mov	al,[esi]		;Get the record type.
	mov	FixupRecType,al
	movzx	edx,w[esi+1]
	add	esi,3
	sub	ecx,3
	jmp	[ObjPassTwoTable+eax*4]
ObjPassTwoProcess endp


;------------------------------------------------------------------------------
;
;Deal with an entry we're not really interested in.
;
ObjPassTwoNext	proc	near
	add	esi,edx
	sub	ecx,edx
	or	ecx,ecx
	jz	@@6
	jns	ObjPassTwoProcess
@@6:	mov	ErrorNumber,5
	jmp	ObjPassTwoError
ObjPassTwoNext	endp


;------------------------------------------------------------------------------
;
;Move to next OBJ record.
;
ObjPassTwoNextObj proc near
	popm	edi,ebp
	add	edi,4
	dec	ebp
	jnz	ObjPassTwoProcessObj
	jmp	ObjPassTwoExit
ObjPassTwoNextObj endp


;------------------------------------------------------------------------------
;
;Bad (un-recognised) record type found, display info and exit.
;
ObjPassTwoBad	proc	near
	call	ShowInvalidObjRecType
	jmp	ObjPassTwoError
ObjPassTwoBad	endp


;------------------------------------------------------------------------------
;
;An error of some sort occured so set carry and exit.
;
ObjPassTwoError proc near
	mov	esp,FixupEntryESP
	stc
	jmp	ObjPassTwoExit
ObjPassTwoError endp


;------------------------------------------------------------------------------
;
;Everything went OK so clear carry and exit.
;
ObjPassTwoOK	proc	near
	clc
	jmp	ObjPassTwoExit
ObjPassTwoOK	endp


;------------------------------------------------------------------------------
;
;Clean up the stack and return.
;
ObjPassTwoExit	proc	near
	popad
	ret
ObjPassTwoExit	endp


;------------------------------------------------------------------------------
;
;Get modules source file name for debug info storage.
;
ObjPassTwo_THEADR proc near
	mov	Obj.MName[edi],esi
	;
	;Now fake a LINNUM entry incase this name doesn't generate any
	;real line number info.
	;
	cmp	WantSYMs,0
	jz	@@9
	;
	pushm	ecx,esi
	mov	esi,LINNUMList
	mov	ecx,[esi]
	test	ecx,(ListBumpG/4)-1
	clc
	jnz	@@lmal0
	add	ecx,(ListBumpG/4)
	shl	ecx,4
	add	ecx,4
	call	ReMalloc
@@lmal0:	mov	ebx,esi
	popm	ecx,esi
	mov	ErrorNumber,2	;This list is double entry so
	jc	ObjPassTwoError
	mov	LINNUMList,ebx
	;
	mov	eax,[ebx]
	inc	d[ebx]
	shl	eax,4
	lea	ebx,[ebx+4+eax]	;point to new entry.
	mov	d[ebx],offset DummyLINNUM	;store pointer to this record so
	mov	eax,Obj.MName[edi]
	mov	[ebx+4],eax		;store a pointer to the name.
	mov	[ebx+8],edi		;store pointer to OBJ struc.
	xor	eax,eax
	mov	[ebx+12],eax		;we'll need it.
	;
@@9:	jmp	ObjPassTwoNext
ObjPassTwo_THEADR endp


;------------------------------------------------------------------------------
;
;Deal with a comment.
;
ObjPassTwo_COMENT proc near
	mov	al,[esi+1]		;Get the comment type.
	cmp	al,COM_PHARLAP	;32-bit default?
	jnz	@@com0
	or	PharLapFlag,-1
	jmp	ObjPassTwoNext
	;
@@com0:	mov	ebx,esi
	cmp	al,COM_DEPEND
	jz	@@com0_0
	cmp	al,COM_DEPEND2
	jnz	@@com1
	add	ebx,3
@@com0_0:	add	ebx,3
	mov	SymbolObjHead,ebx
	cmp	edx,6
	jc	@@9
	;
	;Now fake a LINNUM entry incase this name doesn't generate any
	;real line number info.
	;
	cmp	WantSYMs,0
	jz	@@9
	;
	pushm	ecx,esi
	mov	esi,LINNUMList
	mov	ecx,[esi]
	test	ecx,(ListBumpG/4)-1
	clc
	jnz	@@lmal0
	add	ecx,(ListBumpG/4)
	shl	ecx,4
	add	ecx,4
	call	ReMalloc
@@lmal0:	mov	ebx,esi
	popm	ecx,esi
	mov	ErrorNumber,2	;This list is double entry so
	jc	ObjPassTwoError
	mov	LINNUMList,ebx
	;
	mov	eax,[ebx]
	inc	d[ebx]
	shl	eax,4
	lea	ebx,[ebx+4+eax]	;point to new entry.
	mov	d[ebx],offset DummyLINNUM	;store pointer to this record so
	mov	eax,SymbolObjHead
	mov	[ebx+4],eax		;store a pointer to the name.
	mov	[ebx+8],edi		;store pointer to OBJ struc.
	xor	eax,eax
	mov	[ebx+12],eax		;we'll need it.
	;
@@9:	jmp	ObjPassTwoNext
	;
@@com1:	jmp	ObjPassTwoNext
ObjPassTwo_COMENT endp


;------------------------------------------------------------------------------
;
;Process line number information.
;
ObjPassTwo_LINNUM proc near
	cmp	WantSYMs,0
	jz	ObjPassTwoNext
	;
	pushm	ecx,esi
	mov	esi,LINNUMList
	mov	ecx,[esi]
	test	ecx,(ListBumpG/4)-1
	clc
	jnz	@@lmal0
	add	ecx,(ListBumpG/4)
	shl	ecx,4
	add	ecx,4
	call	ReMalloc
@@lmal0:	mov	ebx,esi
	popm	ecx,esi
	mov	ErrorNumber,2	;This list is double entry so
	jc	ObjPassTwoError
	mov	LINNUMList,ebx
	;
	mov	eax,[ebx]
	inc	d[ebx]
	shl	eax,4
	lea	ebx,[ebx+4+eax]	;point to new entry.
	mov	d[ebx],esi		;store pointer to this record so
	sub	d[ebx],3		;we can find it again.
	mov	eax,Obj.MName[edi]
	mov	[ebx+4],eax		;store a pointer to the name.
	mov	[ebx+8],edi		;store pointer to OBJ struc.
	xor	eax,eax
	mov	al,PharLapFlag	;store bigfields flag state cos
	mov	[ebx+12],eax		;we'll need it.
	;
	jmp	ObjPassTwoNext
ObjPassTwo_LINNUM endp


;------------------------------------------------------------------------------
;
;Deal with a fixup.
;
ObjPassTwo_FIXUPP proc near
	cmp	PharLapFlag,0
	jz	@@fixx0
	mov	al,FIXUPPL
	mov	FixupRecType,al
@@fixx0:	;
	mov	FixupType,al
	dec	edx		;Don't include checksum.
	jmp	ObjPassTwo_FIXUPP2
ObjPassTwo_FIXUPP endp


;------------------------------------------------------------------------------
;
;The main fixup loop point.
;
ObjPassTwo_FIXUPP2 proc near
@@fl0:	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
	test	al,128		;FIXUP or THREAD?
	jnz	@@NOT_THREAD
;
;This is a thread update so update a thread!
;
	push	eax
	and	eax,3		;Get the thread number.
	mov	ThreadIndex,eax
	pop	eax
	test	al,64		;Target or Frame?
	jnz	@@T_FRAME		;Frame.
	and	al,00001100b		;Get the method.
	shr	al,2
	mov	ebx,ThreadIndex
	mov	[ThreadTargetMethod+ebx],al ;store target method.
	xor	eax,eax
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
	test	al,128
	jz	@@ttm0
	and	al,127
	shl	eax,8
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
@@ttm0:	mov	ebx,ThreadIndex
	dec	eax
	mov	[ThreadTargetIndex+ebx*4],eax
	jmp	ObjPassTwo_FIXUPPNext
	;
@@T_FRAME:	and	eax,00011100b
	shr	al,2
	mov	ebx,ThreadIndex
	mov	[ThreadFrameMethod+ebx],al ;store frame method.
	xor	eax,eax
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
	test	al,128
	jz	@@ttm1
	and	al,127
	shl	eax,8
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
@@ttm1:	dec	eax
	mov	[ThreadFrameIndex+ebx*4],eax
	jmp	ObjPassTwo_FIXUPPNext
	;
@@NOT_THREAD:	;Do a FIXUP
	;

	inc	FixupNumber

	movzx	eax,al
	shl	eax,8
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
	mov	FixupBigFlags,ah
	;
	push	eax
	shr	eax,10
	and	al,7
	mov	FixupLocationType,al
	pop	eax
	and	eax,03ffh		;Isolate the offset.
	mov	FixupLocationOffset,eax
	jmp	ObjPassTwo_FIXUPPEIP
ObjPassTwo_FIXUPP2 endp


;------------------------------------------------------------------------------
;
;Additional entry point to suit entry CS:EIP processing.
;
ObjPassTwo_FIXUPPEIP proc near
	mov	FixupExternName,0
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
	mov	FixupFlags,al
	mov	FixupTargetOffset,0
	;
	mov	ah,al
	and	al,3
	mov	FixupTargetMethod,al
	shr	ah,4
	and	ah,7
	mov	FixupFrameMethod,ah
	;
	mov	al,FixupFlags
	test	al,128		;FRAME from THREAD?
	jz	@@NFT0
	and	al,30h		; mask off F bit and high bit of 3-bit frame field for thread value
	shr	al,4		; convert to relative 0
	xor	ebx,ebx
	mov	bl,al
	mov	al,[ThreadFrameMethod+ebx]
	mov	FixupFrameMethod,al
	mov	eax,[ThreadFrameIndex+ebx*4]
	mov	FixupFrameIndex,eax
	mov	al,FixupFlags
	;
@@NFT0:	test	al,8		;TARGET from THREAD?
	jz	@@NFT1
	xor	ebx,ebx
	and	al,3
	mov	bl,al
	mov	al,[ThreadTargetMethod+ebx]
	mov	FixupTargetMethod,al
	mov	eax,[ThreadTargetIndex+ebx*4]
	mov	FixupTargetIndex,eax
	mov	al,FixupFlags
@@NFT1:	;
	cmp	FixupTargetMethod,2+1
	mov	ErrorNumber,16
	jnc	@@FixupBadTarget
	cmp	FixupFrameMethod,5
	jz	@@fl13
	cmp	FixupFrameMethod,2+1
	mov	ErrorNumber,15
	jnc	@@FixupBadTarget
	;
@@fl13:	;Get the FRAME index.
	;
	test	FixupFlags,128
	jnz	@@fl2
	cmp	FixupFrameMethod,5	;get FRAME from TARGET?
	jz	@@fl2
	xor	eax,eax
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
	test	al,128
	jz	@@fl1
	and	al,127
	shl	eax,8
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
@@fl1:	dec	eax		;make it relative to zero.
	mov	FixupFrameIndex,eax
	;
@@fl2:	;Get the TARGET index.
	;
	test	FixupFlags,8
	jnz	@@fl4
	xor	eax,eax
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
	test	al,128
	jz	@@fl3
	and	al,127
	shl	eax,8
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
@@fl3:	dec	eax		;make it relative to zero.
	mov	FixupTargetIndex,eax
	;
@@fl4:	;Get the target offset if present.
	;
	test	FixupFlags,4
	jnz	@@fl5
	movzx	eax,w[esi]
	add	esi,2
	sub	edx,2
	sub	ecx,2
	cmp	FixupType,FIXUPPL
	jnz	@@fl4_0
	rol	eax,16
	mov	ax,[esi]
	rol	eax,16
	add	esi,2
	sub	edx,2
	sub	ecx,2
@@fl4_0:	mov	FixupTargetOffset,eax
	;
@@fl5:	;Now work out what the target address is.
	;
	mov	FixupTargetIMPORT,0
	cmp	FixupTargetMethod,0	;LSEG target?
	jz	@@fl7
	cmp	FixupTargetMethod,1	;GROUP target?
	jz	@@fl6
	cmp	FixupTargetMethod,2	;EXTDEF target?
	jz	@@fl30
	mov	ErrorNumber,15
	jmp	@@FixupBadTarget
	;
@@fl30:	;Target is an external so fetch its value.
	;
	mov	eax,FixupTargetIndex	;Get the name index.
	shl	eax,2		;dword entries.
	add	eax,4		;skip count dword.
	add	eax,Obj.MExtDefs[edi]
	mov	eax,[eax]		;get global index.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip counter.
	add	eax,EXTDEFList
	mov	eax,[eax]		;point to this EXTDEF.
	test	EXT.EFlags[eax],1	;IMPORT?
	mov	eax,EXT.EPub[eax]	;point to PUBDEF that fits.
	jz	@@notimp0
	or	FixupTargetIMPORT,-1
	mov	FixupTargetSEGDEF,eax
	mov	eax,IMPORT.IMP_IName[eax]
;	shl	eax,2
;	add	eax,4
;	add	eax,IMPORTNames
;	mov	eax,[eax]
	mov	FixupExternName,eax
	jmp	@@fl8
;**IMP**	;
@@notimp0:	push	eax
	mov	eax,PUB.PName[eax]
	mov	FixupExternName,eax
	pop	eax
	push	eax
	mov	eax,PUB.PValue[eax]	;get offset value.
	add	FixupTargetOffset,eax	;update target value.
	pop	eax
	or	FixupTargetSEGDEF,-1
	push	ecx
	mov	ecx,PUB.PGRP[eax]
	or	ecx,PUB.PSEG[eax]
	pop	ecx
	jz	@@fl8		;no SEGDEF to account for.
	;
	cmp	PUB.PSEG[eax],0
	jz	@@fl30_9
	;
	;It's a SEG relative extern.
	;
	push	edi
	mov	edi,PUB.PObj[eax]	;Point to OBJ.
	mov	eax,PUB.PSEG[eax]	;Must be SEG relative.
	dec	eax
	or	edi,edi
	jz	@@abs_seg0
	shl	eax,4
	add	eax,4
	add	eax,Obj.MSegs[edi]	;point to this entry.
	mov	edi,OSeg.OBase[eax]	;get local offset.
	add	FixupTargetOffset,edi
	mov	eax,OSeg.OGSeg[eax]	;get global SEGDEF number.
@@abs_seg0:	mov	FixupTargetSEGDEF,eax
	pop	edi
	jmp	@@fl8
	;
@@fl30_9:	;It's a GRP relative extern.
	;
	push	edi
	mov	edi,PUB.PObj[eax]	;Point to OBJ.
	mov	eax,PUB.PGRP[eax]	;Get group.
	dec	eax
	shl	eax,2
	add	eax,4
	add	eax,Obj.MGroups[edi]	;index to this entry.
	mov	eax,[eax]		;get global GROUP index.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip name & count dword.
	add	eax,GROUPList	;index to this group.
	mov	eax,[eax]		;point to this group list.
	add	eax,4+4		;skip name and count dwords.
	mov	eax,[eax]		;get base LSEG
	mov	FixupTargetSEGDEF,eax
	;
	;Find the target SEGDEF in the local SEGDEF list and fetch
	;the base offset.
	;
	push	ecx
	mov	edi,Obj.MSegs[edi]	;point to local SEGDEF's
	mov	ecx,[edi]		;get number of entries.
	add	edi,4
@@fl30_0:	cmp	eax,OSeg.OGSeg[edi]	;right global number?
	jz	@@fl30_1
	add	edi,size OSeg
	dec	ecx
	jnz	@@fl30_0
	jmp	@@fl30_2
@@fl30_1:	mov	edi,OSeg.OBase[edi]	;get local offset.
	add	FixupTargetOffset,edi
@@fl30_2:	pop	ecx
	pop	edi
	jmp	@@fl8
	;
@@fl6:	;Target is GROUP so translate to global GROUP number and get base
	;LSEG of the group.
	;
	mov	eax,FixupTargetIndex	;Get the GROUP index.
@@fl6_3:	shl	eax,2		;dword per entry.
	add	eax,4		;skip count dword.
	add	eax,Obj.MGroups[edi]	;index to this entry.
	mov	eax,[eax]		;get global GROUP index.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip name & count dword.
	add	eax,GROUPList	;index to this group.
	mov	eax,[eax]		;point to this group list.
	add	eax,4+4		;skip name and count dwords.
	mov	eax,[eax]		;get base LSEG
	mov	FixupTargetSEGDEF,eax
	;
	;Find the target SEGDEF in the local SEGDEF list and fetch
	;the base offset.
	;
	pushm	ecx,edi
	mov	edi,Obj.MSegs[edi]	;point to local SEGDEF's
	mov	ecx,[edi]		;get number of entries.
	add	edi,4
@@fl6_0:	cmp	eax,OSeg.OGSeg[edi]	;right global number?
	jz	@@fl6_1
	add	edi,size OSeg
	dec	ecx
	jnz	@@fl6_0
	jmp	@@fl6_2
@@fl6_1:	mov	edi,OSeg.OBase[edi]	;get local offset.
	add	FixupTargetOffset,edi
@@fl6_2:	popm	ecx,edi
	jmp	@@fl8
	;
@@fl7:	;Target is LSEG so translate to global LSEG number.
	;
	mov	eax,FixupTargetIndex	;Get the LSEG index.
@@fl7_0:	shl	eax,4		;16 bytes per entry.
	add	eax,4		;skip count dword.
	add	eax,Obj.MSegs[edi]	;point to this entry.
	push	edi
	mov	edi,OSeg.OBase[eax]	;get local offset.
	add	FixupTargetOffset,edi
	pop	edi
	mov	eax,OSeg.OGSeg[eax]	;get global SEGDEF number.
	mov	FixupTargetSEGDEF,eax
	;
@@fl8:	;Work out what the FRAME address is.
	;
	cmp	FixupFrameMethod,0	;LSEG frame?
	jz	@@fl10
	cmp	FixupFrameMethod,1	;GROUP frame?
	jz	@@fl9
	cmp	FixupFrameMethod,2	;EXTDEF frame?
	jz	@@fl40
	cmp	FixupFrameMethod,5	;TARGET frame?
	jz	@@fl14
	mov	ErrorNumber,16
	jmp	@@FixupBadTarget
	;
@@fl40:	;Frame is an external so fetch its value.
	;
	mov	eax,FixupFrameIndex	;Get the name index.
	shl	eax,2		;dword entries.
	add	eax,4		;skip count dword.
	add	eax,Obj.MExtDefs[edi]
	mov	eax,[eax]		;get global index.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip counter.
	add	eax,EXTDEFList
	mov	eax,[eax]		;point to this EXTDEF.
	test	EXT.EFlags[eax],1	;IMPORT?
	mov	eax,EXT.EPub[eax]	;point to PUBDEF that fits.
	jz	@@notimp1
	mov	FixupFrameSEGDEF,eax
	or	FixupTargetIMPORT,-1
	jmp	@@fl12
;**IMP**	;
@@notimp1:	or	FixupFrameSEGDEF,-1
	pushm	eax,ecx
	mov	ecx,PUB.PGRP[eax]
	or	ecx,PUB.PSEG[eax]
	popm	eax,ecx
	jz	@@fl11		;no SEGDEF to account for.
	;
	cmp	PUB.PSEG[eax],0
	jz	@@fl40_9
	;
	;It's a SEG relative extern.
	;
	push	edi
	mov	edi,PUB.PObj[eax]	;Point to OBJ.
	mov	eax,PUB.PSEG[eax]	;Must be SEG relative.
	dec	eax
	or	edi,edi
	jz	@@abs_seg1
	shl	eax,4
	add	eax,4
	add	eax,Obj.MSegs[edi]	;point to this entry.
	mov	eax,OSeg.OGSeg[eax]	;get global SEGDEF number.
@@abs_seg1:	mov	FixupFrameSEGDEF,eax
	pop	edi
	jmp	@@fl11
	;
@@fl40_9:	;It's a GRP relative extern.
	;
	push	edi
	mov	edi,PUB.PObj[eax]	;Point to OBJ.
	mov	eax,PUB.PGRP[eax]	;Get group.
	dec	eax
	shl	eax,2
	add	eax,4
	add	eax,Obj.MGroups[edi]	;index to this entry.
	mov	eax,[eax]		;get global GROUP index.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip name & count dword.
	add	eax,GROUPList	;index to this group.
	mov	eax,[eax]		;point to this group list.
	add	eax,4+4		;skip name and count dwords.
	mov	eax,[eax]		;get base LSEG
	mov	FixupFrameSEGDEF,eax
	pop	edi
	jmp	@@fl11
	;
@@fl9:	;Frame is GROUP so translate to global GROUP number and get base
	;LSEG of the group.
	;
	mov	eax,FixupFrameIndex	;Get the GROUP index.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip count dword.
	add	eax,Obj.MGroups[edi]	;index to this entry.
	mov	eax,[eax]		;get global GROUP index.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip count dword.
	add	eax,GROUPList	;index to this group.
	mov	eax,[eax]		;point to this group list.
	add	eax,4+4		;skip name and count dword.
	mov	eax,[eax]		;get base LSEG
	mov	FixupFrameSEGDEF,eax
	jmp	@@fl11
	;
@@fl10:	;Frame is LSEG so translate to global LSEG number.
	;
	mov	eax,FixupFrameIndex	;Get the LSEG index.
	shl	eax,4		;16 bytes per entry.
	add	eax,4		;skip count dword.
	add	eax,Obj.MSegs[edi]	;point to this entry.
	mov	eax,OSeg.OGSeg[eax]	;get global SEGDEF number.
	mov	FixupFrameSEGDEF,eax
	jmp	@@fl11
	;
@@fl14:	;Frame is same as TARGET
	;
	mov	eax,FixupTargetIndex
	mov	FixupFrameIndex,eax
	mov	eax,FixupTargetSEGDEF
	mov	FixupFrameSEGDEF,eax
	jmp	@@fl11
	;
@@fl11:	;If the FRAME and TARGET segments are different then we need
	;to adjust the target offset to reflect that.
	;
;**IMP**
	cmp	FixupTargetIMPORT,0
	jnz	@@fl12

	mov	eax,FixupFrameSEGDEF
	cmp	eax,FixupTargetSEGDEF
	jz	@@fl12
	shl	eax,2		;dword per entry.
	add	eax,4		;skip dword count.
	add	eax,SEGDEFList
	mov	eax,[eax]		;point to this SEGDEF
	mov	ebp,FixupTargetSEGDEF
	shl	ebp,2		;dword per entry.
	add	ebp,4		;skip count dword.
	add	ebp,SEGDEFList
	mov	ebp,[ebp]		;point to this SEGDEF.
	mov	eax,LSeg.SBase[eax]
	cmp	eax,LSeg.SBase[ebp]
	jc	@@fl12_0
	jz	@@fl12_0
	mov	ErrorNumber,17
	jmp	@@FixupBadTarget
	;
@@fl12_0:	sub	eax,LSeg.SBase[ebp]
	neg	eax		;Get distance from FRAME.
	add	FixupTargetOffset,eax	;Update the target offset.
	;
@@fl12:	;Now finaly we perform the fixup based on the location type and
	;if we're dealing with LEDATA or LIDATA. If we're dealing with
	;LIDATA then things get a bit more complicated and we have to
	;sit in a loop doing multiple fixups.
	;
	cmp	LastLEDATA_Type,0
	jz	@@fl15
	pushad
	;
	;Setup the LIDATA loop details.
	;
	mov	eax,FixupLocationOffset
	add	eax,LastLEDATA_Type+4
	mov	FixupLIDATAOffset,eax
	mov	FixupLocationOffset,0
	;
	xor	ebp,ebp		;STACK_COUNT=0
	mov	ebx,1		;LOOP_COUNT=1
	mov	ecx,1		;REPEAT_COUNT=1
	mov	edx,1		;BLOCK_COUNT=1
	mov	esi,LastLEDATA_TYPE+4
@@fli0:	;
	pushm	ebx,ecx,edx,esi
	inc	ebp
	mov	ebx,edx		;LOOP_COUNT=BLOCK_COUNT
@@fli5:	movzx	ecx,w[esi]
	movzx	edx,w[esi+2]
	add	esi,4
;
;Watcom LIB's seem to interpret a 32-bit version of LIDATA as only haveing the
;offset as 32-bit. The REPEAT_COUNT is still 16-bit. MASM writes the
;REPEAT_COUNT as 32-bit as well. Here we check to see which it is and get the
;extra data if needed.
;
	cmp	PharLapFlag,0	;Watcom variant?
	jnz	@@fli12
	cmp	FixupRecLIType,LIDATAL
	jnz	@@fli12
	rol	ecx,16
	mov	cx,dx
	rol	ecx,16
	movzx	edx,w[esi]
	add	esi,2
@@fli12:	mov	ErrorNumber,20
	or	ecx,ecx		;Check for REPEAT_COUNT=0
	jz	@@BadRepeatCount
	or	edx,edx		;IF BLOCK_COUNT=0
	jnz	@@fli0
	;
	mov	edx,1		;BLOCK_COUNT=1
@@fli2:	pushm	ecx,esi
	movzx	ecx,b[esi]
	inc	esi
	cmp	esi,FixupLIDATAOffset	;Want a fixup?
	jz	@@fl15
@@fli10:	add	FixupLocationOffset,ecx
	popm	ecx,esi
	dec	ecx
	jnz	@@fli2
	movzx	ecx,b[esi]
	inc	ecx
	add	esi,ecx
	;
@@fli3:	dec	ebx
	jnz	@@fli5
	;
	dec	ebp		;Anything on the stack?
	js	@@fl16
	popm	ebx,ecx,edx,eax
	dec	ecx
	jz	@@fli3
	mov	esi,eax
	jmp	@@fli0
	;
@@fl15:	;Do the actual fixups.
	;
	cmp	FixupLocationType,0	;LOBYTE
	jz	@@FixLOBYTE
	cmp	FixupLocationType,1	;OFFSET
	jz	@@FixOFFSET
	cmp	FixupLocationType,2	;BASE
	jz	@@FixBASE
	cmp	FixupLocationType,3	;POINTER
	jz	@@FixPOINTER
	cmp	FixupLocationType,4	;HIBYTE
	jz	@@FixHIBYTE
	cmp	FixupLocationType,5	;OFFSET
	jz	@@FixOFFSET
	;
	or	FixupType,1		;Force 32-bit processing.
	;
	cmp	FixupLocationType,6	;POINTER32/48, Seg16:offset32
	jz	@@FixPOINTER
	cmp	FixupLocationType,9	;offset32
	jz	@@FixOFFSET
	cmp	FixupLocationType,11	;POINTER32/48, Seg16:offset32
	jz	@@FixPOINTER
	cmp	FixupLocationType,13	;offset32
	jz	@@FixOFFSET
	;
	mov	ErrorNumber,18
	jmp	@@FixupBadTarget
	;
@@FixLOBYTE:	;Do a LOBYTE fixup.
	;
	cmp	FixupTargetImport,0
	jz	@@notimp2
;
;It's an IMPORT fixup so deal with it.
;
	or	ax,-1
	mov	es,ax

@@notimp2:	pushm	eax,ebp
	mov	ebp,LastLEDATA_SEGDEF
	mov	eax,LastLEDATA_OFFSET
	add	eax,LSeg.SAddress[ebp]
	add	eax,FixupLocationOffset
	mov	ebp,eax		;Point to the location.
	mov	eax,FixupTargetOffset	;Get offset.
	test	FixupBigFlags,64	;Self relative?
	jnz	@@FixL3
	mov	eax,LastLEDATA_OFFSET
	add	eax,FixupLocationOffset
	add	eax,1
	sub	eax,FixupTargetOffset
	neg	eax
@@FixL3:	add	[ebp],al
	or	eax,eax
	jns	@@FixL4
	neg	eax
@@FixL4:	cmp	eax,127+1		;Out of range?
	popm	eax,ebp
	jnc	@@FixupRangeWarning
	jmp	@@FixDone
	;
@@FixOFFSET:	;Do an OFFSET fixup.
	;
	cmp	FixupTargetImport,0
	jz	@@notimp3
;
;It's an IMPORT fixup so deal with it.
;
	pushad
	mov	al,0		;Fixup type = 16-bit offset
	cmp	FixupType,FIXUPPL
	jnz	@@fimp0
	inc	al		;Fixup type = 32-bit offset
@@fimp0:	test	FixupBigFlags,64	;Self relative?
	jnz	@@fimp1
	or	al,64		;mark it.
@@fimp1:	mov	ebp,LastLEDATA_SEGDEF
	mov	ebx,LastLEDATA_OFFSET
	add	ebx,FixupLocationOffset
	add	ebx,LSeg.SBase[ebp]
	mov	edi,FixupTargetSEGDEF
	mov	ecx,IMPORT.IMP_Module[edi]
	mov	edx,IMPORT.IMP_Name[edi]
	or	eax,IMPORT.IMP_Flags[edi]
	call	AddIRelocationEntry	;Add it to the IMPORT list.
	popad
	jmp	@@FixDone
;
;Not an IMPORT.
;
@@notimp3:	push	eax
	mov	eax,FixupTargetOffset	;Copy the target so repeated
	mov	FixupTargetOffset+4,eax	;FLAT LIDATA doesn't mess up.
	pop	eax
	;
	cmp	FlatSegments,0	;We want FLAT fixups?
	jz	@@FixO2
	cmp	FixupTargetSEGDEF,-1	;Absolute value?
	jz	@@FixO2
	;
	;For FLAT offset relocations we need to adjust the offset by
	;the FRAMEs offset from the start of the image.
	;
	pushm	eax,ebp
	mov	eax,FixupFrameSEGDEF
	shl	eax,2		;dword entries.
	add	eax,4		;skip dword count.
	add	eax,SEGDEFList
	mov	eax,[eax]		;point to the right SEGDEF
	cmp	LSEG.SPage[eax],0	;32-bit frame?
	clc
	jz	@@FixO30		;Frame must be 32-bit.
	cmp	NoFlatCode,0
	jz	@@nfc0
	test	LSeg.SFlags[eax],1+2	;frame musn't be code or stack.
	clc
	jnz	@@FixO30
@@nfc0:	mov	eax,LSeg.SBase[eax]	;get it's offset.
	add	FixupTargetOffset+4,eax	;update the target value.
	;
	test	FixupBigFlags,64	;Self relative?
	clc
	jz	@@FixO30
	;
	mov	ebp,LastLEDATA_SEGDEF
	mov	eax,LastLEDATA_OFFSET
	add	eax,FixupLocationOffset
	add	eax,LSeg.SBase[ebp]
	or	eax,1 shl 28
	call	AddRelocationEntry	;Add it to the list.
@@FixO30:	popm	eax,ebp
	mov	ErrorNumber,2
	jc	ObjPassTwoError
@@FixO2:	;
	pushm	eax,ebp
	mov	ebp,LastLEDATA_SEGDEF
	mov	eax,LastLEDATA_OFFSET
	add	eax,LSeg.SAddress[ebp]
	add	eax,FixupLocationOffset
	mov	ebp,eax		;Point to the location.
	mov	eax,FixupTargetOffset+4	;Get offset.
	test	FixupBigFlags,64	;Self relative?
	jnz	@@FixO3
	mov	eax,LastLEDATA_OFFSET
	add	eax,FixupLocationOffset
	add	eax,2
	cmp	FixupType,FIXUPPL
	jnz	@@FixO4
	add	eax,2
@@FixO4:	cmp	FlatSegments,0	;We want FLAT fixups?
	jz	@@FixO40
	push	eax
	mov	eax,FixupFrameSEGDEF
	shl	eax,2		;dword entries.
	add	eax,4		;skip dword count.
	add	eax,SEGDEFList
	mov	eax,[eax]		;point to the right SEGDEF
	cmp	LSEG.SPage[eax],0	;32-bit frame?
	pop	eax
	jz	@@FixO40
	cmp	NoFlatCode,0
	jz	@@nfc1
	push	eax
	mov	eax,FixupFrameSEGDEF
	shl	eax,2		;dword entries.
	add	eax,4		;skip dword count.
	add	eax,SEGDEFList
	mov	eax,[eax]		;point to the right SEGDEF
	test	LSeg.SFlags[eax],1+2	;frame musn't be code or stack.
	pop	eax
	jnz	@@FixO40
@@nfc1:	push	ebp
	mov	ebp,LastLEDATA_SEGDEF
	add	eax,LSeg.SBase[ebp]
	pop	ebp
@@FixO40:	sub	eax,FixupTargetOffset+4
	neg	eax
	;
@@FixO3:	cmp	FixupType,FIXUPPL
	jz	@@FixO0
	add	[ebp],ax
	mov	ebp,65536
	jmp	@@FixO1
	;
@@FixO0:	add	[ebp],eax
	or	ebp,-1
@@FixO1:	or	eax,eax
	jns	@@FixO5
	neg	eax
@@FixO5:	cmp	eax,ebp
	popm	eax,ebp
	jnc	@@FixupRangeWarning
	jmp	@@FixDone
	;
@@FixBASE:	;Do a BASE fixup.
	;
	cmp	FixupTargetImport,0
	jz	@@notimp4
;
;It's an IMPORT fixup so deal with it.
;
	pushad
	mov	al,4		;Fixup type = 16-bit selector
	mov	ebp,LastLEDATA_SEGDEF
	mov	ebx,LastLEDATA_OFFSET
	add	ebx,FixupLocationOffset
	add	ebx,LSeg.SBase[ebp]
	mov	edi,FixupTargetSEGDEF
	mov	ecx,IMPORT.IMP_Module[edi]
	mov	edx,IMPORT.IMP_Name[edi]
	or	eax,IMPORT.IMP_Flags[edi]
	call	AddIRelocationEntry	;Add it to the IMPORT list.
	popad
	jmp	@@FixDone
;
;Not an import fixup so deal with it normaly.
;
@@notimp4:	pushm	eax,ebp
	mov	ebp,LastLEDATA_SEGDEF
	mov	eax,LastLEDATA_OFFSET
	add	eax,LSeg.SAddress[ebp]
	add	eax,FixupLocationOffset
	mov	ebp,eax		;Point to the location.
	mov	eax,FixupTargetSEGDEF	;Get segment number.
	add	[ebp],ax		;Apply the fixup.
	;
	mov	ebp,LastLEDATA_SEGDEF
	mov	eax,LastLEDATA_OFFSET
	add	eax,FixupLocationOffset
	add	eax,LSeg.SBase[ebp]
	call	AddRelocationEntry	;Add it to the list.
	popm	eax,ebp
	mov	ErrorNumber,2
	jc	ObjPassTwoError
	jmp	@@FixDone
	;
@@FixPOINTER:	;Do a POINTER fixup.
	;
	cmp	FixupTargetImport,0
	jz	@@notimp5
;
;It's an IMPORT fixup so deal with it.
;
	pushad
	mov	al,2		;Fixup type = 16-bit pointer
	cmp	FixupType,FIXUPPL
	jnz	@@fimp2
	inc	al		;Fixup type = 32-bit pointer
@@fimp2:	test	FixupBigFlags,64	;Self relative?
	jnz	@@fimp3
	or	al,64		;mark it.
@@fimp3:	mov	ebp,LastLEDATA_SEGDEF
	mov	ebx,LastLEDATA_OFFSET
	add	ebx,FixupLocationOffset
	add	ebx,LSeg.SBase[ebp]
	mov	edi,FixupTargetSEGDEF
	mov	ecx,IMPORT.IMP_Module[edi]
	mov	edx,IMPORT.IMP_Name[edi]
	or	eax,IMPORT.IMP_Flags[edi]
	call	AddIRelocationEntry	;Add it to the IMPORT list.
	popad
	jmp	@@FixDone
;
;It's not an IMPORT so do it as normal.
;
@@notimp5:	;
	;First the OFFSET part.
	;
	push	eax
	mov	eax,FixupTargetOffset	;Copy the target so repeated
	mov	FixupTargetOffset+4,eax	;FLAT LIDATA doesn't mess up.
	pop	eax
	;
	cmp	FlatSegments,0	;We want FLAT fixups?
	jz	@@FixP2
	cmp	FixupDoingCSEIP,0	;Don't want a fixup entry for
	clc
	jnz	@@FixP2		;entry CS:EIP processing.
	;
	;
	;For FLAT offset relocations we need to adjust the offset by
	;the FRAMEs offset from the start of the image.
	;
	pushm	eax,ebp
	mov	eax,FixupFrameSEGDEF
	shl	eax,2		;dword entries.
	add	eax,4	 	;skip dword count.
	add	eax,SEGDEFList
	mov	eax,[eax]		;point to the right SEGDEF

	cmp	NoFlatCode,0
	jz	@@nfc2
	test	LSeg.SFlags[eax],1+2	;frame musn't be code or stack.
	clc
	jnz	@@FixP3
@@nfc2:
	mov	eax,LSeg.SBase[eax]	;get it's offset.
	add	FixupTargetOffset+4,eax	;update the target value.
	;
	mov	ebp,LastLEDATA_SEGDEF
	mov	eax,LastLEDATA_OFFSET
	add	eax,FixupLocationOffset
	add	eax,LSeg.SBase[ebp]
	or	eax,1 shl 28
	call	AddRelocationEntry	;Add it to the list.
@@FixP3:	popm	eax,ebp
	mov	ErrorNumber,2
	jc	ObjPassTwoError
@@FixP2:	;
	pushm	eax,ebp
	mov	ebp,LastLEDATA_SEGDEF
	mov	eax,LastLEDATA_OFFSET
	add	eax,LSeg.SAddress[ebp]
	add	eax,FixupLocationOffset
	mov	ebp,eax		;Point to the location.
	mov	eax,FixupTargetOffset+4	;Get offset.
	cmp	FixupDoingCSEIP,0
	jz	@@FixP4
	mov	ebp,offset ProgramEntryEIP
	jmp	@@FixP0
@@FixP4:	cmp	FixupType,FIXUPPL
	jz	@@FixP0
	add	[ebp],ax
	jmp	@@FixP1
@@FixP0:	add	[ebp],eax
@@FixP1:	popm	eax,ebp
	;
	;Now do the segment portion of the fixup.
	;
	pushm	eax,ebp
	mov	ebp,LastLEDATA_SEGDEF
	mov	eax,LastLEDATA_OFFSET
	add	eax,LSeg.SAddress[ebp]
	add	eax,FixupLocationOffset
	mov	ebp,eax		;Point to the location.
	mov	eax,FixupTargetSEGDEF	;Get segment number.
	cmp	FixupDoingCSEIP,0
	jz	@@FixP6
	mov	ebp,offset ProgramEntryCS
	jmp	@@FixP7
@@FixP6:	add	ebp,2		;Move to segment part of fixup.
	cmp	FixupType,FIXUPPL
	jnz	@@FixP7
	add	ebp,2
@@FixP7:	add	[ebp],ax		;Apply the fixup.
	;
	cmp	FixupDoingCSEIP,0
	clc
	jnz	@@FixP9
	mov	ebp,LastLEDATA_SEGDEF
	mov	eax,LastLEDATA_OFFSET
	add	eax,FixupLocationOffset
	add	eax,LSeg.SBase[ebp]
	add	eax,2		;Move to segment part of fixup.
	cmp	FixupType,FIXUPPL
	jnz	@@FixP8
	add	eax,2
@@FixP8:	call	AddRelocationEntry	;Add it to the list.
@@FixP9:	popm	eax,ebp
	mov	ErrorNumber,2
	jc	ObjPassTwoError
	cmp	FixupDoingCSEIP,0
	jnz	ObjPassTwo_MODEND2	;Go back to modend stuff.
	jmp	@@FIXDONE
	;
@@FixHIBYTE:	;Do a HIBYTE fixup.
	;
	cmp	FixupTargetImport,0
	jz	@@notimp6
;
;It's an IMPORT fixup so deal with it.
;
	or	ax,-1
	mov	es,ax

@@notimp6:	mov	ErrorNumber,18
	jmp	@@FixupBadTarget
;
;Print error for bad repeat count.
;
@@BadRepeatCount:
	mov	esp,FixupEntryESP
	mov	ErrorNumber,20
	pushad
	mov	edi,ObjPassTwoModule
	mov	eax,Obj.MFileName[edi]
	mov	ErrorName,eax
	mov	esi,Obj.MName[edi]
	or	esi,esi
	jz	@@brc0
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
@@brc0:	popad
	jmp	ObjPassTwoError
;
;Set details ready for fatal exit due to bad target type.
;
@@FixupBadTarget:
	pushad
	mov	eax,Obj.MFileName[edi]
	mov	ErrorName,eax
	mov	esi,Obj.MName[edi]
	or	esi,esi
	jz	@@fbt0
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
@@fbt0:	cmp	FixupExternName,0
	jz	@@fbt1
	mov	esi,FixupExternName
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace3
	mov	ErrorName+8,edi
	rep	movsb
	mov	b[edi],0
@@fbt1:	popad
	jmp	ObjPassTwoError
;
;Display a warning about the fixup range.
;
@@FixupRangeWarning:
	pushad
	mov	ErrorNumber,13
	mov	eax,Obj.MFileName[edi]
	mov	ErrorName,eax
	mov	esi,Obj.MName[edi]
	or	esi,esi
	jz	@@frw0
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
@@frw0:	cmp	FixupExternName,0
	jz	@@frw1
	mov	esi,FixupExternName
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace3
	mov	ErrorName+8,edi
	rep	movsb
	mov	b[edi],0
@@frw1:	call	PrintError
	popad
	jmp	@@FixDone
	;
@@FixDONE:	;If we're doing LIDATA fixups then we might not be finished.
	;
	cmp	LastLEDATA_Type,0
	jz	ObjPassTwo_FIXUPPNext
	jmp	@@fli10
	;
@@fl16:	popad
	jmp	ObjPassTwo_FIXUPPNext
ObjPassTwo_FIXUPPEIP endp


;------------------------------------------------------------------------------
;
;Move onto the next bit.
;
ObjPassTwo_FIXUPPNext proc near
	mov	ErrorNumber,6
	or	edx,edx
	js	ObjPassTwoError
	jnz	ObjPassTwo_FIXUPP2
	inc	esi
	dec	ecx
	xor	edx,edx
	jmp	ObjPassTwoNext
ObjPassTwo_FIXUPPNext endp


;------------------------------------------------------------------------------
;
;Process LEDATA record.
;
ObjPassTwo_LEDATA proc near
	cmp	PharLapFlag,0
	jz	@@ledx0
	mov	FixupRecType,LEDATAL
@@ledx0:	;
	mov	LastLEDATA_Type,0
	;
	xor	eax,eax
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
	test	al,128
	jz	@@le0
	and	al,127
	shl	eax,8
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
@@le0:	dec	eax		;make it zero based.
	mov	ebp,Obj.MSegs[edi]	;point to local segments.
	cmp	[ebp],eax		;Defined segment?
	jnc	@@SegNumOK
	;
	mov	ErrorNumber,9
	mov	eax,Obj.MFileName[edi]
	mov	ErrorName,eax
	pushm	ecx,esi,edi
	mov	esi,Obj.MName[edi]
	or	esi,esi
	jz	@@sno0
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
@@sno0:	popm	ecx,esi,edi
	jmp	ObjPassTwoError
	;
@@SegNumOK:	shl	eax,4		;16 bytes per entry.
	add	ebp,eax
	add	ebp,4		;skip count dword.
	;
	movzx	eax,w[esi]		;Get offset within local segment.
	add	esi,2
	sub	ecx,2
	sub	edx,2
	cmp	FixupRecType,LEDATAL
	jnz	@@le10
	rol	eax,16
	mov	ax,[esi]
	rol	eax,16
	add	esi,2
	sub	ecx,2
	sub	edx,2
@@le10:	;
	pushm	ecx,esi,edi
	mov	ecx,edx
	dec	ecx		;Don't include checksum.
	mov	edi,eax		;Get local offset.
	add	edi,OSeg.OBase[ebp]	;Get global offset.
	mov	LastLEDATA_OFFSET,edi
	mov	ebp,OSeg.OGSeg[ebp]	;Get global SEGDEF number.
	shl	ebp,2		;dword per entry.
	add	ebp,4		;skip count dword.
	add	ebp,SEGDEFList	;index into segment definition list.
	mov	ebp,[ebp]		;point to SEGDEF.
	mov	LastLEDATA_SEGDEF,ebp
	add	edi,LSeg.SAddress[ebp]	;point to target memory.
	rep	movsb		;copy the data.
	popm	ecx,esi,edi
	;
	jmp	ObjPassTwoNext
ObjPassTwo_LEDATA endp


;------------------------------------------------------------------------------
;
;Process LIDATA record.
;
ObjPassTwo_LIDATA proc near
	cmp	PharLapFlag,0
	jz	@@lidx0
	mov	al,LIDATAL
	mov	FixupRecType,al
@@lidx0:	;
	mov	FixupRecLIType,al
	mov	LastLEDATA_Type,esi
	sub	LastLEDATA_Type,3
	;
	xor	eax,eax
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
	test	al,128
	jz	@@li10
	and	al,127
	shl	eax,8
	mov	al,[esi]
	inc	esi
	dec	ecx
	dec	edx
@@li10:	dec	eax		;make it zero based.
	mov	ebp,Obj.MSegs[edi]	;point to local segments.
	cmp	[ebp],eax		;Defined segment?
	jnc	@@SegNumOK
	;
	mov	ErrorNumber,9
	mov	eax,Obj.MFileName[edi]
	mov	ErrorName,eax
	pushm	ecx,esi,edi
	mov	esi,Obj.MName[edi]
	or	esi,esi
	jz	@@sno0
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
@@sno0:	popm	ecx,esi,edi
	jmp	ObjPassTwoError
	;
@@SegNumOK:	shl	eax,4		;16 bytes per entry.
	add	ebp,eax
	add	ebp,4		;skip count dword.
	;
	movzx	eax,w[esi]		;Get offset within local segment.
	add	esi,2
	sub	ecx,2
	sub	edx,2
	;
	;Check for 32-bit offset.
	;
	cmp	FixupRecType,LIDATAL
	jnz	@@li11
	rol	eax,16
	mov	ax,[esi]
	rol	eax,16
	add	esi,2
	sub	ecx,2
	sub	edx,2
	;
@@li11:	;Now work through the data blocks doing the business.
	;
	pushm	ebx,ecx,edx,esi,edi
	mov	edi,eax		;Get local offset.
	add	edi,OSeg.OBase[ebp]	;Get global offset.
	mov	LastLEDATA_OFFSET,edi
	mov	ebp,OSeg.OGSeg[ebp]	;Get global SEGDEF number.
	shl	ebp,2		;dword per entry.
	add	ebp,4		;skip count dword.
	add	ebp,SEGDEFList	;index into segment definition list.
	mov	ebp,[ebp]		;point to SEGDEF.
	mov	LastLEDATA_SEGDEF,ebp
	mov	LastLEDATA_Type+4,esi
	add	edi,LSeg.SAddress[ebp]	;point to target memory.
	;
	xor	ebp,ebp		;STACK_COUNT=0
	mov	ebx,1		;LOOP_COUNT=1
	mov	ecx,1		;REPEAT_COUNT=1
	mov	edx,1		;BLOCK_COUNT=1
@@li0:	;
	pushm	ebx,ecx,edx,esi
	inc	ebp
	mov	ebx,edx		;LOOP_COUNT=BLOCK_COUNT
@@li5:	movzx	ecx,w[esi]
	movzx	edx,w[esi+2]
	add	esi,4
;
;Watcom LIB's seem to interpret a 32-bit version of LIDATA as only haveing the
;offset as 32-bit. The REPEAT_COUNT is still 16-bit. MASM writes the
;REPEAT_COUNT as 32-bit as well. Here we check to see which it is and get the
;extra data if needed.
;
	cmp	PharLapFlag,0	;Watcom variant?
	jnz	@@li12
	cmp	FixupRecType,LIDATAL
	jnz	@@li12
	rol	ecx,16
	mov	cx,dx
	rol	ecx,16
	movzx	edx,w[esi]
	add	esi,2
@@li12:	mov	ErrorNumber,20
	or	ecx,ecx		;Check for REPEAT_COUNT=0
	jz	@@BadRepeatCount
	or	edx,edx		;IF BLOCK_COUNT=0
	jnz	@@li0
	;
	mov	edx,1		;BLOCK_COUNT=1
@@li2:	pushm	ecx,esi
	movzx	ecx,b[esi]
	inc	esi
	rep	movsb
	popm	ecx,esi
	dec	ecx
	jnz	@@li2
	movzx	ecx,b[esi]
	inc	ecx
	add	esi,ecx
	;
@@li3:	dec	ebx
	jnz	@@li5
	;
	dec	ebp		;Anything on the stack?
	js	@@li4
	popm	ebx,ecx,edx,eax
	dec	ecx
	jz	@@li3
	mov	esi,eax
	jmp	@@li0
	;
@@li4:	popm	ebx,ecx,edx,esi,edi
	;
	jmp	ObjPassTwoNext
;
;Print error for bad repeat count.
;
@@BadRepeatCount:
	mov	esp,FixupEntryESP
	mov	ErrorNumber,20
	pushad
	mov	edi,ObjPassTwoModule
	mov	eax,Obj.MFileName[edi]
	mov	ErrorName,eax
	mov	esi,Obj.MName[edi]
	or	esi,esi
	jz	@@fbt0
	movzx	ecx,b[esi]
	inc	esi
	mov	edi,offset ErrorNameSpace2
	mov	ErrorName+4,edi
	rep	movsb
	mov	b[edi],0
@@fbt0:	popad
	jmp	ObjPassTwoError
ObjPassTwo_LIDATA endp


;------------------------------------------------------------------------------
;
;Check for an entry address.
;
ObjPassTwo_MODEND proc near
	mov	al,[esi]
	shr	al,6
	cmp	al,1
	jz	@@me0
	cmp	al,3
	jz	@@me0
	jmp	ObjPassTwo_MODEND2
	;
@@me0:	inc	esi
	or	FixupDoingCSEIP,-1
	mov	LastLEDATA_Type,0
	mov	al,FixupRecType
	and	al,1
	or	al,FIXUPP
	cmp	PharLapFlag,0
	jz	@@me1
	or	al,1
@@me1:	mov	FixupRecType,al
	mov	FixupType,al
	mov	FixupLocationType,3
	jmp	ObjPassTwo_FIXUPPEIP
ObjPassTwo_MODEND endp


;------------------------------------------------------------------------------
;
;Deal with a MODEND _after_ a fixup has been done for entry CS:EIP
;
ObjPassTwo_MODEND2 proc near
	mov	FixupDoingCSEIP,0
	jmp	ObjPassTwoNextObj
ObjPassTwo_MODEND2 endp


;------------------------------------------------------------------------------
;
;Write a MAP file if requested.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
WriteMAPFile	proc	near
	pushad
	cmp	WantMAP,0
	jz	@@8
	;
	;Create the output file.
	;
	mov	edx,MAPFiles
	mov	edx,[edx+4]
	call	CreateFile
	mov	ErrorNumber,12
	jc	@@9
	;
	;Write the program name.
	;
	mov	esi,offset MapFileProgName
	call	LenString
	mov	ecx,eax
	mov	edx,esi
	call	WriteFile
	mov	ErrorNumber,28
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	esi,EXEFiles
	mov	esi,[esi+4]
	call	UpperString
	call	LenString
	mov	ecx,eax
	mov	edx,esi
	call	WriteFile
	mov	ErrorNumber,28
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	edx,offset CarriageReturn
	mov	ecx,2
	call	WriteFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	edx,offset CarriageReturn
	mov	ecx,2
	call	WriteFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	;Write segment list header.
	;
	mov	esi,offset MapFileSegHeader
	call	LenString
	mov	ecx,eax
	mov	edx,esi
	call	WriteFile
	mov	ErrorNumber,28
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	;Work through the segments writeing the details.
	;
	mov	ebp,SEGDEFList
	mov	edi,[ebp]		;get number of entries.
	add	ebp,4
@@2:	push	edi
	mov	esi,[ebp]		;point to this segment.
	;
	;Write base address.
	;
	mov	edi,offset FileNameSpace
	mov	b[edi]," "
	inc	edi
	mov	eax,LSeg.SBase[esi]	;get base address.
	mov	ecx,8
	call	Bin2Hex
	mov	al,"H"
	stosb
	mov	al," "
	stosb
	;
	;Write length.
	;
	mov	eax,LSeg.SLength[esi]	;get length.
	mov	ecx,8
	call	Bin2Hex
	mov	al,"H"
	stosb
	mov	al," "
	stosb
	;
	;Write name.
	;
	mov	eax,LSeg.SName[esi]	;Get name LNAME index.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip count dword.
	add	eax,LNAMEList	;point to LNAME list.
	mov	eax,[eax]		;point to this LNAME.
	push	esi
	mov	esi,eax
	movzx	ecx,b[esi]
	inc	esi
	rep	movsb
	pop	esi
	movzx	ecx,b[eax]
	sub	ecx,20
	neg	ecx
	or	ecx,ecx
	jns	@@0
	xor	ecx,ecx
@@0:	mov	al," "
	rep	stosb
	;
	;Write class.
	;
	mov	eax,LSeg.SClass[esi]	;Get Class LNAME index.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip count dword.
	add	eax,LNAMEList	;point to LNAME list.
	mov	eax,[eax]		;point to this LNAME.
	push	esi
	mov	esi,eax
	movzx	ecx,b[esi]
	inc	esi
	rep	movsb
	pop	esi
	movzx	ecx,b[eax]
	sub	ecx,16
	neg	ecx
	or	ecx,ecx
	jns	@@1
	xor	ecx,ecx
@@1:	mov	al," "
	rep	stosb
	;
	;Write GROUP (if any)
	;
	mov	eax,ebp
	sub	eax,SEGDEFList
	sub	eax,4
	shr	eax,2
	mov	edx,GROUPList
	mov	ecx,[edx]		;Get number of groups.
	or	ecx,ecx
	jz	@@5
	add	edx,4
@@3:	pushm	ecx,edi
	;
	;Check if this is FLAT group. If it is we ignore it.
	;
	mov	edi,[edx]		;Point to this GROUP def.
	mov	edi,[edi]		;Get LNAME index.
	shl	edi,2
	add	edi,4
	add	edi,LNAMEList
	mov	edi,[edi]
	cmp	b[edi],4
	jnz	@@4_1
	inc	edi
	cmp	d[edi],"TALF"	;FLAT doesn't count.
	jz	@@4_0
@@4_1:	mov	edi,[edx]		;Point to this GROUP def.
	mov	ecx,[edi+4]		;Get number of entries.
	or	ecx,ecx
	jnz	@@4
@@4_0:	popm	ecx,edi
	jmp	@@5
@@4:	add	edi,4+4		;Move to real stuff.
	repne	scasd		;Find this SEGDEF?
	popm	ecx,edi
	jz	@@6
	add	edx,4		;Next group
	dec	ecx
	jnz	@@3
@@5:	xor	ecx,ecx
	jmp	@@7
@@6:	mov	edx,[edx]		;Point to matching GROUP again.
	mov	edx,[edx]		;Get LNAME index.
	shl	edx,2
	add	edx,4
	add	edx,LNAMEList
	mov	edx,[edx]		;Point to LNAME
	push	esi
	mov	esi,edx
	movzx	ecx,b[esi]
	inc	esi
	rep	movsb
	pop	esi
	movzx	ecx,b[edx]
@@7:	sub	ecx,16
	neg	ecx
	or	ecx,ecx
	jns	@@12
	xor	ecx,ecx
@@12:	mov	al," "
	rep	stosb
	;
	;Write segment number.
	;
	mov	eax,ebp
	sub	eax,SEGDEFList
	sub	eax,4
	shr	eax,2
	inc	eax		;start at 1
	mov	ecx,4
	call	Bin2Hex
	mov	al,13
	stosb
	mov	al,10
	stosb
	;
	mov	ecx,edi
	sub	ecx,offset FileNameSpace
	pop	edi
	mov	edx,offset FileNameSpace
	call	WriteFile
	mov	ErrorNumber,28
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	add	ebp,4		;next entry.
	dec	edi
	jnz	@@2
	;
@@11:	call	CloseFile
@@8:	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
WriteMAPFile	endp


;------------------------------------------------------------------------------
;
;Write out the symbols.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
WriteSYMFile	proc	near
	pushad
	cmp	WantSYMs,0
	jz	@@8
	;
	call	SortSYMs
	jc	@@9
	call	GetLineInfo		;Get line numbers if present.
	jc	System
	;
	;Create the output file.
	;
	mov	edx,SYMFiles
	mov	edx,[edx+4]
	call	CreateFile
	mov	ErrorNumber,14
	jc	@@9
	;
	;Write ID & version string.
	;
	mov	edx,offset SYMIDString
	mov	ecx,8
	call	WriteFile
	mov	ErrorNumber,27
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	;Work through the symbols writeing them out.
	;
	mov	esi,SYMList
	mov	ecx,[esi]
	or	ecx,ecx
	jz	@@7
	add	esi,4
	;
@@0:	mov	eax,[esi]
	test	PUB.PFlags[eax],2	;internal?
	jz	@@0_0
	test	PUB.PFlags[eax],1	;referanced?
	jz	@@14
	;
@@0_0:	pushm	ecx,esi
	mov	ebp,offset SYMSpace
	mov	SymbolStruc.SymbolNext[ebp],(size SymbolStruc)-1
	mov	edx,[esi]		;Point to the symbol.
	;
	;Copy the name accross.
	;
	mov	esi,PUB.PName[edx]	;Point to the name.
	movzx	ecx,b[esi]		;Get symbol length.
	add	SymbolStruc.SymbolNext[ebp],ecx
	mov	SymbolStruc.SymbolTLen[ebp],cl
	inc	esi
	lea	edi,SymbolStruc.SymbolText[ebp]
@@1:	mov	al,[esi]
	cmp	CaseSensitive,0
	jnz	@@2
	call	UpperChar
@@2:	mov	[edi],al
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@1
	;
	;Set the type.
	;
	mov	esi,PUB.PObj[edx]
	mov	SymbolStruc.SymbolType[ebp],1
	mov	SymbolStruc.SymbolDword[ebp],0
	mov	eax,PUB.PGRP[edx]	;Is this an absolute or seg?
	or	eax,PUB.PSEG[edx]
	jz	@@5
	mov	SymbolStruc.SymbolType[ebp],0
	;
	;Set the segment number.
	;
	mov	eax,PUB.PSEG[edx]
	or	eax,eax
	jz	@@3
	dec	eax
	or	esi,esi
	jz	@@3
	shl	eax,4
	add	eax,4
	add	eax,Obj.MSegs[esi]	;point to this entry.
	mov	ecx,OSeg.OBase[eax]	;get local offset.
	add	SymbolStruc.SymbolDword[ebp],ecx
	mov	eax,OSeg.OGSeg[eax]	;get global SEGDEF number.
	jmp	@@4
	;
@@3:	mov	eax,PUB.PGRP[edx]	;Get group index.
	or	eax,eax
	jz	@@4
	dec	eax
	shl	eax,2
	add	eax,4
	add	eax,Obj.MGroups[esi]	;index to this entry.
	mov	eax,[eax]		;get global GROUP index.
	shl	eax,2		;dword per entry.
	add	eax,4		;skip name & count dword.
	add	eax,GROUPList	;index to this group.
	mov	eax,[eax]		;point to this group list.
	add	eax,4+4		;skip name and count dwords.
	mov	eax,[eax]		;get base LSEG
	;
	;Find the target SEGDEF in the local SEGDEF list and fetch
	;the base offset.
	;
	mov	edi,Obj.MSegs[esi]	;point to local SEGDEF's
	mov	ecx,[edi]		;get number of entries.
	add	edi,4
@@fl30_0:	cmp	eax,OSeg.OGSeg[edi]	;right global number?
	jz	@@fl30_1
	add	edi,size OSeg
	dec	ecx
	jnz	@@fl30_0
	jmp	@@fl30_2
@@fl30_1:	mov	ecx,OSeg.OBase[edi]	;get local offset.
	add	SymbolStruc.SymbolDword[ebp],ecx
@@fl30_2:	;
@@4:	mov	SymbolStruc.SymbolSeg[ebp],ax
	;
@@5:	;Set the offset.
	;
	mov	eax,PUB.PValue[edx]
	add	SymbolStruc.SymbolDword[ebp],eax
;
;Now write this entry.
;
	popm	ecx,esi
	pushm	ecx,esi
	mov	edx,ebp
	mov	eax,ecx
	mov	ecx,SymbolStruc.SymbolNext[edx]
	call	WriteFile
	mov	edx,ecx
	popm	ecx,esi
	mov	ErrorNumber,27
	jc	@@9
	cmp	eax,edx
	jnz	@@9
	mov	eax,SYMOffset
	mov	SYMOffset+4,eax
	add	SYMOffset,edx
	;
@@14:	add	esi,4
	dec	ecx
	jnz	@@0
	;
@@7:	;Now write the line number info (if any).
	;
	mov	esi,LINEList
	mov	ebp,[esi]
	or	ebp,ebp
	jz	@@11
	add	esi,4
@@12:	mov	edx,[esi]		;Point to this entry.
	mov	ecx,SymbolStruc.SymbolNext[edx]
	call	WriteFile
	mov	ErrorNumber,27
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	eax,SYMOffset
	mov	SYMOffset+4,eax
	add	SYMOffset,ecx
	add	esi,4		;next entry.
	dec	ebp
	jnz	@@12
	;
@@11:	;Go back and set length of the last symbol to -1
	;
	mov	ecx,SYMOffset+4
	xor	al,al
	call	SetFilePointer
	or	d[SymbolStruc.SymbolNext+SYMSpace],-1
	mov	edx,offset SYMSpace
	mov	ecx,4
	call	WriteFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	call	CloseFile
@@8:	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
WriteSYMFile	endp


;------------------------------------------------------------------------------
;
;Sort symbols into alpha-numeric ordered list.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
SortSYMs	proc	near
	local @@SymbolCount:dword, @@iValue:dword, @@vValue:dword
	pushad
	;
	;Make a new list that we can safely re-arrange.
	;
	mov	esi,PUBLICList
	mov	ecx,[esi]
	shl	ecx,2
	add	ecx,4
	call	Malloc
	mov	ErrorNumber,2
	jc	@@9
	mov	SYMList,esi
	mov	edi,esi
	mov	esi,PUBLICList
	rep	movsb
	;
	mov	esi,SYMList
	mov	ecx,[esi]
	or	ecx,ecx
	jz	@@8
	mov	@@SymbolCount,ecx
	;
	;Now do the sort.
	;
	mov	eax,@@SymbolCount
	cmp	eax,1
	jbe	@@sortend		; sorted by definition
	xor	ebx,ebx
	mov	edx,ebx
	mov	bl,9
	div	ebx
	mov	ecx,eax		; ecx == quotient, N/9
;
; for (h=1;h<=N/9;h=3*h+1);
;
	mov	eax,ebx		; eax==9
	mov	al,1		; eax==1, h
	mov	bl,3		; ebx==3
	xor	edx,edx		; zero for multiply loop
@@sethloop:	;
	cmp	eax,ecx		; h<=N/9
	ja	@@sort2
	mul	ebx		; 3*h, assume 32-bit result (pretty safe bet)
	inc	eax		; 3*h+1
	jmp	@@sethloop
;
; ebx will play role of j, edx will play role of h
;
@@sort2:	mov	edx,eax		; edx == h
;
; for (;h>0;...
;
@@hloop:	or	edx,edx		; h>0
	je	@@sortend
;
; for(i=h+1...
;
	mov	eax,edx
	inc	eax
	mov	@@iValue,eax
;
; for(...;i<=N;...){
;
@@iloop:	mov	eax,@@iValue
	cmp	eax,@@SymbolCount
	ja	@@nexth
	;
	mov	ecx,SYMList
	mov	ecx,[ecx+eax*4]
	mov	@@vValue,ecx		; v=a[i]
	mov	ebx,eax		; j=i
;
; while(j>h && a[j-h]>v){
;
@@whileloop:	cmp	ebx,edx		; j>h
	jbe	@@whilefail
	;
	mov	eax,ebx
	sub	eax,edx		; eax==j-h
	mov	esi,SYMList
	mov	esi,[esi+eax*4]	; esi==a[j-h]
	mov	edi,@@vValue		; edi==v
	xor	ecx,ecx		; zero high bytes of register for following repe
;
; a[j-h] > v
;
	pushm	esi,edi
	mov	esi,PUB.PName[esi]
	mov	edi,PUB.PName[edi]
	mov	cl,[esi]		; get length of first name
	mov	ah,cl
	mov	al,[edi]		; get length of second name
	cmpsb			; compare name lengths
	je	@@namechk		; same
	cmp	al,ah		; see if second name shorter than first
	ja	@@namechk		; no, (equals does swap, faster than branch)
	mov	cl,al		; update compare length to shortest name
	;
@@namechk:	push	eax
	mov	al,[esi]
	mov	ah,[edi]
	call	UpperChar
	xchg	ah,al
	call	UpperChar
	cmp	ah,al
	pop	eax
	jnz	@@nchk
	inc	esi
	inc	edi
	dec	ecx
	jnz	@@namechk
@@nchk:	popm	esi,edi
	jb	@@whilefail		; first < second, a[j-h]<v
	jne	@@dochange		; second > first, modify a[j]
;
; matched through length of name
;
	cmp	al,ah		; see if second name shorter than first
	jae	@@whilefail		; no, a[j-h]<=v
@@dochange:	;
	mov	eax,ebx
	sub	eax,edx		; eax==j-h
	lea	eax,[eax*4]
	add	eax,SYMList
	mov	eax,[eax]		; eax==a[j-h]
	push	ebx
	lea	ebx,[ebx*4]
	add	ebx,SYMList
	mov	[ebx],eax		; a[j]=a[j-h]
	pop	ebx
	sub	ebx,edx		; j-=h
	jmp	@@whileloop
@@whilefail:	;
	mov	eax,@@vValue
	push	ebx
	lea	ebx,[ebx*4]
	add	ebx,SYMList
	mov	[ebx],eax		; a[j]=v
	pop	ebx
;
; for(...;i++){
;
	inc	@@iValue
	jmp	@@iloop
;
; for (...;h/=3){
;
@@nexth:	mov	eax,edx
	xor	edx,edx
	mov	ecx,edx
	mov	cl,3
	div	ecx
	mov	edx,eax
	jmp	@@hloop
@@sortend:	;
@@8:	clc
	jmp	@@10
@@9:	stc
@@10:	popad
	ret
SortSYMs	endp


;------------------------------------------------------------------------------
;
;Scan OBJ files for line number info.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
GetLineInfo	proc	near
	pushad
	mov	edi,LINNUMList	;Point to list.
	mov	ebp,[edi]		;Get number of entries.
	add	edi,4		;Point to real data.
	or	ebp,ebp
	jz	@@8
	;
@@0:	pushm	edi,ebp
	mov	eax,[edi+12]		;get the flags.
	mov	PharLapFlag,al
	mov	eax,[edi+4]		;get the name address.
	mov	esi,[edi]		;point to LINNUM record.
	mov	edi,[edi+8]		;point to defining OBJ struc.
	mov	Obj.MName[edi],eax	;set name for this section.
	;
	;Now we can process this record.
	;
	mov	edx,1
	cmp	esi,offset DummyLINNUM
	jz	@@LINNUM
	;
	xor	eax,eax
	mov	al,[esi]		;Get the record type.
	mov	ObjPassOneType,al
	movzx	edx,w[esi+1]
	add	esi,3
	;
@@LINNUM:	;Process line number information.
	;
	dec	edx		;don't include checksum.
	;
	cmp	PharLapFlag,0
	jz	@@pdx0
	mov	ObjPassOneType,LINNUML
;
;See if we've already got an entry for this file.
;
@@pdx0:	mov	SymbolObjPos,esi
	mov	SymbolObjHead,edi
	pushm	ecx,esi,edi
	mov	esi,Obj.MName[edi]	;Get name pointer.
	mov	bl,[esi]		;Get name length.
	inc	esi
	mov	edi,LINEList
	mov	ecx,[edi]
	or	ecx,ecx
	jz	@@ff3
	add	edi,4


@@ff0:	pushm	esi,edi
	mov	edi,[edi]		;Point to this entry.
	cmp	bl,SymbolStruc.SymbolTLen[edi] ;right length?
	jnz	@@ff2
	mov	bh,bl
	lea	edi,SymbolStruc.SymbolText[edi]
@@ff1:	mov	al,[esi]
	mov	ah,[edi]
	inc	esi
	inc	edi
	call	UpperChar
	xchg	ah,al
	call	UpperChar
	cmp	al,ah
	jnz	@@ff2
	dec	bh
	jnz	@@ff1
	;
	;Check if it has the right segment index.
	;
	popm	esi,edi
	pushm	esi,edi
	mov	esi,[edi]
	;
;	cmp	SymbolStruc.SymbolSeg[esi],0
;	jz	@@ThisOne
	;
	pushm	edx,esi,edi
	mov	esi,SymbolObjPos
	cmp	esi,offset DummyLINNUM
	jz	@@NotNEAR0
	mov	edi,SymbolObjHead
	GetIndex		;Think this is the GROUP index.
	GetIndex		;Think this is the SEGDEF index.
	dec	eax
	shl	eax,4
	add	eax,4
	add	eax,Obj.MSegs[edi]
	mov	eax,OSeg.OGSeg[eax]	;get global segdef index.
	cmp	IsNEAR,0
	jz	@@NotNEAR0
	inc	eax
@@NotNEAR0:	cmp	esi,offset DummyLINNUM
	popm	edx,esi,edi
	jz	@@ThisOne
	cmp	ax,SymbolStruc.SymbolSeg[esi]
	jnz	@@ff2
@@ThisOne:	popm	esi,edi
	;
	;Found an existing entry so set things up to use that instead.
	;
	push	edx
	mov	eax,edx
	mov	ecx,4		;LINNUM length.
	cmp	ObjPassOneType,LINNUML	;Big offsets?
	jnz	@@ff4
	mov	ecx,6
@@ff4:	xor	edx,edx
	div	ecx		;Get number of entries.
	shl	eax,3		;8 bytes per entry.
	mov	ecx,eax
	pop	edx
	;
	;Add in length of existing data.
	;
	mov	esi,[edi]
	add	ecx,SymbolStruc.SymbolNext[esi]
	call	ReMalloc
	mov	ErrorNumber,2
	jc	System
	mov	[edi],esi		;set new address.
	mov	SymbolStruc.SymbolNext[esi],ecx
	mov	LINNUMSymbolAddress,esi
	;
	pushm	edx,esi,edi
	xor	eax,eax
	mov	esi,SymbolObjPos
	cmp	esi,offset DummyLINNUM
	jz	@@ThisOne0
	mov	edi,SymbolObjHead
	GetIndex		;Think this is the GROUP index.
	GetIndex		;Think this is the SEGDEF index.
	;
	dec	eax
	shl	eax,4
	add	eax,4
	add	eax,Obj.MSegs[edi]
	push	eax
	mov	eax,OSeg.OGSeg[eax]	;get global segdef index.
	cmp	IsNEAR,0
	jz	@@NotNEAR10
	inc	eax
@@NotNEAR10:	push	ebx
	mov	ebx,LINNUMSymbolAddress
	mov	SymbolStruc.SymbolSeg[ebx],ax
	pop	ebx
	pop	eax
	mov	eax,OSeg.OBase[eax]	;get offset within global segment.
@@ThisOne0:	mov	SymbolObjBase,eax
	popm	edx,esi,edi
	;
	push	edx
	mov	eax,edx
	mov	ecx,4		;LINNUM length.
	cmp	ObjPassOneType,LINNUML	;Big offsets?
	jnz	@@ff5
	mov	ecx,6
@@ff5:	xor	edx,edx
	div	ecx		;Get number of entries.
	mov	ecx,eax
	pop	edx
	;
	xor	eax,eax
	mov	al,SymbolStruc.SymbolTLen[esi]
	add	eax,(size SymbolStruc)-1
	add	esi,eax
	mov	eax,[esi]
	add	[esi],ecx		;update number of entries.
	add	esi,4
	lea	edx,[esi+eax*8]
	mov	eax,ecx
	popm	ecx,esi,edi
	mov	ecx,eax
	cmp	esi,offset DummyLINNUM
	jz	@@ln2
	push	edx
	GetIndex
	GetIndex
	pop	edx
	jmp	@@ln2
	;
@@ff2:	popm	esi,edi
	add	edi,4
	dec	ecx
	jnz	@@ff0
@@ff3:	popm	ecx,esi,edi
;
;Find out how much space we need for the name and header.
;
	push	esi
	mov	esi,Obj.MName[edi]	;Get name pointer.
	xor	ebx,ebx
	mov	bl,[esi]		;Get name length.
	pop	esi
	add	ebx,size SymbolStruc-1	;Include header size.
	;
	;Find out how many entries there are and how much space they need.
	;
	mov	eax,edx
	mov	ecx,4		;LINNUM length.
	cmp	ObjPassOneType,LINNUML	;Big offsets?
	jnz	@@ln0
	mov	ecx,6
@@ln0:	xor	edx,edx
	div	ecx		;Get number of entries.
	mov	ecx,eax
	shl	eax,3		;8 bytes per entry.
	add	eax,4		;include count dword.
	add	ebx,eax		;figure it in the total.
	;
	;Get the memory we need.
	;
	ExpandList LINEList,ListBumpG
	mov	ErrorNumber,2
	jc	@@9
	pushm	ecx,esi
	mov	ecx,ebx
	call	Malloc
	mov	edx,esi
	popm	ecx,esi
	mov	ErrorNumber,2
	jc	@@9
	;
	mov	eax,LINEList
	mov	eax,[eax]
	shl	eax,2
	add	eax,4
	add	eax,LINEList
	mov	[eax],edx		;put this entry in the list.
	mov	eax,LINEList
	inc	d[eax]
	;
	;Setup the header.
	;
	mov	SymbolStruc.SymbolNext[edx],ebx
	mov	SymbolStruc.SymbolType[edx],255
	mov	SymbolStruc.SymbolSeg[edx],0
	mov	SymbolStruc.SymbolDword[edx],0
	mov	ebx,Obj.MName[edi]
	mov	al,[ebx]
	mov	SymbolStruc.SymbolTLen[edx],al
	push	edx
	lea	edx,SymbolStruc.SymbolText[edx]
	inc	ebx
	mov	ah,al
@@ln1:	mov	al,[ebx]
	call	UpperChar
	mov	[edx],al
	inc	ebx
	inc	edx
	dec	ah
	jnz	@@ln1
	pop	edx
	;
	cmp	esi,offset DummyLINNUM
	jz	@@ThisOne1
	;
	push	edx
	GetIndex		;Think this is the GROUP index.
	pop	edx
	;
	push	edx
	GetIndex		;Think this is the SEGDEF index.
	pop	edx
	;
	dec	eax
	shl	eax,4
	add	eax,4
	add	eax,Obj.MSegs[edi]
	push	eax
	mov	eax,OSeg.OGSeg[eax]	;get global segdef index.
	cmp	IsNEAR,0
	jz	@@NotNEAR1
	inc	eax
@@NotNEAR1:	mov	SymbolStruc.SymbolSeg[edx],ax
	pop	eax
	mov	eax,OSeg.OBase[eax]	;get offset within global segment.
	mov	SymbolObjBase,eax
	mov	SymbolStruc.SymbolDword[edx],0
	;
@@ThisOne1:	;Now fetch the real info.
	;
	xor	eax,eax
	mov	al,SymbolStruc.SymbolTLen[edx]
	add	eax,(size SymbolStruc)-1
	add	edx,eax
	mov	[edx],ecx		;Store number of entries.
	add	edx,4
	;
@@ln2:	or	ecx,ecx
	jz	@@ThisOne2
	;
	xor	eax,eax
	mov	ax,[esi]		;Get the line number.
	mov	[edx],eax
	add	esi,2
	add	edx,4
	;
	xor	eax,eax
	mov	ax,[esi]
	add	esi,2
	cmp	ObjPassOneType,LINNUML	;Big offsets?
	jnz	@@ln3
	rol	eax,16
	mov	ax,[esi]
	rol	eax,16
	add	esi,2
@@ln3:	add	eax,SymbolObjBase
	mov	[edx],eax
	add	edx,4
	;
	dec	ecx
	jnz	@@ln2
	;
@@ThisOne2:	popm	edi,ebp
	add	edi,4+4+4+4
	dec	ebp
	jnz	@@0
@@8:	clc
	jmp	@@10
@@9:	popm	edi,ebp
	stc
@@10:	popad
	ret
GetLineInfo	endp


;------------------------------------------------------------------------------
;
;Write out the final 3P image file.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
Write3PFile	proc	near
	pushad
	mov	ebp,offset CauseWayHeader
	mov	NewSize[ebp],size NewHeaderStruc ;update 3P size.
	;
	;Check if an auto DS is needed for DGROUP.
	;
	cmp	AutoDSGROUP,0
	jz	@@NoDGROUP
	mov	eax,AutoDSGROUP
	mov	eax,[eax+4+4]	;get base LSEG
	inc	eax
	mov	NewAutoDS[ebp],ax
	;
@@NoDGROUP:	;Set entry SS:ESP values.
	;
	mov	eax,ProgramEntrySS
	mov	NewEntrySS[ebp],ax
	mov	eax,ProgramEntryESP
	mov	NewEntryESP[ebp],eax
	;
	;Set 16/32 flag based on entry CS attributes.
	;
	mov	eax,ProgramEntryCS
	shl	eax,2
	add	eax,4
	add	eax,SEGDEFList
	mov	eax,[eax]
	cmp	LSeg.SPage[eax],0	;32-bit segment?
	jnz	@@32Bit
	or	NewFlags[ebp],(1 shl 14)+1
	;
@@32Bit:	;Now set entry CS:EIP values.
	;
	mov	eax,ProgramEntryCS
	mov	NewEntryCS[ebp],ax
	mov	eax,ProgramEntryEIP
	mov	NewEntryEIP[ebp],eax
	;
	;Check if the entry CS is really a "NEAR" segment and change
	;if it is.
	;
	mov	eax,ProgramEntryCS
	shl	eax,2
	add	eax,4
	add	eax,SEGDEFList
	mov	eax,[eax]
	mov	eax,LSeg.SClass[eax]
	shl	eax,2
	add	eax,4
	add	eax,LNAMEList
	mov	eax,[eax]
	cmp	b[eax],4
	jnz	@@NotNear0
	cmp	d[eax+1],"RAEN"
	jnz	@@NotNear0
	inc	ProgramEntryCS
	inc	NewEntryCS[ebp]
	or	NEARModel,-1
;
;Work out how many segments we have and update main header.
;
@@NotNear0:	mov	esi,SEGDEFList
	mov	eax,[esi]
	cmp	NEARModel,0
	jz	@@NotNear1
	inc	eax
@@NotNear1:	mov	NewSegments[ebp],ax
	shl	eax,3		;8 bytes per entry.
	add	NewSize[ebp],eax	;update 3P size.
	;
	;Work out how many relocations we have and update main header.
	;
	mov	esi,RelocationList
	or	esi,esi
	jz	@@0
	mov	eax,[esi]
	mov	NewRelocs[ebp],eax
	shl	eax,2		;4 bytes per entry.
	add	NewSize[ebp],eax
	;
@@0:	;Work out how big all segments are and update main header.
	;
	mov	esi,SEGDefList
	mov	ecx,[esi]
	add	esi,4
	xor	edx,edx
@@1:	mov	edi,[esi]		;point to this SEGDEF.
	add	edx,LSeg.SILength[edi]	;update length.
	add	esi,4
	dec	ecx
	jnz	@@1
	add	NewSize[ebp],edx	;update 3P size.
	mov	NewLength[ebp],edx	;set EXE image size.
	mov	NewAlloc[ebp],edx	;set memory size.
;
;Work out how long the EXPORT section is and update main header.
;
	mov	esi,EXPORTList
	mov	ecx,[esi]
	add	esi,4
	xor	edx,edx
	or	ecx,ecx
	jz	@@11
	add	edx,4		;dword for entry count.
	add	edx,4		;dword for module name pointer.
@@12:	mov	edi,[esi]
	add	edx,4		;table entry for offset.
	add	edx,2+4		;seg:offset value.
	add	edx,1		;name length.
	movzx	eax,EXPORT.EXP_TextLen[edi]
	add	edx,eax		;text length.
	add	esi,4
	dec	ecx
	jnz	@@12
	mov	edi,ModuleName
	movzx	eax,b[edi]		;get module name length.
	inc	eax		;name length byte.
	add	edx,eax
@@11:	mov	NewExports[ebp],edx
	add	NewSize[ebp],edx	;update 3P size.
;
;Work out how long the IMPORT section is and update main header.
;
	cmp	IMPORTLength,0
	jz	@@17
	add	IMPORTLength,4+4+4	;3 table offsets.
	add	IMPORTLength+4,4+4+4
	add	IMPORTLength+8,4+4+4
	add	IMPORTLength,4+4+4	;3 entry counts.
	add	IMPORTLength+4,4	;1 entry count.
	add	IMPORTLength+8,4+4	;2 entry counts.
	mov	eax,IMPORTLength
	mov	NewImports[ebp],eax
	add	NewSize[ebp],eax
	mov	eax,IMPORTModules
	mov	eax,[eax]
	mov	NewImportModCnt[ebp],eax	;store module count.
;
;Load the extender stub from this program.
;
@@17:	mov	edx,StubName
	or	edx,edx
	jz	@@NoStub0
	call	OpenFile
	mov	ErrorNumber,25
	jc	@@9
	mov	edx,offset ExeSignature
	mov	ecx,1bh
	call	ReadFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	ax,w[ExeLength+2]	;get length in 512 byte blocks
	dec	ax		;lose 1 cos its rounded up
	add	ax,ax		;mult by 2
	mov	dh,0
	mov	dl,ah
	mov	ah,al
	mov	al,dh		;mult by 256=*512
	add	ax,w[ExeLength]	;add length mod 512
	adc	dx,0		;add any carry to dx
	mov	cx,ax
	xchg	cx,dx
	shl	ecx,16
	mov	cx,dx
	mov	ErrorNumber,2
	add	ecx,4
	call	Malloc
	jc	@@9
	sub	ecx,4
	mov	StubMem,esi
	mov	d[esi],ecx
	add	esi,4
	pushm	ecx,esi
	xor	ecx,ecx
	xor	al,al
	call	SetFilePointer
	popm	ecx,esi
	mov	ErrorNumber,25
	mov	edx,esi
	call	ReadFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	call	CloseFile
	;
@@NoStub0:	;Create the output file.
	;
	mov	edx,EXEFiles
	mov	edx,[edx+4]
	call	CreateFile
	mov	ErrorNumber,10
	jc	@@9
	;
	;Write the stub.
	;
	cmp	StubName,0
	jz	@@NoStub1
	mov	ErrorNumber,11
	mov	edx,StubMem
	mov	ecx,[edx]
	add	edx,4
	call	WriteFile
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
@@NoStub1:	;Write the main header.
	;
	mov	edx,ebp
	mov	ecx,size NewHeaderStruc
	call	WriteFile
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	;Write the segment definitions.
	;
	mov	ebp,SEGDEFList
	mov	edi,[ebp]		;get number of entries.
	add	ebp,4
@@2:	mov	esi,[ebp]		;point to this segment.
	sub	esp,4+4
	mov	edx,esp		;point to work space.
	mov	eax,LSeg.SBase[esi]	;get base address.
	mov	[edx+0],eax
	mov	eax,LSeg.SLimit[esi]	;get limit.
	cmp	NEARModel,0
	jz	@@NotNear5
	or	eax,-1
@@NotNear5:	cmp	eax,100000h
	jc	@@3
	shr	eax,12
	or	eax,1 shl 20		;set G bit.
@@3:	or	eax,1 shl 25		;default to 16-bit.
	cmp	LSeg.SPage[esi],0	;D bit set?
	jz	@@4
	and	eax,not (1 shl 25)	;lose 16-bit flag.
	or	eax,1 shl 26		;set 32-bit flag.
@@4:	xor	ecx,ecx		;Default to class of CODE
	mov	esi,LSeg.SClass[esi]	;Get class LNAME index.
	shl	esi,2		;dword per entry.
	add	esi,4		;skip count dword.
	add	esi,LNAMEList	;point to LNAME list.
	mov	esi,[esi]		;point to this LNAME.
	cmp	b[esi],4		;"CODE" is 4 bytes.
	jc	@@DATA
	cmp	d[esi+1],"EDOC"	;start with CODE?
	jz	@@5
@@DATA:	mov	ecx,1 shl 21
@@5:	or	eax,ecx		;include segment type.
	test	eax,1 shl 26		;32-bit segment?
	jz	@@8
	cmp	FlatSegments,0
	jz	@@8

	cmp	NoFlatCode,0
	jz	@@nfc3
	mov	esi,[ebp]
	test	LSeg.SFlags[esi],1+2	;frame musn't be code or stack.
	jnz	@@8
@@nfc3:
	or	eax,1 shl 27		;signal FLAT segment.
@@8:	mov	[edx+4],eax
	mov	ecx,8
	call	WriteFile
	lea	esp,[esp+4+4]
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	add	ebp,4		;next entry.
	dec	edi
	jnz	@@2
	cmp	NEARModel,0
	jz	@@NotNear2
;
;Add a code segment that maps 1st segment.
;
	mov	ebp,SEGDEFList
	add	ebp,4
	mov	esi,[ebp]		;point to this segment.
	sub	esp,4+4
	mov	edx,esp		;point to work space.
	mov	eax,LSeg.SBase[esi]	;get base address.
	mov	[edx+0],eax
	mov	eax,LSeg.SLimit[esi]	;get limit.
	cmp	eax,100000h
	jc	@@n3
	shr	eax,12
	or	eax,1 shl 20		;set G bit.
@@n3:	cmp	LSeg.SPage[esi],0	;D bit set?
	jz	@@n4
	or	eax,1 shl 26
@@n4:	test	eax,1 shl 26		;32-bit segment?
	jz	@@n8
	cmp	FlatSegments,0
	jz	@@n8
	or	eax,1 shl 27		;signal FLAT segment.
@@n8:	mov	[edx+4],eax
	mov	ecx,8
	call	WriteFile
	lea	esp,[esp+4+4]
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
@@NotNear2:	;Write the relocation entries.
	;
	mov	edx,RelocationList
	or	edx,edx
	jz	@@6
	mov	ecx,[edx]		;get number of entries.
	shl	ecx,2		;4 bytes per entry.
	add	edx,4		;skip count dword.
	call	WriteFile
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
;
;Write the export entries.
;
@@6:	mov	esi,EXPORTList
	mov	ebp,[esi]
	or	ebp,ebp
	jz	@@14
	mov	edx,esi
	mov	ecx,4
	call	WriteFile		;Write the entry count.
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	mov	edi,ebp
	shl	edi,2
	add	edi,4		;set initial record offset.
	add	edi,4		;allow for module name entry
	;
	;Write module name offset.
	;
	push	edi
	mov	edx,esp
	mov	ecx,4
	call	WriteFile		;write this records offset.
	pop	edi
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	add	esi,4
	mov	eax,ModuleName
	movzx	eax,b[eax]
	inc	eax
	add	edi,eax
;
;Write the offset table.
;
@@15:	push	edi
	mov	edx,esp
	mov	ecx,4
	call	WriteFile		;write this records offset.
	pop	edi
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	add	edi,2+4		;seg:offset space.
	add	edi,1		;name length space.
	mov	eax,[esi]
	movzx	eax,EXPORT.EXP_TextLen[eax]
	add	edi,eax
	add	esi,4
	dec	ebp
	jnz	@@15
;
;Write the module name.
;
	mov	edx,ModuleName
	movzx	ecx,b[edx]
	inc	ecx
	call	WriteFile
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
;
;Now write the actual EXPORT entries.
;
	mov	esi,EXPORTList
	mov	ebp,[esi]
	add	esi,4
@@16:	mov	edx,[esi]
	movzx	ecx,EXPORT.EXP_TextLen[edx]
	add	ecx,size EXPORT
	call	WriteFile
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	add	esi,4
	dec	ebp
	jnz	@@16
;
;Write the IMPORTS
;
@@14:	cmp	IMPORTLength,0
	jz	@@18
;
;Write the 3 table offsets.
;
	mov	IMPORTLength,4+4+4
	mov	edx,offset IMPORTLength
	mov	ecx,4+4+4
	call	WriteFile
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
;
;Write the module name table. First the entry count then the table.
;
	mov	edx,IMPORTModules
	mov	ecx,4
	call	WriteFile
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	mov	esi,IMPORTModules
	mov	ebp,[esi]
	add	esi,4
	mov	edi,ebp
	shl	edi,2
	add	edi,4
@@19:	or	ebp,ebp
	jz	@@20
	push	edi
	mov	edx,esp
	mov	ecx,4
	call	WriteFile		;write current offset.
	pop	edi
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	eax,[esi]		;point to the name string and
	movzx	eax,b[eax]		;get its length.
	inc	eax
	add	edi,eax		;update offset value.
	add	esi,4
	dec	ebp
	jmp	@@19
;
;Write the module names.
;
@@20:	mov	esi,IMPORTModules
	mov	ebp,[esi]
	add	esi,4
@@21:	or	ebp,ebp
	jz	@@22
	mov	edx,[esi]		;point to the name string.
	movzx	ecx,b[edx]		;get length.
	inc	ecx		;include length byte.
	call	WriteFile
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	add	esi,4
	dec	ebp
	jmp	@@21
;
;Write the name table. First the entry count then the table.
;
@@22:	mov	edx,IMPORTNames
	mov	ecx,4
	call	WriteFile
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	mov	esi,IMPORTNames
	mov	ebp,[esi]
	add	esi,4
	mov	edi,ebp
	shl	edi,2
	add	edi,4
@@23:	or	ebp,ebp
	jz	@@24
	push	edi
	mov	edx,esp
	mov	ecx,4
	call	WriteFile		;write current offset.
	pop	edi
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	mov	eax,[esi]		;point to the name string and
	movzx	eax,b[eax]		;get its length.
	inc	eax
	add	edi,eax		;update offset value.
	add	esi,4
	dec	ebp
	jmp	@@23
;
;Write the names.
;
@@24:	mov	esi,IMPORTNames
	mov	ebp,[esi]
	add	esi,4
@@25:	or	ebp,ebp
	jz	@@26
	mov	edx,[esi]		;point to the name string.
	movzx	ecx,b[edx]		;get length.
	inc	ecx		;include length byte.
	call	WriteFile
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	add	esi,4
	dec	ebp
	jmp	@@25
;
;Write the fixup entries. First the count then the real data.
;
@@26:	mov	edx,IRelocationList
	mov	ecx,4
	call	WriteFile
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	;
	mov	esi,IRelocationList
	mov	ebp,[esi]
	add	esi,4
@@27:	or	ebp,ebp
	jz	@@28
	mov	edi,[esi]		;point to this entry.
	mov	ecx,1+4		;basic length.
	mov	eax,[edi+1]
	shr	eax,30
	add	ecx,eax		;include module index length.
	mov	eax,[edi+1]
	shr	eax,28
	and	eax,3
	add	ecx,eax		;include name index length.
	mov	edx,edi
	call	WriteFile
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	add	esi,4
	dec	ebp
	jnz	@@27
;
@@28:
;
;Write the segments.
;
@@18:	mov	esi,SEGDEFList
	mov	ebp,[esi]		;get number of entries.
	add	esi,4
@@7:	mov	edx,[esi]		;point to this SEGDEF.
	mov	ecx,LSeg.SILength[edx]	;get data length.
	mov	edx,LSeg.SAddress[edx]	;point to the data.
	call	WriteFile
	mov	ErrorNumber,11
	jc	@@9
	cmp	eax,ecx
	jnz	@@9
	add	esi,4
	dec	ebp
	jnz	@@7
	;
	call	CloseFile		;close it.
	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
Write3PFile	endp


;------------------------------------------------------------------------------
;
;Go through the global segment list and allocate memory for each one.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
AllocateSegMem	proc	near
	pushad
	mov	ebp,SEGDEFList	;Point to segments.
	mov	ErrorNumber,8
	or	ebp,ebp
	jz	@@9		;oops, no segments defined.
	mov	edx,[ebp]		;get number of entries.
	add	ebp,4
	or	edx,edx
	jz	@@9		;don't think this can happen.
	xor	ebx,ebx		;clear offset value.
	;
@@0:	mov	edi,[ebp]		;Point to SEGDEF.
	mov	ecx,LSeg.SLength[edi]	;Get its length.
	mov	LSeg.SLimit[edi],ecx	;Set the limit.
	;
	cmp	edx,1		;Last segment?
	jz	@@1
	;
	;Extend this segments length so next segments alignment is
	;correct.
	;
	push	edi
	mov	edi,[ebp+4]		;Get next segment pointer.
	movzx	eax,LSeg.SAlign[edi]	;Get align type.
	cmp	ParaAlign,0
	jz	@@0_0
	mov	eax,3		;Force para alignment.
@@0_0:	add	ecx,[SEGDEFAlignTable+0+eax*8]
	and	ecx,[SEGDEFAlignTable+4+eax*8]
	pop	edi
	;
@@1:	mov	LSeg.SILength[edi],ecx	;Set its image length.
	call	Malloc
	mov	ErrorNumber,2
	jc	@@9
	mov	LSeg.SAddress[edi],esi	;Store segment memorys address.
	mov	LSeg.SBase[edi],ebx
	add	ebx,ecx		;Update base offset.
	mov	LSeg.SPosition[edi],0	;Clear position again.
	pushad
	mov	edi,esi
	xor	al,al
	rep	stosb
	popad
	add	ebp,4		;point to next entry.
	dec	edx
	jnz	@@0
	;
	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
AllocateSegMem	endp


;------------------------------------------------------------------------------
;
;Go through the global segment list and allocate memory for each one.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
ReleaseSegMem	proc	near
	pushad
	mov	ebp,SEGDEFList	;Point to segments.
	mov	ErrorNumber,8
	or	ebp,ebp
	jz	@@9		;oops, no segments defined.
	mov	edx,[ebp]		;get number of entries.
	add	ebp,4
	or	edx,edx
	jz	@@9		;don't think this can happen.
	;
@@0:	mov	edi,[ebp]		;Point to SEGDEF.
	xor	esi,esi
	xchg	esi,LSeg.SAddress[edi]	;get segment memorys address.
	call	Free
	jc	@@9
	add	ebp,4		;point to next entry.
	dec	edx
	jnz	@@0
	;
	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
ReleaseSegMem	endp


;------------------------------------------------------------------------------
;
;Generate an EXE file name if one isn't supplied.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
GetEXEName	proc	near
	pushad
	cmp	EXEFiles,0		;Anything specified?
	jnz	@@8
	mov	esi,ObjFiles
	mov	esi,[esi+4]		;Use base OBJ name.
	mov	edi,offset FileNameSpace
@@1:	mov	al,[esi]
	inc	esi
	cmp	al,"."
	jnz	@@2
	mov	ebx,edi
@@2:	mov	b[edi],al
	inc	edi
	or	al,al
	jnz	@@1
	mov	edi,ebx
	mov	esi,offset ExtensionList+4
	movsd
	mov	b[edi],0
	mov	edi,offset FileNameSpace
	mov	esi,offset EXEFiles
	call	AddFileName2List	;Add this name.
	mov	ErrorNumber,2
	jc	@@9
@@8:	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
GetEXEName	endp


;------------------------------------------------------------------------------
;
;Generate a SYM file name if one isn't supplied.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
GetSYMName	proc	near
	pushad
	mov	esi,EXEFiles		;Get EXE name.
	mov	esi,[esi+4]
	mov	edi,offset FileNameSpace
@@1:	mov	al,[esi]
	inc	esi
	cmp	al,"."
	jnz	@@2
	mov	ebx,edi
@@2:	mov	b[edi],al
	inc	edi
	or	al,al
	jnz	@@1
	mov	edi,ebx
	mov	esi,offset ExtensionList+20
	movsd
	mov	b[edi],0
	mov	edi,offset FileNameSpace
	mov	esi,offset SYMFiles
	call	AddFileName2List	;Add this name.
	mov	ErrorNumber,2
	jc	@@9
@@8:	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
GetSYMName	endp


;------------------------------------------------------------------------------
;
;Generate a MAP file name if one isn't supplied.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
GetMAPName	proc	near
	pushad
	cmp	MAPFiles,0		;Anything specified?
	jnz	@@3
	cmp	WantMAP,0		;Forced MAP name?
	jz	@@8
	;
	mov	esi,EXEFiles		;Get EXE name.
	mov	esi,[esi+4]
	mov	edi,offset FileNameSpace
@@1:	mov	al,[esi]
	inc	esi
	cmp	al,"."
	jnz	@@2
	mov	ebx,edi
@@2:	mov	b[edi],al
	inc	edi
	or	al,al
	jnz	@@1
	mov	edi,ebx
	mov	esi,offset ExtensionList+8
	movsd
	mov	b[edi],0
	mov	edi,offset FileNameSpace
	mov	esi,offset MAPFiles
	call	AddFileName2List	;Add this name.
	mov	ErrorNumber,2
	jc	@@9
@@3:	or	WantMAP,-1
@@8:	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
GetMAPName	endp


;------------------------------------------------------------------------------
;
;Load all object modules specified.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
LoadObjs	proc	near
	pushad
	mov	ebp,ObjFiles
	mov	ebx,[ebp]
	add	ebp,4
@@0:	mov	edx,[ebp]
	mov	ErrorName,edx
	call	AllocLoad		;Read this module.
	jc	@@9
	call	AddObjModule		;add this module to the list.
	jc	@@9
	add	ebp,4
	dec	ebx
	jnz	@@0
	mov	ErrorName,0
	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
LoadObjs	endp


;------------------------------------------------------------------------------
;
;Add an object module to the list.
;
;On Entry:
;
;ESI	- Address of module.
;ECX	- Length of module.
;
;On Exit:
;
;Carry set on error else,
;
;All other registers preserved.
;
AddObjModule	proc	near
	pushad
	mov	ErrorNumber,2
	ExpandList ObjList,ListBumpG
	jc	@@9
	;
	pushm	ecx,esi
	mov	ecx,size Obj
	call	Malloc
	mov	ebp,esi
	popm	ecx,esi
	jc	@@9
	push	ecx
	mov	edi,ebp
	mov	ecx,size Obj
	xor	al,al
	rep	stosb
	pop	ecx
	;
	mov	edi,ObjList
	mov	eax,[edi]		;get current last entry.
	inc	d[edi]		;update number of entries.
	lea	edi,[edi+4+eax*4]
	mov	[edi],ebp		;point to new entry.
	mov	Obj.Module[ebp],esi
	mov	Obj.MLength[ebp],ecx
	mov	Obj.MFileName[ebp],edx
	;
	;Check if we can get a name from it.
	;
	cmp	b[esi],80h
	jnz	@@NoName
	mov	eax,esi
	add	eax,1+2
	mov	Obj.MName[ebp],eax
@@NoName:	;
	mov	ecx,4+(ListBumpL*4)
	call	Malloc
	jc	@@9
	mov	d[esi],0
	mov	Obj.MLNAMES[ebp],esi
	call	Malloc
	jc	@@9
	mov	d[esi],0
	mov	Obj.MSegs[ebp],esi
	call	Malloc
	jc	@@9
	mov	d[esi],0
	mov	Obj.MGROUPS[ebp],esi
	call	Malloc
	jc	@@9
	mov	d[esi],0
	mov	Obj.MExtDefs[ebp],esi
	;
	clc
	;
@@9:	popad
	ret
AddObjModule	endp


;------------------------------------------------------------------------------
;
;Setup the LIB file search paths.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
InitLIBPath	proc	near
	push	es
	pushad
;
;Get current path.
;
	mov	edx,offset FileNameSpace
	call	GetPath
;
;Fetch the environment address so we can look for LIB=
;
	mov	ah,51h		;Get current PSP address so we
	int	21h		;can find the environment strings.
	mov	es,bx
	mov	es,es:w[2ch]		;Point to the environment strings.
;
;Now search for LIB=
;
	xor	edi,edi
@@0:	mov	esi,offset LIBText
	mov	ecx,LIBLen
	repe	cmpsb
	jz	@@2
@@1:	mov	al,es:[edi]
	inc	edi
	or	al,al
	jnz	@@1
	mov	al,es:[edi]
	or	al,al
	jnz	@@0
;
;No LIB environment variable.
;
	xor	ecx,ecx		;reset length to fetch.
	jmp	@@4
;
;Found LIB= so find out how long it is.
;
@@2:	mov	esi,edi
@@12:	mov	al,es:[esi]		;Skip white space.
	inc	esi
	cmp	al," "
	jz	@@12
	dec	esi
	;
	push	esi
	xor	ecx,ecx
@@3:	mov	al,es:[esi]		;Scan for zero to get length.
	inc	esi
	inc	ecx
	or	al,al
	jnz	@@3
	pop	esi
;
;Get length of current path and allocate enough memory for LIB path + current.
;
@@4:	pushm	esi,ecx,es,ds
	pop	es
	mov	esi,offset FileNameSpace
	call	LenString
	add	ecx,eax
	inc	ecx		;Include terminator.
	inc	ecx		;Include real terminator.
	call	Malloc
	mov	edi,esi
	popm	esi,ecx,es
	mov	ErrorNumber,2
	jc	@@9
	mov	LIBPaths,edi
;
;Copy current path into the list.
;
	push	esi
	mov	esi,offset FileNameSpace
@@5:	mov	al,[esi]
	mov	[edi],al
	inc	esi
	inc	edi
	cmp	b[esi-1],0
	jnz	@@5
	pop	esi
;
;Copy environment stuff into the list.
;
	or	ecx,ecx		;Anything to fetch?
	jz	@@8
@@6:	mov	al,es:[esi]
	inc	esi
	mov	[edi],al
	inc	edi
	or	al,al		;End of them all?
	jz	@@8
	cmp	al,";"		;Local terminator?
	jz	@@7
	mov	[edi],al
	jmp	@@6
@@7:	mov	b[edi-1],0
	jmp	@@6
;
;Add final terminator and exit.
;
@@8:	mov	b[edi],-1
	clc
	jmp	@@10
@@9:	stc
@@10:	popad
	pop	es
	ret
InitLIBPath	endp


;------------------------------------------------------------------------------
;
;Load all library modules specified.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
LoadLibs	proc	near
	pushad
	mov	ebp,LibFiles
	or	ebp,ebp
	jz	@@8
	mov	ebx,[ebp]		;Get number of entries.
	add	ebp,4
	or	ebx,ebx
	jz	@@8
@@l0:	mov	edx,[ebp]
	call	LoadLIBFile
	jc	@@9
	add	ebp,4
	dec	ebx
	jnz	@@l0
@@8:	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
LoadLibs	endp


;------------------------------------------------------------------------------
;
;Load a LIB file. The current list of loaded LIB's is searched first to make
;sure we don't already have this library. The current directory and then the
;LIB environment are used as search paths for the file.
;
;On Entry:
;
;EDX	- File name to load.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
LoadLIBFile	proc	near
	pushad
;
;Search loaded lib list for a matching entry.
;
	mov	esi,LIBList
	mov	ecx,[esi]		;Get number of entries.
	or	ecx,ecx
	jz	@@LoadIt
	add	esi,4
@@0:	pushm	edx,esi
	lea	esi,LIB.LName[esi]
@@1:	mov	al,[esi]
	mov	ah,[edx]
	call	UpperChar
	xchg	ah,al
	call	UpperChar
	cmp	al,ah
	jnz	@@2
	or	al,al
	jz	@@2
	inc	esi
	inc	edx
	jmp	@@1
@@2:	popm	edx,esi
	jz	@@8
	add	esi,size LIB
	dec	ecx
	jnz	@@0
;
;Didn't find this name in the list so we'd better load it.
;
@@LoadIt:	mov	eax,LIBPaths		;Setup base path pointer.
	mov	LIBPaths+4,eax
	cmp	b[edx+1],":"		;Path specified already?
	jnz	@@4
	mov	esi,LIBPaths
@@5:	mov	al,[esi]
	inc	esi
	cmp	al,-1
	jnz	@@5
	dec	esi
	mov	LIBPaths+4,esi	;Force end of list.
	mov	edi,offset LIBFileNameSpace
	jmp	@@7
;
;Fetch current search path and append real name.
;
@@4:	mov	edi,offset LIBFileNameSpace
	mov	esi,LIBPaths+4
@@6:	movsb
	cmp	b[esi-1],0
	jnz	@@6
	dec	edi
	cmp	b[edi-1],"\"		;Trailing "\"?
	jz	@@7
	mov	b[edi],"\"
	inc	edi
	mov	LIBPaths+4,esi	;Store new path pointer.
;
;Append specified name.
;
@@7:	mov	esi,edx
	cmp	b[esi],"\"		;Leading "\"?
	jnz	@@11
	inc	esi
@@11:	movsb
	cmp	b[esi-1],0
	jnz	@@11
;
;Try and load the file.
;
	mov	ErrorName,edx
	push	edx
	mov	edx,offset LIBFileNameSpace
	call	AllocLoad		;Read this module.
	pop	edx
	jnc	@@3
	cmp	ErrorNumber,3
	jnz	@@9
	mov	esi,LIBPaths+4
	cmp	b[esi],-1		;End of the list?
	jnz	@@4		;Keep looking.
	jmp	@@9
	;
@@3:	call	AddLibModule		;add this library to the list.
	jc	@@9
	;
@@8:	mov	ErrorName,0
	clc
	jmp	@@10
@@9:	stc
@@10:	popad
	ret
LoadLIBFile	endp


;------------------------------------------------------------------------------
;
;Add a LIB file to the list.
;
;On Entry:
;
;EDX	- File name.
;ESI	- Address of module.
;ECX	- Length of module.
;
;On Exit:
;
;Carry set on error else,
;
;All other registers preserved.
;
AddLibModule	proc	near
	pushad
	mov	ErrorNumber,2
	pushm	ecx,esi
	mov	esi,LibList		;point to the list of LIB's.
	mov	ecx,[esi]
	inc	ecx		;+1 for this new entry.
	mov	eax,size Lib
	push	edx
	mul	ecx
	pop	edx
	mov	ecx,eax
	add	ecx,4		;include count dword.
	call	ReMalloc		;try and get new size.
	mov	edi,esi
	popm	ecx,esi
	jc	@@9
	mov	LibList,edi		;store the list pointer.
	;
	mov	eax,[edi]		;get current last entry.
	inc	d[edi]		;update number of entries.
	add	edi,4		;skip count field.
	push	edx
	mov	edx,size Lib
	mul	edx
	pop	edx
	pushm	eax,ecx,edi
	add	edi,eax
	xor	al,al
	mov	ecx,size Lib
	rep	stosb
	popm	eax,ecx,edi
	mov	Lib.LAddress[edi+eax],esi
	mov	Lib.LLength[edi+eax],ecx
	lea	edi,LIB.LName[edi+eax]
	mov	esi,edx
@@1:	movsb
	cmp	b[esi-1],0		;Copy the name through.
	jnz	@@1
	;
	clc
	;
@@9:	popad
	ret
AddLibModule	endp


;------------------------------------------------------------------------------
;
;Allocate memory for specified file and return its load address.
;
;On Entry:
;
;EDX	- Fully qualified file name.
;
;On Exit:
;
;Carry set on error else,
;
;ESI	- Load address.
;ECX	- Length.
;
AllocLoad	proc	near
	pushm	eax,ebx,edx
	mov	ErrorNumber,3
	call	OpenFile		;Try and open the file.
	jc	@@9
	xor	ecx,ecx
	mov	al,2		;Move to the end of the file so
	call	SetFilePointer	;we know how long it is.
	mov	ErrorNumber,2
	call	Malloc		;allocate memory for it.
	jc	@@8
	push	ecx
	xor	ecx,ecx
	xor	al,al
	call	SetFilePointer	;back to start of the file.
	pop	ecx
	mov	edx,esi
	mov	ErrorNumber,4
	call	ReadFile		;read the file.
	jc	@@7
	cmp	eax,ecx		;did we read enough?
	jnz	@@7
	call	CloseFile		;close the file again.
	clc
	jmp	@@10		;exit with pertinent values.
	;
@@7:	call	Free		;lose the memory we allocated.
@@8:	call	CloseFile		;close the file.
@@9:	stc
@@10:	popm	eax,ebx,edx
	ret
AllocLoad	endp


;------------------------------------------------------------------------------
;
;Add an entry to the relocation list.
;
;On Entry:
;
;EAX	- Item to add.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
AddRelocationEntry proc near
	pushad
;
;Expand the list and add an entry.
;
	ExpandList RelocationList,ListBumpG
	mov	ErrorNumber,2
	jc	@@9
	mov	esi,RelocationList
	mov	ecx,[esi]
	inc	d[esi]	
	mov	d[esi+4+ecx*4],eax
	;
	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
AddRelocationEntry endp


;------------------------------------------------------------------------------
;
;Add an entry to the IMPORT relocation list.
;
;On Entry:
;
;EAX	- Item to add.
;
;On Exit:
;
;Carry set on error else,
;
;All registers preserved.
;
AddIRelocationEntry proc near
	pushad
;
;Make up a relocation entry.
;
	mov	edi,offset IMPORTSpace
	mov	b[edi],al		;set type.
	inc	edi
	mov	d[edi],ebx		;set fixup address.
	mov	esi,edi		;store flags address.
	add	edi,4
;
;Work out how many bytes the module name index needs.
;
	mov	eax,ecx
	push	ecx
	mov	ecx,32
@@cb0:	rcl	eax,1
	jc	@@cb1
	dec	ecx
	jnz	@@cb0
@@cb1:	mov	eax,ecx
	pop	ecx
	or	eax,eax
	jnz	@@cb4
	inc	eax
@@cb4:	add	eax,7
	shr	eax,3		;get byte count.
;
;Update flags with byte count.
;
	push	eax
	shl	eax,30
	or	d[esi],eax		;set module index length.
	pop	eax
;
;Store the module name index.
;
	dec	eax
	jz	@@mbyte
	dec	eax
	jz	@@mword
	dec	eax
	jz	@@mbword
@@mdword:	mov	[edi],ecx
	add	edi,4
	jmp	@@0
@@mbword:	mov	[edi],cx
	add	edi,2
	shr	ecx,16
	mov	b[edi],cl
	inc	edi
	jmp	@@0
@@mword:	mov	[edi],cx
	add	edi,2
	jmp	@@0
@@mbyte:	mov	[edi],cl
	inc	edi
;
;Work out how many bytes the name index needs.
;
@@0:	mov	eax,edx
	push	ecx
	mov	ecx,32
@@cb2:	rcl	eax,1
	jc	@@cb3
	dec	ecx
	jnz	@@cb2
@@cb3:	mov	eax,ecx
	pop	ecx
	or	eax,eax
	jnz	@@cb5
	inc	eax
@@cb5:	add	eax,7
	shr	eax,3		;get byte count.
;
;Update flags with byte count.
;
	push	eax
	shl	eax,28
	or	d[esi],eax		;set name index length.
	pop	eax
;
;Store the name index.
;
	dec	eax
	jz	@@nbyte
	dec	eax
	jz	@@nword
	dec	eax
	jz	@@nbword
@@ndword:	mov	[edi],edx
	add	edi,4
	jmp	@@1
@@nbword:	mov	[edi],dx
	add	edi,2
	shr	edx,16
	mov	b[edi],dl
	inc	edi
	jmp	@@1
@@nword:	mov	[edi],dx
	add	edi,2
	jmp	@@1
@@nbyte:	mov	[edi],dl
	inc	edi
;
;Now allocate some memory for this entry and stow it away.
;
@@1:	sub	edi,offset IMPORTSpace	;get entry length.
	mov	ecx,edi
	call	Malloc
	mov	ErrorNumber,2
	jc	@@9
	pushm	esi,ecx
	mov	edi,esi
	mov	esi,offset IMPORTSpace
	rep	movsb		;copy this entry through.
	popm	esi,ecx
	add	IMPORTLength,ecx	;update IMPORT table length.
;
;Now expand the IMPORT fixup list and add this entry.
;
	mov	edi,esi
	ExpandList IRelocationList,ListBumpG
	mov	ErrorNumber,2
	jc	@@9
	mov	esi,IRelocationList
	mov	ecx,[esi]
	inc	d[esi]	
	mov	d[esi+4+ecx*4],edi
	;
	clc
	jmp	@@10
	;
@@9:	stc
@@10:	popad
	ret
AddIRelocationEntry endp


;------------------------------------------------------------------------------
Start	proc	near
;
;Store useful values.
;
	mov	ax,DGROUP
	mov	ds,ax
	mov	PSPSegment,es
	mov	ax,es:w[2ch]
	mov	ENVSegment,ax
	push	ds
	pop	es
;
;Try to lock programs memory. Uses currently un-documented entries in the PSP.
;
	mov	fs,PSPSegment
	mov	esi,fs:[EPSP_MemBase]
	mov	ecx,fs:[EPSP_MemSize]
	sys	LockMem32
	mov	edx,offset PhysicalMemError
	jc	an_error
;
;Get a code segment alias.
;
	mov	bx,cs
	sys	AliasSel
	mov	edx,offset SelectorError
	jc	an_error
	mov	CodeSegAlias,ax
;
;Get linear base of DS
;
	mov	bx,ds
	sys	GetSelDet32
	mov	DataLinearBase,edx
	or	ecx,-1
	sys	SetSelDet32
;
;Move ESP into the data segment.
;
	mov	bx,ds
	sys	GetSelDet32
	mov	esi,edx
	mov	bx,ss
	sys	GetSelDet32
	sub	edx,esi
	mov	ax,ds
	mov	ss,ax
	add	esp,edx
;
;Get 0-4G selector.
;
	pushad
	push	es
	sys	Info
	mov	ZeroSelector,ax
	pop	es
	popad
;
;Install a critical error handler.
;
	mov	bl,24h
	sys	GetVect
	mov	d[OldInt24],edx
	mov	w[OldInt24+4],cx
	mov	edx,offset CriticalHandler
	mov	cx,cs
	sys	SetVect
;
;Get main application file name.
;
	mov	es,ENVSegment
	xor	esi,esi
l0:	mov	al,es:[esi]		;Get a byte.
	inc	esi		;/
	or	al,al		;End of a string?
	jnz	l0		;keep looking.
	mov	al,es:[esi]		;Double zero?
	or	al,al		;/
	jnz	l0		;keep looking.
	add	esi,3		;Skip last 0 and word count.
	mov	edi,offset ExecutionPath
	mov	ecx,128
	pushm	ds,ds,es
	popm	es,ds
l2:	movsb
	cmp	b[esi-1],0		;got to the end yet?
	jz	l3
	loop	l2
l3:	pop	ds
	push	edi
	mov	ecx,edi
	mov	esi,offset ExecutionPath
	mov	edi,offset ExecutionName
	sub	ecx,esi
	rep	movsb
	pop	edi
l4:	cmp	b[edi-1],"\"
	jz	l5
	cmp	b[edi-1],":"
	jz	l5
	dec	edi
	jmp	l4
l5:	mov	b[edi],0
;
;Call the main code.
;
	call	Main
	push	eax
;
;Check if we should remove critical error handler.
;
	cmp	w[OldInt24+4],0
	jz	l9
	mov	bl,24h
	mov	edx,d[OldInt24]
	mov	cx,w[OldInt24+4]
	sys	SetVect
;
;terminate.
;
l9:	pop	eax
	mov	ah,4ch
	int	21h
;
;Error conditions during startup come here.
;
an_error:	mov	ah,9
	int	21h
	mov	eax,-1
	push	eax
	jmp	l9
Start	endp


;-------------------------------------------------------------------------
;
;Critical error handler, returns FAIL code so just checking for carry will
;take care of things.
;
CriticalHandler proc far
	mov	ax,3
	iretd
CriticalHandler endp


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
	push	eax
	sys	GetMemLinear32
	jc	l0
	sub	esi,DataLinearBase
	clc
	jmp	l1
l0:	xor	esi,esi
	stc
l1:	pop	eax
	ret
Malloc	endp


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
	pushm	eax,esi
	add	esi,DataLinearBase	;make it a linear address again.
	sys	RelMemLinear32
	popm	eax,esi
	ret
Free	endp


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
	pushm	eax,edx
	add	esi,DataLinearBase
	sys	ResMemLinear32
	jc	l0
	sub	esi,DataLinearBase
	clc
	jmp	l1
l0:	xor	esi,esi
	stc
l1:	popm	eax,edx
	ret
ReMalloc	endp


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
ReadFile	proc	near
	pushm	ecx,edx,esi
	xor	esi,esi		;reset length read.
@@0:	pushm	ebx,ecx,edx,esi
	cmp	ecx,65535		;size of chunks to load.
	jc	@@1
	mov	ecx,65535		;as close to 64k as can get.
@@1:	mov	ah,3fh
	int	21h		;read from the file.
	popm	ebx,ecx,edx,esi
	jc	@@2		;DOS error so exit NOW.
	movzx	eax,ax		;get length read.
	add	esi,eax		;update length read counter.
	add	edx,eax		;move memory pointer.
	sub	ecx,eax		;update length counter.
	jz	@@2		;read as much as was wanted.
	or	eax,eax		;did we read anything?
	jnz	@@0
@@2:	mov	eax,esi
	popm	ecx,edx,esi
	ret
ReadFile	endp


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
WriteFile	proc	near
	pushm	ecx,edx,esi
	xor	esi,esi
@@0:	pushm	ebx,ecx,edx,esi
	cmp	ecx,65535		;size of chunks to load.
	jc	@@1
	mov	ecx,65535		;as close to 64k as can get.
@@1:	mov	ah,40h
	int	21h		;read from the file.
	popm	ebx,ecx,edx,esi
	jc	@@2
	movzx	eax,ax		;get length read.
	add	esi,eax		;update length read counter.
	add	edx,eax		;move memory pointer.
	sub	ecx,eax		;update length counter.
	jz	@@2		;read as much as was wanted.
	or	eax,eax		;did we write anything?
	jz	@@2
	jmp	@@0
@@2:	mov	eax,esi
	popm	ecx,edx,esi
	ret
WriteFile	endp


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
OpenFile	proc	near
	push	eax
	mov	ax,3d02h		;Open with read & write access.
	int	21h
	mov	bx,ax
	jnc	l0
	xor	bx,bx
	stc
l0:	pop	eax
	movzx	ebx,bx
	ret
OpenFile	endp


;-------------------------------------------------------------------------
;
;Close a file.
;
;On Entry:
;
;EBX	- file handle.
;
CloseFile	proc	near
	push	eax
	mov	ah,3eh
	int	21h
	pop	eax
	ret
CloseFile	endp


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
CreateFile	proc	near
	pushm	eax,ecx
	mov	ah,3ch		;Create function.
	xor	cx,cx		;normal attributes.
	int	21h
	mov	bx,ax
	jnc	l0
	xor	bx,bx
	stc
l0:	movzx	ebx,bx
	popm	eax,ecx
	ret
CreateFile	endp


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
SetFilePointer	proc	near
	pushm	eax,edx
	mov	dx,cx
	shr	ecx,16
	mov	ah,42h		;set pointer function.
	int	21h
	mov	cx,dx
	shl	ecx,16
	mov	cx,ax		;fetch small result.
	popm	eax,edx
	ret
SetFilePointer	endp


;-------------------------------------------------------------------------
;
;Print null terminated string on screen via DOS.
;
;On Entry:
;
;ESI	- pointer to string to print.
;
PrintString	proc	near
	pushm	eax,esi,edx
l0:	mov	dl,[esi]
	inc	esi
	or	dl,dl
	jz	l2
	mov	ah,2
	int	21h
	jmp	l0
l2:	popm	eax,esi,edx
	ret
PrintString	endp


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
LenString	proc	near
	pushm	edi,ecx
	mov	edi,esi
	or	ecx,-1
	xor	al,al
	repnz	scasb		;get the strings length.
	mov	eax,ecx
	not	eax
	dec	eax		;Don't include terminator.
	popm	edi,ecx
	ret
LenString	endp


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
UpperChar	proc	near
	cmp	al,61h		; 'a'
	jb	@@1
	cmp	al,7Ah		; 'z'
	ja	@@1
	and	al,5Fh		;convert to upper case.
@@1:	ret
UpperChar	endp


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
GetPath	proc	near
	pushm	eax,edx,esi,edx
	mov	ah,19h	;get current disc
	int	21h
	mov	dl,al
	add	al,'A'	;make it a character
	pop	esi
	mov	[esi],al
	inc	esi
	mov	b[esi],":"
	inc	esi
	mov	b[esi],"\"
	inc	esi
	mov	b[esi],0
	mov	ah,47h	;get current directory
	xor	dl,dl	;default drive
	int	21h	;get text
	popm	eax,edx,esi
	ret
GetPath	endp


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
Bin2Hex	proc	near
	pushm	eax,ebx,ecx,edx,edi
	mov	ebx,offset HexTable
	add	edi,ecx
	dec	edi
	mov	edx,eax
@@0:	mov	al,dl
	shr	edx,4
	and	al,15
	xlat
	mov	[edi],al
	dec	edi
	loop	@@0
	popm	eax,ebx,ecx,edx,edi
	add	edi,ecx
	ret
Bin2Hex	endp


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
Bin2Dec	proc	near
	pushm	eax,ebx,ecx,edx
	xor	bl,bl		;Clear output flag.
	mov	ecx,1000000000	;Set initial divisor.
l0:	xor	edx,edx
	div	ecx
	cmp	ecx,1		;Last digit?
	jz	l1
	or	al,al		;a zero?
	jnz	l1
	or	bl,bl		;done any others yet?
	jz	l2
l1:	mov	[edi],al		;Store character.
	add	b[edi],'0'		;make it ASCII.
	inc	edi
	inc	bl
l2:	push	edx		;store remainder.
	mov	eax,ecx
	xor	edx,edx
	mov	ecx,10		;work out new divisor.
	div	ecx
	mov	ecx,eax
	pop	eax		;get remaindor back.
	or	ecx,ecx
	jnz	l0
	popm	eax,ebx,ecx,edx
	ret
Bin2Dec	endp


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
UpperString	proc	near
	pushm	eax,esi,edi
	mov	edi,esi
l0:	lodsb
	cmp	al,61h		; 'a'
	jb	l1
	cmp	al,7Ah		; 'z'
	ja	l1
	and	al,5Fh		;convert to upper case.
l1:	stosb
	or	al,al
	jnz	l0
	popm	eax,esi,edi
	ret
UpperString	endp


	.data
;
Copyright	db 13,10
	db 79 dup ("Ä"),13,10
	db "CauseWay Linker v1.37 Copyright 1994 John Wildsmith; all rights reserved.",13,10
	db 79 dup ("Ä"),13,10
	db 13,10,0
;
ErrorESP	dd ?
ErrorNumber	dd 0
ErrorCount	dd 0
ErrorName	dd 0,0,0
ErrorNameSpace1 db 256 dup (0)
ErrorNameSpace2 db 256 dup (0)
ErrorNameSpace3 db 256 dup (0)
;
ErrorList	dd ErrorM00,ErrorM01,ErrorM02,ErrorM03,ErrorM04,ErrorM05,ErrorM06,ErrorM07
	dd ErrorM08,ErrorM09,ErrorM10,ErrorM11,ErrorM12,ErrorM13,ErrorM14,ErrorM15
	dd ErrorM16,ErrorM17,ErrorM18,ErrorM19,ErrorM20,ErrorM21,ErrorM22,ErrorM23
	dd ErrorM24,ErrorM25,ErrorM26,ErrorM27,ErrorM28,ErrorM29,ErrorM30,ErrorM31
	dd ErrorM32,ErrorM33,ErrorM34,ErrorM35
;
ErrorM00	db "Operation completed succesfully.",13,10,0
ErrorM01	db "Usage:",13,10
	db 13,10
	db "CWL [options] objfiles, exefile, mapfile, libfiles, deffile",13,10
	db 13,10
	db "Options:",13,10
	db 13,10
	db " ALP             - Force segment para align.",13,10
	db " FLAT            - Enable FLAT EXE generation.",13,10
	db " M[AP]           - Write MAP file.",13,10
	db " NOD             - No default library search.",13,10
	db " NOI             - No ignore case of symbols.",13,10
	db " NOSTACK         - Don't force a stack.",13,10
	db " NOSTUB          - Don't add stub loader.",13,10
	db " ST[ACK]:<size>  - Set stack size.",13,10
	db " SY[M]           - Write SYM file.",13,10
	db 13,10
	db " The options above that have a + before them are ON by default.",13,10
	db 13,10,13,10,0
;
ErrorM02	db "FATAL: Not enough memory.",13,10,0
ErrorM03	db "FATAL: Unable to open file: ",0
ErrorM04	db "FATAL: Error reading file: ",0
ErrorM05	db "FATAL: Corrupt object module, end not found in: ",0
ErrorM06	db "FATAL: Corrupt object module, invalid checksum in: ",0
ErrorM07	db "WARNING: Un-known command in: ",0
ErrorM08	db "FATAL: No segments defined.",13,10,0
ErrorM09	db "FATAL: Data emitted to undefined segment in: ",0
ErrorM10	db "FATAL: Could not create EXE file.",13,10,0
ErrorM11	db "FATAL: DOS reported an error while writeing EXE file.",13,10,0
ErrorM12	db "FATAL: Could not create MAP file.",13,10,0
ErrorM13	db "WARNING: (FIXUP) Target out of range.",13,10,0
ErrorM14	db "FATAL: Could not create SYM file.",13,10,0
ErrorM15	db "FATAL: (FIXUP) Bad target type.",13,10,0
ErrorM16	db "FATAL: (FIXUP) Bad frame type.",13,10,0
ErrorM17	db "FATAL: (FIXUP) FRAME is beyond TARGET.",13,10,0
ErrorM18	db "FATAL: (FIXUP) Bad location type.",13,10,0
ErrorM19	db "FATAL: Bad object record type: 0"
ErrorM19_N	db "00h in: ",0
ErrorM20	db "FATAL: (LIDATA) Invalid repeat length (0).",13,10,0
ErrorM21	db "FATAL: Too many file name fields used.",13,10,0
ErrorM22	db "WARNING: (EXPORT) No matching PUBLIC in: ",0
ErrorM23	db "FATAL: (SEGDEF) Bad align type in: ",0
ErrorM24	db "WARNING: Unknown command line option: ",0
ErrorM25	db "FATAL: Error opening stub file.",13,10,0
ErrorM26	db "WARNING: Un-resolved external referance.",13,10,0
ErrorM27	db "FATAL: DOS reported an error while writing SYM file.",13,10,0
ErrorM28	db "FATAL: DOS reported an error while writing MAP file.",13,10,0
ErrorM29	db "WARNING: Duplicate public definition in: ",0
ErrorM30	db " already defined in: ",0
ErrorM31	db "Invalid character in /ST[ACK]: option.",13,10,0
ErrorM32	db 32 dup (0)
ErrorM32_0	db " Error(s)/Warning(s).",0
ErrorM33	db "WARNING: (IMPORT) Unexpected terminator in: ",0
ErrorM34	db "WARNING: (IMPORT) No internal name for ordinal in: ",0
ErrorM35	db "WARNING: (IMPORT) Invalid ordinal in: ",0
;
ObjList	dd 0
LibList	dd 0
LNAMEList	dd 0
SEGDEFList	dd 0
GROUPList	dd 0
PUBLICList	dd 0
EXTDEFList	dd 0,0,0
RelocationList	dd 0
IRelocationList dd 0
SYMList	dd 0
LINEList	dd 0
LINNUMList	dd 0
EXPORTList	dd 0
IMPORTList	dd 0
IMPORTModules	dd 0
IMPORTNames	dd 0
;
CauseWayHeader	NewHeaderStruc <>
;
FixupType	db 0
FixupFlags	db 0
FixupLocationType db 0
FixupLocationOffset dd 0
FixupFrameMethod db 0
FixupFrameIndex dd 0
FixupFrameSEGDEF dd 0
FixupTargetMethod db 0
FixupTargetIMPORT db 0
FixupTargetIndex dd 0
FixupTargetSEGDEF dd 0
FixupTargetOffset dd 0,0
FixupEntryESP	dd 0
FixupBigFlags	db 0
;
LastLEDATA_SEGDEF dd 0
LastLEDATA_OFFSET dd 0
LastLEDATA_TYPE dd 0,0
;
LNAMEetcRecType db 0
FixupRecType	db 0
FixupRecLIType	db 0
FixupLIDATAOffset dd 0
;
SEGDEFAlignTable dd 0,0, 0,-1, 1,not 1, 15,not 15, 4095, not 4095, 3,not 3
;
AutoStackSize	dd 1024
;
StackLNAME	db 5,"STACK"
;
ProgramEntrySS	dd 0
ProgramEntryESP dd 0
;
FixupDoingCSEIP db 0
ProgramEntryEIP dd 0
ProgramEntryCS	 dd 0
;
ExtensionList	db ".OBJ.EXE.MAP.LIB.DEF.SYM"
;
FileNameSpace	db 256 dup (0)
;
NamesList	dd ObjFiles,EXEFiles,MAPFiles,LIBFiles,DEFFiles
ObjFiles	dd 0
EXEFiles	dd 0
MAPFiles	dd 0
LIBFiles	dd 0
DEFFiles	dd 0
SYMFiles	dd 0
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
	db 20h-1bh dup (0)
;
StubMem	dd 0
ObjPassOneType	db 0
;
PUBDEF_GRP	dd 0
PUBDEF_SEG	dd 0
PUBDEF_FRAME	dd 0
;
SYMSpace	db (size SymbolStruc)+256 dup (0)
;
PharlapFlag	db 0
FixupNumber	dd 0
AutoDSGROUP	dd 0
;
ThreadIndex	dd 0
ThreadTargetMethod db 4 dup (0)
ThreadFrameMethod db 4 dup (0)
ThreadTargetIndex dd 4 dup (0)
ThreadFrameIndex dd 4 dup (0)
;
MapFileSegHeader db " Start     Length    Name                Class           Group           Count",13,10,0
MapFileProgName db 13,10,"  PROGRAM: ",0
CarriageReturn	db 13,10,0
QuoteString	db '"',0
SpaceString	db " ",0
ParenOpenString db "(",0
ParenCloseString db ")",0
GreaterString	db ">",0
NoNameString	db "name not available",0
;
SegClassCODE	db 4,"CODE"
SegClassBEGDATA db 7,"BEGDATA"
SegClassDATA	db 4,"DATA"
SegClassOTHER	db 1,"*"
SegClassBSS	db 3,"BSS"
SegClassSTACK	db 5,"STACK"
;
SegNameBEGTEXT	db 7,"BEGTEXT"
;
InternalPUBDEF__end PUB <0, 0, 0, 0, 0, 0, InternalPUBDEF_T__end>
InternalPUBDEF_T__end db 4,"_end"
InternalPUBDEF__edata PUB <0, 0, 0, 0, 0, 0, InternalPUBDEF_T__edata>
InternalPUBDEF_T__edata db 6,"_edata"
InternalPUBDEF__estack PUB <0, 0, 0, 0, 0, 0, InternalPUBDEF_T__estack>
InternalPUBDEF_T__estack db 7,"_estack"
;
DummyPUB	PUB < 0, 0, 0, 0, 0, 0, DummyPUBName>
DummyPUBName	db 20,"un-resolved external"
;
LIBText	db	"LIB="
LIBLen	equ	$-LIBText
LIBPaths	dd 0,0
LIBFileNameSpace db 256 dup (0)
;
NEARModel	db 0
CommandLine	dd 0
;
SymbolObjBase	dd 0
SymbolObjPos	dd 0
SymbolObjHead	dd 0
;
ExtSearchScope	dd 0
ExtSearchObj	dd 0
;
ObjPassOneTable label dword
	dd 80h dup (ObjPassOneBad)	;000h-07Fh
	dd ObjPassOne_THEADR	;080h
	dd 01h dup (ObjPassOneBad)	;081h
	dd ObjPassOne_THEADR	;082h
	dd 05h dup (ObjPassOneBad)	;083h-087h
	dd ObjPassOne_COMENT	;088h
	dd 01h dup (ObjPassOneBad)	;089h
	dd ObjPassOne_MODEND	;08Ah
	dd ObjPassOne_MODEND	;08Bh
	dd ObjPassOne_EXTDEF	;08Ch
	dd 03h dup (ObjPassOneBad)	;08Dh-08F
	dd ObjPassOne_PUBDEF	;090h
	dd ObjPassOne_PUBDEF	;091h
	dd 02h dup (ObjPassOneBad)	;092h-093h
	dd ObjPassOneNext		;094h
	dd ObjPassOneNext		;095h
	dd ObjPassOne_LNAMES	;096h
	dd 01h dup (ObjPassOneBad)	;097h
	dd ObjPassOne_SEGDEF	;098h
	dd ObjPassOne_SEGDEF	;099h
	dd ObjPassOne_GRPDEF	;09Ah
	dd 01h dup (ObjPassOneBad)	;09Bh
	dd ObjPassOneNext		;09Ch
	dd ObjPassOneNext		;09Dh
	dd 02h dup (ObjPassOneBad)	;09Eh-09Fh
	dd ObjPassOneNext		;0A0h
	dd ObjPassOneNext		;0A1h
	dd ObjPassOneNext		;0A2h
	dd ObjPassOneNext		;0A3h
	dd 10h dup (ObjPassOneBad)	;0A4h-0B3h
	dd ObjPassOne_EXTDEF	;0B4h
	dd 01h dup (ObjPassOneBad)	;0B5h
	dd ObjPassOne_PUBDEF	;0B6h
	dd ObjPassOne_PUBDEF	;0B7h
	dd 48h dup (ObjPassOneBad)	;0B8h-0FFh
;
ObjPassTwoTable label dword
	dd 80h dup (ObjPassTwoBad)	;000h-07Fh
	dd ObjPassTwo_THEADR	;080h
	dd 01h dup (ObjPassTwoBad)	;081h
	dd ObjPassTwo_THEADR	;082h
	dd 05h dup (ObjPassTwoBad)	;083h-087h
	dd ObjPassTwo_COMENT	;088h
	dd 01h dup (ObjPassTwoBad)	;089h
	dd ObjPassTwo_MODEND	;08Ah
	dd ObjPassTwo_MODEND	;08Bh
	dd ObjPassTwoNext		;08Ch
	dd 03h dup (ObjPassTwoBad)	;08Dh-08F
	dd ObjPassTwoNext		;090h
	dd ObjPassTwoNext		;091h
	dd 02h dup (ObjPassTwoBad)	;092h-093h
	dd ObjPassTwo_LINNUM	;094h
	dd ObjPassTwo_LINNUM	;095h
	dd ObjPassTwoNext		;096h
	dd 01h dup (ObjPassTwoBad)	;097h
	dd ObjPassTwoNext		;098h
	dd ObjPassTwoNext		;099h
	dd ObjPassTwoNext		;09Ah
	dd 01h dup (ObjPassTwoBad)	;09Bh
	dd ObjPassTwo_FIXUPP	;09Ch
	dd ObjPassTwo_FIXUPP	;09Dh
	dd 02h dup (ObjPassTwoBad)	;09Eh-09Fh
	dd ObjPassTwo_LEDATA	;0A0h
	dd ObjPassTwo_LEDATA	;0A1h
	dd ObjPassTwo_LIDATA	;0A2h
	dd ObjPassTwo_LIDATA	;0A3h
	dd 10h dup (ObjPassTwoBad)	;0A4h-0B3h
	dd ObjPassTwoNext		;0B4h
	dd 01h dup (ObjPassTwoBad)	;0B5h
	dd ObjPassTwoNext		;0B6h
	dd ObjPassTwoNext		;0B7h
	dd 48h dup (ObjPassTwoBad)	;0B8h-0FFh
;
IsNEAR	dd 0
DummyLINNUM	db 0
LINNUMSymbolAddress dd 0
PUBDEFObj	dd 0
PUBDEFScope	dd 0
FixupExternName dd 0
ObjPassTwoModule dd 0
;
HexTable	db "0123456789ABCDEF"
;
OptionSwitches	dd OptionSwitch0,OptionSwitch1,OptionSwitch2,OptionSwitch3
	dd OptionSwitch4,OptionSwitch5,OptionSwitch5b,OptionSwitch6
	dd OptionSwitch6b,OptionSwitch7,OptionSwitch8a,OptionSwitch8b
	dd OptionSwitch9a
	dd -1
;
OptionSwitch0	dd ParaAlignONText,SetOptionNoneZero,ParaAlign,0
ParaAlignONText db "ALP",0
ParaAlign	dd 0
;
OptionSwitch1	dd CaseSensitiveOFFText,SetOptionNoneZero,CaseSensitive,0
CaseSensitiveOFFText db "NOI",0
CaseSensitive	dd 0
LocalCaseSensitive dd 0
;
OptionSwitch2	dd FlatSegmentsONText,SetOptionNoneZero,FlatSegments,0
FlatSegmentsONText db "FLAT",0
FlatSegments	dd 0
;
OptionSwitch9a	dd NoFlatCodeText,SetOptionNoneZero,NoFlatCode,0
NoFlatCodeText	db "NFC",0
NoFlatCode	dd 0
;
OptionSwitch3	dd StubLoaderOFFText,SetOptionZero,StubName,0
StubLoaderOFFText db "NOSTUB",0
StubName	dd ExecutionName
;
OptionSwitch4	dd ForceStackOFFText,SetOptionNoneZero,ForcedStack,0
ForceStackOFFText db "NOSTACK",0
ForcedStack	dd 0
;
OptionSwitch5	dd WriteSYMsText,SetOptionNoneZero,WantSYMs,0
OptionSwitch5b	dd WriteSYMsTextb,SetOptionNoneZero,WantSYMs,0
WriteSYMsText	db "SYM",0
WriteSYMsTextb	db "SY",0
WantSYMs	dd 0
;
OptionSwitch6	dd WriteMAPText,SetOptionNoneZero,WantMAP,0
OptionSwitch6b	dd WriteMAPTextb,SetOptionNoneZero,WantMAP,0
WriteMAPText	db "MAP",0
WriteMAPTextb	db "M",0
WantMAP	dd 0
;
OptionSwitch7	dd NoDefaultLIBText,SetOptionZero,DefaultLibraries,0
NoDefaultLIBText db "NOD",0
DefaultLibraries dd -1
;
OptionSwitch8a	dd SetStackTexta,SetOptionValue,AutoStackSize,0
OptionSwitch8b	dd SetStackTextb,SetOptionValue,AutoStackSize,0
SetStackTexta	db "ST",0
SetStackTextb	db "STACK",0
;
DOSSegSegmentOrder dd 0
CHKSums	dd 0
;
PhysicalMemError db "Not enough PHYSICAL memory free to run this program.",13,10,"$"
SelectorError	db "Not enough selectors available from host.",13,10,"$"
CodeSegAlias	dw ?
ZeroSelector	dw ?
PSPSegment	dw ?
ENVSegment	dw ?
DataLinearBase	dd ?
ExecutionPath	label byte
	db 256 dup (0)
ExecutionName	db 256 dup (0)
OldInt24	df 0
;
ResponceCOM	dd 0
ResponceLEN	dd 0
ResponceCNT	dd 0
;
FLAT_LNAME	db 4,"FLAT"
;
SYMIDString	db "CWDSYM02"
SYMOffset	dd 8,0
;
FileBufferCount dd 0
FileBufferHandle dd 0
FileBufferPosition dd 0
FileBuffer	db 1024 dup (?)
;
DEFLineBuffer	db 1024 dup (?)
;
DEFCommands	dd DEF_ExportTXT,DEF_ExportCOM,0,0
	dd DEF_ImportTXT,DEF_ImportCOM,0,0
	dd DEF_ImpTXT,DEF_ImportCOM,0,0
	dd DEF_NameTXT,DEF_NameCOM,0,0
	dd DEF_IncludeTXT,DEF_IncludeCOM,0,0
	dd 0
;
DEF_ExportTXT	db "EXPORT",0
DEF_ImportTXT	db "IMPORT",0
DEF_ImpTXT	db "IMP",0
DEF_NameTXT	db "NAME",0
DEF_IncludeTXT	db "INCLUDE",0
;
DEFLineNumber	dd 0
;
IMPORTTemp	dd 0
IMPORTSpace	db 256 dup  (0)
IMPORTLength	dd 0,0,0
IMPORTInternal	dd 0,0
;
ModuleName	dd 0


	end	Start

