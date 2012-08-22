# Prerequisite Tool Build Control File
# ====================================

set PROJDIR=<CWD>
set PROJNAME=wpack

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

set BLD_OPT=prebuild=1
set ODIR=<OWPREOBJDIR>
set TMP_BUILD_PLATFORM=<BUILD_PLATFORM>

[ BLOCK <OWLINUXBUILD> bootstrap ]
#=================================
set BLD_OPT=bootstrp=1
set ODIR=<OWOBJDIR>

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/<ODIR>
    rm -f -r <PROJDIR>/<ODIR>
    rm -f <OWBINDIR>/<PROJNAME>
    rm -f <OWBINDIR>/<PROJNAME>.exe
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> dos386 ]
#================================
    mkdir <PROJDIR>/<OWPREOBJDIR>
    cdsay <PROJDIR>/<OWPREOBJDIR>
    wmake -h -f ../dosi86/makefile <BLD_OPT>
    <CPCMD> <PROJNAME>.exe <OWBINDIR>/<PROJNAME><CMDEXT>

[ BLOCK <BUILD_PLATFORM> linux386 nt386 os2386 ]
#===============================================
    mkdir <PROJDIR>/<OWPREOBJDIR>
    cdsay <PROJDIR>/<OWPREOBJDIR>
    wmake -h -f ../<BUILD_PLATFORM>/makefile <BLD_OPT>
    <CPCMD> <PROJNAME>.exe <OWBINDIR>/<PROJNAME><CMDEXT>
    
[ BLOCK . . ]
#============
set BLD_OPT=
set ODIR=
set BUILD_PLATFORM=<TMP_BUILD_PLATFORM>
set TMP_BUILD_PLATFORM=

cdsay <PROJDIR>
