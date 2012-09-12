# Win386 Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=win386

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> ext/win386.ext              <OWRELROOT>/binw/win386.ext
    <CCCMD> dll/w386dll.ext             <OWRELROOT>/binw/w386dll.ext
    <CCCMD> conv/win386/win386.lib      <OWRELROOT>/lib386/win/win386.lib
    <CCCMD> vxd/wdebug.386              <OWRELROOT>/binw/wdebug.386
    <CCCMD> vxd/wemu387.386             <OWRELROOT>/binw/wemu387.386

    <CCCMD> wbind/dosi86/wbind.exe      <OWRELROOT>/binw/wbind.exe

    <CCCMD> wbind/os2i86/wbind.exe      <OWRELROOT>/binp/wbind.exe

    <CCCMD> wbind/nt386/wbind.exe       <OWRELROOT>/binnt/wbind.exe

    <CCCMD> wbind/linux386/wbind.exe    <OWRELROOT>/binl/wbind


[ BLOCK <1> clean ]
#==================
    @rm -f asm/dllthk.asm
    @rm -f c/dllthunk.c
    @rm -f asm/winglue.asm
    @rm -f h/winglue.inc
    @rm -f ext/win386.ext
    @rm -f dll/w386dll.ext
    @rm -f vxd/wdebug.386
    @rm -f vxd/wemu387.386

[ BLOCK . . ]
#============
cdsay <PROJDIR>
