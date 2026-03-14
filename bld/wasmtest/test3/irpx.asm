.model small
.data
    for arg, <1,2,3,4,5,6,7,8,9,0>
        db arg
    endm
    irp arg, <1,2,3,4,5,6,7,8,9,0>
        dw arg
    endm
    forc arg, <1234567890>
        dd arg
    endm
    repeat 2
        dd 1
    endm
end
