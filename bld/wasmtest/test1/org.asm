.386
.model small
.code
    test ax, ax
    je foo
    org 0A0h
    nop
    nop
    nop
    nop
    foo: xor ax, ax
    nop
    nop
    nop
    nop
    test ax, ax
    je bar
    org 0c0h
    nop
    nop
    nop
    nop
    bar: xor ax, ax
end
