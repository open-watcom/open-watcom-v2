# ftest Builder Control file
# =================================

set PROJDIR=<CWD>
set FTEST_NAME=result

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

rm <FTEST_NAME>.log
rm *.sav

[ INCLUDE nist/builder.ctl ]

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
