.model small
.code

foo:    jmp bar
        inc ax
        mov moose, bx
bar:    jmp foo
moose   word 23

end
