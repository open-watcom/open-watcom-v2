;extrn  abc:word
assume ss:woo
woo segment para 'data'
abc dw ?
woo ends
foo segment para 'code'
mov     ds, cs:abc
mov     ds, word ptr cs:abc
mov     ds, word ptr cs:abc
foo ends
end
