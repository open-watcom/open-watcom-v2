assume ds:pat
pat segment 'data'
extrn def:word
ddd dw 20
abc dw 20
pat ends
text segment 'code'
mov ax,(1 LT 6)
text ends
end
