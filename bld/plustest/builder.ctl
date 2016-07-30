# plustest Builder Control file
# =============================

set PROJNAME=plustest

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ BLOCK <1> test ]
#=================
    wmake -h

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
