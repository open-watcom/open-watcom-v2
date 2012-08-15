;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
StringPrint	proc	near
;
;Print characters on the screen.
;
;On Entry:-
;
;DS:EDX	- String (0 terminated)
;
;ALL registers preserved.
;
	pushm	eax,ebx,ecx,edx,esi,edi,ebp
	mov	esi,edx
@@0:	mov	al,[esi]
	inc	esi
	or	al,al
	jz	@@9
	push	esi
	mov	dl,al
	mov	ah,2
	int	21h
	pop	esi
	jmp	@@0
@@9:	popm	eax,ebx,ecx,edx,esi,edi,ebp
	ret
StringPrint	endp




