;.386
.model small
.code
foo proc c, a:fword, b:qword, cc:tbyte, d:dword
    lea bx, a
    lea bx, b
    lea bx, cc
    lea cx, d
    ret
foo endp
end
