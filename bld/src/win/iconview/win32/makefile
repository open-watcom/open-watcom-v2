platform = Win32
CC = wcc386
CFLAGS = -bt=nt -zp2
res1_flags = -bt=nt

!include ../view.mif
!include ../../master.mif

linkit : .PROCEDURE
    @%append $(LNK) system nt_win
    @%append $(LNK) option stack=64k

bindit : .PROCEDURE
    wrc $(name).res
