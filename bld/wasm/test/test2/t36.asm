;.386

; test jumps

;_text segment byte public use32 'code'
_text segment byte public 'code'
assume cs:_text

foo:    and ax,ax
        jne foo
        jmp bar
        xor bx, bx
bar:    nop

_text ends
end
