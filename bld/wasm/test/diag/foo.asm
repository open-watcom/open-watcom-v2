.386
.model small
.data
    x db 0
    y dw 0
    z dd 0
.code
    mov [ebx], 0
    mov [ebx], 0xffff
    mov [ebx], 0xffffffff
    mov x, 0
    mov y, 0
    mov z, 0
    mov [ebx], ah
    mov [ebx], ax
    mov [ebx], eax
end

