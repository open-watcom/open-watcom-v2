.386
.model small
.code
foo proc watcom_c, a:byte, b:word, cc:fword, d:dword
    mov bl, a
    mov bx, b
;    mov ebx, cc
    mov ecx, d
    ret
foo endp

bar proc watcom_c, b:word, cc:fword, d:word, e:dword
    mov bx, b
;    mov ebx, cc
    mov cx, d
    mov ebx, e
    ret
bar endp

sam proc watcom_c, cc:fword, d:word, e:fword
 ;   mov ebx, cc
    mov cx, d
    lea edx, e
    ret
sam endp

end
