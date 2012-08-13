.386
.model small
foo equ "hello"
bar equ "hello"
.code

kyb macro foo, bar
    ifb foo
        mov ax, -1
    endif
    ifnb bar
        mov ax, -2
    endif
    ifdef foo
        mov ax, 1
    endif
    ifdef bar
        mov ax, 2
    endif

    ifdif <foo>, <bar>
        mov ax, 3
    else
        mov ax, 4
    endif

    ifdif %<foo>, %<bar>
        mov ax, 3
    else
        mov ax, 4
    endif

    ifdif foo, bar
        mov ax, 5
    else
        mov ax, 6
    endif
endm

kyb <foo>, <bar>
end
