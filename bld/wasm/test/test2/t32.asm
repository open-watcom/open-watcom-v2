_data segment byte public 'data'
foo db "this is a string"
bar db 0
_data ends

_data2 segment byte public 'data'
foo2 db "                                "
bar2 db 0
_data2 ends

_TEXT segment byte public 'code'

assume cs:_text
assume es:_data

cld
mov cx, 11
mov ax, seg foo
mov ds, ax
mov si, offset foo
mov ax, seg foo2
mov es, ax
mov di, offset foo2
rep movsb

_TEXT ends

end
