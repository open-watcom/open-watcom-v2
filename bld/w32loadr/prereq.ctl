# W32LOADR Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/<PREOBJDIR>
    rm -f -r <PROJDIR>/<PREOBJDIR>
    rm -f <OWBINDIR>/w32bind<CMDEXT>
    rm -f <OWBINDIR>/os2ldr.exe

[ BLOCK <1> build rel2 ]
#==================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../<BUILD_PLATFORM>/makefile prebuild=1
    <CPCMD> w32bind.exe <OWBINDIR>/w32bind<CMDEXT>
    wmake -h -f ../os2ldr/makefile prebuild=1
    <CPCMD> os2ldr.exe <OWBINDIR>/os2ldr.exe

[ BLOCK . . ]
#============

cdsay <PROJDIR>
