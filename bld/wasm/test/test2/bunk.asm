dgroup group _data
_data segment 'data'
    foo db 44 dup (1,2,3)
_data ends
_text segment 'code'
includelib thelib
include maaak.inc
    xor ax, ax
_text ends
end
