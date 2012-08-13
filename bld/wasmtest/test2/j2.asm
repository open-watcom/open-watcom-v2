.386
.MODEL small
.code
    jmp late
    jmp short late
    jmp far ptr late
    jmp near ptr late
late:
    xor ax, ax
end
