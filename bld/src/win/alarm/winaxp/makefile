CC = wccaxp
CFLAGS = -bt=nt
res1_flags = -bt=nt

!include ..\alarm.mif
!include ..\..\master.mif

linkit : .PROCEDURE
    @%append $(LNK) system ntaxp_win
    @%append $(LNK) option stack=64k

bindit : .PROCEDURE
    wrc $(name).res
