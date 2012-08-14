set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

set TMP_BUILD_PLATFORM=<BUILD_PLATFORM>

[ BLOCK <OWLINUXBUILD> bootstrap ]
#=================================
    set BUILD_PLATFORM=<BUILD_PLATFORM>boot

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/386/<OBJDIR>
    rm -f -r <PROJDIR>/386/<OBJDIR>
    rm -f <OWBINDIR>/wcc386
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> linux386boot ]
#======================================
    echo Building the wcc386 bootstrap
    mkdir <PROJDIR>/386/<OBJDIR>
    cdsay <PROJDIR>/386/<OBJDIR>
    wmake -h -f ../linux386/makefile bootstrap=1
    <CPCMD> wcc386c.exe <OWBINDIR>/wcc386

[ BLOCK . . ]
#============
set BUILD_PLATFORM=<TMP_BUILD_PLATFORM>
set TMP_BUILD_PLATFORM=

cdsay <PROJDIR>
