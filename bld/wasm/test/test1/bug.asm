.model small
.data
    extrn doodle:word
.code
    add ax,word ptr doodle+4H
    add ax,word ptr doodle-4H
    add ax,word ptr doodle[4h]
    db 0x12
    db 0
    db ?
    nop
    nop
    nop
    nop
    nop
    add ax,word ptr 4[doodle] ;
end
