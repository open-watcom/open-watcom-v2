.model small
.data
.code
extern bar_:near
public foo_
foo_:
    mov bx, 10
lbl1: call bar_
    dec bx
    jne lbl1
    retn
end
