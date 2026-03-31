; Test: second EXTRN should preserve its spelling in case-insensitive mode
; when first EXTRN used different case.
; When assembled with -zcm=masm, the EXTDEF record must contain "ARG_BUF"
; (the last EXTRN spelling), not "arg_buf" (the first spelling).

.MODEL small

EXTRN arg_buf:BYTE
EXTRN ARG_BUF:BYTE

_TEXT SEGMENT PUBLIC BYTE
    mov si, offset ARG_BUF
_TEXT ENDS

END
