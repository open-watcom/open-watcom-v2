# Causeway Prerequisite Tool Build Control File
# =============================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/cwc/<OWPREOBJDIR>
    rm -f -r <PROJDIR>/cwc/<OWPREOBJDIR>
    echo rm -f -r <PROJDIR>/cw32/<OWPREOBJDIR>
    rm -f -r <PROJDIR>/cw32/<OWPREOBJDIR>
    rm -f <OWBINDIR>/bcwc<CMDEXT>
    rm -f <OWBINDIR>/cwstub.exe


[ BLOCK <1> build rel2 ]
#==================
    mkdir <PROJDIR>/cwc/<OWPREOBJDIR>
    cdsay <PROJDIR>/cwc/<OWPREOBJDIR>
    wmake -h -f ../<BUILD_PLATFORM>/makefile
    <CPCMD> bcwc.exe <OWBINDIR>/bcwc<CMDEXT>
    mkdir <PROJDIR>/cw32/<OWPREOBJDIR>
    cdsay <PROJDIR>/cw32/<OWPREOBJDIR>
    wmake -h -f ../dosi86/makefile
    <CPCMD> cwstub.exe <OWBINDIR>/cwstub.exe
    cdsay <PROJDIR>

[ BLOCK . . ]
#============

cdsay <PROJDIR>
