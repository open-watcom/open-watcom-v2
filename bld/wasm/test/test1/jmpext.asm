.model small
.data
    kyb equ 1
.code
bar:jne foo + 4 / 2 - 1
    ja foo + 4 / 2 - 1
    jae foo + 4 / 2 - 1
    jb foo + 4 / 2 - 1
    jbe foo + 4 / 2 - 1
    jc foo + 4 / 2 - 1
    je foo + 4 / 2 - 1
;    jcxz foo + 4 / 2 - 1
;    jecxz foo + 4 / 2 - 1
    jg foo + 4 / 2 - 1
    jge foo + 4 / 2 - 1
    jl foo + 4 / 2 - 1
    jle foo + 4 / 2 - 1
    jmp foo + 4 / 2 - 1
    jmpf foo + 4 / 2 - 1
    jna foo + 4 / 2 - 1
    jnae foo + 4 / 2 - 1
    jnb foo + 4 / 2 - 1
    jnbe foo + 4 / 2 - 1
    jnc foo + 4 / 2 - 1
    jne foo + 4 / 2 - 1
    jng foo + 4 / 2 - 1
    jnge foo + 4 / 2 - 1
    jnl foo + 4 / 2 - 1
    jnle foo + 4 / 2 - 1
    jno foo + 4 / 2 - 1
    jnp foo + 4 / 2 - 1
    jns foo + 4 / 2 - 1
    jnz foo + 4 / 2 - 1
    jo foo + 4 / 2 - 1
    jp foo + 4 / 2 - 1
    jpe foo + 4 / 2 - 1
    jpo foo + 4 / 2 - 1
    js foo + 4 / 2 - 1
    jz foo + 4 / 2 - 1
    db 256 dup ( 0 )
    nop
    nop
foo:nop
    nop
    jmp bar
end
