# WDE Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\sdk\wde\win16\wde.exe      <relroot>\rel2\binw\wde.exe
    <CPCMD> <devdir>\sdk\wde\nt\wde.exe         <relroot>\rel2\binnt\wde.exe
#    <CPCMD> <devdir>\sdk\wde\axp\wde.exe       <relroot>\rel2\axpnt\wde.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
