.MODEL small
.data
    savess          dw      0
    savebp          dw      0
.code

    mov     CS:savess,SS                ; save SS and BP since EXEC
    mov     CS:savebp,BP                ; . kills all regs except CS:IP

    mov     BP,CS:savebp                ; restore BP
    mov     SS,CS:savess                ; and SS

end
