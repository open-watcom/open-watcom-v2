# PLUS Builder Control file
# =========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> reference ]
    cdsay <PROJDIR>\c
    cdsay ..\nti86
    wmake /h /k
    wmake /h /k reference
    cdsay ..\nt386
    wmake /h /k
    wmake /h /k reference
    checkall .

[ BLOCK <1> referenceos2 ]
    cdsay <PROJDIR>\c
    cdsay ..\os2i86
    wmake /h /k
    wmake /h /k reference
    cdsay ..\os2386
    wmake /h /k
    wmake /h /k reference
    checkall .

[ BLOCK <1> build rel2 ]
    cdsay <PROJDIR>\c
    cdsay ..\nti86
    wmake /h /k
    cdsay ..\nt386
    wmake /h /k
    cdsay ..\nt386dll
    wmake /h /k
#    MEP - 3/27/00 just make NT for now
    cdsay ..\osii86
    wmake /h /k
    cdsay ..\osi386
    wmake /h /k
    cdsay ..\os2i86
    wmake /h /k
    cdsay ..\os2386
    wmake /h /k
    cdsay ..\qnxi86
    wmake /h /k
    cdsay ..\qnx386
    wmake /h /k
    cd ..

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
#
# 386 NT hosted compilers
#
#  i86 target
    <CPCMD> nti86\wcppi86.exe <relroot>\rel2\binnt\wpp.exe
    <CPCMD> nti86\wppdi86.dll <relroot>\rel2\binnt\wppdi86.dll
    <CPCMD> nti86\wppdi86.sym <relroot>\rel2\binnt\wppdi86.sym
    <CPCMD> nti86\wppi8601.int <relroot>\rel2\binnt\wppi8601.int
#  386 target
    <CPCMD> nt386\wcpp386.exe <relroot>\rel2\binnt\wpp386.exe
    <CPCMD> nt386\wppd386.dll <relroot>\rel2\binnt\wppd386.dll
    <CPCMD> nt386\wppd386.sym <relroot>\rel2\binnt\wppd386.sym
    <CPCMD> nt386\wpp38601.int <relroot>\rel2\binnt\wpp38601.int

#  Optima 386 target (with -br switch)
    <CPCMD> nt386dll\wcpp386.exe <relroot>\rel2\binnt\rtdll\wpp386.exe
    <CPCMD> nt386dll\wppd386.dll <relroot>\rel2\binnt\rtdll\wppd386.dll
    <CPCMD> nt386dll\wppd386.sym <relroot>\rel2\binnt\rtdll\wppd386.sym
    <CPCMD> nt386dll\wpp38601.int <relroot>\rel2\binnt\rtdll\wpp38601.int

#
# 386 OS/2 hosted compilers
#
#  i86 target
    <CPCMD> os2i86\wcppi86.exe <relroot>\rel2\binp\wpp.exe
    <CPCMD> os2i86\wppdi86.dll <relroot>\rel2\binp\dll\wppdi86.dll
    <CPCMD> os2i86\wppdi86.sym <relroot>\rel2\binp\dll\wppdi86.sym
    <CPCMD> os2i86\wppi8601.int <relroot>\rel2\binp\dll\wppi8601.int
#  386 target
    <CPCMD> os2386\wcpp386.exe <relroot>\rel2\binp\wpp386.exe
    <CPCMD> os2386\wppd386.dll <relroot>\rel2\binp\dll\wppd386.dll
    <CPCMD> os2386\wppd386.sym <relroot>\rel2\binp\dll\wppd386.sym
    <CPCMD> os2386\wpp38601.int <relroot>\rel2\binp\dll\wpp38601.int
#
# 386 OSI hosted compilers
#
#  i86 target
    <CPCMD> osii86\wcppi86.exe <relroot>\rel2\binw\wpp.exe
    <CPCMD> osii86\wcppi86.sym <relroot>\rel2\binw\wpp.sym
    <CPCMD> osii86\wppi8601.int <relroot>\rel2\binw\wppi8601.int
#  386 target
    <CPCMD> osi386\wcpp386.exe <relroot>\rel2\binw\wpp386.exe
    <CPCMD> osi386\wcpp386.sym <relroot>\rel2\binw\wpp386.sym
    <CPCMD> osi386\wpp38601.int <relroot>\rel2\binw\wpp38601.int
#
# 386 QNX hosted compilers
#
#  i86 target
    <CPCMD> qnxi86\wcppi86.qnx <relroot>\rel2\qnx\binq\wpp.
    <CPCMD> qnxi86\wcppi86.sym <relroot>\rel2\qnx\sym\wpp.sym
    <CPCMD> qnxi86\wppi8601.int <relroot>\rel2\qnx\sym\wppi8601.int
#  386 target
    <CPCMD> qnx386\wcpp386.qnx <relroot>\rel2\qnx\binq\wpp386.
    <CPCMD> qnx386\wcpp386.sym <relroot>\rel2\qnx\sym\wpp386.sym
    <CPCMD> qnx386\wpp38601.int <relroot>\rel2\qnx\sym\wpp38601.int

[ BLOCK <1> clean ]
#==================
    cdsay <PROJDIR>\c
    cdsay ..\nti86
    wmake /h /k clean
    cdsay ..\nt386
    wmake /h /k clean
    cdsay ..\nt386dll
    wmake /h /k clean
    cdsay ..\osii86
    wmake /h /k clean
    cdsay ..\osi386
    wmake /h /k clean
    cdsay ..\os2i86
    wmake /h /k clean
    cdsay ..\os2386
    wmake /h /k clean
    cdsay ..\qnxi86
    wmake /h /k clean
    cdsay ..\qnx386
    wmake /h /k clean
    cd ..
