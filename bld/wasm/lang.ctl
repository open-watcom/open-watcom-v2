# WASM Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay h
    wmake /h
    cd <PROJDIR>
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
    <CPCMD> <devdir>\wasm\nt.x86\wasm.exe <relroot>\rel2\binnt\wasm.exe
    <CPCMD> <devdir>\wasm\nt.x86\wasm.sym <relroot>\rel2\binnt\wasm.sym
#    <CPCMD> <devdir>\wasm\nt.axp\wasm.exe <relroot>\rel2\axpnt\wasm.exe
#    <CPCMD> <devdir>\wasm\nt.axp\wasm.sym <relroot>\rel2\axpnt\wasm.sym
    <CPCMD> <devdir>\wasm\os2.x86\wasm.exe <relroot>\rel2\binp\wasm.exe
    <CPCMD> <devdir>\wasm\os2.x86\wasm.sym <relroot>\rel2\binp\wasm.sym
    <CPCMD> <devdir>\wasm\osi.x86\wasm.exe <relroot>\rel2\binw\wasm.exe
    <CPCMD> <devdir>\wasm\osi.x86\wasm.sym <relroot>\rel2\binw\wasm.sym
    <CPCMD> <devdir>\wasm\qnx.x86\wasm.qnx <relroot>\rel2\qnx\binq\wasm.
    <CPCMD> <devdir>\wasm\qnx.x86\wasm.sym <relroot>\rel2\qnx\sym\

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
