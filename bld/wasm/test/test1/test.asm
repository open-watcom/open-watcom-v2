.386
.model small
.data
    a db 0,1,2
    b dw 4 dup ( 1, 2 )
.code
    mov ax, size a
    mov ax, length a
    mov bx, size b
    mov bx, length b
end
