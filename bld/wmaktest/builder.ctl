# wasmtest Builder Control file
# =================================

set PROJDIR=<CWD>
set WMAKETEST_NAME=result

[ INCLUDE "<OWROOT>/build/master.ctl" ]

cdsay .

rm <WMAKETEST_NAME>.log
rm *.sav

[ BLOCK <BLDRULE> test ]
#=======================
    set TESTARG=
[ INCLUDE "<OWROOT>/build/deftest.ctl" ]

[ BLOCK <BLDRULE> testclean ]
#============================
    cdsay .
    wmake -h clean

[ BLOCK <BLDRULE> cleanlog ]
#============================
    cdsay .
    echo rm -f *.log *.sav
    rm -f *.log *.sav

[ BLOCK . . ]

cdsay .
