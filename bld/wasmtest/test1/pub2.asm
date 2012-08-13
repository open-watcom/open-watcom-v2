                name    test
_TEXT           segment public 'CODE'
                assume  cs:_TEXT

                public  fubar
                public  bunk
fubar   equ     0000h           ; pubdef
bunk    equ     1234h           ; pubdef

                public  foo
foo             proc far        ; pubdef
                ret
foo             endp

bar             proc near       ; static pubdef
                ret
bar             endp

_TEXT           ends

_DATA           segment public 'DATA'
                public hi
                hi db 12

                public  fubar2
                public  bunk2
                public  sam2

                fubar2 db 12
                bunk2 db 12
                sam2 db 12

                public bye
                bye db 12
_DATA ends
_TEXT           segment public 'CODE'
                public kyb
                kyb: xor ax, ax
_TEXT ends
                end
