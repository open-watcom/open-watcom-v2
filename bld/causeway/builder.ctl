# CauseWay Builder Control file
# =============================

set PROJNAME=causeway

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> cwdll/dos386/cwdll.lib  <OWRELROOT>/lib386/dos/
    <CCCMD> inc/cwdll.h             <OWRELROOT>/h/

    <CCCMD> cw32/dos386/*.exe       <OWRELROOT>/binw/
    <CCCMD> cw32/dos386/*.exe       <OWRELROOT>/binl/

    <CCCMD> cwc/dosi86/cwc.exe      <OWRELROOT>/binw/
    <CCCMD> cwc/nt386/cwc.exe       <OWRELROOT>/binnt/
    <CCCMD> cwc/os2386/cwc.exe      <OWRELROOT>/binp/
    <CCCMD> cwc/linux386/cwc.exe    <OWRELROOT>/binl/cwc

    <CCCMD> cwc/ntx64/cwc.exe       <OWRELROOT>/binnt64/
    <CCCMD> cwc/linuxx64/cwc.exe    <OWRELROOT>/binl64/cwc

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
