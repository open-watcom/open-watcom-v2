# cvpack Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\cvpack\qnx\cvpack.exe <relroot>\rel2\qnx\binq\cvpack
    <CPCMD> <devdir>\cvpack\qnx\cvpack.sym <relroot>\rel2\qnx\sym\cvpack.sym
    <CPCMD> <devdir>\cvpack\dos386\cvpack.exe <relroot>\rel2\binw\cvpack.exe
    <CPCMD> <devdir>\cvpack\os2\cvpack.exe <relroot>\rel2\binp\cvpack.exe
    <CPCMD> <devdir>\cvpack\nt386\cvpack.exe <relroot>\rel2\binnt\cvpack.exe
#    <CPCMD> <devdir>\cvpack\ntaxp\cvpack.exe <relroot>\rel2\axpnt\cvpack.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK <1> relink ]
#===================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h killnonobj
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
