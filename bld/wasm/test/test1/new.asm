.386
.model small
.code
    xor ax, ax
    foo: xor bx, bx
    org foo + 8
    xor cx, cx
    org $ + 4
    xor dx, dx
    org $ - 5
    nop
    org 20
    jmp foo
    end
