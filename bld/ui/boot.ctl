# ui lib Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/unix/<OBJDIR>
    rm -f -r <PROJDIR>/unix/<OBJDIR>

[ BLOCK <1> boot ]
#=================
    echo Building the ui bootstrap
    mkdir <PROJDIR>/unix/<OBJDIR>
    cdsay <PROJDIR>/unix/<OBJDIR>
    wmake -h -f ../bootmake
    cdsay <PROJDIR>
