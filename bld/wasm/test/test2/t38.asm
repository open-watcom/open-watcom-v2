.386

; test jumps

_data segment byte public use32 'code'
sam: xor ax, ax
     jmp far ptr foo

_data ends

_text segment byte public use32 'code'
        assume cs:_text, ds:_data, ss:_data

        xor ax, ax
start:  xor ax, ax
dude:   xor ax, ax
        jmp far ptr sam
        mov ax, seg foo
        mov ax, offset foo
        mov ax, seg sam
        mov ax, offset sam
foo:    and ax,ax
        ;jne foo
        jmp bar
        jmp far ptr sam
        xor bx, bx
bar:    nop
        ;mov ax, foo
        jmp ax
        jmp far ptr bar
        jmp word ptr es:[bx+si]
        jmp word ptr es:[si]
        jmp word ptr cs:[si]
        jmp word ptr ds:[si]
        jmp dword ptr es:[bx+si]
        jmp dword ptr es:[si]
        jmp dword ptr cs:[si]
        jmp dword ptr ds:[si]
        shl bx,1
        jmp snert
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
        mov ax,12345
snert:  xor ax,ax
        jmp foo
        xor ax, ax
        jmp far ptr dude
_text ends

end start
