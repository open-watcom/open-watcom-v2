.model small
.code
bar macro arg
    db arg
    if arg eq 3
        .err
    endif
endm
foo macro
    bar 1
    bar 2
    bar 3
    bar 4
    bar 5
    bar 6
    bar 7
    bar 8
    bar 9
    bar 0
endm
foo
end
