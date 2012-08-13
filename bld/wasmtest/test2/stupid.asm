.286
.model small
.code
foo: nop
sam: jmp bar
xor ax, ax
bar: jmp foo
lea bx, foo
end
