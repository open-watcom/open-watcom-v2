.MODEL small
bar = 12
foo equ 3 + 4 * 5 - 4 / 2
.code

moose macro v1, v2, v3, v4, v5, v6
    mov ax, &v1&        ; comment
    mov ax, &v1
    mov ax, v1&
    mov ax, v1

    mov bx, v2

    mov cx, v3
    mov dx, v4

    mov si, v5
    mov di, v6
endm

moose 2+3, ax, foo, %foo, 2, 3
moose %2+3, ax, bx, 1, 2, 3
moose bar, 1, 2, 3, 4, 5

end
