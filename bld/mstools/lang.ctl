# MSTOOLS Builder Control file
# ============================
set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
#    <CPCMD> <devdir>\mstools\asaxp\build\nt.386\asaxp.exe <relroot>\rel2\binnt\asaxp.exe
#    <CPCMD> <devdir>\mstools\asaxp\build\nt.axp\asaxp.exe <relroot>\rel2\axpnt\asaxp.exe
    <CPCMD> <devdir>\mstools\cl\build\nt.386\cl.exe <relroot>\rel2\binnt\cl.exe
    <CPCMD> <devdir>\mstools\cl\build\nt.386\cl386.exe <relroot>\rel2\binnt\cl386.exe
#    <CPCMD> <devdir>\mstools\cl\build\nt.386\claxp.exe <relroot>\rel2\binnt\claxp.exe
#    <CPCMD> <devdir>\mstools\cl\build\nt.axp\cl.exe <relroot>\rel2\axpnt\cl.exe
#    <CPCMD> <devdir>\mstools\cl\build\nt.axp\cl386.exe <relroot>\rel2\axpnt\cl386.exe
#    <CPCMD> <devdir>\mstools\cl\build\nt.axp\claxp.exe <relroot>\rel2\axpnt\claxp.exe
    <CPCMD> <devdir>\mstools\cvtres\build\nt.386\cvtres.exe <relroot>\rel2\binnt\cvtres.exe
#    <CPCMD> <devdir>\mstools\cvtres\build\nt.axp\cvtres.exe <relroot>\rel2\axpnt\cvtres.exe
    <CPCMD> <devdir>\mstools\lib\build\nt.386\lib.exe <relroot>\rel2\binnt\lib.exe
    <CPCMD> <devdir>\mstools\lib\build\nt.386\lib386.exe <relroot>\rel2\binnt\lib386.exe
#    <CPCMD> <devdir>\mstools\lib\build\nt.386\libaxp.exe <relroot>\rel2\binnt\libaxp.exe
#    <CPCMD> <devdir>\mstools\lib\build\nt.axp\lib.exe <relroot>\rel2\axpnt\lib.exe
#    <CPCMD> <devdir>\mstools\lib\build\nt.axp\lib386.exe <relroot>\rel2\axpnt\lib386.exe
#    <CPCMD> <devdir>\mstools\lib\build\nt.axp\libaxp.exe <relroot>\rel2\axpnt\libaxp.exe
    <CPCMD> <devdir>\mstools\link\build\nt.386\link.exe <relroot>\rel2\binnt\link.exe
    <CPCMD> <devdir>\mstools\link\build\nt.386\link386.exe <relroot>\rel2\binnt\link386.exe
#    <CPCMD> <devdir>\mstools\link\build\nt.386\linkaxp.exe <relroot>\rel2\binnt\linkaxp.exe
#    <CPCMD> <devdir>\mstools\link\build\nt.axp\link.exe <relroot>\rel2\axpnt\link.exe
#    <CPCMD> <devdir>\mstools\link\build\nt.axp\link386.exe <relroot>\rel2\axpnt\link386.exe
#    <CPCMD> <devdir>\mstools\link\build\nt.axp\linkaxp.exe <relroot>\rel2\axpnt\linkaxp.exe
    <CPCMD> <devdir>\mstools\nmake\build\nt.386\nmake.exe <relroot>\rel2\binnt\nmake.exe
#    <CPCMD> <devdir>\mstools\nmake\build\nt.axp\nmake.exe <relroot>\rel2\axpnt\nmake.exe
    <CPCMD> <devdir>\mstools\rc\build\nt.386\rc.exe <relroot>\rel2\binnt\rc.exe
#    <CPCMD> <devdir>\mstools\rc\build\nt.axp\rc.exe <relroot>\rel2\axpnt\rc.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

