; <label> LABEL NEAR is a valid way of creating a label -- Anthony
;
.386p
                NAME    fib
_TEXT           SEGMENT BYTE PUBLIC USE32 'CODE'
                ASSUME  CS:_TEXT
                PUBLIC  `W?fib$n(i)i`
`W?fib$n(i)i`:  push    ebx
                push    edx
                mov     edx,eax
                cmp     eax,00000001H
                je      short bottom
                dec     eax
                call    near ptr `W?fib$n(i)i`
                mov     ebx,eax
                lea     eax,-2H[edx]
                call    near ptr `W?fib$n(i)i`
                lea     edx,[ebx+eax]
bottom          label   near
                mov     eax,edx
                pop     edx
                pop     ebx
                ret
_TEXT           ENDS

                END
