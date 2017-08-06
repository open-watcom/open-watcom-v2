# ctest Builder Control file
# =================================

set PROJDIR=<CWD>
set CTEST_NAME=result

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

rm <CTEST_NAME>.log
rm *.sav

[ INCLUDE positive/builder.ctl ]
[ INCLUDE codegen/builder.ctl ]
[ INCLUDE callconv/builder.ctl ]
[ INCLUDE diagnose/builder.ctl ]
[ INCLUDE inline/builder.ctl ]


[ BLOCK <1> test ]
#=================
    cdsay .
    wmake -h

[ BLOCK <1> testclean ]
#======================
    cdsay .
    wmake -h clean

[ BLOCK <1> cleanlog ]
#=====================
    cdsay .
    echo rm -f *.log *.sav
    rm -f *.log *.sav

[ BLOCK . . ]

cdsay .
