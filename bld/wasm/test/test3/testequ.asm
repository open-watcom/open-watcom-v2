.MODEL small
bunk equ 12
.code

errgen macro num, msg
    public err&num
    err&num byte   'Error &num: percent sign->%<- &msg'
endm

mov ax, 1

.data

foo = "this is a message"
foo1 equ "this is a message"
foo2 textequ "this is a message"

errgen %2+3*5, %foo

errgen bunk, foo
errgen bunk1, foo1
errgen bunk2, foo2

errgen %bunk, "asdf"
errgen 1, %"asdf"

errgen 666, "3, commas, in, here"

end
