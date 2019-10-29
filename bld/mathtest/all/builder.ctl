# mathtest Builder Control file
# =============================

set PROJNAME=mathtest

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

[ BLOCK <BLDRULE> test ]
#=======================
    cdsay .
    wmake -h test0=1
    cdsay .
    wmake -h test1=1
    cdsay .
    wmake -h test2=1
    cdsay .
    wmake -h test3=1
[ IFDEF <EXTRA_ARCH> 386 axp ppc mps ]
    cdsay .
    wmake -h test0=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test1=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test2=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test3=1 arch=<EXTRA_ARCH>
[ ENDIF ]
[ IFDEF <EXTRA_ARCH> i86 ]
    cdsay .
    wmake -h test0=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test1=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test2=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test3=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test4=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test5=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test6=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test7=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test8=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test9=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test10=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test11=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test12=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test13=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test14=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test15=1 arch=<EXTRA_ARCH>
[ ENDIF ]

[ BLOCK <BLDRULE> testclean ]
#============================
    cdsay .
    wmake -h clean

[ BLOCK . . ]

cdsay .
