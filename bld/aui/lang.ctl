# DUI Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay olo
    wmake /h /i
    cdsay ..\of
    wmake /h /i
    cdsay ..\nt
    wmake /h /i
#    cdsay ..\ntaxp
#    wmake /h /i
    cdsay ..\win
    wmake /h /i
    cdsay ..\ol
    wmake /h /i
    cdsay ..\pm
    wmake /h /i
    cdsay ..\o32s
    wmake /h /i
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================

[ BLOCK <1> clean ]
#==================
    cdsay olo
    wmake /h /i clean
    cdsay ..\of
    wmake /h /i clean
    cdsay ..\nt
    wmake /h /i clean
#    cdsay ..\ntaxp
#    wmake /h /i clean
    cdsay ..\win
    wmake /h /i clean
    cdsay ..\ol
    wmake /h /i clean
    cdsay ..\pm
    wmake /h /i clean
    cdsay ..\o32s
    wmake /h /i clean
    cd <PROJDIR>
