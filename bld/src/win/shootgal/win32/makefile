CC = wcc386
CFLAGS = -bt=nt
res1_flags = -bt=nt -d__NT__

!include ..\shootgal.mif
!include ..\..\master.mif

linkit : .PROCEDURE
    @%append $(LNK) system nt_win
    @%append $(LNK) option stack=64k

bindit : .PROCEDURE
    wrc $(name).res
