# UI Builder Control file
# =======================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay dos\ofw
    wmake /h /i
    cdsay ..\..\os2\olw
    wmake /h /i
    cdsay ..\..\qnx\olwq
    wmake /h /i
    cdsay ..\os32q
    wmake /h /i
    cdsay ..\..\win\olw
    wmake /h /i
    cdsay ..\..\nt\ofw
    wmake /h /i
    cdsay ..\..\necdos\ofw
    wmake /h /i
    cdsay ..\..\necwin\olw
    wmake /h /i
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <PROJDIR>\qnx\tix\*.tix <relroot>\rel2\qnx\tix\

[ BLOCK <1> clean ]
#==================
    sweep killobjs
