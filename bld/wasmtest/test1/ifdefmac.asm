; Test: directives taking a bare macro/symbol name as operand must not
; crash or silently misbehave when that name is also later defined as a
; macro.
;
; Regression test for ddanila/open-watcom-v2#18. On the second assembly
; pass ExpandMacro expanded the operand of IFDEF / IFNDEF / ELSEIFDEF /
; ELSEIFNDEF / .ERRDEF / .ERRNDEF / ERRIFDEF / ERRIFNDEF / PURGE even
; though those directives take the bare identifier as their argument,
; mangling the token buffer and causing a NULL deref in get_cond_state.
;
; T_PURGE is covered by test1/purge.asm.
; T_DOT_ERRDEF / T_ERRIFDEF share the same switch case as the tested
; directives and cannot be exercised cleanly (they fire whenever their
; argument is defined — which is exactly the repro condition).
; T_ERRIFDEF / T_ERRIFNDEF are TASM-only aliases not recognised in the
; default WASM mode and share the same switch case as the .ERR* forms.
;
.8086
.model small
.code

foo macro
    nop
endm

; 1. IFDEF foo — true, empty body.
IFDEF foo
ENDIF

; 2. IFNDEF foo — false, empty body.
IFNDEF foo
ENDIF

; 3. ELSEIFDEF foo — reached after outer IFDEF fails; takes this branch.
IFDEF never_defined_xyz_1
ELSEIFDEF foo
ENDIF

; 4. ELSEIFNDEF foo — never taken since foo is defined.
IFDEF never_defined_xyz_2
ELSEIFNDEF foo
ENDIF

; 5. .ERRNDEF foo — doesn't fire because foo is defined.
.ERRNDEF foo

    foo
end
