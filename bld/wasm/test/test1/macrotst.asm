.386
.model small
foo macro bar
    mov ax, 9 * bar / 2
endm
.code
    foo 4+4
    foo %4+4
    end


