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


[ BLOCK <BLDRULE> test ]
#=======================
    cdsay .
    wmake -h

[ BLOCK <BLDRULE> testclean ]
#============================
    cdsay .
    wmake -h clean

[ BLOCK <BLDRULE> cleanlog ]
#===========================
    cdsay .
    echo rm -f *.log *.sav
    rm -f *.log *.sav

[ BLOCK . . ]

cdsay .
