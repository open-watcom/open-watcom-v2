# gui Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/ui/<OBJDIR>
    rm -f -r <PROJDIR>/ui/<OBJDIR>

[ BLOCK <1> boot ]
#=================
    echo Building the gui bootstrap
    mkdir <PROJDIR>/ui/<OBJDIR>
    cdsay <PROJDIR>/ui/<OBJDIR>
    wmake -h -f ../bootmake
    cdsay <PROJDIR>
