.MODEL small, c, os_dos, NEARSTACK
.STACK

.DATA

foo dw 10

.DATA?
bar dw ?
dude dw 12

.CONST

neg1 db "hello"

.FARDATA maaak

foo1 dw 2

.FARDATA? kaaam

dog dw ?

.CODE
        ;.STARTUP
        jmp start
        xor ax, ax
start:  xor bx, bx
next:   inc bx
        cmp bx, foo
        jge next

;end  start
end
