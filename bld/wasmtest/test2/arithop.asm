.386
.model small

.code

foo equ 0
bar equ 1

if stack
    mov ax, 1234
endif

if foo and bar
    mov ax, 1
endif
if foo or bar
    mov ax, 2
endif

if not foo
    mov ax, 3
else
    mov ax, 4
endif

if foo eq bar
    mov ax, 5
endif

if foo lt bar
    mov ax, 6
endif

if foo le bar
    mov ax, 7
endif

if foo gt bar
    mov ax, 8
endif

if foo ge bar
    mov ax, 9
endif

ife foo
    mov ax, 10
endif

ife foo xor bar
    mov ax, 11
endif


end
