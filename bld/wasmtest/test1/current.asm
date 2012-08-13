.386
.387
.model small
.data
    public foo
    public kyb
    public bar
    public sam
    public a
    public b
    public d

    foo dd $
    kyb dd $
    bar dd $ + 8
        dd $ - 4
        dd foo
        dd foo +4
        dd bar -4
    db 2,4,6,7,8
.code
jmp $
jmp $ + 2
jmp $ + 4
jmp $ - 4
jmp foo + 4
jmp foo + 8
nop
nop
nop
sam: lea ax, $
     lea ax, sam
a: jmp sam
b: jmp a
d: jmp b

mov eax, [foo]
mov eax, [kyb]
lea eax, [bar]
end


