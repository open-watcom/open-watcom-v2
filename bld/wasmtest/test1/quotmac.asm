; Test: a single- or double-quoted character argument passed to a
; macro must round-trip its delimiters when substituted into the body.
; Embedded delimiters must be doubled in MASM idiom (`'a''b'`, `"x""y"`),
; non-delimiter chars must pass through verbatim, and `<...>` substitution
; (used by STRUC.INC for raw-text insertion) must keep dropping its
; delimiters.
;
.8086
.model small
.code

m macro x
    DB x
endm

    m '/'           ; single quote, single char
    m "ab"          ; double quote, plain content
    m "x'y"         ; double quote with embedded `'` -- must NOT be escaped
    m 'a''b'        ; single quote with embedded `'` -- must round-trip the doubled escape
    m <99>          ; angle brackets -- raw text, no quoting in output
end
