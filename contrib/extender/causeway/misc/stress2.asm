; STRESS2.ASM program
.model medium
dosseg
CYCLECOUNT  EQU 70
public  storage
.stack 400h
.data
counter DW  0
toggle  DB  0
CheckValues	DD	0
			DD	12345678h
			DD	23456789h
			DD	3456789ah
			DD	456789abh
			DD	56789abch
			DD	6789abcdh
			DD	789abcdeh
			DD	89abcdefh
			DD	9abcdef0h
			DD	0abcdef01h
			DD	0bcdef012h
			DB	'End of check values'
.data?
storage DW CYCLECOUNT DUP (12 DUP (?))
	DB	'Hi Michael!'	; debug signature check
.const
ErrorText   DB  "Error",13,10
WorksText   DB  "Works",13,10
CycleText   DB  'Cycle',13,10
CycleAgainText  DB  'Cycle again',13,10
.code
.386p
start:
    mov ax,DGROUP
    mov ds,ax
    mov es,ax
    mov di,OFFSET DGROUP:storage

looper:
    mov dx,OFFSET DGROUP:CycleText
    mov cx,7
    xor toggle,1
    jne show
    mov dx,OFFSET DGROUP:CycleAgainText
    mov cx,13

show:
    mov bx,1
    mov ah,40h
    int 21h

; 1K
    mov dx,1*1024
    xor cx,cx
    mov ax,0ff0bh
    int 31h
	mov	cx,dx
    mov dx,1
    jc  error
    mov ax,bx
    stosw
	mov	dx,30
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(1*1024)/4
	xor	di,di
	mov	eax,0
	rep	stosd
	pop	es
	pop	di

; 3K
    mov dx,3*1024
    xor cx,cx
    mov ax,0ff0bh
    int 31h
	mov	cx,dx
    mov dx,2
    jc  error
    mov ax,bx
    stosw
	mov	dx,31
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(3*1024)/4
	xor	di,di
	mov	eax,12345678h
	rep	stosd
	pop	es
	pop	di

; 4K
    mov dx,4*1024
    xor cx,cx
    mov ax,0ff0bh
    int 31h
	mov	cx,dx
    mov dx,3
    jc  error
    mov ax,bx
    stosw
	mov	dx,32
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(4*1024)/4
	xor	di,di
	mov	eax,23456789h
	rep	stosd
	pop	es
	pop	di

; 5K
    mov dx,5*1024
    xor cx,cx
    mov ax,0ff0bh
    int 31h
	mov	cx,dx
    mov dx,4
    jc  error
    mov ax,bx
    stosw
	mov	dx,33
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(5*1024)/4
	xor	di,di
	mov	eax,3456789ah
	rep	stosd
	pop	es
	pop	di

; 15K
    mov dx,15*1024
    xor cx,cx
    mov ax,0ff0bh
    int 31h
	mov	cx,dx
    mov dx,5
    jc  error
    mov ax,bx
    stosw
	mov	dx,34
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(15*1024)/4
	xor	di,di
	mov	eax,456789abh
	rep	stosd
	pop	es
	pop	di

; 16K
    mov dx,16*1024
    xor cx,cx
    mov ax,0ff0bh
    int 31h
	mov	cx,dx
    mov dx,6
    jc  error
    mov ax,bx
    stosw
	mov	dx,35
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(16*1024)/4
	xor	di,di
	mov	eax,56789abch
	rep	stosd
	pop	es
	pop	di

; 17K
    mov dx,17*1024
    xor cx,cx
    mov ax,0ff0bh
    int 31h
	mov	cx,dx
    mov dx,7
    jc  error
    mov ax,bx
    stosw
	mov	dx,36
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(17*1024)/4
	xor	di,di
	mov	eax,6789abcdh
	rep	stosd
	pop	es
	pop	di

; 31K
    mov dx,31*1024
    xor cx,cx
    mov ax,0ff0bh
    int 31h
	mov	cx,dx
    mov dx,8
    jc  error
    mov ax,bx
    stosw
	mov	dx,37
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(31*1024)/4
	xor	di,di
	mov	eax,789abcdeh
	rep	stosd
	pop	es
	pop	di

; 32K
    mov dx,32*1024
    xor cx,cx
    mov ax,0ff0bh
    int 31h
	mov	cx,dx
    mov dx,9
    jc  error
    mov ax,bx
    stosw
	mov	dx,38
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(32*1024)/4
	xor	di,di
	mov	eax,89abcdefh
	rep	stosd
	pop	es
	pop	di

; 33K
    mov dx,33*1024
    xor cx,cx
    mov ax,0ff0bh
    int 31h
	mov	cx,dx
    mov dx,10
    jc  error
    mov ax,bx
    stosw
	mov	dx,39
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(33*1024)/4
	xor	di,di
	mov	eax,9abcdef0h
	rep	stosd
	pop	es
	pop	di

; 63K
    mov dx,63*1024
    xor cx,cx
    mov ax,0ff0bh
    int 31h
	mov	cx,dx
    mov dx,11
    jc  error
    mov ax,bx
    stosw
	mov	dx,40
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(63*1024)/4
	xor	di,di
	mov	eax,0abcdef01h
	rep	stosd
	pop	es
	pop	di

; 64K
    xor dx,dx
    mov cx,1
    mov ax,0ff0bh
    int 31h
    mov dx,12
    jc  error
    mov ax,bx
    stosw
	mov	dx,41
	movzx	ebx,bx
	lsl	eax,ebx
	inc	eax
	cmp	eax,10000h
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(64*1024)/4
	xor	di,di
	mov	eax,0bcdef012h
	rep	stosd
	pop	es
	pop	di

; check free memory operations
    mov dx,-1
    mov cx,-1
    mov ax,0ff0bh
    int 31h

    mov dx,-2
    mov cx,-1
    mov ax,0ff0bh
    int 31h

; bump cycle, check for end
    inc counter
    mov ax,counter
    cmp ax,CYCLECOUNT
    je  fini
    mov bl,5
    div bl
    or  ah,ah
    jne looper

; resize and release things every fifth cycle
; free
    mov si,di
    sub si,(2*(2*12))+2
    mov bp,12

loop2:
    mov bx,ds:[si]
	mov	WORD PTR ds:[si],0
    sub si,2
    mov ax,0ff0fh
    int 31h
    mov dx,15
    jc  error
    dec bp
    jne loop2

; resize 63K
    mov si,di
    sub si,(3*(2*12))+2
    mov bp,12

loop3:
    mov bx,ds:[si]
    sub si,2
    mov dx,63*1024
    xor cx,cx
    mov ax,0ff0dh
    int 31h
	mov	cx,dx
    mov dx,16
    jc  error
	mov	dx,42
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(63*1024)/4
	xor	di,di
	mov	eax,es:[0]
	rep	stosd
	pop	es
	pop	di
    dec bp
    jne loop3

; resize 15K
    mov si,di
    sub si,(4*(2*12))+2
    mov bp,12

loop4:
    mov bx,ds:[si]
    sub si,2
    mov dx,15*1024
    xor cx,cx
    mov ax,0ff0dh
    int 31h
	mov	cx,dx
    mov dx,17
    jc  error
	mov	dx,43
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(15*1024)/4
	xor	di,di
	mov	eax,es:[0]
	rep	stosd
	pop	es
	pop	di
    dec bp
    jne loop4

; resize 15K
    mov si,di
    sub si,(3*(2*12))+2
    mov bp,12

loop5:
    mov bx,ds:[si]
    sub si,2
    mov dx,15*1024
    xor cx,cx
    mov ax,0ff0dh
    int 31h
	mov	cx,dx
    mov dx,18
    jc  error
	mov	dx,44
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(15*1024)/4
	xor	di,di
	mov	eax,es:[0]
	rep	stosd
	pop	es
	pop	di
    dec bp
    jne loop5

; resize 63K
    mov si,di
    sub si,(4*(2*12))+2
    mov bp,12

loop6:
    mov bx,ds:[si]
    sub si,2
    mov dx,63*1024
    xor cx,cx
    mov ax,0ff0dh
    int 31h
	mov	cx,dx
    mov dx,19
    jc  error
	mov	dx,45
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(63*1024)/4
	xor	di,di
	mov	eax,es:[0]
	rep	stosd
	pop	es
	pop	di
    dec bp
    jne loop6

; resize 64K
    mov si,di
    sub si,(3*(2*12))+2
    mov bp,12

loop7:
    mov bx,ds:[si]
    sub si,2
    xor dx,dx
    mov cx,1
    mov ax,0ff0dh
    int 31h
    mov dx,20
    jc  error
	mov	dx,46
	movzx	ebx,bx
	lsl	eax,ebx
	inc	eax
	cmp	eax,10000h
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(64*1024)/4
	xor	di,di
	mov	eax,es:[0]
	rep	stosd
	pop	es
	pop	di
    dec bp
    jne loop7

; resize 1K
    mov si,di
    sub si,(4*(2*12))+2
    mov bp,12

loop8:
    mov bx,ds:[si]
    sub si,2
    mov dx,1*1024
    xor cx,cx
    mov ax,0ff0dh
    int 31h
	mov	cx,dx
    mov dx,21
    jc  error
	mov	dx,47
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(1*1024)/4
	xor	di,di
	mov	eax,es:[0]
	rep	stosd
	pop	es
	pop	di
    dec bp
    jne loop8

; resize same size
    mov si,di
    sub si,(3*(2*12))+2
    mov bp,12

loop9:
    mov bx,ds:[si]
    sub si,2
    xor dx,dx
    mov cx,1
    mov ax,0ff0dh
    int 31h
	mov	cx,dx
    mov dx,22
    jc  error
	mov	dx,48
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(64*1024)/4
	xor	di,di
	mov	eax,es:[0]
	rep	stosd
	pop	es
	pop	di
    dec bp
    jne loop9

; resize same size
    mov si,di
    sub si,(4*(2*12))+2
    mov bp,12

loop10:
    mov bx,ds:[si]
    sub si,2
    mov dx,1*1024
    xor cx,cx
    mov ax,0ff0dh
    int 31h
	mov	cx,dx
    mov dx,23
    jc  error
	mov	dx,49
	lsl	ax,bx
	inc	ax
	cmp	ax,cx
	jne	error
	push	di
	push	es
	mov	es,bx
	mov	cx,(1*1024)/4
	xor	di,di
	mov	eax,es:[0]
	rep	stosd
	pop	es
	pop	di
    dec bp
    jne loop10

   jmp NEAR PTR looper

fini:
	mov	bp,OFFSET DGROUP:storage
	mov	bx,CYCLECOUNT
	mov	counter,0
	mov	si,OFFSET DGROUP:CheckValues

checker:
	mov	ax,ds:[bp]
	add	bp,2
	lsl	cx,ax
	or	cx,cx
	je	checknext
	mov	es,ax
	shr	cx,2
	inc	cx
	xor	di,di
	mov	eax,ds:[si]
	repe	scasd
	jne	error
	add	si,4
	cmp	si,OFFSET DGROUP:CheckValues+(12*4)
	jb	checknext
	mov	si,OFFSET DGROUP:CheckValues

; bump cycle, check for end
checknext:
    inc counter
    mov ax,counter
    cmp ax,CYCLECOUNT+1
    jb  checker

    mov dx,OFFSET DGROUP:WorksText
    mov cx,7
    mov bx,1
    mov ah,40h
    int 21h
    mov ax,4c00h
    int 21h

error:
    mov si,counter
    mov cs:[0],si   ; force GPF, dx shows where, cx shows count

    mov dx,OFFSET DGROUP:ErrorText
    mov cx,7
    mov bx,1
    mov ah,40h
    int 21h
    mov ax,4c01h
    int 21h
end start
