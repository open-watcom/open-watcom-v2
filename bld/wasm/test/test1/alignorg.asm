.386
;.model small
;.code
_TEXT segment page use32 'CODE'
    org 1
    align 2 ; 1 byte
    org 6
    align 4 ; 2 byte
    org 9
    align 4 ; 3 byte
    org 20
    align 8 ; 4 byte
    org 27
    align 8 ; 5 byte
    org 34
    align 8 ; 6 byte
;    org $ + 1
    org 41
    align 8 ; 7 byte
;    org $+8
    org 56
    align 16 ; 8 byte
    mov eax, $
_TEXT ends
end


