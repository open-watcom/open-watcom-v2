# plustest Builder Control file
# =============================

set PROJNAME=plustest

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

[ BLOCK <BLDRULE> test ]
#=======================
    cdsay .
    wmake -h test0=1
    cdsay .
    wmake -h test1=1
[ IFDEF <EXTRA_ARCH> i86 386 axp ppc mps ]
    cdsay .
    wmake -h test0=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test1=1 arch=<EXTRA_ARCH>
[ ENDIF ]

[ BLOCK <BLDRULE> testclean ]
#============================
    cdsay .
    wmake -h clean

[ BLOCK . . ]

cdsay .
