.model small
.code
    mov ax, word ptr [bx].a+1
    mov ax, word ptr [bx].a+2
    mov ax, word ptr [bx].a+3

    mov word ptr [bx]+1, ax
    mov word ptr [bx]+1, ax
foo struc
    a db 0, 1, 2, 3
foo ends
end

