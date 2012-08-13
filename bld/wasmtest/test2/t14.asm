don segment 'data'
foo db 01
don ends
assume ds:don
pat segment 'code'
mov ax, word ptr foo+2
pat ends
end
