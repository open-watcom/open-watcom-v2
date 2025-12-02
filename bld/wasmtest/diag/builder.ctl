# wasmtest Builder Control file
# =============================

set PROJNAME=wasmtest

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/master.ctl" ]

[ BLOCK <BLDRULE> test ]
#=======================
    set TESTARG=test0=1
[ INCLUDE "<OWROOT>/build/deftest.ctl" ]

[ BLOCK <BLDRULE> testclean ]
#============================
    cdsay .
    wmake -h clean

[ BLOCK . . ]

cdsay .
