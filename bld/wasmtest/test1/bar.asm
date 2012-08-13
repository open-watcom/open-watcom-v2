.386
.model small
.data
foo struc
    x db 0
    y dw 0
    z dd 0,2
foo ends
comment ~
    mov ax, bar.y
    mov ax, offset bar+y
    mov ax, offset bar.y
    mov ax, size bar.z
~
public a,b,d,e,f,g,h,i,j,k,p,q
    bar foo <>
a    dd bar.x
p    dd bar.y
q    dd bar.z
b    dd offset bar+y ; bug
d    dd offset bar.y
e    dd size bar.y
f    dd size bar.y
g    dd length bar.y
h    dd length bar.x
i    dd length bar.y
j    dd length bar.z
k    dd length bar.z+2
.code

end
