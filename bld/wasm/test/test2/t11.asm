flora group abcd,pat
assume ss:pat
assume ds:flora
pat segment 'data'
foo dw 1
pat ends
assume cs:abcd
abcd segment 'code'
mov ax,++(((1+1)+((-1*--2)))+1)+1-99
mov ax,ax
mov ax,pat:foo
abcd ends
end
