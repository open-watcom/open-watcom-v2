.model small
.code
    jmp foo
    jmp bar
    db 62H dup ( 0 )
    foo: xor ax, ax
    db 62H dup ( 0 )
    bar: xor ax, ax
    db 62H dup ( 0 )
    jmp foo
    db 62H dup ( 0 )
    jmp bar
end
