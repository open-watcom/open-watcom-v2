.model small
.code
    a equ b
    d equ e
    extern a:near
    extern "C", d:near
    extern "C", bar:dword

end
