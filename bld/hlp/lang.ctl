# HLP Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\hlp\*.hlp <relroot>\rel2\binw\
    <CPCMD> <devdir>\hlp\watcomc.inf <relroot>\rel2\binp\help\

[ BLOCK <1> clean ]
#==================
    sweep killobjs
