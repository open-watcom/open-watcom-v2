CFLAGS = -zq -I.

hello.exe : hello.obj main.obj
    wlink op q sys os2v2 file { $< } lib somtk

hello.obj : hello.cpp hello.xih hello.xh
    wpp386 $(CFLAGS) $[@

main.obj : main.cpp hello.xh
    wpp386 $(CFLAGS) $[@

hello.xih hello.xh : hello.idl
    sc -s"xih;xh" $<

clean : .symbolic
    rm -f *.obj *.exe *.xh *.xih *.err
