# ZOOM Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 ]
    <CPCMD> <devdir>\sdk\zoom\wini86\wzoom.exe <relroot>\rel2\binw\wzoom.exe
    <CPCMD> <devdir>\sdk\zoom\nt386\wzoom.exe <relroot>\rel2\binnt\wzoom.exe
#    <CPCMD> <devdir>\sdk\zoom\ntaxp\wzoom.exe <relroot>\rel2\axpnt\wzoom.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
