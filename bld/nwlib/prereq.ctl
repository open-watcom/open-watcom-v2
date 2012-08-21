# WLIB Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

set TMP_BUILD_PLATFORM=<BUILD_PLATFORM>

[ BLOCK <OWLINUXBUILD> bootstrap ]
#=================================
    set BUILD_PLATFORM=<BUILD_PLATFORM>boot

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/<PREOBJDIR>
    rm -f -r <PROJDIR>/<PREOBJDIR>
    rm -f <OWBINDIR>/bwlib<CMDEXT>
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> dos386 os2386 nt386 ntaxp linux386 ]
#============================================================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../<BUILD_PLATFORM>/makefile prebuild=1
    <CPCMD> wlib.exe <OWBINDIR>/bwlib<CMDEXT>

[ BLOCK <BUILD_PLATFORM> linux386boot ]
#======================================
    echo Building the wlib bootstrap
    mkdir <PROJDIR>/<OBJDIR>
    cdsay <PROJDIR>/<OBJDIR>
    wmake -h -f ../linux386/makefile bootstrap=1
    <CPCMD> wlib.exe <OWBINDIR>/bwlib<CMDEXT>

[ BLOCK . . ]
#============
set BUILD_PLATFORM=<TMP_BUILD_PLATFORM>
set TMP_BUILD_PLATFORM=

cdsay <PROJDIR>
