CFLAGS = -zq -I.

hello.exe : hello.obj main.obj
    wlink op q sys os2v2 file { $< } lib somtk

hello.obj : hello.c hello.ih hello.h
    wcc386 $(CFLAGS) $[@

main.obj : main.c hello.h
    wcc386 $(CFLAGS) $[@

hello.ih hello.h : hello.idl
    sc -s"ih;h" -maddstar $<

clean : .symbolic
    rm -f *.obj *.exe *.h *.ih *.err
