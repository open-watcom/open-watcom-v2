	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set hardware palette. Generates new system to hardware translation table
;and re-maps all "system" bitmaps.
;
;On Entry:
;
;EAX	- Base
;ECX	- Number.
;ESI	- data.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
VideoSetPalette proc near
	pushad
;
;Copy into the palette buffer first.
;
	mov	edi,eax
	shl	edi,1
	add	edi,eax
	add	edi,offset HardwarePalette
	mov	eax,ecx
	shl	ecx,1
	add	ecx,eax
	cld
	rep	movsb
;
;Now scan the palette & update hardware for values that have changed.
;
	mov	esi,offset HardwarePalette	;palette values to check.
	xor	ebx,ebx		;initial pen number.
	mov	ecx,ebx		;initial count value.
	mov	edx,ebx		;running base pen number.
	mov	_VSP_Count,256	;number of pens to scan.
ScanHard:	mov	al,0[esi]
	cmp	al,0+(256*3)[esi]
	jnz	NoteHard
	mov	al,1[esi]
	cmp	al,1+(256*3)[esi]
	jnz	NoteHard
	mov	al,2[esi]
	cmp	al,2+(256*3)[esi]
	jnz	NoteHard
;
;Any values need sending?
;
CheckHard:	or	cx,cx		;did we have any values to send?
	jz	NextHard
	push	esi
	mov	esi,edi
	call	SetDacs
	pop	esi
	xor	ecx,ecx		;clear counter.
	jmp	NextHard
;
;Set vars to reflect finding a change.
;
NoteHard:	inc	ecx		;update counter.
	cmp	ecx,1		;have we set base values?
	jnz	NextHard
	mov	ebx,edx		;store pen number.
	mov	edi,esi		;store table position.
;
;Move along.
;
NextHard:	inc	edx		;update base pen number.
	add	esi,3		;move to next pens details.
	dec	_VSP_Count
	jnz	ScanHard
	or	cx,cx		;anything waiting to be sent?
	jz	NoSendHard
	mov	esi,edi
	call	SetDacs
;
;Copy new palette into storage.
;
NoSendHard:	mov	esi,offset HardwarePalette
	mov	edi,offset HardwarePalette+(256*3)
	mov	ecx,256*3
	rep	movsb		;copy new palette into old space.
;
;Update system bitmaps
;
	call	RemapSystemBitmaps
;
	popad
	ret
VideoSetPalette endp


	sdata

_VSP_Count	dd ?


	efile
	end

