.model small
.data

foo dw 1 or 4
bar dw 1 and 4
sam dw 1 xor 4

.code

xor ax, foo
xor ax, bar
xor ax, sam

end
