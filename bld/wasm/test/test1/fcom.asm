.8087
.model small
.data
    extern x:near
    extern x:near    ; duplicate extern for WASM check
.code
fcom dword ptr x
end
