; Test: macro default arguments must capture constant values at macro
; definition time, even though formal parameter names on the MACRO line
; must not be expanded.
;
.386
.model small

defval = 4

use_default macro count:=defval
    mov ax, count
endm

defval = 1

.code
    use_default <>
end
