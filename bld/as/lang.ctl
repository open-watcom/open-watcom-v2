# asaxp Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 ]
#========================
#<CPCMD> <devdir>\as\alpha\os2386\wasaxp.exe <relroot>\rel2\binp\wasaxp.exe
#<CPCMD> <devdir>\as\alpha\nt386\wasaxp.exe <relroot>\rel2\binnt\wasaxp.exe
#<CPCMD> <devdir>\as\alpha\ntaxp\wasaxp.exe <relroot>\rel2\axpnt\wasaxp.exe
#<CPCMD> <devdir>\as\alpha\dos386\wasaxp.exe <relroot>\rel2\binw\wasaxp.exe

[ BLOCK <1> clean ]
#==================
    pmake -d all -h clean
