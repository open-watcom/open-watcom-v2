# WTOUCH Builder Control file
# ===========================

set PROJNAME=wtouch

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/deftool.ctl" ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay "<PROJDIR>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dosi86/wtouch.exe    "<OWRELROOT>/binw/"
    <CCCMD> os2386/wtouch.exe    "<OWRELROOT>/binp/"
    <CCCMD> nt386/wtouch.exe     "<OWRELROOT>/binnt/"
    <CCCMD> linux386/wtouch.exe  "<OWRELROOT>/binl/wtouch"
    <CCCMD> ntaxp/wtouch.exe     "<OWRELROOT>/axpnt/"

    <CCCMD> bsdx64/wtouch.exe    "<OWRELROOT>/binb64/wtouch"
    <CCCMD> ntx64/wtouch.exe     "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/wtouch.exe  "<OWRELROOT>/binl64/wtouch"
    <CCCMD> linuxarm/wtouch.exe  "<OWRELROOT>/arml/wtouch"
    <CCCMD> linuxa64/wtouch.exe  "<OWRELROOT>/arml64/wtouch"
    <CCCMD> osxx64/wtouch.exe    "<OWRELROOT>/bino64/wtouch"
    <CCCMD> osxarm/wtouch.exe    "<OWRELROOT>/armo/wtouch"
    <CCCMD> osxa64/wtouch.exe    "<OWRELROOT>/armo64/wtouch"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
