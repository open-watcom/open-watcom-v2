CC = wcc386
CFLAGS  = -q -s -wx
INCS = -I"$(%WATCOM)/h/nt"
LIBC   = $(%WATCOM)/lib386/nt

CFLAGS += -d1

OBJS = devio.obj

.c.obj : .autodepend
    $(CC) $(CFLAGS) $(INCS) $<

devio.exe : $(OBJS)
    wlink op quiet, map f { $(OBJS) }

clean : .symbolic
    @rm -f *.exe
    @rm -f *.obj
    @rm -f *.map
    @rm -f *.err
