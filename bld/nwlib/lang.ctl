# NWLIB Builder Control file
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
    <CPCMD> osi.x86\wlib.exe <relroot>\rel2\binw\wlib.exe
#    <CPCMD> nt.axp\wlib.exe <relroot>\rel2\axpnt\wlib.exe
#    <CPCMD> nt.axp\wlibd.dll <relroot>\rel2\axpnt\wlibd.dll
    <CPCMD> os2.x86\wlib.exe <relroot>\rel2\binp\wlib.exe
    <CPCMD> os2.x86\wlibd.dll <relroot>\rel2\binp\dll\wlibd.dll
    <CPCMD> nt.x86\wlib.exe <relroot>\rel2\binnt\wlib.exe
    <CPCMD> nt.x86\wlibd.dll <relroot>\rel2\binnt\wlibd.dll
    <CPCMD> qnx.x86\wlib.exe <relroot>\rel2\qnx\wlib.
    <CPCMD> qnx.x86\wlib.sym <relroot>\rel2\qnx\wlib.sym
#    <CPCMD> dos.x86\wlib.exe <relroot>\rel2\binw\wlib.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
