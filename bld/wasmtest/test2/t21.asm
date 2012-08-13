foo segment para 'data'
assume ds:foo
abc dw ?
pat:
foo ends

poo segment para 'code'
mov ax, abc
poo ends

end
