; Test: IFDEF name followed later by `name MACRO` must not crash.
; Regression test for ddanila/open-watcom-v2#18: wasm dereferenced NULL in
; get_cond_state because ExpandMacro expanded `foo` as a macro invocation
; even when it was the operand of IFDEF/IFNDEF/ERRIFDEF/etc.
;
.8086
.model small
.code
IFDEF foo
ENDIF
foo macro
    nop
endm
    foo
end
