;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Generate a CWD.ERR file for current program.
;
ResSize	equ	4096		;Size of resource block.
ResHead	equ	16		;Size of main header.
ResNum	equ	((((4096-ResHead)/5) shr 2) shl 2)	;Number of entries.
ResCount	equ	ResNum-4		;count for free entry checking.

Res_NULL	equ	0	;MUST be zero.
Res_PSP	equ	1	;single entry.
Res_SEL	equ	2	;single entry.
Res_MEM	equ	3	;3 entries.
Res_LOCK	equ	4	;2 entries.
Res_DOSMEM	equ	5	;?
Res_CALLBACK	equ	6	;3 entries.
Res_NOTHING	equ	254
Res_CHAIN	equ	255	;single entry.

mcbChunkLast	equ	0	;pointer to previous chunk in chain.
mcbChunkNext	equ	4	;pointer to next chunk in chain.
mcbBiggest	equ	8	;biggest free block in this chunk.
mcbChunkSize	equ	10	;size of this chunk.
mcbChunkLen	equ	12	;length of chunk control entry.

mcbID	equ	0	;ID for corruption checking.
mcbLast	equ	1	;previous MCB pointer status.
mcbLastSize	equ	2	;pointer to previous MCB.
mcbFreeUsed	equ	4	;free or used status.
mcbNext	equ	5	;next MCB pointer status.
mcbNextSize	equ	6	;pointer to next MCB.
mcbLen	equ	8	;length of an MCB entry.


GenerateCWD_ERR proc near
	mov	d[ErrHandle],0
	mov	TotalLinearMem,0
	mov	TotalLinearMem+4,0
	mov	TotalLinearMem+8,0
	mov	TotalSelectors,0
	;
	;Process segment registers to produce application relative base
	;addresses.
	;
	mov	ax,DebugCS
	mov	edi,offset DebugCSApp
	call	DebugSegmentDPMI
	mov	ax,DebugDS
	mov	edi,offset DebugDSApp
	call	DebugSegmentDPMI
	mov	ax,DebugES
	mov	edi,offset DebugESApp
	call	DebugSegmentDPMI
	mov	ax,DebugFS
	mov	edi,offset DebugFSApp
	call	DebugSegmentDPMI
	mov	ax,DebugGS
	mov	edi,offset DebugGSApp
	call	DebugSegmentDPMI
	mov	ax,DebugSS
	mov	edi,offset DebugSSApp
	call	DebugSegmentDPMI
	;
	mov	edx,offset eDebugName
	xor	cx,cx
	mov	ax,3c00h
	int	21h
	jc	@@NoFile
	mov	d[ERRHandle],eax
	;
@@NoFile:	;Display debug info.
	;
	mov	esi,offset Copyright+1
	xor	ecx,ecx
@@LookCEnd:	cmp	b[esi],0
	jz	@@AtCEnd
	inc	ecx
	inc	esi
	jmp	@@LookCEnd
@@AtCEnd:	mov	edx,offset Copyright+1
	mov	ebx,d[ERRHandle]
	mov	ah,40h
	int	21h
	;
	mov	ax,DebugTR
	mov	ecx,4
	mov	edi,offset eDebugTRt
	call	bin2hex_err
	mov	eax,DebugCR0
	mov	ecx,8
	mov	edi,offset eDebugCR0t
	call	bin2hex_err
	mov	eax,DebugCR2
	mov	ecx,8
	mov	edi,offset eDebugCR2t
	call	bin2hex_err
	mov	eax,DebugCR3
	mov	ecx,8
	mov	edi,offset eDebugCR3t
	call	bin2hex_err
	;
	mov	eax,DebugEAX
	mov	ecx,8
	mov	edi,offset eDebugEAXt
	call	bin2hex_err
	mov	eax,DebugEBX
	mov	ecx,8
	mov	edi,offset eDebugEBXt
	call	bin2hex_err
	mov	eax,DebugECX
	mov	ecx,8
	mov	edi,offset eDebugECXt
	call	bin2hex_err
	mov	eax,DebugEDX
	mov	ecx,8
	mov	edi,offset eDebugEDXt
	call	bin2hex_err
	mov	eax,DebugESI
	mov	ecx,8
	mov	edi,offset eDebugESIt
	call	bin2hex_err
	mov	eax,DebugEDI
	mov	ecx,8
	mov	edi,offset eDebugEDIt
	call	bin2hex_err
	mov	eax,DebugEBP
	mov	ecx,8
	mov	edi,offset eDebugEBPt
	call	bin2hex_err
	mov	eax,DebugESP
	mov	ecx,8
	mov	edi,offset eDebugESPt
	call	bin2hex_err
	mov	eax,DebugEIP
	mov	ecx,8
	mov	edi,offset eDebugEIPt
	call	bin2hex_err
	mov	eax,DebugEFL
	mov	ecx,8
	mov	edi,offset eDebugEFLt
	call	bin2hex_err
	;
	mov	ax,DebugCS
	mov	ecx,4
	mov	edi,offset eDebugCSt
	call	bin2hex_err
	mov	eax,DebugCSApp
	mov	ecx,8
	mov	edi,offset eDebugCSAppt
	cmp	eax,-1
	jnz	@@0
	mov	d[edi],'xxxx'
	mov	d[edi+4],'xxxx'
	add	edi,8
	jmp	@@1
@@0:	call	bin2hex_err
@@1:	mov	ax,DebugDS
	mov	ecx,4
	mov	edi,offset eDebugDSt
	call	bin2hex_err
	mov	eax,DebugDSApp
	mov	ecx,8
	mov	edi,offset eDebugDSAppt
	cmp	eax,-1
	jnz	@@2
	mov	d[edi],'xxxx'
	mov	d[edi+4],'xxxx'
	add	edi,8
	jmp	@@3
@@2:	call	bin2hex_err
@@3:	mov	ax,DebugES
	mov	ecx,4
	mov	edi,offset eDebugESt
	call	bin2hex_err
	mov	eax,DebugESApp
	mov	ecx,8
	mov	edi,offset eDebugESAppt
	cmp	eax,-1
	jnz	@@4
	mov	d[edi],'xxxx'
	mov	d[edi+4],'xxxx'
	add	edi,8
	jmp	@@5
@@4:	call	bin2hex_err
@@5:	mov	ax,DebugFS
	mov	ecx,4
	mov	edi,offset eDebugFSt
	call	bin2hex_err
	mov	eax,DebugFSApp
	mov	ecx,8
	mov	edi,offset eDebugFSAppt
	cmp	eax,-1
	jnz	@@6
	mov	d[edi],'xxxx'
	mov	d[edi+4],'xxxx'
	add	edi,8
	jmp	@@7
@@6:	call	bin2hex_err
@@7:	mov	ax,DebugGS
	mov	ecx,4
	mov	edi,offset eDebugGSt
	call	bin2hex_err
	mov	eax,DebugGSApp
	mov	ecx,8
	mov	edi,offset eDebugGSAppt
	cmp	eax,-1
	jnz	@@10
	mov	d[edi],'xxxx'
	mov	d[edi+4],'xxxx'
	add	edi,8
	jmp	@@11
@@10:	call	bin2hex_err
@@11:	mov	ax,DebugSS
	mov	ecx,4
	mov	edi,offset eDebugSSt
	call	bin2hex_err
	mov	eax,DebugSSApp
	mov	ecx,8
	mov	edi,offset eDebugSSAppt
	cmp	eax,-1
	jnz	@@12
	mov	d[edi],'xxxx'
	mov	d[edi+4],'xxxx'
	add	edi,8
	jmp	@@13
@@12:	call	bin2hex_err
@@13:	;
	mov	edi,offset DebugSysFlags
	movzx	eax,w[SystemFlags]
	mov	ecx,8
	call	bin2hex_err
	;
	mov	edx,offset eDebugRegs
	mov	ecx,offset eDebugHeaderEnd-(eDebugRegs+1)
	mov	ebx,d[ERRHandle]
	mov	ah,40h
	int	21h

CSEIPDEBUGDUMPCOUNT	EQU	64	; should be multiple of 16, <=256
SSESPDEBUGDUMPCOUNT	EQU	64	; should be multiple of 16, <=256
SSEBPDEBUGDUMPCOUNT	EQU	64	; should be multiple of 16, <=256

	push	fs

	;
	;Do a CS:EIP dump.
	;
	mov	ecx,CSEIPDEBUGDUMPCOUNT
	mov	fs,DebugCS
	mov	esi,DebugEIP
	test	SystemFlags,1
	jz	@@cseip32
	movzx	esi,si

@@cseip32:
	xor	eax,eax
	mov	ax,fs
	lsl	eax,eax
	cmp	eax,-1
	jz	@@flat0
	inc	eax

@@flat0:
	mov	ebx,esi
	add	ebx,ecx
	cmp	ebx,eax
	jc	@@cseip2
	sub	ebx,eax
	sub	ecx,ebx
	jz	@@cseip3
	js	@@cseip3

@@cseip2:
	;
	mov	edi,offset eDebugHeader
	mov	edx,OFFSET CSEIPDebugText
	call	DebugTextCopy
	xor	edx,edx
@@cseip0:
	push	ecx
	mov	ecx,2
	mov	al,fs:[esi]
	call	bin2hex_err
	pop	ecx
	mov	b[edi]," "
	inc	edi
	inc	esi
	inc	edx
	and	dl,0fh
	jnz	@@cseip1
	mov	WORD PTR ds:[edi],0a0dh
	add	edi,2

	mov	eax,20202020h
	mov	DWORD PTR ds:[edi],eax
	mov	DWORD PTR ds:[edi+4],eax
	add	edi,8
	mov	BYTE PTR ds:[edi],al
	inc	edi
	;
@@cseip1:
	dec	ecx
	jnz	@@cseip0
	mov	DWORD PTR ds:[edi],0a0d0a0dh
	add	edi,4
	mov	b[edi],"$"
	;
	mov	edx,offset eDebugHeader
	sub	edi,edx
	mov	ecx,edi
	mov	ah,40h
	mov	ebx,d[ErrHandle]
	int	21h
	;

@@cseip3:

	;
	;Do a SS:ESP dump.
	;
	mov	ecx,SSESPDEBUGDUMPCOUNT
	mov	fs,DebugSS
	mov	esi,DebugESP
	test	SystemFlags,1
	jz	@@ssesp32
	movzx	esi,si

@@ssesp32:
	xor	eax,eax
	mov	ax,fs
	lsl	eax,eax
	cmp	eax,-1
	jz	@@flat1
	inc	eax

@@flat1:
	mov	ebx,esi
	add	ebx,ecx
	cmp	ebx,eax
	jc	@@ssesp2
	sub	ebx,eax
	sub	ecx,ebx
	jz	@@ssesp3
	js	@@ssesp3

@@ssesp2:
	;
	mov	edi,offset eDebugHeader
	mov	edx,OFFSET SSESPDebugText
	call	DebugTextCopy
	xor	edx,edx

@@ssesp0:
	push	ecx
	mov	ecx,2
	mov	al,fs:[esi]
	call	bin2hex_err
	pop	ecx
	mov	b[edi]," "
	inc	edi
	inc	esi
	inc	edx
	test	dl,0fh
	jnz	@@ssesp1
	mov	WORD PTR ds:[edi],0a0dh
	add	edi,2

	mov	eax,20202020h
	mov	DWORD PTR ds:[edi],eax
	mov	DWORD PTR ds:[edi+4],eax
	add	edi,8
	mov	BYTE PTR ds:[edi],al
	inc	edi
	;
@@ssesp1:
	dec	ecx
	jnz	@@ssesp0

	mov	DWORD PTR ds:[edi],0a0d0a0dh
	add	edi,4
	mov	b[edi],"$"
	;
	mov	edx,offset eDebugHeader
	sub	edi,edx
	mov	ecx,edi
	mov	ah,40h
	mov	ebx,d[ErrHandle]
	int	21h

@@ssesp3:
	;
	;Do a SS:EBP dump.
	;
; do a pre-EBP dump for stack frames
	mov	ecx,SSEBPDEBUGDUMPCOUNT
	mov	fs,DebugSS
	mov	esi,DebugEBP
	test	esi,esi		; see if any previous bytes
	je	ebpdump			; no, nothing to display
	cmp	esi,ecx			; see if stack frame as large as display byte dump
	jae	med2			; yes
	mov	ecx,esi

med2:
	sub	esi,ecx

	test	SystemFlags,1
	jz	medssebp32
	movzx	esi,si

medssebp32:
	xor	eax,eax
	mov	ax,fs
	lsl	eax,eax
	cmp	eax,-1
	jz	medflat2
	inc	eax

medflat2:
	mov	ebx,esi
	add	ebx,ecx
	cmp	ebx,eax
	jc	medssebp2
	sub	ebx,eax
	sub	ecx,ebx
	jz	ebpdump
	js	ebpdump

medssebp2:
	;
	xor	edx,edx

	mov	edi,offset eDebugHeader
	mov	eax,20202020h
	mov	DWORD PTR ds:[edi],eax
	mov	DWORD PTR ds:[edi+4],eax
	add	edi,8
	mov	BYTE PTR ds:[edi],al
	inc	edi

	push	ecx
	and	ecx,0fh
	je	med3
	mov	dl,16			; high bytes known zero
	sub	dl,cl			; save hex bytes left to display on row
	mov	cl,dl

medloop:
	mov	ds:[edi],ax		; 3-byte pads until at proper display position
	mov	ds:[edi+2],al
	add	edi,3
	dec	ecx
	jne	medloop

med3:
	pop	ecx

medssebp0:
	push	ecx
	mov	al,fs:[esi]
	mov	ecx,2
	call	bin2hex_err
	pop	ecx
	
	mov	b[edi]," "
	inc	edi
	inc	esi
	inc	edx
	test	dl,0fh
	jnz	medssebp1

	mov	WORD PTR ds:[edi],0a0dh
	add	edi,2
	cmp	ecx,1
	je	medssebp1
	mov	eax,20202020h
	mov	DWORD PTR ds:[edi],eax
	mov	DWORD PTR ds:[edi+4],eax
	add	edi,8
	mov	BYTE PTR ds:[edi],al
	inc	edi

	;
medssebp1:
	dec	ecx
	jne	medssebp0

;	mov	WORD PTR ds:[edi],0a0dh
;	add	edi,2
;	mov	b[edi],"$"
	;
	mov	edx,offset eDebugHeader
	sub	edi,edx
	mov	ecx,edi
	mov	ah,40h
	mov	ebx,d[ErrHandle]
	int	21h
	;

ebpdump:
	mov	ecx,SSEBPDEBUGDUMPCOUNT
	mov	fs,DebugSS
	mov	esi,DebugEBP
	test	SystemFlags,1
	jz	@@ssebp32
	movzx	esi,si

@@ssebp32:
	xor	eax,eax
	mov	ax,fs
	lsl	eax,eax
	cmp	eax,-1
	jz	@@flat2
	inc	eax

@@flat2:
	mov	ebx,esi
	add	ebx,ecx
	cmp	ebx,eax
	jc	@@ssebp2
	sub	ebx,eax
	sub	ecx,ebx
	jz	@@ssebp3
	js	@@ssebp3

@@ssebp2:
	;
	mov	edi,offset eDebugHeader
	mov	edx,OFFSET SSEBPDebugText
	call	DebugTextCopy
	xor	edx,edx

@@ssebp0:
	push	ecx
	mov	ecx,2
	mov	al,fs:[esi]
	call	bin2hex_err
	pop	ecx
	mov	b[edi]," "
	inc	edi
	inc	esi
	inc	edx
	test	dl,0fh
	jnz	@@ssebp1
	mov	WORD PTR ds:[edi],0a0dh
	add	edi,2

	mov	eax,20202020h
	mov	DWORD PTR ds:[edi],eax
	mov	DWORD PTR ds:[edi+4],eax
	add	edi,8
	mov	BYTE PTR ds:[edi],al
	inc	edi
	;
@@ssebp1:
	dec	ecx
	jnz	@@ssebp0
	mov	DWORD PTR ds:[edi],0a0d0a0dh
	add	edi,4
	mov	b[edi],"$"
	;
	mov	edx,offset eDebugHeader
	sub	edi,edx
	mov	ecx,edi
	mov	ah,40h
	mov	ebx,d[ErrHandle]
	int	21h
	;

@@ssebp3:
	pop	fs


	;
	;Do resource output stuff.
	;
	push	fs
	mov	fs,DebugPSP
	push	es
	pushad
	cmp	fs:d[EPSP_Resource],0
	jz	@@r8
	mov	edx,offset ResHeader
	mov	ah,40h
	mov	ecx,ResHeaderLen
	mov	ebx,d[ERRHandle]
	int	21h
	mov	edx,offset SelHeader
	mov	ah,40h
	mov	ecx,SelHeaderLen
	mov	ebx,d[ERRHandle]
	int	21h
	mov	es,RealSegment
	mov	esi,fs:d[EPSP_Resource]
@@LookLoop:	push	esi
	mov	ebp,ResNum		;get number of entries.
	mov	edi,esi
	add	esi,ResHead+ResNum
	add	edi,ResHead
@@r0:	cmp	es:b[edi],Res_SEL
	jnz	@@r1
	mov	ax,fs:w[EPSP_DPMIMem]
	cmp	es:w[esi],ax
	jz	@@r1
	;
@@SEL:	pushad
	mov	edi,offset eDebugHeader
	mov	eax,es:[esi]
	mov	ecx,4
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	ebx,es:[esi]
	sys	GetSelDet32
	push	ecx
	mov	eax,edx
	mov	ecx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	pop	eax
	mov	ecx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	eax,es:[esi]
	lar	eax,eax
	test	eax,0000100000000000b
	mov	ebx,'EDOC'
	jnz	@@dc0
	mov	ebx,'ATAD'
@@dc0:	mov	[edi],ebx
	add	edi,4
	mov	b[edi],' '
	inc	edi
	mov	eax,es:[esi]
	lar	eax,eax
	mov	bx,"61"
	test	eax,00400000h
	jz	@@Use32It
	mov	bx,"23"
@@Use32It:	mov	[edi],bx
	add	edi,2
	mov	b[edi],' '
	inc	edi
	inc	TotalSelectors
	;
	;See if there is a memory block that matches this selector.
	;
	mov	ebx,es:[esi]
	sys	GetSelDet32
	mov	ebx,edx
	pushad
	mov	esi,fs:d[EPSP_Resource]
@@s2:	push	esi
	mov	ebp,ResNum		;get number of entries.
	mov	edi,esi
	add	esi,ResHead+ResNum
	add	edi,ResHead
@@s0:	cmp	es:b[edi],Res_MEM	;Anything here?
	jnz	@@s1
	mov	eax,fs:d[EPSP_INTMem]
	cmp	es:[esi],eax
	jz	@@s1
	cmp	fs:w[EPSP_DPMIMem],0
	jz	@@nodpmi
	pushad
	mov	bx,fs:w[EPSP_DPMIMem]
	sys	GetSelDet32
	cmp	es:[esi],edx
	popad
	jz	@@s1
@@nodpmi:	;
	cmp	ebx,es:[esi]		;Fits this block?
	jnz	@@s1
	pop	esi
	clc
	jmp	@@s3
	;
@@s1:	add	esi,4
	inc	edi
	dec	ebp
	jnz	@@s0
	pop	esi
	mov	esi,es:[esi+8]	;link to next list.
	or	esi,esi
	jnz	@@s2
	;
	;Have a look in the MCB list.
	;
	cmp	fs:d[EPSP_mcbHead],0
	jz	@@nomcbsel
	mov	esi,fs:d[EPSP_mcbHead]
@@mcbs0:	mov	edi,esi
	add	esi,mcbChunkLen
@@mcbs1:	mov	eax,esi
	add	eax,mcbLen
	cmp	ebx,eax
	jnz	@@mcbs2
	clc
	jmp	@@s3
@@mcbs2:	cmp	es:b[esi+mcbNext],"M"
	jz	@@mcbs3
	mov	esi,es:[edi+mcbChunkNext]
	or	esi,esi
	jz	@@nomcbsel
	jmp	@@mcbs0
@@mcbs3:	movzx	eax,es:w[esi+mcbNextSize]
	add	eax,mcbLen
	add	esi,eax
	jmp	@@mcbs1
@@nomcbsel:	stc
@@s3:	popad
	jc	@@r4
	;
	mov	b[edi],'Y'
	jmp	@@r5
	;
@@r4:	mov	d[edi],'N'
	;
@@r5:	inc	edi
	mov	b[edi],' '
	inc	edi
	;
	mov	eax,es:[esi]
	mov	bx,fs:w[EPSP_SegBase]
	cmp	ax,bx
	jc	@@r2
	mov	bx,fs:w[EPSP_SegSize]
	add	bx,fs:w[EPSP_SegBase]
	cmp	ax,bx
	jnc	@@r2
	mov	bx,fs:w[EPSP_SegBase]
	shr	bx,3
	shr	ax,3
	sub	ax,bx
	inc	ax
	mov	ecx,4
	call	bin2hex_err
	jmp	@@r20
	;
@@r2:	mov	d[edi],'xxxx'
	add	edi,4
	;
@@r20:	mov	b[edi],13
	mov	b[edi+1],10
	mov	b[edi+2],'$'
	add	edi,2
	push	edi
	pop	ecx
	sub	ecx,offset eDebugHeader
	mov	edx,offset eDebugHeader
	mov	ah,40h
	mov	ebx,d[ERRHandle]
	int	21h
	popad
	;
@@r1:	add	esi,4
	inc	edi
	dec	ebp
	jnz	@@r0
	pop	edi
	mov	esi,es:[edi+8]	;link to next list.
	or	esi,esi
	jnz	@@LookLoop
	;


	mov	edi,offset TotalSelsNum
	mov	eax,TotalSelectors
	mov	ecx,4
	call	bin2hex_err
	mov	ecx,TotalSelsTLen
	mov	edx,offset TotalSelsText
	mov	ah,40h
	mov	ebx,d[ERRHandle]
	int	21h
	;
	;Now do memory blocks.
	;
	mov	edx,offset MemHeader
	mov	ah,40h
	mov	ecx,MemHeaderLen
	mov	ebx,d[ERRHandle]
	int	21h
	;
	mov	esi,fs:d[EPSP_Resource]
	mov	es,RealSegment
@@mLookLoop:	push	esi
	mov	ebp,ResNum		;get number of entries.
	mov	edi,esi
	add	esi,ResHead+ResNum
	add	edi,ResHead
@@m0:	cmp	es:b[edi],Res_MEM
	jnz	@@m1
	;
	mov	eax,fs:d[EPSP_INTMem]
	cmp	es:[esi],eax
	jz	@@m1
	cmp	fs:w[EPSP_DPMIMem],0
	jz	@@nodpmimem
	pushad
	mov	bx,fs:w[EPSP_DPMIMem]
	sys	GetSelDet32
	cmp	es:[esi],edx
	popad
	jz	@@m1
	;
@@nodpmimem:	;Check if this is an MCB block.
	;
	push	edi
	cmp	fs:d[EPSP_mcbHead],0
	jz	@@MEM
	mov	ebx,es:[esi]
	mov	edi,fs:d[EPSP_mcbHead]
@@mcbmc0:	cmp	ebx,edi
	jz	@@mcbmc1
	mov	edi,es:[edi+mcbChunkNext]
	or	edi,edi
	jz	@@MEM
	jmp	@@mcbmc0
	;
@@mcbmc1:	;Update Total memory value and skip this block.
	;
	mov	eax,es:[esi+8]
	add	eax,4095
	and	eax,0fffff000h
	add	TotalLinearMem+4,eax
	pop	edi
	jmp	@@m1
@@MEM:	pop	edi
	pushad
	mov	edi,offset eDebugHeader
	mov	eax,es:[esi+4]
	mov	ecx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	eax,es:[esi]
	mov	ecx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	eax,es:[esi+8]
	add	TotalLinearMem,eax
	push	eax
	add	eax,4095
	and	eax,0fffff000h
	add	TotalLinearMem+4,eax
	pop	eax
	inc	TotalLinearMem+8
	mov	ecx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	;
	mov	b[edi],13
	mov	b[edi+1],10
	mov	b[edi+2],'$'
	add	edi,2
	push	edi
	pop	ecx
	push	es
	sub	ecx,offset eDebugHeader
	mov	edx,offset eDebugHeader
	mov	ah,40h
	mov	ebx,d[ERRHandle]
	int	21h
	pop	es
	popad
	;
@@m1:	add	esi,4
	inc	edi
	dec	ebp
	jnz	@@m0
	pop	edi
	mov	esi,es:[edi+8]	;link to next list.
	or	esi,esi
	jnz	@@mLookLoop
	;
	;Now print MCB controlled blocks.
	;
	cmp	fs:d[EPSP_mcbHead],0
	jz	@@nomcbdis
	mov	esi,fs:d[EPSP_mcbHead]
@@mcb0:	mov	edi,esi
	add	esi,mcbChunkLen
@@mcb1:	cmp	es:b[esi+mcbFreeUsed],"J"	;Free block?
	jz	@@mcb2
	mov	eax,fs:d[EPSP_INTMem]
	sub	eax,mcbLen
	cmp	esi,eax
	jz	@@mcb2
	cmp	fs:w[EPSP_DPMIMem],0
	jz	@@nodpmimemhere
	pushad
	mov	bx,fs:w[EPSP_DPMIMem]
	sys	GetSelDet32
	sub	edx,mcbLen
	cmp	esi,edx
	popad
	jz	@@mcb2
@@nodpmimemhere:
	pushad
	mov	edi,offset eDebugHeader
	mov	eax,esi
	add	eax,mcbLen
	mov	cx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	eax,esi
	add	eax,mcbLen
	mov	cx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	movzx	eax,es:w[esi+mcbNextSize]
	inc	TotalLinearMem+8
	mov	cx,8
	push	eax
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	pop	eax
	add	TotalLinearMem,eax
	;
	mov	b[edi],13
	mov	b[edi+1],10
	mov	b[edi+2],'$'
	add	edi,2
	push	edi
	pop	ecx
	push	es
	sub	ecx,offset eDebugHeader
	mov	edx,offset eDebugHeader
	mov	ah,40h
	mov	ebx,d[ERRHandle]
	int	21h
	pop	es
	popad
@@mcb2:	cmp	es:b[esi+mcbNext],"M"	;end of the chain?
	jz	@@mcb3
	mov	esi,es:[edi+mcbChunkNext]
	or	esi,esi
	jnz	@@mcb0
	jmp	@@nomcbdis
	;
@@mcb3:	movzx	eax,es:w[esi+mcbNextSize]
	add	eax,mcbLen
	add	esi,eax
	jmp	@@mcb1
	;
@@nomcbdis:	;Display totals.
	;
	mov	edi,offset TotalMemNum1
	mov	eax,TotalLinearMem
	mov	ecx,8
	call	bin2hex_err
	mov	edi,offset TotalMemNum2
	mov	eax,TotalLinearMem+4
	mov	ecx,8
	call	bin2hex_err
	mov	edi,offset TotalMemNum3
	mov	eax,TotalLinearMem+8
	mov	ecx,8
	call	bin2hex_err
	mov	ecx,TotalMemTLen
	mov	edx,offset TotalMemText
	mov	ah,40h
	mov	ebx,d[ERRHandle]
	int	21h
	;
	;Now do lock details.
	;
	mov	edx,offset LockHeader
	mov	ah,40h
	mov	ecx,LockHeaderLen
	mov	ebx,d[ERRHandle]
	int	21h
	;
	mov	esi,fs:d[EPSP_Resource]
@@lLookLoop:	push	esi
	mov	ebp,ResNum		;get number of entries.
	mov	edi,esi
	add	esi,ResHead+ResNum
	add	edi,ResHead
@@l0:	cmp	es:b[edi],Res_LOCK
	jnz	@@l1
	;
@@LOCK:	pushad
	mov	edi,offset eDebugHeader
	mov	eax,es:[esi]
	mov	cx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	eax,es:[esi+4]
	mov	cx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	;
	mov	b[edi],13
	mov	b[edi+1],10
	mov	b[edi+2],'$'
	add	edi,2
	push	edi
	pop	ecx
	sub	ecx,offset eDebugHeader
	mov	edx,offset eDebugHeader
	mov	ah,40h
	mov	ebx,d[ERRHandle]
	int	21h
	popad
	;
@@l1:	add	esi,4
	inc	edi
	dec	ebp
	jnz	@@l0
	pop	edi
	mov	esi,es:[edi+8]	;link to next list.
	or	esi,esi
	jnz	@@lLookLoop
	;
	;Now do DOS memory details.
	;
	mov	edx,offset DosMemHeader
	mov	ah,40h
	mov	ecx,DosMemHeaderLen
	mov	ebx,d[ERRHandle]
	int	21h
	;
	mov	esi,fs:d[EPSP_Resource]
@@dmLookLoop:	push	esi
	mov	ebp,ResNum		;get number of entries.
	mov	edi,esi
	add	esi,ResHead+ResNum
	add	edi,ResHead
@@dm0:	cmp	es:b[edi],Res_DOSMEM
	jnz	@@dm1
	;
	pushad
	mov	edi,offset eDebugHeader
	mov	eax,es:[esi]
	mov	cx,4
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	pushm	ebx,ecx,edx
	mov	bx,es:[esi]
	sys	GetSelDet32
	mov	eax,edx
	popm	ebx,ecx,edx
	mov	cx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	eax,es:[esi+4]
	mov	cx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	;
	mov	b[edi],13
	mov	b[edi+1],10
	mov	b[edi+2],'$'
	add	edi,2
	push	edi
	pop	ecx
	sub	ecx,offset eDebugHeader
	mov	edx,offset eDebugHeader
	mov	ah,40h
	mov	ebx,d[ERRHandle]
	int	21h
	popad
	;
@@dm1:	add	esi,4
	inc	edi
	dec	ebp
	jnz	@@dm0
	pop	edi
	mov	esi,es:[edi+8]	;link to next list.
	or	esi,esi
	jnz	@@dmLookLoop
	cmp	fs:d[EPSP_INTMem],0
	jz	@@r8
	;
	;Now do protected mode int details.
	;
	mov	edx,offset PIntHeader
	mov	ah,40h
	mov	ecx,PIntHeaderLen
	mov	ebx,d[ERRHandle]
	int	21h
	;
	mov	ecx,256
	mov	esi,fs:d[EPSP_INTMem]
	mov	ebx,0
@@pv0:	pushm	ebx,ecx,esi
	sys	GetVect
	cmp	cx,es:w[esi+4]
	jnz	@@pv10
	cmp	edx,es:d[esi]
	jz	@@pv1
@@pv10:	;
	mov	edi,offset eDebugHeader
	mov	eax,ebx
	mov	cx,2
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	ax,es:w[esi+4]
	mov	cx,4
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	eax,es:d[esi]
	mov	cx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	;
	mov	b[edi],13
	mov	b[edi+1],10
	mov	b[edi+2],'$'
	add	edi,2
	push	edi
	pop	ecx
	sub	ecx,offset eDebugHeader
	mov	edx,offset eDebugHeader
	mov	ah,40h
	mov	ebx,d[ERRHandle]
	int	21h
	;
@@pv1:	popm	ebx,ecx,esi
	add	esi,6
	inc	ebx
	loop	@@pv0
	;
	;Now do protected mode exception details.
	;
	mov	edx,offset EIntHeader
	mov	ah,40h
	mov	ecx,EIntHeaderLen
	mov	ebx,d[ERRHandle]
	int	21h
	;
	mov	ecx,32
	mov	esi,fs:d[EPSP_INTMem]
	add	esi,256*6
	mov	ebx,0
@@pe0:	pushm	ebx,ecx,esi
	sys	GetEVect
	cmp	cx,es:w[esi+4]
	jnz	@@pv100
	cmp	edx,es:d[esi]
	jz	@@pe1
@@pv100:	;
	mov	edi,offset eDebugHeader
	mov	eax,ebx
	mov	cx,2
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	ax,es:w[esi+4]
	mov	cx,4
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	eax,es:d[esi]
	mov	cx,8
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	;
	mov	b[edi],13
	mov	b[edi+1],10
	mov	b[edi+2],'$'
	add	edi,2
	push	edi
	pop	ecx
	sub	ecx,offset eDebugHeader
	mov	edx,offset eDebugHeader
	mov	ah,40h
	mov	ebx,d[ERRHandle]
	int	21h
	;
@@pe1:	popm	ebx,ecx,esi
	add	esi,6
	inc	ebx
	loop	@@pe0
	;
	;Now do real mode int details.
	;
	mov	edx,offset RIntHeader
	mov	ah,40h
	mov	ecx,RIntHeaderLen
	mov	ebx,d[ERRHandle]
	int	21h
	;
	mov	ecx,256
	mov	esi,fs:d[EPSP_INTMem]
	add	esi,(256*6)+(32*6)
	mov	ebx,0
@@ri0:	pushm	ebx,ecx,esi
	sys	GetRVect
	cmp	cx,es:w[esi+2]
	jnz	@@pv1000
	cmp	dx,es:w[esi]
	jz	@@ri1
@@pv1000:	;
	mov	edi,offset eDebugHeader
	mov	eax,ebx
	mov	cx,2
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	ax,es:w[esi+2]
	mov	cx,4
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	ax,es:w[esi]
	mov	cx,4
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	;
	mov	b[edi],13
	mov	b[edi+1],10
	mov	b[edi+2],'$'
	add	edi,2
	push	edi
	pop	ecx
	sub	ecx,offset eDebugHeader
	mov	edx,offset eDebugHeader
	mov	ah,40h
	mov	ebx,d[ERRHandle]
	int	21h
	;
@@ri1:	popm	ebx,ecx,esi
	add	esi,4
	inc	ebx
	loop	@@ri0
	;
	;Now do call-back details.
	;
	mov	edx,offset CallBackHeader
	mov	ah,40h
	mov	ecx,CallBackHeaderLen
	mov	ebx,d[ERRHandle]
	int	21h
	;
	mov	esi,fs:d[EPSP_Resource]
@@cbLookLoop:	push	esi
	mov	ebp,ResNum		;get number of entries.
	mov	edi,esi
	add	esi,ResHead+ResNum
	add	edi,ResHead
@@cb0:	cmp	es:b[edi],Res_CALLBACK
	jnz	@@cb1
	;
	pushad
	mov	edi,offset eDebugHeader
	mov	eax,es:[esi]
	shr	eax,16
	mov	cx,4
	call	bin2hex_err
	mov	b[edi],':'
	inc	edi
	mov	eax,es:[esi]
	mov	cx,4
	call	bin2hex_err
	mov	b[edi],' '
	inc	edi
	mov	eax,es:[esi+8]
	mov	cx,4
	call	bin2hex_err
	mov	b[edi],':'
	inc	edi
	mov	eax,es:[esi+4]
	mov	cx,8
	call	bin2hex_err
	;
	mov	b[edi],13
	mov	b[edi+1],10
	mov	b[edi+2],'$'
	add	edi,2
	push	edi
	pop	ecx
	sub	ecx,offset eDebugHeader
	mov	edx,offset eDebugHeader
	mov	ah,40h
	mov	ebx,d[ERRHandle]
	int	21h
	popad
	;
@@cb1:	add	esi,4
	inc	edi
	dec	ebp
	jnz	@@cb0
	pop	edi
	mov	esi,es:[edi+8]	;link to next list.
	or	esi,esi
	jnz	@@cbLookLoop
@@r8:	popad
	pop	es
	pop	fs
	;
	mov	ebx,d[ERRHandle]
	mov	ah,3eh
	int	21h
	;
@@9:	ret
GenerateCWD_ERR endp


;-------------------------------------------------------------------------
;
;Convert number into ASCII Hex version.
;
;On Entry:-
;
;EAX	- Number to convert.
;CX	- Digits to do.
;DS:EDI	- Buffer to put string in.
;
Bin2Hex_err	proc	near
	pushm	eax,ebx,ecx,edx,edi
	movzx	ecx,cx
	mov	ebx,offset HexTable
	add	edi,ecx
	dec	edi
	mov	edx,eax
@@0:	mov	al,dl
	shr	edx,4
	and	al,15
	xlat	[ebx]
	mov	[edi],al
	dec	edi
	dec	ecx
	jnz	@@0
	popm	eax,ebx,ecx,edx,edi
	push	ecx
	movzx	ecx,cx
	add	edi,ecx
	pop	ecx
	ret
Bin2Hex_err	endp


;-------------------------------------------------------------------------
;
;Convert segment value into real & application relative.
;
;On Entry:-
;
;AX	- Selector.
;DS:EDI	- Buffer address.
;
DebugSegmentDPMI proc near
	push	fs
	mov	fs,DebugPSP
	mov	ebx,-1
	;
	push	eax
	mov	ax,fs
	movzx	eax,ax
	lsl	eax,eax
	cmp	eax,(size PSP_Struc)+(size EPSP_Struc)
	pop	eax
	jc	@@9
	;
	mov	dx,ax
	cmp	ax,fs:w[EPSP_SegBase]	;inside application selector space?
	jc	@@9
	mov	cx,fs:w[EPSP_SegSize]
	add	cx,fs:w[EPSP_SegBase]	;add in base selector.
	cmp	ax,cx
	jnc	@@9		;outside application startup selectors.
	mov	bx,dx
	pushm	cx,dx
	mov	ax,0006h
	int	31h
	mov	ax,cx
	shl	eax,16
	mov	ax,dx
	popm	cx,dx
	sub	eax,fs:d[EPSP_MemBase]	;get offset within application.
	mov	ebx,eax
@@9:	pop	fs
	mov	[edi],ebx		;store generated value.
	ret
DebugSegmentDPMI endp

;-------------------------------------------------------------------------
; copy debug text into output buffer
; upon entry ds:edi -> out buffer, ds:edx -> in buffer
;
DebugTextCopy	PROC	NEAR

dbdloop:
	mov	al,ds:[edx]
	test	al,al
	je	dbdret		; null terminator flags end of text, don't transfer
	mov	ds:[edi],al
	inc	edx
	inc	edi
	jmp	dbdloop

dbdret:
	ret
DebugTextCopy	ENDP
