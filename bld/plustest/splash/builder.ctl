# plustest Builder Control file
# =============================

set PROJNAME=plustest

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

cdsay .

[ BLOCK <1> test ]
#=================
    wmake -h test0=1
    cdsay .
    wmake -h test1=1
    cdsay .
    wmake -h test2=1
[ IFDEF <EXTRA_ARCH> i86 386 axp ppc mps ]
    cdsay .
    wmake -h test0=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test1=1 arch=<EXTRA_ARCH>
    cdsay .
    wmake -h test2=1 arch=<EXTRA_ARCH>
[ ENDIF ]

[ BLOCK <1> testclean ]
#======================
    wmake -h clean

[ BLOCK <1> cleanlog ]
#======================
    cdsay <PROJDIR>
    echo rm -f *.log
    rm -f *.log

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
