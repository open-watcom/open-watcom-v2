.model small
foo struc
    x db 0
    y dw 0
    z dd 0,2
foo ends
.code
    mov ax, size a
    mov ax, sizeof a
    mov ax, size b
    mov ax, sizeof b
    mov ax, size d
    mov ax, sizeof d
    mov ax, size e
    mov ax, sizeof e
    mov ax, size f
    mov ax, sizeof f
    mov ax, size g
    mov ax, sizeof g
    mov ax, size h
    mov ax, sizeof h
    mov ax, size j
    mov ax, sizeof j
    mov ax, size j.x
    mov ax, sizeof j.x
    mov ax, size j.y
    mov ax, sizeof j.y
    mov ax, size j.z
    mov ax, sizeof j.z
    mov ax, size j.z+2
    mov ax, sizeof j.z+2

    mov bx, length a
    mov bx, lengthof a
    mov bx, length b
    mov bx, lengthof b
    mov bx, length g
    mov bx, lengthof g
    mov bx, length h
    mov bx, lengthof h
    mov bx, length j
    mov bx, lengthof j
    mov bx, length j.z
    mov bx, lengthof j.z

    mov cx, size q
    mov cx, sizeof q
    mov cx, length q
    mov cx, lengthof q

.data
    a db 0
    b db 0,0
    d dw 0
    e dw 0,0
    f dd 0
    g dd 2 dup ( 0 )
    h dd 4 dup ( 0 )
    j foo 2 dup( <> )

    q dd 2 dup ( 0 ), 3 dup ( 4, 2 )

end
