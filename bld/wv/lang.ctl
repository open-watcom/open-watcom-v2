# WATCOM Debugger Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cdsay <projdir>
#    ECHO.
#    ECHO RFX make suppressed - Literal string problems
#    ECHO.
#    #CDSAY ..\rfx\dos
#    #wmake /h /i w
#    #CDSAY ..\os2
#    #wmake /h /i w

[ BLOCK <1> rel2 cprel2 ]
#========================
##    <CPCMD> <devdir>\bin\bin\rfx.exe <relroot>\rel2\binw\rfx.exe
##    <CPCMD> <devdir>\bin\binp\rfx.exe <relroot>\rel2\binp\rfx.exe

    <CPCMD> <devdir>\bin\bin.nec\dv.exe <relroot>\rel2\n98bw\wd.exe

    <CPCMD> <devdir>\bin\bin\dv.exe <relroot>\rel2\binw\wd.exe
    <CPCMD> <devdir>\bin\bin\dv.sym <relroot>\rel2\binw\wd.sym

    <CPCMD> <devdir>\bin\binp\dv.exe <relroot>\rel2\binp\wd.exe
    <CPCMD> <devdir>\bin\binp\dvw.exe <relroot>\rel2\binp\wdw.exe
    <CPCMD> <devdir>\bin\binp\dv.sym <relroot>\rel2\binp\wd.sym
    <CPCMD> <devdir>\bin\binp\dvw.sym <relroot>\rel2\binp\wdw.sym

    <CPCMD> <devdir>\bin\bin.nec\dvc.exe <relroot>\rel2\n98bw\wdc.exe
    <CPCMD> <devdir>\bin\bin.nec\dvc.sym <relroot>\rel2\n98bw\wdc.sym

    <CPCMD> <devdir>\bin\bin\dvc.exe <relroot>\rel2\binw\wdc.exe
    <CPCMD> <devdir>\bin\bin\dvw.exe <relroot>\rel2\binw\wdw.exe
    <CPCMD> <devdir>\bin\bin\dvc.sym <relroot>\rel2\binw\wdc.sym
    <CPCMD> <devdir>\bin\bin\dvw.sym <relroot>\rel2\binw\wdw.sym

    <CPCMD> <devdir>\bin\binnt\dv.exe <relroot>\rel2\binnt\wd.exe
    <CPCMD> <devdir>\bin\binnt\dvw.exe <relroot>\rel2\binnt\wdw.exe
    <CPCMD> <devdir>\bin\binnt\dv.sym <relroot>\rel2\binnt\wd.sym
    <CPCMD> <devdir>\bin\binnt\dvw.sym <relroot>\rel2\binnt\wdw.sym

#    <CPCMD> <devdir>\bin\axpnt\dv.exe <relroot>\rel2\axpnt\wd.exe
#    <CPCMD> <devdir>\bin\axpnt\dvw.exe <relroot>\rel2\axpnt\wdw.exe
#    <CPCMD> <devdir>\bin\axpnt\dv.sym <relroot>\rel2\axpnt\wd.sym
#    <CPCMD> <devdir>\bin\axpnt\dvw.sym <relroot>\rel2\axpnt\wdw.sym

    <CPCMD> <devdir>\bin\qnx\dv.sym <relroot>\rel2\qnx\sym\wd.sym
    <CPCMD> <devdir>\bin\qnx\dv.qnx <relroot>\rel2\qnx\binq\wd

    <CPCMD> <devdir>\wv\dbg\*.dbg <relroot>\rel2\binw\
    <CPCMD> <devdir>\wv\ssl\*.prs <relroot>\rel2\binw\

    <CPCMD> <devdir>\wv\dbg\*.dbg <relroot>\rel2\qnx\watcom\wd\
    <CPCMD> <devdir>\wv\ssl\*.prs <relroot>\rel2\qnx\watcom\wd\


[ BLOCK <1> clean ]
#==================
    sweep killobjs
