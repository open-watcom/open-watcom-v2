# MSGSTRM utility -- make message streams (DOS)


comp_sys = /bt=dos
link_sys = pharlap

!include ../makefile

msgstrm.exe : $(OBJS) makefile msgstrm.lnk
    $(linker) @msgstrm
    $(pharlap)bind386 $(pharlap)run386b msgstrm.exp -exe msgstrm.exe
    @rm -f msgstrm.exp
