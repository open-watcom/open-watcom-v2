	.code


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Evaluate the expresion in the buffer and produce a 32 bit value.
;
;First all numbers/labels are converted into 32 bit numbers, operators are
;just stored.
;
EvaluateData	proc	near
	lea	esi,EvaluateBuffer	;source data.
	call	ExpandLine
	;
	lea	esi,EvaluateBuffer	;source data.
	lea	edi,EvaluateBuffer2	;destination.
	mov	b[edi],-1		;pre-terminate it.
@@0:	;
	;skip spaces/tabs and insert any operators.
	;
	lodsb
	or	al,al
	jz	@@3		;go strieght to the end.
	cmp	al,' '		;skip spaces.
	jz	@@0
	cmp	al,9		;skip tabs.
	jz	@@0
	cmp	al,'*'
	jz	@@0_1
	;
	cmp	al,'$'
	jnz	@@NotPC0
	;
	cmp	b[esi],'0'		;followed by valid hex digit?
	jc	@@CheckAlpha1
	cmp	b[esi],'9'+1
	jc	@@NotPC0
	;
@@CheckAlpha1:	cmp	b[esi],'A'
	jc	@@CheckAlpha2
	cmp	b[esi],'F'+1
	jc	@@NotPC0
	;
@@CheckAlpha2:	cmp	b[esi],'a'
	jc	@@IsPC
	cmp	b[esi],'f'+1
	jc	@@NotPC0
	;
@@IsPC:	mov	ecx,ProgramCounter	;Get program counter.
	mov	edx,ProgramCounter+4
	mov	b[edi],0
	mov	[edi+1],ecx
	mov	d[edi+5],edx
	mov	b[edi+9],-1		;preset terminator.
	add	edi,9		;move to next field.
	jmp	@@0
	;
@@NotPC0:	cmp	al,'%'
	jz	@@0_0
	cmp	al,'"'
	jz	@@0_0
	cmp	al,"'"
	jz	@@0_0
	cmp	al,'-'
	jz	@@0_1
	cmp	al,'+'
	jz	@@0_1
	cmp	al,'/'
	jz	@@0_1
	cmp	al,'('
	jz	@@0_1
	cmp	al,')'
	jz	@@0_1
	cmp	al,'&'		;AND
	jz	@@0_1
	cmp	al,'|'		;OR
	jz	@@0_1
	;
	cmp	al,'<'
	jnz	@@NotLeft0
	cmp	b[esi],'<'
	jnz	@@9	;NotLeft0
	inc	esi		;skip second shift operator.
	jmp	@@0_1
	;
@@NotLeft0:	cmp	al,'>'
	jnz	@@NotRight0
	cmp	b[esi],'>'
	jnz	@@9	;NotRight0
	inc	esi
	jmp	@@0_1
	;
@@NotRight0:	jmp	@@0_0
@@0_1:	mov	b[edi],al		;store this operator.
	mov	b[edi+1],-1		;make sure its terminated.
	inc	edi		;step over it.
	jmp	@@0		;keep looking.
@@0_0:	dec	esi		;move back to last data.
	;
	;first decide if this is decimal,hex,binary,label or a string.
	;
	xor	dl,dl		;reset number flag.
	xor	ah,ah		;default to decimal.
	push	esi
@@1:	lodsb
	call	UpperChar
	cmp	al,'0'		;check for digits.
	jc	@@1_0		;/
	cmp	al,'9'+1		;/
	jnc	@@1_0		;/
	mov	dl,1		;flag some numbers found.
	jmp	@@1		;keep looking.
@@1_0:	or	al,al		;end of string?
	jz	@@2		;/
	cmp	al,'%'
	jz	@@1_3		;set flag for binary.
	cmp	al,'$'
	jnz	@@1_6		;check set flag for hex?
	;
	mov	al,[esi]
	cmp	al,'0'		;is it 0-9?
	jc	@@1_7		;/
	cmp	al,'9'+1		;/
	jc	@@1_2		;go for hex.
	call	UpperChar
	cmp	al,'A'		;is it A-F?
	jc	@@1_7		;/
	cmp	al,'F'+1		;/
	jc	@@1_2		;go for hex.
	;
@@1_7:	mov	al,[esi-1]		;get '$' again.
@@1_6:	cmp	al,"'"
	jz	@@1_4		;set string.
	cmp	al,'"'
	jz	@@1_4		;set string.
	cmp	al,'-'		;/
	jz	@@2		;/
	cmp	al,'+'		;/
	jz	@@2		;/
	cmp	al,'*'		;/
	jz	@@2		;/
	cmp	al,'/'		;/
	jz	@@2		;/
	cmp	al,'('		;/
	jz	@@2		;/
	cmp	al,')'		;/
	jz	@@2		;/
	cmp	al,'&'
	jz	@@2
	cmp	al,'|'
	jz	@@2
	cmp	al,'<'
	jz	@@2
	cmp	al,'>'
	jz	@@2
	cmp	al,' '		;/
	jz	@@2		;/
	or	dl,dl		;did we find any numbers first?
	jz	@@1_1		;assume its a label.
	cmp	al,'H'
	jz	@@1_2		;assume its hex.
	;
	;look to see if this is the end of the string.
	;
	cmp	b[esi],0
	jz	@@1_5
	cmp	b[esi],' '
	jz	@@1_5
	cmp	b[esi],9
	jz	@@1_5
	cmp	b[esi],'-'
	jz	@@1_5
	cmp	b[esi],'+'
	jz	@@1_5
	cmp	b[esi],'*'
	jz	@@1_5
	cmp	b[esi],'/'
	jz	@@1_5
	cmp	b[esi],'('
	jz	@@1_5
	cmp	b[esi],')'
	jz	@@1_5
	cmp	b[esi],'&'
	jz	@@1_5
	cmp	b[esi],'|'
	jz	@@1_5
	cmp	b[esi],'<'
	jz	@@1_5
	cmp	b[esi],'>'
	jz	@@1_5
	cmp	al,'A'
	jc	@@1_5
	cmp	al,'F'+1
	jc	@@1_2		;assume its hex at last.
@@1_5:	cmp	al,'B'
	jz	@@1_3		;assume its binary.
	pop	esi
@@1_1:	mov	ah,3		;set flag for label.
	jmp	@@2
@@1_2:	mov	ah,1		;set flag for hex.
	jmp	@@2
@@1_3:	mov	ah,2		;set flag for binary.
	jmp	@@2
@@1_4:	mov	ah,4		;set flag for string.
@@2:	pop	esi
	;
	;now convert this number into a useful format.
	;
	or	ah,ah		;Decimal?
	jz	@@DecGet
	dec	ah		;Hex?
	jz	@@HexGet
	dec	ah		;binary?
	jz	@@BinGet
	dec	ah
	jz	@@LabGet		;assume label then.
	jmp	@@StrGet		;get a string.
@@DecGet:	;
	;Convert this number from decimal to internal.
	;
	mov	b[edi],0		;Set number flag.
	inc	edi		;move to value field.
	push	edi
	xor	edx,edx
	xor	ecx,ecx		;reset accumulator.
	xor	edi,edi
@@DecGet_0:	lodsb
	or	al,al		;finished this number?
	jz	@@DecGet_1
	cmp	al,' '
	jz	@@DecGet_1
	cmp	al,9
	jz	@@DecGet_1
	cmp	al,'-'
	jz	@@DecGet_2
	cmp	al,'+'
	jz	@@DecGet_2
	cmp	al,'*'
	jz	@@DecGet_2
	cmp	al,'/'
	jz	@@DecGet_2
	cmp	al,'('
	jz	@@DecGet_2
	cmp	al,')'
	jz	@@DecGet_2
	cmp	al,'&'
	jz	@@DecGet_2
	cmp	al,'|'
	jz	@@DecGet_2
	cmp	al,'<'
	jz	@@DecGet_2
	cmp	al,'>'
	jz	@@DecGet_2
	sub	al,'0'		;make it a real number.
	movzx	ebp,al		;/
	shl	ecx,1
	rcl	edx,1
	rcl	edi,1
	mov	eax,ecx
	mov	ebx,edx
	shl	ecx,1
	rcl	edx,1
	rcl	edi,1
	shl	ecx,1
	rcl	edx,1
	rcl	edi,1
	add	ecx,eax
	adc	edx,ebx
	adc	edi,0
	add	ecx,ebp
	adc	edx,0
	adc	edi,0
	or	edi,edi
	jz	@@DecGet_0
	pop	edi
	jmp	@@13
@@DecGet_2:	dec	esi		;move back to operator.
@@DecGet_1:	pop	edi
	mov	[edi],ecx
	mov	[edi+4],edx
	mov	b[edi+8],-1		;preset terminator.
	add	edi,8		;move to next field.
	jmp	@@3
@@HexGet:	;
	;get a hex number.
	;
	mov	b[edi],0		;Set number flag.
	inc	edi		;move to value field.
	xor	edx,edx
	xor	ecx,ecx		;reset accumulator.
	xor	ebp,ebp
	cmp	b[esi],'$'		;$ at the start?
	jnz	@@HexGet_0
	inc	esi		;skip the $
@@HexGet_0:	lodsb
	call	UpperChar
	or	al,al		;finished this number?
	jz	@@HexGet_1
	cmp	al,'H'
	jz	@@HexGet_1
	cmp	al,' '
	jz	@@HexGet_1
	cmp	al,9
	jz	@@HexGet_1
	cmp	al,'-'
	jz	@@HexGet_2
	cmp	al,'+'
	jz	@@HexGet_2
	cmp	al,'*'
	jz	@@HexGet_2
	cmp	al,'/'
	jz	@@HexGet_2
	cmp	al,'('
	jz	@@HexGet_2
	cmp	al,')'
	jz	@@HexGet_2
	cmp	al,'&'
	jz	@@HexGet_2
	cmp	al,'|'
	jz	@@HexGet_2
	cmp	al,'<'
	jz	@@HexGet_2
	cmp	al,'>'
	jz	@@HexGet_2
	cmp	al,'0'
	jc	@@9		;bad value!
	cmp	al,'9'+1
	jc	@@HexGet_3
	cmp	al,'A'
	jc	@@9		;bad value!
	cmp	al,'F'+1
	jnc	@@9		;bad value.
@@HexGet_3:	cmp	al,'A'		;number or alpha?
	jc	@@HexGet_4
	sub	al,'A'		;make it real.
	add	al,10		;/
	jmp	@@HexGet_5
@@HexGet_4:	sub	al,'0'		;make it real.
@@HexGet_5:	cmp	bp,16+1
	jz	@@13
	inc	bp
	movzx	eax,al
	shl	ecx,1		;move current value up a nibble.
	rcl	edx,1
	shl	ecx,1
	rcl	edx,1
	shl	ecx,1
	rcl	edx,1
	shl	ecx,1
	rcl	edx,1
	add	ecx,eax
	adc	edx,0		;add in new value.
	jmp	@@HexGet_0		;keep looking.
@@HexGet_2:	dec	esi		;move back to operator.
@@HexGet_1:	mov	[edi],ecx
	mov	[edi+4],edx
	mov	b[edi+8],-1		;preset terminator.
	add	edi,8		;move to next field.
	jmp	@@3
@@BinGet:	;
	;get a binary number.
	;
	mov	b[edi],0		;Set number flag.
	inc	edi		;move to value field.
	xor	edx,edx
	xor	ecx,ecx		;reset accumulator.
	xor	ebp,ebp
	cmp	b[esi],'%'		;% at the start
	jnz	@@BinGet_0
	inc	esi		;step over the %
@@BinGet_0:	lodsb
	call	UpperChar
	or	al,al		;finished this number?
	jz	@@BinGet_1
	cmp	al,'B'
	jz	@@BinGet_1
	cmp	al,' '
	jz	@@BinGet_1
	cmp	al,9
	jz	@@BinGet_1
	cmp	al,'-'
	jz	@@BinGet_2
	cmp	al,'+'
	jz	@@BinGet_2
	cmp	al,'*'
	jz	@@BinGet_2
	cmp	al,'/'
	jz	@@BinGet_2
	cmp	al,'('
	jz	@@BinGet_2
	cmp	al,')'
	jz	@@BinGet_2
	cmp	al,'&'
	jz	@@BinGet_2
	cmp	al,'|'
	jz	@@BinGet_2
	cmp	al,'<'
	jz	@@BinGet_2
	cmp	al,'>'
	jz	@@BinGet_2
	cmp	al,'0'
	jc	@@9		;bad value!
	cmp	al,'1'+1
	jnc	@@9		;bad value.
	sub	al,'0'		;make it real.
	cmp	bp,64
	jz	@@13		;overflow error.
	inc	bp
	movzx	eax,al
	shl	ecx,1		;move current value up a bit.
	rcl	edx,1
	add	ecx,eax
	adc	edx,0		;add in new value.
	jmp	@@BinGet_0		;keep looking.
@@BinGet_2:	dec	esi		;move back to operator.
@@BinGet_1:	mov	[edi],ecx
	mov	[edi+4],edx
	mov	b[edi+8],-1		;preset terminator.
	add	edi,8		;move to next field.
	jmp	@@3
@@LabGet:	;
	;get a label value.
	;
	pushm	edi,esi
	call	GetSymbolValue	;get the value
	popm	edi,esi
	jc	@@12
	mov	b[edi],0
	inc	edi
	mov	[edi],ecx
	mov	[edi+4],edx
	mov	b[edi+8],-1
	add	edi,8		;next entry.
@@LabGet_0:	lodsb
	or	al,al
	jz	@@3		;end of this item.
	cmp	al,' '
	jz	@@LabGet_1
	cmp	al,9
	jz	@@LabGet_1
	cmp	al,'-'
	jz	@@LabGet_1
	cmp	al,'+'
	jz	@@LabGet_1
	cmp	al,'*'
	jz	@@LabGet_1
	cmp	al,'/'
	jz	@@LabGet_1
	cmp	al,'&'
	jz	@@LabGet_1
	cmp	al,'|'
	jz	@@LabGet_1
	cmp	al,'<'
	jz	@@LabGet_1
	cmp	al,'>'
	jz	@@LabGet_1
	cmp	al,'('
	jz	@@LabGet_1
	cmp	al,')'
	jnz	@@LabGet_0
@@LabGet_1:	dec	esi
	jmp	@@3
@@StrGet:	;
	;get a string value. If only one character then it will be put
	;in the buffer as a number. If more than 1 character then it
	;will be returned as is after quotes have been removed with CX
	;holding its length.
	;
	lodsb			;get quote type.
	xchg	ah,al
	cmp	b[esi+1],ah		;matching quote?
	jnz	@@StrGet_0		;copy the string.
	mov	b[edi],0		;set number flag.
	inc	edi
	lodsb			;get the character.
	movzx	eax,al
	mov	[edi],eax
	mov	d[edi+4],0		;set value in buffer.
	mov	b[edi+8],-1		;preset terminator.
	add	edi,8		;move to next field.
	lodsb			;get closing quote.
	jmp	@@3
	;
@@StrGet_0:	lea	edi,EvaluateBuffer	;copy over itself.
	push	ds
	pop	es
@@StrGet_1:	lodsb
	stosb
	cmp	al,ah		;matching quote?
	jz	@@StrGet_2		;end of string.
	or	al,al
	jz	@@10		;missing quote.
	jmp	@@StrGet_1
@@StrGet_2:	dec	edi
	mov	b[edi],0		;terminate it.
	sub	edi,offset EvaluateBuffer
	mov	ecx,edi		;set string length.
	mov	ax,-2		;set flags for a string.
	stc
	ret
@@3:	;
	;check if any more values to be converted.
	;
	cmp	b[esi-1],0		;end of this expresion?
	jnz	@@0
	sub	edi,Offset EvaluateBuffer2 ;get length of expresion.
	mov	ecx,edi		;/
	inc	ecx		;include terminator.
	lea	esi,EvaluateBuffer2
	lea	edi,EvaluateBuffer
	push	ds
	pop	es
	;
	;If buffer starts with an operator (+-() then insert a value of
	;zero at the start of the buffer.
	;
	mov	al,[esi]
	cmp	al,'-'
	jz	@@3_0
	cmp	al,'+'
	jz	@@3_0
	cmp	al,'&'
	jz	@@3_0
	cmp	al,'|'
	jz	@@3_0
	cmp	al,'<'
	jz	@@3_0
	cmp	al,'>'
	jz	@@3_0
	cmp	al,'*'
	jz	@@9
	cmp	al,'/'
	jz	@@9
	cmp	al,'('
	jz	@@3_1
	cmp	al,')'
	jz	@@9
	jmp	@@3_2
@@3_0:	mov	b[edi],0
	mov	d[edi+1],0
	mov	d[edi+5],0
	add	edi,9
	jmp	@@3_2
@@3_1:	mov	b[edi],0
	mov	d[edi+1],0
	mov	d[edi+5],0
	mov	b[edi+9],'+'
	add	edi,10
@@3_2:	rep	movsb		;copy it back into the buffer.
	;
	;now expand any parenthesis
	;
@@ParenLook:	lea	esi,EvaluateBuffer
	xor	dl,dl		;reset found flag.
	xor	dh,dh		;reset open flag.
@@ParenLook_0:	cmp	b[esi],-1		;end of the line?
	jz	@@ParenLook_1
	cmp	b[esi],'('		;opening?
	jz	@@ParenLook_4
	or	dh,dh
	jnz	@@b0
	cmp	b[esi],')'
	jz	@@14		;Missing opening parenthesis.
@@b0:	inc	esi
	cmp	b[esi-1],0		;number?
	jnz	@@ParenLook_0
	add	esi,8		;skip number.
	jmp	@@ParenLook_0		;keep looking.
@@ParenLook_4:	mov	dh,1
	mov	ebp,esi		;store this position.
	inc	esi		;move over it
	jmp	@@ParenLook_0		;find closing or next!
@@ParenLook_1:	or	dh,dh		;find anything?
	jz	@@ParenLook_10
	;
	mov	esi,ebp		;Opening pos again.
@@ParenLook_11: cmp	b[esi],-1
	jz	@@10		;missing closing.
	cmp	b[esi],')'
	jz	@@ParenLook_12
	inc	esi
	cmp	b[esi-1],0
	jnz	@@ParenLook_11
	add	esi,8
	jmp	@@ParenLook_11
@@ParenLook_12:	;
	;copy current line into a buffer and copy section between
	;parenthesis into normal buffer.
	;
	push	ds
	pop	es
	push	esi
	lea	esi,EvaluateBuffer
	lea	edi,EvaluateBuffer3
	mov	ecx,MaxLineLength/4
	rep	movsd		;copy it.
	pop	esi
	;
	;copy section between , BP+1 & SI-1 to start of buffer.
	;
	pushm	ebp,esi
	dec	esi
	inc	ebp
	sub	esi,ebp
	mov	ecx,esi
	inc	ecx
	mov	esi,ebp
	lea	edi,EvaluateBuffer
	sub	esi,offset EvaluateBuffer
	add	esi,offset EvaluateBuffer3 ;use 3 as source.
	;
	;If buffer starts with an operator (+-() then insert a value of
	;zero at the start of the buffer.
	;
	mov	al,[esi]
	cmp	al,'-'
	jz	@@P3_0
	cmp	al,'+'
	jz	@@P3_0
	cmp	al,'&'
	jz	@@P3_0
	cmp	al,'|'
	jz	@@P3_0
	cmp	al,'<'
	jz	@@9
	cmp	al,'>'
	jz	@@9
	cmp	al,'*'
	jz	@@9
	cmp	al,'/'
	jz	@@9
	cmp	al,'('
	jz	@@P3_1
	cmp	al,')'
	jz	@@9
	jmp	@@P3_2
@@P3_0:	mov	b[edi],0
	mov	d[edi+1],0
	mov	d[edi+5],0
	add	edi,9
	jmp	@@P3_2
@@P3_1:	mov	b[edi],0
	mov	d[edi+1],0
	mov	d[edi+5],0
	mov	b[edi+9],'+'
	add	edi,10
@@P3_2:	rep	movsb
	mov	b[edi],-1		;make sure its terminated.
	;
	;Evaluate the expresion
	;
	call	CalculateNum
	push	ds
	pop	es
	popm	ebp,esi
	jc	@@9		;bad number!
	;
	;put result somewhere safe.
	;
	pushm	esi,ebp
	lea	esi,EvaluateBuffer
	lea	edi,EvaluateBuffer2
	mov	ecx,9
	rep	movsb
	popm	esi,ebp
	;
	;insert new value.
	;
	mov	edx,esi
	lea	esi,EvaluateBuffer3
	lea	edi,EvaluateBuffer
	mov	ecx,ebp
	sub	ecx,offset EvaluateBuffer	;how much to copy back.
	or	ecx,ecx
	jz	@@ParenLook_6
	rep	movsb		;move up to parenthesis.
@@ParenLook_6:	lea	esi,EvaluateBuffer2
	mov	ecx,9
	rep	movsb		;copy new value.
	mov	esi,edx
	inc	esi		;point to data after closing paren
	sub	esi,offset EvaluateBuffer
	mov	ecx,esi
	add	esi,offset EvaluateBuffer3
	sub	ecx,MaxLineLength
	neg	ecx		;get length remaining.
	rep	movsb		;copy the rest.
@@ParenLook_5:	jmp	@@ParenLook
@@ParenLook_10:	;
	;Calculate real number now!
	;
	call	CalculateNum
	jc	@@9
	;
	;now move the result into a useful place.
	;
	lea	esi,EvaluateBuffer+1
	lea	edi,EvaluateBuffer
	cmp	b[edi],0		;final check for a number.
	jnz	@@9
	movsd
	movsd			;shift the result!
	lea	esi,EvaluateBuffer
	test	w[esi+4],1 shl 31
	jnz	@@CheckNeg
	mov	eax,[esi]
	and	eax,VarSizeMask
	mov	edx,eax
	mov	eax,[esi+4]
	and	eax,VarSizeMask+4
	or	eax,edx
	jnz	@@13
	jmp	@@ItsOk
@@CheckNeg:	xor	eax,eax
	xor	edx,edx
	sub	eax,[esi]
	sbb	edx,[esi+4]
	and	eax,VarSizeMask
	and	edx,VarSizeMask+4
	or	eax,edx
	jnz	@@13
@@ItsOk:	;
	;set flags for succesful exit.
	;
	clc
	ret
@@15:	mov	ax,16
	jmp	@@11
@@14:	mov	ax,17
	jmp	@@11
@@13:	mov	ax,18
	jmp	@@11
@@12:	mov	ax,19
	jmp	@@11
@@10:	mov	ax,16
	jmp	@@11
@@9:	mov	ax,21
@@11:	stc
	ret
EvaluateData	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;convert the number in the buffer into a real number.
;
CalculateNum	proc	near
@@ShlExpand:	lea	esi,EvaluateBuffer
	lea	edi,EvaluateBuffer2
	push	ds
	pop	es
	xor	bx,bx		;reset expansion flag.
@@ShlExpand_0:	cmp	b[esi],'<'
	jz	@@ShlExpand_2		;do the addition.
	mov	ebp,edi		;point to current value.
	movsb			;move type byte.
	cmp	b[esi-1],0		;is it a number?
	jnz	@@ShlExpand_1
	mov	ecx,[esi]
	movsd			;move the value.
	mov	edx,[esi]		;setup the accumulator.
	movsd
@@ShlExpand_1:	cmp	ds:b[ebp],-1		;last value?
	jnz	@@ShlExpand_0		;keep looking.
	jmp	@@ShlExpand_3
@@ShlExpand_2:	inc	esi		;move to next number.
	cmp	b[esi],0		;check its a number.
	jnz	@@9		;bad expresion.
	mov	eax,ecx
	mov	ecx,[esi+1]
	or	ecx,ecx
	jz	@@ShlLoop99
@@ShlLoop:	shl	eax,1
	rcl	edx,1
	jc	@@10
	loop	@@ShlLoop
@@ShlLoop99:	mov	ds:[ebp+1],eax
	mov	ds:[ebp+5],edx		;store the result.
	add	esi,9		;point to next entry.
	mov	bl,1		;flag action.
	jmp	@@ShlExpand_0		;go look for more action.
@@ShlExpand_3:	mov	b[ResultBuffer+8],0
	sub	edi,offset EvaluateBuffer2 ;get length of expresion.
	mov	ecx,edi		;/
	inc	ecx		;include terminator.
	lea	esi,EvaluateBuffer2
	lea	edi,EvaluateBuffer
	rep	movsb		;copy it back into the buffer.
	or	bl,bl		;did we expand anything?
	jnz	@@ShlExpand		;do it again till no more work.
@@ShrExpand:	lea	esi,EvaluateBuffer
	lea	edi,EvaluateBuffer2
	push	ds
	pop	es
	xor	bx,bx		;reset expansion flag.
@@ShrExpand_0:	cmp	b[esi],'>'
	jz	@@ShrExpand_2		;do the addition.
	mov	ebp,edi		;point to current value.
	movsb			;move type byte.
	cmp	b[esi-1],0		;is it a number?
	jnz	@@ShrExpand_1
	mov	ecx,[esi]
	movsd			;move the value.
	mov	edx,[esi]		;setup the accumulator.
	movsd
@@ShrExpand_1:	cmp	ds:b[ebp],-1		;last value?
	jnz	@@ShrExpand_0		;keep looking.
	jmp	@@ShrExpand_3
@@ShrExpand_2:	inc	esi		;move to next number.
	cmp	b[esi],0		;check its a number.
	jnz	@@9		;bad expresion.
	mov	eax,ecx
	mov	ecx,[esi+1]
	or	ecx,ecx
	jz	@@ShrLoop99
@@ShrLoop:	shr	edx,1
	rcr	eax,1
	loop	@@ShrLoop
@@ShrLoop99:	mov	ds:[ebp+1],eax
	mov	ds:[ebp+5],edx	;store the result.
	add	esi,9		;point to next entry.
	mov	bl,1		;flag action.
	jmp	@@ShrExpand_0		;go look for more action.
@@ShrExpand_3:	mov	b[ResultBuffer+8],0
	sub	edi,offset EvaluateBuffer2 ;get length of expresion.
	mov	ecx,edi		;/
	inc	ecx		;include terminator.
	lea	esi,EvaluateBuffer2
	lea	edi,EvaluateBuffer
	rep	movsb		;copy it back into the buffer.
	or	bl,bl		;did we expand anything?
	jnz	@@ShrExpand		;do it again till no more work.
	;
	;now expand any multiplication
	;
@@MulExpand:	lea	esi,EvaluateBuffer
	lea	edi,EvaluateBuffer2
	push	ds
	pop	es
	xor	bl,bl		;reset expansion flag.
@@MulExpand_0:	cmp	b[esi],'*'
	jz	@@MulExpand_2		;do the multiplication.
	mov	ebp,edi		;point to current value.
	movsb			;move type byte.
	cmp	b[esi-1],0		;is it a number?
	jnz	@@MulExpand_1
	mov	ecx,[esi]
	movsd			;move the value.
	mov	edx,[esi]		;setup the accumulator.
	movsd
@@MulExpand_1:	cmp	ds:b[ebp],-1		;last value?
	jnz	@@MulExpand_0		;keep looking.
	jmp	@@MulExpand_3
@@MulExpand_2:	inc	esi		;move to next number.
	cmp	b[esi],0		;check its a number.
	jnz	@@9		;bad expresion.
	pushm	esi,edi,bx
	inc	esi
	mov	edi,ebp
	inc	edi
	call	Mult64
	lea	ebx,ResultBuffer
	mov	ecx,[ebx]
	mov	edx,[ebx+4]		;get the result.
	mov	eax,[ebx+12]
	or	eax,[ebx+8]
	popm	esi,edi,bx
	jnz	@@10
	mov	ds:[ebp+1],ecx
	mov	ds:[ebp+5],edx	;store the result.
	add	esi,9		;point to next entry.
	mov	bl,1		;flag action.
	jmp	@@MulExpand_0		;go look for more action.
@@MulExpand_3:	sub	edi,offset EvaluateBuffer2 ;get length of expresion.
	mov	ecx,edi		;/
	inc	ecx		;include terminator.
	lea	esi,EvaluateBuffer2
	lea	edi,EvaluateBuffer
	rep	movsb		;copy it back into the buffer.
	or	bl,bl		;did we expand anything?
	jnz	@@MulExpand		;do it again till no more work.
	;
	;expand any division
	;
@@DivExpand:	lea	esi,EvaluateBuffer
	lea	edi,EvaluateBuffer2
	push	ds
	pop	es
	xor	bl,bl		;reset expansion flag.
@@DivExpand_0:	cmp	b[esi],'/'
	jz	@@DivExpand_2		;do the multiplication.
	mov	ebp,edi		;point to current value.
	movsb			;move type byte.
	cmp	b[esi-1],0		;is it a number?
	jnz	@@DivExpand_1
	mov	ecx,[esi]
	movsd			;move the value.
	mov	edx,[esi]		;setup the accumulator.
	movsd
@@DivExpand_1:	cmp	ds:b[ebp],-1		;last value?
	jnz	@@DivExpand_0		;keep looking.
	jmp	@@DivExpand_3
@@DivExpand_2:	inc	esi		;move to next number.
	cmp	b[esi],0		;check its a number.
	jnz	@@9		;bad expresion.
	pushm	esi,edi,ebx,ebp
	inc	esi		;point to divisor.
	mov	eax,[esi]
	mov	ExtraSpace,eax
	mov	eax,[esi+4]
	mov	ExtraSpace+4,eax
	mov	edi,ebp
	inc	edi
	mov	eax,[edi]
	mov	TempWord,eax
	mov	eax,[edi+4]
	mov	TempWord+4,eax
	lea	esi,TempWord
	lea	edi,ExtraSpace
	call	Div64
	lea	ebx,TempWord
	mov	ecx,[ebx]
	mov	edx,[ebx+4]		;get the result.
	popm	esi,edi,ebx,ebp
	mov	ds:[ebp+1],ecx
	mov	ds:[ebp+5],edx	;store the result.
	add	esi,9		;point to next entry.
	mov	bl,1		;flag action.
	jmp	@@DivExpand_0		;go look for more action.
@@DivExpand_3:	sub	edi,offset EvaluateBuffer2 ;get length of expresion.
	mov	ecx,edi		;/
	inc	ecx		;include terminator.
	lea	esi,EvaluateBuffer2
	lea	edi,EvaluateBuffer
	rep	movsb		;copy it back into the buffer.
	or	bl,bl		;did we expand anything?
	jnz	@@DivExpand		;do it again till no more work.
	;
	;now expand any addition/subtraction.
	;
@@AddExpand:	lea	esi,EvaluateBuffer
	lea	edi,EvaluateBuffer2
	push	ds
	pop	es
	xor	bx,bx		;reset expansion flag.
@@AddExpand_0:	cmp	b[esi],-1		;end of the line?
	jz	@@AddExpand_3		;copy back to buffer.
	cmp	b[esi],0
	jz	@@AddExpand_5		;copy number.
	mov	al,[esi]		;get operator.
	inc	esi
	cmp	b[esi],0		;check number next.
	jnz	@@9
	;
	cmp	al,'+'
	jz	@@AddExpand_2		;do the addition.
	cmp	al,'-'
	jz	@@SubExpand_2		;do the subtraction.
	cmp	al,'&'
	jz	@@AndExpand_2		;do the and.
	cmp	al,'|'
	jz	@@OrExpand_2		;do the or.
	;
	jmp	@@9		;dunno what this is!
	;
@@AddExpand_5:	mov	ebp,edi		;point to current value.
	movsb			;move type byte.
	mov	ecx,[esi]
	movsd			;move the value.
	mov	edx,[esi]		;setup the accumulator.
	movsd
	jmp	@@AddExpand_0	;keep looking.
	;
@@AddExpand_2:	add	ecx,[esi+1]
	adc	edx,[esi+5]		;perform addition.
	jnc	@@AddExpand_4
	cmp	ds:d[ebp+5],0
	jns	@@10
@@AddExpand_4:	mov	ds:[ebp+1],ecx
	mov	ds:[ebp+5],edx	;store the result.
	add	esi,9		;point to next entry.
	mov	bl,1		;flag action.
	jmp	@@AddExpand_0		;go look for more action.
	;
@@SubExpand_2:	sub	ecx,[esi+1]
	sbb	edx,[esi+5]		;perform subtraction.
	jmp	@@AddExpand_4
	;
@@AndExpand_2:	and	ecx,[esi+1]
	and	edx,[esi+5]		;perform addition.
	jmp	@@AddExpand_4
	;
@@OrExpand_2: 	or	ecx,[esi+1]
	or	edx,[esi+5]		;perform addition.
	jmp	@@AddExpand_4
	;
@@AddExpand_3:	mov	b[edi],-1
	mov	b[ResultBuffer+8],bh
	or	bh,bh
	jnz	@@9
	sub	edi,offset EvaluateBuffer2 ;get length of expresion.
	mov	ecx,edi		;/
	inc	ecx		;include terminator.
	lea	esi,EvaluateBuffer2
	lea	edi,EvaluateBuffer
	rep	movsb		;copy it back into the buffer.
	or	bl,bl		;did we expand anything?
	jnz	@@AddExpand		;do it again till no more work.
	;
	clc
	ret
	;
@@10:	mov	ax,18
@@9:	stc
	ret
CalculateNum	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;MULT two 64 bit numbers together
;point EDI at one, ESI at the other,
;result in RESULT, can be 128 bit, to allow checking for overflow
;
MULT64	proc	near
	mov	resultbuffer,0
	mov	resultbuffer+4,0
	mov	resultbuffer+8,0
	mov	resultbuffer+12,0
	mov	eax,[edi]	;move to where it can be worked on
	mov	tempword,eax
	mov	eax,[edi+4]	;ditto second word
	mov	tempword+4,eax
	mov	TempWord+8,0
	mov	TempWord+12,0
	lea	edi,tempword	;point to workspace
	mov	eax,[esi]	;move to where it can be worked on
	mov	tempdiv,eax
	mov	eax,[esi+4]	;sitto second word
	mov	tempdiv+4,eax
	mov	Tempdiv+8,0
	mov	TempDiv+12,0
	lea	esi,tempdiv	;point to workspace
	mov	ecx,64
@@MULTLP:	shr	b[esi+7],1
	rcr	b[esi+6],1
	rcr	b[esi+5],1
	rcr	b[esi+4],1
	rcr	b[esi+3],1
	rcr	b[esi+2],1
	rcr	b[esi+1],1
	rcr	b[esi+0],1
	jnc	@@NOADD
	mov	eax,[edi]
	add	resultbuffer,eax	;this SHOULD be JUST an ADD not ADC!
	mov	eax,[edi+4]
	adc	resultbuffer+4,eax	
	mov	eax,[edi+8]		;this can give you a 64 bit result!!!
	adc	resultbuffer+8,eax	
	mov	eax,[edi+12]
	adc	resultbuffer+12,eax	
@@NOADD:	mov	eax,[edi]
	add	[edi],eax	;this SHOULD be JUST an ADD not ADC!
	mov	eax,[edi+4]
	adc	[edi+4],eax
	mov	eax,[edi+8]
	adc	[edi+8],eax
	mov	eax,[edi+12]
	adc	[edi+12],eax
	loop	@@MULTLP
	ret
Mult64	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
DIV64	proc	near
	mov	temp,0	;init temp
	mov	temp+4,0	
	mov	ecx,64	;NUMBER OF BITS!!!!!!!
@@DIVLP:	rcl	b[esi+0],1
	rcl	b[esi+1],1
	rcl	b[esi+2],1
	rcl	b[esi+3],1
	rcl	b[esi+4],1
	rcl	b[esi+5],1
	rcl	b[esi+6],1
	rcl	b[esi+7],1
	mov	eax,temp	;get low 'bx'
	adc	temp,eax	;add to itself
	mov	eax,temp+4 
	adc	temp+4,eax
	mov	eax,[edi]
	sbb	temp,eax
	mov	eax,[edi+4]
	sbb	[temp+4],eax
	jnc	@@OKAY
	mov	eax,[edi]
	add	[temp],eax	;this SHOULD be JUST an ADD not ADC!
	mov	eax,[edi+4]
	adc	[temp+4],eax
	stc
@@OKAY:	cmc		;complement carry (diff if jumped from above)
	loop	@@DIVLP
	rcl	b[esi+0],1
	rcl	b[esi+1],1
	rcl	b[esi+2],1
	rcl	b[esi+3],1
	rcl	b[esi+4],1
	rcl	b[esi+5],1
	rcl	b[esi+6],1
	rcl	b[esi+7],1
	ret
Div64	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
ExpandLine	proc	near
	ret
ExpandLine	endp


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Look through symbols for a match.
;
GetSymbolValue	proc	near
	;
	;Check if a register is being used.
	;
	cmp	b[esi+1],0
	jz	@@NotReg
	cmp	b[esi+2],0
	jz	@@Reg16
	cmp	b[esi+2]," "
	jz	@@Reg16
	cmp	b[esi+2],"+"
	jz	@@Reg16
	cmp	b[esi+2],"-"
	jz	@@Reg16
	cmp	b[esi+2],"*"
	jz	@@Reg16
	cmp	b[esi+2],"("
	jz	@@Reg16
	cmp	b[esi+2],")"
	jz	@@Reg16
	cmp	b[esi+2],"/"
	jz	@@Reg16
	cmp	b[esi+2],"&"
	jz	@@Reg16
	cmp	b[esi+2],"<"
	jz	@@Reg16
	cmp	b[esi+2],">"
	jz	@@Reg16
	;
	cmp	b[esi+3],0
	jz	@@Reg32
	cmp	b[esi+3]," "
	jz	@@Reg32
	cmp	b[esi+3],"+"
	jz	@@Reg32
	cmp	b[esi+3],"-"
	jz	@@Reg32
	cmp	b[esi+3],"*"
	jz	@@Reg32
	cmp	b[esi+3],"("
	jz	@@Reg32
	cmp	b[esi+3],")"
	jz	@@Reg32
	cmp	b[esi+3],"/"
	jz	@@Reg32
	cmp	b[esi+3],"&"
	jz	@@Reg32
	cmp	b[esi+3],"<"
	jz	@@Reg32
	cmp	b[esi+3],">"
	jz	@@Reg32
	;
	jmp	@@NotReg
	;
@@Reg16:	push	ebx
	mov	bl,b[esi+2]
	mov	b[esi+2],0
	pushm	ebx,esi
	call	WatchSegAtESI
	popm	ebx,esi
	mov	b[esi+2],bl
	pop	ebx
	jc	@@Reg16g
	call	WatchRegToValue
	mov	edx,eax
	xor	ecx,ecx
	jmp	@@GotReg
	;
@@Reg16g:	push	ebx
	mov	bl,b[esi+2]
	mov	b[esi+2],0
	pushm	ebx,esi
	call	WatchERegAtESI
	popm	ebx,esi
	mov	b[esi+2],bl
	pop	ebx
	jc	@@NotReg
	jmp	@@GetgReg
	;
@@Reg32:	push	ebx
	mov	bl,[esi+3]
	mov	b[esi+3],0
	pushm	ebx,esi
	call	WatchERegAtESI
	popm	ebx,esi
	mov	b[esi+3],bl
	pop	ebx
	jc	@@NotReg
@@GetgReg:	call	WatchRegToValue
	mov	ecx,eax
	xor	edx,edx
	;
@@GotReg:	clc
	ret
@@NotReg:	mov	edi,SymbolList
@@0:	cmp	d[edi],-1		;end of the list?
	jz	@@9
	pushm	esi,edi
	movzx	ecx,SymbolTLen[edi]
	add	edi,SymbolText
@@1:	lodsb
	call	UpperChar
	mov	ah,al
	mov	al,[edi]
	inc	edi
	call	UpperChar
	;
	cmp	al,ah
	jnz	@@3
	loop	@@1
	lodsb
	cmp	al,' '
	jz	@@4
	cmp	al,'+'
	jz	@@4
	cmp	al,'-'
	jz	@@4
	cmp	al,'*'
	jz	@@4
	cmp	al,'('
	jz	@@4
	cmp	al,')'
	jz	@@4
	cmp	al,'/'
	jz	@@4
	cmp	al,'&'
	jz	@@4
	cmp	al,'<'
	jz	@@4
	cmp	al,'>'
	jz	@@4
	or	al,al
	jz	@@4
@@3:	popm	esi,edi
	add	edi,SymbolNext[edi]
	jmp	@@0
@@4:	popm	esi,edi
	mov	ecx,SymbolDWord[edi]
	movzx	edx,SymbolSeg[edi]
	clc
	ret
@@9:	stc
	ret
GetSymbolValue	endp


	.data


EvaluateBuffer	db MaxLineLength dup (?)
EvaluateBuffer2 db MaxLineLength dup (?)
EvaluateBuffer3 db MaxLineLength dup (?)
VarSizeMask	dd 0,0
Temp	dd ?,?
ResultBuffer	dd 2*4 dup (0)
TempDiv	dd 2*4 dup (0)
TempWord	dd 2*4 dup (0)
ExtraSpace	dd 2*4 dup (0)
ProgramCounter	dd 0,0

