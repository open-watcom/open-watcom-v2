.386
.model small
.data
bar struc
    g db 8
    h db 9
bar ends

foo struc
    a db 0,1,2,3
    b dw 1
    d dd 2
    e bar <>
    f df 4
foo ends

    va db 0
    vb dw 1
    vd dd 2
    ve bar <>
    vf df 4

    kyb foo <>
.code
    mov kyb.a, bl
    mov kyb.b, bx
    mov kyb.d, ebx
    mov kyb.e.g, bl
    mov kyb.e.h, bh
;    call kyb.f

    mov kyb.a, 7
    mov kyb.b, 7
    mov kyb.d, 7
    mov kyb.e.g, 7
    mov kyb.e.h, 7
;    mov kyb.f, 7

    mov va, 7
    mov vb, 7
    mov vd, 7
    mov ve.g, 7
    mov ve.h, 7
end
