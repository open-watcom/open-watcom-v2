.386
.model small
.data
    foo dd 1234h
.code

ifndef NO_ERRS_16
    ifndef NO_ERRS_32
        jmp byte ptr [bx]
        jmp qword ptr [bx]
        jmp tbyte ptr [bx]
    endif

    jmp near dword ptr [bx]
    jmp near dword ptr [bx]+2

    jmp near dword ptr foo
    jmp near dword ptr foo+2

    ifndef NO_ERRS_32
        jmpf word ptr [bx]
        jmpf word ptr [bx]+2
    endif

endif

    jmp dword ptr [bx]
    jmp dword ptr [bx]+2

    jmp far dword ptr [bx]
    jmp far dword ptr [bx]+2

    jmpf dword ptr [bx]
    jmpf dword ptr [bx]+2


    jmp dword ptr foo
    ;jmp dword ptr foo+2

    jmp far dword ptr foo
    ;jmp far dword ptr foo+2

    jmpf dword ptr foo
    ;jmpf dword ptr foo+2
end
