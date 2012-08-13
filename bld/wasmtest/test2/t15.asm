extrn start:near
first = 2
second = 4
third = first + second
forth = third * third
fifth = ( third ) * ( third )
sixth = first * fifth * third
first = second

;assume cs:flora
;pat group flora
flora segment 'code'
mov ax, forth
mov ax, fifth
mov ax, sixth
;fifth = fifth + fifth
mov ax, fifth
mov ax, first
flora ends
end start
