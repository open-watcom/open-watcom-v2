
;test string variables

_data segment byte public 'data'
foo db "this is a string"
dog byte "this is a string"
moo dw 12
cat dw "ab"
sam word "cd"
food dd 123
cat1 dd "asdf"
sam1 dword "fred"
a byte 10 dup (65)
ed  db "long string that is split to",
        " two lines."
b byte 10 dup (65)
ed2 db "long string that is split to",
        " two lines."
d byte 10 dup (65)
x db "long string that is split to",
        " two lines."
e byte 10 dup (65)
next db "really long string, split to",
        " not just 2 but ",
        "3 lines"
jim byte 10 dup (65)
bar dw 0
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
