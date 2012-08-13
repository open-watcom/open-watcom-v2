.386
.model small
.data
a dd 1

foo struc
    e   db "hello",0
    x   dd 1
    m   dd a
    b   dd offset a
    n   dd size a
    o   dd length a
        db "unnamed"

foo ends

bar foo <>

.code

end
