_text segment byte public 'code'
assume cs:_text

CMP     ES:[49h], BL    ;   Get current video mode
CMP     ES:49h, BL    ;   Get current video mode
mov     word ptr es:[bx], 40h

_text ends
end

