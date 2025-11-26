# ftest Builder Control file
# =============================

set PROJNAME=ftest

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ BLOCK <BLDRULE> test ]
#=======================
    set TESTARG=test0=1
[ INCLUDE "<OWROOT>/build/deftest.ctl" ]
    set TESTARG=test1=1
[ INCLUDE "<OWROOT>/build/deftest.ctl" ]
    set TESTARG=test2=1
[ INCLUDE "<OWROOT>/build/deftest.ctl" ]
    set TESTARG=test3=1
[ INCLUDE "<OWROOT>/build/deftest.ctl" ]
    set TESTARG=test4=1
[ INCLUDE "<OWROOT>/build/deftest.ctl" ]
    set TESTARG=test5=1
[ INCLUDE "<OWROOT>/build/deftest.ctl" ]
    set TESTARG=test6=1
[ INCLUDE "<OWROOT>/build/deftest.ctl" ]
    set TESTARG=test7=1
[ INCLUDE "<OWROOT>/build/deftest.ctl" ]
    set TESTARG=test8=1
[ INCLUDE "<OWROOT>/build/deftest.ctl" ]
    set TESTARG=test9=1
[ INCLUDE "<OWROOT>/build/deftest.ctl" ]

[ BLOCK <BLDRULE> testclean ]
#============================
    cdsay .
    wmake -h clean

[ BLOCK . . ]

cdsay .
