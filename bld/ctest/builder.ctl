# ctest Builder Control file
# ===========================

set PROJNAME=ctest

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ BLOCK <1> test ]
#=================
    wmake -h

[ BLOCK <1> testclean ]
#======================
    wmake -h clean

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
