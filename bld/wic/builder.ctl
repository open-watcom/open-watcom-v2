# WIC Builder Control file
# ========================

set PROJNAME=wic

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#========================
    <CCCMD> dos386/wic.exe <OWRELROOT>/binw/wic.exe
    <CCCMD> os2386/wic.exe <OWRELROOT>/binp/wic.exe
    <CCCMD> nt386/wic.exe  <OWRELROOT>/binnt/wic.exe

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
