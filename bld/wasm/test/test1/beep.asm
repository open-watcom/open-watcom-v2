.model tiny
beep macro
    mov ah,2
    mov dl,7
    int 21h
endm
.code
da_start:
    beep
    beep
    ret
end da_start
