.386
.model small
.code
    jmp foo
    jmp near ptr foo
    jmp far ptr foo

    call foo
    call near ptr foo
    call far ptr foo

    xor ax, ax
foo:    xor ax, ax
    xor ax, ax

    jmp foo
    jmp near ptr foo
    jmp far ptr foo

    call foo
    call near ptr foo
    call far ptr foo

end

