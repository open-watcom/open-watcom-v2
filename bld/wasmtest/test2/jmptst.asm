.386
.MODEL small
.code
        jmp dword ptr [bx]+2
        xor ax, ax
foo:    nop
        nop
        nop
        nop
        jmp bar
        xor ax, ax
bar:    jmp foo+1
        xor ax, ax
        add eax, dword ptr foo+4
        add eax, dword ptr foo[4]
        add eax, dword ptr 4[foo]
        jmp late
        jmp far ptr late
        jmp short late
        jmp near ptr late
        jmp foo
        jmp far ptr foo
        jmp short foo
        jmp near ptr foo
late:
        call dword ptr [bx]+2
        xor ax, ax
foo2:   nop
        nop
        nop
        nop
        call bar2
        xor ax, ax
bar2:   call foo2+1
        xor ax, ax
        add eax, dword ptr foo2+4
        add eax, dword ptr foo2[4]
        add eax, dword ptr 4[foo2]
        call late2
        call far ptr late2
        call near ptr late2
        call foo2
        call far ptr foo2
        call foo2
        call near ptr foo2
late2:
end
