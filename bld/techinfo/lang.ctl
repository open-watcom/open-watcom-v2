# TECHINFO Builder Control file
# =============================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]

cdsay .

[ BLOCK <1> build rel2 ]
    wmake -h

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <PROJDIR>\techinfo.exe <relroot>\rel2\binw\
