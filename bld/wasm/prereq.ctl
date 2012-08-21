# WASM Builder Control file
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
    rm -f <OWBINDIR>/bwasm
    rm -f <OWBINDIR>/bwasm.exe
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> dos386 ]
#================================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../dos386/makefile prebuild=1
    <CPCMD> wasm.exe <OWBINDIR>/bwasm.exe

[ BLOCK <BUILD_PLATFORM> os2386 ]
#================================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../os2386/makefile prebuild=1
    <CPCMD> wasm.exe <OWBINDIR>/bwasm.exe

[ BLOCK <BUILD_PLATFORM> nt386 ]
#===============================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../nt386/makefile prebuild=1
    <CPCMD> wasm.exe <OWBINDIR>/bwasm.exe

[ BLOCK <BUILD_PLATFORM> linux386boot ]
#======================================
    echo Building the wasm bootstrap
    mkdir <PROJDIR>/<OBJDIR>
    cdsay <PROJDIR>/<OBJDIR>
    wmake -h -f ../linux386/makefile bootstrap=1
    <CPCMD> wasm.exe <OWBINDIR>/bwasm

[ BLOCK <BUILD_PLATFORM> linux386 ]
#==================================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../linux386/makefile prebuild=1
    <CPCMD> wasm.exe <OWBINDIR>/bwasm

[ BLOCK . . ]
#============
set BUILD_PLATFORM=<TMP_BUILD_PLATFORM>
set TMP_BUILD_PLATFORM=

cdsay <PROJDIR>
