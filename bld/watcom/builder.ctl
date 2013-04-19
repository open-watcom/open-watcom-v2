# WATCOM CRTL extension libraries Builder Control file
# ====================================================

set PROJDIR=<CWD>
set PROJNAME=watcom

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay <PROJDIR>

[ BLOCK <1> boot ]
#=================
    mkdir <OWOBJDIR>
    cdsay <OWOBJDIR>
    wmake -h -f ../binmake bootstrap=1
    wmake -h -f ../binmake bootstrap=1 sharedlib=1
    cdsay <PROJDIR>

[ BLOCK <1> bootclean ]
#======================
    echo rm -r -f <OWOBJDIR>
    rm -r -f <OWOBJDIR>

[ BLOCK . . ]
#============
cdsay <PROJDIR>
