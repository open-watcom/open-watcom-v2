.386
.MODEL small

.code
    xor ax, ax
    bar:
        xor ax, ax
    mov     eax, offset bar
    push    offset bar
end
