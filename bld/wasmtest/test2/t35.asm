.386

; test scaling factors

_data segment byte public USE32 'data'
foo db "this is a string"
_data ends

_text segment byte public USE32 'code'
assume cs:_text
assume es:_data

mov ebx,[eax+2]
mov ebx,[eax-2]
mov ebx,[eax*2]
mov ebx,[2*eax]
mov ebx,[eax*4]
mov ebx,[4*eax]
mov ebx,[eax*8]
mov ebx,[8*eax]
xor ax, ax
mov ebx,[eax*2+2]
mov ebx,[eax*(8-4)]
mov ebx,[eax*(8/4)]
mov ebx,[eax*2][edx]
mov ebx,[edx][eax*2]

mov ebx,[eax][edx*2]
mov ebx,[edx*2][eax]

_text ends
end
