# WTOUCH Builder Control file
# ===========================

set PROJNAME=wtouch

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dosi86/wtouch.exe    <OWRELROOT>/binw/
    <CCCMD> os2386/wtouch.exe    <OWRELROOT>/binp/
    <CCCMD> nt386/wtouch.exe     <OWRELROOT>/binnt/
    <CCCMD> linux386/wtouch.exe  <OWRELROOT>/binl/wtouch
    <CCCMD> ntaxp/wtouch.exe     <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wtouch.exe     <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wtouch.exe  <OWRELROOT>/binl64/wtouch
    <CCCMD> linuxarm/wtouch.exe  <OWRELROOT>/arml/wtouch
    <CCCMD> osxx64/wtouch.exe    <OWRELROOT>/osx64/wtouch

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
