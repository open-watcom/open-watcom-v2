.586
.model small
.data
    bar db "hi"
    foo qword 0xdeadbeef
.code
    cmpxchg8b foo
    cpuid
    rdtsc
    rdmsr
    wrmsr
    rsm
    mov CR4, eax
    mov eax, CR4
    end
