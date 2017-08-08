# clibtest Builder Control file
# =============================

set PROJNAME=clibtest

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

[ BLOCK <1> test ]
#=================
    cdsay .
    wmake -h

[ BLOCK <1> testclean ]
#======================
    cdsay .
    wmake -h clean

[ BLOCK . . ]

cdsay .
