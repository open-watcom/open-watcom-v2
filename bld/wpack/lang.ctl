# WPACK Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay o
    wmake /h /i
    cdsay ..\nt
    wmake /h /i
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\wpack\nt\wpack.exe <distroot>\supp\

[ BLOCK <1> clean ]
#==================
    sweep killobjs
