# w32ldr Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    wmake -h

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
    <CPCMD> os2ldr.exe <tooldr>\tools\os2ldr.exe
    <CPCMD> x32run.exe <relroot>\rel2\binw\x32run.exe
    <CPCMD> x32run.exe <relroot>\rel2\binw\w32run.exe
    <CPCMD> d4grun.exe <relroot>\rel2\binw\d4grun.exe
    <CPCMD> tntrun.exe <relroot>\rel2\binw\tntrun.exe
    <CPCMD> ntrunner.exe <relroot>\rel2\binnt\w32run.exe
#    these are now real NT exe's - don't copy the stub
#    <CPCMD> nt\*.exe <relroot>\rel2\binnt\

[ BLOCK <1> clean ]
#==================
    wmake -h clean
