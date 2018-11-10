# wasmtest Builder Control file
# =================================

set PROJDIR=<CWD>
set WASMTEST_NAME=result

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

rm <WASMTEST_NAME>.log
rm *.sav

[ INCLUDE test1/builder.ctl ]
[ INCLUDE test2/builder.ctl ]
[ INCLUDE test3/builder.ctl ]
[ INCLUDE ideal/builder.ctl ]
[ INCLUDE diag/builder.ctl ]

[ BLOCK <BLDRULE> test ]
#=======================
    cdsay .
    wmake -h

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
