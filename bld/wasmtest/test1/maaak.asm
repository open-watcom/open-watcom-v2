.386
.model small
.data
foo struc
    a db 0,1,?
    b dw 3 dup ( 4, 2 )
    d dd 2 dup ( ? ), 5 , 3 dup( 2 dup ( 1 ) )
    e db 0
foo ends
.code
    mov ah, [bx].a
    mov ax, [bx].b
    mov eax, [bx].d
bar:    mov ah, [bx].e

    mov ah, bar.a
comment ~
    mov ax, bar.b
    mov eax, bar.d
    mov ah, bar.e
~
end
