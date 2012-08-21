# WHPCVT Prerequisite Tool Build Control File
# =========================================

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
    rm -f <OWBINDIR>/objchg<CMDEXT>
    rm -f <OWBINDIR>/objfind<CMDEXT>
    rm -f <OWBINDIR>/objlist<CMDEXT>
    rm -f <OWBINDIR>/objxdef<CMDEXT>
    rm -f <OWBINDIR>/objxref<CMDEXT>
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> dos386 os2386 nt386 linux386 ]
#======================================================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../<BUILD_PLATFORM>/makefile prebuild=1
    <CPCMD> objchg.exe   <OWBINDIR>/objchg<CMDEXT>
    <CPCMD> objfind.exe  <OWBINDIR>/objfind<CMDEXT>
    <CPCMD> objlist.exe  <OWBINDIR>/objlist<CMDEXT>
    <CPCMD> objxdef.exe  <OWBINDIR>/objxdef<CMDEXT>
    <CPCMD> objxref.exe  <OWBINDIR>/objxref<CMDEXT>

[ BLOCK . . ]
#============
set BUILD_PLATFORM=<TMP_BUILD_PLATFORM>
set TMP_BUILD_PLATFORM=

cdsay <PROJDIR>
