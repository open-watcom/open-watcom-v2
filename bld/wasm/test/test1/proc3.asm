.386
.model small
.code
foo proc watcom_c, a:word, b:word, cc:word, d:word, e:word, f:word
    mov ax, a
    mov ax, b
    mov ax, cc
    mov ax, d
    mov ax, e
    mov ax, f
    ret
foo endp

sam proc c, a:word, b:word, cc:word, d:word, e:word, f:word
    mov ax, a
    mov ax, b
    mov ax, cc
    mov ax, d
    mov ax, e
    mov ax, f
    ret
sam endp

bar proc watcom_c, a:byte, b:byte, cc:byte, d:byte, e:byte, f:byte
    mov ah, a
    mov ah, b
    mov ah, cc
    mov ah, d
    mov ah, e
    mov ah, f
    ret
bar endp

ifdef __386__
kyb proc watcom_c, a:dword, b:dword, cc:dword, d:dword, e:dword, f:dword
    mov esi, a
    mov esi, b
    mov esi, cc
    mov esi, d
    mov esi, e
    mov esi, f
    ret
kyb endp
call kyb
endif

mov ax, 1
mov bx, 2
call foo
call bar
call sam

end
