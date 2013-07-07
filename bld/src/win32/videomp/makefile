CC = wcc386
RC = wrc
CFLAGS = -q -s -ecd -wx
RCFLAGS = -q -r -zm
INCS = -I"$(%WATCOM)/h/nt/ddk"
LIBS = $(%WATCOM)/lib386/nt/ddk

# Debug flag - note that the video port in Windows NT 3.x only exports
# VideoDebugPort in checked builds!
#CFLAGS += -DDBG

CFLAGS += -d1 -hc

OBJS = videomp.obj vidmpdat.obj boxv.obj

.c.obj : .autodepend
    $(CC) $(CFLAGS) $(INCS) $<

boxvideo.sys : $(OBJS) $(__MAKEFILES__) videomp.res
    wlink op quiet, map name $@ format windows nt runtime native=1.0 &
      debug codeview op symf, cvpack &
      option start='_DriverEntry@8' &
      option offset=0x10000, checksum, osversion=1.0, version=1.0 &
      option stack=0x100000, heapsize=0x100000 &
      segment class CODE nonpageable, class DATA nonpageable &
      file { $OBJS } libpath $(LIBS) lib videoprt resource videomp.res

# Normally we would like to use the following:
#      option alignment=0x20, objalign=0x20
# but that causes the driver image to fail loading on old NT releases;
# the driver is small enough that it's not worth the trouble

videomp.res : videomp.rc
    $(RC) $(RCFLAGS) $(INCS) $<

# Create a 1.44MB distribution floppy image.
# NB: The mkimage tool is not supplied.
videomp.img : boxvideo.sys disk1 readme.txt oemsetup.inf vidmini.inf
    if not exist dist mkdir dist
    cp disk1        dist
    cp boxvideo.sys dist
    cp boxvideo.sym dist
    cp oemsetup.inf dist
    cp vidmini.inf  dist
    cp readme.txt   dist
    mkimage -l VIDEOMP -o videomp.img dist
    
clean : .symbolic
    rm -f *.obj
    rm -f *.sys
    rm -f *.sym
    rm -f *.res
    rm -f *.map
    rm -f videomp.img
    rm -f dist/disk1
    rm -f dist/boxvideo.sys
    rm -f dist/boxvideo.sym
    rm -f dist/oemsetup.inf
    rm -f dist/vidmini.inf
    rm -f dist/readme.txt
    if exist dist rmdir dist
