platform = Win16
CC = wcc386
CFLAGS = -zW -DSTRICT -DWINVER=0x30A
res1_flags = -bt=windows -31

!include ../edit.mif
!include ../../master.mif

linkit : .PROCEDURE
    @%append $(LNK) system win386
    @%append $(LNK) option mindata=100K
    @%append $(LNK) option maxdata=100K
    @%append $(LNK) option stack=8k

bindit : .PROCEDURE
    wbind $(name) -R $(name).res
