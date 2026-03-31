; Test: a macro parameter name matching a global symbol must not be expanded
; at macro definition time. Before the fix, WASM expanded 'num' on the
; MACRO line, so the DUP count in the recorded body was frozen to the
; global EQU value instead of using the later macro argument.
;
.386
.model small
.data

num EQU 1

build macro num
x label byte
    db num dup(0)
y label byte
    mov al, y - x
endm

build 3          ; fixed: emits 3 zero bytes, then mov al,3
                 ; buggy: emits 1 zero byte, then mov al,1

.code
end
