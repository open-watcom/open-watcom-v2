.386
.model small
.data
    a db "hello"
public foo
public bar
.code
    nop
    nop
    nop
    nop
    jmp $ + ( bar - foo )
    nop
    nop
    nop
    nop
    nop
    jmp $ + ( foo - bar )
    nop
foo:nop
    nop
bar:nop
end
