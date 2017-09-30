# ctest Builder Control file
# =============================

set PROJNAME=ctest

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

[ BLOCK <1> test ]
#=================
    cdsay .
    wmake -h
[ IFDEF <EXTRA_ARCH> i86 386 axp ppc mps ]
    cdsay .
    wmake -h arch=<EXTRA_ARCH>
[ ENDIF ]

[ BLOCK <1> testclean ]
#======================
    cdsay .
    wmake -h clean

[ BLOCK . . ]

cdsay .
