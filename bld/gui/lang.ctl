# GUI Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay ui\obj
    wmake /h /i
    cdsay ..\objw
    wmake /h /i
    cdsay ..\objq
    wmake /h /i
    cdsay ..\obj32q
    wmake /h /i
    cdsay ..\obj2
    wmake /h /i
    cdsay ..\objf
    wmake /h /i
    cdsay ..\objnt
    wmake /h /i
    cdsay ..\..\win\obj
    wmake /h /i
    cdsay ..\objpmf
    wmake /h /i
    cdsay ..\objpm
    wmake /h /i
    cdsay ..\objnt
    wmake /h /i
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================

[ BLOCK <1> clean ]
#==================
    sweep killobjs
