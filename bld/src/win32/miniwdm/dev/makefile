CC = wcc386
RC = wrc
CFLAGS  = -q -s -wx
RCFLAGS = -q -r -zm
INCS = -I"$(%WATCOM)/h/nt/ddk"
LIBDDK = $(%WATCOM)/lib386/nt/ddk
LIBC   = $(%WATCOM)/lib386/nt

# Debug flag
CFLAGS += -DDBG

CFLAGS += -d1 -hc

OBJS = wdmdev.obj

.c.obj : .autodepend
    $(CC) $(CFLAGS) $(INCS) $<

boxdev.sys : $(OBJS) $(__MAKEFILES__) boxdev.res
    wlink op quiet, map name $@ format windows nt runtime native=4.0 &
      debug codeview op cvpack, symf &
      option start='_DriverEntry@8' &
      option offset=0x10000, checksum, osversion=1.0, version=1.0 &
      option stack=0x100000, heapsize=0x100000 &
      segment class CODE nonpageable, class DATA nonpageable &
      file { $OBJS } libpath $(LIBDDK) libpath $(LIBC) &
      lib ntoskrnl, hal, clib3r resource boxdev.res

#      option start='_DriverEntry@8', alignment=0x20, objalign=0x20 &

boxdev.res : boxdev.rc
    $(RC) $(RCFLAGS) $(INCS) $<

clean : .symbolic
    @rm -f *.sys
    @rm -f *.sym
    @rm -f *.obj
    @rm -f *.map
    @rm -f *.err
    @rm -f *.res
