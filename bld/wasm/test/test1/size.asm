.model small
foo struc
    x db 0
    y dw 0
    z dd 0,2
foo ends
.data
    a db 0
    b db 0,0
    d dw 0
    e dw 0,0
    f dd 0
    g dd 2 dup ( 0 )
    h dd 4 dup ( 0 )
    j foo <>

.code
    mov ax, sizeof a
    mov ax, sizeof b
    mov ax, sizeof d
    mov ax, sizeof e
    mov ax, sizeof f
    mov ax, sizeof g
    mov ax, sizeof h
    mov ax, sizeof foo
    mov ax, sizeof j
    mov ax, sizeof j.x
    mov ax, sizeof j.y
    mov ax, sizeof j.z
    mov ax, sizeof j.z+2
end
