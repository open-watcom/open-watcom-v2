; Test: &macro/&endm keyword escaping for nested macro definitions.
; MASM 5.x supports & prefix to escape macro/endm keywords,
; enabling macro-generating macros (cmacros.inc pattern).
;
.8086
.model small
.code
defop macro name
op_&name &macro
    nop
&endm
endm
    defop foo
    defop bar
    op_foo
    op_bar
end
