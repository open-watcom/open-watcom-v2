.386
.model small
.data
    foo dd 1234h
.code
        mov word ptr foo[0], offset bar
        mov word ptr foo[2], seg bar
bar:    xor ax, ax
        mov word ptr bar, offset bar
        mov word ptr bar[2], seg bar

        mov ax, word ptr [bx]

        jmp bar
        jmp bx

        jmp word ptr [bx]
        jmp word ptr [bx]+2
        jmp word ptr 2h[bx]

        jmp dword ptr [bx]
        jmp dword ptr [bx]+2
        jmp dword ptr 2h[bx]

        jmp fword ptr [bx]
        jmp fword ptr [bx]+2
        jmp fword ptr 2h[bx]

        jmp word ptr [ebx]
        jmp word ptr [ebx]+2
        jmp word ptr 2h[ebx]

        jmp dword ptr [ebx]
        jmp dword ptr [ebx]+2
        jmp dword ptr 2h[ebx]

        jmp fword ptr [ebx]
        jmp fword ptr [ebx]+2
        jmp fword ptr 2h[ebx]

        jmp far ptr bar
        jmp far bar
;       jmpf bar
        jmpf bx

        jmpf word ptr [bx]
        jmpf word ptr [bx]+2
        jmpf word ptr 2h[bx]

        jmpf dword ptr [bx]
        jmpf dword ptr [bx]+2
        jmpf dword ptr 2h[bx]

        jmpf fword ptr [bx]
        jmpf fword ptr [bx]+2
        jmpf fword ptr 2h[bx]

        jmpf word ptr [ebx]
        jmpf word ptr [ebx]+2
        jmpf word ptr 2h[ebx]

        jmpf dword ptr [ebx]
        jmpf dword ptr [ebx]+2
        jmpf dword ptr 2h[ebx]

        jmpf fword ptr [ebx]
        jmpf fword ptr [ebx]+2
        jmpf fword ptr 2h[ebx]
end
