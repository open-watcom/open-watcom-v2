;pat segment para 'data' ??
pat segment para 'code'
assume es:pat       ;without this line we have an error
abc:mov ax,ax
don dw ?
pat ends
text segment para 'code'
mov ax,don
jmp don
ifdef errors
  jmp abc
endif
text ends

end
