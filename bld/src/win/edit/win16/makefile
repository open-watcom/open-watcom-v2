platform = Win16
CC = wcc
CFLAGS = -zW -DSTRICT -DWINVER=0x30A
res1_flags = -bt=windows -31

!include ../edit.mif
!include ../../master.mif

linkit : .PROCEDURE
    @%append $(LNK) system windows
    @%append $(LNK) lib commdlg.lib

bindit : .PROCEDURE
    wrc $(name).res
