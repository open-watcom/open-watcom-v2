# WTOUCH Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cdsay .

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\wtouch\o\wtouch.exe <relroot>\rel2\binw\wtouch.exe
    <CPCMD> <devdir>\wtouch\os2\wtouch.exe <relroot>\rel2\binp\wtouch.exe
    <CPCMD> <devdir>\wtouch\nt.386\wtouch.exe <relroot>\rel2\binnt\wtouch.exe
#    <CPCMD> <devdir>\wtouch\nt.axp\wtouch.exe <relroot>\rel2\axpnt\wtouch.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
