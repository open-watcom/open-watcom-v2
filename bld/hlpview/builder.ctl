# whelp Builder Control file
# ===========================

set PROJNAME=whelp

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/whelp.exe    <OWRELROOT>/binw/whelp.exe
    <CCCMD> os2386/whelp.exe    <OWRELROOT>/binp/whelp.exe
    <CCCMD> linux386/whelp.exe  <OWRELROOT>/binl/whelp

    <CCCMD> linuxx64/whelp.exe  <OWRELROOT>/binl64/whelp
    
[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
