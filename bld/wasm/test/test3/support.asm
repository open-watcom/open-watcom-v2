; support.asm - assembler support functions for DOS
; 
; This program written by Paul Edwards
; Released to the public domain

% .model memodel, c

assume cs:_TEXT, ds:DGROUP

_DATA   segment word public 'DATA'
_DATA   ends
_BSS    segment word public 'BSS'
_BSS    ends

_TEXT segment word public 'CODE'

public int86
int86 proc uses ax bx cx dx si di ds es, \
           intnum:word, regsin:ptr, regsout:ptr

if @DataSize
  lds si, regsin
else
  mov si, regsin
endif

mov ax, word ptr [si + 0]
mov bx, word ptr [si + 2]
mov cx, word ptr [si + 4]
mov dx, word ptr [si + 6]
mov di, word ptr [si + 10]
mov si, word ptr [si + 8]

push bp

cmp intnum, 08h
jne not8
int 08h
jmp fintry
not8:

cmp intnum, 09h
jne not9
int 09h
jmp fintry
not9:

cmp intnum, 010h
jne not10
int 010h
jmp fintry
not10:

cmp intnum, 013h
jne not13
int 013h
jmp fintry
not13:

cmp intnum, 015h
jne not15
clc  ; interrupt doesn't set it it seems
int 015h
jmp fintry
not15:

cmp intnum, 016h
jne not16
int 016h
jmp fintry
not16:

cmp intnum, 020h
jne not20
int 020h
jmp fintry
not20:

cmp intnum, 021h
jne not21
int 021h
jmp fintry
not21:

fintry:

pop bp
push si

if @DataSize
  lds si, regsout
else
  mov si, regsout
endif

mov [si + 0], ax
mov [si + 2], bx
mov [si + 4], cx
mov [si + 6], dx
mov [si + 10], di
pop ax ; actually si
mov [si + 8], ax
mov word ptr [si + 12], 0
jnc flagclear
mov word ptr [si + 12], 1
flagclear:
pushf
pop ax
mov word ptr [si + 14], ax

ret
int86 endp



public int86x
int86x proc uses ax bx cx dx si di ds es, \
            intnum:word, regsin:ptr, regsout:ptr, sregs:ptr

push ds; for restoration after interrupt

if @DataSize
  lds si, sregs
else
  mov si, sregs
endif

mov es, [si + 6]
push es ; new value for ds

mov es, [si + 0]

if @DataSize
  lds si, regsin
else
  mov si, regsin
endif

mov ax, word ptr [si + 0]
mov bx, word ptr [si + 2]
mov cx, word ptr [si + 4]
mov dx, word ptr [si + 6]
mov di, word ptr [si + 10]
mov si, word ptr [si + 8]

pop ds; load previously saved value for ds

cmp intnum, 08h
jne xnot8
int 08h
jmp xfintry
xnot8:

cmp intnum, 09h
jne xnot9
int 09h
jmp xfintry
xnot9:

cmp intnum, 010h
jne xnot10
int 010h
jmp xfintry
xnot10:

cmp intnum, 013h
jne xnot13
int 013h
jmp xfintry
xnot13:

cmp intnum, 015h
jne xnot15
clc ; interrupt doesn't set it it seems
int 015h
jmp xfintry
xnot15:

cmp intnum, 016h
jne xnot16
int 016h
jmp xfintry
xnot16:

cmp intnum, 021h
jne xnot21
int 021h
jmp xfintry
xnot21:

xfintry:

push es
push ds
push si
push ax
push bp

mov bp, sp
mov ax, [bp+10]; restore ds immediately, can't move without it
pop bp
mov ds, ax

if @DataSize
  lds si, regsout
else
  mov si, regsout
endif

pop ax
mov [si + 0], ax
mov [si + 2], bx
mov [si + 4], cx
mov [si + 6], dx
mov [si + 10], di
pop ax
mov [si + 8], ax ; si
mov word ptr [si + 12], 0
jnc xflagclear
mov word ptr [si + 12], 1
xflagclear:

if @DataSize
  lds si, sregs
else
  mov si, sregs
endif

pop ax
mov [si + 6], ax; restore ds
pop ax
mov [si + 0], ax ; restore es

pop ds  ; restore value saved over interrupt (but accessed directly already)
ret
int86x endp

_TEXT ends

end
