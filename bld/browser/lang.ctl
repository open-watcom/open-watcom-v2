# Browser Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]

cdsay <projdir>\contain\lib\objwin
wmake -i -h
cdsay <projdir>\contain\lib\obj32
wmake -i -h
cdsay <projdir>\dlgprs\o
wmake -i -h
cdsay <projdir>\gen
wmake -i -h
cd <projdir>
pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\browser\winprod\wbrw.exe <relroot>\rel2\binw\wbrw.exe
    <CPCMD> <devdir>\browser\nt\wbrw.exe <relroot>\rel2\binnt\wbrw.exe
#    <CPCMD> <devdir>\browser\axp\wbrw.exe <relroot>\rel2\axpnt\wbrw.exe
    <CPCMD> <devdir>\browser\os2\wbrw.exe <relroot>\rel2\binp\wbrw.exe
    <CPCMD> <devdir>\browser\brg\objosi\wbrg.exe <relroot>\rel2\binw\wbrg.exe
    <CPCMD> <devdir>\browser\brg\objnt\wbrg.exe <relroot>\rel2\binnt\wbrg.exe
#    <CPCMD> <devdir>\browser\brg\objaxp\wbrg.exe <relroot>\rel2\axpnt\wbrg.exe

[ BLOCK <1> clean ]
#==================
    #pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    sweep killobjs
