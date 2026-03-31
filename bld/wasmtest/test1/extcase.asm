; Test: EXTERNDEF should preserve its spelling in case-insensitive mode
; when label was defined with different case.
; When assembled with -zcm=masm, the PUBDEF record must contain "ARG_BUF"
; (the EXTERNDEF spelling), not "arg_buf" (the definition spelling).

.MODEL small

DATA SEGMENT PUBLIC BYTE
arg_buf db 80 dup(?)
DATA ENDS

EXTERNDEF ARG_BUF:BYTE

END
