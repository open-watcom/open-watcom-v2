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
    echo rm -f -r <PROJDIR>/intel/386/<OBJDIR>
    rm -f -r <PROJDIR>/intel/386/<OBJDIR>
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> linux386boot ]
#======================================
    echo Building the 386 codegen bootstrap
    mkdir <PROJDIR>/intel/386/<OBJDIR>
    cdsay <PROJDIR>/intel/386/<OBJDIR>
    wmake -h -f ../linux386/makefile bootstrap=1

[ BLOCK . . ]
#============
set BUILD_PLATFORM=<TMP_BUILD_PLATFORM>
set TMP_BUILD_PLATFORM=

cdsay <PROJDIR>
