.386
.model small
extern foo:near
.code
    call foo
    call 4000h
    dw 0
    dw 0
    call cs:4000h
    dw 0
    nop
    call $ + 4000h
    nop
    nop
    call dword ptr 4000h
    nop
    nop
    call dword 4000h
    nop
    nop
    call word ptr 4000h
    nop
    nop
    call word 4000h
    nop
    nop
    call near 4000h
    nop
end
