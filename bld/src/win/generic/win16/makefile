platform = Win16
CC = wcc
CFLAGS = -zW
res1_flags = -bt=windows -30

!include ../generic.mif
!include ../../master.mif

linkit : .PROCEDURE
    @%append $(LNK) system windows

bindit : .PROCEDURE
    wrc $(name).res
