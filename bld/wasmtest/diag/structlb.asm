.model tiny

.code

S STRUC
    first   db      ?
    marker  label   byte
    second  dw      ?
S ENDS

anon    S       <1,2>

end
