# clibtest Builder Control file
# =================================

set PROJDIR=<CWD>
set CLIBTEST_NAME=result

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

rm <CLIBTEST_NAME>.log
rm *.sav

[ INCLUDE _dos/builder.ctl ]
[ INCLUDE bios/builder.ctl ]
[ INCLUDE cgsupp/builder.ctl ]
[ INCLUDE direct/builder.ctl ]
[ INCLUDE environ/builder.ctl ]
[ INCLUDE file/builder.ctl ]
[ INCLUDE fmtint/builder.ctl ]
[ INCLUDE fmttest/builder.ctl ]
[ INCLUDE handleio/builder.ctl ]
[ INCLUDE heap/builder.ctl ]
[ INCLUDE heap2/builder.ctl ]
[ INCLUDE char/builder.ctl ]
[ INCLUDE intel/builder.ctl ]
[ INCLUDE intmath/builder.ctl ]
[ INCLUDE mbyte/builder.ctl ]
[ INCLUDE memory/builder.ctl ]
[ INCLUDE misc/builder.ctl ]
[ INCLUDE ntthrd/builder.ctl ]
[ INCLUDE os2thrd/builder.ctl ]
[ INCLUDE process/builder.ctl ]
[ INCLUDE safefile/builder.ctl ]
[ INCLUDE safefmt/builder.ctl ]
[ INCLUDE safeio/builder.ctl ]
[ INCLUDE safembyt/builder.ctl ]
[ INCLUDE safesrch/builder.ctl ]
[ INCLUDE safestr/builder.ctl ]
[ INCLUDE safwstr/builder.ctl ]
[ INCLUDE safwtime/builder.ctl ]
[ INCLUDE search/builder.ctl ]
[ INCLUDE startup/builder.ctl ]
[ INCLUDE streamio/builder.ctl ]
[ INCLUDE string/builder.ctl ]
[ INCLUDE time/builder.ctl ]

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
