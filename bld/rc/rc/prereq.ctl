set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
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
    echo rm -f -r <PROJDIR>/wres/<ODIR>
    rm -f -r <PROJDIR>/wres/<ODIR>
    echo rm -f -r <PROJDIR>/rc/<ODIR>
    rm -f -r <PROJDIR>/rc/<ODIR>
    rm -f <OWBINDIR>/wrc
    rm -f <OWBINDIR>/bwrc
    rm -f <OWBINDIR>/bwrc.exe
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> dos386 ]
#================================
    mkdir <PROJDIR>/wres/<PREOBJDIR>
    cdsay <PROJDIR>/wres/<PREOBJDIR>
    wmake -h -f ../small386/ms_r/makefile prebuild=1
    mkdir <PROJDIR>/rc/<PREOBJDIR>
    cdsay <PROJDIR>/rc/<PREOBJDIR>
    wmake -h -f ../dos386/makefile prebuild=1
    <CPCMD> wrce.exe <OWBINDIR>/bwrc.exe

[ BLOCK <BUILD_PLATFORM> nt386 ]
#===============================
    mkdir <PROJDIR>/wres/<PREOBJDIR>
    cdsay <PROJDIR>/wres/<PREOBJDIR>
    wmake -h -f ../flat386/mf_r/makefile prebuild=1
    mkdir <PROJDIR>/rc/<PREOBJDIR>
    cdsay <PROJDIR>/rc/<PREOBJDIR>
    wmake -h -f ../nt386/makefile prebuild=1
    <CPCMD> wrce.exe <OWBINDIR>/bwrc.exe

[ BLOCK <BUILD_PLATFORM> ntaxp ]
#===============================
    mkdir <PROJDIR>/wres/<PREOBJDIR>
    cdsay <PROJDIR>/wres/<PREOBJDIR>
    wmake -h -f ../ntaxp/_s/makefile prebuild=1
    mkdir <PROJDIR>/rc/<PREOBJDIR>
    cdsay <PROJDIR>/rc/<PREOBJDIR>
    wmake -h -f ../ntaxp/makefile prebuild=1
    <CPCMD> wrce.exe <OWBINDIR>/bwrc.exe

[ BLOCK <BUILD_PLATFORM> os2386 ]
#================================
    mkdir <PROJDIR>/wres/<PREOBJDIR>
    cdsay <PROJDIR>/wres/<PREOBJDIR>
    wmake -h -f ../flat386/mf_r/makefile prebuild=1
    mkdir <PROJDIR>/rc/<PREOBJDIR>
    cdsay <PROJDIR>/rc/<PREOBJDIR>
    wmake -h -f ../os2386/makefile prebuild=1
    <CPCMD> wrce.exe <OWBINDIR>/bwrc.exe

[ BLOCK <BUILD_PLATFORM> linux386 ]
#==================================
    mkdir <PROJDIR>/wres/<PREOBJDIR>
    cdsay <PROJDIR>/wres/<PREOBJDIR>
    wmake -h -f ../linux386/mf_r/makefile prebuild=1
    mkdir <PROJDIR>/rc/<PREOBJDIR>
    cdsay <PROJDIR>/rc/<PREOBJDIR>
    wmake -h -f ../linux386/makefile prebuild=1
    <CPCMD> wrce.exe <OWBINDIR>/bwrc

[ BLOCK <BUILD_PLATFORM> linux386boot ]
#======================================
    echo Building the wres library and the resource compiler
    mkdir <PROJDIR>/wres/<OBJDIR>
    cdsay <PROJDIR>/wres/<OBJDIR>
    wmake -h -f ../linux386/mf_r/makefile bootstrap=1
    mkdir <PROJDIR>/rc/<OBJDIR>
    cdsay <PROJDIR>/rc/<OBJDIR>
    wmake -h -f ../linux386/makefile bootstrap=1
    <CPCMD> wrce.exe <OWBINDIR>/bwrc

[ BLOCK . . ]
#============
set BUILD_PLATFORM=<TMP_BUILD_PLATFORM>
set TMP_BUILD_PLATFORM=

cdsay <PROJDIR>
