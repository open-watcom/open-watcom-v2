.386
.model small
;extrn foo:near
.code
    call near ptr foo
    call far ptr foo
    jmp foo
    call foo
    jmp foo
    jmp near ptr foo
    jmp far ptr foo
    foo: xor ax, ax
end

