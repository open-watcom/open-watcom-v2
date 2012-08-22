# wlink Builder Control file
# =========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/<OWOBJDIR>
    rm -f -r <PROJDIR>/<OWOBJDIR>
    wmake -h clean

[ BLOCK <1> boot ]
#=================
    echo Building the wlink bootstrap
    wmake -h
    <CPCMD> wlsystem.lnk <OWRELROOT>/binw/wlsystem.lnk
    mkdir <PROJDIR>/<OWOBJDIR>
    cdsay <PROJDIR>/<OWOBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> wl.exe <DEVDIR>/build/bin/wlink
    cdsay <PROJDIR>
