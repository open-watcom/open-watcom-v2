	.code


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Initialise disasembly stuff.
;
InitDisas	proc	near
	mov	SymbolList,ebx
	sys	GetSel
	mov	DisasTempSel,bx
	ret
InitDisas	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Generate text disasembly of data.
;
;On Entry:
;
;FS:ESI	Instruction buffer to disasemble.
;ES:EDI	Buffer to produce text in.
;
;On Exit:
;
;FS:ESI	Points to next instruction.
;
;AX	- Instruction type. Types in disas.inc
;
Disasemble	proc	near
	push	fs
	mov	RealFS,fs
	mov	dTargetCS2,0
	;
	pushm	esi,edi,es,fs
	mov	bx,fs
	sys	GetSelDet
	mov	bx,DisasTempSel
	mov	si,-1
	mov	di,si
	sys	SetSelDet
	popm	esi,edi,es,fs
	;
	mov	fs,DisasTempSel
	mov	Use32Bit,0
	xor	eax,eax
	mov	ax,RealFS
	lar	eax,eax
	test	eax,00400000h
	jz	@@Use32It
	mov	Use32Bit,3
@@Use32It:	;
	push	edi
	mov	_D_Source,esi
	mov	al,' '
	mov	ecx,24
	rep	stosb
	pop	edi
	;
	;Print the address (as an offset).
	;
	push	edi
	mov	eax,esi
	mov	ecx,8
	test	Use32Bit,1
	jnz	@@BigAddr
	mov	ecx,4
@@BigAddr:	call	Bin2Hex
	mov	es:b[edi],' '
	inc	edi
	mov	_D_Destination,edi
	pop	edi
	add	edi,24
	mov	_D_Destination+4,edi
	dec	_D_Destination+4
	;
	pushm	esi,edi
	push	esi
	mov	bx,DisasTempSel
	sys	GetSelDet
	shl	ecx,16
	mov	cx,dx
	pop	esi
	add	esi,ecx
	mov	ax,0fffch
	int	31h
	jc	@@BadAddr
	add	esi,14
	int	31h
	jnc	@@AddrOK
@@BadAddr:	popm	esi,edi
	inc	esi
	push	esi
	mov	esi,offset BadAddrText
@@BadCopy:	movsb
	cmp	b[esi-1],0
	jnz	@@BadCopy
	pop	esi
	mov	ReturnCode,RetCode_INV
	jmp	@@DisDone
@@AddrOK:	popm	esi,edi
	;
	mov	eaMode,0
	mov	eaOffset,0
	mov	eaSeg,0
	mov	eaReg,0
	mov	eaIndex,0
	mov	eaScale,0
	;
	mov	ReturnCode,0
	mov	SegOver_T,0
	mov	al,Use32Bit
	mov	In32Bit,al
	mov	FWait_Waiting,0
@@OpCodeLoop:	mov	AnotherOpCode,0
	;
	mov	ebp,offset InstTab
	movzx	eax,fs:b[esi]	;Get op-code.
	inc	esi
	;
@@NewTable:	mov	AnotherTable,0
	mov	PointType,0
	mov	bx,size xxh_struc
	mul	bx
	shl	eax,1		;2 entries per table.
	add	ebp,eax		;index into the table.
	;
	test	In32Bit,1
	jz	@@no32
	add	ebp,size xxh_struc	;assume 32 bit for now.
@@no32:	push	esi
	mov	esi,ds:xxh_text1[ebp]	;point to first text string.
@@0:	movsb
	cmp	b[esi-1],0		;end of string?
	jnz	@@0
	dec	edi
	pop	esi
	;
	push	esi
	mov	esi,ds:xxh_text2[ebp]	;point to second text string.
@@1:	movsb
	cmp	b[esi-1],0		;end of string?
	jnz	@@1
	dec	edi
	pop	esi
	;
	mov	eax,ds:xxh_extended[ebp]
	mov	PointType,eax
	;
	call	ds:xxh_routine[ebp]
	;
	cmp	AnotherTable,0
	jnz	@@NewTable
	cmp	AnotherOpCode,0
	jnz	@@OpCodeLoop		;fetch next one.
	;
	pushm	esi,edi
	mov	ecx,esi
	sub	ecx,_D_Source
	mov	esi,_D_Source
	mov	edi,_D_Destination
@@2:	mov	al,fs:[esi]
	inc	esi
	cmp	edi,_D_Destination+4
	jnc	@@3
	push	ecx
	mov	ecx,2
	call	Bin2Hex
	pop	ecx
	loop	@@2
	mov	es:b[edi],' '
@@3:	popm	esi,edi
	;
@@DisDone:	mov	ax,ReturnCode
	movzx	ebx,eaMode
	mov	cx,dTargetCS
	mov	edx,dTargetEIP
	mov	gs,dTargetCS2
	mov	ebp,dTargetEIP2
	;
@@9:	pop	fs
	ret
Disasemble	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Look for value as a symbol, outputting symbol text if found or hex value if not.
;
;On Entry:-
;
;EAX	- Value.
;DX	- Segment (0=none)
;ECX	- Digits.
;ES:EDI	- Output buffer.
;
SymbolBin2Hex	proc	near
	pushm	eax,ebx,ecx,edx,esi,ebp
	push	esi
	call	Bin2Symbol
	jc	@@9
	rep	movsb
	pop	esi
	jmp	@@8
	;
@@9:	pop	esi
	call	Bin2Hex
@@8:	popm	eax,ebx,ecx,edx,esi,ebp
	ret
SymbolBin2Hex	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Bin2Symbol	proc	near
	cmp	SymbolTranslate,0
	jz	@@9
	;
	mov	esi,SymbolList
@@0:	cmp	d[esi],-1		;end of the list?
	jz	@@9
	push	esi
	cmp	SymbolType[esi],255
	jz	@@1
	cmp	eax,SymbolDWord[esi]	;right value?
	jnz	@@1
	or	dx,dx
	jz	@@2
	cmp	dx,SymbolSeg[esi]
	jnz	@@1
@@2:	add	esi,SymbolTLen
	movzx	ecx,b[esi]		;get text length.
	inc	esi
	pop	eax
	clc
	ret
	;
@@1:	pop	esi
	add	esi,SymbolNext[esi]
	jmp	@@0
	;
@@9:	stc
	ret
Bin2Symbol	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Bin2Hex	proc	near
;
;Convert number into ASCII Hex version.
;
;On Entry:-
;
;EAX	- Number to convert.
;ECX	- Digits to do.
;ES:EDI	- Buffer to put string in.
;
	pushm	edi,ecx,ebx,edx
	lea	ebx,HexTable
	add	edi,ecx
	dec	edi
	mov	edx,eax
@@0:	mov	al,dl
	shr	edx,4
	and	al,15
	xlat
	mov	es:[edi],al
	dec	edi
	loop	@@0
	popm	edi,ecx,ebx,edx
	add	edi,ecx
	ret
Bin2Hex	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RETWORD	proc	near
	mov	ReturnCode,RetCode_RET
	ret
RETWORD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RETDWORD	proc	near
	mov	ReturnCode,RetCode_RET32
	ret
RETDWORD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RETDWORD2	proc	near
	mov	ReturnCode,RetCode_RETF
	ret
RETDWORD2	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
W_IRET	proc	near
	mov	ReturnCode,RetCode_IRET
	ret
W_IRET	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
D_IRET	proc	near
	mov	ReturnCode,RetCode_IRET32
	ret
D_IRET	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RETFWORD	proc	near
	mov	ReturnCode,RetCode_RETF32
	ret
RETFWORD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMREGB	proc	near
	mov	eaMode,1
	call	get_modregrm
	lea	edx,T_ByteRegs
	mov	al,0
	call	outrm2
	ret
RMREGB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMREGW	proc	near
	mov	eaMode,2
	call	get_modregrm
	lea	edx,T_WordRegs
	mov	al,1
	call	outrm2
	ret
RMREGW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMREGD	proc	near
	mov	eaMode,4
	call	get_modregrm
	lea	edx,T_DWordRegs
	mov	al,2
	call	outrm2
	ret
RMREGD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
CRMREGD	proc	near
	call	get_modregrm
	lea	edx,T_CDWordRegs
	mov	al,2
	call	outrm2
	ret
CRMREGD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DRMREGD	proc	near
	call	get_modregrm
	lea	edx,T_DDWordRegs
	mov	al,2
	call	outrm2
	ret
DRMREGD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
TRMREGD	proc	near
	call	get_modregrm
	lea	edx,T_TDWordRegs
	mov	al,2
	call	outrm2
	ret
TRMREGD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
REGRMB	proc	near
	mov	eaMode,1
	call	get_modregrm
	lea	edx,T_ByteRegs
	mov	al,0
	call	outrb2
	ret
REGRMB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
REGRMW	proc	near
	mov	eaMode,2
	call	get_modregrm
	lea	edx,T_WordRegs
	mov	al,1
	call	outrb2
	ret
REGRMW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
REGRMSW	proc	near
	mov	eaMode,5
	call	get_modregrm
	lea	edx,T_WordRegs
	mov	al,1
	call	outrb2
	ret
REGRMSW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
MOVZXDW	proc	near
	mov	eaMode,2
	call	get_modregrm
	lea	edx,T_DWordRegs
	mov	al,1
	call	outrb2
	ret
MOVZXDW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
MOVZXDB	proc	near
	mov	eaMode,1
	call	get_modregrm
	lea	edx,T_DWordRegs
	mov	al,0
	call	outrb2
	ret
MOVZXDB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
MOVZXWB	proc	near
	mov	eaMode,1
	call	get_modregrm
	lea	edx,T_WordRegs
	mov	al,0
	call	outrb2
	ret
MOVZXWB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
REGRMD	proc	near
	mov	eaMode,4
	call	get_modregrm
	lea	edx,T_DWordRegs
	mov	al,2
	call	outrb2
	ret
REGRMD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
REGRMSF	proc	near
	mov	eaMode,6
	call	get_modregrm
	lea	edx,T_DWordRegs
	mov	al,2
	call	outrb2
	ret
REGRMSF	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
CREGRMD	proc	near
	call	get_modregrm
	lea	edx,T_CDWordRegs
	mov	al,2
	call	outrb2
	ret
CREGRMD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DREGRMD	proc	near
	call	get_modregrm
	lea	edx,T_DDWordRegs
	mov	al,2
	call	outrb2
	ret
DREGRMD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
TREGRMD	proc	near
	call	get_modregrm
	lea	edx,T_TDWordRegs
	mov	al,2
	call	outrb2
	ret
TREGRMD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMB	proc	near
	mov	eaMode,1
	call	get_modregrm
	lea	edx,T_ByteRegs
	mov	al,0
	call	out_ea
	ret
RMB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMW	proc	near
	mov	eaMode,2
	call	get_modregrm
	lea	edx,T_WordRegs
	mov	al,1
	call	out_ea
	ret
RMW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMWF	proc	near
	mov	eaMode,6
	call	get_modregrm
	lea	edx,T_ByteRegs
	mov	al,0
	call	out_ea
	ret
RMWF	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMD	proc	near
	mov	eaMode,4
	call	get_modregrm
	lea	edx,T_ByteRegs
	mov	al,2
	call	out_ea
	ret
RMD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHLDIMBW	proc	near
	call	get_modregrm
	lea	edx,T_WordRegs
	mov	al,1
	call	outrm2
	mov	al,0
	call	out_imm
	ret
SHLDIMBW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHLDIMBD	proc	near
	call	get_modregrm
	lea	edx,T_DWordRegs
	mov	al,2
	call	outrm2
	mov	al,0
	call	out_imm
	ret
SHLDIMBD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHLDCLW	proc	near
	call	get_modregrm
	lea	edx,T_WordRegs
	mov	al,1
	call	outrm2
	mov	es:b[edi],','
	inc	edi
	push	esi
	lea	esi,T_CL
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	pop	esi
	ret
SHLDCLW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHLDCLD	proc	near
	call	get_modregrm
	lea	edx,T_DWordRegs
	mov	al,2
	call	outrm2
	mov	es:b[edi],','
	inc	edi
	push	esi
	lea	esi,T_CL
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	pop	esi
	ret
SHLDCLD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
GvEvIvW	proc	near
	call	get_modregrm
	lea	edx,T_WordRegs
	mov	al,1
	call	outrm2
	mov	al,1
	call	out_imm
	ret
GvEvIvW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
GvEvIbW	proc	near
	call	get_modregrm
	lea	edx,T_WordRegs
	mov	al,1
	call	outrm3
	mov	al,0
	call	out_imm
	ret
GvEvIbW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
GvEvIvD	proc	near
	call	get_modregrm
	lea	edx,T_DWordRegs
	mov	al,2
	call	outrm2
	mov	al,2
	call	out_imm
	ret
GvEvIvD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
GvEvIbD	proc	near
	call	get_modregrm
	lea	edx,T_DWordRegs
	mov	al,2
	call	outrm3
	mov	al,0
	call	out_imm
	ret
GvEvIbD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
out_imm	proc	near
	cmp	al,2
	jz	out_immdword
	cmp	al,0
	jnz	out_immword
	jmp	immbyte
out_imm	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
out_imms	proc	near
	cmp	al,2
	jz	out_immdword
	cmp	al,0
	jnz	out_immword
	jmp	out_immbytes
out_imms	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
immbyte	proc	near
	jmp	out_immbyte
immbyte	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
out_immbyte	proc	near
	mov	al,','
	stosb
	jmp	immbyte2
out_immbyte	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
out_immbytes	proc	near
	mov	al,','
	stosb
	jmp	immbyte2s
out_immbytes	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SetInt	proc	near
	mov	ReturnCode,RetCode_INT
	ret
SetInt	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SetInt2	proc	near
	call	immbyte2
	call	SetInt
	ret
SetInt2	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
immbyte2	proc	near
	mov	al,fs:[esi]
	inc	esi
	mov	ecx,2
	mov	dx,0
	call	SymbolBin2Hex
	mov	es:b[edi],0
	ret
immbyte2	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
immbyte2s	proc	near
	movsx	eax,fs:b[esi]
	inc	esi
	mov	ecx,8
	mov	dx,0
	call	SymbolBin2Hex
	mov	es:b[edi],0
	ret
immbyte2s	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
LodsSegb	proc	near
	mov	eaMode,1
	jmp	LodsSeg
LodsSegb	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
LodsSegw	proc	near
	mov	eaMode,2
	jmp	LodsSeg
LodsSegw	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
LodsSegd	proc	near
	mov	eaMode,4
	jmp	LodsSeg
LodsSegd	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
LodsSeg	proc	near
	or	eaMode,128
	mov	eaReg,6+1	;SI
	mov	eaSeg,0
	test	Use32Bit,2
	jz	@@16_0
	or	eaReg,128
@@16_0:	call	ea2Real
	mov	dTargetEIP,eax	;setup for execution.
	mov	dTargetCS,cx
	;
	cmp	SegOver_T,0
	jz	@@0
	pushm	eax,esi
	mov	esi,SegOver_T
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	dec	edi
	mov	al,':'
	stosb
	mov	es:b[edi],0
	popm	eax,esi
@@0:	ret
LodsSeg	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
stosSegb	proc	near
	mov	eaMode,1
	jmp	stosSeg
stosSegb	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
stosSegw	proc	near
	mov	eaMode,2
	jmp	stosSeg
stosSegw	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
stosSegd	proc	near
	mov	eaMode,4
	jmp	stosSeg
stosSegd	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
stosSeg	proc	near
	or	eaMode,128
	mov	eaReg,7+1	;DI
	mov	eaSeg,0+1
	test	Use32Bit,2
	jz	@@16_0
	or	eaReg,128
@@16_0:	call	ea2Real
	mov	dTargetEIP,eax	;setup for execution.
	mov	dTargetCS,cx
	ret
stosSeg	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
movsSegb	proc	near
	mov	eaMode,1
	jmp	movsSeg
movsSegb	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
movsSegw	proc	near
	mov	eaMode,2
	jmp	movsSeg
movsSegw	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
movsSegd	proc	near
	mov	eaMode,4
	jmp	movsSeg
movsSegd	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
movsSeg	proc	near
	mov	ReturnCode,RetCode_MOVS
	;
	or	eaMode,128
	mov	al,eaSeg
	push	eax
	mov	eaSeg,1
	mov	eaReg,7+1	;DI
	test	Use32Bit,2
	jz	@@16_0
	or	eaReg,128
@@16_0:	call	ea2Real
	mov	dTargetEIP2,eax	;setup for execution.
	mov	dTargetCS2,cx
	pop	eax
	mov	eaSeg,al
	;
	cmp	eaSeg,0
	jnz	@@16_2
	mov	eaSeg,1
@@16_2:	mov	eaReg,6+1	;SI
	test	Use32Bit,2
	jz	@@16_1
	or	eaReg,128
@@16_1:	call	ea2Real
	mov	dTargetEIP,eax	;setup for execution.
	mov	dTargetCS,cx
	;
	cmp	SegOver_T,0
	jz	@@0
	pushm	eax,esi
	mov	esi,SegOver_T
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	dec	edi
	mov	al,':'
	stosb
	mov	es:b[edi],0
	popm	eax,esi
@@0:	ret
movsSeg	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
IMMWORD	proc	near
	jmp	out_immword
IMMWORD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
out_immword	proc	near
	mov al,','
	stosb
	jmp	immword2
out_immword	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
W_RETFnn	proc	near
	call	immword2
	mov	ReturnCode,RetCode_RETFnn
	ret
W_RETFnn	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
W_RETnn	proc	near
	call	immword2
	mov	ReturnCode,RetCode_RETnn
	ret
W_RETnn	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
immword2	proc	near
	movzx	eax,fs:w[esi]
	mov	eaOffset,eax
	add	esi,2
	mov	ecx,4
	mov	dx,0
	call	SymbolBin2Hex
	mov	es:b[edi],0
	ret
immword2	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
IMMDWORD	proc	near
	jmp	out_immdword
IMMDWORD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
out_immdword	proc	near
	mov al,','
	stosb
	jmp	immdword2
out_immdword	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
D_RETnn	proc	near
	call	immword2
	mov	ReturnCode,RetCode_RETnn32
	ret
D_RETnn	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
D_RETFnn	proc	near
	call	immword2
	mov	ReturnCode,RetCode_RETFnn32
	ret
D_RETFnn	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
F_RETFnn	proc	near
	call	immword2
	mov	ReturnCode,RetCode_RETFnn32
	ret
F_RETFnn	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
immdword2	proc	near
	mov	eax,fs:[esi]
	mov	eaOffset,eax
	add	esi,4
	mov	ecx,8
	mov	dx,0
	call	SymbolBin2Hex
	mov	es:b[edi],0
	ret
immdword2	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
IMMWORDBYTE	proc	near
	mov	ax,fs:[esi]
	add	esi,2
	mov	ecx,4
	mov	dx,0
	call	SymbolBin2Hex
	mov al,','
	stosb
	mov	al,fs:[esi]
	inc	esi
	mov	ecx,2
	mov	dx,0
	call	SymbolBin2Hex
	mov	es:b[edi],0
	ret
IMMWORDBYTE	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
NONE	proc	near
	ret
NONE	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SEGOVER	proc	near
	push	esi
	movzx	esi,fs:b[esi-1]
	shr	esi,3
	and	esi,3

	push	ax
	mov	ax,si
	mov	eaSeg,al
	inc	eaSeg
	pop	ax

	shl	esi,2
	add	esi,offset T_SegRegs
	mov	esi,[esi]
	mov	SegOver_T,esi
	pop	esi
	mov	AnotherOpCode,1
	ret
SEGOVER	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SegOverFS	proc	near
	mov	eax,offset T_FS
	mov	SegOver_T,eax
	mov	eaSeg,4+1
	mov	AnotherOpCode,1
	ret
SegOverFS	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SegOverGS	proc	near
	mov	eax,offset T_GS
	mov	SegOver_T,eax
	mov	eaSeg,5+1
	mov	AnotherOpCode,1
	ret
SegOverGS	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DoOpsize	proc	near
	mov	ah,In32Bit
	and	ah,255-1
	mov	al,Use32Bit
	and	al,1
	xor	al,1
	or	al,ah
	mov	In32Bit,al
	mov	AnotherOpCode,1
	ret
DoOpsize	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DoAddsize	proc	near
	mov	ah,In32Bit
	and	ah,255-2
	mov	al,Use32Bit
	and	al,2
	xor	al,2
	or	al,ah
	mov	In32Bit,al
	mov	AnotherOpCode,1
	ret
DoAddsize	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DODISPB_C	proc	near
	call	DODISPB
	mov	ReturnCode,RetCode_CALL
	ret
DODISPB_C	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DODISPB_J	proc	near
	call	DODISPB
	mov	ReturnCode,RetCode_JMP
	ret
DODISPB_J	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DODISPB	proc	near
	movsx	eax,fs:b[esi]
	inc	esi
	add	eax,esi
	mov	ReturnCode,RetCode_Jcc
	mov	dTargetEIP,eax
	mov	ecx,4
	test	In32Bit,1
	jz	@@0
	mov	ecx,8
@@0:	mov	dx,RealFS
	mov	dTargetCS,dx
	call	SymbolBin2Hex
	mov	es:b[edi],0
	ret
DODISPB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DODISPW_C	proc	near
	call	DODISPW
	mov	ReturnCode,RetCode_CALL
	ret
DODISPW_C	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DODISPW_J	proc	near
	call	DODISPW
	mov	ReturnCode,RetCode_JMP
	ret
DODISPW_J	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DODISPW	proc	near
	mov	eax,esi
	mov	ax,fs:w[esi]
	add	esi,2
	add	ax,si
	mov	dTargetEIP,eax
	mov	ReturnCode,RetCode_Jcc
	mov	ecx,4
	mov	dx,RealFS
	mov	dTargetCS,dx
	call	SymbolBin2Hex
	mov	es:b[edi],0
	ret
DODISPW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DODISPD_C	proc	near
	call	DODISPD
	mov	ReturnCode,RetCode_CALL
	ret
DODISPD_C	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DODISPD_J	proc	near
	call	DODISPD
	mov	ReturnCode,RetCode_JMP
	ret
DODISPD_J	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DODISPD	proc	near
	mov	eax,fs:d[esi]
	add	esi,4
	add	eax,esi
	mov	dTargetEIP,eax
	mov	ReturnCode,RetCode_Jcc
	mov	ecx,8
	mov	dx,RealFS
	mov	dTargetCS,dx
	call	SymbolBin2Hex
	mov	es:b[edi],0
	ret
DODISPD	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
EXTINST	proc	near
	mov	ebp,ds:xxh_extended[ebp]	;get other table address.
	movzx	eax,fs:b[esi]
	mov	ch,al
	mov	bl,al
	mov	cl,6
	shr	ch,cl
	and	ch,3
	and	bl,7
	xor	ah,ah
	and	al,38h
	shr	eax,3
	mov	AnotherTable,1
	ret
EXTINST	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
LoseByte	proc	near
	inc	esi
	ret
LoseByte	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
Invalid	proc	near
	mov	ReturnCode,Retcode_INV
	ret
Invalid	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ExtInst0F	proc	near
	mov	ebp,ds:xxh_extended[ebp]	;get other table address.
	movzx	eax,fs:b[esi]
	shr	eax,4		;lose low nibble.
	mov	ebp,ds:[ebp+eax*4]	;get new table address.
	or	ebp,ebp		;anything there?
	jnz	@@0
	;
	push	esi
	mov	esi,offset T_INV	;better do something.
@@1:	movsb
	cmp	b[esi-1],0
	jnz	@@1
	mov	es:b[edi],0
	pop	esi
	inc	esi		;skip this byte.
	mov	ReturnCode,Retcode_INV
	ret
	;
@@0:	movzx	eax,fs:b[esi]
	and	eax,0fh		;make sure only lower nibble counts.
	inc	esi
	mov	AnotherTable,1
	ret
ExtInst0F	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SEGRM	proc	near
	mov	eaMode,2
	call	get_modregrm
	push	esi
	and	bh,7
	movzx esi,bh
	cmp	bh,2		;SS?
	jnz	@@NotSS
	mov	ReturnCode,RetCode_SS	;Flag skip next as well.
@@NotSS:	shl	esi,2
	add	esi,offset T_SegRegs
	mov	esi,[esi]
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	mov	es:b[edi-1],','
	mov	es:b[edi],0
	pop	esi
	mov al,1
	call	out_ea
	ret
SEGRM	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
INTERAD_J	proc	near
	call	do_interseg
	mov	ReturnCode,RetCode_JMP
	ret
interad_J	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
INTERAD_C	proc	near
	call	do_interseg
	mov	ReturnCode,RetCode_CALL
	ret
interad_C	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
do_interseg	proc	near
	test	In32Bit,1
	jnz	@@Use32_0
	movzx	eax,fs:w[esi]	;Get segment.
	mov	bx,fs:[esi+2]
	add	esi,4
	jmp	@@0
@@Use32_0:	mov	eax,fs:[esi+0]
	mov	bx,fs:[esi+4]
	add	esi,6
@@0:	;
	mov	dTargetEIP,eax
	mov	dTargetCS,bx
	;
	pushm	eax,ebx
	mov	ax,bx
	mov	ecx,4
	call	Bin2Hex
	mov	al,':'
	stosb
	popm	eax,ebx
	mov	ecx,4
	test	In32Bit,1
	jz	@@Use32_1
	mov	ecx,8
@@Use32_1:	mov	dx,bx
	call	SymbolBin2Hex
	mov	es:b[edi],0
	ret
do_interseg	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
MEMACB	proc	near
	mov	eaMode,1
	mov	al,','
	stosb
	call	outmem
	ret
MEMACB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
MEMACW	proc	near
	mov	eaMode,2
	mov	al,','
	stosb
	call	outmem
	ret
MEMACW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
MEMACD	proc	near
	mov	eaMode,4
	mov	al,','
	stosb
	call	outmem
	ret
MEMACD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ACMEMB	proc	near
	mov	eaMode,1
	call	outmem
	mov	al,','
	stosb
	push	esi
	lea	esi,T_AL
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	pop	esi
	ret
ACMEMB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ACMEMW	proc	near
	mov	eaMode,2
	call	outmem
	mov	al,','
	stosb
	push	esi
	lea	esi,T_AX
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	pop	esi
	ret
ACMEMW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ACMEMD	proc	near
	mov	eaMode,4
	call	outmem
	mov	al,','
	stosb
	push	esi
	lea	esi,T_EAX
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	pop	esi
	ret
ACMEMD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
PORTIN	proc	near
	call	SetInOut
	call	IMMBYTE
	ret
PORTIN	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
PORTOUTB	proc	near
	call	SetInOut
	lea	eax,T_AL
	call	pow2
	ret
PORTOUTB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
PORTOUTW	proc	near
	call	SetInOut
	lea	eax,T_AX
	test	In32Bit,1
	jz	pow2
	mov	eax,offset T_EAX
	jmp	pow2
PORTOUTW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
pow2	proc	near
	push eax
	call immbyte2
	mov al,','
	stosb
	pop eax
	push	esi
	mov	esi,eax
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	pop	esi
	ret
pow2	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ANOTHER	proc	near
	mov	AnotherOpCode,1
	ret
ANOTHER	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
UmREP	proc	near
	mov	al,'e'
	mov	ebx,esi
	cmp	fs:b[esi],66h
	jz	@@1
	cmp	fs:b[esi],67h
	jnz	@@2
@@1:	inc	ebx
@@2:	cmp	fs:b[ebx],0a6h
	jz	@@0
	cmp	fs:b[ebx],0a7h
	jz	@@0
	cmp	fs:b[ebx],0aeh
	jz	@@0
	cmp	fs:b[ebx],0afh
	jz	@@0
	mov	al,' '
@@0:	stosb
	mov	al,' '
	stosb
	stosb
	mov	es:b[edi],0
	mov	AnotherOpCode,1
	ret
UmREP	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMIMMBN	proc	near
	mov	eaMode,1
	call	get_modregrm
	mov	al,0
	call	out_ea
	mov	al,0
	call	out_imm
	ret
RMIMMBN	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMIMMB	proc	near
	mov	eaMode,1
	call	get_modregrm
	mov	al,0
	call	out_ea
	mov	al,0
	call	out_imm
	ret
RMIMMB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMIMMWN	proc	near
	mov	eaMode,2
	call	get_modregrm
	mov	al,1
	call	out_ea
	mov	al,1
	call	out_imm
	ret
RMIMMWN	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMIMMDN	proc	near
	mov	eaMode,4
	call	get_modregrm
	mov	al,2
	call	out_ea
	mov	al,2
	call	out_imm
	ret
RMIMMDN	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMIMMW	proc	near
	mov	eaMode,2
	call	get_modregrm
	mov	al,1
	call	out_ea
	mov	al,1
	call	out_imm
	ret
RMIMMW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMIMMD	proc	near
	mov	eaMode,4
	call	get_modregrm
	mov	al,2
	call	out_ea
	mov	al,2
	call	out_imms
	ret
RMIMMD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMIMMW8	proc	near
	mov	eaMode,2
	call	get_modregrm
	mov	al,1
	call	out_ea
	mov	al,0
	call	out_imm
	ret
RMIMMW8	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMIMMD8	proc	near
	mov	eaMode,4
	call	get_modregrm
	mov	al,2
	call	out_ea
	mov	al,0
	call	out_imms
	ret
RMIMMD8	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMONLYB	proc	near
	mov	eaMode,1
	call	get_modregrm
	mov	al,0
	call	out_ea
	ret
RMONLYB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMONLY_NW_C	proc	near
	call	RMONLYW
	test	eaMode,128
	jnz	@@0
	call	ea2Real		;get address.
	mov	dTargetEIP,eax	;setup for execution.
	mov	dTargetCS,cx
@@0:	mov	ReturnCode,RetCode_CALLea
	ret
RMONLY_NW_C	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMONLY_FW_C	proc	near
	mov	eaMode,4
	call	get_modregrm
	mov	al,1
	call	out_ea
	mov	ReturnCode,RetCode_CALLFea
	ret
RMONLY_FW_C	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMONLY_ND_C	proc	near
	call	RMONLYD
	test	eaMode,128
	jnz	@@0
	call	ea2Real		;get address.
	mov	dTargetEIP,eax	;setup for execution.
	mov	dTargetCS,cx
@@0:	mov	ReturnCode,RetCode_CALLea32
	ret
RMONLY_ND_C	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMONLY_FD_C	proc	near
	call	RMONLYD
	mov	ReturnCode,RetCode_CALLFea32
	ret
RMONLY_FD_C	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMONLY_NW_J	proc	near
	call	RMONLYW
	mov	ReturnCode,RetCode_JMPea
	ret
RMONLY_NW_J	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMONLY_FW_J	proc	near
	mov	eaMode,4
	call	get_modregrm
	mov	al,1
	call	out_ea
	mov	ReturnCode,RetCode_JMPFea
	ret
RMONLY_FW_J	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMONLY_ND_J	proc	near
	call	RMONLYD
	mov	ReturnCode,RetCode_JMPea32
	ret
RMONLY_ND_J	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMONLY_FD_J	proc	near
	call	RMONLYD
	mov	ReturnCode,RetCode_JMPFea32
	ret
RMONLY_FD_J	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMONLYW	proc	near
	mov	eaMode,2
	call	get_modregrm
	mov	al,1
	call	out_ea
	ret
RMONLYW	endp

;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMONLYD	proc	near
	mov	eaMode,4
	call	get_modregrm
	mov	al,2
	call	out_ea
	ret
RMONLYD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Convert ea variables to seg:offset address.
;
ea2Real	proc	near
	mov	eax,eaOffset
	;
	mov	dl,eaSeg
	or	dl,dl
	jnz	@@SegOK
	;
	cmp	eaIndex,0	;any index?
	jz	@@Ind32
	test	eaIndex,128	;16 bit?
	jnz	@@Ind32
	;
	mov	dh,eaIndex
	and	dh,127
	dec	dh
	mov	dl,3+1	;default to DS.
	cmp	dh,1+1
	jc	@@SegOK
	mov	dl,2+1	;default to SS.
	cmp	dh,3+1
	jc	@@SegOK
	mov	dl,3+1	;default to DS.
	cmp	dh,5+1
	jc	@@SegOK
	mov	dl,2+1	;default to SS.
	jmp	@@SegOK
	;
@@Ind32:	mov	dh,eaReg
	and	dh,127
	dec	dh
	mov	dl,3+1	;default to DS.
	cmp	dh,4
	jc	@@SegOK
	cmp	dh,5+1
	jnc	@@SegOK
	mov	dl,2+1	;default to SS.
@@SegOK:	;
	mov	bl,eaReg
	or	bl,bl
	jz	@@NoReg
	dec	bl
	mov	cl,bl
	and	ebx,127
	and	cl,128
	mov	ebx,[ea2DebugRegs+ebx*4]
	mov	ebx,[ebx]
	or	cl,cl
	jnz	@@GotReg
	movzx	ebx,bx
@@GotReg:	add	eax,ebx
	;
@@NoReg:	mov	bl,eaIndex
	or	bl,bl
	jz	@@NoInd
	dec	bl
	mov	cl,bl
	and	ebx,127
	and	cl,128
	or	cl,cl
	jz	@@Ind16
	mov	ebx,[ea2DebugRegs+ebx*4]
	mov	ebx,[ebx]
	jmp	@@GotInd
	;
@@Ind16:	or	bl,bl
	jnz	@@Ind16_1
	mov	ebx,DebugEBX
	add	ebx,DebugESI
	movzx	ebx,bx
	cmp	eaSeg,0
	jnz	@@GotInd
	mov	dl,3+1
	jmp	@@GotInd
	;
@@Ind16_1:	cmp	bl,1
	jnz	@@Ind16_2
	mov	ebx,DebugEBX
	add	ebx,DebugEDI
	movzx	ebx,bx
	cmp	eaSeg,0
	jnz	@@GotInd
	mov	dl,3+1
	jmp	@@GotInd
	;
@@Ind16_2:	cmp	bl,2
	jnz	@@Ind16_3
	mov	ebx,DebugEBP
	add	ebx,DebugESI
	movzx	ebx,bx
	cmp	eaSeg,0
	jnz	@@GotInd
	mov	dl,2+1
	jmp	@@GotInd
	;
@@Ind16_3:	cmp	bl,3
	jnz	@@Ind16_4
	mov	ebx,DebugEBP
	add	ebx,DebugEDI
	movzx	ebx,bx
	cmp	eaSeg,0
	jnz	@@GotInd
	mov	dl,2+1
	jmp	@@GotInd
	;
@@Ind16_4:	cmp	bl,4
	jnz	@@Ind16_5
	mov	ebx,DebugESI
	movzx	ebx,bx
	cmp	eaSeg,0
	jnz	@@GotInd
	mov	dl,3+1
	jmp	@@GotInd
	;
@@Ind16_5:	cmp	bl,5
	jnz	@@Ind16_6
	mov	ebx,DebugEDI
	movzx	ebx,bx
	cmp	eaSeg,0
	jnz	@@GotInd
	mov	dl,3+1
	jmp	@@GotInd
	;
@@Ind16_6:	cmp	bl,6
	jnz	@@Ind16_7
	mov	ebx,DebugEBP
	movzx	ebx,bx
	cmp	eaSeg,0
	jnz	@@GotInd
	mov	dl,2+1
	jmp	@@GotInd
	;
@@Ind16_7:	cmp	bl,7
	jnz	@@Ind16_8
	mov	ebx,DebugEBX
	movzx	ebx,bx
	cmp	eaSeg,0
	jnz	@@GotInd
	mov	dl,3+1
	jmp	@@GotInd
	;
@@Ind16_8:	xor	ebx,ebx
	;
@@GotInd:	mov	cl,eaScale
	shl	ebx,cl
	add	eax,ebx
@@NoInd:	;
	movzx	edx,dl		;get seg reg.
	dec	edx
	mov	edx,[ea2sDebugRegs+edx*4]
	mov	cx,w[edx]
	;
	ret
ea2Real	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHIFT1B	proc	near
	mov	eaMode,1
	call	get_modregrm
	mov	al,0
	call	out_ea
	mov	al,','
	stosb
	mov al,'1'
	stosb
	mov	es:b[edi],0
	ret
SHIFT1B	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHIFT1W	proc	near
	mov	eaMode,2
	call	get_modregrm
	mov	al,1
	call	out_ea
	mov	al,','
	stosb
	mov al,'1'
	stosb
	mov	es:b[edi],0
	ret
SHIFT1W	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHIFT1D	proc	near
	mov	eaMode,4
	call	get_modregrm
	mov	al,2
	call	out_ea
	mov	al,','
	stosb
	mov al,'1'
	stosb
	mov	es:b[edi],0
	ret
SHIFT1D	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHIFT1BIMB	proc	near
	mov	eaMode,1
	call	get_modregrm
	mov	al,0
	call	out_ea
	mov	al,','
	stosb
	mov	al,fs:[esi]
	inc	esi
	mov	ecx,2
	mov	dx,0
	call	SymbolBin2Hex
	mov	es:b[edi],0
	ret
SHIFT1BIMB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHIFT1WIMB	proc	near
	mov	eaMode,2
	call	get_modregrm
	mov	al,1
	call	out_ea
	mov	al,','
	stosb
	mov	al,fs:[esi]
	inc	esi
	mov	ecx,2
	mov	dx,0
	call	SymbolBin2Hex
	mov	es:b[edi],0
	ret
SHIFT1WIMB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHIFT1DIMB	proc	near
	mov	eaMode,4
	call	get_modregrm
	mov	al,2
	call	out_ea
	mov	al,','
	stosb
	mov	al,fs:[esi]
	inc	esi
	mov	ecx,2
	mov	dx,0
	call	SymbolBin2Hex
	mov	es:b[edi],0
	ret
SHIFT1DIMB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHIFTCLB	proc	near
	mov	eaMode,1
	call	get_modregrm
	mov	al,0
	call	out_ea
	mov	al,','
	stosb
	push	esi
	lea	esi,T_CL
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	pop	esi
	ret
SHIFTCLB	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHIFTCLW	proc	near
	mov	eaMode,2
	call	get_modregrm
	mov	al,1
	call	out_ea
	mov	al,','
	stosb
	push	esi
	lea	esi,T_CL
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	pop	esi
	ret
SHIFTCLW	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SHIFTCLD	proc	near
	mov	eaMode,4
	call	get_modregrm
	mov	al,2
	call	out_ea
	mov	al,','
	stosb
	push	esi
	lea	esi,T_CL
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	pop	esi
	ret
SHIFTCLD	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
RMSEG	proc	near
	mov	eaMode,2
	call	get_modregrm
	and	bh,7
	push	bx
	mov	al,1
	call	out_ea
	mov	al,','
	stosb
	pop	bx
	push	esi
	movzx	esi,bh
	shl	esi,2
	add	esi,offset T_SegRegs
	mov	esi,[esi]
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	pop	esi
	ret
RMSEG	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
get_modregrm	proc	near
;
;Get mode,r/m,register.
;
	mov	al,fs:[esi]
	inc	esi
	mov ch,al
	mov bl,ch
	mov bh,ch
	mov cl,6
	shr ch,cl       ;ch = mod
	and bl,7        ;bl = r/m
	shr bh,1
	shr bh,1
	shr bh,1
	and bh,7        ;bh = reg
	ret
get_modregrm	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
outrm2	proc	near
	push eax
	push ebx
	push edx
	call out_ea
	mov al,','
	stosb
	pop edx
	pop ebx
	pop eax
	push	esi
	movzx	esi,bh
	shl	esi,2
	add	esi,edx
	mov	esi,[esi]
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	pop	esi
	ret
outrm2	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
outrm3	proc	near
	push eax
	push ebx
	push edx
	push	esi
	movzx	esi,bh
	shl	esi,2
	add	esi,edx
	mov	esi,[esi]
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	pop	esi
	mov al,','
	stosb
	pop edx
	pop ebx
	pop eax
	call out_ea
	ret
outrm3	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
outrb2	proc	near
;
;Print register number then address if any.
;
	push	esi
	movzx	esi,bh
	shl esi,2
	add esi,edx
	mov	esi,[esi]
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	mov	es:b[edi-1],','
	mov	es:b[edi],0
	pop	esi
	call out_ea
	ret
outrb2	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
out_ea	proc	near
	cmp	al,2
	jnz	@@old
	mov	eax,offset T_DWordRegs
	jmp	outea1
	;
@@old:	or al,al
	mov	eax,offset T_ByteRegs
	jz outea1
	mov	eax,offset T_WordRegs
outea1:	;
	cmp	SegOver_T,0
	jz	outea2
	pushm	eax,esi
	mov	esi,SegOver_T
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	dec	edi
	mov	al,':'
	stosb
	mov	es:b[edi],0
	popm	eax,esi
outea2:	;
	cmp	ch,3		;register,register?
	jnz	outea3
	;
	;register
	;
	mov	eaReg,bl
	inc	eaReg
	cmp	eax,offset T_WordRegs
	jz	@@gotreg
	cmp	eax,offset T_DWordRegs
	jnz	@@GotReg
	or	eaReg,128
@@GotReg:	push	esi
	movzx	esi,bl
	shl	esi,2
	add	esi,eax
	mov	esi,[esi]
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	jmp	@@eaExit
outea3:	;
	cmp	PointType,0
	jz	@@NoPoint
	push	esi
	mov	esi,PointType
@@DoPoint:	movsb
	cmp	b[esi-1],0
	jnz	@@DoPoint
	dec	edi
	pop	esi
@@NoPoint:	;
	test	In32Bit,2		;check address formation size.
	jz	@@old16
	;
	or	eaMode,128
	mov	es:b[edi],'['
	inc	edi
	;
	mov	dx,cx
	mov	ax,bx
	mov	bl,100b		;initialise to NO index.
	mov	bh,100b
	cmp	al,100b		;escape to 2 byte?
	jnz	@@32_0
	pushm	ax,dx
	call	Get_ModRegRM
	popm	ax,dx
	xchg	bl,al
@@32_0:	;
	cmp	dh,00b		;mod 0?
	jnz	@@NotEa32
	cmp	al,101b
	jnz	@@NotEa32
	jmp	@@NoBase32		;skip base register and put in 32 bit displacement.
	;
@@NotEa32:	push	esi
	movzx	esi,al		;get the base register
	mov	eaReg,al
	inc	eaReg
	or	eaReg,128
	shl	esi,2
	add	esi,offset T_DWordRegs
	mov	esi,[esi]
@@32_3:	movsb
	cmp	b[esi-1],0
	jnz	@@32_3
	dec	edi
	pop	esi
	;
	cmp	dh,01b		;8 bit displacement?
	jnz	@@32_1
	mov	es:b[edi],'+'
	inc	edi
	pushm	eax,ebx,ecx,edx
	movsx	eax,fs:b[esi]
	mov	eaOffset,eax
	inc	esi
	or	eax,eax
	jns	@@sign0
	neg	eax
	mov	es:d[edi-1],'-'
@@sign0:	mov	ecx,2
	mov	dx,0
	call	Bin2Hex
	mov	es:b[edi],0
	popm	eax,ebx,ecx,edx
	jmp	@@32_2
	;
@@32_1:	cmp	dh,10b		;32 bit displacement?
	jnz	@@32_2
	mov	es:b[edi],'+'
	inc	edi
@@NoBase32:	pushm	eax,ebx,ecx,edx
	mov	eax,fs:[esi]
	mov	eaOffset,eax
	add	esi,4
	or	eax,eax
	jns	@@sign1

; MED 11/16/96
; don't negative sign a 32-bit displacement
;	neg	eax
;	mov	es:d[edi-1],'-'

@@sign1:	mov	ecx,8
	mov	dx,0
	call	SymbolBin2Hex
	mov	es:b[edi],0
	popm	eax,ebx,ecx,edx
	;
@@32_2:	cmp	bl,100b		;any index?
	jnz	@@32_4
	cmp	bh,100b		;index of ESP, ie, no index?
	jz	@@32_4
	;
	mov	es:b[edi],']'
	inc	edi
	mov	es:b[edi],'['
	inc	edi
	push	esi
	movzx	esi,bh		;get the index register
	mov	eaIndex,bh
	inc	eaIndex
	or	eaIndex,128
	shl	esi,2
	add	esi,offset T_DWordRegs
	mov	esi,[esi]
@@32_5:	movsb
	cmp	b[esi-1],0
	jnz	@@32_5
	dec	edi
	pop	esi
	;
	cmp	ch,0		;any scaleing?
	jz	@@32_4
	mov	es:b[edi],'*'
	inc	edi
	mov	eaScale,1
	mov	al,'2'
	cmp	ch,1
	jz	@@32_6
	mov	eaScale,2
	mov	al,'4'
	cmp	ch,2
	jz	@@32_6
	mov	eaScale,3
	mov	al,'8'
@@32_6:	stosb
	;
@@32_4:	mov	es:b[edi],']'
	inc	edi
	mov	es:b[edi],0
	jmp	@@eaExit
	;
@@old16:	cmp	ch,0
	jnz	notea00
	cmp	bl,6
	jnz	notea00
	;
	or	eaMode,128
	mov	al,'['
	stosb
	movzx	eax,fs:w[esi]
	add	esi,2
	mov	ecx,4
	mov	dx,0
	mov	eaOffset,eax
	call	SymbolBin2Hex
	mov al,']'
	stosb
	xor	al,al
	stosb
	dec	edi
	jmp	@@eaExit
notea00:	;
	or	eaMode,128
	push	esi
	movzx	esi,bl
	mov	eaIndex,bl
	inc	eaIndex
	shl	esi,2
	add	esi,offset T_RMS
	mov	esi,[esi]
@@1:	movsb
	cmp	b[esi-1],0
	jnz	@@1
	dec	edi
	pop	esi
	cmp ch,0
	jz outeaend
	cmp ch,1
	jnz outea4
	;
	;mod=01
	;
	mov al,'+'
	stosb
	movsx	eax,fs:b[esi]
	mov	eaOffset,eax
	inc	esi
	or	eax,eax
	jns	@@sign2
	neg	eax
	mov	es:b[edi-1],'-'
@@sign2:	mov	ecx,2
	mov	dx,0
	call	SymbolBin2Hex
	mov al,']'
	stosb
	xor	al,al
	stosb
	dec	edi
	jmp	@@eaExit
	;
outea4:	;mod = 10
	;
	mov al,'+'
	stosb
	movsx	eax,fs:w[esi]
	mov	eaOffset,eax
	add	esi,2
	or	eax,eax
	jns	@@sign3
	neg	eax
	mov	es:b[edi-1],'-'
@@sign3:	mov	ecx,4
	mov	dx,0
	call	SymbolBin2Hex
outeaend:	;
	mov al,']'
	stosb
	xor	al,al
	stosb
	dec	edi
	;
@@eaExit:	test	eaMode,128
	jz	@@eaDone
	call	ea2Real		;get address.
	mov	dTargetEIP,eax	;setup for execution.
	mov	dTargetCS,cx
@@eaDone:	ret
out_ea	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
SetInOut	proc	near
	mov	ReturnCode,RetCode_INOUT
	ret
SetInOut	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
outmem	proc	near
	cmp	SegOver_T,0
	jz	outmem2
	pushm	eax,esi
	mov	esi,SegOver_T
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	dec	edi
	mov	al,':'
	stosb
	mov	es:b[edi],0
	popm	eax,esi
outmem2:	;
	cmp	PointType,0
	jz	@@NoPoint
	push	esi
	mov	esi,PointType
@@DoPoint:	movsb
	cmp	b[esi-1],0
	jnz	@@DoPoint
	dec	edi
	pop	esi
	;
@@NoPoint:	mov al,'['
	stosb
	test	In32Bit,2
	jz	@@old0
	or	eaMode,128
	call	immdword2
	jmp	@@old1
@@old0:	or	eaMode,128
	call immword2
@@old1:	;
	call	ea2Real		;get address.
	mov	dTargetEIP,eax	;setup for execution.
	mov	dTargetCS,cx
	;
	mov al,']'
	stosb
	xor	al,al
	stosb
	dec	edi
	ret
outmem	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 0 code 000.
;
ESC_0_000w	proc	near
	mov	al,1
	jmp	ESC_0_000
ESC_0_000w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_000d	proc	near
	mov	al,2
	jmp	ESC_0_000
ESC_0_000d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_000	proc	near
	;
	;Copy the FADD bit streight through.
	;
	push	esi
	mov	esi,offset T_FADD
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	;
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3		;which form of the instruction?
	jz	@@1
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	jmp	@@9
	;
@@1:	;Doing ST(i)
	;
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl		;get register number.
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0
	;
@@9:	ret
ESC_0_000	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 0 code 001.
;
ESC_0_001w	proc	near
	mov	al,1
	jmp	ESC_0_001
ESC_0_001w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_001d	proc	near
	mov	al,2
	jmp	ESC_0_001
ESC_0_001d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_001	proc	near
	;
	;Copy the FMUL bit streight through.
	;
	push	esi
	mov	esi,offset T_FMUL
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	;
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3		;which form of the instruction?
	jz	@@1
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	jmp	@@9
	;
@@1:	;Doing ST(i)
	;
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl		;get register number.
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0
	;
@@9:	ret
ESC_0_001	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 0 code 010.
;
ESC_0_010w	proc	near
	mov	al,1
	jmp	ESC_0_010
ESC_0_010w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_010d	proc	near
	mov	al,2
	jmp	ESC_0_010
ESC_0_010d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_010	proc	near
	;
	;Copy the FCOM bit streight through.
	;
	push	esi
	mov	esi,offset T_FCOM
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	;
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3		;which form of the instruction?
	jz	@@1
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	jmp	@@9
	;
@@1:	;Doing ST(i)
	;
	cmp	bl,1
	jz	@@2
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@2
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl		;get register number.
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@2:	mov	es:b[edi],0
	;
@@9:	ret
ESC_0_010	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 0 code 010.
;
ESC_0_011w	proc	near
	mov	al,1
	jmp	ESC_0_011
ESC_0_011w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_011d	proc	near
	mov	al,2
	jmp	ESC_0_011
ESC_0_011d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_011	proc	near
	;
	;Copy the FCOMP bit streight through.
	;
	push	esi
	mov	esi,offset T_FCOMP
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	;
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3		;which form of the instruction?
	jz	@@1
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	jmp	@@9
	;
@@1:	;Doing ST(i)
	;
	cmp	bl,1
	jz	@@2
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@2
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl		;get register number.
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@2:	mov	es:b[edi],0
	;
@@9:	ret
ESC_0_011	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 0 code 100.
;
ESC_0_100w	proc	near
	mov	al,1
	jmp	ESC_0_100
ESC_0_100w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_100d	proc	near
	mov	al,2
	jmp	ESC_0_100
ESC_0_100d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_100	proc	near
	;
	;Copy the FSUB bit streight through.
	;
	push	esi
	mov	esi,offset T_FSUB
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	;
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3		;which form of the instruction?
	jz	@@1
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	jmp	@@9
	;
@@1:	;Doing ST(i)
	;
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl		;get register number.
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0
	;
@@9:	ret
ESC_0_100	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 0 code 101.
;
ESC_0_101w	proc	near
	mov	al,1
	jmp	ESC_0_101
ESC_0_101w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_101d	proc	near
	mov	al,2
	jmp	ESC_0_101
ESC_0_101d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_101	proc	near
	;
	;Copy the FSUBR bit streight through.
	;
	push	esi
	mov	esi,offset T_FSUBR
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	;
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3		;which form of the instruction?
	jz	@@1
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	jmp	@@9
	;
@@1:	;Doing ST(i)
	;
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl		;get register number.
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0
	;
@@9:	ret
ESC_0_101	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 0 code 110.
;
ESC_0_110w	proc	near
	mov	al,1
	jmp	ESC_0_110
ESC_0_110w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_110d	proc	near
	mov	al,2
	jmp	ESC_0_110
ESC_0_110d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_110	proc	near
	;
	;Copy the FDIV bit streight through.
	;
	push	esi
	mov	esi,offset T_FDIV
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	;
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3		;which form of the instruction?
	jz	@@1
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	jmp	@@9
	;
@@1:	;Doing ST(i)
	;
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl		;get register number.
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0
	;
@@9:	ret
ESC_0_110	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 0 code 111.
;
ESC_0_111w	proc	near
	mov	al,1
	jmp	ESC_0_111
ESC_0_111w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_111d	proc	near
	mov	al,2
	jmp	ESC_0_111
ESC_0_111d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_0_111	proc	near
	;
	;Copy the FDIVR bit streight through.
	;
	push	esi
	mov	esi,offset T_FDIVR
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	;
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3		;which form of the instruction?
	jz	@@1
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	jmp	@@9
	;
@@1:	;Doing ST(i)
	;
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl		;get register number.
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0
	;
@@9:	ret
ESC_0_111	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 1 code 000.
;
ESC_1_000w	proc	near
	mov	al,1
	jmp	ESC_1_000
ESC_1_000w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_000d	proc	near
	mov	al,2
	jmp	ESC_1_000
ESC_1_000d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_000	proc	near
	;
	;Copy the FLD bit streight through.
	;
	push	esi
	mov	esi,offset T_FLD
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	;
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3		;which for of the instruction?
	jz	@@1
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	jmp	@@9
	;
@@1:	;Doing ST(i)
	;
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl		;get register number.
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0
	;
@@9:	ret
ESC_1_000	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 1 code 001.
;
ESC_1_001w	proc	near
	mov	al,1
	jmp	ESC_1_001
ESC_1_001w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_001d	proc	near
	mov	al,2
	jmp	ESC_1_001
ESC_1_001d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_001	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	;
	;Copy the FXCH bit streight through.
	;
	push	esi
	mov	esi,offset T_FXCH
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	cmp	bl,1
	jz	@@3
	;
	;Doing ST(i)
	;
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl		;get register number.
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0
	;
@@9:	ret
ESC_1_001	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 1 code 010.
;
ESC_1_010w	proc	near
	mov	al,1
	jmp	ESC_1_010
ESC_1_010w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_010d	proc	near
	mov	al,2
	jmp	ESC_1_010
ESC_1_010d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_010	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	;Do FNOP
	;
	push	esi
	mov	esi,offset T_FNOP
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	jmp	@@9
	;
@@1:	;Do FST
	;
	push	esi
	mov	esi,offset T_FST
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	;
@@9:	ret
ESC_1_010	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 1 code 011.
;
ESC_1_011w	proc	near
	mov	al,1
	jmp	ESC_1_011
ESC_1_011w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_011d	proc	near
	mov	al,2
	jmp	ESC_1_011
ESC_1_011d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_011	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	;
	;Do FSTP
	;
	push	esi
	mov	esi,offset T_FSTP
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	;
@@9:	ret
ESC_1_011	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 1 code 100.
;
ESC_1_100w	proc	near
	mov	al,1
	jmp	ESC_1_100
ESC_1_100w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_100d	proc	near
	mov	al,2
	jmp	ESC_1_100
ESC_1_100d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_100	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	;
	cmp	ch,3		;mm+r/m?
	jnz	@@1
	push	esi
	mov	esi,offset T_FCHS
	or	bl,bl
	jz	@@0
	mov	esi,offset T_FABS
	cmp	bl,1
	jz	@@0
	mov	esi,offset T_FTST
	cmp	bl,4
	jz	@@0
	mov	esi,offset T_FXAM
	cmp	bl,5
	jz	@@0
	mov	esi,offset T_INV
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	jmp	@@9
	;
@@1:	;Do FLDENV
	;
	push	esi
	mov	esi,offset T_FLDENV
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	call	out_ea
	;
@@9:	ret
ESC_1_100	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 1 code 101.
;
ESC_1_101w	proc	near
	mov	al,1
	jmp	ESC_1_101
ESC_1_101w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_101d	proc	near
	mov	al,2
	jmp	ESC_1_101
ESC_1_101d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_101	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	;
	cmp	ch,3
	jz	@@1
	;
	;Do FLDCW
	;
	push	esi
	mov	esi,offset T_FLDCW
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	jmp	@@9
	;
@@1:	push	esi
	movzx	ebx,bl
	mov	esi,[T_FLD_Table+ebx*4]
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	;
@@9:	ret
ESC_1_101	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 1 code 110.
;
ESC_1_110w	proc	near
	mov	al,1
	jmp	ESC_1_110
ESC_1_110w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_110d	proc	near
	mov	al,2
	jmp	ESC_1_110
ESC_1_110d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_110	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	;
	cmp	ch,3
	jz	@@1
	;
	;Do FNSTENV
	;
	push	esi
	mov	esi,offset T_FNSTENV
	cmp	FWait_Waiting,0
	jz	@@0
	mov	esi,offset T_FSTENV
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	call	out_ea
	jmp	@@9
	;
@@1:	push	esi
	movzx	ebx,bl
	mov	esi,[T_E110_Table+ebx*4]
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	;
@@9:	ret
ESC_1_110	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 1 code 101.
;
ESC_1_111w	proc	near
	mov	al,1
	jmp	ESC_1_111
ESC_1_111w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_111d	proc	near
	mov	al,2
	jmp	ESC_1_111
ESC_1_111d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_1_111	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	;
	cmp	ch,3
	jz	@@1
	;
	;Do FNSTCW
	;
	push	esi
	mov	esi,offset T_FNSTCW
	cmp	FWait_Waiting,0
	jz	@@0
	mov	esi,offset T_FSTCW
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	jmp	@@9
	;
@@1:	push	esi
	movzx	ebx,bl
	mov	esi,[T_E111_Table+ebx*4]
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	;
@@9:	ret
ESC_1_111	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 2 code 000.
;
ESC_2_000w	proc	near
	mov	al,1
	jmp	ESC_2_000
ESC_2_000w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_000d	proc	near
	mov	al,2
	jmp	ESC_2_000
ESC_2_000d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_000	proc	near
	push	esi
	mov	esi,offset T_FIADD
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	ret
ESC_2_000	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 2 code 001.
;
ESC_2_001w	proc	near
	mov	al,1
	jmp	ESC_2_001
ESC_2_001w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_001d	proc	near
	mov	al,2
	jmp	ESC_2_001
ESC_2_001d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_001	proc	near
	push	esi
	mov	esi,offset T_FIMUL
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	ret
ESC_2_001	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 2 code 010.
;
ESC_2_010w	proc	near
	mov	al,1
	jmp	ESC_2_010
ESC_2_010w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_010d	proc	near
	mov	al,2
	jmp	ESC_2_010
ESC_2_010d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_010	proc	near
	push	esi
	mov	esi,offset T_FICOM
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	ret
ESC_2_010	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 2 code 011.
;
ESC_2_011w	proc	near
	mov	al,1
	jmp	ESC_2_011
ESC_2_011w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_011d	proc	near
	mov	al,2
	jmp	ESC_2_011
ESC_2_011d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_011	proc	near
	push	esi
	mov	esi,offset T_FICOMP
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	ret
ESC_2_011	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 2 code 100.
;
ESC_2_100w	proc	near
	mov	al,1
	jmp	ESC_2_100
ESC_2_100w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_100d	proc	near
	mov	al,2
	jmp	ESC_2_100
ESC_2_100d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_100	proc	near
	push	esi
	mov	esi,offset T_FISUB
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	ret
ESC_2_100	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 2 code 101.
;
ESC_2_101w	proc	near
	mov	al,1
	jmp	ESC_2_101
ESC_2_101w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_101d	proc	near
	mov	al,2
	jmp	ESC_2_101
ESC_2_101d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_101	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@2
	push	esi
	mov	esi,offset T_FUCOMPP
@@1:	movsb
	cmp	b[esi-1],0
	jnz	@@1
	pop	esi
	dec	edi
	jmp	@@9
	;
@@2:	push	esi
	mov	esi,offset T_FISUBR
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
@@9:	ret
ESC_2_101	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 2 code 110.
;
ESC_2_110w	proc	near
	mov	al,1
	jmp	ESC_2_110
ESC_2_110w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_110d	proc	near
	mov	al,2
	jmp	ESC_2_110
ESC_2_110d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_110	proc	near
	push	esi
	mov	esi,offset T_FIDIV
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	ret
ESC_2_110	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 2 code 111.
;
ESC_2_111w	proc	near
	mov	al,1
	jmp	ESC_2_111
ESC_2_111w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_111d	proc	near
	mov	al,2
	jmp	ESC_2_111
ESC_2_111d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_2_111	proc	near
	push	esi
	mov	esi,offset T_FIDIVR
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	ret
ESC_2_111	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 3 code 000.
;
ESC_3_000w	proc	near
	mov	al,1
	jmp	ESC_3_000
ESC_3_000w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_3_000d	proc	near
	mov	al,2
	jmp	ESC_3_000
ESC_3_000d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_3_000	proc	near
	push	esi
	mov	esi,offset T_FILD
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	ret
ESC_3_000	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 3 code 010.
;
ESC_3_010w	proc	near
	mov	al,1
	jmp	ESC_3_010
ESC_3_010w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_3_010d	proc	near
	mov	al,2
	jmp	ESC_3_010
ESC_3_010d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_3_010	proc	near
	push	esi
	mov	esi,offset T_FIST
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	ret
ESC_3_010	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 3 code 011.
;
ESC_3_011w	proc	near
	mov	al,1
	jmp	ESC_3_011
ESC_3_011w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_3_011d	proc	near
	mov	al,2
	jmp	ESC_3_011
ESC_3_011d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_3_011	proc	near
	push	esi
	mov	esi,offset T_FISTP
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	mov	PointType,offset T_DWORD
	mov	eaMode,4
	call	out_ea
	ret
ESC_3_011	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 3 code 100.
;
ESC_3_100w	proc	near
	mov	al,1
	jmp	ESC_3_100
ESC_3_100w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_3_100d	proc	near
	mov	al,2
	jmp	ESC_3_100
ESC_3_100d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_3_100	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	push	esi
	mov	esi,offset T_FNCLEX
	cmp	FWait_Waiting,0
	jz	@@0
	mov	esi,offset T_FCLEX
@@0:	cmp	bl,010b
	jz	@@1
	mov	esi,offset T_FNINIT
	cmp	FWait_Waiting,0
	jz	@@2
	mov	esi,offset T_FINIT
@@2:	cmp	bl,011b
	jz	@@1
	mov	esi,offset T_INV
@@1:	movsb
	cmp	b[esi-1],0
	jnz	@@1
	pop	esi
	dec	edi
	ret
ESC_3_100	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 3 code 101.
;
ESC_3_101w	proc	near
	mov	al,1
	jmp	ESC_3_101
ESC_3_101w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_3_101d	proc	near
	mov	al,2
	jmp	ESC_3_101
ESC_3_101d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_3_101	proc	near
	push	esi
	mov	esi,offset T_FLD
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	mov	PointType,offset T_TBYTE

;	mov	eaMode,10
	mov	eaMode,4

	call	out_ea
	ret
ESC_3_101	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 3 code 111.
;
ESC_3_111w	proc	near
	mov	al,1
	jmp	ESC_3_111
ESC_3_111w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_3_111d	proc	near
	mov	al,2
	jmp	ESC_3_111
ESC_3_111d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_3_111	proc	near
	push	esi
	mov	esi,offset T_FSTP
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	mov	PointType,offset T_TBYTE

;	mov	eaMode,10
	mov	eaMode,4

	call	out_ea
	ret
ESC_3_111	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 4 code 000.
;
ESC_4_000w	proc	near
	mov	al,1
	jmp	ESC_4_000
ESC_4_000w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_000d	proc	near
	mov	al,2
	jmp	ESC_4_000
ESC_4_000d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_000	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FADD
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FADD
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	;
@@9:	ret
ESC_4_000	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 4 code 001.
;
ESC_4_001w	proc	near
	mov	al,1
	jmp	ESC_4_001
ESC_4_001w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_001d	proc	near
	mov	al,2
	jmp	ESC_4_001
ESC_4_001d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_001	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FMUL
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FMUL
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	;
@@9:	ret
ESC_4_001	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 4 code 010.
;
ESC_4_010w	proc	near
	mov	al,1
	jmp	ESC_4_010
ESC_4_010w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_010d	proc	near
	mov	al,2
	jmp	ESC_4_010
ESC_4_010d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_010	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	push	esi
	mov	esi,offset T_FCOM
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	ret
ESC_4_010	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 4 code 011.
;
ESC_4_011w	proc	near
	mov	al,1
	jmp	ESC_4_011
ESC_4_011w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_011d	proc	near
	mov	al,2
	jmp	ESC_4_011
ESC_4_011d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_011	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	push	esi
	mov	esi,offset T_FCOMP
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	ret
ESC_4_011	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 4 code 100.
;
ESC_4_100w	proc	near
	mov	al,1
	jmp	ESC_4_100
ESC_4_100w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_100d	proc	near
	mov	al,2
	jmp	ESC_4_100
ESC_4_100d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_100	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FSUBR
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FSUB
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	;
@@9:	ret
ESC_4_100	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 4 code 101.
;
ESC_4_101w	proc	near
	mov	al,1
	jmp	ESC_4_101
ESC_4_101w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_101d	proc	near
	mov	al,2
	jmp	ESC_4_101
ESC_4_101d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_101	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FSUB
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FSUBR
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	;
@@9:	ret
ESC_4_101	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 4 code 110.
;
ESC_4_110w	proc	near
	mov	al,1
	jmp	ESC_4_110
ESC_4_110w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_110d	proc	near
	mov	al,2
	jmp	ESC_4_110
ESC_4_110d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_110	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FDIVR
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FDIV
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	;
@@9:	ret
ESC_4_110	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 4 code 111.
;
ESC_4_111w	proc	near
	mov	al,1
	jmp	ESC_4_111
ESC_4_111w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_111d	proc	near
	mov	al,2
	jmp	ESC_4_111
ESC_4_111d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_4_111	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FDIV
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FDIVR
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	;
@@9:	ret
ESC_4_111	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 5 code 000.
;
ESC_5_000w	proc	near
	mov	al,1
	jmp	ESC_5_000
ESC_5_000w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_000d	proc	near
	mov	al,2
	jmp	ESC_5_000
ESC_5_000d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_000	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FFREE
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FLD
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	;
@@9:	ret
ESC_5_000	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 5 code 010.
;
ESC_5_010w	proc	near
	mov	al,1
	jmp	ESC_5_010
ESC_5_010w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_010d	proc	near
	mov	al,2
	jmp	ESC_5_010
ESC_5_010d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_010	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FST
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FST
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	;
@@9:	ret
ESC_5_010	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 5 code 011.
;
ESC_5_011w	proc	near
	mov	al,1
	jmp	ESC_5_011
ESC_5_011w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_011d	proc	near
	mov	al,2
	jmp	ESC_5_011
ESC_5_011d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_011	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FSTP
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FSTP
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	;
@@9:	ret
ESC_5_011	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 5 code 100.
;
ESC_5_100w	proc	near
	mov	al,1
	jmp	ESC_5_100
ESC_5_100w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_100d	proc	near
	mov	al,2
	jmp	ESC_5_100
ESC_5_100d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_100	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FUCOM
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	cmp	bl,1
	jz	@@3
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FRSTOR
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	call	out_ea
	;
@@9:	ret
ESC_5_100	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 5 code 101.
;
ESC_5_101w	proc	near
	mov	al,1
	jmp	ESC_5_101
ESC_5_101w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_101d	proc	near
	mov	al,2
	jmp	ESC_5_101
ESC_5_101d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_101	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax

	push	esi
	mov	esi,offset T_FUCOMP
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	cmp	bl,1
	jz	@@3
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@3
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@3:	mov	es:b[edi],0

	ret
ESC_5_101	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 5 code 110.
;
ESC_5_110w	proc	near
	mov	al,1
	jmp	ESC_5_110
ESC_5_110w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_110d	proc	near
	mov	al,2
	jmp	ESC_5_110
ESC_5_110d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_110	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	push	esi
	mov	esi,offset T_FNSAVE
	cmp	FWait_Waiting,0
	jz	@@0
	mov	esi,offset T_FSAVE
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	call	out_ea
	ret
ESC_5_110	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_111w	proc	near
	mov	al,2
	jmp	ESC_5_111
ESC_5_111w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_111d	proc	near
	mov	al,2
	jmp	ESC_5_111
ESC_5_111d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_5_111	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	push	esi
	mov	esi,offset T_FNSTSW
	cmp	FWait_Waiting,0
	jz	@@0
	mov	esi,offset T_FSTSW
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	ret
ESC_5_111	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 6 code 000.
;
ESC_6_000w	proc	near
	mov	al,1
	jmp	ESC_6_000
ESC_6_000w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_000d	proc	near
	mov	al,2
	jmp	ESC_6_000
ESC_6_000d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_000	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FADDP
	cmp	bl,1
	jnz	@@2
	mov	esi,offset T_FADD
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	cmp	bl,1
	jz	@@3
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@4
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@4:	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
@@3:	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FIADD
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	;
@@9:	ret
ESC_6_000	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 6 code 001.
;
ESC_6_001w	proc	near
	mov	al,1
	jmp	ESC_6_001
ESC_6_001w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_001d	proc	near
	mov	al,2
	jmp	ESC_6_001
ESC_6_001d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_001	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FMULP
	cmp	bl,1
	jnz	@@2
	mov	esi,offset T_FMUL
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	cmp	bl,1
	jz	@@3
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@4
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@4:	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
@@3:	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FIMUL
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	;
@@9:	ret
ESC_6_001	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 6 code 010.
;
ESC_6_010w	proc	near
	mov	al,1
	jmp	ESC_6_010
ESC_6_010w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_010d	proc	near
	mov	al,2
	jmp	ESC_6_010
ESC_6_010d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_010	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	push	esi
	mov	esi,offset T_FICOM
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	ret
ESC_6_010	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 6 code 011.
;
ESC_6_011w	proc	near
	mov	al,1
	jmp	ESC_6_011
ESC_6_011w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_011d	proc	near
	mov	al,2
	jmp	ESC_6_011
ESC_6_011d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_011	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FCOMPP
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FICOMP
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	;
@@9:	ret
ESC_6_011	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 6 code 100.
;
ESC_6_100w	proc	near
	mov	al,1
	jmp	ESC_6_100
ESC_6_100w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_100d	proc	near
	mov	al,2
	jmp	ESC_6_100
ESC_6_100d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_100	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FSUBRP
	cmp	bl,1
	jnz	@@2
	mov	esi,offset T_FSUBR
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	cmp	bl,1
	jz	@@3
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@4
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@4:	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
@@3:	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FISUB
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	;
@@9:	ret
ESC_6_100	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 6 code 101.
;
ESC_6_101w	proc	near
	mov	al,1
	jmp	ESC_6_101
ESC_6_101w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_101d	proc	near
	mov	al,2
	jmp	ESC_6_101
ESC_6_101d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_101	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FSUBP
	cmp	bl,1
	jnz	@@2
	mov	esi,offset T_FSUB
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	cmp	bl,1
	jz	@@3
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@4
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@4:	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
@@3:	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FISUBR
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	;
@@9:	ret
ESC_6_101	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 6 code 110.
;
ESC_6_110w	proc	near
	mov	al,1
	jmp	ESC_6_110
ESC_6_110w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_110d	proc	near
	mov	al,2
	jmp	ESC_6_110
ESC_6_110d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_110	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FDIVRP
	cmp	bl,1
	jnz	@@2
	mov	esi,offset T_FDIVR
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	cmp	bl,1
	jz	@@3
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@4
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@4:	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
@@3:	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FIDIV
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	;
@@9:	ret
ESC_6_110	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 6 code 111.
;
ESC_6_111w	proc	near
	mov	al,1
	jmp	ESC_6_111
ESC_6_111w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_111d	proc	near
	mov	al,2
	jmp	ESC_6_111
ESC_6_111d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_6_111	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FDIVP
	cmp	bl,1
	jnz	@@2
	mov	esi,offset T_FDIV
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	cmp	bl,1
	jz	@@3
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
	or	bl,bl
	jz	@@4
	mov	es:b[edi],"("
	inc	edi
	mov	al,bl
	mov	ecx,1
	call	Bin2Hex
	mov	es:b[edi],")"
	inc	edi
@@4:	mov	es:b[edi],","
	inc	edi
	mov	es:b[edi],"s"
	inc	edi
	mov	es:b[edi],"t"
	inc	edi
@@3:	mov	es:b[edi],0
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FIDIVR
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	;
@@9:	ret
ESC_6_111	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 7 code 000.
;
ESC_7_000w	proc	near
	mov	al,1
	jmp	ESC_7_000
ESC_7_000w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_000d	proc	near
	mov	al,2
	jmp	ESC_7_000
ESC_7_000d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_000	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	push	esi
	mov	esi,offset T_FILD
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	ret
ESC_7_000	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 7 code 010.
;
ESC_7_010w	proc	near
	mov	al,1
	jmp	ESC_7_010
ESC_7_010w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_010d	proc	near
	mov	al,2
	jmp	ESC_7_010
ESC_7_010d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_010	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	push	esi
	mov	esi,offset T_FIST
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	ret
ESC_7_010	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 7 code 011.
;
ESC_7_011w	proc	near
	mov	al,1
	jmp	ESC_7_011
ESC_7_011w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_011d	proc	near
	mov	al,2
	jmp	ESC_7_011
ESC_7_011d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_011	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	push	esi
	mov	esi,offset T_FISTP
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_WORD
	mov	eaMode,2
	call	out_ea
	ret
ESC_7_011	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 7 code 100.
;
ESC_7_100w	proc	near
	mov	al,1
	jmp	ESC_7_100
ESC_7_100w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_100d	proc	near
	mov	al,2
	jmp	ESC_7_100
ESC_7_100d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_100	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	cmp	ch,3
	jnz	@@1
	;
	push	esi
	mov	esi,offset T_FNSTSW_AX
	or	bl,bl
	jz	@@2
	mov	esi,offset T_INV
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_FBLD
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_TBYTE

;	mov	eaMode,10
	mov	eaMode,4

	call	out_ea
	;
@@9:	ret
ESC_7_100	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 7 code 101.
;
ESC_7_101w	proc	near
	mov	al,1
	jmp	ESC_7_101
ESC_7_101w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_101d	proc	near
	mov	al,2
	jmp	ESC_7_101
ESC_7_101d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_101	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	push	esi
	mov	esi,offset T_FILD
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	ret
ESC_7_101	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 7 code 110.
;
ESC_7_110w	proc	near
	mov	al,1
	jmp	ESC_7_110
ESC_7_110w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_110d	proc	near
	mov	al,2
	jmp	ESC_7_110
ESC_7_110d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_110	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	push	esi
	mov	esi,offset T_FBSTP
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_TBYTE

;	mov	eaMode,10
	mov	eaMode,4

	call	out_ea
	ret
ESC_7_110	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Process word version of ESC 7 code 111.
;
ESC_7_111w	proc	near
	mov	al,1
	jmp	ESC_7_111
ESC_7_111w	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_111d	proc	near
	mov	al,2
	jmp	ESC_7_111
ESC_7_111d	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ESC_7_111	proc	near
	push	eax
	call	get_modregrm		;find out more about the instruction.
	pop	eax
	push	esi
	mov	esi,offset T_FISTP
@@0:	movsb
	cmp	b[esi-1],0
	jnz	@@0
	pop	esi
	dec	edi
	mov	PointType,offset T_QWORD

;	mov	eaMode,8
	mov	eaMode,4

	call	out_ea
	ret
ESC_7_111	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
FWAIT_SET	proc	near
	mov	eax,esi
@@7:	cmp	fs:b[eax],67h
	jz	@@6
	cmp	fs:b[eax],26h
	jz	@@6
	cmp	fs:b[eax],2eh
	jz	@@6
	cmp	fs:b[eax],36h
	jz	@@6
	cmp	fs:b[eax],3eh
	jz	@@6
	cmp	fs:b[eax],64h
	jz	@@6
	cmp	fs:b[eax],65h
	jz	@@6
	jmp	@@8
@@6:	inc	eax
	jmp	@@7
@@8:	mov	ax,fs:w[eax]
	cmp	ax,0e2dbh		;FCLEX?
	jz	@@0
	cmp	ax,0e3dbh		;FINIT?
	jz	@@0
	and	ah,00111000b
	shr	ah,3
	cmp	ax,06ddh		;FSAVE?
	jz	@@0
	cmp	ax,07d9h		;FSTCW?
	jz	@@0
	cmp	ax,06d9h		;FSTENV?
	jz	@@0
	cmp	ax,07ddh		;FSTSW?
	jz	@@0
	jmp	@@1
	;
@@0:	or	FWait_Waiting,-1
	mov	AnotherOpCode,1
	jmp	@@9
	;
@@1:	push	esi
	mov	esi,offset T_WAIT
@@2:	movsb
	cmp	b[esi-1],0
	jnz	@@2
	pop	esi
	dec	edi
	;
@@9:	ret
FWAIT_SET	endp


;
;/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
;
	.data


xxh_struc	struc
xxh_text1	dd ?
xxh_text2	dd ?
xxh_routine	dd ?
xxh_extended	dd ?
xxh_bytes	dd ?
xxh_struc	ends


xxh	macro text1,text2,rout,table,bytes
	xxh_struc <text1,text2,rout,table,bytes>
	endm


INSTTAB	label byte
 xxh T_ADD	,T_NULL	,RMREGB	,T_BYTE	,0	; 00
 xxh T_ADD	,T_NULL	,RMREGB	,T_BYTE	,0	; 00

 xxh T_ADD	,T_NULL	,RMREGW	,T_WORD	,0	; 01
 xxh T_ADD	,T_NULL	,RMREGD	,T_DWORD	,0	; 01

 xxh T_ADD	,T_NULL	,REGRMB	,T_BYTE	,0	; 02
 xxh T_ADD	,T_NULL	,REGRMB	,T_BYTE	,0	; 02

 xxh T_ADD	,T_NULL	,REGRMW	,T_WORD	,0	; 03
 xxh T_ADD	,T_NULL	,REGRMD	,T_DWORD	,0	; 03

 xxh T_ADD	,T_AL		,IMMBYTE	,0	,0	; 04
 xxh T_ADD	,T_AL		,IMMBYTE	,0	,0	; 04

 xxh T_ADD	,T_AX		,IMMWORD	,0	,0	; 05
 xxh T_ADD	,T_EAX	,IMMDWORD	,0	,0	; 05

 xxh T_PUSH	,T_ES		,NONE	,0	,0	; 06
 xxh T_PUSH	,T_ES		,NONE	,0	,0	; 06

 xxh T_POP	,T_ES		,NONE	,0 	,0	; 07
 xxh T_POP	,T_ES		,NONE	,0 	,0	; 07

 xxh T_OR	,T_NULL	,RMREGB	,T_BYTE	,0	; 08
 xxh T_OR	,T_NULL	,RMREGB	,T_BYTE	,0	; 08

 xxh T_OR	,T_NULL	,RMREGW	,T_WORD	,0	; 09
 xxh T_OR	,T_NULL	,RMREGD	,T_DWORD	,0	; 09

 xxh T_OR	,T_NULL	,REGRMB	,T_BYTE	,0	; 0A
 xxh T_OR	,T_NULL	,REGRMB	,T_BYTE	,0	; 0A

 xxh T_OR	,T_NULL	,REGRMW	,T_WORD	,0	; 0B
 xxh T_OR	,T_NULL	,REGRMD	,T_DWORD	,0	; 0B

 xxh T_OR	,T_AL		,IMMBYTE	,0	,0	; 0C
 xxh T_OR	,T_AL		,IMMBYTE	,0	,0	; 0C

 xxh T_OR	,T_AX		,IMMWORD	,0	,0	; 0D
 xxh T_OR	,T_EAX	,IMMDWORD	,0	,0	; 0D

 xxh T_PUSH	,T_CS		,NONE	,0	,0	; 0E
 xxh T_PUSH	,T_CS		,NONE	,0	,0	; 0E

 xxh T_NULL	,T_NULL	,EXTINST0F	,EXT_0F	,0	; 0F
 xxh T_NULL	,T_NULL	,EXTINST0F	,EXT_0F	,0	; 0F
;
 xxh T_ADC	,T_NULL	,RMREGB	,T_BYTE	,0	; 10
 xxh T_ADC	,T_NULL	,RMREGB	,T_BYTE	,0	; 10

 xxh T_ADC	,T_NULL	,RMREGW	,T_WORD	,0	; 11
 xxh T_ADC	,T_NULL	,RMREGD	,T_DWORD	,0	; 11

 xxh T_ADC	,T_NULL	,REGRMB	,T_BYTE	,0	; 12
 xxh T_ADC	,T_NULL	,REGRMB	,T_BYTE	,0	; 12

 xxh T_ADC	,T_NULL	,REGRMW	,T_WORD	,0	; 13
 xxh T_ADC	,T_NULL	,REGRMD	,T_DWORD	,0	; 13

 xxh T_ADC	,T_AL		,IMMBYTE	,0	,0	; 14
 xxh T_ADC	,T_AL		,IMMBYTE	,0	,0	; 14

 xxh T_ADC	,T_AX		,IMMWORD	,0	,0	; 15
 xxh T_ADC	,T_EAX	,IMMDWORD	,0	,0	; 15

 xxh T_PUSH	,T_SS		,NONE	,0	,0	; 16
 xxh T_PUSH	,T_SS		,NONE	,0	,0	; 16

 xxh T_POP	,T_SS		,NONE	,0	,0	; 17
 xxh T_POP	,T_SS		,NONE	,0	,0	; 17

 xxh T_SBB	,T_NULL	,RMREGB 	,T_BYTE	,0	; 18
 xxh T_SBB	,T_NULL	,RMREGB 	,T_BYTE	,0	; 18

 xxh T_SBB	,T_NULL	,RMREGW 	,T_WORD	,0	; 19
 xxh T_SBB	,T_NULL	,RMREGD 	,T_DWORD	,0	; 19

 xxh T_SBB	,T_NULL	,REGRMB 	,T_BYTE	,0	; 1A
 xxh T_SBB	,T_NULL	,REGRMB 	,T_BYTE	,0	; 1A

 xxh T_SBB	,T_NULL	,REGRMW 	,T_WORD	,0	; 1B
 xxh T_SBB	,T_NULL	,REGRMD 	,T_DWORD	,0	; 1B

 xxh T_SBB	,T_AL		,IMMBYTE	,0	,0	; 1C
 xxh T_SBB	,T_AL		,IMMBYTE	,0	,0	; 1C

 xxh T_SBB	,T_AX		,IMMWORD	,0	,0	; 1D
 xxh T_SBB	,T_EAX	,IMMDWORD	,0	,0	; 1D

 xxh T_PUSH	,T_DS		,NONE	,0	,0	; 1E
 xxh T_PUSH	,T_DS		,NONE	,0	,0	; 1E

 xxh T_POP	,T_DS		,NONE	,0	,0	; 1F
 xxh T_POP	,T_DS		,NONE	,0	,0	; 1F
;
 xxh T_AND	,T_NULL	,RMREGB	,T_BYTE	,0	; 20
 xxh T_AND	,T_NULL	,RMREGB	,T_BYTE	,0	; 20

 xxh T_AND	,T_NULL	,RMREGW	,T_WORD	,0	; 21
 xxh T_AND	,T_NULL	,RMREGD	,T_DWORD	,0	; 21

 xxh T_AND	,T_NULL	,REGRMB	,T_BYTE	,0	; 22
 xxh T_AND	,T_NULL	,REGRMB	,T_BYTE	,0	; 22

 xxh T_AND	,T_NULL	,REGRMW	,T_WORD	,0	; 23
 xxh T_AND	,T_NULL	,REGRMD	,T_DWORD	,0	; 23

 xxh T_AND	,T_AL		,IMMBYTE	,0	,0	; 24
 xxh T_AND	,T_AL		,IMMBYTE	,0	,0	; 24

 xxh T_AND	,T_AX		,IMMWORD	,0	,0	; 25
 xxh T_AND	,T_EAX	,IMMDWORD	,0	,0	; 25

 xxh T_NULL	,T_NULL	,SEGOVER	,0	,0	; 26
 xxh T_NULL	,T_NULL	,SEGOVER	,0	,0	; 26

 xxh T_DAA	,T_NULL	,NONE	,0	,0	; 27
 xxh T_DAA	,T_NULL	,NONE	,0	,0	; 27

 xxh T_SUB	,T_NULL	,RMREGB 	,T_BYTE	,0	; 28
 xxh T_SUB	,T_NULL	,RMREGB 	,T_BYTE	,0	; 28

 xxh T_SUB	,T_NULL	,RMREGW 	,T_WORD	,0	; 29
 xxh T_SUB	,T_NULL	,RMREGD 	,T_DWORD	,0	; 29

 xxh T_SUB	,T_NULL	,REGRMB 	,T_BYTE	,0	; 2A
 xxh T_SUB	,T_NULL	,REGRMB 	,T_BYTE	,0	; 2A

 xxh T_SUB	,T_NULL	,REGRMW 	,T_WORD	,0	; 2B
 xxh T_SUB	,T_NULL	,REGRMD 	,T_DWORD	,0	; 2B

 xxh T_SUB	,T_AL		,IMMBYTE	,0	,0	; 2C
 xxh T_SUB	,T_AL		,IMMBYTE	,0	,0	; 2C

 xxh T_SUB	,T_AX		,IMMWORD	,0	,0	; 2D
 xxh T_SUB	,T_EAX	,IMMDWORD	,0	,0	; 2D

 xxh T_NULL	,T_NULL	,SEGOVER	,0	,0	; 2E
 xxh T_NULL	,T_NULL	,SEGOVER	,0	,0	; 2E

 xxh T_DAS	,T_NULL	,NONE 	,0	,0	; 2F
 xxh T_DAS	,T_NULL	,NONE 	,0	,0	; 2F
;
 xxh T_XOR	,T_NULL	,RMREGB 	,T_BYTE	,0	;30
 xxh T_XOR	,T_NULL	,RMREGB 	,T_BYTE	,0	;30

 xxh T_XOR	,T_NULL	,RMREGW 	,T_WORD	,0	;31
 xxh T_XOR	,T_NULL	,RMREGD 	,T_DWORD	,0	;31

 xxh T_XOR	,T_NULL	,REGRMB 	,T_BYTE	,0	;32
 xxh T_XOR	,T_NULL	,REGRMB 	,T_BYTE	,0	;32

 xxh T_XOR	,T_NULL	,REGRMW 	,T_WORD	,0	;33
 xxh T_XOR	,T_NULL	,REGRMD 	,T_DWORD	,0	;33

 xxh T_XOR	,T_AL		,IMMBYTE	,0	,0	;34
 xxh T_XOR	,T_AL		,IMMBYTE	,0	,0	;34

 xxh T_XOR	,T_AX		,IMMWORD	,0	,0	;35
 xxh T_XOR	,T_EAX	,IMMDWORD	,0	,0	;35

 xxh T_NULL	,T_NULL	,SEGOVER	,0 	,0	;36
 xxh T_NULL	,T_NULL	,SEGOVER	,0 	,0	;36

 xxh T_AAA	,T_NULL	,NONE 	,0	,0	;37
 xxh T_AAA	,T_NULL	,NONE 	,0	,0	;37

 xxh T_CMP	,T_NULL	,RMREGB 	,T_BYTE	,0	;38
 xxh T_CMP	,T_NULL	,RMREGB 	,T_BYTE	,0	;38

 xxh T_CMP	,T_NULL	,RMREGW 	,T_WORD	,0	;39
 xxh T_CMP	,T_NULL	,RMREGD 	,T_DWORD	,0	;39

 xxh T_CMP	,T_NULL	,REGRMB 	,T_BYTE	,0	;3A
 xxh T_CMP	,T_NULL	,REGRMB 	,T_BYTE	,0	;3A

 xxh T_CMP	,T_NULL	,REGRMW 	,T_WORD	,0	;3B
 xxh T_CMP	,T_NULL	,REGRMD 	,T_DWORD	,0	;3B

 xxh T_CMP	,T_AL		,IMMBYTE	,0	,0	;3C
 xxh T_CMP	,T_AL		,IMMBYTE	,0	,0	;3C

 xxh T_CMP	,T_AX		,IMMWORD	,0	,0   	;3D
 xxh T_CMP	,T_EAX	,IMMDWORD	,0	,0   	;3D

 xxh T_NULL	,T_NULL	,SEGOVER	,0	,0	;3E
 xxh T_NULL	,T_NULL	,SEGOVER	,0	,0	;3E

 xxh T_AAS	,T_NULL	,NONE	,0	,0	;3F
 xxh T_AAS	,T_NULL	,NONE	,0	,0	;3F
;
 xxh T_INC	,T_AX		,NONE 	,0	,0	;40
 xxh T_INC	,T_EAX	,NONE 	,0	,0	;40

 xxh T_INC	,T_CX		,NONE 	,0	,0	;41
 xxh T_INC	,T_ECX	,NONE 	,0	,0	;41

 xxh T_INC	,T_DX		,NONE 	,0	,0	;42
 xxh T_INC	,T_EDX	,NONE 	,0	,0	;42

 xxh T_INC	,T_BX		,NONE 	,0	,0	;43
 xxh T_INC	,T_EBX	,NONE 	,0	,0	;43

 xxh T_INC	,T_SP		,NONE 	,0	,0	;44
 xxh T_INC	,T_ESP	,NONE 	,0	,0	;44

 xxh T_INC	,T_BP		,NONE 	,0	,0	;45
 xxh T_INC	,T_EBP	,NONE 	,0	,0	;45

 xxh T_INC	,T_SI		,NONE 	,0	,0	;46
 xxh T_INC	,T_ESI	,NONE 	,0	,0	;46

 xxh T_INC	,T_DI		,NONE 	,0	,0	;47
 xxh T_INC	,T_EDI	,NONE 	,0	,0	;47

 xxh T_DEC	,T_AX		,NONE 	,0	,0	;48
 xxh T_DEC	,T_EAX	,NONE 	,0	,0	;48

 xxh T_DEC	,T_CX		,NONE 	,0	,0	;49
 xxh T_DEC	,T_ECX	,NONE 	,0	,0	;49

 xxh T_DEC	,T_DX		,NONE 	,0	,0	;4A
 xxh T_DEC	,T_EDX	,NONE 	,0	,0	;4A

 xxh T_DEC	,T_BX		,NONE 	,0	,0	;4B
 xxh T_DEC	,T_EBX	,NONE 	,0	,0	;4B

 xxh T_DEC	,T_SP		,NONE 	,0	,0	;4C
 xxh T_DEC	,T_ESP	,NONE 	,0	,0	;4C

 xxh T_DEC	,T_BP		,NONE 	,0	,0	;4D
 xxh T_DEC	,T_EBP	,NONE 	,0	,0	;4D

 xxh T_DEC	,T_SI		,NONE 	,0	,0	;4E
 xxh T_DEC	,T_ESI	,NONE 	,0	,0	;4E

 xxh T_DEC	,T_DI		,NONE 	,0	,0	;4F
 xxh T_DEC	,T_EDI	,NONE 	,0	,0	;4F
;
 xxh T_PUSH	,T_AX		,NONE 	,0	,0	;50
 xxh T_PUSH	,T_EAX	,NONE 	,0	,0	;50

 xxh T_PUSH	,T_CX		,NONE 	,0	,0	;51
 xxh T_PUSH	,T_ECX	,NONE 	,0	,0	;51

 xxh T_PUSH	,T_DX		,NONE 	,0	,0	;52
 xxh T_PUSH	,T_EDX	,NONE 	,0	,0	;52

 xxh T_PUSH	,T_BX		,NONE 	,0	,0	;53
 xxh T_PUSH	,T_EBX	,NONE 	,0	,0	;53

 xxh T_PUSH	,T_SP		,NONE 	,0	,0	;54
 xxh T_PUSH	,T_ESP	,NONE 	,0	,0	;54

 xxh T_PUSH	,T_BP		,NONE 	,0	,0	;55
 xxh T_PUSH	,T_EBP	,NONE 	,0	,0	;55

 xxh T_PUSH	,T_SI		,NONE 	,0	,0	;56
 xxh T_PUSH	,T_ESI	,NONE 	,0	,0	;56

 xxh T_PUSH	,T_DI		,NONE 	,0	,0	;57
 xxh T_PUSH	,T_EDI	,NONE 	,0	,0	;57

 xxh T_POP	,T_AX		,NONE 	,0	,0	;58
 xxh T_POP	,T_EAX	,NONE 	,0	,0	;58

 xxh T_POP	,T_CX		,NONE 	,0	,0	;59
 xxh T_POP	,T_ECX	,NONE 	,0	,0	;59

 xxh T_POP	,T_DX		,NONE 	,0	,0	;5A
 xxh T_POP	,T_EDX	,NONE 	,0	,0	;5A

 xxh T_POP	,T_BX		,NONE 	,0	,0	;5B
 xxh T_POP	,T_EBX	,NONE 	,0	,0	;5B

 xxh T_POP	,T_SP		,NONE 	,0	,0	;5C
 xxh T_POP	,T_ESP	,NONE 	,0	,0	;5C

 xxh T_POP	,T_BP		,NONE 	,0	,0	;5D
 xxh T_POP	,T_EBP	,NONE 	,0	,0	;5D

 xxh T_POP	,T_SI		,NONE 	,0	,0	;5E
 xxh T_POP	,T_ESI	,NONE 	,0	,0	;5E

 xxh T_POP	,T_DI		,NONE 	,0	,0	;5F
 xxh T_POP	,T_EDI	,NONE 	,0	,0	;5F
;
 xxh T_PUSHA	,T_NULL	,NONE	,0	,0	;60
 xxh T_PUSHAD	,T_NULL	,NONE	,0	,0	;60

 xxh T_POPA	,T_NULL	,NONE	,0	,0	;61
 xxh T_POPAD	,T_NULL	,NONE	,0	,0	;61

 xxh T_BOUND	,T_NULL	,REGRMW	,T_WORD	,0	;62
 xxh T_BOUND	,T_NULL	,REGRMD	,T_DWORD	,0	;62

 xxh T_ARPL	,T_NULL	,RMREGW	,T_WORD	,0	;63
 xxh T_ARPL	,T_NULL	,RMREGW	,T_WORD	,0	;63

 xxh T_NULL	,T_NULL	,SegOverFS	,0	,0	;64
 xxh T_NULL	,T_NULL	,SegOverFS	,0	,0	;64

 xxh T_NULL	,T_NULL	,SegOverGS	,0	,0	;65
 xxh T_NULL	,T_NULL	,SegOverGS	,0	,0	;65

 xxh T_NULL	,T_NULL	,DoOpsize	,0	,0	;66
 xxh T_NULL	,T_NULL	,DoOpsize	,0	,0	;66

 xxh T_NULL	,T_NULL	,DoAddSize	,0	,0	;67
 xxh T_NULL	,T_NULL	,DoAddSize	,0	,0	;67

 xxh T_PUSH	,T_NULL	,IMMWORD2	,0	,0	;68
 xxh T_PUSH	,T_NULL	,IMMDWORD2	,0	,0	;68

 xxh T_IMUL	,T_NULL	,GvEvIvW	,T_WORD	,0	;69
 xxh T_IMUL	,T_NULL	,GvEvIvD	,T_DWORD	,0	;69

 xxh T_PUSH	,T_NULL	,IMMBYTE2	,0	,0	;6A
 xxh T_PUSH	,T_NULL	,IMMBYTE2	,0	,0	;6A

 xxh T_IMUL	,T_NULL	,GvEvIbW	,T_WORD	,0	;6B
 xxh T_IMUL	,T_NULL	,GvEvIbD	,T_DWORD	,0	;6B

 xxh T_INSB	,T_NULL	,SetInOut	,0	,0	;6C
 xxh T_INSB	,T_NULL	,SetInOut	,0	,0	;6C

 xxh T_INSW	,T_NULL	,SetInOut	,0	,0	;6D
 xxh T_INSD	,T_NULL	,SetInOut	,0	,0	;6D

 xxh T_OUTSB	,T_NULL	,SetInOut	,0	,0	;6E
 xxh T_OUTSB	,T_NULL	,SetInOut	,0	,0	;6E

 xxh T_OUTSW	,T_NULL	,SetInOut	,0	,0	;6F
 xxh T_OUTSD	,T_NULL	,SetInOut	,0	,0	;6F
;
 xxh T_JO	,T_NULL	,DODISPB	,0	,0	;70
 xxh T_JO	,T_NULL	,DODISPB	,0	,0	;70

 xxh T_JNO	,T_NULL	,DODISPB	,0	,0	;71
 xxh T_JNO	,T_NULL	,DODISPB	,0	,0	;71

 xxh T_JB	,T_NULL	,DODISPB	,0	,0	;72
 xxh T_JB	,T_NULL	,DODISPB	,0	,0	;72

 xxh T_JNB	,T_NULL	,DODISPB	,0	,0	;73
 xxh T_JNB	,T_NULL	,DODISPB	,0	,0	;73

 xxh T_JE	,T_NULL	,DODISPB	,0	,0	;74
 xxh T_JE	,T_NULL	,DODISPB	,0	,0	;74

 xxh T_JNE	,T_NULL	,DODISPB	,0	,0	;75
 xxh T_JNE	,T_NULL	,DODISPB	,0	,0	;75

 xxh T_JBE	,T_NULL	,DODISPB	,0	,0	;76
 xxh T_JBE	,T_NULL	,DODISPB	,0	,0	;76

 xxh T_JNBE	,T_NULL	,DODISPB	,0	,0	;77
 xxh T_JNBE	,T_NULL	,DODISPB	,0	,0	;77

 xxh T_JS	,T_NULL	,DODISPB	,0	,0	;78
 xxh T_JS	,T_NULL	,DODISPB	,0	,0	;78

 xxh T_JNS	,T_NULL	,DODISPB	,0	,0	;79
 xxh T_JNS	,T_NULL	,DODISPB	,0	,0	;79

 xxh T_JP	,T_NULL	,DODISPB	,0	,0	;7A
 xxh T_JP	,T_NULL	,DODISPB	,0	,0	;7A

 xxh T_JNP	,T_NULL	,DODISPB	,0	,0	;7B
 xxh T_JNP	,T_NULL	,DODISPB	,0	,0	;7B

 xxh T_JL	,T_NULL	,DODISPB	,0	,0	;7C
 xxh T_JL	,T_NULL	,DODISPB	,0	,0	;7C

 xxh T_JNL	,T_NULL	,DODISPB	,0	,0	;7D
 xxh T_JNL	,T_NULL	,DODISPB	,0	,0	;7D

 xxh T_JLE	,T_NULL	,DODISPB	,0	,0	;7E
 xxh T_JLE	,T_NULL	,DODISPB	,0	,0	;7E

 xxh T_JNLE	,T_NULL	,DODISPB	,0	,0	;7F
 xxh T_JNLE	,T_NULL	,DODISPB	,0	,0	;7F
;
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_80	,0	;80
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_80	,0	;80

 xxh T_NULL	,T_NULL	,EXTINST	,EXT_81	,0	;81
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_81	,0	;81

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;82
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;82

 xxh T_NULL	,T_NULL	,EXTINST	,EXT_83	,0	;83
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_83	,0	;83

 xxh T_TEST	,T_NULL	,RMREGB 	,T_BYTE	,0	;84
 xxh T_TEST	,T_NULL	,RMREGB 	,T_BYTE	,0	;84

 xxh T_TEST	,T_NULL	,RMREGW 	,T_WORD	,0	;85
 xxh T_TEST	,T_NULL	,RMREGD 	,T_DWORD	,0	;85

 xxh T_XCHG	,T_NULL	,RMREGB 	,T_BYTE	,0	;86
 xxh T_XCHG	,T_NULL	,RMREGB 	,T_BYTE	,0	;86

 xxh T_XCHG	,T_NULL	,RMREGW 	,T_WORD	,0	;87
 xxh T_XCHG	,T_NULL	,RMREGD 	,T_DWORD	,0	;87

 xxh T_MOV	,T_NULL	,RMREGB 	,T_BYTE	,0	;88
 xxh T_MOV	,T_NULL	,RMREGB 	,T_BYTE	,0	;88

 xxh T_MOV	,T_NULL	,RMREGW 	,T_WORD	,0	;89
 xxh T_MOV	,T_NULL	,RMREGD 	,T_DWORD	,0	;89

 xxh T_MOV	,T_NULL	,REGRMB 	,T_BYTE	,0	;8A
 xxh T_MOV	,T_NULL	,REGRMB 	,T_BYTE	,0	;8A

 xxh T_MOV	,T_NULL	,REGRMW 	,T_WORD	,0	;8B
 xxh T_MOV	,T_NULL	,REGRMD 	,T_DWORD	,0	;8B

 xxh T_MOV	,T_NULL	,RMSEG 	,T_WORD	,0	;8C
 xxh T_MOV	,T_NULL	,RMSEG 	,T_WORD	,0	;8C

 xxh T_LEA	,T_NULL	,REGRMW 	,0	,0	;8D
 xxh T_LEA	,T_NULL	,REGRMD 	,0	,0	;8D

 xxh T_MOV	,T_NULL	,SEGRM 	,T_WORD	,0	;8E
 xxh T_MOV	,T_NULL	,SEGRM 	,T_WORD	,0	;8E

 xxh T_POP	,T_NULL	,RMONLYW	,T_WORD	,0	;0
 xxh T_POP	,T_NULL	,RMONLYD	,T_DWORD	,0	;0
;
 xxh T_NOP	,T_NULL	,NONE 	,0	,0	;90
 xxh T_NOP	,T_NULL	,NONE 	,0	,0	;90

 xxh T_XCHG	,T_CXAX	,NONE 	,0	,0	;91
 xxh T_XCHG	,T_ECXEAX	,NONE 	,0	,0	;91

 xxh T_XCHG	,T_DXAX	,NONE 	,0	,0	;92
 xxh T_XCHG	,T_EDXEAX	,NONE 	,0	,0	;92

 xxh T_XCHG	,T_BXAX	,NONE 	,0	,0	;93
 xxh T_XCHG	,T_EBXEAX	,NONE 	,0	,0	;93

 xxh T_XCHG	,T_SPAX	,NONE 	,0	,0	;94
 xxh T_XCHG	,T_ESPEAX	,NONE 	,0	,0	;94

 xxh T_XCHG	,T_BPAX	,NONE 	,0	,0	;95
 xxh T_XCHG	,T_EBPEAX	,NONE 	,0	,0	;95

 xxh T_XCHG	,T_SIAX	,NONE 	,0	,0	;96
 xxh T_XCHG	,T_ESIEAX	,NONE 	,0	,0	;96

 xxh T_XCHG	,T_DIAX	,NONE 	,0	,0	;97
 xxh T_XCHG	,T_EDIEAX	,NONE 	,0	,0	;97

 xxh T_CBW	,T_NULL	,NONE 	,0	,0	;98
 xxh T_CWDE	,T_NULL	,NONE 	,0	,0	;98

 xxh T_CWD	,T_NULL	,NONE 	,0	,0	;99
 xxh T_CDQ	,T_NULL	,NONE 	,0	,0	;99

 xxh T_CALL	,T_NULL	,INTERAD_C	,0	,0	;9A
 xxh T_CALL	,T_NULL	,INTERAD_C	,0	,0	;9A

 xxh T_NULL	,T_NULL	,FWAIT_SET 	,0	,0	;9B
 xxh T_NULL	,T_NULL	,FWAIT_SET	,0	,0	;9B

 xxh T_PUSHF	,T_NULL	,NONE 	,0	,0	;9C
 xxh T_PUSHFD	,T_NULL	,NONE 	,0	,0	;9C

 xxh T_POPF	,T_NULL	,NONE 	,0	,0	;9D
 xxh T_POPFD	,T_NULL	,NONE 	,0	,0	;9D

 xxh T_SAHF	,T_NULL	,NONE 	,0	,0	;9E
 xxh T_SAHF	,T_NULL	,NONE 	,0	,0	;9E

 xxh T_LAHF	,T_NULL	,NONE 	,0	,0	;9F
 xxh T_LAHF	,T_NULL	,NONE 	,0	,0	;9F
;
 xxh T_MOV	,T_AL		,MEMACB 	,T_BYTE	,0 	;A0
 xxh T_MOV	,T_AL		,MEMACB 	,T_BYTE	,0 	;A0

 xxh T_MOV	,T_AX		,MEMACW 	,T_WORD	,0	;A1
 xxh T_MOV	,T_EAX	,MEMACD 	,T_DWORD	,0	;A1

 xxh T_MOV	,T_NULL	,ACMEMB 	,T_BYTE	,0	;A2
 xxh T_MOV	,T_NULL	,ACMEMB 	,T_BYTE	,0	;A2

 xxh T_MOV	,T_NULL	,ACMEMW 	,T_WORD	,0	;A3
 xxh T_MOV	,T_NULL	,ACMEMD 	,T_DWORD	,0	;A3

 xxh T_MOVSB	,T_NULL	,movsSegb 	,0	,0	;A4
 xxh T_MOVSB	,T_NULL	,movsSegb	,0	,0	;A4

 xxh T_MOVSW	,T_NULL	,movsSegw	,0	,0	;A5
 xxh T_MOVSD	,T_NULL	,movsSegd	,0	,0	;A5

 xxh T_CMPSB	,T_NULL	,movsSegb	,0	,0	;A6
 xxh T_CMPSB	,T_NULL	,movsSegb	,0	,0	;A6

 xxh T_CMPSW	,T_NULL	,movsSegw	,0	,0	;A7
 xxh T_CMPSD	,T_NULL	,movsSegd	,0	,0	;A7

 xxh T_TEST	,T_AL		,IMMBYTE	,0	,0	;A8
 xxh T_TEST	,T_AL		,IMMBYTE	,0	,0	;A8

 xxh T_TEST	,T_AX		,IMMWORD	,0	,0	;A9
 xxh T_TEST	,T_EAX	,IMMDWORD	,0	,0	;A9

 xxh T_STOSB	,T_NULL	,stosSegb 	,0	,0	;AA
 xxh T_STOSB	,T_NULL	,stosSegb	,0	,0	;AA

 xxh T_STOSW	,T_NULL	,stosSegw 	,0	,0	;AB
 xxh T_STOSD	,T_NULL	,stosSegd	,0	,0	;AB

 xxh T_LODSB	,T_NULL	,LodsSegb 	,0	,0	;AC
 xxh T_LODSB	,T_NULL	,LodsSegb	,0	,0	;AC

 xxh T_LODSW	,T_NULL	,LodsSegw	,0	,0	;AD
 xxh T_LODSD	,T_NULL	,LodsSegd	,0	,0	;AD

 xxh T_SCASB	,T_NULL	,stosSegb	,0	,0	;AE
 xxh T_SCASB	,T_NULL	,stosSegb	,0	,0	;AE

 xxh T_SCASW	,T_NULL	,stosSegw	,0	,0	;AF
 xxh T_SCASD	,T_NULL	,stosSegd	,0	,0	;AF
;
 xxh T_MOV	,T_AL		,IMMBYTE	,0	,0	;B0
 xxh T_MOV	,T_AL		,IMMBYTE	,0	,0	;B0

 xxh T_MOV	,T_CL		,IMMBYTE	,0	,0	;B1
 xxh T_MOV	,T_CL		,IMMBYTE	,0	,0	;B1

 xxh T_MOV	,T_DL		,IMMBYTE	,0	,0	;B2
 xxh T_MOV	,T_DL		,IMMBYTE	,0	,0	;B2

 xxh T_MOV	,T_BL		,IMMBYTE	,0	,0	;B3
 xxh T_MOV	,T_BL		,IMMBYTE	,0	,0	;B3

 xxh T_MOV	,T_AH		,IMMBYTE	,0	,0	;B4
 xxh T_MOV	,T_AH		,IMMBYTE	,0	,0	;B4

 xxh T_MOV	,T_CH		,IMMBYTE	,0	,0	;B5
 xxh T_MOV	,T_CH		,IMMBYTE	,0	,0	;B5

 xxh T_MOV	,T_DH		,IMMBYTE	,0	,0	;B6
 xxh T_MOV	,T_DH		,IMMBYTE	,0	,0	;B6

 xxh T_MOV	,T_BH		,IMMBYTE	,0	,0	;B7
 xxh T_MOV	,T_BH		,IMMBYTE	,0	,0	;B7

 xxh T_MOV	,T_AX		,IMMWORD	,0	,0	;B8
 xxh T_MOV	,T_EAX	,IMMDWORD	,0	,0	;B8

 xxh T_MOV	,T_CX		,IMMWORD	,0	,0	;B9
 xxh T_MOV	,T_ECX	,IMMDWORD	,0	,0	;B9

 xxh T_MOV	,T_DX		,IMMWORD	,0	,0	;BA
 xxh T_MOV	,T_EDX	,IMMDWORD	,0	,0	;BA

 xxh T_MOV	,T_BX		,IMMWORD	,0	,0	;BB
 xxh T_MOV	,T_EBX	,IMMDWORD	,0	,0	;BB

 xxh T_MOV	,T_SP		,IMMWORD	,0	,0	;BC
 xxh T_MOV	,T_ESP	,IMMDWORD	,0	,0	;BC

 xxh T_MOV	,T_BP		,IMMWORD	,0	,0	;BD
 xxh T_MOV	,T_EBP	,IMMDWORD	,0	,0	;BD

 xxh T_MOV	,T_SI		,IMMWORD	,0	,0	;BE
 xxh T_MOV	,T_ESI	,IMMDWORD	,0	,0	;BE

 xxh T_MOV	,T_DI		,IMMWORD	,0	,0	;BF
 xxh T_MOV	,T_EDI	,IMMDWORD	,0	,0	;BF
;
 xxh T_NULL	,T_NULL	,EXTINST 	,EXT_C0	,0	;C0
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_C0	,0	;C0

 xxh T_NULL	,T_NULL	,EXTINST 	,EXT_C1	,0	;C1
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_C1	,0	;C1

 xxh T_RET	,T_NULL	,W_RETnn	,0	,0	;C2
 xxh T_RET	,T_NULL	,D_RETnn	,0	,0	;C2

 xxh T_RET	,T_NULL	,RETWORD 	,0	,0	;C3
 xxh T_RET	,T_NULL	,RETDWORD	,0	,0	;C3

 xxh T_LES	,T_NULL	,REGRMSW 	,T_DWORD	,0	;C4
 xxh T_LES	,T_NULL	,REGRMSF 	,T_FWORD	,0	;C4

 xxh T_LDS	,T_NULL	,REGRMSW 	,T_DWORD	,0	;C5
 xxh T_LDS	,T_NULL	,REGRMSF 	,T_FWORD	,0	;C5

 xxh T_MOV	,T_NULL	,RMIMMBN	,T_BYTE	,0	;C6
 xxh T_MOV	,T_NULL	,RMIMMBN	,T_BYTE	,0	;C6

 xxh T_MOV	,T_NULL	,RMIMMWN	,T_WORD	,0	;C7
 xxh T_MOV	,T_NULL	,RMIMMDN	,T_DWORD	,0	;C7

 xxh T_ENTER	,T_NULL	,IMMWORDBYTE	,0	,0	;C8
 xxh T_ENTER	,T_NULL	,IMMWORDBYTE	,0	,0	;C8

 xxh T_LEAVE	,T_NULL	,NONE 	,0	,0	;C9
 xxh T_LEAVE	,T_NULL	,NONE 	,0	,0	;C9

 xxh T_RETF	,T_NULL	,W_RETFnn	,0	,0	;CA
 xxh T_RETF	,T_NULL	,D_RETFnn	,0	,0	;CA

 xxh T_RETF	,T_NULL	,RETDWORD2 	,0	,0	;CB
 xxh T_RETF	,T_NULL	,RETFWORD	,0	,0	;CB

 xxh T_INT	,T_3		,SETINT 	,0	,0	;CC
 xxh T_INT	,T_3		,SETINT	,0	,0	;CC

 xxh T_INT	,T_NULL	,SETINT2	,0	,0	;CD
 xxh T_INT	,T_NULL	,SETINT2	,0	,0	;CD

 xxh T_INTO	,T_NULL	,SETINT	,0	,0	;CE
 xxh T_INTO	,T_NULL	,SETINT	,0	,0	;CE

 xxh T_IRET	,T_NULL	,W_IRET 	,0	,0	;CF
 xxh T_IRET	,T_NULL	,D_IRET	,0	,0	;CF
;
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_D0	,0	;D0
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_D0	,0	;D0

 xxh T_NULL	,T_NULL	,EXTINST	,EXT_D1	,0	;D1
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_D1	,0	;D1

 xxh T_NULL	,T_NULL	,EXTINST	,EXT_D2	,0	;D2
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_D2	,0	;D2

 xxh T_NULL	,T_NULL	,EXTINST	,EXT_D3	,0	;D3
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_D3	,0	;D3

 xxh T_AAM	,T_NULL	,LoseByte	,0	,0	;D4
 xxh T_AAM	,T_NULL	,LoseByte	,0	,0	;D4

 xxh T_AAD	,T_NULL	,LoseByte	,0	,0	;D5
 xxh T_AAD	,T_NULL	,LoseByte	,0	,0	;D5

 xxh T_INV	,T_NULL	,Invalid 	,0	,0	;D6
 xxh T_INV	,T_NULL	,Invalid 	,0	,0	;D6

 xxh T_XLAT	,T_NULL	,NONE 	,0	,0	;D7
 xxh T_XLAT	,T_NULL	,NONE 	,0	,0	;D7

 xxh T_NULL	,T_NULL	,EXTINST	,EXT_D8	,0	;D8
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_D8	,0	;D8

 xxh T_NULL	,T_NULL	,EXTINST 	,EXT_D9	,0	;D9
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_D9	,0	;D9

 xxh T_NULL	,T_NULL	,EXTINST 	,EXT_DA	,0	;DA
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_DA	,0	;DA

 xxh T_NULL	,T_NULL	,EXTINST 	,EXT_DB	,0	;DB
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_DB	,0	;DB

 xxh T_NULL	,T_NULL	,EXTINST 	,EXT_DC	,0	;DC
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_DC	,0	;DC

 xxh T_NULL	,T_NULL	,EXTINST 	,EXT_DD	,0	;DD
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_DD	,0	;DD

 xxh T_NULL	,T_NULL	,EXTINST 	,EXT_DE	,0	;DE
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_DE	,0	;DE

 xxh T_NULL	,T_NULL	,EXTINST 	,EXT_DF	,0	;DF
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_DF	,0	;DF
;
 xxh T_LOOPNZ	,T_NULL	,DODISPB	,0	,0	;E0
 xxh T_LOOPNZ	,T_NULL	,DODISPB	,0	,0	;E0

 xxh T_LOOPZ	,T_NULL	,DODISPB	,0	,0	;E1
 xxh T_LOOPZ	,T_NULL	,DODISPB	,0	,0	;E1

 xxh T_LOOP	,T_NULL	,DODISPB	,0	,0	;E2
 xxh T_LOOP	,T_NULL	,DODISPB	,0	,0	;E2

 xxh T_JCXZ	,T_NULL	,DODISPB	,0	,0	;E3
 xxh T_JECXZ	,T_NULL	,DODISPB	,0	,0	;E3

 xxh T_IN	,T_AL		,PORTIN 	,0	,0	;E4
 xxh T_IN	,T_AL		,PORTIN 	,0	,0	;E4

 xxh T_IN	,T_AX		,PORTIN 	,0	,0	;E5
 xxh T_IN	,T_EAX	,PORTIN 	,0	,0	;E5

 xxh T_OUT	,T_NULL	,PORTOUTB	,0	,0 	;E6
 xxh T_OUT	,T_NULL	,PORTOUTB	,0	,0 	;E6

 xxh T_OUT	,T_NULL	,PORTOUTW	,0	,0	;E7
 xxh T_OUT	,T_NULL	,PORTOUTW	,0	,0	;E7

 xxh T_CALL	,T_NULL	,DODISPW_C	,0	,0	;E8
 xxh T_CALL	,T_NULL	,DODISPD_C	,0	,0	;E8

 xxh T_JMP	,T_NULL	,DODISPW_J	,0	,0	;E9
 xxh T_JMP	,T_NULL	,DODISPD_J	,0	,0	;E9

 xxh T_JMP	,T_NULL	,INTERAD_J	,0	,0	;EA
 xxh T_JMP	,T_NULL	,INTERAD_J	,0	,0	;EA

 xxh T_JMP	,T_NULL	,DODISPB_J	,0	,0	;EB
 xxh T_JMP	,T_NULL	,DODISPB_J	,0	,0	;EB

 xxh T_INALDX	,T_NULL	,SetInOut 	,0	,0	;EC
 xxh T_INALDX	,T_NULL	,SetInOut	,0	,0	;EC

 xxh T_INAXDX	,T_NULL	,SetInOut 	,0	,0	;ED
 xxh T_INEAXDX	,T_NULL	,SetInOut 	,0	,0	;ED

 xxh T_OUTDXAL	,T_NULL	,SetInOut	,0	,0	;EE
 xxh T_OUTDXAL	,T_NULL	,SetInOut	,0	,0	;EE

 xxh T_OUTDXAX	,T_NULL	,SetInOut	,0	,0	;EF
 xxh T_OUTDXEAX ,T_NULL	,SetInOut	,0	,0	;EF
;
 xxh T_LOCK	,T_NULL	,NONE 	,0	,0	;F0
 xxh T_LOCK	,T_NULL	,NONE 	,0	,0	;F0

 xxh T_INV	,T_NULL	,Invalid 	,0	,0	;F1
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;F1

 xxh T_REPNE	,T_NULL	,ANOTHER	,0	,0	;F2
 xxh T_REPNE	,T_NULL	,ANOTHER	,0	,0	;F2

 xxh T_REP	,T_NULL	,UmREP	,0	,0	;F3
 xxh T_REP	,T_NULL	,UmREP	,0	,0	;F3

 xxh T_HLT	,T_NULL	,NONE 	,0	,0	;F4
 xxh T_HLT	,T_NULL	,NONE 	,0	,0	;F4

 xxh T_CMC	,T_NULL	,NONE 	,0	,0	;F5
 xxh T_CMC	,T_NULL	,NONE 	,0	,0	;F5

 xxh T_NULL	,T_NULL	,EXTINST	,EXT_F6	,0	;F6
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_F6	,0	;F6

 xxh T_NULL	,T_NULL	,EXTINST	,EXT_F7	,0	;F7
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_F7	,0	;F7

 xxh T_CLC	,T_NULL	,NONE 	,0	,0	;F8
 xxh T_CLC	,T_NULL	,NONE 	,0	,0	;F8

 xxh T_STC	,T_NULL	,NONE 	,0	,0	;F9
 xxh T_STC	,T_NULL	,NONE 	,0	,0	;F9

 xxh T_CLI	,T_NULL	,NONE 	,0	,0	;FA
 xxh T_CLI	,T_NULL	,NONE 	,0	,0	;FA

 xxh T_STI	,T_NULL	,NONE 	,0	,0	;FB
 xxh T_STI	,T_NULL	,NONE 	,0	,0	;FB

 xxh T_CLD	,T_NULL	,NONE 	,0	,0	;FC
 xxh T_CLD	,T_NULL	,NONE 	,0	,0	;FC

 xxh T_STD	,T_NULL	,NONE 	,0	,0	;FD
 xxh T_STD	,T_NULL	,NONE 	,0	,0	;FD

 xxh T_NULL	,T_NULL	,EXTINST	,EXT_FE	,0	;FE
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_FE	,0	;FE

 xxh T_NULL	,T_NULL	,EXTINST	,EXT_FF	,0	;FF
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_FF	,0	;FF


EXT_0F	label byte
	dd EXT_0F_0,0,EXT_0F_2,0,0,0,0,0,EXT_0F_8,EXT_0F_9,EXT_0F_A,EXT_0F_B,EXT_0F_C,0,0,0

EXT_0F_0	label byte
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_0F_00	,0	;00
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_0F_00	,0	;00

 xxh T_NULL	,T_NULL	,EXTINST	,EXT_0F_01	,0	;01
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_0F_01	,0	;01

 xxh T_LAR	,T_NULL	,REGRMW	,T_WORD	,0	;02
 xxh T_LAR	,T_NULL	,REGRMD	,T_DWORD	,0	;02

 xxh T_LSL	,T_NULL	,REGRMW	,T_WORD	,0	;03
 xxh T_LSL	,T_NULL	,REGRMD	,T_DWORD	,0	;03

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;04
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;04

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;05
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;05

 xxh T_CLTS	,T_NULL	,NONE	,0	,0	;06
 xxh T_CLTS	,T_NULL	,NONE	,0	,0	;06

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;07
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;07

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;08
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;08

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;09
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;09

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0A
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0A

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0B
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0B

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0C
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0C

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0D
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0D

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0E
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0E

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0F
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0F


EXT_0F_2	label byte
 xxh T_MOV	,T_NULL	,CRMREGD	,0	,0	;20
 xxh T_MOV	,T_NULL	,CRMREGD	,0	,0	;20

 xxh T_MOV	,T_NULL	,DRMREGD	,0	,0	;21
 xxh T_MOV	,T_NULL	,DRMREGD	,0	,0	;21

 xxh T_MOV	,T_NULL	,CREGRMD	,0	,0	;22
 xxh T_MOV	,T_NULL	,CREGRMD	,0	,0	;22

 xxh T_MOV	,T_NULL	,DREGRMD	,0	,0	;23
 xxh T_MOV	,T_NULL	,DREGRMD	,0	,0	;23

 xxh T_MOV	,T_NULL	,TRMREGD	,0	,0	;24
 xxh T_MOV	,T_NULL	,TRMREGD	,0	,0	;24

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;25
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;25

 xxh T_MOV	,T_NULL	,TREGRMD	,0	,0	;26
 xxh T_MOV	,T_NULL	,TREGRMD	,0	,0	;26

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;27
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;27

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;28
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;28

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;29
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;29

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2A
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2A

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2B
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2B

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2C
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2C

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2D
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2D

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2E
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2E

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2F
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2F


EXT_0F_8	label byte
 xxh T_JO	,T_NULL	,DODISPW	,0	,0	;80
 xxh T_JO	,T_NULL	,DODISPD	,0	,0	;80

 xxh T_JNO	,T_NULL	,DODISPW	,0	,0	;81
 xxh T_JNO	,T_NULL	,DODISPD	,0	,0	;81

 xxh T_JB	,T_NULL	,DODISPW	,0	,0	;82
 xxh T_JB	,T_NULL	,DODISPD	,0	,0	;82

 xxh T_JNB	,T_NULL	,DODISPW	,0	,0	;83
 xxh T_JNB	,T_NULL	,DODISPD	,0	,0	;83

 xxh T_JE	,T_NULL	,DODISPW	,0	,0	;84
 xxh T_JE	,T_NULL	,DODISPD	,0	,0	;84

 xxh T_JNE	,T_NULL	,DODISPW	,0	,0	;85
 xxh T_JNE	,T_NULL	,DODISPD	,0	,0	;85

 xxh T_JBE	,T_NULL	,DODISPW	,0	,0	;86
 xxh T_JBE	,T_NULL	,DODISPD	,0	,0	;86

 xxh T_JNBE	,T_NULL	,DODISPW	,0	,0	;87
 xxh T_JNBE	,T_NULL	,DODISPD	,0	,0	;87

 xxh T_JS	,T_NULL	,DODISPW	,0	,0	;88
 xxh T_JS	,T_NULL	,DODISPD	,0	,0	;88

 xxh T_JNS	,T_NULL	,DODISPW	,0	,0	;89
 xxh T_JNS	,T_NULL	,DODISPD	,0	,0	;89

 xxh T_JP	,T_NULL	,DODISPW	,0	,0	;8A
 xxh T_JP	,T_NULL	,DODISPD	,0	,0	;8A

 xxh T_JNP	,T_NULL	,DODISPW	,0	,0	;8B
 xxh T_JNP	,T_NULL	,DODISPD	,0	,0	;8B

 xxh T_JL	,T_NULL	,DODISPW	,0	,0	;8C
 xxh T_JL	,T_NULL	,DODISPD	,0	,0	;8C

 xxh T_JNL	,T_NULL	,DODISPW	,0	,0	;8D
 xxh T_JNL	,T_NULL	,DODISPD	,0	,0	;8D

 xxh T_JLE	,T_NULL	,DODISPW	,0	,0	;8E
 xxh T_JLE	,T_NULL	,DODISPD	,0	,0	;8E

 xxh T_JNLE	,T_NULL	,DODISPW	,0	,0	;8F
 xxh T_JNLE	,T_NULL	,DODISPD	,0	,0	;8F


EXT_0F_9	label byte
 xxh T_SETO	,T_NULL	,RMB	,T_BYTE	,0	;90
 xxh T_SETO	,T_NULL	,RMB	,T_BYTE	,0	;90

 xxh T_SETNO	,T_NULL	,RMB	,T_BYTE	,0	;91
 xxh T_SETNO	,T_NULL	,RMB	,T_BYTE	,0	;91

 xxh T_SETB	,T_NULL	,RMB	,T_BYTE	,0	;92
 xxh T_SETB	,T_NULL	,RMB	,T_BYTE	,0	;92

 xxh T_SETNB	,T_NULL	,RMB	,T_BYTE	,0	;93
 xxh T_SETNB	,T_NULL	,RMB	,T_BYTE	,0	;93

 xxh T_SETZ	,T_NULL	,RMB	,T_BYTE	,0	;94
 xxh T_SETZ	,T_NULL	,RMB	,T_BYTE	,0	;94

 xxh T_SETNZ	,T_NULL	,RMB	,T_BYTE	,0	;95
 xxh T_SETNZ	,T_NULL	,RMB	,T_BYTE	,0	;95

 xxh T_SETBE	,T_NULL	,RMB	,T_BYTE	,0	;96
 xxh T_SETBE	,T_NULL	,RMB	,T_BYTE	,0	;96

 xxh T_SETNBE	,T_NULL	,RMB	,T_BYTE	,0	;97
 xxh T_SETNBE	,T_NULL	,RMB	,T_BYTE	,0	;97

 xxh T_SETS	,T_NULL	,RMB	,T_BYTE	,0	;98
 xxh T_SETS	,T_NULL	,RMB	,T_BYTE	,0	;98

 xxh T_SETNS	,T_NULL	,RMB	,T_BYTE	,0	;99
 xxh T_SETNS	,T_NULL	,RMB	,T_BYTE	,0	;99

 xxh T_SETP	,T_NULL	,RMB	,T_BYTE	,0	;9A
 xxh T_SETP	,T_NULL	,RMB	,T_BYTE	,0	;9A

 xxh T_SETNP	,T_NULL	,RMB	,T_BYTE	,0	;9B
 xxh T_SETNP	,T_NULL	,RMB	,T_BYTE	,0	;9B

 xxh T_SETL	,T_NULL	,RMB	,T_BYTE	,0	;9C
 xxh T_SETL	,T_NULL	,RMB	,T_BYTE	,0	;9C

 xxh T_SETNL	,T_NULL	,RMB	,T_BYTE	,0	;9D
 xxh T_SETNL	,T_NULL	,RMB	,T_BYTE	,0	;9D

 xxh T_SETLE	,T_NULL	,RMB	,T_BYTE	,0	;9E
 xxh T_SETLE	,T_NULL	,RMB	,T_BYTE	,0	;9E

 xxh T_SETNLE	,T_NULL	,RMB	,T_BYTE	,0	;9F
 xxh T_SETNLE	,T_NULL	,RMB	,T_BYTE	,0	;9F


EXT_0F_A	label byte
 xxh T_PUSH	,T_FS		,NONE	,0	,0	;A0
 xxh T_PUSH	,T_FS		,NONE	,0	,0	;A0

 xxh T_POP	,T_FS		,NONE	,0	,0	;A1
 xxh T_POP	,T_FS		,NONE	,0	,0	;A1

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;A2
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;A2

 xxh T_BT	,T_NULL	,RMREGW	,T_WORD	,0	;A3
 xxh T_BT	,T_NULL	,RMREGD	,T_DWORD	,0	;A3

 xxh T_SHLD	,T_NULL	,SHLDIMBW	,T_WORD	,0	;A4
 xxh T_SHLD	,T_NULL	,SHLDIMBD	,T_DWORD	,0	;A4

 xxh T_SHLD	,T_NULL	,SHLDCLW	,T_WORD	,0	;A5
 xxh T_SHLD	,T_NULL	,SHLDCLD	,T_DWORD	,0	;A5

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;A6
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;A6

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;A7
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;A7

 xxh T_PUSH	,T_GS		,NONE	,0	,0	;A8
 xxh T_PUSH	,T_GS		,NONE	,0	,0	;A8

 xxh T_POP	,T_GS		,NONE	,0	,0	;A9
 xxh T_POP	,T_GS		,NONE	,0	,0	;A9

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;AA
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;AA

 xxh T_BTS	,T_NULL	,RMREGW	,T_WORD	,0	;AB
 xxh T_BTS	,T_NULL	,RMREGD	,T_DWORD	,0	;AB

 xxh T_SHRD	,T_NULL	,SHLDIMBW	,T_WORD	,0	;AC
 xxh T_SHRD	,T_NULL	,SHLDIMBD	,T_DWORD	,0	;AC

 xxh T_SHRD	,T_NULL	,SHLDCLW	,T_WORD	,0	;AD
 xxh T_SHRD	,T_NULL	,SHLDCLD	,T_DWORD	,0	;AD

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;AE
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;AE

 xxh T_IMUL	,T_NULL	,REGRMW	,T_WORD	,0	;AF
 xxh T_IMUL	,T_NULL	,REGRMD	,T_DWORD	,0	;AF


EXT_0F_B	label byte
 xxh T_CMPXCHG	,T_NULL	,RMREGB	,T_BYTE	,0	;B0
 xxh T_CMPXCHG	,T_NULL	,RMREGB	,T_BYTE	,0	;B0

 xxh T_CMPXCHG	,T_NULL	,RMREGW	,T_WORD	,0	;B1
 xxh T_CMPXCHG	,T_NULL	,RMREGD	,T_DWORD	,0	;B1

 xxh T_LSS	,T_NULL	,REGRMSW	,T_DWORD	,0	;B2
 xxh T_LSS	,T_NULL	,REGRMSF	,T_FWORD	,0	;B2

 xxh T_BTR	,T_NULL	,RMREGW	,T_WORD	,0	;B3
 xxh T_BTR	,T_NULL	,RMREGD	,T_DWORD	,0	;B3

 xxh T_LFS	,T_NULL	,REGRMSW 	,T_DWORD	,0	;B4
 xxh T_LFS	,T_NULL	,REGRMSF 	,T_FWORD	,0	;B4

 xxh T_LGS	,T_NULL	,REGRMSW 	,T_DWORD	,0	;B5
 xxh T_LGS	,T_NULL	,REGRMSF 	,T_FWORD	,0	;B5

 xxh T_MOVZX	,T_NULL	,MOVZXWB	,T_BYTE	,0	;B6
 xxh T_MOVZX	,T_NULL	,MOVZXDB	,T_BYTE	,0	;B6

 xxh T_MOVZX	,T_NULL	,MOVZXDW	,T_WORD	,0	;B7
 xxh T_MOVZX	,T_NULL	,MOVZXDW	,T_WORD	,0	;B7

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;B8
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;B8

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;B9
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;B9

 xxh T_NULL	,T_NULL	,EXTINST	,EXT_0F_BA	,0	;BA
 xxh T_NULL	,T_NULL	,EXTINST	,EXT_0F_BA	,0	;BA

 xxh T_BTC	,T_NULL	,RMREGW	,T_WORD	,0	;BB
 xxh T_BTC	,T_NULL	,RMREGD	,T_DWORD	,0	;BB

 xxh T_BSF	,T_NULL	,REGRMW	,T_WORD	,0	;BC
 xxh T_BSF	,T_NULL	,REGRMD	,T_DWORD	,0	;BC

 xxh T_BSR	,T_NULL	,REGRMW	,T_WORD	,0	;BD
 xxh T_BSR	,T_NULL	,REGRMD	,T_DWORD	,0	;BD

 xxh T_MOVSX	,T_NULL	,MOVZXWB	,T_BYTE	,0	;BE
 xxh T_MOVSX	,T_NULL	,MOVZXDB	,T_BYTE	,0	;BE

 xxh T_MOVSX	,T_NULL	,MOVZXDW	,T_WORD	,0	;BF
 xxh T_MOVSX	,T_NULL	,MOVZXDW	,T_WORD	,0	;BF


EXT_0F_C	label byte
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C0
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C0

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C1
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C1

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C2
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C2

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C3
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C3

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C4
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C4

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C5
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C5

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C6
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C6

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C7
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;C7

 xxh T_BSWAP	,T_EAX	,NONE	,0	,0	;C8
 xxh T_BSWAP	,T_EAX	,NONE	,0	,0	;C8

 xxh T_BSWAP	,T_ECX	,NONE	,0	,0	;C9
 xxh T_BSWAP	,T_ECX	,NONE	,0	,0	;C9

 xxh T_BSWAP	,T_EDX	,NONE	,0	,0	;CA
 xxh T_BSWAP	,T_EDX	,NONE	,0	,0	;CA

 xxh T_BSWAP	,T_EBX	,NONE	,0	,0	;CB
 xxh T_BSWAP	,T_EBX	,NONE	,0	,0	;CB

 xxh T_BSWAP	,T_ESP	,NONE	,0	,0	;CC
 xxh T_BSWAP	,T_ESP	,NONE	,0	,0	;CC

 xxh T_BSWAP	,T_EBP	,NONE	,0	,0	;CD
 xxh T_BSWAP	,T_EBP	,NONE	,0	,0	;CD

 xxh T_BSWAP	,T_ESI	,NONE	,0	,0	;CE
 xxh T_BSWAP	,T_ESI	,NONE	,0	,0	;CE

 xxh T_BSWAP	,T_EDI	,NONE	,0	,0	;CF
 xxh T_BSWAP	,T_EDI	,NONE	,0	,0	;CF


EXT_0F_00	label byte
 xxh T_SLDT	,T_NULL	,RMW	,T_WORD	,0	;0
 xxh T_SLDT	,T_NULL	,RMW	,T_WORD	,0	;0

 xxh T_STR	,T_NULL	,RMW	,T_WORD	,0	;1
 xxh T_STR	,T_NULL	,RMW	,T_WORD	,0	;1

 xxh T_LLDT	,T_NULL	,RMW	,T_WORD	,0	;2
 xxh T_LLDT	,T_NULL	,RMW	,T_WORD	,0	;2

 xxh T_LTR	,T_NULL	,RMW	,T_WORD	,0	;3
 xxh T_LTR	,T_NULL	,RMW	,T_WORD	,0	;3

 xxh T_VERR	,T_NULL	,RMW	,T_WORD	,0	;4
 xxh T_VERR	,T_NULL	,RMW	,T_WORD	,0	;4

 xxh T_VERW	,T_NULL	,RMW	,T_WORD	,0	;5
 xxh T_VERW	,T_NULL	,RMW	,T_WORD	,0	;5

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;6
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;6

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;7
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;7


EXT_0F_01	label byte
 xxh T_SGDT	,T_NULL	,RMWF	,T_FWORD	,0	;0
 xxh T_SGDT	,T_NULL	,RMWF	,T_FWORD	,0	;0

 xxh T_SIDT	,T_NULL	,RMWF	,T_FWORD	,0	;1
 xxh T_SIDT	,T_NULL	,RMWF	,T_FWORD	,0	;1

 xxh T_LGDT	,T_NULL	,RMWF	,T_FWORD	,0	;2
 xxh T_LGDT	,T_NULL	,RMWF	,T_FWORD	,0	;2

 xxh T_LIDT	,T_NULL	,RMWF	,T_FWORD	,0	;3
 xxh T_LIDT	,T_NULL	,RMWF	,T_FWORD	,0	;3

 xxh T_SMSW	,T_NULL	,RMW	,T_WORD	,0	;4
 xxh T_SMSW	,T_NULL	,RMW	,T_WORD	,0	;4

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;5
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;5

 xxh T_LMSW	,T_NULL	,RMW	,T_WORD	,0	;6
 xxh T_LMSW	,T_NULL	,RMW	,T_WORD	,0	;6

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;7
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;7


EXT_0F_BA	label byte
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;1
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;1

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;2

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;3
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;3

 xxh T_BT	,T_NULL	,RMIMMW8	,T_WORD	,0	;4
 xxh T_BT	,T_NULL	,RMIMMD8	,T_DWORD	,0	;4

 xxh T_BTS	,T_NULL	,RMIMMW8	,T_WORD	,0	;5
 xxh T_BTS	,T_NULL	,RMIMMD8	,T_DWORD	,0	;5

 xxh T_BTR	,T_NULL	,RMIMMW8	,T_WORD	,0	;6
 xxh T_BTR	,T_NULL	,RMIMMD8	,T_DWORD	,0	;6

 xxh T_BTC	,T_NULL	,RMIMMW8	,T_WORD	,0	;7
 xxh T_BTC	,T_NULL	,RMIMMD8	,T_DWORD	,0	;7


EXT_80	label byte
 xxh T_ADD	,T_NULL	,RMIMMB 	,T_BYTE	,0	;0
 xxh T_ADD	,T_NULL	,RMIMMB 	,T_BYTE	,0	;0

 xxh T_OR 	,T_NULL	,RMIMMB 	,T_BYTE	,0	;1
 xxh T_OR 	,T_NULL	,RMIMMB 	,T_BYTE	,0	;1

 xxh T_ADC	,T_NULL	,RMIMMB 	,T_BYTE	,0	;2
 xxh T_ADC	,T_NULL	,RMIMMB 	,T_BYTE	,0	;2

 xxh T_SBB	,T_NULL	,RMIMMB 	,T_BYTE	,0	;3
 xxh T_SBB	,T_NULL	,RMIMMB 	,T_BYTE	,0	;3

 xxh T_AND	,T_NULL	,RMIMMB 	,T_BYTE	,0	;4
 xxh T_AND	,T_NULL	,RMIMMB 	,T_BYTE	,0	;4

 xxh T_SUB	,T_NULL	,RMIMMB 	,T_BYTE	,0	;5
 xxh T_SUB	,T_NULL	,RMIMMB 	,T_BYTE	,0	;5

 xxh T_XOR	,T_NULL	,RMIMMB 	,T_BYTE	,0	;6
 xxh T_XOR	,T_NULL	,RMIMMB 	,T_BYTE	,0	;6

 xxh T_CMP	,T_NULL	,RMIMMB 	,T_BYTE	,0	;7
 xxh T_CMP	,T_NULL	,RMIMMB 	,T_BYTE	,0	;7


EXT_81	label byte
 xxh T_ADD	,T_NULL	,RMIMMW 	,T_WORD	,0	;0
 xxh T_ADD	,T_NULL	,RMIMMD 	,T_DWORD	,0	;0

 xxh T_OR 	,T_NULL	,RMIMMW 	,T_WORD	,0	;1
 xxh T_OR 	,T_NULL	,RMIMMD 	,T_DWORD	,0	;1

 xxh T_ADC	,T_NULL	,RMIMMW 	,T_WORD	,0	;2
 xxh T_ADC	,T_NULL	,RMIMMD 	,T_DWORD	,0	;2

 xxh T_SBB	,T_NULL	,RMIMMW 	,T_WORD	,0	;3
 xxh T_SBB	,T_NULL	,RMIMMD 	,T_DWORD	,0	;3

 xxh T_AND	,T_NULL	,RMIMMW 	,T_WORD	,0	;4
 xxh T_AND	,T_NULL	,RMIMMD 	,T_DWORD	,0	;4

 xxh T_SUB	,T_NULL	,RMIMMW 	,T_WORD	,0	;5
 xxh T_SUB	,T_NULL	,RMIMMD 	,T_DWORD	,0	;5

 xxh T_XOR	,T_NULL	,RMIMMW 	,T_WORD	,0	;6
 xxh T_XOR	,T_NULL	,RMIMMD 	,T_DWORD	,0	;6

 xxh T_CMP	,T_NULL	,RMIMMW 	,T_WORD	,0	;7
 xxh T_CMP	,T_NULL	,RMIMMD 	,T_DWORD	,0	;7


EXT_83	label byte
 xxh T_ADD	,T_NULL	,RMIMMW8	,T_WORD	,0	;0
 xxh T_ADD	,T_NULL	,RMIMMD8	,T_DWORD	,0	;0

 xxh T_OR 	,T_NULL	,RMIMMW8	,T_WORD	,0	;1
 xxh T_OR 	,T_NULL	,RMIMMD8	,T_DWORD	,0	;1

 xxh T_ADC	,T_NULL	,RMIMMW8	,T_WORD	,0	;2
 xxh T_ADC	,T_NULL	,RMIMMD8	,T_DWORD	,0	;2

 xxh T_SBB	,T_NULL	,RMIMMW8	,T_WORD	,0	;3
 xxh T_SBB	,T_NULL	,RMIMMD8	,T_DWORD	,0	;3

 xxh T_AND 	,T_NULL 	,RMIMMW8	,T_WORD	,0	;4
 xxh T_AND 	,T_NULL 	,RMIMMD8	,T_DWORD	,0	;4

 xxh T_SUB	,T_NULL	,RMIMMW8	,T_WORD	,0	;5
 xxh T_SUB	,T_NULL	,RMIMMD8	,T_DWORD	,0	;5

 xxh T_XOR 	,T_NULL	,RMIMMW8	,T_WORD	,0	;6
 xxh T_XOR 	,T_NULL	,RMIMMD8	,T_DWORD	,0	;6

 xxh T_CMP	,T_NULL	,RMIMMW8	,T_WORD	,0	;7
 xxh T_CMP	,T_NULL	,RMIMMD8	,T_DWORD	,0	;7


EXT_C0	label byte
 xxh T_ROL	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;0
 xxh T_ROL	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;0

 xxh T_ROR	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;1
 xxh T_ROR	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;1

 xxh T_RCL	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;2
 xxh T_RCL	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;2

 xxh T_RCR	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;3
 xxh T_RCR	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;3

 xxh T_SHL	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;4
 xxh T_SHL	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;4

 xxh T_SHR	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;5
 xxh T_SHR	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;5

 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;6
 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;6

 xxh T_SAR	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;7
 xxh T_SAR	,T_NULL	,SHIFT1BIMB	,T_BYTE	,0	;7


EXT_C1	label byte
 xxh T_ROL	,T_NULL	,SHIFT1WIMB	,T_WORD	,0	;0
 xxh T_ROL	,T_NULL	,SHIFT1DIMB	,T_DWORD	,0	;0

 xxh T_ROR	,T_NULL	,SHIFT1WIMB	,T_WORD	,0	;1
 xxh T_ROR	,T_NULL	,SHIFT1DIMB	,T_DWORD	,0	;1

 xxh T_RCL	,T_NULL	,SHIFT1WIMB	,T_WORD	,0	;2
 xxh T_RCL	,T_NULL	,SHIFT1DIMB	,T_DWORD	,0	;2

 xxh T_RCR	,T_NULL	,SHIFT1WIMB	,T_WORD	,0	;3
 xxh T_RCR	,T_NULL	,SHIFT1DIMB	,T_DWORD	,0	;3

 xxh T_SHL	,T_NULL	,SHIFT1WIMB	,T_WORD	,0	;4
 xxh T_SHL	,T_NULL	,SHIFT1DIMB	,T_DWORD	,0	;4

 xxh T_SHR	,T_NULL	,SHIFT1WIMB	,T_WORD	,0	;5
 xxh T_SHR	,T_NULL	,SHIFT1DIMB	,T_DWORD	,0	;5

 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;6
 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;6

 xxh T_SAR	,T_NULL	,SHIFT1WIMB	,T_WORD	,0	;7
 xxh T_SAR	,T_NULL	,SHIFT1DIMB	,T_DWORD	,0	;7


EXT_D0	label byte
 xxh T_ROL	,T_NULL	,SHIFT1B	,T_BYTE	,0	;0
 xxh T_ROL	,T_NULL	,SHIFT1B	,T_BYTE	,0	;0

 xxh T_ROR	,T_NULL	,SHIFT1B	,T_BYTE	,0	;1
 xxh T_ROR	,T_NULL	,SHIFT1B	,T_BYTE	,0	;1

 xxh T_RCL	,T_NULL	,SHIFT1B	,T_BYTE	,0	;2
 xxh T_RCL	,T_NULL	,SHIFT1B	,T_BYTE	,0	;2

 xxh T_RCR	,T_NULL	,SHIFT1B	,T_BYTE	,0	;3
 xxh T_RCR	,T_NULL	,SHIFT1B	,T_BYTE	,0	;3

 xxh T_SHL	,T_NULL	,SHIFT1B	,T_BYTE	,0	;4
 xxh T_SHL	,T_NULL	,SHIFT1B	,T_BYTE	,0	;4

 xxh T_SHR	,T_NULL	,SHIFT1B	,T_BYTE	,0	;5
 xxh T_SHR	,T_NULL	,SHIFT1B	,T_BYTE	,0	;5

 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;6
 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;6

 xxh T_SAR	,T_NULL	,SHIFT1B	,T_BYTE	,0	;7
 xxh T_SAR	,T_NULL	,SHIFT1B	,T_BYTE	,0	;7


EXT_D1	label byte
 xxh T_ROL	,T_NULL	,SHIFT1W	,T_WORD	,0	;0
 xxh T_ROL	,T_NULL	,SHIFT1D	,T_DWORD	,0	;0

 xxh T_ROR	,T_NULL	,SHIFT1W	,T_WORD	,0	;1
 xxh T_ROR	,T_NULL	,SHIFT1D	,T_DWORD	,0	;1

 xxh T_RCL	,T_NULL	,SHIFT1W	,T_WORD	,0	;2
 xxh T_RCL	,T_NULL	,SHIFT1D	,T_DWORD	,0	;2

 xxh T_RCR	,T_NULL	,SHIFT1W	,T_WORD	,0	;3
 xxh T_RCR	,T_NULL	,SHIFT1D	,T_DWORD	,0	;3

 xxh T_SHL	,T_NULL	,SHIFT1W	,T_WORD	,0	;4
 xxh T_SHL	,T_NULL	,SHIFT1D	,T_DWORD	,0	;4

 xxh T_SHR	,T_NULL	,SHIFT1W	,T_WORD	,0	;5
 xxh T_SHR	,T_NULL	,SHIFT1D	,T_DWORD	,0	;5

 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;6
 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;6

 xxh T_SAR	,T_NULL	,SHIFT1W	,T_WORD	,0	;7
 xxh T_SAR	,T_NULL	,SHIFT1D	,T_DWORD	,0	;7


EXT_D2	label byte
 xxh T_ROL	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;0
 xxh T_ROL	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;0

 xxh T_ROR	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;1
 xxh T_ROR	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;1

 xxh T_RCL	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;2
 xxh T_RCL	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;2

 xxh T_RCR	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;3
 xxh T_RCR	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;3

 xxh T_SHL	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;4
 xxh T_SHL	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;4

 xxh T_SHR	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;5
 xxh T_SHR	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;5

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;6
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;6

 xxh T_SAR	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;7
 xxh T_SAR	,T_NULL	,SHIFTCLB	,T_BYTE	,0	;7


EXT_D3	label byte
 xxh T_ROL	,T_NULL	,SHIFTCLW	,T_WORD	,0	;0
 xxh T_ROL	,T_NULL	,SHIFTCLD	,T_DWORD	,0	;0

 xxh T_ROR	,T_NULL	,SHIFTCLW	,T_WORD	,0	;1
 xxh T_ROR	,T_NULL	,SHIFTCLD	,T_DWORD	,0	;1

 xxh T_RCL	,T_NULL	,SHIFTCLW	,T_WORD	,0	;2
 xxh T_RCL	,T_NULL	,SHIFTCLD	,T_DWORD	,0	;2

 xxh T_RCR	,T_NULL	,SHIFTCLW	,T_WORD	,0	;3
 xxh T_RCR	,T_NULL	,SHIFTCLD	,T_DWORD	,0	;3

 xxh T_SHL	,T_NULL	,SHIFTCLW	,T_WORD	,0	;4
 xxh T_SHL	,T_NULL	,SHIFTCLD	,T_DWORD	,0	;4

 xxh T_SHR	,T_NULL	,SHIFTCLW	,T_WORD	,0	;5
 xxh T_SHR	,T_NULL	,SHIFTCLD	,T_DWORD	,0	;5

 xxh T_INV 	,T_NULL	,Invalid	,0	,0	;6
 xxh T_INV 	,T_NULL	,Invalid	,0	,0	;6

 xxh T_SAR	,T_NULL	,SHIFTCLW	,T_WORD	,0	;7
 xxh T_SAR	,T_NULL	,SHIFTCLD	,T_DWORD	,0	;7


EXT_D8	label byte
 xxh T_NULL	,T_NULL	,ESC_0_000w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_0_000d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_0_001w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_0_001d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_0_010w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_0_010d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_0_011w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_0_011d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_0_100w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_0_100d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_0_101w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_0_101d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_0_110w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_0_110d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_0_111w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_0_111d	,0	,0	;0


EXT_D9	label byte
 xxh T_NULL	,T_NULL	,ESC_1_000w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_1_000d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_1_001w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_1_001d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_1_010w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_1_010d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_1_011w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_1_011d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_1_100w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_1_100d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_1_101w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_1_101d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_1_110w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_1_110d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_1_111w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_1_111d	,0	,0	;0


EXT_DA	label byte
 xxh T_NULL	,T_NULL	,ESC_2_000w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_2_000d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_2_001w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_2_001d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_2_010w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_2_010d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_2_011w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_2_011d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_2_100w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_2_100d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_2_101w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_2_101d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_2_110w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_2_110d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_2_111w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_2_111d	,0	,0	;0


EXT_DB	label byte
 xxh T_NULL	,T_NULL	,ESC_3_000w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_3_000d	,0	,0	;0

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_3_010w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_3_010d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_3_011w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_3_011d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_3_100w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_3_100d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_3_101w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_3_101d	,0	,0	;0

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_3_111w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_3_111d	,0	,0	;0


EXT_DC	label byte
 xxh T_NULL	,T_NULL	,ESC_4_000w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_4_000d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_4_001w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_4_001d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_4_010w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_4_010d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_4_011w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_4_011d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_4_100w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_4_100d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_4_101w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_4_101d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_4_110w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_4_110d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_4_111w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_4_111d	,0	,0	;0


EXT_DD	label byte
 xxh T_NULL	,T_NULL	,ESC_5_000w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_5_000d	,0	,0	;0

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0
 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_5_010w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_5_010d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_5_011w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_5_011d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_5_100w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_5_100d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_5_101w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_5_101d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_5_110w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_5_110d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_5_111w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_5_111d	,0	,0	;0


EXT_DE	label byte
 xxh T_NULL	,T_NULL	,ESC_6_000w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_6_000d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_6_001w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_6_001d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_6_010w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_6_010d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_6_011w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_6_011d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_6_100w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_6_100d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_6_101w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_6_101d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_6_110w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_6_110d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_6_111w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_6_111d	,0	,0	;0


EXT_DF	label byte
 xxh T_NULL	,T_NULL	,ESC_7_000w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_7_000d	,0	,0	;0

 xxh T_INV	,T_NULL	,Invalid	,0	,0	;0
 xxh T_NULL	,T_NULL	,Invalid	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_7_010w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_7_010d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_7_011w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_7_011d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_7_100w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_7_100d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_7_101w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_7_101d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_7_110w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_7_110d	,0	,0	;0

 xxh T_NULL	,T_NULL	,ESC_7_111w	,0	,0	;0
 xxh T_NULL	,T_NULL	,ESC_7_111d	,0	,0	;0


EXT_F6	label byte
 xxh T_TEST	,T_NULL	,RMIMMB	,T_BYTE	,0	;0
 xxh T_TEST	,T_NULL	,RMIMMB	,T_BYTE	,0	;0

 xxh T_INV 	,T_NULL	,Invalid	,0	,0	;1
 xxh T_INV 	,T_NULL	,Invalid	,0	,0	;1

 xxh T_NOT	,T_NULL	,RMONLYB	,T_BYTE	,0	;2
 xxh T_NOT	,T_NULL	,RMONLYB	,T_BYTE	,0	;2

 xxh T_NEG	,T_NULL	,RMONLYB	,T_BYTE	,0	;3
 xxh T_NEG	,T_NULL	,RMONLYB	,T_BYTE	,0	;3

 xxh T_MUL	,T_NULL	,RMONLYB	,T_BYTE	,0	;4
 xxh T_MUL	,T_NULL	,RMONLYB	,T_BYTE	,0	;4

 xxh T_IMUL	,T_NULL	,RMONLYB	,T_BYTE	,0	;5
 xxh T_IMUL	,T_NULL	,RMONLYB	,T_BYTE	,0	;5

 xxh T_DIV	,T_NULL	,RMONLYB	,T_BYTE	,0	;6
 xxh T_DIV	,T_NULL	,RMONLYB	,T_BYTE	,0	;6

 xxh T_IDIV	,T_NULL	,RMONLYB	,T_BYTE	,0	;7
 xxh T_IDIV	,T_NULL	,RMONLYB	,T_BYTE	,0	;7


EXT_F7	label byte
 xxh T_TEST	,T_NULL	,RMIMMW 	,T_WORD	,0	;0
 xxh T_TEST	,T_NULL	,RMIMMD 	,T_DWORD	,0	;0

 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;1
 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;1

 xxh T_NOT	,T_NULL	,RMONLYW	,T_WORD	,0	;2
 xxh T_NOT	,T_NULL	,RMONLYD	,T_DWORD	,0	;2

 xxh T_NEG	,T_NULL	,RMONLYW	,T_WORD	,0	;3
 xxh T_NEG	,T_NULL	,RMONLYD	,T_DWORD	,0	;3

 xxh T_MUL	,T_NULL	,RMONLYW	,T_WORD	,0	;4
 xxh T_MUL	,T_NULL	,RMONLYD	,T_DWORD	,0	;4

 xxh T_IMUL	,T_NULL	,RMONLYW	,T_WORD	,0	;5
 xxh T_IMUL	,T_NULL	,RMONLYD	,T_DWORD	,0	;5

 xxh T_DIV	,T_NULL	,RMONLYW	,T_WORD	,0	;6
 xxh T_DIV	,T_NULL	,RMONLYD	,T_DWORD	,0	;6

 xxh T_IDIV	,T_NULL	,RMONLYW	,T_WORD	,0	;7
 xxh T_IDIV	,T_NULL	,RMONLYD	,T_DWORD	,0	;7


EXT_FE	label byte
 xxh T_INC	,T_NULL	,RMONLYB	,T_BYTE	,0	;0
 xxh T_INC	,T_NULL	,RMONLYB	,T_BYTE	,0	;0

 xxh T_DEC	,T_NULL	,RMONLYB	,T_BYTE	,0	;1
 xxh T_DEC	,T_NULL	,RMONLYB	,T_BYTE	,0	;1

 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;2
 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;2

 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;3
 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;3

 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;4
 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;4

 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;5
 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;5

 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;6
 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;6

 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;7
 xxh T_INV 	,T_NULL	,Invalid 	,0	,0	;7


EXT_FF	label byte
 xxh T_INC	,T_NULL	,RMONLYW	,T_WORD	,0	;0
 xxh T_INC	,T_NULL	,RMONLYD	,T_DWORD	,0	;0

 xxh T_DEC	,T_NULL	,RMONLYW	,T_WORD	,0	;1
 xxh T_DEC	,T_NULL	,RMONLYD	,T_DWORD	,0	;1

 xxh T_CALL	,T_NULL	,RMONLY_NW_C	,T_WORD	,0	;2
 xxh T_CALL	,T_NULL	,RMONLY_ND_C	,T_DWORD	,0	;2

 xxh T_CALL	,T_NULL	,RMONLY_FW_C	,T_DWORD	,0	;3
 xxh T_CALL	,T_NULL	,RMONLY_FD_C	,T_FWORD	,0	;3

 xxh T_JMP	,T_NULL	,RMONLY_NW_J	,T_WORD	,0	;4
 xxh T_JMP	,T_NULL	,RMONLY_ND_J	,T_DWORD	,0	;4

 xxh T_JMP	,T_NULL	,RMONLY_FW_J	,T_DWORD	,0	;5
 xxh T_JMP	,T_NULL	,RMONLY_FD_J	,T_FWORD	,0	;5

 xxh T_PUSH	,T_NULL	,RMONLYW	,T_WORD	,0	;6
 xxh T_PUSH	,T_NULL	,RMONLYD	,T_DWORD	,0	;6

 xxh T_INV	,T_NULL	,Invalid 	,0	,0	;7
 xxh T_INV	,T_NULL	,Invalid 	,0	,0	;7


T_AAA	db 'aaa',0
T_AAD	db 'aad',0
T_AAM	db 'aam',0
T_AAS	db 'aas',0
T_ADC	db 'adc   ',0
T_ADD	db 'add   ',0
T_AND	db 'and   ',0
T_INTO	db 'into',0
T_CALL	db 'call  ',0
T_CBW	db 'cbw',0
T_CWDE	db 'cwde',0
T_CLC	db 'clc',0
T_CLD	db 'cld',0
T_CLI	db 'cli',0
T_CMC	db 'cmc',0
T_CMP	db 'cmp   ',0
T_CWD	db 'cwd',0
T_CDQ	db 'cdq',0
T_DAA	db 'daa',0
T_DAS	db 'das',0
T_DEC	db 'dec   ',0
T_DIV	db 'div   ',0
T_ESC	db 'esc   ',0
T_HLT	db 'hlt',0
T_IDIV	db 'idiv  ',0
T_IMUL	db 'imul  ',0
T_IN	db 'in    ',0
T_INC	db 'inc   ',0
T_INT	db 'int   ',0
T_IRET	db 'iret  ',0
T_JA	db 'ja    ',0
T_JAE	db 'jae   ',0
T_JB	db 'jb    ',0
T_JBE	db 'jbe   ',0
T_JC	db 'jc    ',0
T_JE	db 'je    ',0
T_JG	db 'jg    ',0
T_JGE	db 'jge   ',0
T_JL	db 'jl    ',0
T_JLE	db 'jle   ',0
T_JNA	db 'jna   ',0
T_JNAE	db 'jnae  ',0
T_JNB	db 'jnb   ',0
T_JNBE	db 'jnbe  ',0
T_JNC	db 'jnc   ',0
T_JNE	db 'jne   ',0
T_JNG	db 'jng   ',0
T_JNGE	db 'jnge  ',0
T_JNL	db 'jnl   ',0
T_JNLE	db 'jnle  ',0
T_JNO	db 'jno   ',0
T_JNP	db 'jnp   ',0
T_JNS	db 'jns   ',0
T_JNZ	db 'jnz   ',0
T_JO	db 'jo    ',0
T_JP	db 'jp    ',0
T_JPE	db 'jpe   ',0
T_JPO	db 'jpo   ',0
T_JS	db 'js    ',0
T_JZ	db 'jz    ',0
T_JCXZ	db 'jcxz  ',0
T_JECXZ	db 'jecxz ',0
T_JMP	db 'jmp   ',0
T_LAHF	db 'lahf  ',0
T_LDS	db 'lds   ',0
T_LES	db 'les   ',0
T_LSS	db 'lss   ',0
T_LFS	db 'lfs   ',0
T_LGS	db 'lgs   ',0
T_LEA	db 'lea   ',0
T_LOCK	db 'lock',0
T_LOOPE	db 'loope ',0
T_LOOPNE	db 'loopne ',0
T_LOOPZ	db 'loopz ',0
T_LOOPNZ	db 'loopnz ',0
T_LOOP	db 'loop  ',0
T_MOV	db 'mov   ',0
T_MOVZX	db 'movzx ',0
T_MOVSX	db 'movsx ',0
T_MUL	db 'mul   ',0
T_NEG	db 'neg   ',0
T_NOP	db 'nop',0
T_NOT	db 'not   ',0
T_OR	db 'or    ',0
T_OUT	db 'out   ',0
T_POP	db 'pop   ',0
T_POPA	db 'popa',0
T_POPAD	db 'popad',0
T_POPF	db 'popf',0
T_POPFD	db 'popfd',0
T_PUSH	db 'push  ',0
T_PUSHA	db 'pusha',0
T_PUSHAD	db 'pushad',0
T_PUSHF	db 'pushf',0
T_PUSHFD	db 'pushfd',0
T_RCL	db 'rcl   ',0
T_RCR	db 'rcr   ',0
T_REP	db 'rep',0
T_REPE	db 'repe  ',0
T_REPZ	db 'repz  ',0
T_REPNE	db 'repne ',0
T_REPNZ	db 'repnz ',0
T_RET	db 'ret   ',0
T_RETF	db 'retf  ',0
T_ROL	db 'rol   ',0
T_ROR	db 'ror   ',0
T_SAHF	db 'sahf',0
T_SHL	db 'shl   ',0
T_SAR	db 'sar   ',0
T_SBB	db 'sbb   ',0
T_SHR	db 'shr   ',0
T_STC	db 'stc',0
T_STD	db 'std',0
T_STI	db 'sti',0
T_CMPSB	db 'cmpsb ',0
T_CMPSW	db 'cmpsw ',0
T_CMPSD	db 'cmpsd ',0
T_MOVSB	db 'movsb ',0
T_MOVSW	db 'movsw ',0
T_MOVSD	db 'movsd ',0
T_SCASB	db 'scasb ',0
T_SCASW	db 'scasw ',0
T_SCASD	db 'scasd ',0
T_LODSB	db 'lodsb ',0
T_LODSW	db 'lodsw ',0
T_LODSD	db 'lodsd ',0
T_STOSB	db 'stosb ',0
T_STOSW	db 'stosw ',0
T_STOSD	db 'stosd ',0
T_SUB	db 'sub   ',0
T_TEST	db 'test  ',0
T_WAIT	db 'wait  ',0
T_XCHG	db 'xchg  ',0
T_SETO	db 'seto  ',0
T_SETNO	db 'setno ',0
T_SETB	db 'setb  ',0
T_SETNB	db 'setnb ',0
T_SETZ	db 'setz  ',0
T_SETNZ	db 'setnz ',0
T_SETBE	db 'setbe ',0
T_SETNBE	db 'setnbe ',0
T_SETS	db 'sets  ',0
T_SETNS	db 'setns ',0
T_SETP	db 'setp  ',0
T_SETNP	db 'setnp ',0
T_SETL	db 'setl  ',0
T_SETNL	db 'setnl ',0
T_SETLE	db 'setle ',0
T_SETNLE	db 'setnle ',0
T_XLAT	db 'xlat',0
T_XOR	db 'xor   ',0
T_INALDX	db 'in    al,dx',0
T_INAXDX	db 'in    ax,dx',0
T_INEAXDX	db 'in    eax,dx',0
T_OUTDXAL	db 'out   dx,al',0
T_OUTDXAX	db 'out   dx,ax',0
T_OUTDXEAX	db 'out   dx,eax',0
T_CLTS	db 'clts',0
T_SLDT	db 'sldt  ',0
T_STR	db 'str   ',0
T_LLDT	db 'lldt  ',0
T_LTR	db 'ltr   ',0
T_VERR	db 'verr  ',0
T_VERW	db 'verw  ',0
T_SGDT	db 'sgdt  ',0
T_SIDT	db 'sidt  ',0
T_LGDT	db 'lgdt  ',0
T_LIDT	db 'lidt  ',0
T_SMSW	db 'smsw  ',0
T_LMSW	db 'lmsw  ',0
T_LAR	db 'lar   ',0
T_LSL	db 'lsl   ',0
T_BT	db 'bt    ',0
T_BTS	db 'bts   ',0
T_BTR	db 'btr   ',0
T_BTC	db 'btc   ',0
T_BSF	db 'bsf   ',0
T_BSR	db 'bsr   ',0
T_SHLD	db 'shld  ',0
T_SHRD	db 'shrd  ',0
T_BOUND	db 'bound ',0
T_ARPL	db 'arpl  ',0
T_INSB	db 'insb',0
T_INSW	db 'insw',0
T_INSD	db 'insd',0
T_OUTSB	db 'outsb',0
T_OUTSW	db 'outsw',0
T_OUTSD	db 'outsd',0
T_ENTER	db 'enter ',0
T_LEAVE	db 'leave',0
T_BSWAP	db 'bswap ',0
T_CMPXCHG	db 'cmpxchg ',0
T_BYTE	db 'byte ptr ',0
T_WORD	db 'word ptr ',0
T_DWORD	db 'dword ptr ',0
T_FWORD	db 'fword ptr ',0
T_QWORD	db 'qword ptr ',0
T_TBYTE	db 'tbyte ptr ',0
T_OFFSET	db 'offset ',0
T_INV	db '??',0
T_SHIFT	db ' ',0
T_IMMED	db ' ',0
T_NULL	db 0
T_GRP1	db ' ',0
T_GRP2	db ' ',0

T_FLD_Table	dd T_FLD1,T_FLDL2T,T_FLDL2E,T_FLDPI,T_FLDLG2,T_FLDLN2,T_FLDZ,T_INV
T_E110_Table	dd T_F2XM1,T_FYL2X,T_FPTAN,T_FPATAN,T_FXTRACT,T_FPREM1,T_FDECSTP,T_FINCSTP
T_E111_Table	dd T_FPREM,T_FYL2XP1,T_FSQRT,T_FSINCOS,T_FRNDINT,T_FSCALE,T_FSIN,T_FCOS

T_FLD	db "fld   ",0
T_FXCH	db "fxch  ",0
T_FNOP	db "fnop  ",0
T_FST	db "fst   ",0
T_FSTP	db "fstp  ",0
T_FABS	db "fabs  ",0
T_FCHS	db "fchs  ",0
T_FLDENV	db "fldenv ",0
T_FXAM	db "fxam  ",0
T_FTST	db "ftst  ",0
T_FLD1	db "fld1  ",0
T_FLDL2T	db "fldl2t ",0
T_FLDL2E	db "fldl2e ",0
T_FLDPI	db "fldpi ",0
T_FLDLG2	db "fldlg2 ",0
T_FLDLN2	db "fldln2 ",0
T_FLDZ	db "fldz  ",0
T_FLDCW	db "fldcw ",0
T_FSTENV	db "fstenv ",0
T_FNSTENV	db "fnstenv ",0
T_F2XM1	db "f2xm1 ",0
T_FPTAN	db "fptan ",0
T_FPATAN	db "fpatan ",0
T_FPREM1	db "fprem1 ",0
T_FDECSTP	db "fdecstp ",0
T_FINCSTP	db "fincstp ",0
T_FSTCW	db "fstcw ",0
T_FNSTCW	db "fnstcw ",0
T_FPREM	db "fprem ",0
T_FSQRT	db "fsqrt ",0
T_FSINCOS	db "fsincos ",0
T_FRNDINT	db "frndint ",0
T_FSCALE	db "fscale ",0
T_FSIN	db "fsin  ",0
T_FCOS	db "fcos  ",0
T_FYL2X	db "fyl2x ",0
T_FYL2XP1	db "fyl2xp1 ",0
T_FXTRACT	db "fxtract ",0
T_FADD	db "fadd  ",0
T_FMUL	db "fmul  ",0
T_FCOM	db "fcom  ",0
T_FCOMP	db "fcomp ",0
T_FUCOM	db "fucom ",0
T_FUCOMP	db "fucomp ",0
T_FSUB	db "fsub  ",0
T_FSUBR	db "fsubr ",0
T_FDIV	db "fdiv  ",0
T_FDIVR	db "fdivr ",0
T_FIADD	db "fiadd ",0
T_FIMUL	db "fimul ",0
T_FICOM	db "ficom ",0
T_FICOMP	db "ficomp ",0
T_FISUB	db "fisub ",0
T_FISUBR	db "fisubr ",0
T_FUCOMPP	db "fucompp ",0
T_FIDIV	db "fidiv ",0
T_FIDIVR	db "fidivr ",0
T_FILD	db "fild  ",0
T_FIST	db "fist  ",0
T_FISTP	db "fistp ",0
T_FNCLEX	db "fnclex ",0
T_FCLEX	db "fclex ",0
T_FINIT	db "finit ",0
T_FNINIT	db "fninit ",0
T_FFREE	db "ffree ",0
T_FRSTOR	db "frstor ",0
T_FSAVE	db "fsave ",0
T_FNSAVE	db "fnsave ",0
T_FADDP	db "faddp ",0
T_FMULP	db "fmulp ",0
T_FCOMPP	db "fcompp ",0
T_FSUBRP	db "fsubrp ",0
T_FSUBP	db "fsubp ",0
T_FDIVRP	db "fdivrp ",0
T_FDIVP	db "fdivp ",0
T_FSTSW	db "fstsw ",0
T_FNSTSW	db "fnstsw ",0
T_FBSTP	db "fbstp ",0
T_FBLD	db "fbld  ",0
T_FNSTSW_AX	db "fnstsw ax",0

T_ByteRegs	dd T_AL,T_CL,T_DL,T_BL,T_AH,T_CH,T_DH,T_BH
T_AL	db 'al',0
T_CL	db 'cl',0
T_DL	db 'dl',0
T_BL	db 'bl',0
T_AH	db 'ah',0
T_CH	db 'ch',0
T_DH	db 'dh',0
T_BH	db 'bh',0

T_WordRegs	dd T_AX,T_CX,T_DX,T_BX,T_SP,T_BP,T_SI,T_DI
T_AX	db 'ax',0
T_CX	db 'cx',0
T_DX	db 'dx',0
T_BX	db 'bx',0
T_SP	db 'sp',0
T_BP	db 'bp',0
T_SI	db 'si',0
T_DI	db 'di',0

T_DWordRegs	dd T_EAX,T_ECX,T_EDX,T_EBX,T_ESP,T_EBP,T_ESI,T_EDI
T_EAX	db 'eax',0
T_ECX	db 'ecx',0
T_EDX	db 'edx',0
T_EBX	db 'ebx',0
T_ESP	db 'esp',0
T_EBP	db 'ebp',0
T_ESI	db 'esi',0
T_EDI	db 'edi',0

T_CDWordRegs	dd T_CR0,T_INV,T_CR2,T_CR3,T_INV,T_INV,T_INV,T_INV
T_CR0	db 'cr0',0
T_CR2	db 'cr2',0
T_CR3	db 'cr3',0

T_DDWordRegs	dd T_DR0,T_DR1,T_DR2,T_DR3,T_INV,T_INV,T_DR6,T_DR7
T_DR0	db 'dr0',0
T_DR1	db 'dr1',0
T_DR2	db 'dr2',0
T_DR3	db 'dr3',0
T_DR6	db 'dr6',0
T_DR7	db 'dr7',0

T_TDWordRegs	dd T_INV,T_INV,T_INV,T_TR3,T_TR4,T_TR5,T_TR6,T_TR7
T_TR3	db 'tr3',0
T_TR4	db 'tr4',0
T_TR5	db 'tr5',0
T_TR6	db 'tr6',0
T_TR7	db 'tr7',0

T_SegRegs	dd T_ES,T_CS,T_SS,T_DS,T_FS,T_GS,T_INV,T_INV,T_INV
T_ES	db 'es',0
T_CS	db 'cs',0
T_SS	db 'ss',0
T_DS	db 'ds',0
T_FS	db 'fs',0
T_GS	db 'gs',0

T_RMS	dd T_RM0,T_RM1,T_RM2,T_RM3,T_RM4,T_RM5,T_RM6,T_RM7
T_RM0	db '[bx+si',0
T_RM1	db '[bx+di',0
T_RM2	DB '[bp+si',0
T_RM3	DB '[bp+di',0
T_RM4	DB '[si',0
T_RM5	DB '[di',0
T_RM6	DB '[bp',0
T_RM7	DB '[bx',0

T_CXAX	db 'cx,ax',0
T_DXAX	DB 'dx,ax',0
T_BXAX	db 'bx,ax',0
T_SPAX	DB 'sp,ax',0
T_BPAX	db 'bp,ax',0
T_SIAX	DB 'si,ax',0
T_DIAX	DB 'di,ax',0

T_ECXEAX	db 'ecx,eax',0
T_EDXEAX	DB 'edx,eax',0
T_EBXEAX	db 'ebx,eax',0
T_ESPEAX	DB 'esp,eax',0
T_EBPEAX	db 'ebp,eax',0
T_ESIEAX	DB 'esi,eax',0
T_EDIEAX	DB 'edi,eax',0

T_0	db '0',0
T_1	db '1',0
T_2	db '2',0
T_3	db '3',0
T_4	db '4',0
T_5	db '5',0
T_6	db '6',0
T_7	db '7',0

ReturnCode	dw 0

PointType	dd 0
SegOver_T	dd 0
AnotherOpCode	db 0
AnotherTable	db 0
Use32Bit	db 0
In32Bit	db 0

DisasTempSel	dw ?

RealFS	dw ?

dTargetEIP	dd ?
dTargetCS	dw ?
dTargetEIP2	dd ?
dTargetCS2	dw ?

eaMode	db 0
eaOffset	dd 0
eaScale	db 0
eaSeg	db 0
eaIndex	db 0
eaReg	db 0

ea2DebugRegs	dd DebugEAX,DebugECX,DebugEDX,DebugEBX,DebugESP,DebugEBP,DebugESI,DebugEDI
ea2sDebugRegs	dd DebugES,DebugCS,DebugSS,DebugDS,DebugFS,DebugGS

BadAddrText	db 'Invalid address.',0

_D_Destination	dd ?,?
_D_Source	dd ?

FWait_Waiting	db 0

HexTable	db '0123456789ABCDEF'


