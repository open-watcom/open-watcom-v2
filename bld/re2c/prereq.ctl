# RE2C Prerequisite Tool Build Control File
# =========================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

set TMP_BUILD_PLATFORM=<BUILD_PLATFORM>
set ODIR=<PREOBJDIR>

[ BLOCK <OWLINUXBUILD> bootstrap ]
#=================================
    set BUILD_PLATFORM=<BUILD_PLATFORM>boot
    set ODIR=<OBJDIR>

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/<ODIR>
    rm -f -r <PROJDIR>/<ODIR>
    rm -f <OWBINDIR>/re2c
    rm -f <OWBINDIR>/re2c.exe
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> dos386 ]
#================================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../dos386/makefile prebuild=1
    <CPCMD> re2c.exe <OWBINDIR>/re2c.exe

[ BLOCK <BUILD_PLATFORM> os2386 ]
#================================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../os2386/makefile prebuild=1
    <CPCMD> re2c.exe <OWBINDIR>/re2c.exe

[ BLOCK <BUILD_PLATFORM> nt386 ]
#===============================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../nt386/makefile prebuild=1
    <CPCMD> re2c.exe <OWBINDIR>/re2c.exe

[ BLOCK <BUILD_PLATFORM> ntaxp ]
#===============================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../ntaxp/makefile prebuild=1
    <CPCMD> re2c.exe <OWBINDIR>/re2c.exe

[ BLOCK <BUILD_PLATFORM> linux386 ]
#===============================
    mkdir <PROJDIR>/<PREOBJDIR>
    cdsay <PROJDIR>/<PREOBJDIR>
    wmake -h -f ../linux386/makefile prebuild=1
    <CPCMD> re2c.exe <OWBINDIR>/re2c

[ BLOCK <BUILD_PLATFORM> linux386boot ]
#===============================
    mkdir <PROJDIR>/<OBJDIR>
    cdsay <PROJDIR>/<OBJDIR>
    wmake -h -f ../linux386/makefile prebuild=1 bootstrap=1
    <CPCMD> re2c.exe <OWBINDIR>/re2c

[ BLOCK . . ]
#============
set BUILD_PLATFORM=<TMP_BUILD_PLATFORM>
set TMP_BUILD_PLATFORM=

cdsay <PROJDIR>
