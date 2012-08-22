# wd Builder Control file
# =======================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/<OWOBJDIR>
    rm -f -r <PROJDIR>/<OWOBJDIR>
    [ INCLUDE <PROJDIR>/linux/boot.ctl ]

[ BLOCK <1> boot ]
#=================
    echo Building the debugger bootstrap
    cdsay <PROJDIR>/ssl
    wmake -h
    <CPCMD> <PROJDIR>/ssl/*.prs <DEVDIR>/build/bin/
    <CPCMD> <PROJDIR>/dbg/*.dbg <DEVDIR>/build/bin/
    [ INCLUDE <PROJDIR>/linux/boot.ctl ]
    cdsay <PROJDIR>
