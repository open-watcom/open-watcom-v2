# WPROF Builder Control file
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
    <CPCMD> <devdir>\wprof\pm\wprof.exe <relroot>\rel2\binp\wprof.exe
    <CPCMD> <devdir>\wprof\win\wprof.exe <relroot>\rel2\binw\wprofw.exe
    <CPCMD> <devdir>\wprof\dos\wprof.exe <relroot>\rel2\binw\wprof.exe
    <CPCMD> <devdir>\wprof\nec\wprofnec.exe <relroot>\rel2\n98bw\wprof.exe
    <CPCMD> <devdir>\wprof\nt\wprof.exe <relroot>\rel2\binnt\wprof.exe
#    <CPCMD> <devdir>\wprof\axp\wprof.exe <relroot>\rel2\axpnt\wprof.exe
    <CPCMD> <devdir>\wprof\qnx\wprof.qnx <relroot>\rel2\qnx\binq\wprof.
    <CPCMD> <devdir>\wprof\qnx\wprof.sym <relroot>\rel2\qnx\sym\wprof.sym

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
