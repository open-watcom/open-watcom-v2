; Test: EXTRN should preserve its spelling in case-insensitive mode
; when symbol was previously forward-referenced with different case.
; When assembled with -zcm=masm, the EXTDEF record must contain "ARG_BUF"
; (the EXTRN spelling), not "arg_buf" (the reference spelling).

.MODEL small

_TEXT SEGMENT PUBLIC BYTE
    mov si, offset arg_buf
_TEXT ENDS

EXTRN ARG_BUF:BYTE

END
