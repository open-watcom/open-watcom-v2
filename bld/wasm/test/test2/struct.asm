.386
.model small
.data
moose struct
    g db 1
    h db 0
moose ends
foo struct
    a db 0
    b dw 1
    d dd 2
    j moose <>
    e df 4
foo ends
    db 7 dup ( 0 )

    da_foo_struct foo < >
    db 20 dup ( 0 )

    da_other_foo_struct foo "7, 6"

    kyb foo { 0,19,19,<19>,19 }
    db 20 dup ( 0 )
.code
    xor ax, ax
    mov ebx, offset da_foo_struct
bar:    mov ax, [ebx].b
    mov ah, [ebx].a
    mov eax, [ebx].d
    mov ah, [ebx].j.g
    mov ah, [ebx].j.h


    mov da_foo_struct.a, bh
    mov da_foo_struct.b, bx
    mov da_foo_struct.d, ebx
    mov da_foo_struct.j.g, bh
    mov da_foo_struct.j.h, bl

    mov da_other_foo_struct.a, bl
    mov da_other_foo_struct.b, bx
    mov da_other_foo_struct.d, ebx
    mov da_other_foo_struct.j.g, bh
    mov da_other_foo_struct.j.h, bl

    mov word ptr kyb.a, bx
    mov kyb.b, bx
    mov word ptr kyb.d, bx
    mov word ptr kyb.e, bx
    mov word ptr kyb.j.g, bx
    mov word ptr kyb.j.h, bx
end
