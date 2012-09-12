# POSIX Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=build POSIX tools

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <PROJDIR>/<OWOBJDIR>/*.exe <OWBINDIR>/

[ BLOCK <1> bootclean ]
#======================
    cd <PROJDIR>
    mkdir <PROJDIR>/<OWOBJDIR>
    cd <PROJDIR>/<OWOBJDIR>
    wmake -h -f ../bootmake clean
    cd <PROJDIR>

[ BLOCK . . ]
#============
cdsay <PROJDIR>
