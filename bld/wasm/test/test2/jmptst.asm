.386
.MODEL small
.code
        jmp dword ptr [bx]+2
        xor ax, ax
foo:    jmp bar
        xor ax, ax
bar:    jmp foo+1
        xor ax, ax
        add eax, dword ptr foo+4
        add eax, dword ptr foo[4]
        add eax, dword ptr 4[foo]
        jmp late
;    jmp far late
        jmp short late
;    jmp near late

;    jmp far ptr late       ; this doesnt work
        jmp short ptr late
    jmp near ptr late
        jmp foo
        jmp far foo     ; fails under masm
        jmp short foo
        jmp near foo    ; fails under masm
late:
;   now calls
        call dword ptr [bx]+2
        xor ax, ax
foo2:   call bar2
        xor ax, ax
bar2:   call foo2+1
        xor ax, ax
        add eax, dword ptr foo2+4
        add eax, dword ptr foo2[4]
        add eax, dword ptr 4[foo2]
        call late2
;    call far late2
        call short late2
;    call near late2

;    call far ptr late2     ; this doesnt work
        call short ptr late2
    call near ptr late2
        call foo2
        call far foo2   ; fails under masm
        call short foo2
        call near foo2  ; fails under masm
late2:
end
