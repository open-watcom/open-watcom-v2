# WSTUB Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    wmake /h /i wstub.exe
    wmake /h /i wstubq.exe

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\wstub\wstub.exe <relroot>\rel2\binw\wstub.exe
    <CPCMD> <devdir>\wstub\wstubq.exe <relroot>\rel2\binw\wstubq.exe
    <CPCMD> <devdir>\wstub\wstub.c <relroot>\rel2\src\wstub.c

[ BLOCK <1> clean ]
#==================
    wmake /h clean
