_TEXT segment byte public 'code'

assume cs:_text
extrn hug:word

; test backquoted symbols

bob = 23467 ; big const
`evil_const_in_quotes(@&$)(#*`=2
`evil_const_in_quotes2@&$)(#*`equ 4
sam equ 2
dog: mov ax, bob
xor ax, ax
mov ax, `evil_const_in_quotes2@&$)(#*`
mov ax,`evil_const_in_quotes2@&$)(#*`
`evil_label)(*#)@!`: mov bx, ax
jmp`evil_label)(*#)@!`

_TEXT ends

end
