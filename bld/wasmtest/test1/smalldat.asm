; Test: wdis -a must not crash on small data-in-code sections.
; A single DB before code produces a 1-byte data region that
; triggered an unsigned underflow in tryDUP(), crashing wdis.
;
.8086
_TEXT SEGMENT BYTE PUBLIC 'CODE'
my_var DB 0
    mov al, my_var
_TEXT ENDS
END
