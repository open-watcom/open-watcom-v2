CC = wcc
CFLAGS = -zW
res1_flags = -bt=windows -30

!include ..\life.mif
!include ..\..\master.mif

linkit : .PROCEDURE
    @%append $(LNK) system windows
    @%append $(LNK) lib commdlg.lib

bindit : .PROCEDURE
    wrc $(name).res
