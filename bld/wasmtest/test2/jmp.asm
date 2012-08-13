.386
.MODEL small
.data

extern foo:fword
extern a:byte
extern b:word
extern d:dword
extern e:qword
extern f:tbyte
.code

jmp bx
jmp ebx
if 0
    jmp [bx]
    jmp [ebx]
endif

jmp word ptr [bx]
jmp dword ptr [bx]
jmp fword ptr [bx]

jmp word ptr [ebx]
jmp dword ptr [ebx]
jmp fword ptr [ebx]
if 0
    jmp foo
    jmp [foo]
    jmp [foo+2]
    jmp [foo]+2
endif

jmp word ptr [foo+2]
jmp dword ptr [foo+3]
jmp fword ptr [foo+4]

jmp word ptr [foo]
jmp dword ptr [foo]
    jmp fword ptr [foo]

    jmp [b]
    jmp [d]
end
