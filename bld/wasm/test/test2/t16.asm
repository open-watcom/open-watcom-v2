extrn outside:word
flora group text, donna
assume cs:flora
donna segment 'data'
;bogus dd 01
extrn cde:word
abc dw 01
donna ends
text segment 'code'
start:mov ax,abc
mov ax,cde[2][3[2]]
mov ax,outside
text ends
end start+2
