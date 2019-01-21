# HEAPWALKER Builder Control file
# ============================

set PROJNAME=whepwlk

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> wini86/wheapwlk.exe <OWRELROOT>/binw/wheapwlk.exe

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
