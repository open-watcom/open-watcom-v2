                .386
code            segment 'CODE'
assume          cs:code, ds:code
                add     al, '@' - '9'
                add     al, '@' - 4
                mov ax, '01'
                mov eax, 'kybo'
code            ends
                end
