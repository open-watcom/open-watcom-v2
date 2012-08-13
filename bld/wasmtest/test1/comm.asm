DGROUP  group   _DATA

comm    foo:dword:1024
comm    bar:dword:1024

_DATA   segment byte PUBLIC 'DATA'
        a       db "before"
        b       db "after"
_DATA   ends

_TEXT   segment dword  PUBLIC '_OVLCODE'
        assume  CS:_TEXT, DS:_DATA
        mov     word ptr DS:foo,CS
_TEXT ends
end
