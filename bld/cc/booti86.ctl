# wcc Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/<OBJDIR>
    rm -f -r <PROJDIR>/<OBJDIR>

[ BLOCK <1> boot ]
#=================
    echo Building the wcc bootstrap
    mkdir <PROJDIR>/<OBJDIR>.i86
    cdsay <PROJDIR>/<OBJDIR>.i86
    cp -f ../linux386.i86/target.h .
    wmake -h -f ../makei86
    <CPCMD> wcci86c.exe <DEVDIR>/build/bin/wcc
    cdsay <PROJDIR>
