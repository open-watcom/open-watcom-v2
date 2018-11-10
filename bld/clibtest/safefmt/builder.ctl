# clibtest Builder Control file
# =============================

set PROJNAME=clibtest

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

[ BLOCK <BLDRULE> test ]
#=======================
    cdsay .
    wmake -h

[ BLOCK <BLDRULE> testclean ]
#============================
    cdsay .
    wmake -h clean

[ BLOCK . . ]

cdsay .
