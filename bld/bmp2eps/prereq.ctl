# bmp2eps Prerequisite Tool Build Control File
# ============================================

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
    echo rm -f -r <PROJDIR>/<OWPREOBJDIR>
    rm -f -r <PROJDIR>/<OWPREOBJDIR>
    rm -f <OWBINDIR>/bmp2eps.exe
    rm -f <OWBINDIR>/bmp2eps
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> dos386 ]
#================================
    mkdir <PROJDIR>/<OWPREOBJDIR>
    cdsay <PROJDIR>/<OWPREOBJDIR>
    wmake -h -f ../dos386/makefile prebuild=1
    <CPCMD> bmp2eps.exe <OWBINDIR>/bmp2eps.exe

[ BLOCK <BUILD_PLATFORM> os2386 ]
#================================
    mkdir <PROJDIR>/<OWPREOBJDIR>
    cdsay <PROJDIR>/<OWPREOBJDIR>
    wmake -h -f ../os2386/makefile prebuild=1
    <CPCMD> bmp2eps.exe <OWBINDIR>/bmp2eps.exe

[ BLOCK <BUILD_PLATFORM> nt386 ]
#===============================
    mkdir <PROJDIR>/<OWPREOBJDIR>
    cdsay <PROJDIR>/<OWPREOBJDIR>
    wmake -h -f ../nt386/makefile prebuild=1
    <CPCMD> bmp2eps.exe <OWBINDIR>/bmp2eps.exe

[ BLOCK <BUILD_PLATFORM> linux386 ]
#==================================
    mkdir <PROJDIR>/<OWPREOBJDIR>
    cdsay <PROJDIR>/<OWPREOBJDIR>
    wmake -h -f ../linux386/makefile prebuild=1
    <CPCMD> bmp2eps.exe <OWBINDIR>/bmp2eps

[ BLOCK . . ]
#============
set BUILD_PLATFORM=<TMP_BUILD_PLATFORM>
set TMP_BUILD_PLATFORM=

cdsay <PROJDIR>
