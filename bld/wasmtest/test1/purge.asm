; Test: PURGE directive removes a macro so it can be redefined.
; MASM 5.x supports PURGE and silent macro redefinition.
;
.8086
.model small
.code
myfoo macro
    nop
endm
    PURGE myfoo
myfoo macro
    nop
    nop
endm
    myfoo
end
