platform = Win32
CC = wcc386
CFLAGS = -bt=nt
res1_flags = -bt=nt

!include ../helpex.mif
!include ../../master.mif

linkit : .PROCEDURE
    @%append $(LNK) system nt_win
    @%append $(LNK) option stack=64k

bindit : .PROCEDURE
    wrc $(name).res

../helpex.hlp: ../helpex.rtf
    cd ..
    hc helpex
    cd win32
