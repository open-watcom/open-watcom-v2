	.code


;==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==
;
;Ask DOS for any pending key press information.
;
;No entry conditions.
;
;On Exit:-
;
;If flags none zero,AL - main byte  AH - Auxiluary byte
;otherwise, no keypress available.
;
;KEYS - copy of AX
;
GetKeys	proc	near
	mov	ah,1
	int	16h
	mov	ax,0
	jz	@@0
	mov	ah,0
	int	16h
	mov	Keys,ax
	cmp	ax,0
@@0:	ret
;
GetKeys	endp


	.data


Keys	dw 0

