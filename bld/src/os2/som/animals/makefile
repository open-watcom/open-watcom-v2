CFLAGS = -zq -I.

animals.exe : animals.obj main.obj
    wlink op q sys os2v2 file { $< } lib somtk

animals.obj : animals.c animals.ih animals.h
    wcc386 $(CFLAGS) $[@

main.obj : main.c animals.h
    wcc386 $(CFLAGS) $[@

animals.ih animals.h : animals.idl
    sc -s"ih;h" -maddstar $<

clean : .symbolic
    rm -f *.obj *.exe *.h *.ih *.err
