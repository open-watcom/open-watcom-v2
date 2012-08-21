# Causeway Prerequisite Tool Build Control File
# =============================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/cwc/<PREOBJDIR>
    rm -f -r <PROJDIR>/cwc/<PREOBJDIR>
    echo rm -f -r <PROJDIR>/cw32/<PREOBJDIR>
    rm -f -r <PROJDIR>/cw32/<PREOBJDIR>
    rm -f <OWBINDIR>/bcwc<CMDEXT>
    rm -f <OWBINDIR>/cwstub.exe


[ BLOCK <1> build rel2 ]
#==================
    mkdir <PROJDIR>/cwc/<PREOBJDIR>
    cdsay <PROJDIR>/cwc/<PREOBJDIR>
    wmake -h -f ../<BUILD_PLATFORM>/makefile
    <CPCMD> bcwc.exe <OWBINDIR>/bcwc<CMDEXT>
    mkdir <PROJDIR>/cw32/<PREOBJDIR>
    cdsay <PROJDIR>/cw32/<PREOBJDIR>
    wmake -h -f ../dosi86/makefile
    <CPCMD> cwstub.exe <OWBINDIR>/cwstub.exe
    cdsay <PROJDIR>

[ BLOCK . . ]
#============

cdsay <PROJDIR>
