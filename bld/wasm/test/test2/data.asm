.386
.model small
.data

filler db   "some stuff", 0
a   dw 0
;b   dw a
d   dd a

;e   dw offset a
f   dd offset a

;g   dw foo
;h   dw offset foo

i   dd foo
j   dd offset foo

;k   dw m
;l   dw offset n

m   dd o
n   dd offset p

o   dw seg a
p   dw seg foo

q   dd dgroup:p
r   dd _TEXT:foo

s   dw dgroup
u   dw seg dgroup

.code
        xor ax, ax
foo:    mov ax, a
        mov ax, a
        mov eax, d
        mov ax, dgroup
        mov ax, seg dgroup

;       mov eax, dgroup
        mov ax, seg dgroup
        xor ax, ax
end
