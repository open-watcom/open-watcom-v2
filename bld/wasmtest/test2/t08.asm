pat segment 'data'
assume ds:pat
cde dw 10
abc dw 20 dup (0)
pat ends
assume cs:text
text segment 'code'
cat:mov ax,abc[8*8]
mov ax, [cde]+4
text ends
end
