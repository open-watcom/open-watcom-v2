                name    test
_TEXT           segment public 'CODE'
                assume  cs:_TEXT

                public  fubar
                public  bunk
                public  sam
fubar   equ     0000h           ; pubdef
bunk    equ     1234h           ; pubdef
sam     equ     0               ; pubdef

                xor ax, ax
foo             proc far        ; pubdef
                public  foo
                ret
foo             endp

bar             proc near       ; static pubdef
                ret
bar             endp

_TEXT           ends
                end
