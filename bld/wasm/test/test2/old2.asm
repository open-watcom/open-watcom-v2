_data segment byte public 'data'

y   df 0abcdef12345h
z   dq 0abcdef123456789ah
w   dt 0abcdef123456789abcdeh

;foo  equ "hello there"
;bar  equ foo

_data ends

_text segment byte public 'code'
assume cs:_text
assume ds:_data

;foo:  mov bx, OFFSET foo
;mov ax, SEG foo
jmp   word ptr es:[bx]
_text ends
end
