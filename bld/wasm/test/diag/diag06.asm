.386

foo struc
    x db 0
    y dw 0
    z dd 0,2
foo ends

_TEXT16 segment USE16 'CODE'
bar16:
    mov ax, size bar16
    mov ax, sizeof bar16
    mov bx, length bar16
    mov bx, lengthof bar16
    mov cx, length foo
    mov cx, lengthof foo
_TEXT16 ends

_TEXT32 segment USE32 'CODE'
bar32:
    mov ax, size bar32
    mov ax, sizeof bar32
    mov bx, length bar32
    mov bx, lengthof bar32
    mov cx, length foo
    mov cx, lengthof foo
_TEXT32 ends

end
