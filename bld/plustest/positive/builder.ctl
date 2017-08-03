# plustest Builder Control file
# =============================

set PROJNAME=plustest

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

cdsay .

[ BLOCK <1> test ]
#=================
    wmake -h test0=1
    wmake -h test1=1
    wmake -h test2=1
    wmake -h test3=1
    wmake -h test4=1
    wmake -h test5=1
    wmake -h test6=1
    wmake -h test7=1
    wmake -h test8=1
    wmake -h test9=1
    wmake -h test10=1
    wmake -h test11=1
[ IFDEF <EXTRA_ARCH> i86 386 axp ppc mps ]
    wmake -h test0=1 arch=<EXTRA_ARCH>
    wmake -h test1=1 arch=<EXTRA_ARCH>
    wmake -h test2=1 arch=<EXTRA_ARCH>
    wmake -h test3=1 arch=<EXTRA_ARCH>
    wmake -h test4=1 arch=<EXTRA_ARCH>
    wmake -h test5=1 arch=<EXTRA_ARCH>
    wmake -h test6=1 arch=<EXTRA_ARCH>
    wmake -h test7=1 arch=<EXTRA_ARCH>
    wmake -h test8=1 arch=<EXTRA_ARCH>
    wmake -h test9=1 arch=<EXTRA_ARCH>
    wmake -h test10=1 arch=<EXTRA_ARCH>
    wmake -h test11=1 arch=<EXTRA_ARCH>
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
