CC = wcc386
CFLAGS = -bt=nt
res1_flags = -bt=nt

!include ..\watzee.mif
!include ..\..\master.mif

linkit : .PROCEDURE
    @%append $(LNK) system nt_win
    @%append $(LNK) option stack=64k

bindit : .PROCEDURE
    wrc $(name).res
