.model small
STDOUT  EQU 1
.stack 100h
.data
crlf	DB	13,10
	DW	4000 DUP (0)	; bulk up size by 8K to make large enough for modifications
.code
start:
	mov	ds,es:[2ch]	; ds -> e-var block (es -> PSP on entry)
	xor	si,si		; init offset in block
	mov	bx,STDOUT

loop2:
	mov	al,ds:[si]	; get first char of string
	or	al,al		; see if null (no more strings)
	je	done

loop1:
	mov	dx,si		; ds:dx -> string to print
	mov	cx,1
	mov	ah,40h		; write to device
	int	21h
	inc	si			; move to next char in string
	mov	al,ds:[si]	; print remaining chars in string one at a time
	or	al,al		; see if null (end of string)
	jne	loop1		; nope

	push	ds		; save ds -> e-var block
	mov	ax,DGROUP
	mov	ds,ax
	mov	dx,OFFSET DGROUP:crlf	; write CR/LF pair after string end
	mov	cl,2
	mov	ah,40h		; write to device
	int	21h
	pop	ds			; restore ds -> e-var block
	inc	si
	jmp	SHORT loop2

done:
	mov	ax,4c00h	; terminate with zero return code
	int	21h
end start
