; Test: PUBLIC directive should preserve its own spelling of the symbol name
; in case-insensitive mode (-zcm=masm).
; When assembled with -zcm=masm, the PUBDEF record must contain "ARG_BUF"
; (the PUBLIC spelling), not "arg_buf" (the definition spelling).

.MODEL small

CONST SEGMENT PUBLIC BYTE
ptr1 dd arg_buf
CONST ENDS

DATA SEGMENT PUBLIC BYTE
PUBLIC ARG_BUF
arg_buf db 80 dup(?)
DATA ENDS

END
