assume ss:pat
flora group pat
assume cs:flora
pat segment 'code'
foo dw 1
poo dw 1
mov ax,1+[foo][bx][di[(2)]]
mov ax,[1][2][4]
mov ax,(1*2+((3+2)-5+2*3*(1+1))*4+-1)-3
mov ax,[1[2[4]]]
mov ax,1+[2][3][4[(5+6)]]
mov ax,foo+[bx[di]]
mov ax,foo+2
;jmp pat:foo
pat ends
end
