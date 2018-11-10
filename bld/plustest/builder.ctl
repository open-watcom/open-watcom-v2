# plustest Builder Control file
# =================================

set PROJDIR=<CWD>
set PLUSTEST_NAME=result

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

rm <PLUSTEST_NAME>.log
rm *.sav

[ INCLUDE arith/builder.ctl ]
[ INCLUDE bits/builder.ctl ]
[ INCLUDE bits64/builder.ctl ]
[ INCLUDE bitset/builder.ctl ]
[ INCLUDE claslist/builder.ctl ]
[ INCLUDE contain/builder.ctl ]
[ INCLUDE cool/builder.ctl ]
[ INCLUDE datainit/builder.ctl ]
[ INCLUDE dcmat/builder.ctl ]
[ INCLUDE diagnose/builder.ctl ]
[ INCLUDE ehmix/builder.ctl ]
[ INCLUDE except/builder.ctl ]
[ INCLUDE exercise/builder.ctl ]
[ INCLUDE extref/builder.ctl ]
[ INCLUDE field/builder.ctl ]
[ INCLUDE field64/builder.ctl ]
[ INCLUDE format/builder.ctl ]
[ INCLUDE getlong/builder.ctl ]
[ INCLUDE gstring/builder.ctl ]
[ INCLUDE inline/builder.ctl ]
[ INCLUDE jam/builder.ctl ]
[ INCLUDE nih/builder.ctl ]
[ INCLUDE nullptr/builder.ctl ]
[ INCLUDE overload/builder.ctl ]
[ INCLUDE owstl/builder.ctl ]
[ INCLUDE perl/builder.ctl ]
[ INCLUDE positive/builder.ctl ]
[ INCLUDE preproc/builder.ctl ]
[ INCLUDE re2c/builder.ctl ]
[ INCLUDE re2c05/builder.ctl ]
[ INCLUDE sizelist/builder.ctl ]
[ INCLUDE splash/builder.ctl ]
[ INCLUDE stepanov/builder.ctl ]
[ INCLUDE stl.94/builder.ctl ]
[ INCLUDE stl.95/builder.ctl ]
[ INCLUDE torture/builder.ctl ]
[ INCLUDE wclass/builder.ctl ]
[ INCLUDE winnt/builder.ctl ]
[ INCLUDE yacl/builder.ctl ]

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
