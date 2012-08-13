flora group text
text segment para public 'code'
mov ax,ax
abc:mov dx,dx
text ends
donna segment page public 'code'
extrn woo:word
mov dx,dx
foo dw ?
mov ax,offset abc
donna ends
end
