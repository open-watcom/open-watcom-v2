# wcl Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

set TMP_BUILD_PLATFORM=<BUILD_PLATFORM>

#[ BLOCK <OWLINUXBUILD> bootstrap ]
#=================================
#    set BUILD_PLATFORM=<BUILD_PLATFORM>boot

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/<PREOBJDIR>
    rm -f -r <PROJDIR>/<PREOBJDIR>
    rm -f <OWBINDIR>/wcl386
    rm -f <OWBINDIR>/bwcl386
    rm -f <OWBINDIR>/bwcl386.exe
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> dos386 ]
#================================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../dosi86.386/makefile prebuild=1
    <CPCMD> wcl386.exe <OWBINDIR>/bwcl386.exe

[ BLOCK <BUILD_PLATFORM> os2386 ]
#================================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../os2386.386/makefile prebuild=1
    <CPCMD> wcl386.exe <OWBINDIR>/bwcl386.exe

[ BLOCK <BUILD_PLATFORM> nt386 ]
#===============================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../nt386.386/makefile prebuild=1
    <CPCMD> wcl386.exe <OWBINDIR>/bwcl386.exe

[ BLOCK <BUILD_PLATFORM> ntaxp ]
#===============================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../ntaxp.axp/makefile prebuild=1
    <CPCMD> wcl386.exe <OWBINDIR>/bwcl386.exe

[ BLOCK <BUILD_PLATFORM> linux386boot ]
#======================================
    echo Building the wcl bootstrap
    mkdir <PROJDIR>/<OBJDIR>
    cdsay <PROJDIR>/<OBJDIR>
    wmake -h -f ../linux386.386/makefile bootstrap=1
    <CPCMD> wcl386.exe <OWBINDIR>/bwcl386
    <CPCMD> wcl386.exe <OWBINDIR>/wcl386

[ BLOCK <BUILD_PLATFORM> linux386 ]
#==================================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../linux386.386/makefile prebuild=1
    <CPCMD> wcl386.exe <OWBINDIR>/bwcl386

[ BLOCK . . ]
#==================
set BUILD_PLATFORM=<TMP_BUILD_PLATFORM>
set TMP_BUILD_PLATFORM=

cdsay <PROJDIR>
