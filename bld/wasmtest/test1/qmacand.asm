; Regression test: `&p` inside a body string.
; MASM/TASM `&` is a text-substitution operator. Used inside "..." it
; splices the parameter's raw text into the surrounding string, so the
; arg's outer quotes (if any) must be dropped. `m "C"` with body
; `extrn "&p1",...` (the mdef.inc idiom) must produce `extrn "C",...`,
; not `extrn ""C"",...`.
;
.8086
.model small
.code

mq macro p
    DB "&p"
endm

    mq Y            ; bare ident   -> "Y"  -> 59H
    mq "Z"          ; double-quote -> "Z"  -> 5aH (regression case)
    mq 'W'          ; single-quote -> "W"  -> 57H
    mq <V>          ; angle-text   -> "V"  -> 56H

mp macro p
    DB "x&p"
endm

    mp "ab"         ; literal + &p inside "..." -> "xab" -> 78H, 61H, 62H
    mp 'c'          ;                            -> "xc"  -> 78H, 63H
end
