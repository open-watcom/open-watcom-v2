.386
.MODEL small
.data

extern a:byte
extern b:word
extern d:dword
extern f:qword
extern g:tbyte
extern foo:fword

a2      byte    12
b2      word    1234
f2      qword   1234
g2      tbyte   1234

.code
    jmp fword ptr [ebx]
    jmp foo
    jmp fword ptr foo

comment #
    jmp [foo]
    jmp [foo+2]
    jmp [foo]+2
    jmp fword ptr [foo+4]
    jmp fword ptr [foo]
        jmp a
    jmp b
    jmp d
        jmp f
        jmp g

    jmp a2
    jmp b2
    jmp f2
    jmp g2
    #
end
