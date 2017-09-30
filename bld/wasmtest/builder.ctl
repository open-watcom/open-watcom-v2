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

[ BLOCK <1> test ]
#=================
    cdsay .
    wmake -h

[ BLOCK <1> testclean ]
#======================
    cdsay .
    wmake -h clean

[ BLOCK <1> cleanlog ]
#======================
    cdsay .
    echo rm -f *.log *.sav
    rm -f *.log *.sav

[ BLOCK . . ]

cdsay .
