# Win386 Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=win386

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> ext/win386.ext              <OWRELROOT>/binw/
    <CCCMD> dll/w386dll.ext             <OWRELROOT>/binw/
    <CCCMD> conv/win386/win386.lib      <OWRELROOT>/lib386/win/
    <CCCMD> vxd/wdebug.386              <OWRELROOT>/binw/
    <CCCMD> vxd/wemu387.386             <OWRELROOT>/binw/

    <CCCMD> wbind/dosi86/wbind.exe      <OWRELROOT>/binw/
    <CCCMD> wbind/os2i86/wbind.exe      <OWRELROOT>/binp/
    <CCCMD> wbind/nt386/wbind.exe       <OWRELROOT>/binnt/
    <CCCMD> wbind/linux386/wbind.exe    <OWRELROOT>/binl/wbind

    <CCCMD> wbind/ntx64/wbind.exe       <OWRELROOT>/binnt64/
    <CCCMD> wbind/linuxx64/wbind.exe    <OWRELROOT>/binl64/wbind

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
