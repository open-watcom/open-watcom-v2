;.386
;__386__ = 1
.model small
.code

bar proc c, a:byte, b:byte, cc:byte, d:byte, e:byte, f:byte
    mov ah, a
    mov ah, b
    mov ah, cc
    mov ah, d
    mov ah, e
    mov ah, f
    ret
bar endp

sam proc c, a:word, b:word, cc:word, d:word, e:word, f:word
    mov ax, a
    mov ax, b
    mov ax, cc
    mov ax, d
    mov ax, e
    mov ax, f
    ret
sam endp

;ifdef __386__
kyb proc c, a:dword, b:dword, cc:dword, d:dword, e:dword, f:dword
    lea dx, a
    lea dx, b
    lea dx, cc
    lea dx, d
    lea dx, e
    lea dx, f
    ret
kyb endp
;endif

end
