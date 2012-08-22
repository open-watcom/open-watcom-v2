# ui lib Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/unix/<OWOBJDIR>
    rm -f -r <PROJDIR>/unix/<OWOBJDIR>

[ BLOCK <1> boot ]
#=================
    echo Building the ui bootstrap
    mkdir <PROJDIR>/unix/<OWOBJDIR>
    cdsay <PROJDIR>/unix/<OWOBJDIR>
    wmake -h -f ../bootmake
    cdsay <PROJDIR>
