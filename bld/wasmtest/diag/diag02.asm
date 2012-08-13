.model flat, stdcall

.586
.code

Start PROC

mov eax, 0
push hwndMain

ret

Start ENDP

Foo PROC hwndMain:DWORD

ret

Foo ENDP

END

