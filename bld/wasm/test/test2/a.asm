.model small
comment @
.data
    kyb db 0
    p db 0
    q db 0
    r db 0
.code
public foo
foo equ 1
bar equ 1
public bar
sam equ kyb
public sam

j equ p,q,r
public j

moose equ rat
public moose

z equ w
extern z:near
x equ kyb
extern x:byte
@
.data
    q db 0
.code
p equ q
r equ s
t equ u

u:nop

public p
public t
;public r

end
