flora group text
text segment para public 'code'
assume cs:text
mov ax,ax
loo: mov dx,dx
text ends
assume cs:donna
donna segment page public 'data'
extrn woo:word
mov dx,dx
foo dw ?
mov ax,offset loo
donna ends
end
