text1 segment 'data'
assume ds:text1
x dw ?
text1 ends
text2 segment 'code'
mov word ptr x-2, ax
mov word ptr [x-2], ax
text2 ends
end
