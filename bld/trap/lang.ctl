# Debugger Trap Files Control file
# ================================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 ]
#========================
#dos
    <CPCMD> <devdir>\bin\bin\???.trp <relroot>\rel2\binw\
    <CPCMD> <devdir>\bin\bin\???serv.exe <relroot>\rel2\binw\
    <CPCMD> <devdir>\bin\bin\???help.ex? <relroot>\rel2\binw\
#nec98
    <CPCMD> <devdir>\bin\bin.nec\???.trp <relroot>\rel2\n98bw\
    <CPCMD> <devdir>\bin\bin.nec\???serv.exe <relroot>\rel2\n98bw\
    <CPCMD> <devdir>\bin\bin.nec\???help.ex? <relroot>\rel2\n98bw\
#os2
    <CPCMD> <devdir>\bin\binp\dll\???.dll <relroot>\rel2\binp\dll\
    <CPCMD> <devdir>\bin\binp\dll\std??.dll <relroot>\rel2\binp\dll\
    <CPCMD> <devdir>\bin\binp\dll\*.exe <relroot>\rel2\binp\dll\
    <CPCMD> <devdir>\bin\binp\dll\wdpmhook.dll <relroot>\rel2\binp\dll\
    <CPCMD> <devdir>\bin\binp\???serv.exe <relroot>\rel2\binp\
    <CPCMD> <devdir>\bin\binp\wdpmhelp.exe <relroot>\rel2\binp\
    <CPCMD> <devdir>\bin\binp\nmpbind.exe <relroot>\rel2\binp\
#windows
    <CPCMD> <devdir>\bin\bin\???.dll <relroot>\rel2\binw\
    <CPCMD> <devdir>\bin\bin\wint32.dll <relroot>\rel2\binw\
    <CPCMD> <devdir>\bin\bin\???servw.exe <relroot>\rel2\binw\
#nt
    <CPCMD> <devdir>\bin\binnt\???.dll <relroot>\rel2\binnt\
    <CPCMD> <devdir>\bin\binnt\???serv.exe <relroot>\rel2\binnt\
#axp
#    <CPCMD> <devdir>\bin\axpnt\???.dll <relroot>\rel2\axpnt\
#    <CPCMD> <devdir>\bin\axpnt\???serv.exe <relroot>\rel2\axpnt\
#nlm
    <CPCMD> <devdir>\bin\nlm\*.nlm <relroot>\rel2\nlm\
#qnx
    <CPCMD> <devdir>\bin\qnx\*.trp <relroot>\rel2\qnx\watcom\wd\
    <CPCMD> <devdir>\bin\qnx\parserv.qnx <relroot>\rel2\qnx\binq\parserv
    <CPCMD> <devdir>\bin\qnx\serserv.qnx <relroot>\rel2\qnx\binq\serserv
    <CPCMD> <devdir>\bin\qnx\tcpserv.qnx <relroot>\rel2\qnx\binq\tcpserv
# NT parallel port device driver and installer
    <CPCMD> <devdir>\trap\par\ntsupp\dbgport.sys <relroot>\rel2\binnt\
    <CPCMD> <devdir>\trap\par\ntsupp\dbginst.exe <relroot>\rel2\binnt\

[ BLOCK <1> clean ]
#==================
    sweep killobjs
