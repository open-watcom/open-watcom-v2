# WSTRIP Builder Control file
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
    <CPCMD> <devdir>\wstrip\dos\strip.exe <relroot>\rel2\binw\wstrip.exe
    <CPCMD> <devdir>\wstrip\nt\strip.exe <relroot>\rel2\binnt\wstrip.exe
#    <CPCMD> <devdir>\wstrip\ntaxp\strip.exe <relroot>\rel2\axpnt\wstrip.exe
    <CPCMD> <devdir>\wstrip\qnx\strip.exe <relroot>\rel2\qnx\binq\wstrip.

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
