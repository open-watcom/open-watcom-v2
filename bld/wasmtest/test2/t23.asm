w equ word ptr
text2 segment byte 'code'
abcd proc far stdcall ,a:word, b:word, cc:word
assume cs:text2
local loc:word
local locc:word
add ax, word ptr [bp-2]
add ax, word ptr locc
add ax, loc + 9
add ax, word ptr B
add ax, B
mov ax,word ptr cc
cmp w loc,'ZM'
cmp w[locc],"MZ"
ret 5
abcd endp
mov ax,ax
text2 ends

assume cs:text2
text segment 'code'
call abcd

text ends
end
