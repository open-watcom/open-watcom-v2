# WOMP Builder Control file
# =========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay h
    wmake /h /i
#   cdsay ..\release
#   wmake /h /i

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
#   <CPCMD> womp.exe <relroot>\rel2\binw\womp.exe
#   <CPCMD> wompj.exe <relroot>\rel2\binw\japan\womp.exe

[ BLOCK <1> clean ]
#==================
    sweep killobjs
