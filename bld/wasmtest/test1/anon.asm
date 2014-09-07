.model small
.code
    nop
    jmp @F
    nop
    @@: mov ax, 0
    nop
    jmp @B
    nop
    nop
    jmp @F
    nop
    @@: mov ax, 1
    nop
    jmp @B
    nop
    nop
    jmp @F
    jmp @B
    jmp @F
    jmp @B
    nop
    @@: mov ax, 2
    nop
    jmp @B
    nop
end
