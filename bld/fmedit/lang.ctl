# FMEDIT Builder Control file
# ===========================
# This should really be part of the SDK
# it is called from SDK\lang.ctl

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay obj
    wmake /h /i
    cdsay ..\nt
    wmake /h /i

[ BLOCK <1> rel2 cprel2 ]
<cpcmd> <projdir>\obj\fmedit.dll <relroot>\rel2\binw\
<cpcmd> <projdir>\nt\fmedit.dll <relroot>\rel2\binnt\

[ BLOCK <1> clean ]
#==================
    cdsay obj
    wmake /h /i clean
    cdsay ..\nt
    wmake /h /i clean
