.386
.model small
.data
ifdef __WASM__
    db "its wasm"
else
    db "its not wasm"
endif
.code
    xor ax, ax
end
