# Win386 Builder Control file
# ===========================

set PROJNAME=win386

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
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

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
