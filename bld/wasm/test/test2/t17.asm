flora group text

pat segment para 'code'
assume ss:pat
abc dw ?
pat ends

text segment para 'code'
assume ds:flora
mov ax,abc
text ends

end
