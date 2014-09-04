.model flat, stdcall

.586
.code

Start PROC

xor eax, eax
push hwndMain

ret

Start ENDP

Foo PROC hwndMain:DWORD

ret

Foo ENDP

END

