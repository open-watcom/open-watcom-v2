;.386
; test jumps

_data segment byte public 'code'
bar:    xor ax, ax
        jmp far ptr foo
_data ends

;_text segment byte public use32 'code'
_text segment byte public 'code'
assume cs:_text
assume ds:_data
assume es:_data2

        mov ax, 1
foo:    add ax, 2
        add ax, 0fffh
        inc ax
        jmp far ptr bar
        jmp foo
        jmp far ptr foo

        jmp far ptr foo2
        jmp far ptr bar2
        xor ax, ax
foo2:   xor ax, ax

_text ends

_data2 segment byte public 'code'
bar2:   xor ax, ax
        jmp far ptr foo
_data2 ends
end
