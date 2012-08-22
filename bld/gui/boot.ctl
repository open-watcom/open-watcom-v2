# gui Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/ui/<OWOBJDIR>
    rm -f -r <PROJDIR>/ui/<OWOBJDIR>

[ BLOCK <1> boot ]
#=================
    echo Building the gui bootstrap
    mkdir <PROJDIR>/ui/<OWOBJDIR>
    cdsay <PROJDIR>/ui/<OWOBJDIR>
    wmake -h -f ../bootmake
    cdsay <PROJDIR>
