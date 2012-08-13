_text segment byte public 'code'
assume cs:_text

CMP     ES:[si+7], BL
CMP     ES:[49h], BL    ;   Get current video mode
MOV     AL,ES:[0487h]   ;   check VC state
MOV     AL, ES:[0487h]  ;   check VC state
MOV     AX,ES:[0487h]   ;   check VC state
MOV     AX, ES:[0487h]  ;   check VC state

_text ends
end

