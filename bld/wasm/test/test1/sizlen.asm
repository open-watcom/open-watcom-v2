.model small

.data

x0  dw ?
x1  dw 15 dup(?)

extrn x2:dword

.code

y0:
    add ax, size x0
    add ax, sizeof x0
    add ax, length x0
    add ax, lengthof x0
    add ax, size x1
    add ax, sizeof x1
    add ax, length x1
    add ax, lengthof x1
; doesn't work (FIXME)
;    add ax, size x2
;    add ax, sizeof x2
;    add ax, length x2
;    add ax, lengthof x2
    add ax, length y0
    add ax, size y0

end
