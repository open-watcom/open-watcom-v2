.386
.model small
_emu_init_start segment word public 'EMU'
_emu_init_start ends

_emu_init_end segment word public 'EMU'
_emu_init_end ends

DGROUP group _emu_init_start

;,_emu_init_end

_TEXT segment
start:
        mov     BP,offset _emu_init_end  ; see if we have an emulator
        cmp     BP,offset DGROUP:_emu_init_start ; ...
_TEXT ends
end start

