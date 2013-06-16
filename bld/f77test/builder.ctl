# ftest Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=f77test

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> test ]
    wmake -h

[ BLOCK <1> testclean ]
    wmake -h clean

[ BLOCK . . ]
#============
cdsay <PROJDIR>
