assume ss:pat
flora group pat
assume cs:flora
don segment 'data'
assume ds:don
foo dw 1
poo dw 1
don ends
pat segment 'code'
mov ax,[poo][bx][1]
mov ax,[poo+bx+di]
start:mov ax,poo[bx][di]
mov ax,[bx][di]+poo
mov ax,poo[bx][di]+6
mov ax,[1][2]
mov ax,not 4
;mov ax,seg poo + offset foo
;mov ax,poo[bx-di]
mov ax,flora:(poo+1)
mov ax, poo[bx]
;mov ax,don:foo - don:poo
mov ax, poo[bx+di]
;mov ax, poo[bx[foo]]
;mov ax, (seg ss:foo)
;mov ax, seg offset foo
;mov ax, foo + bx
mov ax, (5+(3*2*(1+2)-3)*2)-(4*(2-1))
jmp start
pat ends
end
