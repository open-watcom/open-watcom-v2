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
    cdsay <PROJDIR>

[ BLOCK <1> bootclean ]
#======================
    echo rm -rf <OWOBJDIR>
    rm -rf <OWOBJDIR>

[ BLOCK . . ]
#============
cdsay <PROJDIR>
