# Wlink Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LEXLIB>\builder\logfile.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .
    
[ BLOCK <1> build rel2 ]
    cdsay ntrtdll
    wmake /h /i
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    
[ BLOCK <1> clean ]
#==================
    cdsay ntrtdll
    wmake /h /i clean
    cd <PROJDIR>
