extrn cat:word
foo segment para 'code'
assume ds:foo
abc dw ?
pat:
foo ends

poo segment para 'code'
mov ax, abc
mov ax,cat
poo ends

end
