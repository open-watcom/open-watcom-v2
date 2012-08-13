.386

_TEXT32 SEGMENT USE32 'CODE'

public myFunc32

myFunc32 proc near
    LOCAL  x1:WORD, x2[3]:BYTE, x3:QWORD, x4:BYTE, x5:DWORD, x6:BYTE, x7:FWORD, x8:BYTE

    mov x1, 05656h
    mov x2, 5
;    mov x3, 5
    mov x4, 5
    mov x5, 05h
    mov x6, 5
;    mov x7, 5
    mov x8, 5
    ret

myFunc32 endp

_TEXT32 ENDS


_TEXT16 SEGMENT USE16 'CODE'

public myFunc16

myFunc16 proc near
    LOCAL  x1:WORD, x2[3]:BYTE, x3:QWORD, x4:BYTE, x5:DWORD, x6:BYTE, x7:FWORD, x8:BYTE

    mov x1, 05656h
    mov x2, 5
;    mov x3, 5
    mov x4, 5
    mov x5, 05h
    mov x6, 5
;    mov x7, 5
    mov x8, 5
    ret

myFunc16 endp

_TEXT16 ENDS

end

