.386
.model small
.data

b LABEL DWORD
a dw 20 dup ( 1,2,3,4,5 )
.code

mov ax, a[2]
mov ax, a[4]
mov ax, a[5]
mov ax, a[6]
mov ax, a[13]

mov ebx, b[1]
mov ebx, b[2]
mov ebx, b[3]
mov ebx, b[7]
mov ebx, b[9]


end
