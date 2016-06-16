# HEAPWALKER Builder Control file
# ============================

set PROJNAME=whepwlk

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/wheapwlk.exe <OWRELROOT>/binw/wheapwlk.exe

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
