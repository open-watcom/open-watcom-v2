.386
.MODEL small
    foo:
.code
    xor ax, ax
    bar:
        xor ax, ax
    mov     eax, offset bar
    push    offset bar
end
