foo segment 'code'
don dw ?
jmp don
foo ends
woo segment 'code'
abc:
jmp abc
woo ends
end
