# HEAPWALKER Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    wmake -h

[ BLOCK <1> rel2 cprel2 ]
    <CPCMD> <devdir>\sdk\heapwalk\wheapwlk.exe <relroot>\rel2\binw\wheapwlk.exe

[ BLOCK <1> clean ]
#==================
    wmake -h clean
