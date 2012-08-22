# wcc Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/<OWOBJDIR>
    rm -f -r <PROJDIR>/<OWOBJDIR>

[ BLOCK <1> boot ]
#=================
    echo Building the wcc bootstrap
    mkdir <PROJDIR>/<OWOBJDIR>.i86
    cdsay <PROJDIR>/<OWOBJDIR>.i86
    cp -f ../linux386.i86/target.h .
    wmake -h -f ../makei86
    <CPCMD> wcci86c.exe <DEVDIR>/build/bin/wcc
    cdsay <PROJDIR>
