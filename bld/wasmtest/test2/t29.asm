flora group text
text segment para public 'code'
assume cs:text
xor ax,ax
mov si,ax
mov 2[si],ax
mov [si+2],ax
text ends
end
