; Test: a macro invocation ending with a trailing comma must leave the
; trailing positional parameter blank without consuming the next source
; line. This is the documented MASM/TASM behavior; wasm previously
; treated trailing comma as a line-continuation in all modes (a WATCOM
; extension), silently absorbing the following directive (typically END)
; and producing E249 "End directive required at end of file".
;
; Cross-checked against:
;   * Microsoft Macro Assembler 5.10 — accepts cleanly, OBJ produced.
;   * Turbo Assembler 4.1            — accepts cleanly, OBJ produced.
;
; Wired into makefile with `asm_flags_trailcm = -zcm=masm`. Default
; WATCOM mode preserves the legacy comma-continuation extension.
;
.8086
.model small
.code

mymacro macro a, b
    nop
endm

    mymacro 1,
end
