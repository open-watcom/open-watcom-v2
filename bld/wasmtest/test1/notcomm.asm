DGROUP  group   _DATA

extern bar:word

_DATA   segment byte PUBLIC 'DATA'
        ;comm   foo:dword:1024
        public  foo
        a       db "before"
        foo     dd 1024 dup( ? )
        b       db "after"
_DATA   ends

_TEXT   segment dword  PUBLIC '_OVLCODE'
        assume  CS:_TEXT, DS:_DATA
        mov     word ptr DS:foo,CS
        mov     bx, word ptr foo
        add     bx, bar
        mov     word ptr [bx], cs
        xor ax, bar
_TEXT ends
end
