name fubar
.386
.model small
d = 0
foo macro bar
local a, b
    d = d + 2
    public a
    public b
    a: xor ax, bar
    b: xor bar, bx
        jmp b
        jmp a
    xor ax, d
endm
.code
    foo ax
    foo dx
    foo cx
end
