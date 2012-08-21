set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

set BUILDP=<BUILD_PLATFORM>
[ BLOCK <1> clean ]
#==================
    set BUILDP=<BUILDP>clean

[ BLOCK <OWLINUXBUILD> bootstrap ]
#=================================
    set BUILDP=boot<BUILDP>

[ BLOCK <BUILDP> bootlinux386clean ]
#===================================
    echo rm -f -r <PROJDIR>/<OBJDIR>
    rm -f -r <PROJDIR>/<OBJDIR>
    rm -f <OWBINDIR>/wlink<CMDEXT>
    wmake -h clean

[ BLOCK <BUILDP> bootlinux386 ]
#==============================
    wmake -h
    <CPCMD> wlsystem.lnk <RELROOT>/binw/wlsystem.lnk
    mkdir <PROJDIR>/<OBJDIR>
    cdsay <PROJDIR>/<OBJDIR>
    wmake -h -f ../<BUILD_PLATFORM>/makefile bootstrap=1
    <CPCMD> wl.exe <OWBINDIR>/wlink

[ BLOCK <BUILDP> dos386clean os2386clean nt386clean linux386clean ]
#==================================================================
    echo rm -f -r <PROJDIR>/<PREOBJDIR>
    rm -f -r <PROJDIR>/<PREOBJDIR>

[ BLOCK <BUILDP> dos386 os2386 nt386 linux386 ]
#==============================================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wsplice -k Pwlsystem ../specs.sp wlsystem.lnk
    wmake -h -f ../<BUILD_PLATFORM>/makefile prebuild=1

[ BLOCK . . ]
#============
set BUILDP=

cdsay <PROJDIR>
