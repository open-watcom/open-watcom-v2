# WDISASM Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 ]
#========================
<CPCMD> <projdir>\STAND\qnx.x86\wdis.qnx <relroot>\rel2\qnx\binq\wdis
<CPCMD> <projdir>\STAND\NT.X86\wdis.exe <relroot>\rel2\binnt\
<CPCMD> <projdir>\STAND\OS2.X86\wdis.exe <relroot>\rel2\binp\
<CPCMD> <projdir>\STAND\osi.x86\wdis.exe <relroot>\rel2\binw\
#<CPCMD> <projdir>\STAND\NT.axp\wdis.exe <relroot>\rel2\axpnt\

<CPCMD> <projdir>\STAND\qnx.x86\wdis.sym <relroot>\rel2\qnx\sym\
<CPCMD> <projdir>\STAND\NT.X86\wdis.sym <relroot>\rel2\binnt\
<CPCMD> <projdir>\STAND\OS2.X86\wdis.sym <relroot>\rel2\binp\
<CPCMD> <projdir>\STAND\osi.x86\wdis.sym <relroot>\rel2\binw\
#<CPCMD> <projdir>\STAND\NT.axp\wdis.sym <relroot>\rel2\axpnt\

[ BLOCK <1> clean ]
#==================
pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
